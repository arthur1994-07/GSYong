#include "pch.h"
#include "../../SigmaCore/Util/SystemInfo.h"
#include "../../Enginelib/Common/profile.h"
#include "../FieldServer/GLGaeaServer.h"
#include "../Database/ODBC/s_COdbcManager.h"
#include "../Database/ADO/ADOManager.h"
#include "../Client/NetInstanceClientMan.h"
#include "../Util/IpAddressChecker.h"
#include "./InstanceServerConfigXml.h"
#include "./InstanceServer.h"


// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


InstanceServer::InstanceServer(HWND hWnd, HWND hEditBox, HWND hEditBoxInfo, SUBPATH* pPath, EMSERVICE_PROVIDER ServiceProvider)
	: NetServer(hWnd, hEditBox, hEditBoxInfo, pPath, ServiceProvider)
	, m_dwInstanceServerNum(0)
	, m_pClientManager(NULL)	
	, m_bReservationServerStop(false)
	, m_fReservationtime(0.0f)
	, m_dwHeartBeatStart1(0)
	, m_dwHeartBeatStart2(0)
	, m_pGaeaServer(NULL)
    , m_CacheServerSlot(net::INVALID_SLOT)
    , m_SessionServerSlot(net::INVALID_SLOT)
	, m_MatchServerSlot(net::INVALID_SLOT)	
{
#ifndef _USE_TBB
	sc::writeLogError(std::string("MatchSystem requires TBB! Turn on TBB for InstanceServer!"));
	exit(0);
#endif

	m_pDbManager = NULL;	//NOT USED for Instance
	m_pAdoManager= NULL;	//NOT USED for Instance

	ZeroMemory(m_paDbMan, sizeof(m_paDbMan));
	ZeroMemory(m_paAdoMan, sizeof(m_paAdoMan));

	sc::LogMan::getInstance()->attachEditControl(hEditBox, hEditBoxInfo);
	initPacketFunc();
    m_pConfig = new InstanceServerConfigXml;
    m_pGaeaServer = new GLGaeaServer(m_ServiceProvider, pPath);
}

InstanceServer::~InstanceServer()
{
	if (m_pGaeaServer)
		m_pGaeaServer->CleanUp();	//MUST be called before the destructor of GLGaeaServer.
    SAFE_DELETE(m_pGaeaServer);

	SAFE_DELETE(m_pClientManager);
    SAFE_DELETE(m_pConfig);    
}

int InstanceServer::StartClientManager()
{
	///////////////////////////////////////////////////////////////////////////
	// 클라이언트 정보저장 클래스 생성
	SAFE_DELETE(m_pClientManager);

	//m_nMaxClient = m_nMaxClient * 2;	//mjeon.AF

	m_pClientManager = new NetInstanceClientMan(m_nMaxClient, m_hIOServer, m_pConfig->GetReferenceCountMax());
	if (!m_pClientManager)
	{
		m_bIsRunning = false;
		return NET_ERROR;
	}
	m_pClientManager->SetMatchIP(m_pConfig->GetMatchServerIP());
	return NET_OK;
}

int InstanceServer::Start()
{
    SAFE_DELETE(m_pGaeaServer);
    m_pGaeaServer = new GLGaeaServer(m_ServiceProvider, m_pPath);

	int nRetCode = 0;
	HRESULT hr;
	m_bIsRunning = true;
    m_vecCloseList.clear();

    const std::string XmlFileName("InstanceServer.xml");    
    if (!m_pConfig->Load(XmlFileName))
    {
        sc::writeLogError(
            sc::string::format(
            "%1% file load error", XmlFileName));
        m_bIsRunning = false;
        return NET_ERROR;
    }
    if (StartCfg(m_pConfig) == NET_ERROR)
        return NET_ERROR;
    
	m_pGaeaServer->SetServiceProvider(m_ServiceProvider);
	
	if (StartMsgManager(m_nMaxClient) == NET_ERROR)
        return NET_ERROR;

	if (StartIOCP() == NET_ERROR)
        return NET_ERROR;

	if (StartClientManager() == NET_ERROR)
        return NET_ERROR;	

	//if (StartDbManager() == NET_ERROR)
    //   return NET_ERROR;

	if (StartWorkThread(m_pConfig) == NET_ERROR)
        return NET_ERROR;

#ifndef _DEBUG 
	// nProtect GameGuard dll 을 초기화 한다.
#ifndef _NP30
	DWORD dwGGErrCode = ERROR_SUCCESS;
	dwGGErrCode = InitGameguardAuth ("", 1000, FALSE, 0x03);
	if (dwGGErrCode != ERROR_SUCCESS)
	{	
		m_bGameGuardInit = false;
		sc::writeLogError(std::string("nProtect GameGuard server DLL load failed"));
	}
	else
	{
		m_bGameGuardInit = true;		
		sc::writeLogInfo(std::string("nProtect GameGuard server DLL load success"));
		m_bGameGuardAuth = m_pConfig->GemeGuardAuth();
		if (m_bGameGuardAuth == true)
		{
			sc::writeLogInfo(std::string("nProtect GameGuard Auth ON"));
		}
		else
		{
			sc::writeLogInfo(std::string("nProtect GameGuard Auth OFF"));
		}
        // nProtect 서버 인증을 끈다. 말레이지아 문제.
        m_bGameGuardAuth = false;
	}
#else
	std::string strScriptPath( sc::getAppPath() );
	::SetCurrentDirectory(strScriptPath.c_str());

	UINT32 uReturn = InitCSAuth3("./GameGuard/");
	if (uReturn != ERROR_SUCCESS)
	{	
		m_bGameGuardInit = false;
		sc::writeLogError(
            sc::string::format(
                "nProtect GameGuard server DLL load failed. InitGameguardAuth %1%",
                uReturn));
	}
	else
	{
		m_bGameGuardInit = true;
		sc::writeLogInfo(
            sc::string::format(
                "nProtect GameGuard server DLL load success. InitGameguardAuth %1%",
                uReturn));

		m_bGameGuardAuth = m_pConfig->GemeGuardAuth();
		if (m_bGameGuardAuth == true)
			sc::writeLogInfo(std::string("nProtect GameGuard Auth ON"));
		else
			sc::writeLogInfo(std::string("nProtect GameGuard Auth OFF"));

        // nProtect 서버 인증을 끈다. 말레이지아 문제.
        m_bGameGuardAuth = false;
	}
#endif
#endif
	
	DWORD dwMaxClient = m_pConfig->GetMaxClient();

	bool bPKMODE = m_pConfig->IsPKServer() == true;	
	BYTE ExcessExpProcessType = m_pConfig->GetExcessExpProcessType();
	bool bPKServer = m_pConfig->IsFreePKServer();

	sc::CSystemInfo SysInfo;

	m_dwInstanceServerNum = m_pConfig->GetInstanceServerNum();


	hr = DxServerInstance::FieldCreate(
        m_pGaeaServer,
        SysInfo.GetAppPath(),
        this,
        m_pDbManager,
		m_pAdoManager,
        m_ServiceProvider,
        m_pPath,
        NULL,
        m_dwInstanceServerNum,	//InstanceServer의 Number
        dwMaxClient,
        bPKMODE,
        0,						//Channel - InstanceServer는 항상 0
        m_bPKLess,
        ExcessExpProcessType,
        m_LanguageSet,
        bPKServer,
        false,
		NULL);
	if ( FAILED(hr) )
	{
		sc::writeLogError(std::string("DxServerInstance::Create hr failed"));
		m_bIsRunning = false;
		return NET_ERROR;
	}


	if (StartUpdateThread() == NET_ERROR)
        return NET_ERROR;

	if (StartListenThread() == NET_ERROR)
        return NET_ERROR;
	
	// Connect Session server
	nRetCode = SessionConnect();
	if (nRetCode != NET_OK)
	{	
		m_bIsRunning = false;
		return NET_ERROR;
	}

	SessionSndSvrInfo();

	if (MatchConnect() == NET_ERROR)
    {
		sc::writeLogError(std::string("MatchConnect failure."));
        m_bIsRunning = false;
        return NET_ERROR;
    }
	
	//mjeon.연속적인 enum형태로 수정하였으므로 이제는 총 몇개의 메시지가 사용되는지가 의미있다.
	sc::writeLogInfo( sc::string::format("Number of used network messages: %d", NET_MSG_GCTRL_MAX) );	
	//sc::writeLogInfo( sc::string::format("NET_MSG_BASE %d", NET_MSG_BASE) );
	
	sc::writeLogInfo(std::string(">Server Start OK"));

#ifdef _USE_TBB
	sc::writeLogInfo(std::string("*** Parallelized by TBB ***"));
#else
	sc::writeLogWarn(std::string("*** Not-Parallelized ***"));
#endif

	sc::writeLogInfo(std::string("======================================================="));

	m_nStatus = S_SERVER_RUNING;

	// 알고 있는 주소에 자신의 주소, Session 서버의 주소, Match 서버의 주소를 등록한다.	
    m_pAddressChecker->SetConnectionCountAllow(m_pConfig->GetConnectionPerIpAddress());
    m_pAddressChecker->AddAllowIp(m_pConfig->GetServerIP());
    m_pAddressChecker->AddAllowIp(m_pConfig->GetSessionServerIP());
    m_pAddressChecker->AddAllowIp(m_pConfig->GetMatchServerIP());

	
	std::string strTitle;

	char szTempChar[64];
	GetWindowText(m_hWnd, szTempChar, sizeof(szTempChar));

	strTitle = std::string(szTempChar) + sc::string::format(" (%1%)", m_dwInstanceServerNum);
	
	SetWindowText(m_hWnd, strTitle.c_str());

	return NET_OK;
}

int InstanceServer::FinalStop()
{
	// Stop All Thread and exit
	DWORD dwExitCode = 0;

    lockOn();
	m_bIsRunning = false;
	lockOff();

	CloseAllClient();

	sc::writeLogInfo(std::string("== Please wait until server final stop"));
	
	if (m_bUseEventThread)	
	{
		::SetEvent( m_hUpdateQuitEvent );

		::WaitForSingleObject(m_hUpdateThread, INFINITE);
		SAFE_CLOSE_HANDLE(m_hUpdateQuitEvent);
	}
    else
    {
		while (m_hUpdateThread)
		{
			Sleep(0);
		}
	}

	sc::writeLogInfo(std::string("== Update Thread stopped."));

	StopListenThread();	
	StopWorkThread();
	StopIOCP();	

	// 클라이언트 저장 클래스 종료
	SAFE_DELETE(m_pClientManager);
	sc::writeLogInfo(std::string("== Client Manager closed"));
	// Message Queue 종료
	SAFE_DELETE(m_pRecvMsgManager);
	sc::writeLogInfo(std::string("== Message Manager closed"));
	
	// DB 연결종료
	SAFE_DELETE(m_pDbManager);

	sc::writeLogInfo(std::string("== Database Manager closed"));	

#ifndef _DEBUG
	// nProtect GameGuard 종료
	if (m_bGameGuardInit == true)
	{
#ifndef _NP30
		CleanupGameguardAuth();
#else
		CloseCSAuth3();
#endif
	}
#endif

	// Put message to console
	sc::writeLogInfo(std::string("Server Stop OK"));
	sc::writeLogInfo(std::string("======================================================="));

	m_nStatus = S_SERVER_STOP;

	return NET_OK;
}

int InstanceServer::Stop()
{
	// 서버의 정지를 예약한다.
	m_bReservationServerStop = true;
	sc::writeLogInfo(std::string("== Please wait until server stop"));
	GLMSG::SNET_REQ_SERVERSTOP InnerMsg;
	InsertMsg (DBACTION_CLIENT_ID, (char*) &InnerMsg);

	return NET_OK;
}

void InstanceServer::ForceStop( DWORD Reason )
{
	sc::writeLogInfo( std::string( "== Please wait until server stop(Force)" ) );
}

void InstanceServer::CloseAllClient()
{	
	if (m_pClientManager)
	{
		for (DWORD i=0; i < (DWORD) m_pClientManager->GetMaxClient(); ++i)
		{
			if (m_pClientManager->IsOnline(i))
				CloseClient(i);
		}		
	}
}

void InstanceServer::ResetGaeaID(DWORD dwClient)
{
	m_pClientManager->ResetGaeaID(dwClient);
}

// 사용자를 로그아웃 시킨다.
void InstanceServer::CloseClient(DWORD dwClient, net::CLOSE_TYPE emCloseType, int nLastError)
{	
	//lockOn();
	lockOn();
	{
		if (m_pClientManager->IsOnline(dwClient) == false)
		{
			lockOff();
			
			return;
		}

		int   nUserNum = 0;	
		DWORD dwGaeaID = 0;

		if (dwClient == m_SessionServerSlot)
		{			
			sc::writeLogInfo(sc::string::format("Session server connection closed. slot %1%", dwClient));
            m_pClientManager->CloseClient(m_SessionServerSlot);            
            if (m_bIsRunning)
			{	
				sc::writeLogInfo(
					sc::string::format("Session server connection closed. will try to reconnect. slot %1%", m_SessionServerSlot));
                m_SessionServerSlot = net::INVALID_SLOT;
                SessionConnectSndSvrInfo();
				lockOff();
				return;
			}
		}
		else if (dwClient == m_MatchServerSlot)
		{			
			sc::writeLogInfo(sc::string::format("Match server connection closed. slot %1%", dwClient));
            m_pClientManager->CloseClient(m_MatchServerSlot);            
            if (m_bIsRunning)
			{	
				sc::writeLogInfo(
					sc::string::format("Match server connection closed. will try to reconnect. slot %1%", m_MatchServerSlot));
                m_MatchServerSlot = net::INVALID_SLOT;				
                SessionConnectSndSvrInfo();
				lockOff();
				return;
			}
		}
		else // 일반 사용자 종료
		{
            // 등록된 IP 리스트를 제거한다.
            // SERVER_UTIL::CAddressChecker::GetInstance()->Delete(m_pClientManager->GetClientIP(dwClient));
			std::string ClientIp = m_pClientManager->GetClientIP(dwClient);
            m_pAddressChecker->DelConnection(ClientIp);

			if (!m_pClientManager->IsClientSlot(dwClient))
			{
				//GASSERT(0&&"WrongClientSlot_3");
			}
			else
			{
				nUserNum = m_pClientManager->UserDbNum(dwClient);

				if (nUserNum > 0)
				{
					dwGaeaID = m_pClientManager->GetGaeaID(dwClient);
					
					if (dwGaeaID != GAEAID_NULL)
					{						
						sc::writeLogInfo(
                            sc::string::format(
                                "Slot %1% GaeaID %2% Drop & Save Character", dwClient, dwGaeaID));

						// 가이아서버 캐릭터 드랍 예약
                        m_pGaeaServer->ReserveDropOutPC(dwGaeaID, db::DbActionNull::Null);
					}
				}
				m_pClientManager->CloseClient(dwClient);
			}

			// 현재 서버상태 전송
			SessionSndSvrCurState();
		}
	}

	lockOff();
}

/*
void InstanceServer::Send(DWORD dwClient, PER_IO_OPERATION_DATA* PerIoData, DWORD dwSize)
{	
	sc::writeLogError(sc::string::format("Illegal Function call Send %1%", dwClient));
}
*/

void InstanceServer::SendAllClient(EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc)
{
    NET_MSG_PACK MsgPack;
    if (MsgPack.SetData(MsgType, PackBuffer, UseCrc))
        SendAllClient(&MsgPack);
}

void InstanceServer::SendAllClient(NET_MSG_GENERIC* pBuffer)
{
    if (pBuffer)
    {
        DWORD MaxClient = static_cast<DWORD> (m_nMaxClient);
        sc::writeLogInfo(std::string("Sending message to all users"));
        for (DWORD i=1; i<MaxClient; ++i)
        {
            // 게임중이면 메시지를 보낸다
            if (m_pClientManager->IsGaming(i))
                SendClient(i, pBuffer);
        }
    }
}

int InstanceServer::SendClient(EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, DWORD dwChaNum, bool UseCrc)
{
    NET_MSG_PACK MsgPack;
    if (MsgPack.SetData(MsgType, PackBuffer, UseCrc))
        return SendClient(&MsgPack, dwChaNum);
    else
        return NET_ERROR;
}

//mjeon.AF
int InstanceServer::SendClient(NET_MSG_GENERIC* pBuffer, DWORD dwChaNum)
{
	DWORD dwClient = m_pClientManager->GetClientID(dwChaNum);

	if (dwClient == CLIENTID_NULL)
	{		
		return NET_ERROR;
	}

	return SendClient(dwClient, pBuffer);
}

int InstanceServer::SendClient(DWORD dwClient, EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc)
{
    NET_MSG_PACK MsgPack;
    if (MsgPack.SetData(MsgType, PackBuffer, UseCrc))
        return SendClient(dwClient, &MsgPack);
    else
        return NET_ERROR;
}

int InstanceServer::SendClient(DWORD dwClient, NET_MSG_GENERIC* pBuffer)
{
    if (!pBuffer || !m_pClientManager->IsOnline(dwClient)) 
	{		
		return NET_ERROR;
	}
	else
	{
		PROFILE_MSG_COUNT_SEND( pBuffer );
		return m_pClientManager->SendClient(dwClient, pBuffer);
	}
}

int InstanceServer::SendMatch(EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc)
{
    NET_MSG_PACK MsgPack;
    if (MsgPack.SetData(MsgType, PackBuffer, UseCrc))
        return SendMatch(&MsgPack);
    else
        return NET_ERROR;
}

int InstanceServer::SendMatch(NET_MSG_GENERIC* pBuffer)
{
	GASSERT(!m_pClientManager->IsClientSlot(m_MatchServerSlot)&&"m_MatchServerSlot is client slot!!");

	if (!pBuffer)
	{
		return NET_ERROR;
	}
	else
	{
		PROFILE_MSG_COUNT_SEND( pBuffer );
		return m_pClientManager->SendMatch(m_MatchServerSlot, pBuffer);	// m_dwMatchSlot을 매개변수로 넘길 필요가 없는데....??
	}

	return 0;
}

int InstanceServer::SendAgent(EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc)
{
    NET_MSG_PACK MsgPack;
    if (MsgPack.SetData(MsgType, PackBuffer, UseCrc))
        return SendAgent(&MsgPack);
    else
        return NET_ERROR;
}

int InstanceServer::SendAgent(NET_MSG_GENERIC* pBuffer)
{
	//InstanceServer는 Agent와 연동해서 동작해서는 안된다. Instance 자체적으로 모든걸 처리할 수 있도록 하라

	GASSERT(0&&"InstanceServer::SendAgent()");

	return 0;
}

int InstanceServer::SendAgent ( DWORD dwClient, NET_MSG_GENERIC* pBuffer )
{
	return NET_ERROR;
}

int InstanceServer::SendAgent ( DWORD dwClient, EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc )
{
	return NET_ERROR;
}

int InstanceServer::SendAgent_ServerGroup ( DWORD dwServerGroup, NET_MSG_GENERIC* pBuffer )
{
	return NET_ERROR;
}

int InstanceServer::SendAgent_ServerGroup ( DWORD dwServerGroup, EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc )
{
	return NET_ERROR;
}


int InstanceServer::CheckTracingUser(const char *pUserAccount, const int nUserNum)
{
    for (size_t i = 0; i < m_vecTracingData.size(); ++i)
	{
		const STracingData& data = m_vecTracingData[i];
		if (_strcmpi(data.m_szAccount, pUserAccount) == 0 || data.dwUserNum  == nUserNum)
		{
			return static_cast<int> (i);
		}
	}
	return -1;
}

void InstanceServer::RecvMsgProcess()
{
	// Flip Messge Queue    
	m_pRecvMsgManager->MsgQueueFlip();
	
    // Get Messages and process message, 메시지 처리
    MSG_LIST* pMsg = m_pRecvMsgManager->GetMsg( NULL );
	while (pMsg)
	{		
		MsgProcess(pMsg);
		pMsg = m_pRecvMsgManager->GetMsg( pMsg ); // Get next message
	}
}

void InstanceServer::FieldSrvHeartBeatCheck( DWORD dwUdpateTime )
{
    DWORD dwCurrentTime = timeGetTime();

    //mjeon	
    //Only heartbeat check for session server is done.
    //Currently, there is no hearbeat check code for clients in this function.	
    if ((dwCurrentTime-m_dwHeartBeatTime) >= dwUdpateTime)
    {	
        // Client check
        // 잘못된 접속 종료, 접속만 하고 더이상 메시지를 보내지 않아서 GAEAID_NULL 인 상태
        for (DWORD dwCheck=1; dwCheck<(DWORD) m_nMaxClient; ++dwCheck)
        {
            if (m_pClientManager->IsClientSlot(dwCheck) && 
                m_pClientManager->GetGaeaID(dwCheck) == GAEAID_NULL)
            {
                sc::writeLogInfo(
                    sc::string::format(
                        "Slot(%1%) is closed. It's GaeaID is NULL",
                        dwCheck));
                CloseClient(dwCheck);
            }
        }

        // Session server check
        if ((dwCurrentTime - m_dwHeartBeatStart1) > SESSION_CHECK_TIME)
        {
            if (m_SessionServerSlot != net::INVALID_SLOT && !m_pClientManager->IsOnline(m_SessionServerSlot))
            {
                m_SessionServerSlot = net::INVALID_SLOT;
                SessionConnectSndSvrInfo();
            }
            m_dwHeartBeatStart1 = timeGetTime();
            m_pAddressChecker->ClearIpData();
        }
        
        m_dwHeartBeatTime = dwCurrentTime;
    }
}

void InstanceServer::PrintDebugMsg( DWORD dwUdpateTime )
{
	std::string &strLog = std::string("");

	// 프레임 측정
	DWORD dwCurrentTime = timeGetTime();
	if ( (dwCurrentTime-m_dwFrameTime) >= dwUdpateTime )
	{
		// 프로파일링 
		DWORD dwProfileStart = timeGetTime();
		PROFILE_DUMPOUTPUT_FILE();
		DWORD dwProfileEnd = timeGetTime() - dwProfileStart;

		// 처리시간이 500msec 이상이라면 기록한다.
		if (dwProfileEnd > 500)
		{	
			sc::writeLogInfo( sc::string::format("PROFILE_DUMPOUTPUT_FILE %d msec", dwProfileEnd) );
		}
		
		// 업데이트 정보 출력
		float fFrame = ( (float) ( (m_dwFrameCount * dwUdpateTime) / (float) (timeGetTime() - m_dwFrameTime) ) );
		m_dwFrameCount = 0;
		
		strLog = sc::string::format(
            "Update %1% FPS Avg %2% msec Character %3%",
            (int) (fFrame/(dwUdpateTime/1000)), 
            (int) (dwUdpateTime/fFrame),
            (int) m_pGaeaServer->GetNumPC());
		
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
			//m_pClientManager->GetUseSendMemoryCount(),
            //m_pClientManager->GetUseRecvMemoryCount()
			);

		sc::writeInfo(strLog);
        //sc::writeLogInfo(strLog);

		m_pClientManager->resetPacketCount();

		m_dwFrameTime = dwCurrentTime;
	} 
	else
	{
		DWORD dwProfileDumpStart = timeGetTime();
		g_profile_1.DumpOutputToNon();
		DWORD dwProfileDumpEnd = timeGetTime() - dwProfileDumpStart;
		if (dwProfileDumpEnd > 500)
			sc::writeLogInfo( sc::string::format("INFO:PROFILE::DumpOutputToNon() %d msec", dwProfileDumpEnd) );
	}
}

void InstanceServer::EndUpdateThread()
{
	TRACE( "[UpdateProc End]\n" );
	sc::writeLogInfo( std::string("= [UpdateProc End] - Begin") );
	{
		//	Note : 모든 활동 PC,NPC를 종료시킨다.
		m_pGaeaServer->ClearDropObj();
		m_pGaeaServer->ClearReservedDropOutPC();

		//	Note : 클럽배틀 정보를 저장한다.
		m_pGaeaServer->SaveClubBattle();
		
		// 가이아 서버 중단뒤 메모리 해제.
		DxServerInstance::FieldCleanUp(m_pGaeaServer);
	}
	sc::writeLogInfo(std::string("= [UpdateProc End] - End"));
	TRACE("[Gaea Server CleanUp]\n");

	//	업데이트 스래드 사라짐.
	CloseHandle( m_hUpdateThread );
	m_hUpdateThread = NULL;
	m_bUpdateEnd	= true;
}

void InstanceServer::ReserveCloseClient(DWORD dwClient)
{
    DWORD MaxClient = static_cast<DWORD> (m_nMaxClient);
    if (dwClient != 0 && dwClient<MaxClient && m_pClientManager->IsClientSlot(dwClient))
    {
        std::vector<DWORD>::iterator pos = std::find(m_vecCloseList.begin(), m_vecCloseList.end(), dwClient);
        if (pos == m_vecCloseList.end()) 
            m_vecCloseList.push_back(dwClient);
    }
}

void InstanceServer::CloseClientFinal()
{
    std::vector<DWORD>::iterator pos = m_vecCloseList.begin();
    std::vector<DWORD>::iterator pos_end = m_vecCloseList.end();    
    for ( ; pos != pos_end; ++pos)
    {
        CloseClient(*pos);
    }
    m_vecCloseList.clear();
}

bool InstanceServer::IsReserveCloseClient( DWORD dwClient )
{
    std::vector<DWORD>::iterator pos = std::find( m_vecCloseList.begin(), m_vecCloseList.end(), dwClient );
        
    if ( pos == m_vecCloseList.end() ) 
        return false;
    else 
        return true;
}

void InstanceServer::ClearBlockIp()
{
    if (m_pAddressChecker)
        m_pAddressChecker->ClearIpData();
}

std::string InstanceServer::GetClientIP(DWORD dwClient) const
{ 
    return m_pClientManager->GetClientIP(dwClient);
}

net::EMSERVERTYPE InstanceServer::GetServerType()
{
	return m_pConfig->GetServerType();
}

std::string InstanceServer::GetIP()
{
	return std::string(m_pConfig->GetServerIP());
}

int	InstanceServer::GetPort()
{
	return m_pConfig->GetServicePort();
}

int InstanceServer::SendField(DWORD dwClient, EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc)
{
    GASSERT(0&&"InstanceServer::SendField()");
    return 0;
}

int InstanceServer::SendField(DWORD dwClient, NET_MSG_GENERIC* pBuffer)
{ 
    GASSERT(0&&"InstanceServer::SendField()");
    return 0;
}

int InstanceServer::SendField(DWORD dwChannel, DWORD dwFieldSvrNum, EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc)
{
    GASSERT(0&&"InstanceServer::SendField()"); 
    return 0;
}

int InstanceServer::SendField(DWORD dwChannel, DWORD dwFieldSvrNum, NET_MSG_GENERIC* pBuffer)
{ 
    GASSERT(0&&"InstanceServer::SendField()"); 
    return 0;
}

int InstanceServer::SendFieldSvr(int nSvrNum, EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, int nServerChannel, bool UseCrc)
{
    GASSERT(0&&"InstanceServer::SendFieldSvr"); 
    return 0;
}

int InstanceServer::SendFieldSvr(int nSvrNum, NET_MSG_GENERIC* pBuffer, int nServerChannel)
{ 
    GASSERT(0&&"InstanceServer::SendField()"); 
    return 0;
}	

void InstanceServer::SendAllField(EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, int nServerChannel, bool UseCrc)
{
    assert(0&&"");
}

void InstanceServer::SendAllField(NET_MSG_GENERIC* pBuffer, int nServerChannel)
{ 
    assert(0&&"");
}

void InstanceServer::SendChannel(EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, int nServerChannel, bool UseCrc)
{
    assert(0&&"");
}

void InstanceServer::SendChannel(NET_MSG_GENERIC* pBuffer, int nServerChannel)
{ 
    assert(0&&"");
}

int InstanceServer::SendAllChannel(EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc)
{
    assert(0&&"");
	return 0;
}

int InstanceServer::SendAllChannel(NET_MSG_GENERIC* pBuffer)
{
    assert(0&&"");
	return 0;
}

void InstanceServer::SendCache(NET_MSG_GENERIC* pBuffer)
{ 
    GASSERT(0&&"InstanceServer::SendCache()");
}

void InstanceServer::SendCache(EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc)
{
    GASSERT(0&&"InstanceServer::SendCache()");
}