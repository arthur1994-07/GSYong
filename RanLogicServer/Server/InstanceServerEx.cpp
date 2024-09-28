#include "pch.h"
#include "../../Enginelib/Common/profile.h"
#include "../../RanLogic/Msg/LoginMsg.h"
#include "../../RanLogic/Msg/TraceMsg.h"
#include "../../RanLogic/Msg/ServerMsg.h"

#include "../FieldServer/GLGaeaServer.h"
#include "../Client/NetInstanceClientMan.h"
#include "./ServerCfg.h"
#include "./InstanceServerConfigXml.h"
#include "./NetServerHelper.h"
#include "./InstanceServer.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

//! Session Server message process
void InstanceServer::SessionMsgProcess(MSG_LIST* pMsg)
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
	case NET_MSG_RECONNECT_MATCH:
        sc::writeLogInfo(std::string("Reconnect to match server"));
        MatchConnect();
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
    default:
        sc::writeLogError(
            sc::string::format(
                "SessionMsgProcess unknown message type %1%", pPacket->Type()));
        break;
    }
}

BOOL InstanceServer::UpdateTracingCharacter(NET_MSG_GENERIC* nmg )
{
	//
	//mjeon.STracingData에 std::string이 포함되어 있어서 Release 버전과 Debug 버전 사이에서 문제가 된다. 차 후, 이 메시지에서 std::string을 들어내야한다.
	//
#ifdef _DEBUG
	return TRUE;
#endif

    GLMSG::NET_UPDATE_TRACINGCHAR_ALL* pNetMsg = (GLMSG::NET_UPDATE_TRACINGCHAR_ALL*) nmg;
    if (!pNetMsg)
        return FALSE;

    size_t nPreSize = m_vecTracingData.size();

    m_vecTracingData.clear();    
    for(size_t i=0; i<MAX_TRACING_NUM; ++i)
    {
        if (pNetMsg->tracingData[i].dwUserNum == 0)
            break;
        std::string Account(pNetMsg->tracingData[i].m_szAccount);
        if (Account.empty())
            break;
        m_vecTracingData.push_back(pNetMsg->tracingData[i]);
    }
    size_t nPostSize = m_vecTracingData.size();

    // 추가 되었을 경우
    DWORD iMAX_CLIENT = (DWORD)m_pClientManager->GetMaxClient();
    if( nPreSize < nPostSize )
    {	
        GLMSG::NET_UPDATE_TRACINGCHAR msg;		
        DWORD dwGaeaID;
        for (DWORD i=1; i<iMAX_CLIENT; ++i)
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

                        //	Note : 클라이언트에 Tracing 변화 메시지를 보낸다.
                        //  1이면 TRUE
                        GLMSG::NET_UPDATE_TRACINGCHAR NetMsgNew;
                        NetMsgNew.updateNum = 1;
                        m_pGaeaServer->SENDTOCLIENT(pChar->ClientSlot(), &NetMsgNew);

                    }
                }
            }
        }		
        // 삭제 되었을 경우
    }
    else if (nPreSize > nPostSize)
    {        
        for (DWORD i=1; i<iMAX_CLIENT; ++i)
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
                        // 클라이언트에 Tracing 변화 메시지를 보낸다.
                        // 0이면 FALSE
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

void InstanceServer::SessionMsgHeartbeatServerAns()
{
	NET_HEARTBEAT_SERVER_ANS MsgHeartbeatServerAns;
    SendSession( &MsgHeartbeatServerAns, MsgHeartbeatServerAns.Size() );
}

void InstanceServer::SessionMsgSvrCmd(NET_MSG_GENERIC* nmg)
{
	if (!nmg)
        return;

	NET_SVR_CMD_CHANGE_STATE* pPacket = reinterpret_cast <NET_SVR_CMD_CHANGE_STATE*> (nmg);

	sc::writeLogInfo(std::string("Received Server Command From Session Server"));

	switch (pPacket->nCmdType)
	{
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
	case NET_MSG_SVR_RESTART: // Server restart message
		sc::writeLogInfo(std::string("Server Will Restart"));
		Stop();
		Start();
		break;
	default:
		break;
	}
}

int InstanceServer::SendSession(NET_MSG_GENERIC* pBuffer, DWORD SendSize)
{
    if (pBuffer && m_SessionServerSlot != net::INVALID_SLOT)
    {
        PROFILE_MSG_COUNT_SEND(pBuffer);
        return m_pClientManager->SendClient2(m_SessionServerSlot, pBuffer);
    }
    else 
    {
        sc::writeLogError(std::string("SendSession pBuffer NULL or m_SessionServerSlot NULL"));
        return NET_ERROR;        
    }
}

// 세션서버에서 오는 채팅메시지 처리
void InstanceServer::SessionMsgChatProcess(
	NET_MSG_GENERIC* nmg )
{
    if (nmg == NULL) return;

	NET_CHAT_CTRL* nc = (NET_CHAT_CTRL*) nmg;

	switch (nc->emType) 
	{
	case CHAT_TYPE_GLOBAL: // 전체 공지 메시지
		sc::writeLogInfo(std::string("Sending system message to all users"));
		MsgSndChatGlobal(nc->szChatMsg);
		break;
	default:
		break;
	}	
}

// 세션서버 -> 게임서버 : 로그인결과 데이타
void InstanceServer::SessionMsgLoginFeedBack(NET_MSG_GENERIC* nmg)
{	
    if (!nmg)
        return;

	NET_LOGIN_FEEDBACK_DATA		nlfd;
	NET_LOGIN_FEEDBACK_DATA2*	nlfd2;
	
	DWORD	dwClient;	
	DWORD	dwSndBytes = 0;
	CHAR	szUserid[USR_ID_LENGTH+1] = {0};
	INT		nUserNum = 0;

	// 서버에서 넘어온 로그인 결과 메시지
	nlfd2 = (NET_LOGIN_FEEDBACK_DATA2*) nmg;
	dwClient = (DWORD) nlfd2->m_ClientSlot;
	
	// 로그인 요청한 클라이언트와 동일한지 검사한다.
	dwSndBytes = sizeof(NET_LOGIN_FEEDBACK_DATA2);
	nlfd.SetSize( dwSndBytes );
	nlfd.nType = NET_MSG_LOGIN_FB;
	
	// 온라인이고, ip, id 가 동일할때...
	if ((m_pClientManager->IsOnline(dwClient) == true) &&
		(m_pClientManager->GetClientIP(dwClient) == nlfd2->szIp == 0) &&
		(strcmp(m_pClientManager->GetUserID(dwClient), nlfd2->szUserid) == 0))
	{
		nlfd.m_Result = nlfd2->m_emResult;
		// 로그인이 정상적으로 되었으면 사용자 번호를 세팅한다.
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
	// 로그인 결과 전송	
	SendClient(dwClient, &nlfd);
}

// Send server info to session server
int InstanceServer::SessionSndSvrInfo()
{
    G_SERVER_INFO ServerInfo;

    ServerInfo.nControlPort			= m_pConfig->GetControlPort();
    ServerInfo.nServerCurrentClient	= m_pClientManager->GetCurrentClientNumber();
    ServerInfo.nServerGroup			= m_pConfig->GetServerGroup();
    ServerInfo.nServerMaxClient		= m_pConfig->GetServerMaxClient();
    ServerInfo.nServerNumber		= m_pConfig->GetServerNumber();
    ServerInfo.ServerType			= m_pConfig->GetServerType();    
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
int InstanceServer::SessionSndSvrCurState()
{
	// 현재접속자, 최대접속자
	NET_SERVER_CUR_INFO nsci;
	nsci.gsci.nServerCurrentClient = (int) m_pGaeaServer->GetNumPC();
	nsci.gsci.nServerMaxClient	   = m_pConfig->GetServerMaxClient();	
	return SendSession(&nsci, nsci.Size());
}

void InstanceServer::SessionSndLogOutInfoSimple(const char* szUsrID)
{
    if (szUsrID)
    {
	    NET_LOGOUT_DATA_SIMPLE nlods;
	    nlods.nType	= NET_MSG_LOGOUT_G_S_S;	
	    nlods.SetUserId(szUsrID);
	    SendSession(&nlods, nlods.Size());
    }
}

void InstanceServer::SessionSndLogOutInfo(const char* szUsrID, int nUserNum, int nGameTime, int nChaNum)
{	
    if (szUsrID == NULL) return;
	
	NET_LOGOUT_DATA nlod;
	nlod.nType	= NET_MSG_LOGOUT_G_S;	
	nlod.nUserNum   = nUserNum;
	nlod.nGameTime  = nGameTime;
	nlod.nChaNum    = nChaNum;
	nlod.SetUserId(szUsrID);

	SendSession(&nlod, nlod.Size());
}

//! Session 서버에 접속하고 서버의 정보를 전송한다.
int InstanceServer::SessionConnectSndSvrInfo()
{
	if (SessionConnect() == NET_ERROR)
		return NET_ERROR;
	else
		return SessionSndSvrInfo();
}

int	InstanceServer::SessionConnect()
{
	// 추 후에 int CAgentServer::CacheConnect() 참조해서 수정해야 한다.
    /*if (m_SessionServerSlot != net::INVALID_SLOT && m_pClientManager->IsOnline(m_SessionServerSlot))
    {
        sc::writeLogInfo(std::string("Session is online"));
        return NET_OK;
    }
    
    int SessionSlot = m_pClientManager->GetFreeClientID(true);
    if (SessionSlot == NET_ERROR)
    {
        sc::writeLogInfo(std::string("Not enough free network slot"));
        return NET_ERROR;
    }

    PER_IO_OPERATION_DATA* pIOData = m_pClientManager->GetFreeOverIO(NET_RECV_POSTED);
    net::CONNECT_RESULT Connect = net::ConnectAndReceive(
        m_pConfig->GetSessionServerIP(),
        m_pConfig->GetSessionServerPort(),
        m_hIOServer,
        SessionSlot,
        pIOData);
    int Result = Connect.get<0> ();
    SOCKET ConnectSocket = Connect.get<1> ();
    if (Result == NET_ERROR)
    {
        m_pClientManager->ReleaseOperationData(pIOData);
        m_pClientManager->ReleaseClientID(SessionSlot);
        sc::writeLogError(std::string("Session server connecting failed"));
        return NET_ERROR;
    }    
    
    m_SessionServerSlot = SessionSlot;
    m_pClientManager->SetAcceptedClient (m_SessionServerSlot, ConnectSocket, net::SLOT_SESSION);
    m_pClientManager->SetSlotType       (m_SessionServerSlot, net::SLOT_SESSION);    
    
    sc::writeLogInfo(sc::string::format("Session server connected. slot %1%", m_SessionServerSlot));
	*/
    return NET_OK;
}

void InstanceServer::SessionMsgVersionSS(NET_MSG_GENERIC* nmg)
{
    NET_VERSION_SS* pPacket = (NET_VERSION_SS*) nmg;
    m_LauncherVersion = pPacket->m_LauncherVersion;
    m_PatchVersion = pPacket->m_PatchVersion;

    //sc::writeLogInfo(sc::string::format("Launcher version %1%", pPacket->m_LauncherVersion));
    //sc::writeLogInfo(sc::string::format("Patch version %1%", pPacket->m_PatchVersion));
}

void InstanceServer::DoNothing(NET_MSG_GENERIC* pMsg)
{
    sc::writeLogInfo(
        sc::string::format(
            "Received message but do nothing. %1%", pMsg->Type()));
}

PER_IO_OPERATION_DATA* InstanceServer::GetFreeOverIO(EMMSG_IO_TYPE nType)
{
    return m_pClientManager->GetFreeOverIO(nType);
}

void InstanceServer::ReleaseOperationData(PER_IO_OPERATION_DATA* pData)
{
    m_pClientManager->ReleaseOperationData(pData);
}

int InstanceServer::MatchConnect()
{
	// 추 후에 int CAgentServer::CacheConnect() 참조해서 수정해야 한다.
    /*if (m_MatchServerSlot != net::INVALID_SLOT && m_pClientManager->IsOnline(m_MatchServerSlot))
    {
        sc::writeLogError(sc::string::format("MatchServer is online on the slot %1%", m_MatchServerSlot));
        return NET_OK;
    }

    int MatchSlot = m_pClientManager->GetFreeClientID(true);
    if (MatchSlot == NET_ERROR)
    {
        sc::writeLogError(std::string("CAgentServer::MatchConnect() - Network slot is NOT enough"));
        return NET_ERROR;
    }

    PER_IO_OPERATION_DATA* pIOData = m_pClientManager->GetFreeOverIO(NET_RECV_POSTED);
    net::CONNECT_RESULT Connect = net::ConnectAndReceive(
										m_pConfig->GetMatchServerIP(),
										m_pConfig->GetMatchServerPort(),
										m_hIOServer,
										MatchSlot,
										pIOData);

    int Result = Connect.get<0>();
    SOCKET ConnectSocket = Connect.get<1> ();
    if (Result == NET_ERROR)
    {
        m_pClientManager->ReleaseOperationData(pIOData);
        m_pClientManager->ReleaseClientID(MatchSlot);
        sc::writeLogError(std::string("MatchServer connection failed"));
        return NET_ERROR;
    }

    m_MatchServerSlot = MatchSlot;
    m_pClientManager->SetAcceptedClient (m_MatchServerSlot, ConnectSocket, net::SLOT_MATCH);    
    
    sc::writeLogInfo(sc::string::format("MatchServer is connected on the slot %1%", m_MatchServerSlot));

	NET_NEW_INSTANCE msg(m_dwInstanceServerNum);
	msg.CrcValue = sc::string::getCrc32(&msg, msg.Size() - sizeof(unsigned int));
	SendMatch(&msg);*/

    return NET_OK;
}

int InstanceServer::MatchCloseConnect()
{
    if (!m_pClientManager)
    {
        sc::writeLogError(std::string("MatchCloseConnect() - ClientManager is NULL."));
        return FALSE;
    }

    if (m_pClientManager->IsOnline(m_MatchServerSlot))
    {
        m_pClientManager->CloseClient(m_MatchServerSlot);

		return NET_OK;
    }
    else
    {
        sc::writeLogInfo(std::string("MatchCloseConnect is already offline"));
        return NET_OK;
    }
}
