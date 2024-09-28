#include "pch.h"
#include <process.h>
#include "../../SigmaCore/Util/SystemInfo.h"
#include "../../SigmaCore/Net/NetUtil.h"
#include "../../EngineLib/DxCommon/dxutil.h"
#include "../../RanLogic/Msg/ServerMsg.h"
#include "../Client/NetMatchClientMan.h"
#include "./MatchServerConfigXml.h"
#include "./NetServerHelper.h"
#include "./MatchServer.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


BOOL MatchServer::ReadConfig()
{
    SAFE_DELETE(m_pConfig);
    m_pConfig = new MatchServerConfigXml;
    if (!m_pConfig->Load(std::string("MatchServer.xml")))
    {
        SAFE_DELETE(m_pConfig);
        return FALSE;
    }

    m_nMaxClient = m_pConfig->GetMaxClient();
    m_nPort = m_pConfig->GetServicePort();
    m_ServiceProvider = m_pConfig->GetServiceProvider();
    return TRUE;
}

int MatchServer::StartClientManager()
{
	SetUpAgentServerInfo();	//AgentServer ���� ����

    SAFE_DELETE(m_pClientManager);
    m_pClientManager = new NetMatchClientMan(m_nMaxClient, m_hIOServer, m_pConfig->GetReferenceCountMax(), m_AgentServerInfo);
	
    return NET_OK;
}

int MatchServer::ListenProc(void)
{
    PER_IO_OPERATION_DATA* IOData = NULL;
	SOCKET	Accept;
	DWORD	dwRecvNumBytes = 0;
	DWORD	Flags = 0;
	HANDLE	hRetCode = NULL;
	int		nClientNumber = 0;
	DWORD   dwClient = 0;
	int		nRetCode = 0;
	int		nReachedErrorCnt = 0;	

	while (m_bIsRunning)
	{
		Accept = ::WSAAccept(m_sServer, NULL, NULL, NULL, 0);

		if (Accept == INVALID_SOCKET)
		{
			nRetCode = WSAGetLastError();
			sc::writeLogError(sc::string::format("WSAAccept %1%", nRetCode));
			if (nRetCode == WSAENOTSOCK || nRetCode == WSAEINTR)
				break;
			else
				continue;
		}

		// Create per-handle data information structure to associate with the socket
		nClientNumber = m_pClientManager->GetFreeClientID(false); // Get free client slot number
		if (nClientNumber == NET_ERROR)
		{
            sc::writeLogInfo(std::string("Reached max client number"));
			::closesocket(Accept);

			continue;
		}
        
		dwClient = (DWORD) nClientNumber;
		// Set client information
		// Ŭ���̾�Ʈ�� ip, port, ���ӽð��� �����.
		if (m_pClientManager->SetAcceptedClient(dwClient, Accept) == NET_ERROR)
		{
			m_pClientManager->ReleaseClientID( dwClient );
			continue;
		}

		hRetCode = ::CreateIoCompletionPort(
            (HANDLE) Accept,
            m_hIOServer,
            (DWORD) nClientNumber,
            0);
		if (!hRetCode)
		{
			sc::writeLogError(std::string("CreateIoCompletionPort Error"));
			CloseClient(dwClient);
			continue;
		}
		
		// Start processing I/O on ther accepted socket
		// First WSARecv, TCP/IP Send 8 bytes (ignored byte)			
		dwRecvNumBytes = sizeof(NET_MSG_GENERIC);        
		IOData = m_pClientManager->GetFreeOverIO(NET_RECV_POSTED);
		if (!IOData)
		{
			CloseClient(dwClient);
			continue;
		}
		
		IOData->dwRcvBytes	 = 0;
		IOData->dwTotalBytes = dwRecvNumBytes;
		
		// Ŭ���̾�Ʈ �޽��� �ޱ� �غ�
		::WSARecv(
            Accept,
            &(IOData->DataBuf),
            1,
            &dwRecvNumBytes,
            &Flags ,
            &(IOData->Overlapped),
            NULL);
	}

	// ServerAcceptThread Running End
	sc::writeLogInfo(std::string("ListenProc end"));
	return 0;
}

int MatchServer::StartWorkThread(ServerConfigXml* pConfig)
{
    sc::EM_MS_OS_TYPE OsType;
	sc::writeLogInfo(sc::string::format("%1%", sc::getOSName(OsType)));	
    sc::writeLogInfo(sc::string::format("%1% %2% Mhz", sc::getProcessorName(), sc::getCpuSpeed()));	
    sc::writeLogInfo(sc::string::format("%1% System Processor Detected", sc::getProcessorNumber()));
	
	// Work Thread ���� ����
	// CPU ���� X 2 �� �⺻���̴�.
	m_dwCpuCount = sc::getProcessorNumber();
	m_dwWorkerThreadNumber = m_dwCpuCount * S_HEURISTIC_NUM; // Processor * 2
	
	// ���������� �ִ밪 ���� ũ�� �������Ͽ� ���� �ִ밪���� �����Ѵ�.
	if (m_dwWorkerThreadNumber > (DWORD) m_pConfig->GetMaxThread())
		m_dwWorkerThreadNumber = m_pConfig->GetMaxThread();

	// �̸� ���ǵ� �ִ밪���� ũ�� ���α׷��� ���ǵ� �ִ밪���� �����Ѵ�.
	if (m_dwWorkerThreadNumber > MAX_WORKER_THREAD)
		m_dwWorkerThreadNumber = MAX_WORKER_THREAD;

	// Creaate worker threads based on the number of processors	
	DWORD dwThreadId = 0;
	
	sc::writeLogInfo(
        sc::string::format("%1% WorkerThread(s) run", m_dwWorkerThreadNumber));
	
	for (DWORD dwCPU=0; dwCPU < m_dwWorkerThreadNumber; ++dwCPU)
    { 		
		HANDLE hThread = (HANDLE) _beginthreadex(
            NULL, 
            0, 
            CServerWorkProc,
            this, 
            0, 
            (unsigned int*) &dwThreadId);
		if (!hThread)
		{
			sc::writeLogError(
                sc::string::format(
                    "%1% Worker Thread Create Error code : %d",
                    dwCPU+1,
                    GetLastError()));
			return NET_ERROR;
        } 
		else 
		{
			sc::writeLogInfo(
                sc::string::format(
                    "#%1% Worker Thread Create OK",
                    dwCPU+1));
			// The system schedules threads on their preferred processors whenever possible.
			::SetThreadIdealProcessor(hThread, dwCPU % m_dwCpuCount);
        }		
		// store thread handle
		m_hWorkerThread[dwCPU] = hThread;
		::CloseHandle(hThread);
	}
	return NET_OK;
}

int MatchServer::WorkProc()
{
	PER_IO_OPERATION_DATA* pPerIoData = NULL;
	DWORD dwSndBytes   = 0;
	DWORD dwRcvBytes   = 0;
	DWORD dwByteTrans  = 0;
	DWORD Flags        = 0;
	DWORD dwClient     = -1;
	int	  nRetCode     = 0;		
	DWORD dwMaxClient  = m_pClientManager->GetMaxClient();
	int   nLastErr(0);
	NET_MSG_GENERIC* pNmg = NULL;

	while (m_bIsRunning)
	{
		nRetCode = GetQueuedCompletionStatus(
            m_hIOServer,
            &dwByteTrans, 
            (LPDWORD) &dwClient,
            (LPOVERLAPPED*) &pPerIoData,			
            INFINITE); 
		
		// ������ �����ϱ� ���ؼ� ����ó�� ��ƾ�� ȣ��������...
		if (dwClient == m_dwCompKey && pPerIoData == NULL && dwByteTrans == 0)
			break;
		
		// Illegal Message Skip. case 1
		if ((dwClient < 0) || (dwClient >= dwMaxClient))
		{
			m_pClientManager->ReleaseOperationData(pPerIoData);
			sc::writeLogInfo(
                sc::string::format(
                    "WorkProc case 1 %d", dwClient));
			continue;
		}
		
		if (nRetCode == 0) // ��������
		{
			// 64 �� �����̸� ������ ����
			nLastErr = GetLastError();
			CloseClient(dwClient);
            m_pClientManager->ReleaseOperationData(pPerIoData);
			continue;
		}

		if (nRetCode != 0 && !pPerIoData)
		{
			CloseClient(dwClient);
			continue;
		}
		
		if (dwByteTrans == 0) // ��������
		{
			CloseClient(dwClient);
            m_pClientManager->ReleaseOperationData(pPerIoData);
			continue;
		}

		switch (pPerIoData->OperationType)
		{
		// �б� �Ϸ� �뺸�϶�...
		case NET_RECV_POSTED :
			{
				// Insert to client buffer
				m_pClientManager->addRcvMsg(dwClient, pPerIoData->Buffer, dwByteTrans);
				
				while (true)
                {
					// Get one Message
					pNmg = (NET_MSG_GENERIC*) m_pClientManager->getRcvMsg(dwClient);
					if (NULL == pNmg)
                    {
						break;
					}
					else
                    {
						// Insert to message queue
						m_pRecvMsgManager->MsgQueueInsert(dwClient, pNmg, pNmg->Size());
					}
				}

				// OverlappedIO �޸� ��ȯ
				m_pClientManager->ReleaseOperationData(pPerIoData);
				// WSARecv ��û
				Flags = 0;
				dwRcvBytes = sizeof(NET_MSG_GENERIC);
				pPerIoData = m_pClientManager->GetFreeOverIO(NET_RECV_POSTED);
				if (!pPerIoData)
                    continue;

				SOCKET sSOCKET = m_pClientManager->GetSocket(dwClient);
				nRetCode = ::WSARecv(
								sSOCKET,
								&(pPerIoData->DataBuf), 
								1, 
								&dwRcvBytes,
								&Flags,
								&(pPerIoData->Overlapped),
								NULL);
				
				if ((nRetCode == SOCKET_ERROR) && ((nLastErr=WSAGetLastError()) != WSA_IO_PENDING))
				{						
					// ġ���� ����, �α׿� ��ϳ���. case 8
					m_pClientManager->ReleaseOperationData(pPerIoData);
					CloseClient(dwClient);
					sc::writeLogError(
                        sc::string::format(
                            "case 8 WorkProc WSARecv Client(%d) ERR(%d)", dwClient, nLastErr));
				}
			}
			break;
		// ���� �Ϸ� �뺸 �϶�...
		case NET_SEND_POSTED :
			// �� ���� ����Ʈ��				
			dwSndBytes = dwByteTrans + pPerIoData->dwSndBytes;
			// ���� �Ϸ��
			if (dwSndBytes < pPerIoData->dwTotalBytes)
			{
			    // ���� �Ϸ���� ����. �������� //���� ����Ʈ�� ����
                CloseClient(dwClient);
                sc::writeLogError(sc::string::format("WorkProc case 9 %d", dwClient));
			}
            m_pClientManager->ReleaseOperationData(pPerIoData);
			break;
		default : // case 10
			CloseClient(dwClient);
            m_pClientManager->ReleaseOperationData(pPerIoData);
			sc::writeLogError(sc::string::format("WorkProc case 10 %d", dwClient));            
			break;
		}
	}

	m_pClientManager->ReleaseOperationData(pPerIoData);

	// ServerWorkerThread Running End
	sc::writeLogInfo(std::string("WorkProc End"));	
	return 0;
}

int MatchServer::StartUpdateThread()
{
    SAFE_CLOSE_HANDLE(m_hUpdateQuitEvent);
    m_hUpdateQuitEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

    DWORD dwThreadId = 0;
    m_hUpdateThread = (HANDLE) _beginthreadex(
        NULL,
        0,
        CServerUpdateProc,
        this,
        CREATE_SUSPENDED,
        (unsigned int*) &dwThreadId );
    if (!m_hUpdateThread)
    {	
        sc::writeLogError(
            sc::string::format(
                "Server Update Thread Create Failed Error code : %d", GetLastError()));
        m_bIsRunning = false;
        SAFE_CLOSE_HANDLE(m_hUpdateQuitEvent);
        return NET_ERROR;
    } 
    else 
    {
        if (m_dwCpuCount > 2)
        {            
            // Update Thread �� ���෹���� ���� ������ ����Ų��.
            // ������ CPU ������ 2�� �̻��϶��� ������ ȿ���� �־���.
            SetThreadPriority(m_hUpdateThread, THREAD_PRIORITY_HIGHEST);
        }
        sc::writeLogInfo(std::string("Server Update Thread Create OK"));
        setUpdateFrame(100);
        m_dwUpdateOldTime = GetTickCount();
        ResumeThread(m_hUpdateThread);
        return NET_OK;
    }
}

int MatchServer::UpdateProc()
{
	TRACE("MatchServer::UpdateProc\n");

	DWORD dwUsrChkStart	= timeGetTime();    
	DWORD dwCurrentTime	= dwUsrChkStart;

	m_dwFrameTime		= dwUsrChkStart; // ���� ������ ������ �ð�����    
	m_bUpdateEnd		= false;

	float fOldAppTime	= 0;
	float fAppTime		= 0;
	float fElapsTime	= 0;
	DXUtil_Timer(TIMER_RESET);
	fOldAppTime = DXUtil_Timer(TIMER_GETAPPTIME);

	DWORD dwNewWaitTime = 0;
	DWORD dwReturn = 0;
	DWORD dwNewTime = 0;
	DWORD dwUpdateFrameTime = 0;

	while (m_bIsRunning)
	{
		dwNewTime = timeGetTime(); // ::GetTickCount();
		dwUpdateFrameTime = dwNewTime - m_dwUpdateOldTime;
		m_dwUpdateOldTime = dwNewTime;
	    
		// ����ð� üũ
		int nUpdateElspTime = (int) (m_dwUpdateWaitTime - dwUpdateFrameTime );
		if (0 >= nUpdateElspTime)
			dwNewWaitTime = 0;
		else
			dwNewWaitTime = nUpdateElspTime;

		dwReturn = ::WaitForSingleObject(m_hUpdateQuitEvent, dwNewWaitTime);
		if (WAIT_OBJECT_0 == dwReturn)
		{
			break;
		}
		else if (WAIT_TIMEOUT == dwReturn)
		{
			dwCurrentTime = ::timeGetTime();
			fAppTime   = DXUtil_Timer(TIMER_GETAPPTIME);		// ���� �ð�
			fElapsTime = DXUtil_Timer(TIMER_GETELAPSEDTIME);	// �ҿ�ð� ���

			// ���� Ŭ���̾�Ʈ ID ����.
			m_pClientManager->ResetPreSleepCID();

			RecvMsgProcess();
			HeartbeatCheck();

			m_pClientManager->SendClientFinal();
			SessionServerConnectionCheck();

			if (m_pMatchSystem)
				m_pMatchSystem->FrameMove();

			PrintDebugMsg(FPS_UPDATE_TIME);
			PrintSystemInfo();	// ���� ������ �ý��� WorkingSet size�� ����Ѵ�.
		}
	}

	// ������Ʈ ������ �����.
	m_bUpdateEnd = true;
	return 0;
}


void MatchServer::HeartbeatCheck()
{
    DWORD dwCurrentTime = ::timeGetTime();
    if ((dwCurrentTime - m_HeartbeatTimer) > HEARTBEAT_TIME )
    {       
        // Heartbeat Check
        if (m_bHeartBeat)
            HeartbeatSend();
        else
            HeartbeatRecvCheck();

        m_bHeartBeat = !m_bHeartBeat;
        m_HeartbeatTimer = dwCurrentTime;
    }
}

void MatchServer::HeartbeatSend()
{
    sc::writeInfo(std::string("HeartbeatSend"));
    NET_HEARTBEAT_SERVER_REQ msg;
    for (DWORD ClientSlot=0; ClientSlot<(DWORD) m_nMaxClient; ++ClientSlot)
    {
        // �¶��� �����ΰ��
        if (m_pClientManager->IsOnline(ClientSlot))
        {			
            m_pClientManager->ResetHeartBeat(ClientSlot);
            SendClient(ClientSlot, &msg);
        }
    }
}

void MatchServer::HeartbeatRecvCheck()
{
    for (DWORD ClientSlot=0; ClientSlot<(DWORD) m_nMaxClient; ++ClientSlot)
    {	
        if (true == m_pClientManager->IsOnline(ClientSlot) && false == m_pClientManager->CheckHeartBeat(ClientSlot))
		{
			if (m_pClientManager->IsInstanceSlot(ClientSlot))
			{		
                DWORD i=0;
				for ( ; i<MAXNUM_OF_INSTANCESERVER_TO_MATCH; i++)
				{
					if (m_InstanceServerSlots[i] == ClientSlot)
					{
						m_InstanceServerSlots[i] = NET_INVALID_SLOT;
						m_pMatchSystem->ClosedInstanceServer(i);
						break;
					}
				}

				GASSERT(i != MAXNUM_OF_INSTANCESERVER_TO_MATCH);
			}

            CloseClient(ClientSlot);
		}
    }
}

int MatchServer::SessionConnect()
{
	// �� �Ŀ� int CAgentServer::CacheConnect() �����ؼ� �����ؾ� �Ѵ�.
    /*if (m_SessionSlot != NET_INVALID_SLOT && m_pClientManager->IsOnline(m_SessionSlot))
    {
        sc::writeLogInfo(
            sc::string::format(
                "Session server connection is online %1%", m_SessionSlot));
        return NET_OK;
    }

    std::string SessionIp = m_pConfig->GetSessionServerIP();
    PER_IO_OPERATION_DATA* pIOData = m_pClientManager->GetFreeOverIO(NET_RECV_POSTED);
    int SessionPort = m_pConfig->GetSessionServerPort();
    int SessionSlotNum = m_pClientManager->GetFreeClientID(true);

    net::CONNECT_RESULT Connect = net::ConnectAndReceive(SessionIp.c_str(), SessionPort, m_hIOServer, SessionSlotNum, pIOData);
    int Result = Connect.get<0> ();
    SOCKET ConnectSocket = Connect.get<1> ();
    if (Result == NET_ERROR)
    {
        m_pClientManager->ReleaseOperationData(pIOData);
        m_pClientManager->ReleaseClientID(SessionSlotNum);
        sc::writeLogError(std::string("Session server connection failed"));
        return NET_ERROR;
    } 
        
    m_SessionSlot = SessionSlotNum;

    m_pClientManager->SetAcceptedClient(SessionSlotNum, ConnectSocket, net::SLOT_SESSION);
    m_pClientManager->SetSlotType(SessionSlotNum, net::SLOT_SESSION);    
    //m_pClientManager->SetNetMode(SessionSlotNum, NET_PACKET_HEAD);

    sc::writeLogInfo(
        sc::string::format(
            "Session connected slot %1%",
            SessionSlotNum));*/

    return NET_OK;
}

int MatchServer::SessionCloseConnect()
{
    if (!m_pClientManager)
    {
        sc::writeLogError(std::string("SessionCloseConnect() - ClientManager is NULL."));
        return NET_ERROR;
    }

    if (m_pClientManager->IsOnline(m_SessionServerSlot))
    {
        return m_pClientManager->CloseClient(m_SessionServerSlot);
    }
    else
    {
        sc::writeLogInfo(std::string("SessionServer is already offline"));
        return NET_OK;
    }
}

void MatchServer::SessionServerConnectionCheck()
{
    DWORD CurrentTime = ::timeGetTime();
    if ((CurrentTime - m_SessionCheckTimer) > SESSION_CONNECTION_CHECK_TIME)
    {
        if (m_bIsRunning && (m_SessionSlot==NET_INVALID_SLOT || !m_pClientManager->IsOnline(m_SessionSlot)))
            SessionConnect();
        m_SessionCheckTimer = CurrentTime;
    }
}

// Match Server -> Session Server 
// Send server info to session server
void MatchServer::SessionSndSvrInfo()
{
    G_SERVER_INFO ServerInfo;

    ServerInfo.nServerCurrentClient	= m_pClientManager->GetCurrentClientNumber();    
    ServerInfo.nServerMaxClient		= m_pConfig->GetServerMaxClient();
    ServerInfo.ServerType			= m_pConfig->GetServerType();
    ServerInfo.nServicePort			= m_pConfig->GetServicePort();
    ServerInfo.SetServerIp(m_szAddress);

    NET_SERVER_INFO msg(ServerInfo);
	msg.SetType(NET_MSG_SND_FULL_SVR_INFO);

	SendSession(&msg);	
}

int MatchServer::RegularScheduleProc()
{
	// ���������� ó���ؾ��� �۾��� �ִٸ� ���⼭ ó���Ѵ�

	return 0;
}