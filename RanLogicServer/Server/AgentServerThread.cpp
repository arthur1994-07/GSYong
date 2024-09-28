#include "../pch.h"
#include "Psapi.h"
#pragma comment( lib, "Psapi.lib" )

#include "../../SigmaCore/Util/EBTime.h"
#include "../Client/NetAgentClientMan.h"
#include "../Util/IpAddressChecker.h"
#include "../PacketObserver/GLPacketObserver.h"
#include "./AgentServer.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

int CAgentServer::ListenProc(void)
{
	TRACE(_T("in s_CAgentServerThread.cpp  CAgentServer::ListenProc\n"));

	PER_IO_OPERATION_DATA* IOData = NULL;
	SOCKET	Accept;
	DWORD	dwRecvNumBytes = 0;
	DWORD	Flags = 0;
	HANDLE	hRetCode = NULL;
	int		nClientNumber = 0;
	DWORD   dwClient = 0;
	int		nRetCode = 0;

	int		nReachedErrorCnt = 0;
	int		nServerStateLog  = 0;
	bool	bReachedError = FALSE;
	int		nTest = 0;
	m_bReachedMAX = FALSE;

	while (m_bIsRunning)
	{
		Accept = ::WSAAccept(m_sServer, NULL, NULL, NULL, 0);
		// WSAAccept Error
		if (Accept == INVALID_SOCKET) 
		{
			nRetCode = ::WSAGetLastError();
						
			sc::writeLogError(sc::string::format("WSAAccept %1%", nRetCode));
			if (nRetCode == WSAENOTSOCK || nRetCode == WSAEINTR)
				break;
			else
				continue;
		}

		/////////////////////////////////////////////////////////////////////////////
		sockaddr_in	sAddrIn;
		int nSize = sizeof(sockaddr_in);
		char szIp[MAX_IP_LENGTH+1] = {0};
		
		// Get client ip address and port
		::getpeername(Accept, (sockaddr *) &sAddrIn, &nSize);
		::StringCchCopy(szIp, MAX_IP_LENGTH+1, ::inet_ntoa(sAddrIn.sin_addr));	

		// Check block ip address
        /*
		if (SERVER_UTIL::CAddressChecker::GetInstance()->isBlock(szIp, timeGetTime()) == true)
		{
			//::shutdown(Accept, SB_BOTH);
			::closesocket(Accept);
			continue;
		}
        */

        if (!m_pAddressChecker->IsAllowIp(szIp))
        {
            sc::writeLogError(
                sc::string::format(
                    "Block ip %1%", szIp));
            ::closesocket(Accept);
            continue;
        }
		
		m_bReachedMAX = FALSE;	

		// 짧은 시간동안 계속 접속하는 불법 클라이언트 처리 - ip로 막는것은 무리수이다. 내부만 테스트용으로 둔다.
		/*switch ( m_ServiceProvider )
		{
		case SP_GS:
			{
				if ( false == m_pClientManager->VerifyIP( szIp ) )
				{
					sc::writeLogError(
						sc::string::format(
						"VerifyIP ip %1%", szIp));
					::closesocket(Accept);
					continue;
				}
			}
			break;
		default:
			break;
		}*/


		/////////////////////////////////////////////////////////////////////////////
		// Create per-handle data information structure to associate with the socket
		nClientNumber = m_pClientManager->GetFreeClientID(false); // Get free client slot number
		
		if (nClientNumber == NET_ERROR) 
		{
			//::shutdown(Accept, SB_BOTH);
			::closesocket(Accept);

			if( nReachedErrorCnt % 500 == 0 )
			{				
				sc::writeLogInfo(
                    sc::string::format(
                        "Reached Max Client Number vecIDSize %1% deqIDSize %2% MaxClientNum %3% CurrentClientNum %4% ReachedMaxCnt %5%", 
                        m_pClientManager->GetVecSleepSize(),
                        m_pClientManager->GetDeqSleepSize(),	
                        m_pClientManager->GetMaxClient(),
                        m_pClientManager->GetCurrentClientNumber(),
                        nReachedErrorCnt));
			}

			nReachedErrorCnt++;
			
			m_bReachedMAX = TRUE;
			bReachedError = TRUE;
			continue;
		}
		if( bReachedError )
		{			
			sc::writeLogInfo(
                sc::string::format(
                    "End reached Max Client Number vecIDSize %1% deqIDSize %2% MaxClientNum %3% CurrentClientNum %4%", 
                    m_pClientManager->GetVecSleepSize(),
                    m_pClientManager->GetDeqSleepSize(),
                    m_pClientManager->GetMaxClient(),
                    m_pClientManager->GetCurrentClientNumber()));
			nReachedErrorCnt = 0;
		}

		bReachedError = FALSE;

		if( nServerStateLog >= 500 )
		{
			sc::writeLogInfo(
                sc::string::format(
                    "LISTEN_SERVER_INFO: vecIDSize %1% deqIDSize %2% MaxClientNum %3% CurrentClientNum %4%",
                    m_pClientManager->GetVecSleepSize(),
                    m_pClientManager->GetDeqSleepSize(),	
                    m_pClientManager->GetMaxClient(),
                    m_pClientManager->GetCurrentClientNumber()));
			nServerStateLog = 0;
		}
        else
        {
			nServerStateLog++;
		}
		
		dwClient = (DWORD) nClientNumber;
		
		// Set client information
		// 클라이언트의 ip, port, 접속시간을 기록함.
		if (m_pClientManager->SetAcceptedClient(dwClient, Accept, net::SLOT_CLIENT) == NET_ERROR)
		{
			::closesocket( Accept );
			m_pClientManager->ReleaseClientID(dwClient);
			continue;
		}

		// IP 를 리스트에 등록하고 block Address 인지 검사한다.
		/*
		if (SERVER_UTIL::CAddressChecker::GetInstance()->IsBlock(m_pClientManager->GetClientIP(dwClient)) == true)
		{
            CloseClient(dwClient);
			continue;
		}
		*/
		// SERVER_UTIL::CAddressChecker::GetInstance()->IsBlock(m_pClientManager->GetClientIP(dwClient));
		
		/*int	nZero = 0;
		if ( SOCKET_ERROR == setsockopt( Accept, SOL_SOCKET, SO_RCVBUF, ( const char* )&nZero, sizeof( nZero ) ) )
		{
			sc::writeLogError( std::string( "setsockopt SO_RCVBUF error." ) );
		}

		if ( SOCKET_ERROR == setsockopt( Accept, SOL_SOCKET, SO_SNDBUF, ( const char* )&nZero, sizeof( nZero ) ) )
		{
			sc::writeLogError( std::string( "setsockopt SO_SNDBUF error." ) );
		}*/

		// Associate the accepted socket with the completion port
		hRetCode = ::CreateIoCompletionPort(
            (HANDLE) Accept, 
            m_hIOServer, 
            (DWORD) nClientNumber, 
            0);
		if (hRetCode == NULL) 
		{
			sc::writeLogError(std::string("CreateIoCompletionPort Error"));				
			CloseClient(dwClient, net::ERROR_CREATE_IOCP);
			continue;
		}

		// 세션서버에 현재 접속자수 전송
		SessionSndSvrCurState();

		// Start processing I/O on ther accepted socket
		// First WSARecv, TCP/IP Send 8 bytes (ignored byte)			
		dwRecvNumBytes = sizeof(NET_MSG_GENERIC);
        
		//IOData = getRecvIO( dwClient );
		//IOData = (PER_IO_OPERATION_DATA*) GetFreeOverIO(NET_RECV_POSTED);
        IOData = m_pClientManager->GetFreeOverIO(NET_RECV_POSTED);
		if (!IOData)
		{
			CloseClient(dwClient, net::ERROR_IO_OPERATION, WSAGetLastError());
			continue;
		}
		
		IOData->dwRcvBytes		= 0;
		IOData->dwTotalBytes	= dwRecvNumBytes;
		// Head receive mode
		//m_pClientManager->SetNetMode(dwClient, NET_PACKET_HEAD);
		
		// 이미 설정했는데 또 하고 있다;
        //m_pClientManager->SetSlotType(dwClient, net::SLOT_CLIENT);

		// add reference
		m_pClientManager->AddReference( dwClient );

		// 클라이언트 메시지 받기 준비
		int nRetCode = ::WSARecv(
            Accept,					
            &( IOData->DataBuf ), 
            1,
            &dwRecvNumBytes,
            &Flags ,
            &( IOData->Overlapped ),
            NULL );
		int nLastErr = 0;
		if ( ( nRetCode == SOCKET_ERROR ) && ( ( nLastErr = WSAGetLastError() ) != WSA_IO_PENDING ) )
		{
			// release reference
			m_pClientManager->ReleaseReference( dwClient );
			m_pClientManager->ReleaseOperationData( IOData );
			CloseClient( dwClient, net::ERROR_IO_OPERATION, WSAGetLastError() );
			
			sc::writeLogError(
				sc::string::format(
				"ListenProc WSARecv Client %1% error %2%", dwClient, nLastErr ) );
			continue;
		}

		// Client 가 접속했을 경우에만 보낸다;
		// Agent도 접속할 수가 있다;
		if ( net::SLOT_CLIENT == m_pClientManager->GetSlotType( dwClient ) )
		{
			MsgSndCryptKey(dwClient); // Send crypt key
			MsgSndRandomNumber(dwClient, 0, NULL); // Send random password key

			// nProtect 서버 인증을 사용한다면 접속후 바로 클라이언트에게 인증용 메시지를 보낸다.
			MsgSndGameGuardFirstCheck( dwClient );			
		}
		
		nClientNumber = NET_ERROR;
        m_pAddressChecker->AddConnection(szIp);
        if (!m_bUseEventThread)
            Sleep(0);
	}

	// ServerAcceptThread Running End
	sc::writeLogInfo(std::string("== ListenProc End"));
	

	return 0;
}

int CAgentServer::UpdateProc()
{
	TRACE(_T("in s_CAgentServerThread.cpp  CAgentServer::UpdateProc\n"));


	DWORD nServerStateTime  = timeGetTime(); // 서버 상태 로그 기록용 시간 변수
	m_dwHeartBeatTime		= nServerStateTime;
	//float fFPSUdateTime		= /*3000*/FPS_UPDATE_TIME;
	m_bUpdateEnd			= false;

	HANDLE hProcess	= GetCurrentProcess(); // 메모리 체크용 변수
	if (hProcess)
	{				
		PROCESS_MEMORY_COUNTERS pmc;
		if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc)))
		{
			sc::writeLogInfo(
                sc::string::format(
                    "SERVER_INFO Memory %1% vecIDSize %2% deqIDSize %3% MaxClientNum %4% CurrentClientNum %5%",
                    pmc.WorkingSetSize,
                    m_pClientManager->GetVecSleepSize(),
                    m_pClientManager->GetDeqSleepSize(),	
                    m_pClientManager->GetMaxClient(),
                    m_pClientManager->GetCurrentClientNumber()));
		}
	}

	// Event 방식의 Thread를 위한 변수
	DWORD dwNewWaitTime = 0;
	DWORD dwReturn = 0;
	DWORD dwNewTime = 0;
	DWORD dwUpdateFrameTime = 0;
	DWORD dwHeartBeatAPEX = timeGetTime();

	DWORD dwBeforeNewWaitTime = 0;//이전에 딜레이를 얼마나 주었는가?

	// Sleeep 방식의 Thread를 위한 변수
	double fBeforeTime = 0.0f;
	double fCurrentTime = 0.0f;
	double fTimeDelta = 0.0f;


	while (m_bIsRunning)
	{
		if( m_bUseEventThread )	
		{
			dwNewTime = ::GetTickCount();
			dwUpdateFrameTime = dwNewTime - m_dwUpdateOldTime;
			m_dwUpdateOldTime = dwNewTime;

			// 경과시간 체크
			int nUpdateElspTime = (int) (m_dwUpdateWaitTime - dwUpdateFrameTime + dwBeforeNewWaitTime ) ;
			if ( 0 >= nUpdateElspTime ) 
				dwNewWaitTime = 0;
			else
				dwNewWaitTime = nUpdateElspTime;


			// 이전에 얼마나 딜레이 시켰는가?
			dwBeforeNewWaitTime = dwNewWaitTime;

			//
			//mjeon
			//m_hUpdateQuitEvent is set only by CFieldServer::Stop() method when the server stops.
			//
			dwReturn = ::WaitForSingleObject( m_hUpdateQuitEvent, dwNewWaitTime );

			if ( WAIT_OBJECT_0 == dwReturn )
				break;
            else if ( WAIT_TIMEOUT != dwReturn )
				continue;
		}
        else
        {
			//현재 시간을 얻어옴
			fCurrentTime = sc::DXUtil_Timer( sc::TIMER_GETAPPTIME );


			// 다음 랜더링 될 시점을 구함
			fTimeDelta = fBeforeTime+0.001f;

			// 만약 현재가 첫 프레임이나 timeGetTime()이 랜더링될 시점의 시간보다 크면 랜더링
			if( fBeforeTime == 0.0f || fCurrentTime >= fTimeDelta )	
			{
				// 메인루프 처리
				// 현재 시간을 g_BeforeTime에 넣음
				fBeforeTime = fCurrentTime;
			}
			else
			{
				Sleep(0);
				continue;
			}
		}

        m_CurrentTime = CTime::GetCurrentTime();

		// 예약된 작업 처리 한다.
		ExecuteReservedTask();

		///////////////////////////////////////////////////////////////////////
		// 유휴 클라이언트 ID 정리.
		m_pClientManager->ResetPreSleepCID ();		


		///////////////////////////////////////////////////////////////////////
		// 받은 메시지들을 처리한다.
		RecvMsgProcess();

		///////////////////////////////////////////////////////////////////////
		// Update Gaea Server

		DxServerInstance::AgentFrameMove(m_pGLAgentServer);

		m_pClientManager->SendClientFinal();

		//
		//mjeon.post
		//mjeon.attendance
		//
		DailyMaintenance();

//#if defined( CH_PARAM ) || defined ( HK_PARAM )  || defined ( TW_PARAM ) //|| defined(_RELEASED) // Apex 적용
		// APEX 버젼 변경
		if (m_ServiceProvider == SP_TAIWAN || m_ServiceProvider == SP_HONGKONG || m_ServiceProvider == SP_CHINA || m_ServiceProvider == SP_THAILAND)
		{
			// Apex 클라이언트 종료			
			{
				ApexCloseClientFinal();			
			}
		}
//#endif
        //  예약된 Client를 종료한다.
        CloseClientFinal();

		///////////////////////////////////////////////////////////////////////
		// 하트비트 체크를 한다.
		AgentSrvHeartBeatCheck( HEARTBEAT_TIME );

		///////////////////////////////////////////////////////////////////////
		// 클라이언트 상태 업데이트 
		UpdateClientState();

		// 블럭 캐릭터 업데이트 
		m_pObservePacketMgr->Update();

		///////////////////////////////////////////////////////////////////////
		// 현재 서버의 상태를 프린트 한다.
		PrintDebugMsg(FPS_UPDATE_TIME);

		// 현재 서버의 시스템 WorkingSet size를 출력한다.
		PrintSystemInfo();

		///////////////////////////////////////////////////////////////////////
		// 서버 상태 로그 기록
		DWORD dwCurrentTime = timeGetTime();
		if ((dwCurrentTime - nServerStateTime) >= SERVER_STATE_CHK_TIME)
		{					
			if (hProcess)
			{				
				PROCESS_MEMORY_COUNTERS pmc;
				if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc)))
				{
					sc::writeLogInfo(
                        sc::string::format(
                            "SERVER_INFO Memory %1% vecIDSize %2% deqIDSize %3% MaxClientNum %4% CurrentClientNum %5%",
                            pmc.WorkingSetSize,
                            m_pClientManager->GetVecSleepSize(),
                            m_pClientManager->GetDeqSleepSize(),	
                            m_pClientManager->GetMaxClient(),
                            m_pClientManager->GetCurrentClientNumber()));
				}
			}
			nServerStateTime = dwCurrentTime;
            
            if (m_pAddressChecker)
                m_pAddressChecker->ClearIpData();
		}
	}
	///////////////////////////////////////////////////////////////////////
    // UpdateThread 종료
	EndUpdateThread();
	return 0;
}

int CAgentServer::WorkProc()
{
	TRACE( _T( "in s_CAgentServerThread.cpp  CAgentServer::WorkProc\n" ) );

	PER_IO_OPERATION_DATA* pPerIoData = NULL;
	DWORD dwSndBytes   = 0;
	DWORD dwRcvBytes   = 0;
	DWORD dwByteTrans  = 0;
	DWORD Flags        = 0;
	DWORD dwClient     = -1;
	int	  nRetCode     = 0;		
	DWORD dwMaxClient  = m_pClientManager->GetMaxClient();
	DWORD dwMaxClient2 = dwMaxClient*2;
	int   nLastErr( 0 );
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
		{				
			break;
		}
		
		// Illegal Message Skip, 잘못된 메시지
		if ( dwClient >= dwMaxClient )
		{				
			m_pClientManager->ReleaseOperationData( pPerIoData );
			continue;
		}

		// release reference
		m_pClientManager->ReleaseReference( dwClient );
		
		if ( nRetCode == 0 ) // 강제종료
		{
			CloseClient( dwClient, net::ERROR_IO_OPERATION, WSAGetLastError() );
			m_pClientManager->ReleaseOperationData( pPerIoData );
			continue;
		}

		if ( nRetCode != 0 && pPerIoData == NULL )
		{
			CloseClient( dwClient, net::ERROR_IO_OPERATION, WSAGetLastError() );
			continue;
		}
	
		if ( dwByteTrans == 0 ) // 정상종료
		{
			CloseClient( dwClient, net::NORMAL_LOGOUT );
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
				
				if ( ( nRetCode == SOCKET_ERROR ) && ( ( nLastErr=WSAGetLastError() ) != WSA_IO_PENDING ) )
				{
					// release reference
					m_pClientManager->ReleaseReference( dwClient );
					m_pClientManager->ReleaseOperationData( pPerIoData );
					CloseClient( dwClient, net::ERROR_IO_OPERATION, WSAGetLastError() );
				}
			}
			break;

		// 쓰기 완료 통보 일때...
		case NET_SEND_POSTED:
			// 총 보낸 바이트수				
			dwSndBytes = dwByteTrans + pPerIoData->dwSndBytes;
			// 쓰기 완료됨
			if ( dwSndBytes >= pPerIoData->dwTotalBytes )
			{
                m_pClientManager->ReleaseOperationData( pPerIoData );
			}
			else
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
						CloseClient( dwClient, net::ERROR_IO_OPERATION, WSAGetLastError() );
					} 
				}
			}
			break;
		default: // case 10				
			m_pClientManager->ReleaseOperationData( pPerIoData );
			CloseClient( dwClient, net::ERROR_UNKOWN_IO_OPERATION );			
			break;
		}
		if ( !m_bUseEventThread ) 
			Sleep( 0 );
	}
	
	// ServerWorkerThread Running End
	sc::writeLogInfo( std::string( "WorkProc End" ) );
	
	return 0;
}

int CAgentServer::RegularScheduleProc()
{
	TRACE(_T("in s_CAgentServerThread.cpp  CAgentServer::RegularScheduleProc\n"));

	while (m_bIsRunning)
	{
		APEX_SERVER::TickDataFromApex();

		Sleep( 1 );
	}

    // GameGuardThread 종료
	EndRegularScheduleThread();
	return 0;
}
