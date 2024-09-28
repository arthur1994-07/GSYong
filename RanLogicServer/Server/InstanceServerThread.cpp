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
		// Ŭ���̾�Ʈ�� ip, port, ���ӽð��� �����.
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

		// ���Ǽ����� ���� �����ڼ� ����
		SessionSndSvrCurState();
		

		// Start processing I/O on ther accepted socket
		// First WSARecv, TCP/IP Send 8 bytes (ignored byte)
		// Client ���� ������ �޴´�.
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
	//��Ȯ����	GetTickCount < timeGetTime < QueryPerformanceCounter
	//�ӵ���	QueryPerformanceCounter < timeGetTime < GetTickCount
	//sc::DXUtil_Timer uses QueryPerformanceCounter inside it.
	//

	m_dwFrameCount		= 0;

	float fOldAppTime = 0, fAppTime = 0, fElapsTime = 0;
	fOldAppTime = sc::DXUtil_Timer ( sc::TIMER_GETAPPTIME );

	// Event�� ����� ��쿡 ���Ǵ� ���� ����
	DWORD dwNewWaitTime = 0;
	DWORD dwReturn = 0;
	DWORD dwNewTime = 0;
	DWORD dwUpdateFrameTime = 0;

	// Sleep�� ����� ��쿡 ���Ǵ� ����
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
			// ���� �ð��� ����
			fCurrentTime = sc::DXUtil_Timer( sc::TIMER_GETAPPTIME );


			// ���� ������ �� ������ ����
			fTimeDelta = fBeforeTime + m_pGaeaServer->GetLimitFPS();

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

		///////////////////////////////////////////////////////////////////////
		// ���� Ŭ���̾�Ʈ ID ����.
		m_pClientManager->ResetPreSleepCID ();


		///////////////////////////////////////////////////////////////////////
		// ���� �޽������� ó���Ѵ�.
		
		RecvMsgProcess();
		

		// Update Gaea Server
		if(m_pClientManager->IsMatchConnected())	//Match�� ���ӵǾ� ���� ���� ���¿��� FrameMove�� ���鼭 SendMatch�� ȣ���ϴ� �κ��� �ִ�. (DropCrow ����)
		{
			HRESULT hr = DxServerInstance::FieldFrameMove(m_pGaeaServer);
			if (hr != S_FALSE)
				m_dwFrameCount++;
		}
		
		m_pClientManager->SendClientFinal();

        //  ����� Client�� �����Ѵ�.
        CloseClientFinal();

		///////////////////////////////////////////////////////////////////////
		// ��Ʈ��Ʈ üũ�� �Ѵ�.
		FieldSrvHeartBeatCheck(FPS_UPDATE_TIME);

		///////////////////////////////////////////////////////////////////////
		// ���� ������ ���¸� ����Ʈ �Ѵ�.
		PrintDebugMsg(FPS_UPDATE_TIME);

		fAppTime    = sc::DXUtil_Timer ( sc::TIMER_GETAPPTIME ); //	���� �ð�.
		fElapsTime  = fAppTime - fOldAppTime;			 //	�޽��� ó�� �ִ� ���� �ð�.
		fOldAppTime = fAppTime;							 //	�����ð� ����.

		
		// ���� ������ �ý��� WorkingSet size�� ����Ѵ�.
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
	// UpdateThread ����
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
		
		// ������ �����ϱ� ���ؼ� ����ó�� ��ƾ�� ȣ��������...
		if (dwClient == m_dwCompKey && pPerIoData == NULL && dwByteTrans == 0)
		{				
			break;
		}
		
		// Illegal Message Skip, �߸��� �޽���
		if (dwClient >= dwMaxClient)
		{				
			sc::writeLogError(sc::string::format("WorkProc case 1 %d",dwClient));
			//ReleaseOperationData(pPerIoData);
            m_pClientManager->ReleaseOperationData(pPerIoData);
			continue;
		}
		
		if (nRetCode == 0) // ��������
		{
			// 64 �� �����̸� ������ ����
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


		if (dwByteTrans == 0) // ��������
		{
			//releaseIO( dwClient, pPerIoData );
			CloseClient( dwClient );
			//ReleaseOperationData(pPerIoData);
            m_pClientManager->ReleaseOperationData(pPerIoData);
			continue;
		}

		switch (pPerIoData->OperationType)
		{
		// �б� �Ϸ� �뺸�϶�...
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
				
				// WSARecv ��û
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
		// ���� �Ϸ� �뺸 �϶�...
		case NET_SEND_POSTED :
			dwSndBytes = dwByteTrans + pPerIoData->dwSndBytes; // �� ���� ����Ʈ��				
			if (dwSndBytes >= pPerIoData->dwTotalBytes) // ���� �Ϸ��
			{
                m_pClientManager->ReleaseOperationData(pPerIoData);
			}
			else // ���� �Ϸ���� ���� ���� ����Ʈ�� ����
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
	// ���������� ó���ؾ��� �۾��� �ִٸ� ���⼭ ó���Ѵ�

	return 0;
}