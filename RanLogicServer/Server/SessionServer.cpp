#include "../pch.h"
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

#include "../../EngineLib/Common/profile.h"

#include "../../SigmaCore/String/StringFormat.h"
#include "../../SigmaCore/Util/DateTime.h"

#include "../../RanLogic/Msg/TraceMsg.h"
#include "../../RanLogic/Msg/ServerMsg.h"
#include "../../RanLogic/Msg/ServerManagerMsg.h"

#include "../Client/NetSessionClientMan.h"
#include "../../RanLogic/Util/GlobalAuthManager.h"
#include "../Database/DbAction/DbActionUser.h"
#include "../Database/ODBC/s_COdbcManager.h"
#include "../Database/ADO/ADOManager.h"
#include "./SessionServerConfigXml.h"
#include "./SessionServer.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CSessionServer::CSessionServer(HWND hWnd, HWND hEditBox, HWND hEditBoxInfo, SUBPATH* pPath, EMSERVICE_PROVIDER ServiceProvider)
	: NetServer(hWnd, hEditBox, hEditBoxInfo, pPath, ServiceProvider)
	, m_pClientManager(NULL)
	, m_nAuthServerSlot(net::INVALID_SLOT)
{
	// 초기화
	MessageEnumMixer::GetInstance();

	sc::LogMan::getInstance()->attachEditControl(hEditBox, hEditBoxInfo);
	initPacketFunc();

	m_dwLoginServerID = -1;
	for (int nGroup=0; nGroup < MAX_SERVER_GROUP; ++nGroup)
	{
		for (int nChannel=0; nChannel < MAX_CHANNEL_NUMBER; ++nChannel)
		{			
			m_sServerChannel[nGroup][nChannel].nServicePort         = 0;
			m_sServerChannel[nGroup][nChannel].nServerGroup         = 0;
			m_sServerChannel[nGroup][nChannel].nServerNumber        = 0;
			m_sServerChannel[nGroup][nChannel].nServerCurrentClient = 0;
			m_sServerChannel[nGroup][nChannel].nServerMaxClient     = 0;
			m_bServerChannelFull[nGroup][nChannel]					= FALSE;
		}
	}

    m_pConfig = new SessionServerConfigXml;
	m_pAuthManager = new sc::CGlobalAuthClientLogic;
	m_bReconnect = FALSE;
	m_nSessionSvrID = 0;
	m_bConnectedAuthServer = false;


}

CSessionServer::~CSessionServer()
{
	SAFE_DELETE(m_pClientManager);
    SAFE_DELETE(m_pConfig);
	SAFE_DELETE(m_pAuthManager);
}

// 클라이언트 정보저장 클래스 생성
int	CSessionServer::StartClientManager()
{	
	SAFE_DELETE(m_pClientManager);	

	m_pClientManager = new NetSessionClientMan(m_nMaxClient, m_hIOServer, m_pConfig->GetReferenceCountMax());

	if (!m_pClientManager) 
	{
		sc::writeLogError(std::string("StartClientManager m_pClientManager NULL"));
		m_bIsRunning = false;
		return NET_ERROR;
	}
	else
	{
		return NET_OK;
	}
}

// Create DB manager and Open DB
int	CSessionServer::StartDbManager()
{
	int	nRetCode = 0;
	
    m_pDbManager = new COdbcManager(this, db::SESSION_SERVER, m_ServiceProvider, m_pConfig->UseCustomDbPool());
	if (!m_pDbManager)
	{
		sc::writeLogError(std::string("DB:Couldn't get an OdbcManager Instance."));
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
        sc::writeLogError( "OdbcOpenUserDB" );

		m_bIsRunning = false;
		return NET_ERROR;
	}
	else
	{
		sc::writeLogInfo(std::string("ODBC:User DB Open OK"));
	}

	nRetCode = m_pDbManager->OdbcOpenLogDB(
        m_pConfig->GetLogOdbcName(),
        m_pConfig->GetLogOdbcUser(),
        m_pConfig->GetLogOdbcPass(),
        m_pConfig->GetLogOdbcDatabase(),
        m_pConfig->UseCustomDbPool());

	if (nRetCode == sc::db::DB_ERROR)
	{
        sc::writeLogError( "OdbcOpenGameDB" );

		m_bIsRunning = false;
		return NET_ERROR;
	}
	else
	{
		sc::writeLogInfo(std::string("ODBC:Log DB Open OK"));
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

	// GSP User Database (Malaysia)
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
        db::LOG_DB,
        m_pConfig->LogDbSource(),
        m_pConfig->LogDbDatabaseName(),
        m_pConfig->LogDbUser(),
		m_pConfig->LogDbPassword(),
		m_pConfig->GetAdoMaxPoolSize(),
		m_pConfig->GetDatabaseTcpPacketSize());
	
	return NET_OK;
}

int CSessionServer::Start()
{	
	m_bIsRunning = true;

    const std::string XmlFileName("SessionServer.xml");    
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

	if (StartIOCP() == NET_ERROR)
        return NET_ERROR;

	if (StartClientManager() == NET_ERROR)
        return NET_ERROR;
	
	if (StartDbManager() == NET_ERROR)
        return NET_ERROR;	

	if (StartWorkThread(m_pConfig) == NET_ERROR)
        return NET_ERROR;

	if (StartUpdateThread() == NET_ERROR)
        return NET_ERROR;

	if (StartListenThread() == NET_ERROR)
        return NET_ERROR;

	sc::writeLogInfo(
		sc::string::format(
		"FD_SETSIZE %1%",
		FD_SETSIZE));

//#ifdef _USE_AUTH
//	//if (m_ServiceProvider != SP_OFFICE_TEST)
//	//{
//		// 세션서버만 인증서버로 연결한다. 다른 서버들은 세션 서버를 통해서 정보를 송수신
//		if(m_pConfig->GetServerType() == net::SERVER_SESSION )
//		{
//			if (ConnectAuthServer() != NET_OK )
//			{
//				// 기존에 인증서버가 연결이 안되는 경우 30일간은 동작하였지만 더이상 사용못하도록 수정. 
//				// 외부 네트워크 연결이 안되는 경우에는 인증서버 동작을 위해서 오픈해야한다고 통지.
//				//if( CertificationInternalAuth() == false )
//				//{
//					sc::writeLogInfo(std::string("connection fail to Auth"));
//					return NET_ERROR;
//				//}
//			}
//			else
//			{
//				if (SendAuthInfoToAuthServer() == NET_ERROR)
//				{
//					sc::writeLogInfo(std::string("Auth info are failed sending to session server"));
//					m_bIsRunning = false;
//					return NET_ERROR;
//				}
//			
//				sc::writeLogInfo(std::string(">>>> success to connect the Auth"));
//			}
//		}
//	//}
//#endif

	sc::writeLogInfo(std::string(">Server Start OK"));
	sc::writeLogInfo(std::string("======================================================="));

	m_nStatus = S_SERVER_RUNING;

	return NET_OK;
}

int CSessionServer::Stop()
{
	// Stop All Thread and exit
	sc::writeLogInfo(std::string("Please wait until server stop "));

	DWORD dwExitCode = 0;	
	
	lockOn();	
	m_bIsRunning = false;
	lockOff();
	
	if (m_bUseEventThread)	
	{
		::SetEvent(m_hUpdateQuitEvent);
		::WaitForSingleObject(m_hUpdateThread, INFINITE);
		SAFE_CLOSE_HANDLE(m_hUpdateQuitEvent);
	}
    else
    {
		Sleep(1000);
	}

	StopListenThread();	
	StopWorkThread();
	StopIOCP();

	CloseAllClient(); 	// close all client connections

	// 클라이언트 저장 클래스 종료
	SAFE_DELETE(m_pClientManager);
	// Message Queue 종료	
	SAFE_DELETE(m_pRecvMsgManager);
	sc::writeLogInfo(std::string("Stop Message Queue"));
	
    // DB 연결종료	
    SAFE_DELETE(m_pDbManager);
	
	sc::writeLogInfo(std::string("Stop DataBase"));
	// CFG class 종료
	//m_pConfig->ReleaseInstance();
	sc::writeLogInfo(std::string("Stop CFG"));
	// Put message to console
	sc::writeLogInfo(std::string("Server Stop OK"));
	sc::writeLogInfo(std::string("======================================================="));
	
	m_nStatus = S_SERVER_STOP;

	return NET_OK;
}

void CSessionServer::ForceStop( DWORD Reason )
{
	sc::writeLogInfo( std::string( "== Please wait until server stop(Force)" ) );
}

/*
int	CSessionServer::Pause()
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

int CSessionServer::Resume()
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

int	CSessionServer::ReStart()
{
	Stop();
	Start();
	return NET_OK;
}
*/

void CSessionServer::CloseAllClient()
{
	for (DWORD dwNum=0; dwNum<(DWORD) m_nMaxClient; ++dwNum)
	{
		CloseClient(dwNum);
	}
}

int CSessionServer::InsertMsg(int nClient, void* pMsg)
{
    NET_MSG_GENERIC* pPacket = (NET_MSG_GENERIC*) pMsg;
    m_pRecvMsgManager->MsgQueueInsert(nClient, pPacket, pPacket->Size());
    return NET_OK;
}

// 클라이언트의 연결을 해제한다.
void CSessionServer::CloseClient(DWORD dwClient)
{
    NET_CLOSE_CLIENT_SESSION InnerMsg(dwClient);
    InsertMsg(DBACTION_CLIENT_ID, &InnerMsg);

	// 종료된 서버가 인증 서버라면 재접속 시도
	if ( dwClient == m_nAuthServerSlot )
	{
		m_bReconnect = TRUE;
		sc::writeLogInfo( std::string( "Try to reconnect the Auth Server" ) );
	}
}

void CSessionServer::MsgCloseClient(DWORD dwClient, MSG_LIST* pMsg)
{
    if ( dwClient != 0 )
        return;

    NET_CLOSE_CLIENT_SESSION* pPacket = ( NET_CLOSE_CLIENT_SESSION* ) pMsg->Buffer;
    DWORD ClientSlot = pPacket->ClientSlot();    

	if ( INVALID_SOCKET != m_pClientManager->GetSocket( ClientSlot ) )
	{
		G_SERVER_INFO gsi = m_pClientManager->GetSvrInfo( ClientSlot );

		sc::writeLogInfo(
        sc::string::format(
		"종료 서버 정보 : IP[%s] Port[%d] SvrType[%d]]",
			gsi.szServerIP,
			gsi.nServicePort,
           gsi.ServerType));

		if ( 
			m_pClientManager->GetSvrType( ClientSlot ) == net::SERVER_AGENT ||
			m_pClientManager->GetSvrType( ClientSlot ) == net::SERVER_FIELD ||
			m_pClientManager->GetSvrType( ClientSlot ) == net::SERVER_LOGIN ||
			m_pClientManager->GetSvrType( ClientSlot ) == net::SERVER_CACHE || 
			m_pClientManager->GetSvrType( ClientSlot ) == net::SERVER_INTEGRATION_CACHE )
		{
			for ( int i=1; i<m_nMaxClient; ++i )
			{
				// server manager 에 알린다.
				// 추가 : Auth 서버도 ServerType이 SERVER_UNKNOWN으로 설정되어 있으므로 m_nAuthServerSlot과 일치하는 경우에는 전송하지 않음.
				if ( INVALID_SOCKET != m_pClientManager->GetSocket( i ) && ( m_pClientManager->GetSvrType( i ) == net::SERVER_UNKNOWN && m_nAuthServerSlot != i ) )
				{
					NET_SERVER_INFO nsi( gsi );
					SendClient( i, &nsi );

					sc::writeLogError( 
						sc::string::format( 
						"Server closed ip: %1% group: %2% type %3% slot %4%", 
						gsi.szServerIP, 
						gsi.nServerGroup, 
						( int ) gsi.ServerType,
						i ) );

				}
			}
		}

		if ( m_pClientManager->IsGameServer( ClientSlot ) == true ) // Agent 서버라면
		{
			// 사용자리스트를 삭제한다.
			m_pClientManager->UserEraseServer( m_pClientManager->GetSvrGrp( ClientSlot ),
				m_pClientManager->GetSvrNum( ClientSlot ) );
			
			// 사용자를 로그아웃 시킨다.
			m_pDbManager->AddLogJob(
				db::DbActionPtr(
				new CUserLogoutSvr(
				m_pClientManager->GetSvrGrp( ClientSlot ), 
				m_pClientManager->GetSvrNum( ClientSlot ) ) ) );
		}

		//if ( m_ServiceProvider == SP_OFFICE_TEST || m_ServiceProvider == SP_TAIWAN ||  m_ServiceProvider == SP_HONGKONG ||  m_ServiceProvider == SP_THAILAND || m_ServiceProvider == SP_KOREA || m_ServiceProvider == SP_KOREA_TEST) 
		//{
			if ( 
				m_vecTracingData.size() != 0 && 
				m_pClientManager->GetSvrType( ClientSlot ) != net::SERVER_AGENT &&
				m_pClientManager->GetSvrType( ClientSlot ) != net::SERVER_FIELD )
			{
				for ( int i=0; i<m_pClientManager->GetMaxClient(); ++i )
				{
					// 서버메니져가 CloseClient되면 초기화한다.
					if (  INVALID_SOCKET != m_pClientManager->GetSocket( i ) )
					{
						GLMSG::NET_UPDATE_TRACINGCHAR_ALL msg;	
						SendClient( i, &msg );

						sc::writeLogInfo( std::string( "INFO:Tracing User All Clear" ) );
					}
				}
			}
		//}

		// 서버상태 재전송
		MsgSndCurSvrInfoToAll();
	}

	int nResult = m_pClientManager->CloseClient( ClientSlot );
}

void CSessionServer::Send(DWORD dwClient, PER_IO_OPERATION_DATA* PerIoData, DWORD dwSize)
{
}

void CSessionServer::UserInsert(const CString& strUsrID, LPCNT_USR_INFO pData)
{
    if (pData)
		m_pClientManager->UserInsert(strUsrID, pData);
}

int CSessionServer::SendClient( DWORD dwClient, NET_MSG_GENERIC* pMsg )
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

void CSessionServer::SendAllClient(NET_MSG_GENERIC* pMsg)
{
    if (pMsg)
    {        
        int MaxClient = m_pClientManager->GetMaxClient();
        for (int i=0; i<MaxClient; ++i)
        {        
            //if (m_pClientManager->IsOnline(i))
			if (m_pClientManager->IsOnline(i) && i != m_nAuthServerSlot) // 모든 클라이언트에 인증서버는 제외
                SendClient(i, pMsg);
        }
    }
}

int CSessionServer::ExecuteCommand(char* strCmd)
{
    if (!strCmd)
        return NET_ERROR;

    std::string strCommand(strCmd);
    std::vector<std::string> SplitVec;
    boost::split(SplitVec, strCommand, boost::algorithm::is_any_of(" "));
    if (SplitVec.empty())
        return NET_ERROR;

	// Request Server Info
	// command : req svrfullinfo
	if (SplitVec[0] == "req") 
	{		
		if (SplitVec.size() > 1) 
		{
			// 서버 정보 요청
			if (SplitVec[1] == "svrfullinfo") 
				MsgReqAllSvrFullInfo();
			if (SplitVec[1] == "svrcurinfo") 
				MsgReqAllSvrCurInfo();
		}
	} 
	else if (SplitVec[0] == "print_ip")
	{
		// SERVER_UTIL::CAddressChecker::GetInstance()->Save();
		return 0;
	}
	else if (SplitVec[0] == "print_user")
	{
		// 사용자 리스트 출력		
		if (SplitVec.size() > 1)
		{
			if (SplitVec[1] == "user")
				m_pClientManager->PrintUsrList();
		}
	}
	else if (SplitVec[0] == "chat")
	{
		// 채팅메시지		
		if (SplitVec.size() > 1)
		{
			// 시스템 메시지
			if (SplitVec[1] == "system")
			{
				char str[] = "chat system ";
				char* pdest;
				int  result;
				pdest = strstr(strCmd, str);
				result = (int) (pdest - strCmd + 1);
				if (pdest)
					MsgSndChatGlobal(pdest);
			}
			// 특정사용자에게 보내는 메시지
			if (SplitVec[1] == "user")
			{
			}	
		}
	}	
	else if (SplitVec[0] == "profile_on")
	{
        // PROFILE 사용.
		g_bProfile = TRUE;
		sc::writeLogInfo(std::string("PROFILE ON"));
	}	
	else if (SplitVec[0] == "profile_off")
	{
        // PROFILE 사용안함.
		g_bProfile = FALSE;
		sc::writeLogInfo(std::string("PROFILE OFF"));
	}
    else
    {	    
    }
    return 0;
}

void CSessionServer::MsgLoginMaintenance( DWORD dwClient, MSG_LIST* pMsg )
{
	SNET_SERVER_LOGINMAINTENANCE_SESSION* pnsll = 
		reinterpret_cast < SNET_SERVER_LOGINMAINTENANCE_SESSION* > (pMsg->Buffer);

	int bOn = (int)pnsll->bOn;
		
	sc::writeLogInfo( sc::string::format("Login Maintenance %d", bOn) );

	SNET_SERVER_LOGINMAINTENANCE_LOGIN nsll;
	nsll.bOn = pnsll->bOn;	

	SendClient(m_dwLoginServerID, &nsll);
}


void CSessionServer::MsgServerMansgerMsg( DWORD dwClient, MSG_LIST* pMsg )
{
    NET_MSG_GENERIC* msg = reinterpret_cast<NET_MSG_GENERIC*>(pMsg->Buffer);

	
    switch( msg->nType )
    {
    case NET_MSG_INSPECTION_REWARD_SET_REQ:
		{
			GLMSG::NET_INSPECTION_REWARD_SET_REQ* NetMsg = (GLMSG::NET_INSPECTION_REWARD_SET_REQ*)msg;
			DWORD dwCacheSlot = m_pClientManager->FindServer(net::SERVER_CACHE, NetMsg->nServerGroup, 0, 0);

			sc::writeLogInfo( sc::string::format("Inspection Reward ServerManaget -> Session IP:[%s] ", m_pClientManager->GetClientIP( dwClient ) ));
			sc::writeLogInfo( sc::string::format("Inspection Reward Server Group:[%d] ", NetMsg->nServerGroup ) );
			sc::writeLogInfo( sc::string::format("Inspection Reward Start Time:[%s] ", sc::time::DateTimeFormat(NetMsg->tStart, true, true) ));
			sc::writeLogInfo( sc::string::format("Inspection Reward End   Time:[%s] ", sc::time::DateTimeFormat(NetMsg->tEnd, true, true) ));
			sc::writeLogInfo( sc::string::format("Inspection Reward Minte:[%d] ", NetMsg->nRewardTime ) );

			SendClient(dwCacheSlot, NetMsg);
		}
        break;
	case NET_MSG_INSPECTION_REWARD_SET_ACK:
		{
			GLMSG::NET_INSPECTION_REWARD_SET_ACK* NetMsg = (GLMSG::NET_INSPECTION_REWARD_SET_ACK*)msg;

			sc::writeLogInfo( sc::string::format("Inspection Reward Cache -> Session Code:[%d] (code %d == OK) ", NetMsg->nResult, GLMSG::NET_INSPECTION_REWARD_SET_ACK::EM_ACK_OK ));
		}
    }




}
