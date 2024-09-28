#include "pch.h"
#include "../../SigmaCore/Util/EBTime.h"
#include "../Util/IpAddressChecker.h"
#include "../FieldServer/GLGaeaServer.h"
#include "../Client/NetInstanceClientMan.h"
#include "./InstanceServer.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


int InstanceServer::ListenProc(void)
{
	PER_IO_OPERATION_DATA* IOData = NULL;
	HANDLE	hRetCode       = NULL;
	DWORD	dwRecvNumBytes = 0;
	DWORD	Flags          = 0;
	int		nClientNumber  = 0;
	DWORD   dwClient       = 0;
	int		nRetCode       = 0;
	SOCKET	Accept;

	while (m_bIsRunning)
	{
		Accept = ::WSAAccept(m_sServer, NULL, NULL, NULL, 0);
		if (Accept == INVALID_SOCKET)
		{
			nRetCode = ::WSAGetLastError();							
			sc::writeLogError(sc::string::format("WSAAccept %1%", nRetCode));
			if (nRetCode == WSAENOTSOCK || nRetCode == WSAEINTR)
			{
				break;
			}
			else
			{
				continue;
			}
		}

        // IP address check ---------------------------------------------------
        sockaddr_in	sAddrIn;
        int nSize = sizeof(sockaddr_in);
        char szIp[MAX_IP_LENGTH+1] = {0};

        // Get client ip address and port
        ::getpeername(Accept, (sockaddr *) &sAddrIn, &nSize);
        ::StringCchCopy(szIp, MAX_IP_LENGTH+1, ::inet_ntoa(sAddrIn.sin_addr));

        if (!m_pAddressChecker->IsAllowIp(szIp))
        {
            sc::writeLogError(
                sc::string::format(
                    "Block ip %1%", szIp));
            ::closesocket(Accept);
            continue;
        }

		///////////////////////////////////////////////////////////////////////
		// Create per-handle data information structure to associate with the socket
		nClientNumber = m_pClientManager->GetFreeClientID(); // Get free client slot number
		if (nClientNumber == NET_ERROR) 
		{
			/*
			LINGER      lingerStruct;     
			lingerStruct.l_onoff  = 1;  
			lingerStruct.l_linger = 5;
			nRetCode = ::setsockopt(Accept, 
									SOL_SOCKET, 
									SO_LINGER, 
									(char *) &lingerStruct, 
									sizeof(lingerStruct));
			*/
			sc::writeLogInfo(std::string("Reached Max Client Number"));
			//::shutdown(Accept, SB_BOTH);
			::closesocket(Accept);
			continue;
		}
		
		dwClient = (DWORD) nClientNumber;

		// Set client information
		// 클라이언트의 ip, port, 접속시간을 기록함.
		if ( m_pClientManager->SetAcceptedClient(dwClient, Accept) == NET_ERROR )
		{	
			m_pClientManager->ReleaseClientID( dwClient );
			continue;
		}

		// Associate the accepted socket with the completion port
		hRetCode = ::CreateIoCompletionPort(
						(HANDLE) Accept, 
						m_hIOServer, 
						(DWORD) nClientNumber, 
						0 );

		if (hRetCode == NULL) 
		{
			sc::writeLogError(std::string("CreateIoCompletionPort Error"));
			CloseClient(dwClient);
			continue;
		}

		// 세션서버에 현재 접속자수 전송
		SessionSndSvrCurState();
		

		// Start processing I/O on ther accepted socket
		// First WSARecv, TCP/IP Send 8 bytes (ignored byte)
		// Client 버전 정보를 받는다.
		dwRecvNumBytes = sizeof( NET_MSG_GENERIC );        
		
		IOData = (PER_IO_OPERATION_DATA*) GetFreeOverIO(NET_RECV_POSTED);

		if (!IOData)
		{
			CloseClient( (DWORD) nClientNumber );
			continue;
		}

		IOData->dwRcvBytes		= 0;
		IOData->dwTotalBytes	= dwRecvNumBytes;
        
		::WSARecv(
			Accept,
			&(IOData->DataBuf), 
			1,
			&dwRecvNumBytes,
			&Flags ,
			&(IOData->Overlapped),
			NULL );

        m_pAddressChecker->AddConnection(szIp);

		if (!m_bUseEventThread)
            Sleep(0);
	}
	// ServerAcceptThread Running End
	sc::writeLogInfo(std::string("ListenProc End"));

	return 0;
}

int InstanceServer::UpdateProc()
{
	DWORD dwMaxClient       = 0;

	//mjeon
	//timeGetTime() returns the system time is the time elapsed since Windows was started.
	//
	DWORD dwTimeTemp		= timeGetTime(); 
	DWORD dwTimeS           = timeGetTime();
	//DWORD dwFPSUdateTime		= /*3000*/FPS_UPDATE_TIME;

	m_dwHeartBeatStart1 = dwTimeS;
	m_dwHeartBeatStart2 = dwTimeS;
	m_dwHeartBeatTime   = dwTimeS;
	m_dwFrameTime		= dwTimeS;
	m_bUpdateEnd		= false;

	//
	//mjeon
	//정확도는	GetTickCount < timeGetTime < QueryPerformanceCounter
	//속도는	QueryPerformanceCounter < timeGetTime < GetTickCount
	//sc::DXUtil_Timer uses QueryPerformanceCounter inside it.
	//

	m_dwFrameCount		= 0;

	float fOldAppTime = 0, fAppTime = 0, fElapsTime = 0;
	fOldAppTime = sc::DXUtil_Timer ( sc::TIMER_GETAPPTIME );

	// Event를 사용할 경우에 사용되는 위한 변수
	DWORD dwNewWaitTime = 0;
	DWORD dwReturn = 0;
	DWORD dwNewTime = 0;
	DWORD dwUpdateFrameTime = 0;

	// Sleep을 사용할 경우에 사용되는 변수
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

			//
			//mjeon
			//m_dwUpdateWaitTime never chages. It is always 10 here. 
			//
			//If UpdateFrameDuration is more than 10ms then do not wait for UpdateQuitEvent.
			//When UpdateFrameDuration is less than 10ms(i.e.when UpdateProc is not busy),
			//wait for UpdateQuitEvent for (10ms - current UpdateFrameDuration).
			//
			int nUpdateElspTime = (int) (m_dwUpdateWaitTime - dwUpdateFrameTime );
			if ( 0 >= nUpdateElspTime ) 
			{
				dwNewWaitTime = 0;
			}
			else
			{
				dwNewWaitTime = nUpdateElspTime;
			}

			//
			//mjeon
			//m_hUpdateQuitEvent is set only by InstanceServer::Stop() method when the server stops.
			//
			dwReturn = ::WaitForSingleObject( m_hUpdateQuitEvent, dwNewWaitTime );

			if ( WAIT_OBJECT_0 == dwReturn )		//signaled
			{
				break;
			}
            else if ( WAIT_TIMEOUT != dwReturn )	//Not WAIT_TIMEOUT
			{
				continue;
			}
		}
		else
		{
			// 현재 시간을 얻어옴
			fCurrentTime = sc::DXUtil_Timer( sc::TIMER_GETAPPTIME );


			// 다음 랜더링 될 시점을 구함
			fTimeDelta = fBeforeTime + m_pGaeaServer->GetLimitFPS();

			// 만약 현재가 첫 프레임이나 timeGetTime()이 랜더링될 시점의 시간보다 크면 랜더링
			if( fBeforeTime == 0.0f || fCurrentTime >= fTimeDelta )	
			{
				// 메인루프 처리
				// 현재 시간을 g_BeforeTime에 넣음
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

		///////////////////////////////////////////////////////////////////////
		// 유휴 클라이언트 ID 정리.
		m_pClientManager->ResetPreSleepCID ();


		///////////////////////////////////////////////////////////////////////
		// 받은 메시지들을 처리한다.
		
		RecvMsgProcess();
		

		// Update Gaea Server
		if(m_pClientManager->IsMatchConnected())	//Match가 접속되어 있지 않은 상태에서 FrameMove를 돌면서 SendMatch를 호출하는 부분이 있다. (DropCrow 관련)
		{
			HRESULT hr = DxServerInstance::FieldFrameMove(m_pGaeaServer);
			if (hr != S_FALSE)
				m_dwFrameCount++;
		}
		
		m_pClientManager->SendClientFinal();

        //  예약된 Client를 종료한다.
        CloseClientFinal();

		///////////////////////////////////////////////////////////////////////
		// 하트비트 체크를 한다.
		FieldSrvHeartBeatCheck(FPS_UPDATE_TIME);

		///////////////////////////////////////////////////////////////////////
		// 현재 서버의 상태를 프린트 한다.
		PrintDebugMsg(FPS_UPDATE_TIME);

		fAppTime    = sc::DXUtil_Timer ( sc::TIMER_GETAPPTIME ); //	현재 시간.
		fElapsTime  = fAppTime - fOldAppTime;			 //	메시지 처리 최대 지연 시간.
		fOldAppTime = fAppTime;							 //	이전시간 저장.

		
		// 현재 서버의 시스템 WorkingSet size를 출력한다.
		PrintSystemInfo();


		if ( m_bReservationServerStop )
		{
			m_fReservationtime += fElapsTime;

			if ( m_fReservationtime > fRESERVATIONTIME )
			{
				PostMessage( m_hWnd, WM_FINALCLEANUPFINISH, 0, 0 );
				m_bReservationServerStop = false;
			}
		}
	}

	///////////////////////////////////////////////////////////////////////
	// UpdateThread 종료
	EndUpdateThread();

	return 0;
}

int InstanceServer::WorkProc()
{
	TRACE(_T("in InstanceThread.cpp  InstanceServer::WorkProc\n"));

	PER_IO_OPERATION_DATA* pPerIoData = NULL;
	DWORD dwSndBytes                  = 0;
	DWORD dwRcvBytes                  = 0;
	DWORD dwByteTrans                 = 0;
	DWORD dwFlags                     = 0;
	DWORD dwClient                    = -1;
	int	  nRetCode                    = 0;
	DWORD nLastError                  = 0;
	
	DWORD dwMaxClient = m_pClientManager->GetMaxClient();
	NET_MSG_GENERIC* pNmg = NULL;

	while (m_bIsRunning)
	{
		// Wait for I/O to complete on any socket
		nRetCode = GetQueuedCompletionStatus(m_hIOServer,
											 &dwByteTrans,
											 (LPDWORD) &dwClient,
											 (LPOVERLAPPED *) &pPerIoData,
											 INFINITE);
		
		// 서버를 종료하기 위해서 종료처리 루틴을 호출했을때...
		if (dwClient == m_dwCompKey && pPerIoData == NULL && dwByteTrans == 0)
		{				
			break;
		}
		
		// Illegal Message Skip, 잘못된 메시지
		if (dwClient >= dwMaxClient)
		{				
			sc::writeLogError(sc::string::format("WorkProc case 1 %d",dwClient));
			//ReleaseOperationData(pPerIoData);
            m_pClientManager->ReleaseOperationData(pPerIoData);
			continue;
		}
		
		if (nRetCode == 0) // 강제종료
		{
			// 64 번 에러이면 무조건 종료
			nLastError = GetLastError();
			
			CloseClient( dwClient );
			
            m_pClientManager->ReleaseOperationData(pPerIoData);
			continue;
		}

		if (nRetCode != 0 && pPerIoData == NULL)
		{
			CloseClient(dwClient);
			continue;
		}


		if (dwByteTrans == 0) // 정상종료
		{
			//releaseIO( dwClient, pPerIoData );
			CloseClient( dwClient );
			//ReleaseOperationData(pPerIoData);
            m_pClientManager->ReleaseOperationData(pPerIoData);
			continue;
		}

		switch (pPerIoData->OperationType)
		{
		// 읽기 완료 통보일때...
		case NET_RECV_POSTED :
			{
				// Insert to client recv buffer
				m_pClientManager->addRcvMsg(dwClient, pPerIoData->Buffer, dwByteTrans);
				 
				while (true)
                {
					pNmg = (NET_MSG_GENERIC*) m_pClientManager->getRcvMsg(dwClient);
					if (!pNmg || 0 == dwClient)
						break;
                    else
						m_pRecvMsgManager->MsgQueueInsert(dwClient, pNmg, pNmg->Size());
				}

                m_pClientManager->ReleaseOperationData(pPerIoData);
				
				dwRcvBytes = sizeof(NET_MSG_GENERIC);

				pPerIoData = (PER_IO_OPERATION_DATA*) GetFreeOverIO(NET_RECV_POSTED);

				if (pPerIoData == NULL) continue;
				
				// WSARecv 요청
				nRetCode = ::WSARecv(
								m_pClientManager->GetSocket(dwClient),
								&(pPerIoData->DataBuf), 
								1, 
								&dwRcvBytes,
								&dwFlags,
								&(pPerIoData->Overlapped),
								NULL );
				
				if ((nRetCode == SOCKET_ERROR) && ((nLastError=WSAGetLastError()) != WSA_IO_PENDING))
				{
                    m_pClientManager->ReleaseOperationData(pPerIoData);
					CloseClient(dwClient);
					sc::writeLogError(
                        sc::string::format(
                            "case 8 WorkProc WSARecv Client %1% ERR %2%", dwClient, nLastError));
				}
			}
			break;
		// 쓰기 완료 통보 일때...
		case NET_SEND_POSTED :
			dwSndBytes = dwByteTrans + pPerIoData->dwSndBytes; // 총 보낸 바이트수				
			if (dwSndBytes >= pPerIoData->dwTotalBytes) // 쓰기 완료됨
			{
                m_pClientManager->ReleaseOperationData(pPerIoData);
			}
			else // 쓰기 완료되지 않음 남은 바이트를 전송
			{
                m_pClientManager->ReleaseOperationData(pPerIoData);
                CloseClient(dwClient);
				sc::writeLogError(std::string("WorkProc can't send message. slot %1%", dwClient));
			}
			break;
		default :
			CloseClient(dwClient);			
			sc::writeLogError(sc::string::format("WorkProc case 10 %1%", dwClient));
			break;
		}	
		if( !m_bUseEventThread ) Sleep( 0 );
	}

	// ServerWorkerThread Running End
	sc::writeLogInfo(std::string("WorkProc End"));

	return 0;
}

int InstanceServer::RegularScheduleProc()
{
	// 정기적으로 처리해야할 작업이 있다면 여기서 처리한다

	return 0;
}