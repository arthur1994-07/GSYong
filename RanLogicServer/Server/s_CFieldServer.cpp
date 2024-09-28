#include "../pch.h"
#include "../../SigmaCore/Util/SystemInfo.h"
#include "../../SigmaCore/Math/SeqUniqueGuid.h"
#include "../../SigmaCore/Math/Random.h"

#include "../../Enginelib/Common/profile.h"


#include "../FieldServer/GLGaeaServer.h"
#include "../Database/ODBC/s_COdbcManager.h"
#include "../Database/ADO/ADOManager.h"
#include "../Database/DBAction/DbActionGameCountry.h"
#include "../Client/NetFieldClientMan.h"
//#include "../InstanceSystem/LogicField/InstanceSystemField.h"
//#include "../TriggerSystem/ServerExitMessage.h"
#include "../../RanLogic/Util/GlobalAuthManager.h"
#include "../Util/IpAddressChecker.h"
#include "./FieldServerConfigXml.h"
#include "./s_CFieldServer.h"


// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


CFieldServer::CFieldServer(HWND hWnd, HWND hEditBox, HWND hEditBoxInfo, SUBPATH* pPath, EMSERVICE_PROVIDER ServiceProvider, const bool bEmulator)
	: NetServer(hWnd, hEditBox, hEditBoxInfo, pPath, ServiceProvider)
	, m_bEmulator(bEmulator)
	, m_dwFieldServerID(0)
	, m_pClientManager(NULL)
	, m_dwAgentSlot(0)
	, m_bReservationServerStop(false)
	, m_fReservationtime(0.0f)
	, m_dwHeartBeatStart1(0)
	, m_dwHeartBeatStart2(0)
	, m_pGaeaServer(NULL)
    , m_CacheServerSlot(net::INVALID_SLOT)
    , m_SessionServerSlot(net::INVALID_SLOT)
	, m_dwCompackHeapTime( timeGetTime() )
	, m_fChaJoinRandom( 0.0f )
	, m_dwErrorCode( 0 )
	, m_dwMemoryCheckTime( timeGetTime() )
	, m_dwForceServerStopCheckTime( 0 )
	, m_bCheckMemoryAndReservationForceServerStop( false )
	, m_dwServerRestartCheckTime( timeGetTime() )
	, m_IntegrationCacheServerSlot( net::INVALID_SLOT )
{
	MessageEnumMixer::GetInstance();

	sc::LogMan::getInstance()->attachEditControl(hEditBox, hEditBoxInfo);
	initPacketFunc();
    m_pConfig = new FieldServerConfigXml;
    m_pGaeaServer = new GLGaeaServer(m_ServiceProvider, pPath);
	m_pAuthManager = new sc::CGlobalAuthClientLogic;
}

CFieldServer::~CFieldServer()
{
	if (m_pGaeaServer)
		m_pGaeaServer->CleanUp();	//MUST be called before the destructor of GLGaeaServer.
    SAFE_DELETE(m_pGaeaServer);
	SAFE_DELETE(m_pAuthManager);

	SAFE_DELETE(m_pClientManager);
    SAFE_DELETE(m_pConfig);    
}

int CFieldServer::StartClientManager()
{
	///////////////////////////////////////////////////////////////////////////
	// 클라이언트 정보저장 클래스 생성
	SAFE_DELETE(m_pClientManager);

	//m_nMaxClient = m_nMaxClient * 2;	//mjeon.AF

	m_pClientManager = new NetFieldClientMan( m_nMaxClient, m_hIOServer, m_pConfig->GetReferenceCountMax(), m_ServiceProvider );
	if (!m_pClientManager)
	{
		m_bIsRunning = false;
		return NET_ERROR;
	}
	m_pClientManager->SetAgentIP(m_pConfig->GetAgentIP());
    m_pClientManager->SetCacheIP(m_pConfig->GetCacheServerIP());
    
	return NET_OK;
}

int CFieldServer::StartDbManager()
{
	int nRetCode;	
	
    SAFE_DELETE(m_pDbManager);

    m_pDbManager = new COdbcManager(this, db::FIELD_SERVER, m_ServiceProvider, m_pConfig->UseCustomDbPool());
	if (!m_pDbManager)
	{
		sc::writeLogError(std::string("DB:Couldn't get an DB Manager Instance."));
		return NET_ERROR;
	}
	
	nRetCode = m_pDbManager->OdbcOpenUserDB(
        m_pConfig->GetUserOdbcName(),
        m_pConfig->GetUserOdbcUser(),
        m_pConfig->GetUserOdbcPass(),
        m_pConfig->GetUserOdbcDatabase(),
        m_pConfig->UseCustomDbPool());

	if (nRetCode == sc::db::DB_ERROR)
	{
		//DB_Shutdown();		

        sc::writeLogError( "OdbcOpenUserDB" );

		m_bIsRunning = false;
		return NET_ERROR;
	}
	else
	{
		sc::writeLogInfo(std::string("ODBC:User DB Open OK"));
	}
	
	nRetCode = m_pDbManager->OdbcOpenGameDB(
        m_pConfig->GetGameOdbcName(),
        m_pConfig->GetGameOdbcUser(),
        m_pConfig->GetGameOdbcPass(),
        m_pConfig->GetGameOdbcDatabase(),
        m_pConfig->UseCustomDbPool());

	if (nRetCode == sc::db::DB_ERROR)
	{
        sc::writeLogError( "OdbcOpenGameDB" );

		m_bIsRunning = false;
		return NET_ERROR;
	}
	else
	{
		sc::writeLogInfo(std::string("ODBC:Game DB Open OK"));
	}

	nRetCode = m_pDbManager->OdbcOpenLogDB(
        m_pConfig->GetLogOdbcName(),
        m_pConfig->GetLogOdbcUser(),
        m_pConfig->GetLogOdbcPass(),
        m_pConfig->GetLogOdbcDatabase(),
        m_pConfig->UseCustomDbPool());

	if (nRetCode == sc::db::DB_ERROR)
	{
        sc::writeLogError( "OdbcOpenLogDB" );

		m_bIsRunning = false;
		return NET_ERROR;
	}
	else
	{
		sc::writeLogInfo(std::string("ODBC:Log DB Open OK"));
	}

	// Shop DB
	nRetCode = m_pDbManager->OdbcOpenShopDB(
        m_pConfig->GetShopOdbcName(),
        m_pConfig->GetShopOdbcUser(),
        m_pConfig->GetShopOdbcPass(),
        m_pConfig->GetShopOdbcDatabase(),
        m_pConfig->UseCustomDbPool());

	if (nRetCode == sc::db::DB_ERROR)
	{
        sc::writeLogError( "OdbcOpenShopDB" );

		m_bIsRunning = false;
		return NET_ERROR;
	}
	else
	{
		sc::writeLogInfo(std::string("ODBC:Shop DB Open OK"));
	}

	// Terra User Database (Malaysia, Philippines, Thailand)
	if (m_ServiceProvider == SP_PHILIPPINES || m_ServiceProvider == SP_MALAYSIA || m_ServiceProvider == SP_THAILAND)
	{
		nRetCode = m_pDbManager->OdbcOpenTerraDB(
            m_pConfig->GetTerraOdbcName(),
            m_pConfig->GetTerraOdbcUser(),
            m_pConfig->GetTerraOdbcPass(),
            m_pConfig->GetTerraOdbcDatabase(),
            m_pConfig->UseCustomDbPool());

		if (nRetCode == sc::db::DB_ERROR)
		{
            sc::writeLogError( "OdbcOpenTerraDB" );

			m_bIsRunning = false;
			return NET_ERROR;
		}
		else
		{
			sc::writeLogInfo(std::string("ODBC:Terra DB Open OK"));
		}
	}	

	// Gsp User Database
	if (m_ServiceProvider == SP_GLOBAL)
	{
		nRetCode = m_pDbManager->OdbcOpenGspDB(
            m_pConfig->GetGspOdbcName(),
            m_pConfig->GetGspOdbcUser(),
            m_pConfig->GetGspOdbcPass(),
            m_pConfig->GetGspOdbcDatabase(),
            m_pConfig->UseCustomDbPool());

		if (nRetCode == sc::db::DB_ERROR)
		{
            sc::writeLogError( "OdbcOpenGspDB" );

			m_bIsRunning = false;
			return NET_ERROR;
		}
		else
		{
			sc::writeLogInfo(std::string("ODBC:Gsp DB Open OK"));
		}
	}

	SAFE_DELETE(m_pAdoManager);
    m_pAdoManager = new db::AdoManager(this, db::AGENT_SERVER, m_ServiceProvider);

	m_pAdoManager->SetConnectionString(
        db::USER_DB,
        m_pConfig->UserDbSource(),
        m_pConfig->UserDbDatabaseName(),
        m_pConfig->UserDbUser(),
		m_pConfig->UserDbPassword(),
		m_pConfig->GetAdoMaxPoolSize(),
		m_pConfig->GetDatabaseTcpPacketSize());    

    m_pAdoManager->SetConnectionString(
        db::GAME_DB,
        m_pConfig->GameDbSource(),
        m_pConfig->GameDbDatabaseName(),
        m_pConfig->GameDbUser(),
		m_pConfig->GameDbPassword(),
		m_pConfig->GetAdoMaxPoolSize(),
		m_pConfig->GetDatabaseTcpPacketSize());

	m_pAdoManager->SetConnectionString(
        db::LOG_DB,
        m_pConfig->LogDbSource(),
        m_pConfig->LogDbDatabaseName(),
        m_pConfig->LogDbUser(),
		m_pConfig->LogDbPassword(),
		m_pConfig->GetAdoMaxPoolSize(),
		m_pConfig->GetDatabaseTcpPacketSize());

    m_pAdoManager->SetConnectionString(
        db::SHOP_DB,
        m_pConfig->ShopDbSource(),
        m_pConfig->ShopDbDatabaseName(),
        m_pConfig->ShopDbUser(),
		m_pConfig->ShopDbPassword(),
		m_pConfig->GetAdoMaxPoolSize(),
		m_pConfig->GetDatabaseTcpPacketSize());

	m_pAdoManager->SetConnectionString(
		db::TEXASHOLDEM_DB,
		m_pConfig->TexasHoldemIFDbSource(),
		m_pConfig->TexasHoldemIFDbDatabaseName(),
		m_pConfig->TexasHoldemIFDbUser(),
		m_pConfig->TexasHoldemIFDbPassword(),
		m_pConfig->GetAdoMaxPoolSize(),
		m_pConfig->GetDatabaseTcpPacketSize());

	// 전장서버는 알파와 GS에서만 사용
	if( m_ServiceProvider == SP_KOREA_TEST || m_ServiceProvider == SP_GS || m_ServiceProvider == SP_OFFICE_TEST || m_ServiceProvider == SP_KOREA )
	{
		m_pAdoManager->SetConnectionString(
			db::WORLDBATTLE_RELATED_DB,
			m_pConfig->WorldBattleDbSource(),
			m_pConfig->WorldBattleDbDatabaseName(),
			m_pConfig->WorldBattleDbUser(),
			m_pConfig->WorldBattleDbPassword(),
			m_pConfig->GetAdoMaxPoolSize(),
			m_pConfig->GetDatabaseTcpPacketSize());    
	}

	if( true == m_pConfig->GetUseRanMobile() )
	{
		m_pAdoManager->SetConnectionString(
			db::RANMOBILE_DB,
			m_pConfig->RanMobileDbSource(),
			m_pConfig->RanMobileDbDatabaseName(),
			m_pConfig->RanMobileDbUser(),
			m_pConfig->RanMobileDbPassword(),
			m_pConfig->GetAdoMaxPoolSize(),
			m_pConfig->GetDatabaseTcpPacketSize());    
	}

	return NET_OK;
}

int CFieldServer::Start()
{
    SAFE_DELETE(m_pGaeaServer);
    m_pGaeaServer = new GLGaeaServer(m_ServiceProvider, m_pPath);

	int nRetCode = 0;
	HRESULT hr;
	m_bIsRunning = true;
    m_vecCloseList.clear();

    const std::string XmlFileName("FieldServer.xml");    
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

	if (StartDbManager() == NET_ERROR)
        return NET_ERROR;

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

	//	Note : 가이아 서버 생성.
	m_dwFieldServerID = m_pConfig->GetServerField();
	DWORD dwMaxClient = m_pConfig->GetMaxClient();
	int nChannel = m_pConfig->GetServerChannel();
	bool bPKMODE = m_pConfig->IsPKServer() == true;	
	BYTE ExcessExpProcessType = m_pConfig->GetExcessExpProcessType();
	bool bPKServer = m_pConfig->IsFreePKServer();

	sc::CSystemInfo SysInfo;

	hr = DxServerInstance::FieldCreate(
        m_pGaeaServer,
        SysInfo.GetAppPath(),
        this,        
        m_pDbManager,
		m_pAdoManager,
        m_ServiceProvider,
        m_pPath,
        NULL,
        m_dwFieldServerID,
        dwMaxClient,
        bPKMODE,
        nChannel,
        m_bPKLess,
        ExcessExpProcessType,
        m_LanguageSet, 
        bPKServer,
        false,
		m_pConfig);
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

    // Connect Cache server
    nRetCode = CacheConnect();
    if (nRetCode != NET_OK)
    {
        m_bIsRunning = false;
        return NET_ERROR;
    }

	// Connect Integration Cache server
	nRetCode = IntegrationCacheConnect();
	if ( nRetCode != NET_OK )
	{
		m_bIsRunning = false;
		return NET_ERROR;
	}

	// Send Server info to Session Server
	SessionSndSvrInfo();
	//SessionSndSvrInfo();

#ifdef _USE_AUTH
	//if (m_ServiceProvider == SP_EU)
	//{	
		// 세션서버로 해당 서버의 정보를 보낸 다음에 인증처리를 진행해야함.
		if (SendAuthInfoToSessionServer() == NET_ERROR)
		{
			m_bIsRunning = false;
			return NET_ERROR;
		}
	//}
#endif
	
	//mjeon.연속적인 enum형태로 수정하였으므로 이제는 총 몇개의 메시지가 사용되는지가 의미있다.
	sc::writeLogInfo( sc::string::format("Number of used network messages: %d", NET_MSG_GCTRL_MAX) );	
	//sc::writeLogInfo( sc::string::format("NET_MSG_BASE %d", NET_MSG_BASE) );
	
	sc::writeLogInfo(
		sc::string::format(
		"FD_SETSIZE %1%",
		FD_SETSIZE));

	sc::writeLogInfo(std::string(">Server Start OK"));

#ifdef _USE_TBB
	sc::writeLogInfo(std::string("*** Parallelized by TBB ***"));
#else
	sc::writeLogWarn(std::string("*** Not-Parallelized ***"));
#endif

	sc::writeLogInfo(std::string("======================================================="));

	m_nStatus = S_SERVER_RUNING;

	// 알고 있는 주소에 자신의 주소, Session 서버의 주소, Agent 서버의 주소를 등록한다.	
    m_pAddressChecker->SetConnectionCountAllow(m_pConfig->GetConnectionPerIpAddress());
    m_pAddressChecker->AddAllowIp(m_pConfig->GetServerIP());
    m_pAddressChecker->AddAllowIp(m_pConfig->GetSessionServerIP());
    m_pAddressChecker->AddAllowIp(m_pConfig->GetAgentIP());

	
	std::string strTitle;

	char szTempChar[64];
	GetWindowText(m_hWnd, szTempChar, sizeof(szTempChar));

	strTitle = std::string(szTempChar) + sc::string::format(" (%1%)", m_dwFieldServerID);
	
	SetWindowText(m_hWnd, strTitle.c_str());

	m_fChaJoinRandom = sc::Random::RANDOM_NUM();

	return NET_OK;
}

int CFieldServer::FinalStop()
{
	if ( m_dwErrorCode )
	{
		return ForceFinalStop();
	}

	// Stop All Thread and exit
	DWORD dwExitCode = 0;

    lockOn();
	m_bIsRunning = false;
	lockOff();

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

	CloseAllClient();

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

int CFieldServer::Stop()
{
	// 서버의 정지를 예약한다.
	m_bReservationServerStop = true;
	sc::writeLogInfo(std::string("== Please wait until server stop"));
	GLMSG::SNET_REQ_SERVERSTOP InnerMsg;
	InsertMsg (DBACTION_CLIENT_ID, (char*) &InnerMsg);

	return NET_OK;
}

void CFieldServer::ForceStop( DWORD Reason )
{
	if ( m_bReservationServerStop )
		return;
	m_dwErrorCode = 1;
	m_bReservationServerStop = true;
	sc::writeLogInfo( std::string( "== Please wait until server stop(Force)" ) );
	GLMSG::SNET_REQ_SERVERSTOP InnerMsg;
	InnerMsg.dwError = Reason;
	InsertMsg( DBACTION_CLIENT_ID, ( char* ) &InnerMsg );
}

/*
int	CFieldServer::Pause()
{
	::closesocket(m_sServer);
	m_sServer = INVALID_SOCKET;
	CloseAllClient();
	m_nStatus = S_SERVER_PAUSE;
	// Put message to console
	sc::writeLogInfo( std::string("=======================================================") );
	sc::writeLogInfo( std::string("Server Pause OK") );
	sc::writeLogInfo( std::string("=======================================================") );

	return NET_OK;	
}

int CFieldServer::Resume()
{
	if (StartListenThread() == NET_OK)
	{
		m_nStatus = S_SERVER_RUNING;
		// Put message to console
		sc::writeLogInfo( std::string("=======================================================") );
		sc::writeLogInfo( std::string("Server Resume OK") );
		sc::writeLogInfo( std::string("=======================================================") );

		return NET_OK;
	}
	else
	{
		// Put message to console
		sc::writeLogError( std::string("=======================================================") );
		sc::writeLogError( std::string("Server Resume Failed") );
		sc::writeLogError( std::string("=======================================================") );

		return NET_ERROR;
	}
}

int	CFieldServer::ReStart()
{
	Stop();
	Start();
	return NET_OK;
}
*/

// close all client connections
void CFieldServer::CloseAllClient()
{
	sc::writeLogInfo( "CFieldServer::CloseAllClient()" );
	if ( m_pClientManager )
	{
		for ( DWORD i=0; i < (DWORD) m_pClientManager->GetMaxClient(); ++i )
		{
			if ( INVALID_SOCKET != m_pClientManager->GetSocket( i ) )
				CloseClient( i, net::LOGOUT_ALL );
		}
	}
}


namespace PROFILE
{
	void DumpOutputToNon ();
};

void CFieldServer::ResetGaeaID(DWORD dwClient)
{
	m_pClientManager->ResetGaeaID(dwClient);
}

// 사용자를 로그아웃 시킨다.
void CFieldServer::CloseClient( DWORD dwClient, net::CLOSE_TYPE emCloseType, int nLastError )
{
	lockOn();

	if ( dwClient >= static_cast<DWORD>(m_nMaxClient) )
	{
		sc::writeLogError( sc::string::format( "CloseClient wrong dwClient is %d, Close Type %d, Error Type %d", dwClient, emCloseType, nLastError ) );
		lockOff();
		return;
	}

	int   nUserNum = 0;	
	DWORD dwGaeaID = 0;

	switch ( m_pClientManager->GetSlotType( dwClient ) )
	{
	case net::SLOT_CACHE:
		{
			m_pClientManager->CloseClient( dwClient );

			// Cache 서버 통신 종료
			if ( m_bIsRunning && INVALID_SOCKET == m_pClientManager->GetSocket( m_CacheServerSlot ) )
			{
				// 재연결 시도                
				sc::writeLogError(
					sc::string::format(
					"Cache server connection closed. will try to reconnect. Current Cache slot is %1%, Close Type %2%, Error Type %3%", m_CacheServerSlot, emCloseType, nLastError ) );
				m_CacheServerSlot = net::INVALID_SLOT;
				CacheConnect();
			}
		}
		break;
	case net::SLOT_SESSION:
		{
			sc::writeLogInfo( sc::string::format( "Session server connection closed. slot %1%, Close Type %2%, Error Type %3%", dwClient, emCloseType, nLastError ) );
			m_pClientManager->CloseClient( dwClient );
			if ( m_bIsRunning && INVALID_SOCKET == m_pClientManager->GetSocket( m_SessionServerSlot ) )
			{	
				sc::writeLogInfo(
					sc::string::format( "Session server connection closed. will try to reconnect. Current Session slot is %1%, Close Type %2%, Error Type %3%", m_SessionServerSlot, emCloseType, nLastError ) );
				m_SessionServerSlot = net::INVALID_SLOT;
				SessionConnectSndSvrInfo();
			}
		}
		break;
	case net::SLOT_AGENT:
		{
			sc::writeLogInfo( sc::string::format( "Agent server connection closed. slot %1%, Close Type %2%, Error Type %3%", dwClient, emCloseType, nLastError ) );
			m_pClientManager->CloseClient( dwClient );
			if ( m_bIsRunning && INVALID_SOCKET == m_pClientManager->GetSocket( m_dwAgentSlot ) )
			{	
				sc::writeLogInfo(
					sc::string::format( "Agent server connection closed. will try to reconnect. Current Agent slot is %1%, Close Type %2%, Error Type %3%", m_dwAgentSlot, emCloseType, nLastError ) );
				m_dwAgentSlot = net::INVALID_SLOT;
				SessionConnectSndSvrInfo();
			}
		}
		break;
	case net::SLOT_CLIENT:
		{
			// 등록된 IP 리스트를 제거한다.
			std::string ClientIp = m_pClientManager->GetClientIP( dwClient );
			m_pAddressChecker->DelConnection( ClientIp );

			if ( !m_pClientManager->IsClientSlot( dwClient ) )
			{
				sc::writeLogInfo(
					sc::string::format(
					"!m_pClientManager->IsClientSlot( dwClient ) %1% ip %2%, Close Type %3%, Error Type %4%", dwClient, ClientIp.c_str(), emCloseType, nLastError ) );
			}
			else
			{
				nUserNum = m_pClientManager->UserDbNum( dwClient );

				if ( nUserNum > 0 )
				{
					dwGaeaID = m_pClientManager->GetGaeaID( dwClient );

					if ( GAEAID_NULL != dwGaeaID )
					{						
						sc::writeLogInfo( sc::string::format( "Client connection closed. slot %1% chanum %2%, Close Type %3%, Error Type %4%", dwClient, m_pClientManager->ChaDbNum( dwClient ), emCloseType, nLastError ) );

// 						// Trigger System 퇴출메시지 전송;
// 						{
// 							TriggerSystem::SServerExitMessage sMsg;
// 							sMsg.dwCharDbNum = (DWORD)m_pClientManager->ChaDbNum( dwClient );
// 							GLChar* pChar = m_pGaeaServer->GetCharByDbNum( sMsg.dwCharDbNum );
// 							if ( pChar )
// 							{
// 								pChar->SendTriggerMessage( &sMsg );
// 							}
// 						}

						// 가이아서버 캐릭터 드랍 예약
						m_pGaeaServer->ReserveDropOutPC( dwGaeaID, db::DbActionNull::Null );

//                         // CloseClient의 경우 접속 종료이므로 Offline 이벤트 발생;
//                         InstanceSystem::ManagerField* _pInstanceMan = m_pGaeaServer->GetInstanceManagerField();
//                         if ( _pInstanceMan )
//                         {
//                             const DWORD charDbNum = (DWORD)m_pClientManager->ChaDbNum(dwClient);
//                             _pInstanceMan->offline(charDbNum);
//                         }
					}
				}
			}

			m_pClientManager->CloseClient( dwClient );

			// 현재 서버상태 전송
			SessionSndSvrCurState();
		}
		break;
	case net::SLOT_INTEGRATION_CACHE:
		{
			m_pClientManager->CloseClient( dwClient );

			// Integration Cache 서버 통신 종료
			if ( m_bIsRunning && INVALID_SOCKET == m_pClientManager->GetSocket( m_IntegrationCacheServerSlot ) )
			{
				// 재연결 시도
				sc::writeLogError(
					sc::string::format(
					"Integration Cache server connection closed. will try to reconnect. Current Integration Cache slot is %1%, Close Type %2%, Error Type %3%", m_IntegrationCacheServerSlot, emCloseType, nLastError ) );
				m_IntegrationCacheServerSlot = net::INVALID_SLOT;
				if ( NET_OK == IntegrationCacheConnect() )
				{
					SendPrivateMarketData();
				}
			}
		}
		break;
	default:
		// 알수없는 연결 끊김
		sc::writeLogInfo( sc::string::format( "Unknown reason connection close. slot %1% client type %2%, Close Type %3%, Error Type %4%", dwClient, m_pClientManager->GetSlotType( dwClient ), emCloseType, nLastError ) );
		break;
	}

	lockOff();
}

/*
void CFieldServer::Send(DWORD dwClient, PER_IO_OPERATION_DATA* PerIoData, DWORD dwSize)
{	
	sc::writeLogError(sc::string::format("Illegal Function call Send %1%", dwClient));
}
*/

void CFieldServer::SendAllClient(EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc)
{
    NET_MSG_PACK MsgPack;
    if (MsgPack.SetData(MsgType, PackBuffer, UseCrc))
        SendAllClient(&MsgPack);
}

void CFieldServer::SendAllClient(NET_MSG_GENERIC* pBuffer)
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

int CFieldServer::SendClient(EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, DWORD dwChaNum, bool UseCrc)
{
    NET_MSG_PACK MsgPack;
    if (MsgPack.SetData(MsgType, PackBuffer, UseCrc))
        return SendClient(&MsgPack, dwChaNum);
    else
        return NET_ERROR;
}

//mjeon.AF
int CFieldServer::SendClient(NET_MSG_GENERIC* pBuffer, DWORD dwChaNum)
{
	DWORD dwClient = m_pClientManager->GetClientID(dwChaNum);

	if (dwClient == CLIENTID_NULL)
	{		
		return NET_ERROR;
	}

	return SendClient(dwClient, pBuffer);
}

int CFieldServer::SendClient(DWORD dwClient, EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc)
{
    NET_MSG_PACK MsgPack;
    if (MsgPack.SetData(MsgType, PackBuffer, UseCrc))
        return SendClient(dwClient, &MsgPack);
    else
        return NET_ERROR;
}

int CFieldServer::SendClient(DWORD dwClient, NET_MSG_GENERIC* pBuffer)
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

int CFieldServer::SendField(DWORD dwClient, EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc)
{
    GASSERT(0&&"CFieldServer::SendField()");
    return 0; 
}

int CFieldServer::SendField(DWORD dwClient, NET_MSG_GENERIC* pBuffer)
{ 
    GASSERT(0&&"CFieldServer::SendField()");
    return 0; 
}

int CFieldServer::SendField(DWORD dwChannel, DWORD dwFieldSvrNum, EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc)
{
    GASSERT(0&&"CFieldServer::SendField()");
    return 0;
}

int CFieldServer::SendField(DWORD dwChannel, DWORD dwFieldSvrNum, NET_MSG_GENERIC* pBuffer)
{ 
    GASSERT(0&&"CFieldServer::SendField()");
    return 0;
}

int CFieldServer::SendFieldSvr(int nSvrNum, EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, int nServerChannel, bool UseCrc)
{
    GASSERT(0&&"CFieldServer::SendField()");
    return 0;
}

int CFieldServer::SendFieldSvr(int nSvrNum, NET_MSG_GENERIC* pBuffer, int nServerChannel)
{ 
    GASSERT(0&&"CFieldServer::SendField()");
    return 0;
}

int CFieldServer::SendAgent(EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc)
{
    NET_MSG_PACK MsgPack;
    if (MsgPack.SetData(MsgType, PackBuffer, UseCrc))
        return SendAgent(&MsgPack);
    else
        return NET_ERROR;
}

int CFieldServer::SendAgent( NET_MSG_GENERIC* pBuffer )
{	
    if ( !pBuffer )
	{
		return NET_ERROR;
	}
	else
	{
		PROFILE_MSG_COUNT_SEND( pBuffer );
		return m_pClientManager->SendAgent( m_dwAgentSlot, pBuffer );
	}
}

int CFieldServer::SendAgent ( DWORD dwClient, NET_MSG_GENERIC* pBuffer )
{
	return NET_ERROR;
}

int CFieldServer::SendAgent ( DWORD dwClient, EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc )
{
	return NET_ERROR;
}

int CFieldServer::SendAgent_ServerGroup ( DWORD dwServerGroup, NET_MSG_GENERIC* pBuffer )
{
	return NET_ERROR;
}

int CFieldServer::SendAgent_ServerGroup ( DWORD dwServerGroup, EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc )
{
	return NET_ERROR;
}

void CFieldServer::SendMySelf(DWORD dwClient, EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc)
{
    NET_MSG_PACK MsgPack;
    if (MsgPack.SetData(MsgType, PackBuffer, UseCrc))
        SendMySelf(dwClient, &MsgPack);
}

void CFieldServer::SendMySelf(DWORD dwClient, NET_MSG_GENERIC* pBuffer)
{
    if ( pBuffer )
    {
        PROFILE_MSG_COUNT_SEND( pBuffer );
        InsertMsg(dwClient, pBuffer);
    }
}

int CFieldServer::CheckTracingUser(const char *pUserAccount, const int nUserNum)
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

void CFieldServer::RecvMsgProcess()
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

void CFieldServer::FieldSrvHeartBeatCheck( DWORD dwUdpateTime )
{
    DWORD dwCurrentTime = timeGetTime();


 	// 1시간에 한번씩 단편화된 메모리 정리해준다.
 	if ( ( dwCurrentTime - m_dwCompackHeapTime ) >= 3600000 )
 	{
 		m_dwCompackHeapTime = dwCurrentTime;
 		sc::heapCompact();
 	}

    //mjeon	
    //Only heartbeat check for session server is done.
    //Currently, there is no hearbeat check code for clients in this function.	
    if ( ( dwCurrentTime - m_dwHeartBeatTime ) >= dwUdpateTime )
    {	
        // Client check
        // 잘못된 접속 종료, 접속만 하고 더이상 메시지를 보내지 않아서 GAEAID_NULL 인 상태
        for ( DWORD dwCheck=1; dwCheck<( DWORD ) m_nMaxClient; ++dwCheck )
        {
			// khkim
			// 클라이언트 슬롯이고
			// 소켓이 정상이고
			// 마지막으로 io 걸린 시간이 dwUdpateTime 전이라면(dwUdpateTime(60초) 동안 io 요청이 없었다면)
			// 정리를 해준다.
			// 1분에서3분으로 조정 
            if ( 
				m_pClientManager->IsClientSlot( dwCheck ) && 
				INVALID_SOCKET != m_pClientManager->GetSocket( dwCheck ) && 
				dwCurrentTime - m_pClientManager->GetLastRequestIOTime( dwCheck ) > 180000/*dwUdpateTime*/ )
            {
                sc::writeLogInfo( sc::string::format( "FieldSrvHeartBeatCheck Clien Slot(%1%) is closed.", dwCheck ) );
				CloseClient( dwCheck, net::KICKOUT_HEARTBEAT );
            }
        }

        // Session server check
        if ( ( dwCurrentTime - m_dwHeartBeatStart1 ) > SESSION_CHECK_TIME )
        {
            if ( m_SessionServerSlot == net::INVALID_SLOT || INVALID_SOCKET == m_pClientManager->GetSocket( m_SessionServerSlot ) )
            {
				sc::writeLogInfo(
					sc::string::format( 
					"FieldSrvHeartBeatCheck Session server connection close detect. will try to reconnect. Current Session slot is %1%", 
					m_SessionServerSlot ) );
				m_SessionServerSlot = net::INVALID_SLOT;
				SessionConnectSndSvrInfo();
            }
            m_dwHeartBeatStart1 = timeGetTime();
            m_pAddressChecker->ClearIpData();
        }

        // Cache server check
        if ( m_CacheServerSlot == net::INVALID_SLOT || INVALID_SOCKET == m_pClientManager->GetSocket( m_CacheServerSlot ) )
        {
			sc::writeLogInfo(
				sc::string::format( 
				"FieldSrvHeartBeatCheck Cache server connection close detect. will try to reconnect. Current Cache slot is %1%", 
				m_CacheServerSlot ) );
            m_CacheServerSlot = net::INVALID_SLOT;
            CacheConnect();
        }

		// Integration Cache server check
		if ( m_IntegrationCacheServerSlot == net::INVALID_SLOT || INVALID_SOCKET == m_pClientManager->GetSocket( m_IntegrationCacheServerSlot ) )
		{
			sc::writeLogInfo(
				sc::string::format( 
				"FieldSrvHeartBeatCheck Integration Cache server connection close detect. will try to reconnect. Current Integration Cache slot is %1%", 
				m_IntegrationCacheServerSlot ) );
			m_IntegrationCacheServerSlot = net::INVALID_SLOT;
			if ( NET_OK == IntegrationCacheConnect() )
			{
				SendPrivateMarketData();
			}
		}

        // Txt log to database
        if ( m_pGaeaServer )
            m_pGaeaServer->TxtLogToDb();

		// 클라이언트 슬롯 체크
		if ( m_pClientManager )
		{
			m_pClientManager->CleanDummyClientSlot();
		}
        
        m_dwHeartBeatTime = dwCurrentTime;

        sc::SeqUniqueGUID::Instance()->reset();
    }
}



void CFieldServer::PrintDebugMsg( DWORD dwUdpateTime )
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
		
// 		strLog = sc::string::format(
//             "Update %1% FPS Avg %2% msec Character %3%",
//             (int) (fFrame/(dwUdpateTime/1000)), 
//             (int) (dwUdpateTime/fFrame),
//             (int) m_pGaeaServer->GetNumPC());
// 		
		// 프레임 측정 디스플레이		
		strLog = sc::string::format(
			"Update %.2f fps. User %d/%d (Char:%d).",
			(fFrame/(dwUdpateTime/1000)),
			(int) m_pClientManager->GetCurrentClientNumber(),
			(int) m_pClientManager->GetMaxClient(),
			(int) m_pGaeaServer->GetNumPC() );

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

void CFieldServer::EndUpdateThread()
{
	TRACE( "[UpdateProc End]\n" );
	sc::writeLogInfo( std::string("= [UpdateProc End] - Begin") );
	{
		//	Note : 모든 활동 PC,NPC를 종료시킨다.
		m_pGaeaServer->ClearDropObj();
		m_pGaeaServer->ClearReservedDropOutPC();

		//	Note : 클럽배틀 정보를 저장한다.
		m_pGaeaServer->SaveClubBattle();

		// DB 작업이 끝날때 까지 기다린다.
		// Last db action
        m_pDbManager->ExecuteLastJob();
		m_pAdoManager->ExecuteLastJob();
		
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

void CFieldServer::ReserveCloseClient(DWORD dwClient)
{
    DWORD MaxClient = static_cast<DWORD> (m_nMaxClient);
    if (dwClient != 0 && dwClient<MaxClient && m_pClientManager->IsClientSlot(dwClient))
    {
        std::vector<DWORD>::iterator pos = std::find(m_vecCloseList.begin(), m_vecCloseList.end(), dwClient);
        if (pos == m_vecCloseList.end()) 
            m_vecCloseList.push_back(dwClient);
    }
}

void CFieldServer::CloseClientFinal()
{
    std::vector<DWORD>::iterator pos = m_vecCloseList.begin();
    std::vector<DWORD>::iterator pos_end = m_vecCloseList.end();    
    for ( ; pos != pos_end; ++pos)
    {
		CloseClient(*pos, net::KICKOUT_PACKET_COUNT);
    }
    m_vecCloseList.clear();
}

bool CFieldServer::IsReserveCloseClient( DWORD dwClient )
{
    std::vector<DWORD>::iterator pos = std::find( m_vecCloseList.begin(), m_vecCloseList.end(), dwClient );
        
    if ( pos == m_vecCloseList.end() ) 
        return false;
    else 
        return true;
}

void CFieldServer::ClearBlockIp()
{
    if (m_pAddressChecker)
        m_pAddressChecker->ClearIpData();
}

Country::SCOUNTRY_INFO CFieldServer::GetCountry ( DWORD dwDBNum ) const
{
	if ( NULL == m_pClientManager )
		return Country::SCOUNTRY_INFO();

	DWORD dwClient = m_pClientManager->GetClientID( dwDBNum );

	return m_pClientManager->GetCountryInfo( dwClient );
}

void CFieldServer::InitializeCountry( DWORD dwClientID )
{
	if ( NULL == m_pClientManager )
		return;

	GLMSG::NET_USE_ITEM_INITIALIZE_COUNTRY_FA sMsg;
	sMsg.m_dwUserDbNum = m_pClientManager->UserDbNum( dwClientID );

	SendAgent( &sMsg );

	/*DWORD dwUserID = static_cast< DWORD >(
		m_pClientManager->UserDbNum ( dwClientID ) );

	AddGameAdoJob( db::DbActionPtr(
		new CUpdateUserCountryAction( dwUserID, Country::SCOUNTRY_INFO(), dwClientID ) ) );*/
}

std::string CFieldServer::GetClientIP(DWORD dwClient) const
{ 
    return m_pClientManager->GetClientIP(dwClient);
}

net::EMSERVERTYPE CFieldServer::GetServerType()
{
	return m_pConfig->GetServerType();
}

std::string CFieldServer::GetIP()
{
	return std::string(m_pConfig->GetServerIP());
}

int	CFieldServer::GetPort()
{
	return m_pConfig->GetServicePort();
}

void CFieldServer::ResetCreatingCharUserID(DWORD dwUserID)
{
    if (m_pGaeaServer)
        m_pGaeaServer->ResetCreatingCharUserID(dwUserID);
}

void CFieldServer::ResetSaveDBUserID( DWORD dwUserID )
{
    if ( m_pGaeaServer )
        m_pGaeaServer->ResetSaveDBUserID( dwUserID );
}

void CFieldServer::SendAllField(NET_MSG_GENERIC* pBuffer, int nServerChannel)
{ 
    assert(0&&"");
}

void CFieldServer::SendAllField(EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, int nServerChannel, bool UseCrc)
{
    assert(0&&"");
}

int CFieldServer::SendMatch(NET_MSG_GENERIC* pMsg)
{ 
    GASSERT(0&&"CFieldServer::SendMatch()");
    return 0;
}

int CFieldServer::SendMatch(EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc)
{
    GASSERT(0&&"CFieldServer::SendMatch()");
    return 0;
}

void CFieldServer::SendChannel(NET_MSG_GENERIC* pBuffer, int nServerChannel)
{ 
    assert(0&&"");
}

void CFieldServer::SendChannel(EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, int nServerChannel, bool UseCrc)
{
    assert(0&&"");
}

int CFieldServer::SendAllChannel(NET_MSG_GENERIC* pBuffer)
{ 
    assert(0&&"");
	return 0;
}

int CFieldServer::SendAllChannel(EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc)
{
    assert(0&&"");
	return 0;
}

void CFieldServer::AddGameJob(db::DbActionPtr spJob)
{
    if (m_pDbManager)
    {
        m_pDbManager->AddGameJob(spJob, m_pAdoManager);
        return;
    }
    
    if (spJob)
    {
        sc::writeLogError(
            sc::string::format(
                "CFieldServer::AddGameJob m_pDbManager NULL %1%",
                typeid(*spJob).name()));
    }
}

void CFieldServer::AddGameAdoJob(db::DbActionPtr spJob)
{
    if (m_pAdoManager)
    {
        m_pAdoManager->AddGameJob(spJob, m_pAdoManager);
        return;
    }
    
    if (spJob)
    {
        sc::writeLogError(
            sc::string::format(
                "CFieldServer::AddGameAdoJob m_pAdoManager NULL %1%",
                typeid(*spJob).name()));
    }
}

void CFieldServer::SaveCharacterInSqlite( SCHARDATA2* pChadata )
{
	m_pGaeaServer->SaveCharacterInSqlite( pChadata );
}

int CFieldServer::ForceFinalStop()
{
	// Stop All Thread and exit
	DWORD dwExitCode = 0;

	lockOn();
	m_bIsRunning = false;
	lockOff();

	sc::writeLogInfo( std::string( "== Please wait until server force final stop" ) );
	if ( m_bUseEventThread )	
	{
		::SetEvent( m_hUpdateQuitEvent );

		::WaitForSingleObject( m_hUpdateThread, INFINITE );
		SAFE_CLOSE_HANDLE( m_hUpdateQuitEvent );
	}
	else
	{
		while ( m_hUpdateThread )
		{
			Sleep( 0 );
		}
	}

	sc::writeLogInfo( std::string( "== Update Thread stopped." ) );

	CloseAllClient();

	StopListenThread();
	StopWorkThread();
	StopIOCP();

	// 클라이언트 저장 클래스 종료
	SAFE_DELETE( m_pClientManager );
	sc::writeLogInfo( std::string( "== Client Manager closed" ) );
	// Message Queue 종료
	SAFE_DELETE( m_pRecvMsgManager );
	sc::writeLogInfo( std::string( "== Message Manager closed" ) );

	// DB 연결종료
	SAFE_DELETE( m_pDbManager );

	sc::writeLogInfo( std::string( "== Database Manager closed" ) );

#ifndef _DEBUG
	// nProtect GameGuard 종료
	if ( m_bGameGuardInit == true )
	{
#ifndef _NP30
		CleanupGameguardAuth();
#else
		CloseCSAuth3();
#endif
	}
#endif

	// Put message to console
	sc::writeLogInfo( std::string( "Server Stop OK" ) );
	sc::writeLogInfo( std::string( "=======================================================" ) );

	m_nStatus = S_SERVER_STOP;

	return NET_OK;
}

void CFieldServer::MsgUpdateUserCountryAF ( DWORD ClientSlot, NET_MSG_GENERIC* pMsg )
{
	sc::writeLogInfo ( "[ Country Log ] [ Change User Country ( Begin Field Server ) ]" );

	GLMSG::NET_COUNTRY_CHANGE_AFC* pCountryMsg =
		static_cast< GLMSG::NET_COUNTRY_CHANGE_AFC* >( pMsg );

	// Update User Country Data;
	if ( pCountryMsg )
	{
		m_pClientManager->SetCountryInfo (
			ClientSlot,
			pCountryMsg->m_sCountryInfo );

		sc::writeLogInfo ( "[ Country Log ] [ Change User Country ( End Field Server ) ]" );
	}
}

void CFieldServer::TraceHang()
{
	PROFILE_EX_TRACE();
}