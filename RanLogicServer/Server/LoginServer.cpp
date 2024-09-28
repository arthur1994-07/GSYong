//! Copyright (c) Min Communications. All rights reserved.
#include "../pch.h"
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>


#include "../../SigmaCore/Util/EBTime.h"
#include "../../SigmaCore/Math/SeqUniqueGuid.h"

#include "../../EngineLib/Common/profile.h"

#include "../../RanLogic/Util/s_CRandomNumber.h"

#include "../Client/NetLoginClientMan.h"
#include "../../RanLogic/Util/GlobalAuthManager.h"
#include "../Database/Odbc/s_COdbcManager.h"
#include "../Util/IpAddressChecker.h"
#include "./LoginServerConfigXml.h"
#include "./LoginServer.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CLoginServer::CLoginServer(HWND hWnd, HWND hEditBox, HWND hEditBoxInfo, SUBPATH* pPath, EMSERVICE_PROVIDER ServiceProvider)
	: NetServer(hWnd, hEditBox, hEditBoxInfo, pPath, ServiceProvider)
	, m_pClientManager(NULL)
	, m_dwLastGameSvrUpdateTime(0)
	, m_bInspecting(false)
    , m_SessionServerSlot(net::INVALID_SLOT)
	, m_pGeoIP(NULL)
{
	MessageEnumMixer::GetInstance();

	sc::LogMan::getInstance()->attachEditControl(hEditBox, hEditBoxInfo);
	initPacketFunc();
	for (int i=0; i<MAX_SERVER_GROUP; i++)
	{
		for (int j=0; j<MAX_SERVER_NUMBER; j++)
		{
			::SecureZeroMemory(&m_sGame[i][j], sizeof(G_SERVER_CUR_INFO_LOGIN));
		}
	}
    m_pConfig = new LoginServerConfigXml;
	m_pAuthManager = new sc::CGlobalAuthClientLogic;
	m_setCountryAccessApprove.clear();
}

CLoginServer::~CLoginServer()
{    
	SAFE_DELETE(m_pAuthManager);

	delete m_pClientManager;
    m_pClientManager = NULL;
    
    delete m_pConfig;
    m_pConfig = NULL;
}

// Ŭ���̾�Ʈ �������� Ŭ���� ����
int CLoginServer::StartClientManager()
{	
	if (!m_pClientManager)
	{		
		m_pClientManager = new NetLoginClientMan(m_nMaxClient, m_hIOServer, m_pConfig->GetReferenceCountMax());
	}

	if (!m_pClientManager) 
	{
		m_bIsRunning = false;
		return NET_ERROR;
	}
	else
	{
		return NET_OK;
	}
}

int CLoginServer::Start()
{
	int		nRetCode;	
	
	m_bIsRunning = true;	
	
    const std::string XmlFileName("LoginServer.xml");    
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

	if (StartMsgManager(m_nMaxClient) == NET_ERROR)
        return NET_ERROR;

	if (StartIOCP()	== NET_ERROR)
        return NET_ERROR;

	if (StartClientManager() == NET_ERROR)
        return NET_ERROR;	

	if (StartWorkThread(m_pConfig) == NET_ERROR)
        return NET_ERROR;

	if (StartUpdateThread() == NET_ERROR)
        return NET_ERROR;	

	if (StartListenThread() == NET_ERROR)
        return NET_ERROR;	 

	// ����Ű ����
	GenerateEncrypt();

	// Connect to Session Server
	nRetCode = SessionConnect(m_pConfig->GetSessionServerIP(), m_pConfig->GetSessionServerPort());
	if (nRetCode != NET_OK)
	{		
		m_bIsRunning = false;
		return NET_ERROR;
	}

	// GeoIP
	// ���� GeoIP.dat �����ε� �̸����� �ش� ����� ������ �� �ֱ⶧���� ���ϸ��� ������
	std::string FilePath(sc::getAppPath());
	FilePath.append("\\cVerGeo.dat");

	m_pGeoIP = GeoIP_open( FilePath.c_str(), GEOIP_STANDARD );

	if( m_pGeoIP == NULL)
	{
		sc::writeLogError("ERROR! GeoIP Init Fail!");
		return NET_ERROR;
	}

//#ifdef _USE_AUTH
//	//if (m_ServiceProvider == SP_EU)
//	//{		
//		// ���Ǽ����� �ش� ������ ������ ���� ������ ����ó���� �����ؾ���.
//		if (SendAuthInfoToSessionServer() == NET_ERROR)
//		{
//			m_bIsRunning = false;
//			return NET_ERROR;
//		}
//	//}
//#endif

	// ���Ӽ����� ������ ��û�Ѵ�.
	SessionReqSvrInfo(); 
	
	m_dwLastGameSvrUpdateTime = timeGetTime();

	sc::writeLogInfo(
		sc::string::format(
		"FD_SETSIZE %1%",
		FD_SETSIZE));

	sc::writeLogInfo(std::string(">Server Start OK"));
	sc::writeLogInfo(std::string("======================================================="));
	
	m_nStatus = S_SERVER_RUNING;

	// SERVER_UTIL::CAddressChecker::GetInstance()->AddKnownAddress(m_pConfig->GetServerIP());
	// SERVER_UTIL::CAddressChecker::GetInstance()->AddKnownAddress(m_pConfig->GetSessionServerIP());
    m_pAddressChecker->SetConnectionCountAllow(m_pConfig->GetConnectionPerIpAddress());
    m_pAddressChecker->AddAllowIp(m_pConfig->GetServerIP());
    m_pAddressChecker->AddAllowIp(m_pConfig->GetSessionServerIP());



	return NET_OK;
}

int CLoginServer::Stop()
{
	sc::writeLogInfo(std::string("Please wait until server stop "));

	// Stop All Thread and exit
	DWORD dwExitCode = 0;	

	//////////////////////////////////////////////////////////////////////////////
	// Disconnect Session Server
	// SessionCloseConnect();

	//EnterCriticalSection(&m_CriticalSection);
	lockOn();
	m_bIsRunning = false;
	lockOff();
	//LeaveCriticalSection(&m_CriticalSection);

	if( m_bUseEventThread )	
	{
		::SetEvent( m_hUpdateQuitEvent );

		::WaitForSingleObject( m_hUpdateThread, INFINITE );
		SAFE_CLOSE_HANDLE( m_hUpdateQuitEvent );
	}else{
		Sleep( 1000 );
	}

	StopListenThread();	
	StopWorkThread();
	StopIOCP();

	CloseAllClient(); 	// close all client connections
	
	// Message Queue ����	
	SAFE_DELETE(m_pRecvMsgManager);
	sc::writeLogInfo(std::string("Stop Meessage Queue"));
	
	// DB ��������
    SAFE_DELETE(m_pDbManager);

	sc::writeLogInfo(std::string("Stop DataBase"));
	// CFG class ����
	//m_pConfig->ReleaseInstance();
	sc::writeLogInfo(std::string("Stop CFG"));
	
	// Put message to console
	sc::writeLogInfo(std::string("Server Stop OK"));
	sc::writeLogInfo(std::string("======================================================="));
	
	m_nStatus = S_SERVER_STOP;

	return NET_OK;
}

void CLoginServer::ForceStop( DWORD Reason )
{
	sc::writeLogInfo( std::string( "== Please wait until server stop(Force)" ) );
}

/*
int	CLoginServer::Pause()
{
	::closesocket(m_sServer);
	m_sServer = INVALID_SOCKET;
	CloseAllClient();
	m_nStatus = S_SERVER_PAUSE;
	// Put message to console
	sc::writeLogInfo(std::string("======================================================="));
	sc::writeLogInfo(std::string("Server Pause OK"));
	sc::writeLogInfo(std::string("======================================================="));

	return NET_OK;	
}

int CLoginServer::Resume()
{
	if (StartListenThread() == NET_OK)
	{
		m_nStatus = S_SERVER_RUNING;
		// Put message to console
		sc::writeLogInfo(std::string("======================================================="));
		sc::writeLogInfo(std::string("Server Resume OK"));
		sc::writeLogInfo(std::string("======================================================="));

		return NET_OK;
	}
	else
	{
		// Put message to console
		sc::writeLogError(std::string("======================================================="));
		sc::writeLogError(std::string("Server Resume Failed"));
		sc::writeLogError(std::string("======================================================="));

		return NET_ERROR;
	}
}

int	CLoginServer::ReStart()
{
	Stop();
	Start();
	return NET_OK;
}
*/

void CLoginServer::CloseClient( DWORD dwClient )
{	
	lockOn();
	if ( net::SLOT_SESSION == m_pClientManager->GetSlotType( dwClient ) )
	{
		// Session ������ �������
		m_pClientManager->CloseClient( dwClient );
		sc::writeLogError( sc::string::format( "Session server connection closed. Current Session slot is %1%", m_SessionServerSlot ) );
		if ( m_bIsRunning && INVALID_SOCKET == m_pClientManager->GetSocket( m_SessionServerSlot ) )
		{
			sc::writeLogError( std::string( "Try to connect session server" ) );
			m_SessionServerSlot = net::INVALID_SLOT;
			SessionConnect();
		}
	}
	else
	{
		// ��ϵ� IP ����Ʈ�� �����Ѵ�.
		std::string IpAddress = m_pClientManager->GetClientIP( dwClient );
		m_pAddressChecker->DelConnection( IpAddress );
		m_pClientManager->CloseClient( dwClient );
	}
	lockOff();
}

// close all client connections
void CLoginServer::CloseAllClient()
{	
    sc::writeLogInfo(std::string("CloseAllClient"));
    for (DWORD dwNum=1; dwNum<(DWORD) m_nMaxClient; ++dwNum)
    {
        if ( INVALID_SOCKET != m_pClientManager->GetSocket( dwNum ) && m_pClientManager->IsClientSlot( dwNum ) )
            CloseClient( dwNum );
    }
}

void CLoginServer::GenerateEncrypt()
{
	SERVER_UTIL::CRandomNumber random;
	CString str;
	random.GenerateRandomString(str, ENCRYPT_KEY +1 , 1, 2, 3);
	::StringCchCopy(m_szEncrypt, ENCRYPT_KEY +1 , str);
}

bool CLoginServer::CheckAllowIP( SOCKET socket )
{
	int	nSize = 0;
	sockaddr_in	sAddrIn;   

	::SecureZeroMemory(&sAddrIn, sizeof(sockaddr_in));
	nSize = sizeof(sockaddr_in);

	// Get client ip address and port
	::getpeername(socket, (sockaddr *) &sAddrIn, &nSize);

	std::string IpAddress(::inet_ntoa(sAddrIn.sin_addr));
    if (m_pConfig->IsAllowIp(IpAddress))
        return true;
    else
	    return false;
}

void CLoginServer::AddCountryAccessApprove( int nIndex )
{
	int nMaxIndex = sizeof(GeoIP_country_code3)/sizeof(*GeoIP_country_code3);
	
	if( nIndex < 0 && nMaxIndex <= nIndex )
		return;

	std::string strCountry = GeoIP_country_code3[nIndex];
	
	sc::writeLogInfo( sc::string::format("Set [Code]:%s", strCountry.c_str() ) );


	m_setCountryAccessApprove.insert( strCountry );
}

bool CLoginServer::CountryAccessApprove( SOCKET socket )
{
	if( m_pGeoIP && m_setCountryAccessApprove.size() != 0 )
	{

	int	nSize = 0;
	sockaddr_in	sAddrIn;   

	::SecureZeroMemory(&sAddrIn, sizeof(sockaddr_in));
	nSize = sizeof(sockaddr_in);

	// Get client ip address and port
	::getpeername(socket, (sockaddr *) &sAddrIn, &nSize);

	std::string IpAddress(::inet_ntoa(sAddrIn.sin_addr));

		const char* strCountryCode3 = GeoIP_country_code3_by_addr(m_pGeoIP, IpAddress.c_str());

		if( strCountryCode3 != NULL )
		{			
			if ( m_setCountryAccessApprove.find( strCountryCode3 ) != m_setCountryAccessApprove.end() )
			{
				sc::writeLogInfo( sc::string::format("[GeoIP]:Success %s - %s", strCountryCode3, IpAddress.c_str()) );
				return true;
			}
			else
			{
				sc::writeLogInfo( sc::string::format("[GeoIP]:Fail %s - %s", strCountryCode3, IpAddress.c_str()) );
				return false;
			}
		}
		else
		{
			sc::writeLogInfo( sc::string::format("[GeoIP]:Unknown - %s", IpAddress.c_str()) );
			// return false;
		}

	}

	return true;
}

int CLoginServer::ListenProc()
{
	PER_IO_OPERATION_DATA* pIOData = NULL;
	SOCKET Accept;
	DWORD  dwRecvNumBytes = 0;
	DWORD  dwFlags        = 0;
	HANDLE hRetCode       = NULL;
	int    nClientNumber  = 0;
	DWORD  dwClient       = 0;
	int	   nRetCode       = 0;

	while (m_bIsRunning)
	{
		Accept = ::WSAAccept(m_sServer, NULL, NULL, NULL, 0);
		if (Accept == INVALID_SOCKET) 
		{
			nRetCode = ::WSAGetLastError();
			sc::writeLogError(sc::string::format("WSAAccept %1%", nRetCode));
			if (nRetCode == WSAENOTSOCK || nRetCode == WSAEINTR)
				break;
			else
				continue;
		}

        // --------------------------------------------------------------------
        // Get client ip address and port, check ip address
        sockaddr_in	sAddrIn;
        int nSize = sizeof(sockaddr_in);
        char szIp[MAX_IP_LENGTH+1] = {0};
        ::getpeername(Accept, (sockaddr*) &sAddrIn, &nSize);
        ::StringCchCopy(szIp, MAX_IP_LENGTH+1, ::inet_ntoa(sAddrIn.sin_addr));
        if (!m_pAddressChecker->IsAllowIp(szIp))
        {
            sc::writeLogError(
                sc::string::format(
                    "Block ip %1%", szIp));
            closesocket(Accept);
            continue;
        }

		///////////////////////////////////////////////////////////////////////
		// Create per-handle data information structure to associate with the socket
		nClientNumber = m_pClientManager->GetFreeClientID(false); // Get free client slot number
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

			::closesocket(Accept);
			sc::writeLogError(std::string("Reached Max Client Number, Not available client slot"));
			
			//
			//mjeon
			//In this case, login-server should clear out all connections that are in the middle of login process.
			//
			CloseAllClient();	

			continue;
		}

		if( m_pClientManager->GetCurrentClientNumber() % 100 == 0  )
		{	
			sc::writeLogError(
                sc::string::format(
                    "Current Connect Client Number %1%",
                    m_pClientManager->GetCurrentClientNumber()));
		}

		dwClient = (DWORD) nClientNumber;
		
		// Set client information
		if (m_pClientManager->SetAcceptedClient(dwClient, Accept) == NET_ERROR)
		{
			::closesocket( Accept );
			m_pClientManager->ReleaseClientID(dwClient);
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

		// IP �� ����Ʈ�� ����Ѵ�.
		// SERVER_UTIL::CAddressChecker::GetInstance()->IsBlock(m_pClientManager->GetClientIP(dwClient));

		// Associate the accepted socket with the completion port
		hRetCode = ::CreateIoCompletionPort(
            (HANDLE) Accept, 
            m_hIOServer, 
            dwClient, 
            0);

		if (!hRetCode)
		{
			sc::writeLogError(std::string("CreateIoCompletionPort Error"));
			CloseClient(dwClient);
			continue;
		}

		// Start processing I/O on ther accepted socket
		// First WSARecv, TCP/IP Send 8 bytes (ignored byte)
		// Client ���� ������ �޴´�.
		dwRecvNumBytes = sizeof(NET_MSG_GENERIC);
        pIOData = (PER_IO_OPERATION_DATA*) GetFreeOverIO(NET_RECV_POSTED);
		//pIOData = getRecvIO( nClientNumber );

		if (!pIOData)
		{
			CloseClient(dwClient);
			continue;
		}
		
		pIOData->dwRcvBytes   = 0;
		pIOData->dwTotalBytes = dwRecvNumBytes;
		// Head receive mode
		//m_pClientManager->SetNetMode(nClientNumber, NET_PACKET_HEAD);
		
		// add reference
		m_pClientManager->AddReference( dwClient );

		int nRetCode = ::WSARecv(
            Accept,
            &(pIOData->DataBuf), 
            1,
            &dwRecvNumBytes,
            &dwFlags ,
            &(pIOData->Overlapped),
            NULL);
		int nLastErr = 0;
		if ( ( nRetCode == SOCKET_ERROR ) && ( ( nLastErr = WSAGetLastError() ) != WSA_IO_PENDING ) )
		{
			// release reference
			m_pClientManager->ReleaseReference( dwClient );
			m_pClientManager->ReleaseOperationData( pIOData );
			CloseClient( dwClient );

			sc::writeLogError(
				sc::string::format(
				"ListenProc WSARecv Client %1% error %2%", dwClient, nLastErr ) );
			continue;
		}

        m_pClientManager->SetSlotType(dwClient, net::SLOT_CLIENT);
        m_pAddressChecker->AddConnection(szIp);        
        if (!m_bUseEventThread)
            Sleep(0);
	}
	return 0;	
}

int CLoginServer::UpdateProc()
{
	// ������ ������ ���� ������
	DWORD dwTimeS          = timeGetTime();	// The system time is the time elapsed since Windows was started.
	DWORD dwHeartBeatStart1 = dwTimeS; // ���� ���� ���� �ð�����
	DWORD dwHeartBeatStart2 = dwTimeS; // ���� ���� ���� �ð�����
	bool  bHeartBeat	   = false;
	DWORD dwMaxClient      = m_nMaxClient;
	DWORD dwN              = 0;
	DWORD dwTimeTemp       = timeGetTime();

	//
	//mjeon
	//��Ȯ����	GetTickCount < timeGetTime < QueryPerformanceCounter
	//�ӵ���	QueryPerformanceCounter < timeGetTime < GetTickCount
	//sc::DXUtil_Timer uses QueryPerformanceCounter inside it.
	//

	float fFrame = 0;
	float fCount = 0;
	
	MSG_LIST* pMsg = NULL;

	// Event�� ����� ��쿡 ���Ǵ� ���� ����
	DWORD dwNewWaitTime = 0;
	DWORD dwReturn = 0;
	DWORD dwNewTime = 0;
	DWORD dwUpdateFrameTime = 0;
	DWORD dwBeforeNewWaitTime = 0;//������ �����̸� �󸶳� �־��°�?

	// Sleep�� ����� ��쿡 ���Ǵ� ���� ����
	double fBeforeTime = 0.0f;
	double fCurrentTime = 0.0f;
	double fTimeDelta = 0.0f;

	//
	//mjeon
	//m_dwUpdateFrame was set by 100 in CServer::setUpdataFrame().
	//m_dwUpdateWaitTime was set by 10 (1000/m_dwUpdateFrame) in CServer::setUpdateFrame().
	//m_dwUpdateOldTime = ::GetTickCount() in CServer::setUpdateFrame().
	//

	while (m_bIsRunning)
	{
		if (m_bUseEventThread)
		{	
			dwNewTime = ::GetTickCount();	//number of milliseconds that have elapsed since the system was started.
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
			if (WAIT_OBJECT_0 == dwReturn) //signaled
				break;
            else if (WAIT_TIMEOUT != dwReturn)	//Not WAIT_TIMEOUT
				continue;
		}
		else
		{
			// ���� �ð��� ����
			fCurrentTime = sc::DXUtil_Timer(sc::TIMER_GETAPPTIME);

			// ���� ������ �� ������ ����
			fTimeDelta = fBeforeTime+0.001f;	// 1ms

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
		pMsg = m_pRecvMsgManager->GetMsg( NULL );	// Get Message	

		while ( pMsg )
		{
			MsgProcess( pMsg );
			pMsg = m_pRecvMsgManager->GetMsg( pMsg );
		}


		// HeartBeat Check
		dwTimeTemp = timeGetTime();
		
		// Connect to Session Server if it is offline.
		if ((dwTimeTemp - dwHeartBeatStart1) > SESSION_CHECK_TIME)	//5min
		{
            if (m_SessionServerSlot != net::INVALID_SLOT && !m_pClientManager->IsOnline(m_SessionServerSlot))	//SessionServer
			{
				//
				//mjeon
				//If SessionServer is offline,
				//try to connect to SessionServer per every 5 minutes.
				//
                m_SessionServerSlot = net::INVALID_SLOT;
				SessionConnectSndSvrInfo();
			}
			dwHeartBeatStart1 = timeGetTime();
            if (m_pAddressChecker)
                m_pAddressChecker->ClearIpData();
		}

		if ((dwTimeTemp - dwHeartBeatStart2) > HEARTBEAT_TIME) //2min
		{
			//
			//mjeon
			//bHearBeat is automatically toggled per a loop just before this if-condition ends up.
			//
			if (bHeartBeat) // Send Heartbeat Request
			{
				sc::writeInfo(std::string("Send Heartbeat Request"));

				// Check Session Server
				//If SessionServer is online
                if ( m_SessionServerSlot != net::INVALID_SLOT && INVALID_SOCKET != m_pClientManager->GetSocket( m_SessionServerSlot ) )
				{
					//HearBeat will be set in SessionMsgProcess() when it receives HearBeatAns msg.
					m_pClientManager->ResetHeartBeat( m_SessionServerSlot );
#ifdef HEARTBEAT_LOG
					sc::writeLogInfo(std::string("[HEARTBEAT] Send Heartbeat Request to Session Svr"));
#endif

                    NET_HEARTBEAT_SERVER_REQ HeartBeatMsgReqServer;
					SendClient( m_SessionServerSlot, &HeartBeatMsgReqServer );
				}
				else
				{
					sc::writeLogError( std::string( "Session server connection closed" ) );
					sc::writeLogError( std::string( "Try to connect session server" ) );
					SessionConnectSndSvrInfo();
				}
				

				// Check Client
				NET_HEARTBEAT_CLIENT_REQ HeartBeatMsgReq;
				for (DWORD dwN=1; dwN < dwMaxClient; ++dwN)
				{
					if ( INVALID_SOCKET != m_pClientManager->GetSocket( dwN ) && m_pClientManager->IsClientSlot( dwN ) )
					{
						//HearBeat will be set in MsgProcess() when it receives HearBeatAns msg.
						m_pClientManager->ResetHeartBeat( dwN );
						SendClient( dwN, &HeartBeatMsgReq );
					}
				}
			}
			else // Check Heartbeat
			{
				sc::writeInfo(std::string("Check Heartbeat"));
				
				// Check Sessein Server - Check HeartBeat
                if ( m_SessionServerSlot != net::INVALID_SLOT && 
					INVALID_SOCKET != m_pClientManager->GetSocket( m_SessionServerSlot ) && 
					!m_pClientManager->CheckHeartBeat( m_SessionServerSlot ) )
				{
					CloseClient( m_SessionServerSlot );
				}

				// Check Client - Check HeartBeat
				for (DWORD dwN=1; dwN<dwMaxClient; ++dwN)
				{					
					if ( 
						INVALID_SOCKET != m_pClientManager->GetSocket( dwN ) && 
						m_pClientManager->IsClientSlot( dwN ) && 
						!m_pClientManager->CheckHeartBeat(dwN))
					{
						CloseClient( dwN );
					}
				}

                sc::SeqUniqueGUID::Instance()->reset();
			}

			//bHearBeat flag toggle.
			bHeartBeat = !bHeartBeat;

			dwHeartBeatStart2 = dwTimeTemp;
		}

		// ������ ����
		if ( (dwTimeTemp - dwTimeS) >= FPS_UPDATE_TIME )	//1min
		{
			// �������ϸ� 
			DWORD dwProfileStart = timeGetTime();
			PROFILE_DUMPOUTPUT_FILE();
			DWORD dwProfileEnd = timeGetTime() - dwProfileStart;

			// ó���ð��� 500msec �̻��̶�� ����Ѵ�.
			if (dwProfileEnd > 500)
				sc::writeLogInfo(sc::string::format("PROFILE_DUMPOUTPUT_FILE %d msec", dwProfileEnd));

			fFrame = ((float)((fCount * FPS_UPDATE_TIME) /(float)(dwTimeTemp - dwTimeS)));
            
            UpdateProcDisplay(fFrame);

			m_pClientManager->resetPacketCount();
			
			dwTimeS = dwTimeTemp;
			fCount = 0;
		}
		else
		{
			fCount++;

			DWORD dwProfileDumpStart = timeGetTime();
			g_profile_1.DumpOutputToNon();
			DWORD dwProfileDumpEnd = timeGetTime() - dwProfileDumpStart;
			if (dwProfileDumpEnd > 500)
				sc::writeLogInfo( sc::string::format("INFO:PROFILE::DumpOutputToNon() %d msec", dwProfileDumpEnd));
		}

		PrintSystemInfo();
	}

	return 0;
}

int CLoginServer::WorkProc()
{
	PER_IO_OPERATION_DATA* pPerIoData = NULL;
	DWORD dwSndBytes  = 0;
	DWORD dwRcvBytes  = 0;
	DWORD dwByteTrans = 0;
	DWORD dwFlags     = 0;
	DWORD dwClient    = -1;
	int	  nRetCode    = 0;
	int   nLastError = 0;
	DWORD dwMaxClient = ( DWORD ) m_pClientManager->GetMaxClient();
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
		if ( ( dwClient < 0 ) || ( dwClient >= dwMaxClient ) )
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

		if (nRetCode != 0 && pPerIoData == NULL)
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
		case NET_RECV_POSTED:			
			{
                m_pClientManager->addRcvMsg( dwClient, pPerIoData->Buffer, dwByteTrans );				
				// Get one Message
				while ( true )
                {
					pNmg = ( NET_MSG_GENERIC* ) m_pClientManager->getRcvMsg( dwClient );
					if ( !pNmg )
                    {
						break;
					}
					else
                    {
						m_pRecvMsgManager->MsgQueueInsert( dwClient, pNmg, pNmg->Size() );
					}
				}
				
                m_pClientManager->ReleaseOperationData( pPerIoData );
				
				dwRcvBytes = sizeof( NET_MSG_GENERIC );
				
				pPerIoData = ( PER_IO_OPERATION_DATA* ) GetFreeOverIO( NET_RECV_POSTED );

				if ( pPerIoData == NULL ) continue;
				
				// add reference
				m_pClientManager->AddReference( dwClient );

				nRetCode = ::WSARecv(m_pClientManager->GetSocket( dwClient ),
									 &( pPerIoData->DataBuf ), 
									 1, 
									 &dwRcvBytes,
									 &dwFlags,
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
		// ������ �Ϸ� �뺸 �϶�...
		case NET_SEND_POSTED:
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
				m_pClientManager->AddReference( dwClient );

				if ( SOCKET_ERROR == ::WSASend(
					m_pClientManager->GetSocket( dwClient ),
					&( pPerIoData->DataBuf ),
					1,
					&dwSndBytes,
					dwFlags,
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
		if( !m_bUseEventThread ) Sleep( 0 );
	}
	
	sc::writeLogError(std::string("WorkProc End"));

	return 0;
}

int	CLoginServer::SendClient( DWORD dwClient, NET_MSG_GENERIC* pMsg )
{
    if ( pMsg )
    {
	    PROFILE_MSG_COUNT_SEND( pMsg );
	    return m_pClientManager->SendClient2( dwClient, pMsg );
    }
    else
    {
         return NET_ERROR;
    }
}

/// ���� ������ ���� ����
void CLoginServer::SetVersion(int PatchVer, int LauncherVer, int PatchCutVersion)
{
	m_PatchVersion    = PatchVer;
	m_LauncherVersion = LauncherVer;
    m_PatchCutVersion = PatchCutVersion;

	m_pConfig->SetVersion( m_LauncherVersion, m_PatchVersion, m_PatchCutVersion );
	//m_pConfig->SetPatchVersion(PatchVer);
	//m_pConfig->SetLauncherVersion(LauncherVer);
    //m_pConfig->SetPatchCutVersion(PatchCutVersion);

    SessionSndVersion();
}

// ������/������� ���� ��ȭ
void CLoginServer::SetInspecting(bool bStatus)
{
	m_bInspecting = bStatus;

	if (m_bInspecting)
		sc::writeLogError(std::string("Inspecting Start"));
	else
		sc::writeLogError(std::string("Inspecting End"));
}

// ���� ���� ��ȯ ������/�������
bool CLoginServer::GetInspection()
{
	return m_bInspecting;
}

// �������϶� ����IP��� ��뿩��
void CLoginServer::SetAllowIP( bool bAllowIP )
{
	m_bAllowIP = bAllowIP;
}

bool CLoginServer::GetAllowIP()
{
    return m_bAllowIP;
}

void CLoginServer::ClearBlockIp()
{
    if (m_pAddressChecker)
        m_pAddressChecker->ClearIpData();
}

int CLoginServer::ExecuteCommand(char* strCmd)
{
    if (!strCmd)
        return NET_ERROR;

    std::string strCommand(strCmd);
    std::vector<std::string> SplitVec;    
    boost::split(SplitVec, strCommand, boost::algorithm::is_any_of(" "));
    if (SplitVec.empty())
        return NET_ERROR;
	
	if (SplitVec[0] == "print_ip")
	{
		// SERVER_UTIL::CAddressChecker::GetInstance()->Save();
		return 0;
	}	
	else if (SplitVec[0] == "req")
	{
        // Request
        if (SplitVec.size() > 1)
        {
			if (SplitVec[1] == "info")
			{
				// SessionReqSvrInfo();
			}
			return 0;
		}
	}	
	else if (SplitVec[0] == "profile_on")
	{
        // PROFILE ���.
		g_bProfile = TRUE;
		sc::writeLogError(std::string("PROFILE ON"));
	}	
	else if (SplitVec[1] == "profile_off")
	{
		g_bProfile = FALSE;
		sc::writeLogError(std::string("PROFILE OFF"));
	}
    else
    {
    }
	return 0;
}

void CLoginServer::UpdateProcDisplay(float fFrame) 
{
    // ���� ����ڼ�/������ ���� ���÷���
    
    sc::writeInfo(
        sc::string::format(
            "Update %1% FPS Avg %2% msec User %3%/%4%",
            (int) (fFrame/(FPS_UPDATE_TIME/1000)),
            (int) (FPS_UPDATE_TIME/fFrame),
            (int) m_pClientManager->GetCurrentClientNumber(),
            (int) m_pClientManager->GetMaxClient()));


    // ��Ŷ ó���� ���÷��� (������)    

    DWORD InTraffic = 8 * m_pClientManager->getInPacketSize() / (FPS_UPDATE_TIME/1000);
    sc::writeInfo(
        sc::string::format(
            "In Packet Count %1%/sec Traffic %2%(bps)/sec",
            (int) (m_pClientManager->getInPacketCount() / (FPS_UPDATE_TIME/1000)),
            (int) InTraffic));


    // ��Ŷ ó���� ���÷��� (������)

    DWORD OutTraffic = 8 * m_pClientManager->getOutPacketSize() / (FPS_UPDATE_TIME/1000);
    sc::writeInfo(
        sc::string::format(
            "Out Packet Count %1%/sec Traffic %2%(bps)/sec",
            (int) (m_pClientManager->getOutPacketCount() / (FPS_UPDATE_TIME/1000)),
            OutTraffic));

	sc::writeInfo(
		sc::string::format(
			"Msg Queue Size %1%", // Send %2% Recv %3%",
			m_pRecvMsgManager->GetCount()
			//m_pClientManager->GetUseSendMemoryCount(),
            //m_pClientManager->GetUseRecvMemoryCount()
			));

}

int CLoginServer::RegularScheduleProc()
{
	// ���������� ó���ؾ��� �۾��� �ִٸ� ���⼭ ó���Ѵ�

	return 0;
}
