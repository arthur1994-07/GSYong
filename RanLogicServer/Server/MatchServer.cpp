#include "pch.h"
#include "../../SigmaCore/Util/SystemInfo.h"
#include "../../RanLogic/GLogicDataMan.h"
#include "../FieldServer/GLGaeaServer.h"
#include "../Database/ADO/ADOManager.h"
#include "../Client/NetMatchClientMan.h"
#include "./MatchServerConfigXml.h"
#include "./NetServerHelper.h"
#include "./MatchServer.h"


// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


MatchServer::MatchServer(HWND hWnd, HWND hEditBox, HWND hEditBoxInfo, SUBPATH* pPath, EMSERVICE_PROVIDER ServiceProvider)
	:NetServer(hWnd, hEditBox, hEditBoxInfo, pPath, ServiceProvider)
	,m_pClientManager(NULL)
	,m_pMatchSystem(NULL)
	,m_pConfig(NULL)
{
	sc::LogMan::getInstance()->attachEditControl(hEditBox, hEditBoxInfo);
    initPacketFunc();
    m_HeartbeatTimer = ::timeGetTime();
    m_SessionCheckTimer = ::timeGetTime();

	for (int i=0; i<MAXNUM_OF_INSTANCESERVER_TO_MATCH; i++)
	{
		m_InstanceServerSlots[i] = NET_INVALID_SLOT;
	}
    
	m_pConfig = new MatchServerConfigXml;
}

MatchServer::~MatchServer()
{
	if (IsRunning())
        Stop();

	SAFE_DELETE(m_pMatchSystem);
	SAFE_DELETE(m_pClientManager);
	SAFE_DELETE(m_pConfig);
	GLogicData::GetInstance().ClearData();
}

int MatchServer::Start()
{
	SAFE_DELETE(m_pMatchSystem);
    m_pMatchSystem = new MatchSystem(this);

    m_bIsRunning = true;

    if (!ReadConfig())
    {
        m_bIsRunning = false;
        return NET_ERROR;
    }

	sc::writeLogInfo(std::string(">Start Configuring the Server..."));
    
    if (StartMsgManager(m_nMaxClient) == NET_ERROR)
    {
        m_bIsRunning = false;
        return NET_ERROR;
    }

    if (StartIOCP() == NET_ERROR)
    {
        m_bIsRunning = false;
        return NET_ERROR;
    }

    if (StartClientManager() == NET_ERROR)
    {
        m_bIsRunning = false;
        return NET_ERROR;
    }

    if (StartWorkThread(m_pConfig) == NET_ERROR)
    {
        m_bIsRunning = false;
        return NET_ERROR;
    }

    if (StartUpdateThread() == NET_ERROR)
    {
        m_bIsRunning = false;
        return NET_ERROR;
    }

    if (StartListenThread() == NET_ERROR)
    {
        m_bIsRunning = false;
        return NET_ERROR;	
    }

	//DWORD dwMaxClient = m_pConfig->GetMaxClient();

    SessionConnect();

    // Logic Load -------------------------------------------------------------
    std::string AppPath(sc::getAppPath());
    HRESULT hr = DxServerInstance::CreateBefore(
        AppPath.c_str(),
        m_pPath,
        m_pConfig->GetLangSet(),
        m_ServiceProvider,
        m_pConfig->IsPKServer(),
        m_pConfig->IsPKLessServer(),
        false);
    if (FAILED(hr))
    {
        sc::writeLogFatal(std::string("DxServerInstance::CreateBefore"));
        Stop();
        return NET_ERROR;
    }

	m_pMatchSystem->Initialize();

    DxServerInstance::CreateAfter();

	SessionSndSvrInfo();
    // ------------------------------------------------------------------------

    m_HeartbeatTimer = ::timeGetTime();
    m_SessionCheckTimer = ::timeGetTime();
    m_bHeartBeat = true;
    
    sc::writeLogInfo(std::string(">Server Start OK"));
	sc::writeLogInfo(std::string("======================================================="));

    return NET_OK;
}

int	MatchServer::Stop()
{
	// Stop All Thread and exit
	DWORD dwExitCode = 0;

	sc::writeLogInfo(std::string("Please wait until server stops"));
	
	m_bIsRunning = false;
	
	::SetEvent(m_hUpdateQuitEvent);
	::WaitForSingleObject(m_hUpdateThread, INFINITE);
	SAFE_CLOSE_HANDLE(m_hUpdateQuitEvent);	

	sc::writeLogInfo(std::string("== Update Thread ends"));

	StopListenThread();
	StopWorkThread();
	StopIOCP();
	
	SessionCloseConnect();
	
	SAFE_DELETE(m_pClientManager);
	sc::writeLogInfo(std::string("== Client Manager closed"));
	
	SAFE_DELETE(m_pRecvMsgManager);
	sc::writeLogInfo(std::string("== Message Manager closed."));

	SAFE_DELETE(m_pDbManager);
	sc::writeLogInfo(std::string("== Database Manager closed OK"));

	SAFE_DELETE(m_pConfig);
	sc::writeLogInfo(std::string("== Config Manager closed OK"));

    // Logic data clear
    GLogicData::GetInstance().ClearData();

	sc::writeLogInfo(std::string("Server Stop OK"));
	sc::writeLogInfo(std::string("======================================================="));
	
	m_nStatus = S_SERVER_STOP;

	return NET_OK;
}

void MatchServer::ForceStop( DWORD Reason )
{
	sc::writeLogInfo( std::string( "== Please wait until server stop(Force)" ) );
}

void MatchServer::SetUpAgentServerInfo()
{
	for (int i=0; i<MAXNUM_OF_AGENTSERVER_TO_MATCH; ++i)
    {
        A_SERVER_INFO* pAgentInfo = m_pConfig->GetAgentServerInfo(i);
        if (pAgentInfo)
            m_AgentServerInfo[i] = *pAgentInfo;
    }
}

void MatchServer::CloseClient(DWORD dwClient)
{
    if (dwClient == NET_INVALID_SLOT)
        return;

    if (dwClient == m_SessionSlot && m_bIsRunning)
    {
        // 서버가 가동중인데 연결이 끊어졌다.
        // SessionServerConnectionCheck 함수에서 재연결을 시도할 것이다.
        sc::writeLogError(
            std::string("Session connection was closed. will try to reconnect"));
        m_SessionSlot = NET_INVALID_SLOT;
    }

	for (DWORD i=0; i<MAXNUM_OF_INSTANCESERVER_TO_MATCH; i++)
	{
		if (m_InstanceServerSlots[i] == dwClient)
		{
			m_InstanceServerSlots[i] = NET_INVALID_SLOT;

			sc::writeLogError(sc::string::format("InstanceServer(%1%) closed the connection.", i));
			m_pMatchSystem->ClosedInstanceServer(i);
			break;
		}
	}

    m_pClientManager->CloseClient(dwClient);
}


//
//NET_MSG_CHARACTER에 정의된 수신할 캐릭터로 해당 메시지를 발송 (Key = SvrGroupNum + ChaNum)
//
void MatchServer::SendClient(NET_MSG_CHARACTER *nmc, DWORD dwSvrGroupNum)
{
	GASSERT(nmc != NULL);
	GASSERT(nmc->dwChaNum != INVALID_CHANUM);

	SendAgent(dwSvrGroupNum, (NET_MSG_GENERIC *)nmc);
}

void MatchServer::SendClient(NET_MSG_UNIQUEKEY *nmu)
{
	GASSERT(nmu != NULL);
	GASSERT(nmu->dwSvrGroupNum <= MAX_SERVER_GROUP);
	GASSERT(nmu->dwChaNum != INVALID_CHANUM);

	SendAgent(nmu->dwSvrGroupNum, (NET_MSG_GENERIC *)nmu);
}

//
//MatchServer에 연결된 Server들을 포함한 모든 Client에 대한 일반적인 SendClient 함수
//
int MatchServer::SendClient(DWORD dwClient, NET_MSG_GENERIC* pMsg)
{
    if (m_pClientManager->IsOnline(dwClient))
        return m_pClientManager->SendClient(dwClient, pMsg);
    else
        return NET_ERROR;
}

/*
void MatchServer::SendAllClient(NET_MSG_GENERIC* pMsg)
{
    for (int i=0; i<m_nMaxClient; ++i)
        SendClient(i, pMsg);
}
*/

void MatchServer::SendSession(NET_MSG_GENERIC* nmg)
{
	if (m_SessionSlot == NET_INVALID_SLOT)
	{
		sc::writeLogError(std::string("MatchServer::SendSession() - SessionSlot is invalid."));
		return;
	}

	SendClient(m_SessionSlot, nmg);
}

void MatchServer::SendAgent(DWORD dwSvrGroupNum, NET_MSG_GENERIC* nmg)
{
	GASSERT(m_AgentServerInfo[dwSvrGroupNum].IsValid());

	if (!m_AgentServerInfo[dwSvrGroupNum].IsValid())
	{
		sc::writeLogError(sc::string::format("MatchServer::SendAgent() - SvrGroupNum(%1%) is invalid.", dwSvrGroupNum));
		return;
	}

	//SendClient(m_AgentServerInfo[dwSvrGroupNum].NetSlotNum, nmg);

	DWORD dwClient = m_AgentServerInfo[dwSvrGroupNum].NetSlotNum;

	if (m_pClientManager->IsOnline(dwClient))
	{
        m_pClientManager->SendAgent(dwClient, nmg);
	}
    else
	{
		sc::writeLogError(sc::string::format("MatchServer::SendAgent() - AgentServer(%1%) is not online on the slot(%2%).", dwSvrGroupNum, dwClient));
        return;
	}
}

BOOL MatchServer::SendInstance(DWORD dwInstanceSvrNum, NET_MSG_GENERIC *nmg)
{
	GASSERT(dwInstanceSvrNum < MAXNUM_OF_INSTANCESERVER_TO_MATCH);

	DWORD dwClient = m_InstanceServerSlots[dwInstanceSvrNum];
	
	//GASSERT(dwClient != NET_INVALID_SLOT);
	if (dwClient == NET_INVALID_SLOT)
		return FALSE;

	if (m_pClientManager->IsOnline(dwClient))
	{
        return (m_pClientManager->SendInstance(dwClient, nmg) != NET_ERROR);
	}
    else
	{
		sc::writeLogError(sc::string::format("MatchServer::SendInstance() - InstanceServer(%1%) is not online on the slot(%2%).", dwInstanceSvrNum, dwClient));
        return FALSE;
	}
}

void MatchServer::PrintDebugMsg( DWORD dwUdpateTime )
{
	std::string &strLog = std::string("");

	// 프레임 측정
	DWORD dwCurrentTime = timeGetTime();
	if ( (dwCurrentTime-m_dwFrameTime) >= dwUdpateTime )
	{
		// 업데이트 정보 출력
		float fFrame = ( (float) ( (m_dwFrameCount * dwUdpateTime) / (float) (timeGetTime() - m_dwFrameTime) ) );
		m_dwFrameCount = 0;
		
		strLog = sc::string::format(
            "Update %1% FPS Avg %2% msec Character",// %3%",
            (int) (fFrame/(dwUdpateTime/1000)), 
            (int) (dwUdpateTime/fFrame));
            //,(int) m_pGaeaServer->GetNumPC());
		
		sc::writeInfo(strLog);

		// 패킷 처리량 디스플레이		
		strLog = sc::string::format("In Packet Count %d/sec Traffic %d(bytes)/sec",
			(int) (m_pClientManager->getInPacketCount() / (dwUdpateTime/1000)),
			(int) (m_pClientManager->getInPacketSize() / (dwUdpateTime/1000)) );

		sc::writeInfo(strLog);

		strLog = sc::string::format("Out Packet Count %d/sec Traffic %d(bytes)/sec",
			(int) (m_pClientManager->getOutPacketCount() / (dwUdpateTime/1000)),
			(int) (m_pClientManager->getOutPacketSize() / (dwUdpateTime/1000)) );

		sc::writeInfo(strLog);        
		
		strLog = sc::string::format("Compress Count %d/sec Traffic %d(bytes)/sec",
			(int) (m_pClientManager->getOutCompressCount() / (dwUdpateTime/1000)),
			(int) (m_pClientManager->getOutCompressSize() / (dwUdpateTime/1000)) );

		sc::writeInfo(strLog);

		strLog = sc::string::format(
			"Msg Queue Size %1%", // Send %2% Recv %3%",
			m_pRecvMsgManager->GetCount()
			);

		sc::writeInfo(strLog);

		m_pClientManager->resetPacketCount();

		m_dwFrameTime = dwCurrentTime;
	}
}

PER_IO_OPERATION_DATA* MatchServer::GetFreeOverIO(EMMSG_IO_TYPE nType)
{
    return m_pClientManager->GetFreeOverIO(nType);
}

void MatchServer::ReleaseOperationData(PER_IO_OPERATION_DATA* pData)
{
    m_pClientManager->ReleaseOperationData(pData);
}
