#include "../pch.h"
#include "../../SigmaCore/Util/EBTime.h"
#include "../../SigmaCore/Math/SeqUniqueGuid.h"
#include "../../EngineLib/Common/profile.h"
#include "../../RanLogic/Msg/ServerMsg.h"

#include "../Client/NetSessionClientMan.h"
#include "../Database/DbAction/DbActionLog.h"
#include "../Database/DbManager.h"
#include "./SessionServer.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


int CSessionServer::ListenProc()
{
	PER_IO_OPERATION_DATA* IOData = NULL;
	SOCKET	Accept;
	DWORD	dwRecvNumBytes = 0;
	DWORD	dwFlags = 0;
	HANDLE	hRetCode = NULL;
	int		nClientNumber = 0;
	int		nRetCode = 0;

	while (m_bIsRunning)
	{
		Accept = ::WSAAccept( m_sServer, NULL, NULL, NULL, 0 );
		if (Accept == INVALID_SOCKET) 
		{
			nRetCode = ::WSAGetLastError();

			sc::writeLogError(sc::string::format("WSAAccept %1%", nRetCode));
			if (nRetCode == WSAENOTSOCK || nRetCode == WSAEINTR)
				break;
			else
				continue;
		}
		
		// Get free client slot number
		nClientNumber = m_pClientManager->GetFreeClientID(false);
		if (nClientNumber == NET_ERROR) 
		{
			closesocket(Accept);
			sc::writeLogError(std::string("CSessionServer::ListenProc Not available client slot"));
			continue;
		}
		else
		{
			// Set client information
			if ( NET_ERROR == m_pClientManager->SetAcceptedClient( nClientNumber, Accept ) )
			{
				::closesocket( Accept );
				m_pClientManager->ReleaseClientID( nClientNumber );
				continue;
			}
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

		// IP �� ����Ʈ�� ����Ѵ�.
		// SERVER_UTIL::CAddressChecker::GetInstance()->IsBlock(m_pClientManager->GetClientIP((DWORD) nClientNumber));

		// Associate the accepted socket with the completion port
		hRetCode = ::CreateIoCompletionPort(
						(HANDLE) Accept,
						m_hIOServer, 
						(DWORD) nClientNumber, 
						0 );
		if (hRetCode == NULL)
		{
			sc::writeLogError(std::string("CSessionServer::ListenProc CreateIoCompletionPort Error"));
			::closesocket(Accept);
			continue;
		}

		// Start processing I/O on ther accepted socket
		// First WSARecv, TCP/IP Send 8 bytes (ignored byte)
		// ���� ������ �޴´�.
		dwRecvNumBytes = sizeof(NET_MSG_GENERIC);
		//IOData = getRecvIO( nClientNumber );
		IOData = (PER_IO_OPERATION_DATA*) GetFreeOverIO(NET_RECV_POSTED);

		if (IOData == NULL)
		{
			CloseClient( nClientNumber );
			continue;
		}

		IOData->dwRcvBytes = 0;
		IOData->dwTotalBytes = dwRecvNumBytes;
		// Head receive mode
		//m_pClientManager->SetNetMode(nClientNumber, NET_PACKET_HEAD);		

		// add reference
		m_pClientManager->AddReference( nClientNumber, false );

		int nRetCode = ::WSARecv(
			Accept,
			&(IOData->DataBuf), 
			1,
			&dwRecvNumBytes,
			&dwFlags,
			&(IOData->Overlapped),
			NULL);
		int nLastErr = 0;
		if ( ( nRetCode == SOCKET_ERROR ) && ( ( nLastErr = WSAGetLastError() ) != WSA_IO_PENDING ) )
		{
			// release reference
			m_pClientManager->ReleaseReference( nClientNumber );
			m_pClientManager->ReleaseOperationData( IOData );
			CloseClient( nClientNumber );

			sc::writeLogError(
				sc::string::format(
				"ListenProc WSARecv Client %1% error %2%", nClientNumber, nLastErr ) );
			continue;
		}
		
        // Request server information
        if (!m_bUseEventThread) 
            Sleep(0);
        MsgReqSvrFullInfo((DWORD) nClientNumber);
        SendVersionInfo();

		if (!m_bUseEventThread)
			Sleep(0);
	}
	sc::writeLogInfo(std::string("Listen Thread Stopped"));
	return 0;
}

int CSessionServer::UpdateProc()
{
	DWORD dwTimeS = 0;
	float fFrame = 0;
	float fCount = 0;

	DWORD dwUsrChkTime	   = 1800000;		// 30 �и��� �ѹ��� ����� ���Ӳ��� üũ
	DWORD dwUsrChkStart	   = timeGetTime(); // �ƹ��͵� ���� �ʴ� ����� ���Ӳ��⸦ ���� �ð�
	DWORD dwUsrSaveTime	   = 60000 * 30;	// ����� ����ð� 30��
	DWORD dwUsrSaveStart   = timeGetTime(); // ����� ����ð� üũ�� ���� �ð�����
	dwTimeS				   = timeGetTime();
	m_bUpdateEnd		   = false;
    DWORD dwHeartBeatStart = timeGetTime(); // ������ ���󰡵������� üũ�Ѵ�.
    bool  bHeartBeat	   = false;			// ���󰡵� üũ �÷���    
    DWORD dwMaxClient      = (DWORD) m_nMaxClient;
	MSG_LIST* pMsg         = NULL;	
	

	// Event ����� Thread�� ���� ����
	DWORD dwNewWaitTime = 0;
	DWORD dwReturn = 0;
	DWORD dwNewTime = 0;
	DWORD dwUpdateFrameTime = 0;
	DWORD dwBeforeNewWaitTime = 0;//������ �����̸� �󸶳� �־��°�?

	double fBeforeTime = 0.0f;
	double fCurrentTime = 0.0f;
	double fTimeDelta = 0.0f;

	//
	//mjeon
	//m_dwUpdateFrame was set by 100 in CServer::setUpdataFrame().
	//m_dwUpdateWaitTime was set by 10 (1000/m_dwUpdateFrame) in CServer::setUpdateFrame().
	//m_dwUpdateOldTime = ::GetTickCount() in CServer::setUpdateFrame().	//
			
	while (m_bIsRunning)
	{
		if( m_bUseEventThread )	
		{
			dwNewTime = ::GetTickCount();
			dwUpdateFrameTime = dwNewTime - m_dwUpdateOldTime;
			m_dwUpdateOldTime = dwNewTime;

			// ����ð� üũ

			//
			//mjeon
			//m_dwUpdateWaitTime never chages. It is always 10 here. 
			//
			//If UpdateFrameDuration is more than 10ms then do not wait for UpdateQuitEvent.
			//When UpdateFrameDuration is less than 10ms(i.e.when UpdateProc is not busy),
			//wait for UpdateQuitEvent for (10ms - current UpdateFrameDuration).
			//
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
			{
				break;
			}else if ( WAIT_TIMEOUT != dwReturn )
			{
				continue;
			}
		}
		else
		{
			// ���� �ð��� ����
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
				//
				//mjeon
				//Sleep() - A value of zero causes the thread to relinquish the remainder of its time slice
				//to any other thread of equal priority that is ready to run. 
				//If there are no other threads of equal priority ready to run, 
				//the function returns immediately, and the thread continues execution.
				//
				Sleep(0);
				continue;
			}
		}

		m_pClientManager->ResetPreSleepCID();		
		m_pRecvMsgManager->MsgQueueFlip();	// Flip Messge Queue		
		pMsg = m_pRecvMsgManager->GetMsg(NULL);	// Get Message
		

		while (pMsg)
		{
			MsgProcess(pMsg);
			pMsg = m_pRecvMsgManager->GetMsg(pMsg);
		}


		/////////////////////////////////////////////////////////////////////////////
		// Additional Update procedure
		/////////////////////////////////////////////////////////////////////////////

        if ((timeGetTime() - dwHeartBeatStart) > HEARTBEAT_TIME)
        {
//#ifdef _USE_AUTH
//			//if (m_ServiceProvider == SP_EU)
//			//{
//				if( m_ServiceProvider == SP_KOREA || m_ServiceProvider == SP_GS || m_ServiceProvider == SP_OFFICE_TEST || m_ServiceProvider == SP_KOREA_TEST )
//				{
//					CheckAuthServerSocket(); // �������� ���������� ����� �ȵǴ� ���������� �ӽ÷� �߰�
//				}
//
//				if ( m_bReconnect )
//				{
//					if (ConnectAuthServer() == NET_OK )
//					{
//						m_bReconnect = FALSE;
//						sc::writeLogInfo(std::string("Reconnect SUCCESS to Auth Server"));
//					}
//					else
//					{
//						sc::writeLogError(std::string("Reconnect FAIL to Auth Server"));
//					}
//				}
//			//}
//#endif

            SendVersionInfo();

            sc::SeqUniqueGUID::Instance()->reset();

			//
			//mjeon
			//bHearBeat is automatically toggled per a loop just before this if-condition ends up.
			//
            if (bHeartBeat) // Send Heartbeat Request
			{
				NET_HEARTBEAT_SERVER_REQ MsgHeartbeatServerReq;

				sc::writeInfo(std::string("Send Heartbeat Request"));

				for (DWORD dwN=1; dwN<dwMaxClient; ++dwN)
				{
					// �¶��� �����ΰ�� // ���������δ� heartbeat Ȯ���� ���� ����
					if ( INVALID_SOCKET != m_pClientManager->GetSocket( dwN ) && dwN != m_nAuthServerSlot )
					{
						m_pClientManager->ResetHeartBeat( dwN );// Set all hearbeat false
						SendClient( dwN, &MsgHeartbeatServerReq );
#ifdef HEARTBEAT_LOG
						sc::writeLogInfo(sc::string::format("[HEARTBEAT] Send Heartbeat Request to all servers ( %1% )", dwN ));
#endif
					}
				}
			}
			else // Check Heartbeat
			{
				sc::writeInfo(std::string("Check Heartbeat"));
#ifdef HEARTBEAT_LOG
				sc::writeLogInfo(std::string("[HEARTBEAT] Check Heartbeat state"));
#endif
				//
				//mjeon
				//SessionServer handles all other kinds of servers as normal clients 
				//that has client number greater than NET_RESERVED_SLOT.
				//Other servers allocate the slots less than NET_RESERVED_SLOT for the other servers.
				//i.e. net::NET_RESERVED_SESSION = NET_RESERVED_SLOT -1
				//

				for ( DWORD dwN=1; dwN<dwMaxClient; ++dwN )
				{
					// ���������δ� heartbeat Ȯ���� ���� ����
					if ( INVALID_SOCKET != m_pClientManager->GetSocket( dwN ) && !m_pClientManager->CheckHeartBeat( dwN ) && m_pClientManager->GetSlotType(dwN) != net::SLOT_AUTH )
					{
						// ������ ��������� �˸���.
						G_SERVER_INFO gsi = m_pClientManager->GetSvrInfo( dwN );

						// ������� ������ ���´�.
						CloseClient( dwN );

						sc::writeLogError(
							sc::string::format(
							"In UpdateProc Session server close this server ip: %1% group: %2% type %3% because !CheckHeartBeat",
							gsi.szServerIP,
							gsi.nServerGroup,
							gsi.ServerType ) );

						// ����Ÿ���� �α���, �ʵ�, ������Ʈ �϶�
                        if ( 
							gsi.ServerType == net::SERVER_LOGIN ||
                            gsi.ServerType == net::SERVER_FIELD ||
                            gsi.ServerType == net::SERVER_AGENT ||
                            gsi.ServerType == net::SERVER_CACHE || 
							gsi.ServerType == net::SERVER_INTEGRATION_CACHE )
                        {
                            // ��� ���� �޴����� ���� �ٿ��� �˸���.
                            for ( DWORD dwL=1; dwL<dwMaxClient; ++dwL )
                            {
								//
								//mjeon
								//In case of SessionServer, if nServerType is not set to any value except 0(initialized value),
								//the client means ServerManager.
								//
                                if ( 
									INVALID_SOCKET != m_pClientManager->GetSocket( dwL ) && 
									( m_pClientManager->GetSvrType( dwL ) == net::SERVER_UNKNOWN ) )
                                {
                                    NET_SERVER_INFO nsi( gsi );
                                    nsi.SetType( NET_MSG_SVR_DOWN );
                                    SendClient( dwL, &nsi );

									sc::writeLogError(
                                        sc::string::format(
                                            "Server close detected ip: %1% group: %2% type %3%",
                                            gsi.szServerIP,
                                            gsi.nServerGroup,
                                            gsi.ServerType ) );
                                }
                            }
                        }
					}
				}
			}
            bHeartBeat = !bHeartBeat;
            dwHeartBeatStart = timeGetTime();
        }

		if ((timeGetTime() - dwTimeS) >= FPS_UPDATE_TIME )
		{
			//
			//mjeon
			//FPS_UPDATE_TIME is 1min (60000ms) that is the frequency of FPS update on the console.
			//That is FPS value is updated once per every single minute.
			//
			//fFrame = ( (float)((fCount * FPS_UPDATE_TIME) /(float)(dwTimeTemp - dwTimeS)) );
			// is equivalent to
			//fFrame = ( (float)(fCount / (float)( (dwTimeTemp - dwTimeS)/FPS_UPDATE_TIME) );
			// In other words, fFrame means the count how many loops(updates) has done in 1 minute.
			//
			//mjeon.ToDo
			//So I believe that the value name "fFrame" must be "fFPM" or "fFramesPerMin"
			//to prevent confusing.
			//

			
			// �������ϸ� 
			DWORD dwProfileStart = timeGetTime();
			PROFILE_DUMPOUTPUT_FILE();
			DWORD dwProfileEnd = timeGetTime() - dwProfileStart;

			// ó���ð��� 500msec �̻��̶�� ����Ѵ�.
			if (dwProfileEnd > 500)
			{
				sc::writeLogInfo( sc::string::format("INFO:PROFILE_DUMPOUTPUT_FILE %d msec", dwProfileEnd) );
			}

			fFrame = ( (float)((fCount * FPS_UPDATE_TIME) /(float)(timeGetTime() - dwTimeS)));
			//nFrame = Count; 
			dwTimeS = timeGetTime();
			fCount = 0;

			// ���� ����ڼ� ���÷���
			// ������ ���� ���÷���			

			sc::writeInfo(sc::string::format("Update %d FPS Avg %d msec User %d/%d",
				(int) (fFrame/(FPS_UPDATE_TIME/1000)),
				(int) (FPS_UPDATE_TIME/fFrame),
				(int) m_pClientManager->GetCurrentClientNumber(),
				(int) m_pClientManager->GetMaxClient() ));


			// ��Ŷ ó���� ���÷���
			/*
			CConsoleMessage::GetInstance()->WriteInfo(
				_T("In Packet Count %d/sec Traffic %d(bytes)/sec"),
				(int) (m_pClientManager->getInPacketCount()/(FPS_UPDATE_TIME/1000)),
				(int) (m_pClientManager->getInPacketSize()/(FPS_UPDATE_TIME/1000)) );
			*/

			sc::writeInfo(sc::string::format("In Packet Count %d/sec Traffic %d(bytes)/sec",
				(int) (m_pClientManager->getInPacketCount()/(FPS_UPDATE_TIME/1000)),
				(int) (m_pClientManager->getInPacketSize()/(FPS_UPDATE_TIME/1000)) ));
			

			sc::writeInfo(sc::string::format("Out Packet Count %d/sec Traffic %d(bytes)/sec",
				(int) (m_pClientManager->getOutPacketCount() / (FPS_UPDATE_TIME/1000)),
				(int) (m_pClientManager->getOutPacketSize() / (FPS_UPDATE_TIME/1000)) ));


			sc::writeInfo(
                sc::string::format(
                    "Msg Queue Size %1%", //Send %2% Recv %3%",
				    m_pRecvMsgManager->GetCount()
                    //m_pClientManager->GetUseSendMemoryCount()
                    //m_pClientManager->GetUseRecvMemoryCount()
					));

			// ���� ���� �����ڼ� DB �� ����
			for (int nLoop=0; nLoop < m_pClientManager->GetMaxClient(); nLoop++)
			{
				if (m_pClientManager->GetSocket(nLoop) != INVALID_SOCKET &&
					m_pClientManager->GetSvrType((DWORD) nLoop) == net::SERVER_AGENT)
				{
					//
					//mjeon
					//Get server informations from AgentServer and save them into DB.
					//
                    if (m_pAdoManager)
                    {
                        m_pAdoManager->AddGameJob(
                            db::DbActionPtr(
                                new db::LogServerState(
						            m_pClientManager->GetSvrGrp((DWORD) nLoop),
						            m_pClientManager->GetSvrNum((DWORD) nLoop),
						            m_pClientManager->GetSvrCrtClient((DWORD) nLoop),
						            m_pClientManager->GetSvrMaxClient((DWORD) nLoop))));
                    }
					//
					//mjeon.ToDo
					//Current DB architecture(flow of DB-process) is too complecated.
					//(interworking of CDBExecter, CDBAction, COdbcManager, COdbcSupervisor)
					//This should be modeled if it's possible.
					//
					
				}
			}
			m_pClientManager->resetPacketCount();
			m_pClientManager->CleanDummyClientSlot();
		}
		else
		{
			fCount++;

			DWORD dwProfileDumpStart = timeGetTime();
			g_profile_1.DumpOutputToNon();
			DWORD dwProfileDumpEnd = timeGetTime() - dwProfileDumpStart;
			if (dwProfileDumpEnd > 500)
				sc::writeLogInfo( sc::string::format("INFO:PROFILE::DumpOutputToNon() %d msec", dwProfileDumpEnd) );
		}

		PrintSystemInfo();
	}

	// Last db action
	m_pDbManager->AllUserLogout();
	m_pDbManager->ExecuteLastJob();
	sc::writeLogInfo(std::string("= Last Database Job Executed"));
	TRACE("CSessionServer::UpdateProc() End \n");
	m_bUpdateEnd = true;
	return 0;
}

int CSessionServer::WorkProc()
{
	PER_IO_OPERATION_DATA* pPerIoData = NULL;
	DWORD dwSndBytes  = 0;
	DWORD dwRcvBytes  = 0;
	DWORD dwByteTrans = 0;
	DWORD dwFlag      = 0;
	DWORD dwClient    = -1;
	int   nRetCode    = 0;
	int   nLastError  = 0;	
	DWORD dwMaxClient = m_pClientManager->GetMaxClient();
	NET_MSG_GENERIC* pNmg = NULL;

	while ( m_bIsRunning )
	{	
		// Wait for I/O to complete on any socket
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
		
        // Illegal Message Skip
		if ( dwClient >= dwMaxClient )
		{
			m_pClientManager->ReleaseOperationData( pPerIoData );
			continue;
		}		
		
		// release reference
		m_pClientManager->ReleaseReference( dwClient );

		if ( nRetCode == 0 ) // ��������
		{
			CloseClient( dwClient );
			m_pClientManager->ReleaseOperationData( pPerIoData );
			continue;
		}

		if ( nRetCode != 0 && pPerIoData == NULL )
		{
			CloseClient( dwClient );
			continue;
		}		
		
		if ( dwByteTrans == 0 ) // ��������
		{
			CloseClient( dwClient );
			m_pClientManager->ReleaseOperationData( pPerIoData );
			continue;
		}

		switch ( pPerIoData->OperationType )
		{
		// �б� �Ϸ� �뺸�϶�...
		case NET_RECV_POSTED :
			{
				m_pClientManager->addRcvMsg( dwClient, pPerIoData->Buffer, dwByteTrans );
				while ( true )
                {
					// �� ���� �޽����� �ִ��� Ȯ���Ѵ�.
					pNmg = ( NET_MSG_GENERIC* ) m_pClientManager->getRcvMsg( dwClient );
					if ( !pNmg )
                    {
						break;
					}
					else
                    {
						// �� ���� �޽����� �޽��� queue �� �ִ´�.
						m_pRecvMsgManager->MsgQueueInsert( dwClient, pNmg, pNmg->Size() );
					}
				}
				
                m_pClientManager->ReleaseOperationData(pPerIoData);

				dwRcvBytes = sizeof( NET_MSG_GENERIC );

				pPerIoData = ( PER_IO_OPERATION_DATA* ) GetFreeOverIO( NET_RECV_POSTED );
				if ( !pPerIoData )
                    continue;
				
				// add reference
				m_pClientManager->AddReference( dwClient, false );

				// ���Ӱ� �ޱ� ��û�� �Ѵ�.
				// WSARecv ��û
				nRetCode = ::WSARecv(
								m_pClientManager->GetSocket( dwClient ),
					            &(pPerIoData->DataBuf), 
					            1, 
					            &dwRcvBytes,
					            &dwFlag,
					            &( pPerIoData->Overlapped ),
					            NULL );
				
				if ( ( nRetCode == SOCKET_ERROR ) && ( ( nLastError = WSAGetLastError() ) != WSA_IO_PENDING ) )
				{
					// release reference
					m_pClientManager->ReleaseReference( dwClient );
					m_pClientManager->ReleaseOperationData( pPerIoData );
					CloseClient( dwClient );
				}
			}
			break;
		// ���� �Ϸ� �뺸 �϶�...
		case NET_SEND_POSTED :
			dwSndBytes = dwByteTrans + pPerIoData->dwSndBytes; // �� ���� ����Ʈ��				
			if ( dwSndBytes >= pPerIoData->dwTotalBytes ) // ���� �Ϸ��
			{
				m_pClientManager->ReleaseOperationData( pPerIoData );
			}
			else // ���� �Ϸ���� ���� ���� ����Ʈ�� ����
			{
				pPerIoData->dwSndBytes  = dwSndBytes; // ���� ����Ʈ�� ������Ʈ				
				pPerIoData->DataBuf.buf = pPerIoData->Buffer + dwSndBytes; // �������� ����Ÿ ������ ������Ʈ				
				dwSndBytes              = pPerIoData->dwTotalBytes - dwSndBytes; // �������� ����Ʈ�� ������Ʈ				
				pPerIoData->DataBuf.len = ( u_long )dwSndBytes; // ���۱��� ������Ʈ

				// add reference
				m_pClientManager->AddReference( dwClient, false );

				if ( SOCKET_ERROR == ::WSASend(
					m_pClientManager->GetSocket( dwClient ),
					&( pPerIoData->DataBuf ),
					1,
					&dwSndBytes,
					dwFlag,
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
		default:
			m_pClientManager->ReleaseOperationData( pPerIoData );
			CloseClient( dwClient );
			break;
		}
		if ( !m_bUseEventThread )
            Sleep(0);
	}

	// ServerWorkerThread Running End
	TRACE( "CSessionServer::WorkProc() End \n" );
	return 0;
}

int CSessionServer::RegularScheduleProc()
{
	// ���������� ó���ؾ��� �۾��� �ִٸ� ���⼭ ó���Ѵ�

	return 0;
}