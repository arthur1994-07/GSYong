#include "../pch.h"
#include "../../Enginelib/Common/profile.h"
#include "../../RanLogic/Msg/LoginMsg.h"
#include "../../RanLogic/Msg/TraceMsg.h"
#include "../../RanLogic/Msg/ServerMsg.h"

#include "../FieldServer/GLGaeaServer.h"
#include "../Client/NetFieldClientMan.h"
#include "./ServerCfg.h"
#include "./FieldServerConfigXml.h"
#include "./NetServerHelper.h"
#include "./s_CFieldServer.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

//! Session Server message process
void CFieldServer::SessionMsgProcess(MSG_LIST* pMsg)
{
    if (!pMsg)
        return;

    NET_MSG_GENERIC* pPacket = (NET_MSG_GENERIC*) pMsg->Buffer;

    if (!pPacket)
        return;

    switch (pPacket->Type())
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
    case NET_MSG_REQ_FULL_SVR_INFO :	
        SessionSndSvrInfo();
        break;
    case NET_MSG_REQ_CUR_STATE :
        SessionSndSvrCurState();
        break;		
    case NET_MSG_LOGIN_FB :
        SessionMsgLoginFeedBack(pPacket);
        break;
    case NET_MSG_CHAT_CTRL :
        SessionMsgChatProcess(pPacket);		
        break;
    case NET_MSG_SVR_CMD :
        SessionMsgSvrCmd(pPacket);
        break;	
    case NET_MSG_UPDATE_TRACING_ALL:		
        UpdateTracingCharacter(pPacket);
        break;
    case NET_MSG_VERSION_SS:
        SessionMsgVersionSS(pPacket);
        break;
    case NET_MSG_PS_RELOAD_SS:
        DoNothing(pPacket);
        break;
    case NET_MSG_GM_LOG_ITEM_RELOAD_SS:
        if (m_pGaeaServer)
            m_pGaeaServer->ReloadLogItem();
        break;
    case NET_MSG_GM_GAME_DATA_UPDATE_SS:
        DoNothing(pPacket);
        break;
	case NET_MSG_AUTH_CERTIFICATION_ANS:
		MsgAuthCertificationResponse(pPacket);
		break;
    default:
        sc::writeLogError(
            sc::string::format(
                "SessionMsgProcess unknown message type %1%", pPacket->Type()));
        break;
    }
}

BOOL CFieldServer::UpdateTracingCharacter(NET_MSG_GENERIC* nmg )
{
	//
	//mjeon.STracingData�� std::string�� ���ԵǾ� �־ Release ������ Debug ���� ���̿��� ������ �ȴ�. �� ��, �� �޽������� std::string�� �����Ѵ�.
	//
#ifdef _DEBUG
	return TRUE;
#endif

    GLMSG::NET_UPDATE_TRACINGCHAR_ALL* pNetMsg = (GLMSG::NET_UPDATE_TRACINGCHAR_ALL*) nmg;
    if (!pNetMsg)
        return FALSE;

    size_t nPreSize = m_vecTracingData.size();

    m_vecTracingData.clear();    
    for (size_t i=0; i<MAX_TRACING_NUM; ++i)
    {
		std::string AccountName(pNetMsg->tracingData[i].m_szAccount);

        if (pNetMsg->tracingData[i].dwUserNum == 0 && AccountName.empty() )
            break;
        
        m_vecTracingData.push_back(pNetMsg->tracingData[i]);
    }
    size_t nPostSize = m_vecTracingData.size();

    // �߰� �Ǿ��� ���
    DWORD iMAX_CLIENT = (DWORD)m_pClientManager->GetMaxClient();
    if( nPreSize < nPostSize )
    {	
        GLMSG::NET_UPDATE_TRACINGCHAR msg;		
        DWORD dwGaeaID;
        for (DWORD i=1; i<(DWORD) iMAX_CLIENT; ++i)
        {
            if (m_pClientManager->IsOnline(i) && m_pClientManager->IsClientSlot(i))
            {
                dwGaeaID = m_pClientManager->GetGaeaID(i);
                GLChar* pChar = m_pGaeaServer->GetChar(dwGaeaID);
                if (pChar && pChar->m_bTracingUser == FALSE)
                {
                    int nTracing = CheckTracingUser(pChar->m_szUID, pChar->GetUserID());
                    if (nTracing != -1)
                    {
                        m_vecTracingData[nTracing].dwOnLine       = TRUE;
                        m_vecTracingData[nTracing].dwUserNum      = pChar->GetUserID();
                        m_vecTracingData[nTracing].nConnectServer = m_nServerGroup;
                        m_vecTracingData[nTracing].SetAccount(pChar->m_szUID);

                        pChar->m_bTracingUser = TRUE;

                        // Ŭ���̾�Ʈ�� Tracing ��ȭ �޽����� ������.
                        // 1�̸� TRUE
                        GLMSG::NET_UPDATE_TRACINGCHAR NetMsgNew;
                        NetMsgNew.updateNum = 1;
                        m_pGaeaServer->SENDTOCLIENT(pChar->ClientSlot(), &NetMsgNew);

                    }
                }
            }
        }		
        // ���� �Ǿ��� ���
    }
    else if (nPreSize > nPostSize)
    {        
        for (DWORD i=1; i<(DWORD) iMAX_CLIENT; ++i)
        {
            if (m_pClientManager->IsOnline(i) && m_pClientManager->IsClientSlot(i))
            {
                DWORD dwGaeaID = m_pClientManager->GetGaeaID(i);
                GLChar* pChar = m_pGaeaServer->GetChar(dwGaeaID);
                if (pChar && pChar->m_bTracingUser)
                {
                    int nTracing = CheckTracingUser(pChar->m_szUID, pChar->GetUserID());
                    if (nTracing == -1)
                    {
                        pChar->m_bTracingUser = FALSE;
                        // Ŭ���̾�Ʈ�� Tracing ��ȭ �޽����� ������.
                        // 0�̸� FALSE
                        GLMSG::NET_UPDATE_TRACINGCHAR NetMsgNew;
                        NetMsgNew.updateNum = 0;
                        m_pGaeaServer->SENDTOCLIENT(pChar->ClientSlot(), &NetMsgNew);
                    }
                }
            }
        }
    }
    return TRUE;
}

void CFieldServer::SessionMsgHeartbeatServerAns()
{
#ifdef HEARTBEAT_LOG
	sc::writeLogInfo(std::string("[HEARTBEAT] Recv Heartbeat Request from Session Svr"));
#endif

	NET_HEARTBEAT_SERVER_ANS MsgHeartbeatServerAns;
    SendSession( &MsgHeartbeatServerAns, MsgHeartbeatServerAns.Size() );
#ifdef HEARTBEAT_LOG
	sc::writeLogInfo(std::string("[HEARTBEAT] Send Heartbeat Response to Session Svr"));
#endif
}

void CFieldServer::SessionMsgSvrCmd(NET_MSG_GENERIC* nmg)
{
	if (!nmg)
        return;

	NET_SVR_CMD_CHANGE_STATE* pPacket = reinterpret_cast <NET_SVR_CMD_CHANGE_STATE*> (nmg);

	// NET_MSG_SVR_RESTART �븸 �ʵ弭�� Stop() ���� �����鼭 ũ���õǴ� �� ���� �켱 �ּ� ó����
	std::string strMessage( 
		sc::string::format( "Received Server Command From Session Server command type %1%", pPacket->nCmdType ) );
	sc::writeLogInfo( strMessage );

	//switch (pPacket->nCmdType)
	//{
		/*
	case NET_MSG_SVR_PAUSE: // Server pause message
		sc::writeLogInfo(std::string("Server Will Pause"));
		Pause();
		break;		
	case NET_MSG_SVR_RESUME: // Server resume message
		sc::writeLogInfo(std::string("Server Will Resume"));
		Resume();
		break;
		*/
	/*case NET_MSG_SVR_RESTART: // Server restart message
		sc::writeLogInfo(std::string("Server Will Restart"));
		Stop();
		Start();
		break;*/
	//default:
	//	break;
	//}
}

int CFieldServer::SendSession( NET_MSG_GENERIC* pBuffer, DWORD SendSize )
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

// ���Ǽ������� ���� ä�ø޽��� ó��
void CFieldServer::SessionMsgChatProcess(
	NET_MSG_GENERIC* nmg )
{
    if (nmg == NULL) return;

	NET_CHAT_CTRL* nc = (NET_CHAT_CTRL*) nmg;

	switch (nc->emType) 
	{
	case CHAT_TYPE_GLOBAL: // ��ü ���� �޽���
		sc::writeLogInfo(std::string("Sending system message to all users"));
		MsgSndChatGlobal(nc->szChatMsg);
		break;
	default:
		break;
	}	
}

// ���Ǽ��� -> ���Ӽ��� : �α��ΰ�� ����Ÿ
void CFieldServer::SessionMsgLoginFeedBack(NET_MSG_GENERIC* nmg)
{	
    if (!nmg)
        return;

	NET_LOGIN_FEEDBACK_DATA		nlfd;
	NET_LOGIN_FEEDBACK_DATA2*	nlfd2;
	
	DWORD	dwClient;	
	DWORD	dwSndBytes = 0;
	CHAR	szUserid[USR_ID_LENGTH+1] = {0};
	INT		nUserNum = 0;

	// �������� �Ѿ�� �α��� ��� �޽���
	nlfd2 = (NET_LOGIN_FEEDBACK_DATA2*) nmg;
	dwClient = (DWORD) nlfd2->m_ClientSlot;
	
	// �α��� ��û�� Ŭ���̾�Ʈ�� �������� �˻��Ѵ�.
	dwSndBytes = sizeof(NET_LOGIN_FEEDBACK_DATA2);
	nlfd.SetSize( dwSndBytes );
	nlfd.nType = NET_MSG_LOGIN_FB;
	
	// �¶����̰�, ip, id �� �����Ҷ�...
	if ((m_pClientManager->IsOnline(dwClient) == true) &&
		(m_pClientManager->GetClientIP(dwClient) == nlfd2->szIp == 0) &&
		(strcmp(m_pClientManager->GetUserID(dwClient), nlfd2->szUserid) == 0))
	{
		nlfd.m_Result = nlfd2->m_emResult;
		// �α����� ���������� �Ǿ����� ����� ��ȣ�� �����Ѵ�.
		if (nlfd2->m_emResult == EM_LOGIN_FB_SUB_OK)
		{
			::StringCchCopy(szUserid, USR_ID_LENGTH+1, m_pClientManager->GetUserID(dwClient));
			if (nlfd2->nUserNum < 1)
			{
				sc::writeLogError(std::string("SessionMsgLoginFeedBack GetUserNum Failed"));				
				return ;
			}
			m_pClientManager->SetUserDbNum(dwClient, nlfd2->nUserNum);
			m_pClientManager->SetLoginTime(dwClient);			
		}
	}
	else
	{
		nlfd.m_Result = EM_LOGIN_FB_SUB_FAIL;
	}
	// �α��� ��� ����	
	SendClient(dwClient, &nlfd);
}

// Field Server -> Session Server 
// Send server info to session server
int CFieldServer::SessionSndSvrInfo()
{
    G_SERVER_INFO ServerInfo;

    ServerInfo.nControlPort			= m_pConfig->GetControlPort();
    ServerInfo.nServerCurrentClient	= m_pClientManager->GetCurrentClientNumber();
    ServerInfo.nServerGroup			= m_pConfig->GetServerGroup();
    ServerInfo.nServerMaxClient		= m_pConfig->GetServerMaxClient();
    ServerInfo.nServerNumber		= m_pConfig->GetServerNumber();
    ServerInfo.ServerType			= m_pConfig->GetServerType();
    ServerInfo.nServerChannel       = m_pConfig->GetServerChannel();
    ServerInfo.nServicePort			= m_pConfig->GetServicePort();	
    ServerInfo.SetServerIp(m_szAddress);

    NET_SERVER_INFO Msg(ServerInfo);
	Msg.SetType(NET_MSG_SND_FULL_SVR_INFO);
	
	if (SendSession(&Msg, Msg.Size()) == NET_ERROR)
	{
		sc::writeLogError(std::string("SessionSndSvrInfo NET_ERROR"));		
		return NET_ERROR;
	}
	else
	{
		sc::writeLogInfo(sc::string::format("SessionSndSvrInfo %1%", m_szAddress));
		return NET_OK;
	}
}

// Game Server -> Session Server
int CFieldServer::SessionSndSvrCurState()
{	
	// Field->Session:���� ��������(������)�� �����Ѵ�.
	// ����������, �ִ�������
	NET_SERVER_CUR_INFO nsci;
	nsci.gsci.nServerCurrentClient = (int) m_pGaeaServer->GetNumPC();
	nsci.gsci.nServerMaxClient	   = m_pConfig->GetServerMaxClient();	
	return SendSession(&nsci, nsci.Size());
}

void CFieldServer::SessionSndLogOutInfoSimple(const char* szUsrID)
{
    if (szUsrID)
    {
	    NET_LOGOUT_DATA_SIMPLE nlods;
	    nlods.nType	= NET_MSG_LOGOUT_G_S_S;	
	    nlods.SetUserId(szUsrID);
	    SendSession(&nlods, nlods.Size());
    }
}

void CFieldServer::SessionSndLogOutInfo(const char* szUsrID, int nUserNum, int nGameTime, int nChaNum)
{	
    if (szUsrID == NULL) return;
	
	NET_LOGOUT_DATA nlod;
	nlod.nType	= NET_MSG_LOGOUT_G_S;	
	nlod.nUserNum  = nUserNum;
	nlod.nGameTime = nGameTime;
	nlod.nChaNum   = nChaNum;
	nlod.SetUserId(szUsrID);

	SendSession(&nlod, nlod.Size());
}

//! Session ������ �����ϰ� ������ ������ �����Ѵ�.
int CFieldServer::SessionConnectSndSvrInfo()
{	
	if ( SessionConnect() == NET_ERROR )
		return NET_ERROR;
	else
		return SessionSndSvrInfo();
}

int	CFieldServer::SessionConnect()
{    
    if ( m_SessionServerSlot != net::INVALID_SLOT && INVALID_SOCKET != m_pClientManager->GetSocket( m_SessionServerSlot ) )
    {
        sc::writeLogInfo( std::string( "Session is online" ) );
        return NET_OK;
    }
    
    int SessionSlot = m_pClientManager->GetFreeClientID( true );
    if ( SessionSlot == NET_ERROR )
    {
        sc::writeLogInfo( std::string( "SessionConnect Not enough free network slot" ) );
        return NET_ERROR;
    }

    net::CONNECT_RESULT Connect = net::ConnectAndReceive(
        m_pConfig->GetSessionServerIP(),
        m_pConfig->GetSessionServerPort(),
        m_hIOServer,
        SessionSlot );
    int Result = Connect.get<0> ();
    SOCKET ConnectSocket = Connect.get<1> ();
    if ( Result == NET_ERROR )
    {
        m_pClientManager->ReleaseClientID( SessionSlot );
        sc::writeLogError( std::string( "Session server connecting failed" ) );
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

void CFieldServer::SessionMsgVersionSS(NET_MSG_GENERIC* nmg)
{
    NET_VERSION_SS* pPacket = (NET_VERSION_SS*) nmg;
    m_LauncherVersion = pPacket->m_LauncherVersion;
    m_PatchVersion = pPacket->m_PatchVersion;

    //sc::writeLogInfo(sc::string::format("Launcher version %1%", pPacket->m_LauncherVersion));
    //sc::writeLogInfo(sc::string::format("Patch version %1%", pPacket->m_PatchVersion));
}

void CFieldServer::DoNothing(NET_MSG_GENERIC* pMsg)
{
    sc::writeLogInfo(
        sc::string::format(
            "Received message but do nothing. %1%", pMsg->Type()));
}
