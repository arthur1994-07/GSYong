#include "../pch.h"
#include "../../EngineLib/Common/profile.h"
#include "../../RanLogic/Msg/ServerMsg.h"
#include "../Client/NetLoginClientMan.h"
#include "./NetServerHelper.h"
#include "./LoginServerConfigXml.h"
#include "./LoginServer.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

int CLoginServer::SessionConnect()
{
	return SessionConnect(m_pConfig->GetSessionServerIP(), m_pConfig->GetSessionServerPort());
}

int	CLoginServer::SessionConnect(const char* szServerIP, int nPort)
{
    if ( m_SessionServerSlot != net::INVALID_SLOT &&
        INVALID_SOCKET != m_pClientManager->GetSocket( m_SessionServerSlot ) )
    {	
        sc::writeLogInfo( std::string( "Session is online" ) );
        return NET_OK;
    }

    int SessionSlot = m_pClientManager->GetFreeClientID( true );
	if ( SessionSlot == NET_ERROR )
	{
		sc::writeLogError( std::string( "SessionConnect Not enough free network slot" ) );
		return NET_ERROR;
	}

    net::CONNECT_RESULT Connect = net::ConnectAndReceive(
        szServerIP,
        nPort,
        m_hIOServer,
        SessionSlot );
    int Result = Connect.get<0> ();
    SOCKET ConnectSocket = Connect.get<1> ();
    if ( Result == NET_ERROR )
    {
        m_pClientManager->ReleaseClientID( SessionSlot );
        sc::writeLogError( std::string( "Session Server Connection Failed" ) );
        return NET_ERROR;
    }  

	if ( NET_ERROR == m_pClientManager->SetAcceptedClient( SessionSlot, ConnectSocket, net::SLOT_SESSION ) )
	{
		::closesocket( ConnectSocket );
		m_pClientManager->ReleaseClientID( SessionSlot );
		sc::writeLogError( std::string( "SessionConnect SetAcceptedClient failed" ) );
		return NET_ERROR;
	}

    m_SessionServerSlot = SessionSlot;
    m_pClientManager->SetSlotType( m_SessionServerSlot, net::SLOT_SESSION );

	PER_IO_OPERATION_DATA* pIOData = m_pClientManager->GetFreeOverIO( NET_RECV_POSTED );
	if ( NULL == pIOData )
	{
		CloseClient( SessionSlot );
		sc::writeLogError( std::string( "SessionConnect GetFreeOverIO failed" ) );
		return NET_ERROR;
	}

	// add reference
	m_pClientManager->AddReference( SessionSlot );

	DWORD Flags = 0;
	DWORD dwRcvBytes = sizeof( NET_MSG_GENERIC );
	int nRetCode = WSARecv( ConnectSocket, &( pIOData->DataBuf ), 1, &dwRcvBytes, &Flags, &( pIOData->Overlapped ), NULL );
	if ( nRetCode == SOCKET_ERROR && ( WSAGetLastError() != ERROR_IO_PENDING ) )
	{
		// release reference
		m_pClientManager->ReleaseReference( SessionSlot );
		m_pClientManager->ReleaseOperationData( pIOData );
		CloseClient( SessionSlot );
		sc::writeLogError( sc::string::format( "SessionConnect WSARecv %1%", WSAGetLastError() ) );
		return NET_ERROR;
	}

    // Head receive mode
    //m_pClientManager->SetNetMode(m_SessionServerSlot, NET_PACKET_HEAD);
    
    SessionReqSvrInfo(); // 서버리스트를 요청한다.
    SessionSndSvrInfo(); // 서버정보를 전송한다.
    SessionSndKey();	 // 키값을 전송한다.
    SessionSndVersion();

    sc::writeLogInfo(
        sc::string::format( "Session Server Connection OK. slot %1%", m_SessionServerSlot ) );

    return NET_OK;
}

void CLoginServer::SessionCloseConnect(void)
{
    if (m_SessionServerSlot != net::INVALID_SLOT)
	    CloseClient(m_SessionServerSlot);
}

// Session Server message process
void CLoginServer::SessionMsgProcess(MSG_LIST* pMsg)
{
	if (!pMsg)
        return;

	NET_MSG_GENERIC* nmg = (NET_MSG_GENERIC*) pMsg->Buffer;
	switch ( nmg->nType )
	{
	case NET_MSG_HEARTBEAT_SERVER_REQ: // Session->Login:Heartbeat check request
		SessionMsgHeartbeatServerAns();
		break;
	case NET_MSG_HEARTBEAT_SERVER_ANS: // Session->Login:Heartbeat check answer
#ifdef HEARTBEAT_LOG
		sc::writeLogInfo(sc::string::format("[HEARTBEAT] Recv Heartbeat Response from Session Svr ( %1% )", pMsg->dwClient));
#endif
		m_pClientManager->SetHeartBeat( pMsg->dwClient );
		break;
	// 세션서버->로그인서버 : 게임서버의 상태정보를 전송한다.
	case NET_MSG_SND_SVR_INFO :
		SessionMsgSvrInfo(nmg);
		break;
	// 세션서버->로그인서버 : 게임서버의 전체정보를 요청한다.
	case NET_MSG_REQ_FULL_SVR_INFO :
        SessionSndSvrInfo();
		break;
	case NET_MSG_SND_SVR_INFO_RESET :
		SessionMsgSvrInfoReset(nmg);
		break;
	case NET_MSG_SVR_CMD:
		SessionMsgSvrCmd(nmg);
		break;
	// Channel Full 상태가 바뀌었을 때
	case NET_MSG_SND_CHANNEL_FULL_STATE:
		SessionSndSvrChannelFullInfo(nmg);
		break;

	case NET_MSG_LOGIN_MAINTENANCE_LOGIN:
		SessionSndSvrLoginMaintenance(nmg);
		break;

    case NET_MSG_VERSION_SS:
        SessionMsgVersionSS(nmg);
        break;

    case NET_MSG_PS_RELOAD_SS:
    case NET_MSG_GM_LOG_ITEM_RELOAD_SS:
    case NET_MSG_GM_GAME_DATA_UPDATE_SS:
        DoNothing(nmg);
        break;
	
	case NET_MSG_AUTH_CERTIFICATION_ANS:
		MsgAuthCertificationResponse(nmg);
		break;

	case NET_MSG_COUNTRY_ACCESS_APPROVE:
		MsgCountryAccessApprove(nmg);
		break;
	default:
		break;
	}
}

void CLoginServer::SessionMsgHeartbeatServerAns()
{
#ifdef HEARTBEAT_LOG
	sc::writeLogInfo(std::string("[HEARTBEAT] Recv Heartbeat Request from Session Svr"));
#endif

	NET_HEARTBEAT_SERVER_ANS MsgHeartbeatServerAns;
    SendSession(&MsgHeartbeatServerAns);

#ifdef HEARTBEAT_LOG
	sc::writeLogInfo(std::string("[HEARTBEAT] Send Heartbeat Response to Session Svr"));
#endif
}

void CLoginServer::SessionMsgSvrCmd(NET_MSG_GENERIC* nmg)
{
	if (nmg == NULL) return;

	NET_SVR_CMD_CHANGE_STATE* pTemp = reinterpret_cast <NET_SVR_CMD_CHANGE_STATE*> (nmg);

	sc::writeLogInfo(std::string("INFO:Received Server Command From Session Server"));

	switch (pTemp->nCmdType)
	{
		/*
	case NET_MSG_SVR_PAUSE: // Server pause message
		sc::writeLogInfo(std::string("INFO:Server Will Pause"));
		Pause();
		break;
	case NET_MSG_SVR_RESUME: // Server resume message
		sc::writeLogInfo(std::string("INFO:Server Will Resume"));
		Resume();
		break;
		*/
	case NET_MSG_SVR_RESTART: // Server restart message
		sc::writeLogInfo(std::string("INFO:Server Will Restart"));
		Stop();
		Start();
		break;
	default:
		break;
	}
}

int CLoginServer::SendSession(NET_MSG_GENERIC* pBuffer)
{
    if ( pBuffer && m_SessionServerSlot != net::INVALID_SLOT )
    {
        PROFILE_MSG_COUNT_SEND( pBuffer );
        return m_pClientManager->SendClient2( m_SessionServerSlot, pBuffer );
    }
    else
    {
		sc::writeLogError( std::string( "SendSession buffer is null or session slot is invalid." ) );
        return NET_ERROR;
    }
}

void CLoginServer::SessionMsgSvrInfoReset(NET_MSG_GENERIC* nmg)
{
	if (nmg == NULL) return;

	int i, j;

	for (i=0; i<MAX_SERVER_GROUP; i++)
	{
		for (j=0; j<MAX_SERVER_NUMBER; j++)
		{
			SecureZeroMemory(&m_sGame[i][j], sizeof(G_SERVER_CUR_INFO_LOGIN));
		}
	}
}

/*
void CLoginServer::SessionSndHeartbeat()
{
    NET_MSG_GENERIC nmg;
    nmg.dwSize = sizeof(NET_MSG_GENERIC);
    nmg.nType = NET_MSG_HEARTBEAT_SVR;
    SendSession(&nmg);
}
*/

// 세션서버에서 넘어온 게임서버 정보를 세팅한다.
void CLoginServer::SessionMsgSvrInfo(NET_MSG_GENERIC* nmg)
{
	if (!nmg)
        return;
	
	NET_CUR_INFO_LOGIN* pPacket = reinterpret_cast<NET_CUR_INFO_LOGIN*> (nmg);		
	if ((pPacket->gscil.nServerGroup >= MAX_SERVER_GROUP) || (pPacket->gscil.nServerNumber >= MAX_SERVER_NUMBER))
	{
		return;
	}
	else
	{
		//lockOn();
		lockOn();
		m_sGame[pPacket->gscil.nServerGroup][pPacket->gscil.nServerNumber] = pPacket->gscil;
		/*
		CConsoleMessage::GetInstance()->WriteInfo(
			_T("SessionMsgSvrInfo %s"), 
			m_sGame[ncil->gscil.nServerGroup][ncil->gscil.nServerNumber].szServerIP );
		*/

		sc::writeInfo(sc::string::format("SessionMsgSvrInfo %s",
			m_sGame[pPacket->gscil.nServerGroup][pPacket->gscil.nServerNumber].szServerIP ));

		lockOff();
		//lockOff();
	}
}

void CLoginServer::SessionSndSvrChannelFullInfo(NET_MSG_GENERIC* nmg)
{
	NET_SERVER_CHANNEL_FULL_INFO* ncil = reinterpret_cast<NET_SERVER_CHANNEL_FULL_INFO*> (nmg);		
	
	//lockOn();
	lockOn();
	m_sGame[ncil->nServerGroup][ncil->nChannel].nServerCurrentClient = ncil->nChannelCurrentUser;

	if( ncil->bChannelFull )
	{		
		sc::writeLogWarn( sc::string::format("Group %d Channel %d is Full", ncil->nServerGroup, ncil->nChannel) );
	}
	else
	{		
		sc::writeLogInfo( sc::string::format("Group %d Channel %d is not Full", ncil->nServerGroup, ncil->nChannel) );
	}
	lockOff();
	//lockOff();
}

void CLoginServer::SessionSndSvrLoginMaintenance( NET_MSG_GENERIC* nmg )
{
	SNET_SERVER_LOGINMAINTENANCE_LOGIN* nsll = reinterpret_cast< SNET_SERVER_LOGINMAINTENANCE_LOGIN* > ( nmg );
	
    bool bOK = nsll->bOn;
	if( bOK )
	{
		//
		//mjeon
		//Turn off checking "Allow IP list".
		//
		SetAllowIP( false );
		sc::writeLogInfo(std::string("LoginMaintenance AllowIP : False"));
	}
	else
	{
		//
		//mjeon
		//Turn on checking "Allow IP list".
		//
		SetAllowIP( true );
		sc::writeLogInfo(std::string("LoginMaintenance AllowIP : True"));
	}
}

// Session Server 에 Game 서버의 현재상태를 요청한다.
void CLoginServer::SessionReqSvrInfo(void)
{
	NET_MSG_GENERIC nmg;
	nmg.SetSize( sizeof(NET_MSG_GENERIC) );	
	nmg.nType = NET_MSG_REQ_SVR_INFO;
	SendSession(&nmg);
}

void CLoginServer::SessionSndKey(void)
{
	NET_ENCRYPT_KEY nmg;
	::StringCchCopy(nmg.szEncryptKey, ENCRYPT_KEY+1, m_szEncrypt );
	SendSession(&nmg);
}

void CLoginServer::SessionSndVersion()
{
    NET_VERSION_LS Msg(m_LauncherVersion, m_PatchVersion);
    SendSession(&Msg);
    sc::writeLogInfo(sc::string::format("Launcher Version %1%", m_LauncherVersion));
    sc::writeLogInfo(sc::string::format("Patch Version %1%", m_PatchVersion));
    sc::writeLogInfo(sc::string::format("Patch Cut Version %1%", m_PatchCutVersion));
}

void CLoginServer::SessionSndSvrInfo(void)
{
    G_SERVER_INFO ServerInfo;
    ServerInfo.nServerCurrentClient	= m_pClientManager->GetCurrentClientNumber();
    ServerInfo.nServerGroup			= m_nServerGroup;	//was set by CCfg.
    ServerInfo.nServerChannel       = m_pConfig->GetServerChannel();
    ServerInfo.nServerMaxClient		= m_pClientManager->GetMaxClient();	
    ServerInfo.nServicePort			= m_nPort;
    ServerInfo.ServerType			= net::SERVER_LOGIN;
    ServerInfo.SetServerIp(m_szAddress);

    NET_SERVER_INFO nsi(ServerInfo);
	nsi.SetType(NET_MSG_SND_FULL_SVR_INFO);

	SendSession(&nsi);
}

int CLoginServer::SessionConnectSndSvrInfo()
{
	return SessionConnect(m_pConfig->GetSessionServerIP(), m_pConfig->GetSessionServerPort());
}

void CLoginServer::SessionMsgVersionSS(NET_MSG_GENERIC* nmg)
{
    NET_VERSION_SS* pPacket = (NET_VERSION_SS*) nmg;
    sc::writeLogInfo(sc::string::format("Session server Launcher version %1%", pPacket->m_LauncherVersion));
    sc::writeLogInfo(sc::string::format("Session server Patch version %1%", pPacket->m_PatchVersion));
}

//                              _-`.
//                           ,-"` |-`.
//                           |   | _-|
//                           |\  | _-|
//                          | \\|   |
//                          |  `| _-|
//                         |   | _-|
//                        |   |   |
//            ,===========;...'.-|
//          ,;||||||||||||||||\ |
//         /\""""""""/"\"`-.||||
//        //\\------/ /\\. `\||||
//       //  \       /  \ `. `.||
//      //  dP;     /   ,\  ` .\|.-.
//  ,-'' --^,'     /__  d#\   |;/   |
// |   _   Y          "-^;#\  `i   /
//  `. \"-.;      )               `.
//     \\ | `--""""""""""""`.      |
//      `:. |  |   |  |  |  |    _/
//        `:.  |   |  |  ' ,'_,-'
//          `:-._      _.-','
//            `--_"""""_.-'  
//                """""
