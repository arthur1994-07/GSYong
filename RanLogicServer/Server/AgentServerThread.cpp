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

		// ª�� �ð����� ��� �����ϴ� �ҹ� Ŭ���̾�Ʈ ó�� - ip�� ���°��� �������̴�. ���θ� �׽�Ʈ������ �д�.
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
		// Ŭ���̾�Ʈ�� ip, port, ���ӽð��� �����.
		if (m_pClientManager->SetAcceptedClient(dwClient, Accept, net::SLOT_CLIENT) == NET_ERROR)
		{
			::closesocket( Accept );
			m_pClientManager->ReleaseClientID(dwClient);
			continue;
		}

		// IP �� ����Ʈ�� ����ϰ� block Address ���� �˻��Ѵ�.
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

		// ���Ǽ����� ���� �����ڼ� ����
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
		
		// �̹� �����ߴµ� �� �ϰ� �ִ�;
        //m_pClientManager->SetSlotType(dwClient, net::SLOT_CLIENT);

		// add reference
		m_pClientManager->AddReference( dwClient );

		// Ŭ���̾�Ʈ �޽��� �ޱ� �غ�
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

		// Client �� �������� ��쿡�� ������;
		// Agent�� ������ ���� �ִ�;
		if ( net::SLOT_CLIENT == m_pClientManager->GetSlotType( dwClient ) )
		{
			MsgSndCryptKey(dwClient); // Send crypt key
			MsgSndRandomNumber(dwClient, 0, NULL); // Send random password key

			// nProtect ���� ������ ����Ѵٸ� ������ �ٷ� Ŭ���̾�Ʈ���� ������ �޽����� ������.
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


	DWORD nServerStateTime  = timeGetTime(); // ���� ���� �α� ��Ͽ� �ð� ����
	m_dwHeartBeatTime		= nServerStateTime;
	//float fFPSUdateTime		= /*3000*/FPS_UPDATE_TIME;
	m_bUpdateEnd			= false;

	HANDLE hProcess	= GetCurrentProcess(); // �޸� üũ�� ����
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

	// Event ����� Thread�� ���� ����
	DWORD dwNewWaitTime = 0;
	DWORD dwReturn = 0;
	DWORD dwNewTime = 0;
	DWORD dwUpdateFrameTime = 0;
	DWORD dwHeartBeatAPEX = timeGetTime();

	DWORD dwBeforeNewWaitTime = 0;//������ �����̸� �󸶳� �־��°�?

	// Sleeep ����� Thread�� ���� ����
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

			// ����ð� üũ
			int nUpdateElspTime = (int) (m_dwUpdateWaitTime - dwUpdateFrameTime + dwBeforeNewWaitTime ) ;
			if ( 0 >= nUpdateElspTime ) 
				dwNewWaitTime = 0;
			else
				dwNewWaitTime = nUpdateElspTime;


			// ������ �󸶳� ������ ���״°�?
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
			//���� �ð��� ����
			fCurrentTime = sc::DXUtil_Timer( sc::TIMER_GETAPPTIME );


			// ���� ������ �� ������ ����
			fTimeDelta = fBeforeTime+0.001f;

			// ���� ���簡 ù �������̳� timeGetTime()�� �������� ������ �ð����� ũ�� ������
			if( fBeforeTime == 0.0f || fCurrentTime >= fTimeDelta )	
			{
				// ���η��� ó��
				// ���� �ð��� g_BeforeTime�� ����
				fBeforeTime = fCurrentTime;
			}
			else
			{
				Sleep(0);
				continue;
			}
		}

        m_CurrentTime = CTime::GetCurrentTime();

		// ����� �۾� ó�� �Ѵ�.
		ExecuteReservedTask();

		///////////////////////////////////////////////////////////////////////
		// ���� Ŭ���̾�Ʈ ID ����.
		m_pClientManager->ResetPreSleepCID ();		


		///////////////////////////////////////////////////////////////////////
		// ���� �޽������� ó���Ѵ�.
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

//#if defined( CH_PARAM ) || defined ( HK_PARAM )  || defined ( TW_PARAM ) //|| defined(_RELEASED) // Apex ����
		// APEX ���� ����
		if (m_ServiceProvider == SP_TAIWAN || m_ServiceProvider == SP_HONGKONG || m_ServiceProvider == SP_CHINA || m_ServiceProvider == SP_THAILAND)
		{
			// Apex Ŭ���̾�Ʈ ����			
			{
				ApexCloseClientFinal();			
			}
		}
//#endif
        //  ����� Client�� �����Ѵ�.
        CloseClientFinal();

		///////////////////////////////////////////////////////////////////////
		// ��Ʈ��Ʈ üũ�� �Ѵ�.
		AgentSrvHeartBeatCheck( HEARTBEAT_TIME );

		///////////////////////////////////////////////////////////////////////
		// Ŭ���̾�Ʈ ���� ������Ʈ 
		UpdateClientState();

		// ���� ĳ���� ������Ʈ 
		m_pObservePacketMgr->Update();

		///////////////////////////////////////////////////////////////////////
		// ���� ������ ���¸� ����Ʈ �Ѵ�.
		PrintDebugMsg(FPS_UPDATE_TIME);

		// ���� ������ �ý��� WorkingSet size�� ����Ѵ�.
		PrintSystemInfo();

		///////////////////////////////////////////////////////////////////////
		// ���� ���� �α� ���
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
    // UpdateThread ����
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
 
		// ������ �����ϱ� ���ؼ� ����ó�� ��ƾ�� ȣ��������...
		if ( dwClient == m_dwCompKey && pPerIoData == NULL && dwByteTrans == 0 )
		{				
			break;
		}
		
		// Illegal Message Skip, �߸��� �޽���
		if ( dwClient >= dwMaxClient )
		{				
			m_pClientManager->ReleaseOperationData( pPerIoData );
			continue;
		}

		// release reference
		m_pClientManager->ReleaseReference( dwClient );
		
		if ( nRetCode == 0 ) // ��������
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
	
		if ( dwByteTrans == 0 ) // ��������
		{
			CloseClient( dwClient, net::NORMAL_LOGOUT );
			m_pClientManager->ReleaseOperationData( pPerIoData );
			continue;
		}

		switch ( pPerIoData->OperationType )
		{
		// �б� �Ϸ� �뺸�϶�...
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

				// OverlappedIO �޸� ��ȯ				
                m_pClientManager->ReleaseOperationData( pPerIoData );

				// WSARecv ��û
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

		// ���� �Ϸ� �뺸 �϶�...
		case NET_SEND_POSTED:
			// �� ���� ����Ʈ��				
			dwSndBytes = dwByteTrans + pPerIoData->dwSndBytes;
			// ���� �Ϸ��
			if ( dwSndBytes >= pPerIoData->dwTotalBytes )
			{
                m_pClientManager->ReleaseOperationData( pPerIoData );
			}
			else
			{
				pPerIoData->dwSndBytes  = dwSndBytes; // ���� ����Ʈ�� ������Ʈ				
				pPerIoData->DataBuf.buf = pPerIoData->Buffer + dwSndBytes; // �������� ����Ÿ ������ ������Ʈ				
				dwSndBytes              = pPerIoData->dwTotalBytes - dwSndBytes; // �������� ����Ʈ�� ������Ʈ				
				pPerIoData->DataBuf.len = ( u_long )dwSndBytes; // ���۱��� ������Ʈ

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

    // GameGuardThread ����
	EndRegularScheduleThread();
	return 0;
}