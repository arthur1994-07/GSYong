#include "../pch.h"
#include "../../Enginelib/Common/profile.h"
#include "../../Enginelib/DxTools/DebugSet.h"
#include "../../RanLogic/Msg/TraceMsg.h"
#include "../../RanLogic/Msg/ServerMsg.h"

#include "../Client/NetAgentClientMan.h"
#include "./AgentServerConfigXml.h"
#include "./NetServerHelper.h"
#include "./AgentServer.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

//! Session Server message process
void CAgentServer::SessionMsgProcess(MSG_LIST* pMsg)
{	
    if (!pMsg)
        return;

    NET_MSG_GENERIC* pPacket = reinterpret_cast<NET_MSG_GENERIC*> (pMsg->Buffer);

    switch (pPacket->nType)
    {
    case NET_MSG_HEARTBEAT_SERVER_REQ: // Session->Login:Heartbeat check request
        SessionMsgHeartbeatServerAns();
        break;
    case NET_MSG_HEARTBEAT_SERVER_ANS:
#ifdef HEARTBEAT_LOG
		sc::writeLogInfo(sc::string::format("[HEARTBEAT] Recv Heartbeat Response from Session Svr ( SessionSlot %1%, SenderSlot %2% )", m_SessionServerSlot, pMsg->dwClient));
#endif

        m_pClientManager->SetHeartBeat(m_SessionServerSlot);
        break;
    case NET_MSG_REQ_FULL_SVR_INFO:
        SessionSndSvrInfo();
        break;
    case NET_MSG_REQ_CUR_STATE:
        SessionSndSvrCurState();
        break;
    case NET_MSG_CHAT_CTRL:
        SessionMsgChatProcess(pPacket);
        break;
    case NET_MSG_RECONNECT_FIELD:
        sc::writeLogInfo(std::string("Reconnect to field server"));
        FieldConnectAll();
        break;
	case NET_MSG_RECONNECT_MATCH:
        sc::writeLogInfo(std::string("Reconnect to match server"));
        MatchConnect();
        break;
    case NET_MSG_SVR_CMD:
        SessionMsgSvrCmd(pPacket);
        break;	
    case NET_MSG_UPDATE_TRACING_ALL:
        UpdateTracingCharacter(pPacket);
        break;
    case NET_MSG_SND_ENCRYPT_KEY:
        SessionMsgEncryptKey(pPacket);
        break;
    case NET_MSG_VERSION_SS:
        SessionMsgVersionSS(pPacket);
        break;
    case NET_MSG_PS_RELOAD_SS:
        DoNothing(pPacket);
        break;
    case NET_MSG_GM_LOG_ITEM_RELOAD_SS:
        if (m_pGLAgentServer)
            m_pGLAgentServer->ReloadLogItem();
        break;
    case NET_MSG_GM_GAME_DATA_UPDATE_SS:
        MsgGmGameDataUpdateSS( pPacket );
        break;
    case NET_MSG_EVENT_EXP_SA:
        if (m_pGLAgentServer)
            m_pGLAgentServer->EventExpSA(pPacket);
        break;
    case NET_MSG_EVENT_EXP_END_SA:
        if (m_pGLAgentServer)
            m_pGLAgentServer->EventExpEndSA(pPacket);
        break;
	case NET_MSG_AUTH_CERTIFICATION_ANS:
		MsgAuthCertificationResponse(pPacket);
		break;
	case NET_MSG_AUTH_EVENT_INFO_REQ:
		 MsgAuthEventInfoRequest(pPacket);
		break;
    default:
        sc::writeLogError(sc::string::format("SessionMsgProcess unknown message type %1%", pPacket->nType));
        break;
    }
}

BOOL CAgentServer::UpdateTracingCharacter(NET_MSG_GENERIC* nmg )
{
	//
	//mjeon.STracingData에 std::string이 포함되어 있어서 Release 버전과 Debug 버전 사이에서 문제가 된다. 차 후, 이 메시지에서 std::string을 들어내야한다.
	//
#ifdef _DEBUG
	return TRUE;
#endif

	GLMSG::NET_UPDATE_TRACINGCHAR_ALL* pPacket = (GLMSG::NET_UPDATE_TRACINGCHAR_ALL*) nmg;
    if (!pPacket)
        return FALSE;

    size_t nPreSize = m_vecTracingData.size();

    m_vecTracingData.clear();
    int i, nTracing;
    for( i = 0; i < MAX_TRACING_NUM; i++ )
    {        
        std::string AccountName(pPacket->tracingData[i].m_szAccount);        

		if (pPacket->tracingData[i].dwUserNum == 0 && AccountName.empty() )
			break;

        m_vecTracingData.push_back(pPacket->tracingData[i]);
    }
    size_t nPostSize = m_vecTracingData.size();

    // 추가 되었을 경우
    int iMAX_CLIENT = m_pClientManager->GetMaxClient();
    if( nPreSize < nPostSize )
    {	
        GLMSG::NET_UPDATE_TRACINGCHAR msg;		
        DWORD dwGaeaID;
        for (DWORD i=1; i<(DWORD)iMAX_CLIENT; ++i)
        {
            if (m_pClientManager->IsOnline((DWORD) i) && m_pClientManager->IsClientSlot((DWORD) i))
            {
                dwGaeaID = m_pClientManager->GetGaeaID((DWORD) i);
                GLCharAG* pChar = m_pGLAgentServer->GetChar(dwGaeaID);
                if ( pChar && pChar->m_bTracingUser == FALSE )
                {
                    nTracing = CheckTracingUser( pChar->m_szUserName, pChar->m_UserDbNum );
                    if( nTracing != -1 )
                    {
                        m_vecTracingData[nTracing].dwOnLine       = TRUE;
                        m_vecTracingData[nTracing].dwUserNum      = pChar->m_UserDbNum;
                        m_vecTracingData[nTracing].nConnectServer = m_nServerGroup;
                        m_vecTracingData[nTracing].SetAccount(pChar->m_szUserName);

                        msg.tracingData = m_vecTracingData[nTracing];
                        msg.updateNum   = nTracing;

                        SendSession(&msg);

                        pChar->m_bTracingUser = TRUE;

                        CDebugSet::ToTracingFile(
                            pChar->m_szUserName,
                            "Tracing User Already Login..., [%s][%s], UserNum:[%u], IP:[%s]", 
                            pChar->m_szUserName,
                            pChar->m_szName,
                            pChar->m_UserDbNum,
                            pChar->m_szIp);
                    }
                }
            }
        }		
        // 삭제 되었을 경우
    }
    else if (nPreSize > nPostSize)
    {
        DWORD dwGaeaID;
        for (DWORD i=1; i<(DWORD)iMAX_CLIENT; ++i)
        {
            if (m_pClientManager->IsOnline((DWORD) i) && m_pClientManager->IsClientSlot((DWORD) i))
            {
                dwGaeaID = m_pClientManager->GetGaeaID((DWORD) i);
                GLCharAG* pChar = m_pGLAgentServer->GetChar(dwGaeaID);
                if (pChar && pChar->m_bTracingUser)
                {
                    nTracing = CheckTracingUser(pChar->m_szUserName, pChar->m_UserDbNum);
                    if (nTracing == -1)
                    {
                        pChar->m_bTracingUser = FALSE;
                        CDebugSet::ToTracingFile(
                            pChar->m_szUserName,
                            "Delete Tracing User List, %1% %2% UserDbNum %3% IP %4%",
                            pChar->m_szUserName,
                            pChar->m_szName,
                            pChar->m_UserDbNum,
                            pChar->m_szIp);
                    }
                }
            }
        }
    }
    return TRUE;
}

void CAgentServer::SessionMsgHeartbeatServerAns()
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

void CAgentServer::SessionMsgSvrCmd(NET_MSG_GENERIC* nmg)
{
    /*
	NET_SVR_CMD_CHANGE_STATE* pTemp = reinterpret_cast <NET_SVR_CMD_CHANGE_STATE*> (nmg);
	sc::writeLogInfo(std::string("INFO:Received Server Command From Session Server"));
	switch (pTemp->nCmdType)
	{		
	//case NET_MSG_SVR_PAUSE: // Server pause message
	//	sc::writeLogInfo(std::string("INFO:Server Will Pause"));
	//	Pause();
	//	break;
	//case NET_MSG_SVR_RESUME: // Server resume message
	//	sc::writeLogInfo(std::string("INFO:Server Will Resume"));
	//	Resume();
	//	break;
	case NET_MSG_SVR_RESTART: // Server restart message
		sc::writeLogInfo(std::string("INFO:Server Will Restart"));
		Stop();
		Start();
		break;
	default:
		break;
	}
    */
}

void CAgentServer::SessionMsgEncryptKey(NET_MSG_GENERIC* pMsg)
{
	if (pMsg == NULL) return;

	NET_ENCRYPT_KEY* nmg = (NET_ENCRYPT_KEY*) pMsg;

	::StringCchCopy( m_szEncrypt, ENCRYPT_KEY+1, nmg->szEncryptKey );
}

int	CAgentServer::SendSession( NET_MSG_GENERIC* pMsg )
{
    if ( pMsg && m_SessionServerSlot != net::INVALID_SLOT )
    {
        PROFILE_MSG_COUNT_SEND( pMsg );
        return m_pClientManager->SendClient2( m_SessionServerSlot, pMsg );
    }
    else
    {
		sc::writeLogError( std::string( "SendSession buffer is null or session slot is invalid." ) );
        return NET_ERROR;
    }
}

// 세션서버에서 오는 채팅메시지 처리
void CAgentServer::SessionMsgChatProcess(NET_MSG_GENERIC* nmg)
{   
    if (nmg == NULL) return;
	NET_CHAT_CTRL* ncc;
	ncc = reinterpret_cast<NET_CHAT_CTRL*> (nmg);

	switch (ncc->emType) 
	{
	case CHAT_TYPE_GLOBAL: // 전체 공지 메시지
		sc::writeLogInfo(std::string("Sending system message to all clients"));		
		MsgSndChatGlobal(ncc->szChatMsg);
		break;
	default:
		break;
	}	
}

//! Session 서버에 접속하고 서버의 정보를 전송한다.
int CAgentServer::SessionConnectSndSvrInfo()
{
	return SessionConnectSndSvrInfo( m_pConfig->GetSessionServerIP(), m_pConfig->GetSessionServerPort() );
}

//! Session 서버에 접속하고 서버의 정보를 전송한다.
int CAgentServer::SessionConnectSndSvrInfo( const char* szServerIP, int nPort )
{
	if ( SessionConnect( szServerIP, nPort ) == NET_ERROR )
		return NET_ERROR;
	else
		return SessionSndSvrInfo();
}

// Agent Server -> Session Server 
// Send server info to session server
int CAgentServer::SessionSndSvrInfo(void)
{
    G_SERVER_INFO ServerInfo;

    ServerInfo.nControlPort			   = m_pConfig->GetControlPort();
    ServerInfo.nServerCurrentClient	   = m_pClientManager->GetCurrentClientNumber();
    ServerInfo.nServerGroup			   = m_pConfig->GetServerGroup();
    ServerInfo.nServerMaxClient		   = m_pConfig->GetServerMaxClient();
    ServerInfo.nServerNumber		   = m_pConfig->GetServerNumber();
    ServerInfo.ServerType			   = m_pConfig->GetServerType();	
    ServerInfo.nServicePort		       = m_pConfig->GetServicePort();
    ServerInfo.nServerChannel          = m_pConfig->GetServerChannel();
    ServerInfo.nServerChannelNumber    = m_pConfig->GetServerChannelNumber();
    ServerInfo.nServerChannellMaxClient= m_pConfig->GetServerChannelMaxClient();
    ServerInfo.bNewServer              = m_pConfig->GetNewServer();
    ServerInfo.SetServerIp(m_szAddress);

	NET_SERVER_INFO Msg(ServerInfo);
	Msg.SetType(NET_MSG_SND_FULL_SVR_INFO);
	
	sc::writeLogInfo(sc::string::format("SessionSndSvrInfo %s CH:%d", m_szAddress, Msg.gsi.nServerChannelNumber ));
	
	if (SendSession(&Msg) == NET_ERROR)
	{
		return NET_ERROR;
	}
	else
	{
		for (int i=0; i<m_pConfig->GetServerChannelNumber(); ++i)
		{
			SessionSndChannelState(i);
		}
		return NET_OK;
	}
}

// Agent Server -> Session Server
int CAgentServer::SessionSndSvrCurState()
{	
    // 로그인서버, 게임서버 -> 세션서버 : 현재 서버상태(간략한)를 전송한다.
    // 현재접속자, 최대접속자
    NET_SERVER_CUR_INFO nsci;
	nsci.gsci.nServerCurrentClient	= m_pClientManager->GetCurrentClientNumber();
	nsci.gsci.nServerMaxClient		= m_nMaxClient;	
	return SendSession(&nsci);
}

int CAgentServer::SessionSndChannelState(int nChannel)
{
	if (nChannel < 0 || nChannel >= m_nServerChannelNumber)
	{
		return NET_ERROR;
	}

	NET_SERVER_CHANNEL_INFO nsci;
	nsci.nChannel            = nChannel;
	nsci.nChannelCurrentUser = m_ChannelUser[nChannel];
	nsci.nChannelMaxUser     = m_nMaxChannelUser;
	nsci.nServerGroup        = m_nServerGroup;
	nsci.bPk                 = m_pConfig->IsPkChannel(nChannel);

	return SendSession(&nsci);
}

int	CAgentServer::SessionConnect( const char* szServerIP, int nPort )
{
    if ( m_SessionServerSlot != net::INVALID_SLOT && INVALID_SOCKET != m_pClientManager->GetSocket( m_SessionServerSlot ) )
    {
        sc::writeLogInfo( std::string( "Session is online" ) );
        return NET_OK;
    }

    int SessionSlot = m_pClientManager->GetFreeClientID( true );
    if ( SessionSlot == NET_ERROR )
    {
        sc::writeLogError(std::string( "SessionConnect Not enough free network slot" ) );
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
        sc::writeLogError( std::string( "Session server connection failed" ) );
        return NET_ERROR;
    }    
    
	if ( NET_ERROR == m_pClientManager->SetAcceptedClient( SessionSlot, ConnectSocket, net::SLOT_SESSION ) )
	{
		::closesocket( ConnectSocket );
		m_pClientManager->ReleaseClientID( SessionSlot );
		sc::writeLogError( std::string( "SessionConnect SetAcceptedClient failed" ) );
		return NET_ERROR;
	}

    // If type is session, all slots are itself.
    m_SessionServerSlot = SessionSlot;
    m_pClientManager->SetSlotType( m_SessionServerSlot, net::SLOT_SESSION );    
    
	PER_IO_OPERATION_DATA* pIOData = m_pClientManager->GetFreeOverIO( NET_RECV_POSTED );
	if ( NULL == pIOData )
	{
		CloseClient( SessionSlot, net::ERROR_IO_OPERATION, WSAGetLastError() );
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
		CloseClient( SessionSlot, net::ERROR_IO_OPERATION, WSAGetLastError() );
		sc::writeLogError( sc::string::format( "SessionConnect WSARecv %1%", WSAGetLastError() ) );
		return NET_ERROR;
	}

	sc::writeLogInfo( 
		sc::string::format( "Connection success to session server. Current session slot is %1%", m_SessionServerSlot ) );
    return NET_OK;
}

int CAgentServer::SessionCloseConnect()
{
	return m_pClientManager->CloseClient( m_SessionServerSlot );
}

// Session 서버와 통신을 체크하고 이상이 있으면 다시 연결한다.
int CAgentServer::SessionReconnect()
{
    if ( !m_pClientManager )
    {
        sc::writeLogError( std::string( "SessionReconnect !m_pClientManager" ) );
        return NET_ERROR;
    }

    if ( INVALID_SOCKET == m_pClientManager->GetSocket( m_SessionServerSlot ) )
    {
        return SessionConnectSndSvrInfo();
    }
    else
    {
        return NET_OK;
    }
}

void CAgentServer::SessionMsgVersionSS(NET_MSG_GENERIC* nmg)
{
    NET_VERSION_SS* pPacket = (NET_VERSION_SS*) nmg;
    m_LauncherVersion = pPacket->m_LauncherVersion;
    m_PatchVersion = pPacket->m_PatchVersion;

    //sc::writeLogInfo(sc::string::format("Launcher version %1%", pPacket->m_LauncherVersion));
    //sc::writeLogInfo(sc::string::format("Patch version %1%", pPacket->m_PatchVersion));
}

void CAgentServer::DoNothing(NET_MSG_GENERIC* pMsg)
{
    sc::writeLogInfo(
        sc::string::format(
            "Received message but do nothing. %1%", pMsg->Type()));
}

//                        ,     
//                   ,   /^\     ___
//                  /^\_/   `...'  /`
//               ,__\    ,'     ~ (
//            ,___\ ,,    .,       \
//             \___ \\\ .'.'   .-.  )
//               .'.-\\\`.`.  '.-. (
//              / (==== ."".  ( o ) \
//            ,/u  `~~~'|  /   `-'   )
//           "")^u ^u^|~| `""".  ~_ /
//             /^u ^u ^\~\     ".  \\
//     _      /u^  u ^u  ~\      ". \\
//    ( \     )^ ^U ^U ^U\~\      ". \\
//   (_ (\   /^U ^ ^U ^U  ~|       ". `\
//  (_  _ \  )U ^ U^ ^U ^|~|        ". `\.
// (_  = _(\ \^ U ^U ^ U^ ~|          ".`.;
//(_ -(    _\_)U ^ ^ U^ ^|~|            ""
//(_    =   ( ^ U^ U^ ^ U ~|
//(_ -  ( ~  = ^ U ^U U ^|~/
// (_  =     (_^U^ ^ U^ U /
//  (_-   ~_(/ \^ U^ ^U^,"
//   (_ =  _/   |^ u^u."  
//    (_  (/    |u^ u.(   
//     (__/     )^u^ u/ 
//             /u^ u^(  
//            |^ u^ u/   
//            |u^ u^(       ____   
//            |^u^ u(    .-'    `-,
//             \^u ^ \  / ' .---.  \
//              \^ u^u\ |  '  `  ;  |
//               \u^u^u:` . `-'  ;  |
//                `-.^ u`._   _.'^'./
//                   "-.^.-```_=~._/
//                      `"------"'
// jgkim
