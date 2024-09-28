#include "../pch.h"
#include <process.h>
#include "../../SigmaCore/Util/SystemInfo.h"
#include "../../SigmaCore/Net/NetUtil.h"
#include "../../SigmaCore/Math/SeqUniqueGuid.h"
#include "../../EngineLib/DxCommon/dxutil.h"
#include "../../RanLogic/Msg/ServerMsg.h"
#include "../Database/ADO/AdoManager.h"
#include "../PrivateMarket/GLPrivateMarketSearchMan.h"

#include "../PostRenew/PostRenewCache.h"

#include "./CacheServerConfigXml.h"
#include "./NetCacheClientMan.h"
#include "./NetServerHelper.h"
#include "./CacheServer.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

//! 설정파일을 읽는다.
bool CacheServer::ReadConfig()
{
    SAFE_DELETE(m_pConfig);
    m_pConfig = new CacheServerConfigXml;
    if (!m_pConfig->Load(std::string("CacheServer.xml")))
    {
        SAFE_DELETE(m_pConfig);
        return false;
    }

	m_nServerGroup	   = m_pConfig->GetServerGroup();		// 서버 그룹

    m_nMaxClient = m_pConfig->GetMaxClient();
    m_nPort = m_pConfig->GetServicePort();
    m_ServiceProvider = m_pConfig->GetServiceProvider();
    const char* szIpAddress = m_pConfig->GetServerIP();
    if (szIpAddress)
        StringCchCopy(m_szAddress, MAX_IP_LENGTH+1, szIpAddress);
    return true;
}

int CacheServer::StartClientManager()
{
    SAFE_DELETE(m_pClientManager);
    m_pClientManager = new NetCacheClientMan(m_nMaxClient, m_hIOServer, m_pConfig->GetReferenceCountMax());
    return NET_OK;
}

//! Database Manager
bool CacheServer::StartDbManager()
{
    SAFE_DELETE(m_pDbManager);
    m_pDbManager = new db::AdoManager(this, db::CACHE_SERVER, m_ServiceProvider);
    m_pDbManager->SetConnectionString(
        db::LOG_DB,
        m_pConfig->LogDbSource(),
        m_pConfig->LogDbDatabaseName(),
        m_pConfig->LogDbUser(),
		m_pConfig->LogDbPassword(),
		m_pConfig->GetAdoMaxPoolSize(),
		m_pConfig->GetDatabaseTcpPacketSize());

    m_pDbManager->SetConnectionString(
        db::GAME_DB,
        m_pConfig->GameDbSource(),
        m_pConfig->GameDbDatabaseName(),
        m_pConfig->GameDbUser(),
		m_pConfig->GameDbPassword(),
		m_pConfig->GetAdoMaxPoolSize(),
		m_pConfig->GetDatabaseTcpPacketSize());

    m_pDbManager->SetConnectionString(
        db::SHOP_DB,
        m_pConfig->ShopDbSource(),
        m_pConfig->ShopDbDatabaseName(),
        m_pConfig->ShopDbUser(),
		m_pConfig->ShopDbPassword(),
		m_pConfig->GetAdoMaxPoolSize(),
		m_pConfig->GetDatabaseTcpPacketSize());

	// 란모바일  DB
	m_pDbManager->SetConnectionString(
		db::RANMOBILE_DB,
		m_pConfig->RanMobileDbSource(),
		m_pConfig->RanMobileDbDatabaseName(),
		m_pConfig->RanMobileDbUser(),
		m_pConfig->RanMobileDbPassword(),
		m_pConfig->GetAdoMaxPoolSize(),
		m_pConfig->GetDatabaseTcpPacketSize());    

    //m_pDbManager->OdbcOpenLogDB();
//  m_pDbManager->OdbcOpenGameDB
//  m_pDbManager->OdbcOpenUserDB
//  m_pDbManager->OdbcOpenBoardDB
//  m_pDbManager->OdbcOpenShopDB
//  m_pDbManager->OdbcOpenTerraDB
//  m_pDbManager->OdbcOpenGspDB
//  m_pDbManager->OdbcOpenThaiDB
//  m_pDbManager->OdbcOpenKorDB
//  m_pDbManager->OdbcOpenMyDB
    return true;
}

int CacheServer::StartWorkThread(ServerConfigXml* pConfig)
{    
	// Determine how many processors are on the system
	// Calcurate number of threads	
    sc::EM_MS_OS_TYPE OsType;
	sc::writeLogInfo(sc::string::format("%1%", sc::getOSName(OsType)));	
    sc::writeLogInfo(sc::string::format("%1% %2% Mhz", sc::getProcessorName(), sc::getCpuSpeed()));	
    sc::writeLogInfo(sc::string::format("%1% System Processor Detected", sc::getProcessorNumber()));
	
	// Work Thread 갯수 결정
	// CPU 갯수 X 2 가 기본값이다.
	m_dwCpuCount = sc::getProcessorNumber();
	m_dwWorkerThreadNumber = m_dwCpuCount * S_HEURISTIC_NUM; // Processor * 2
	
	// 설정파일의 최대값 보다 크면 설정파일에 적힌 최대값으로 제한한다.
	if (m_dwWorkerThreadNumber > (DWORD) m_pConfig->GetMaxThread())
		m_dwWorkerThreadNumber = m_pConfig->GetMaxThread();

	// 미리 정의된 최대값보다 크면 프로그램의 정의된 최대값으로 제한한다.
	if (m_dwWorkerThreadNumber > MAX_WORKER_THREAD)
		m_dwWorkerThreadNumber = MAX_WORKER_THREAD;

	// Creaate worker threads based on the number of processors	
	DWORD dwThreadId = 0;
	
	sc::writeLogInfo(
        sc::string::format("Worker thread number %1%", m_dwWorkerThreadNumber));
	
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

int CacheServer::WorkProc()
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

	while ( m_bIsRunning )
	{
		nRetCode = GetQueuedCompletionStatus(
            m_hIOServer,
            &dwByteTrans, 
            ( LPDWORD ) &dwClient,
            ( LPOVERLAPPED* ) &pPerIoData,			
            INFINITE ); 
		
		// 서버를 종료하기 위해서 종료처리 루틴을 호출했을때...
		if ( dwClient == m_dwCompKey && pPerIoData == NULL && dwByteTrans == 0 )
			break;
		
		// Illegal Message Skip. case 1
		if ( ( dwClient < 0 ) || (dwClient >= dwMaxClient ) )
		{
			m_pClientManager->ReleaseOperationData( pPerIoData );
			continue;
		}

		// release reference
		m_pClientManager->ReleaseReference( dwClient );
		
		if ( nRetCode == 0 ) // 강제종료
		{
			CloseClient( dwClient );
            m_pClientManager->ReleaseOperationData( pPerIoData );
			continue;
		}

		if (nRetCode != 0 && !pPerIoData)
		{
			CloseClient( dwClient );
			continue;
		}
		
		if ( dwByteTrans == 0 ) // 정상종료
		{
			CloseClient( dwClient );
            m_pClientManager->ReleaseOperationData( pPerIoData );
			continue;
		}

		switch ( pPerIoData->OperationType )
		{
		// 읽기 완료 통보일때...
		case NET_RECV_POSTED:
			{
				// Insert to client buffer
				m_pClientManager->addRcvMsg( dwClient, pPerIoData->Buffer, dwByteTrans );
				
				while ( true )
                {
					// Get one Message
					pNmg = ( NET_MSG_GENERIC* ) m_pClientManager->getRcvMsg( dwClient );
					if ( NULL == pNmg )
                    {
						break;
					}
					else
                    {
						// Insert to message queue
						m_pRecvMsgManager->MsgQueueInsert( dwClient, pNmg, pNmg->Size() );
					}
				}

				// OverlappedIO 메모리 반환
				m_pClientManager->ReleaseOperationData( pPerIoData );
				// WSARecv 요청
				Flags = 0;
				dwRcvBytes = sizeof( NET_MSG_GENERIC );
				pPerIoData = m_pClientManager->GetFreeOverIO( NET_RECV_POSTED );
				if ( !pPerIoData )
                    continue;

				// add reference
				m_pClientManager->AddReference( dwClient );

				SOCKET sSOCKET = m_pClientManager->GetSocket( dwClient );
				nRetCode = ::WSARecv(
								sSOCKET,
								&( pPerIoData->DataBuf ), 
								1, 
								&dwRcvBytes,
								&Flags,
								&( pPerIoData->Overlapped ),
								NULL );
				
				if ( ( nRetCode == SOCKET_ERROR ) && ( ( nLastErr = WSAGetLastError() ) != WSA_IO_PENDING ) )
				{
					// release reference
					m_pClientManager->ReleaseReference( dwClient );
					m_pClientManager->ReleaseOperationData( pPerIoData );
					CloseClient( dwClient );
				}
			}
			break;
		// 쓰기 완료 통보 일때...
		case NET_SEND_POSTED:
			dwSndBytes = dwByteTrans + pPerIoData->dwSndBytes; // 총 보낸 바이트수				
			if ( dwSndBytes >= pPerIoData->dwTotalBytes ) // 쓰기 완료됨
			{
				m_pClientManager->ReleaseOperationData( pPerIoData );
			}
			else // 쓰기 완료되지 않음 남은 바이트를 전송
			{
				pPerIoData->dwSndBytes  = dwSndBytes; // 보낸 바이트수 업데이트				
				pPerIoData->DataBuf.buf = pPerIoData->Buffer + dwSndBytes; // 보내야할 데이타 포인터 업데이트				
				dwSndBytes              = pPerIoData->dwTotalBytes - dwSndBytes; // 보내야할 바이트수 업데이트				
				pPerIoData->DataBuf.len = ( u_long )dwSndBytes; // 버퍼길이 업데이트

				// add reference
				m_pClientManager->AddReference( dwClient );

				if ( SOCKET_ERROR == ::WSASend(
					m_pClientManager->GetSocket( dwClient ),
					&( pPerIoData->DataBuf ),
					1,
					&dwSndBytes,
					Flags,
					&( pPerIoData->Overlapped ),
					NULL ) )
				{
					if ( WSAGetLastError() != ERROR_IO_PENDING )
					{
						// release reference
						m_pClientManager->ReleaseReference( dwClient );
						m_pClientManager->ReleaseOperationData( pPerIoData );
						CloseClient( dwClient );
					} 
				}
			}
			break;
		default: // case 10
			CloseClient( dwClient );
            m_pClientManager->ReleaseOperationData( pPerIoData );
			break;
		}
	}

	m_pClientManager->ReleaseOperationData( pPerIoData );

	// ServerWorkerThread Running End
	sc::writeLogInfo( std::string( "WorkProc End" ) );	
	return 0;
}

int CacheServer::StartUpdateThread()
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
            // Update Thread 의 수행레벨을 높여 성능을 향상시킨다.
            // 서버의 CPU 갯수가 2개 이상일때만 실제로 효과가 있었다.
            SetThreadPriority(m_hUpdateThread, THREAD_PRIORITY_HIGHEST);
        }
        sc::writeLogInfo(std::string("Server Update Thread Create OK"));
        setUpdateFrame(100);
        m_dwUpdateOldTime = GetTickCount();
        ResumeThread(m_hUpdateThread);
        return NET_OK;
    }
}

int CacheServer::UpdateProc()
{
    TRACE("CacheServer::UpdateProc\n");

    DWORD dwUsrChkStart		= timeGetTime();
    //DWORD dwGuidUpdate      = dwUsrChkStart; // GUID Update Time
    DWORD dwCurrentTime     = dwUsrChkStart;

    //m_bHeartBeat = false;

    //m_dwHeartbeatTime	= dwUsrChkStart;
    m_dwFrameTime		= dwUsrChkStart; // 서버 프레임 측정용 시간변수
    //m_dwConCurrentTime  = dwUsrChkStart;
    //m_fFrameCount		= 0;
    m_bUpdateEnd		= false;

    float fOldAppTime = 0;
    float fAppTime    = 0;
    float fElapsTime  = 0;
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
        // 경과시간 체크
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
            fAppTime   = DXUtil_Timer(TIMER_GETAPPTIME);		// 현재 시간
            fElapsTime = DXUtil_Timer(TIMER_GETELAPSEDTIME);	// 소요시간 계산

            // 유휴 클라이언트 ID 정리.
            m_pClientManager->ResetPreSleepCID();

            RecvMessageProcess();            
            HeartbeatCheck();

			m_pClientManager->SendClientFinal();

            FrameCheck();
            ConcurrentCheck();
            SessionServerConnectionCheck();

			FrameMove( fElapsTime );
        }

		PrintSystemInfo();	// 현재 서버의 시스템 WorkingSet size를 출력한다.
    }
    
    // Last db action
    m_pDbManager->ExecuteLastJob();
    sc::writeLogInfo(
        std::string(
            "Last Database Job Executed"));

    // 업데이트 스래드 사라짐.
    m_bUpdateEnd = true;	
    return 0;
}

int CacheServer::ListenProc()
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
	//int		nServerStateLog  = 0;
	//int		nTest = 0;
	//bool bReachedMAX = false;

	while (m_bIsRunning)
	{
		Accept = ::WSAAccept(m_sServer, NULL, NULL, NULL, 0);
		// WSAAccept Error
		if (Accept == INVALID_SOCKET) 
		{
			nRetCode = WSAGetLastError();						
			sc::writeLogError(sc::string::format("WSAAccept %1%", nRetCode));
			if (nRetCode == WSAENOTSOCK || nRetCode == WSAEINTR)
				break;
			else
				continue;
		}

		/////////////////////////////////////////////////////////////////////////////
		//sockaddr_in	sAddrIn;
		//int nSize = sizeof(sockaddr_in);
		//char szIp[MAX_IP_LENGTH+1] = {0};
		
		// Get client ip address and port
		//::getpeername(Accept, (sockaddr*) &sAddrIn, &nSize);
		//::StringCchCopy(szIp, MAX_IP_LENGTH+1, ::inet_ntoa(sAddrIn.sin_addr));
		
		//bReachedMAX = false;

		/////////////////////////////////////////////////////////////////////////////
		// Create per-handle data information structure to associate with the socket
		nClientNumber = m_pClientManager->GetFreeClientID(false); // Get free client slot number
		if (nClientNumber == NET_ERROR) 
		{
            sc::writeLogInfo(std::string("Reached max client number"));
			::closesocket(Accept);			
			//m_bReachedMAX = TRUE;
			continue;
		}		
        
		dwClient = (DWORD) nClientNumber;		
		// Set client information
		// 클라이언트의 ip, port, 접속시간을 기록함.
		if (m_pClientManager->SetAcceptedClient(dwClient, Accept) == NET_ERROR)
		{
			::closesocket( Accept );
			m_pClientManager->ReleaseClientID( dwClient );
			continue;
		}

		/*int	nZero = 0;
		if ( SOCKET_ERROR == setsockopt( Accept, SOL_SOCKET, SO_RCVBUF, ( const char* )&nZero, sizeof( nZero ) ) )
		{
			sc::writeLogError( std::string( "setsockopt SO_RCVBUF error." ) );
		}

		if ( SOCKET_ERROR == setsockopt( Accept, SOL_SOCKET, SO_SNDBUF, ( const char* )&nZero, sizeof( nZero ) ) )
		{
			sc::writeLogError( std::string( "setsockopt SO_SNDBUF error." ) );
		}*/

		hRetCode = ::CreateIoCompletionPort(
            (HANDLE) Accept, 
            m_hIOServer, 
            (DWORD) nClientNumber, 
            0);
		if (!hRetCode) 
		{
			sc::writeLogError(std::string("CreateIoCompletionPort Error"));				
			CloseClient( dwClient );
			continue;
		}
		
		// Start processing I/O on ther accepted socket
		// First WSARecv, TCP/IP Send 8 bytes (ignored byte)			
		dwRecvNumBytes = sizeof(NET_MSG_GENERIC);        
		IOData = m_pClientManager->GetFreeOverIO(NET_RECV_POSTED);
		if (!IOData)
		{
			CloseClient( dwClient );
			continue;
		}
		
		IOData->dwRcvBytes	 = 0;
		IOData->dwTotalBytes = dwRecvNumBytes;
		// Head receive mode
		//m_pClientManager->SetNetMode(dwClient, NET_PACKET_HEAD);

		// add reference
		m_pClientManager->AddReference( dwClient );

		// 클라이언트 메시지 받기 준비
		int nRetCode = ::WSARecv(
            Accept,					
            &(IOData->DataBuf), 
            1,
            &dwRecvNumBytes,
            &Flags ,
            &(IOData->Overlapped),
            NULL);
		int nLastErr = 0;
		if ( ( nRetCode == SOCKET_ERROR ) && ( ( nLastErr = WSAGetLastError() ) != WSA_IO_PENDING ) )
		{
			// release reference
			m_pClientManager->ReleaseReference( dwClient );
			m_pClientManager->ReleaseOperationData( IOData );
			CloseClient( dwClient );

			sc::writeLogError(
				sc::string::format(
				"ListenProc WSARecv Client %1% error %2%", dwClient, nLastErr ) );
			continue;
		}
	}

	// ServerAcceptThread Running End
	sc::writeLogInfo(std::string("ListenProc end"));
	return 0;
}

void CacheServer::HeartbeatCheck()
{
    DWORD dwCurrentTime = ::timeGetTime();
    if ((dwCurrentTime - m_HeartbeatTimer) > HEARTBEAT_TIME)
    {       
        // Heartbeat Check
        if (m_bHeartBeat)
            HeartbeatSend();
        else
            HeartbeatRecvCheck();
        m_bHeartBeat = !m_bHeartBeat;
        m_HeartbeatTimer = dwCurrentTime;

		DatabaseCheck();
        ClubLogCheck();

        sc::SeqUniqueGUID::Instance()->reset();
    }
}

void CacheServer::HeartbeatSend()
{
    sc::writeInfo(std::string("HeartbeatSend"));
    NET_HEARTBEAT_SERVER_REQ msg;
    for (DWORD ClientSlot=0; ClientSlot<(DWORD) m_nMaxClient; ++ClientSlot)
    {
        // 온라인 상태인경우
        if (m_pClientManager->IsOnline(ClientSlot))
        {			
            m_pClientManager->ResetHeartBeat(ClientSlot);
            SendClient(ClientSlot, &msg);
#ifdef HEARTBEAT_LOG
			sc::writeLogInfo(sc::string::format("[HEARTBEAT] Send Heartbeat Request to all Svr ( %1% )", ClientSlot));
#endif
        }
    }
}

void CacheServer::HeartbeatRecvCheck()
{
    for ( DWORD ClientSlot=0; ClientSlot<(DWORD) m_nMaxClient; ++ClientSlot )
    {	
        if ( INVALID_SOCKET != m_pClientManager->GetSocket( ClientSlot ) && false == m_pClientManager->CheckHeartBeat( ClientSlot ) )
            CloseClient( ClientSlot );
    }
}

int CacheServer::SessionConnect()
{
    if ( m_SessionSlot != NET_INVALID_SLOT && INVALID_SOCKET != m_pClientManager->GetSocket( m_SessionSlot ) )
    {
        sc::writeLogInfo(
            sc::string::format( "Session server connection is online %1%", m_SessionSlot ) );
        return NET_OK;
    }

	int SessionSlotNum = m_pClientManager->GetFreeClientID( true );
	if ( SessionSlotNum == NET_ERROR )
	{
		sc::writeLogError( std::string( "SessionConnect Not enough free network slot" ) );
		return NET_ERROR;
	}

	std::string SessionIp = m_pConfig->GetSessionServerIP();
	int SessionPort = m_pConfig->GetSessionServerPort();
    net::CONNECT_RESULT Connect = net::ConnectAndReceive( 
		SessionIp.c_str(), 
		SessionPort, 
		m_hIOServer, 
		SessionSlotNum );
    int Result = Connect.get<0> ();
    SOCKET ConnectSocket = Connect.get<1> ();
    if ( Result == NET_ERROR )
    {
        m_pClientManager->ReleaseClientID( SessionSlotNum );
        sc::writeLogError( std::string( "Session server connection failed" ) );
        return NET_ERROR;
    }
        
	if ( NET_ERROR == m_pClientManager->SetAcceptedClient( SessionSlotNum, ConnectSocket, net::SLOT_SESSION ) )
	{
		::closesocket( ConnectSocket );
		m_pClientManager->ReleaseClientID( SessionSlotNum );
		sc::writeLogError( std::string( "SessionConnect SetAcceptedClient failed" ) );
		return NET_ERROR;
	}

    m_SessionSlot = SessionSlotNum;
    m_pClientManager->SetSlotType( SessionSlotNum, net::SLOT_SESSION );    

	PER_IO_OPERATION_DATA* pIOData = m_pClientManager->GetFreeOverIO( NET_RECV_POSTED );
	if ( NULL == pIOData )
	{
		CloseClient( SessionSlotNum );
		sc::writeLogError( std::string( "SessionConnect GetFreeOverIO failed" ) );
		return NET_ERROR;
	}

	// add reference
	m_pClientManager->AddReference( SessionSlotNum );

	DWORD Flags = 0;
	DWORD dwRcvBytes = sizeof( NET_MSG_GENERIC );
	int nRetCode = WSARecv( ConnectSocket, &( pIOData->DataBuf ), 1, &dwRcvBytes, &Flags, &( pIOData->Overlapped ), NULL );
	if ( nRetCode == SOCKET_ERROR && ( WSAGetLastError() != ERROR_IO_PENDING ) )
	{
		// release reference
		m_pClientManager->ReleaseReference( SessionSlotNum );
		m_pClientManager->ReleaseOperationData( pIOData );
		CloseClient( SessionSlotNum );
		sc::writeLogError( sc::string::format( "SessionConnect WSARecv %1%", WSAGetLastError() ) );
		return NET_ERROR;
	}

    sc::writeLogInfo(
        sc::string::format( "Session connected slot %1%", SessionSlotNum ) );

    SessionSndSvrInfo();

    return NET_OK;
}

void CacheServer::SessionSndSvrInfo()
{
    G_SERVER_INFO ServerInfo;

    ServerInfo.nControlPort			= m_pConfig->GetControlPort();
    ServerInfo.nServerCurrentClient	= m_pClientManager->GetCurrentClientNumber();
    ServerInfo.nServerGroup			= m_pConfig->GetServerGroup();
    ServerInfo.nServerMaxClient		= m_pConfig->GetServerMaxClient();
    ServerInfo.nServerNumber		= m_pConfig->GetServerNumber();
    ServerInfo.ServerType			= m_pConfig->GetServerType();
    ServerInfo.nServerChannel       = 0;
    ServerInfo.nServicePort			= m_pConfig->GetServicePort();
    ServerInfo.SetServerIp(m_szAddress);

    NET_SERVER_INFO Msg(ServerInfo);
    Msg.SetType(NET_MSG_SND_FULL_SVR_INFO);

    if (SendClient(m_SessionSlot, &Msg) == NET_ERROR)
        sc::writeLogError(std::string("SessionSndSvrInfo"));
    else
        sc::writeLogInfo(sc::string::format("SessionSndSvrInfo %1%", m_szAddress));
}

void CacheServer::SessionServerConnectionCheck()
{
    DWORD CurrentTime = ::timeGetTime();
    if ((CurrentTime - m_SessionCheckTimer) > SESSION_CONNECTION_CHECK_TIME)
    {
        if (m_bIsRunning && (m_SessionSlot==NET_INVALID_SLOT || !m_pClientManager->IsOnline(m_SessionSlot)))
            SessionConnect();
        m_SessionCheckTimer = CurrentTime;
    }
}

int CacheServer::SessionCloseConnect(void)
{
    if ( !m_pClientManager )
    {
        sc::writeLogError( std::string( "SessionCloseConnect() - ClientManager is NULL." ) );
        return NET_ERROR;
    }

    return m_pClientManager->CloseClient( m_SessionSlot );
}

void CacheServer::DatabaseCheck()
{
    m_pDbManager->IsAlive();
}

void CacheServer::FrameCheck()
{
}

void CacheServer::ConcurrentCheck()
{
}

/*
int	CacheServer::Pause()
{
    return NET_OK;
}

int CacheServer::Resume()
{
    return NET_OK;
}

int	CacheServer::ReStart()
{
    return NET_OK;
}
*/

void CacheServer::FrameMove( float fElapsTime )
{

	FrameMovePrivateMarket();                   // 개인상점

	if (m_spPostCache)
		m_spPostCache->CleanupProcess( fElapsTime );

	if ( m_pMoneyMan && net::SERVER_CACHE == m_pConfig->GetServerType() )
		m_pMoneyMan->OnUpdate( fElapsTime );
}

void CacheServer::AddGameAdoJob(db::DbActionPtr spJob)
{
    if (m_pDbManager)
    {
        m_pDbManager->AddGameJob(spJob, m_pDbManager);
        return;
    }

    if (spJob)
    {
        sc::writeLogError(
            sc::string::format(
            "CacheServer::AddGameAdoJob m_pDbManager NULL %1%",
            typeid(*spJob).name()));
    }
}

void CacheServer::AddUserAdoJob(db::DbActionPtr spJob)
{
    if (m_pDbManager)
    {
        m_pDbManager->AddUserJob(spJob, m_pDbManager);
        return;
    }

    if (spJob)
    {
        sc::writeLogError(
            sc::string::format(
            "CacheServer::AddUserAdoJob m_pDbManager NULL %1%",
            typeid(*spJob).name()));
    }
}

void CacheServer::AddLogAdoJob(db::DbActionPtr spJob)
{
    if (m_pDbManager)
    {
        m_pDbManager->AddLogJob(spJob, m_pDbManager);
        return;
    }

    if (spJob)
    {
        sc::writeLogError(
            sc::string::format(
            "CacheServer::AddLogAdoJob m_pDbManager NULL %1%",
            typeid(*spJob).name()));
    }
}

void CacheServer::AddShopAdoJob(db::DbActionPtr spJob)
{
    if (m_pDbManager)
    {
        m_pDbManager->AddShopJob(spJob, m_pDbManager);
        return;
    }

    if (spJob)
    {
        sc::writeLogError(
            sc::string::format(
            "CacheServer::AddShopAdoJob m_pDbManager NULL %1%",
            typeid(*spJob).name()));
    }
}

int CacheServer::RegularScheduleProc()
{
	// 정기적으로 처리해야할 작업이 있다면 여기서 처리한다

	return 0;
}