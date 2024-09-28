#include "pch.h"
#include "../../Enginelib/Common/profile.h"
#include "../../RanLogic/Msg/ServerMsg.h"
#include "../../RanLogic/Msg/PointShopMsg.h"

#include "../Client/NetAgentClientMan.h"
#include "./AgentServerConfigXml.h"
#include "./NetServerHelper.h"
#include "./AgentServer.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

int CAgentServer::MatchConnect()
{
	// 추 후에 int CAgentServer::CacheConnect() 참조해서 수정해야 한다.
	/*
    if (m_MatchServerSlot != net::INVALID_SLOT && m_pClientManager->IsOnline(m_MatchServerSlot))
    {
        sc::writeLogInfo(sc::string::format("MatchServer is online on the slot %1%", m_MatchServerSlot));
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

	
	NET_I_AM_AGENT msg;
	msg.dwSvrGroupNum = GetServerGroup();
    msg.CrcValue = sc::string::getCrc32(&msg, msg.Size()-sizeof(unsigned int));

	SendMatch(&msg);*/

    return NET_OK;
}

int CAgentServer::MatchCloseConnect()
{
    if (!m_pClientManager)
    {
        sc::writeLogError(std::string("MatchCloseConnect() - ClientManager is NULL."));
        return FALSE;
    }

    if (m_pClientManager->IsOnline(m_MatchServerSlot))
    {
        m_pClientManager->CloseClient(m_MatchServerSlot);
    }
    else
    {
        sc::writeLogInfo(std::string("MatchCloseConnect is already offline"));	
    }

	if (m_bIsRunning)
	{
		// 재연결 시도
		sc::writeLogInfo(std::string("MatchServer Connection is closed. Trying to reconnect"));
		m_MatchServerSlot = net::SLOT_UNKNOWN;
		MatchConnect();
	}
	return NET_OK;
}

int CAgentServer::SendMatch(EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc)
{
    NET_MSG_PACK MsgPack;
    if (MsgPack.SetData(MsgType, PackBuffer, UseCrc))
        return SendMatch(&MsgPack);
    else
        return NET_ERROR;
}

int CAgentServer::SendMatch(NET_MSG_GENERIC* pMsg)
{
    if (pMsg && m_MatchServerSlot != net::INVALID_SLOT)
    {
        return m_pClientManager->SendClient2(m_MatchServerSlot, pMsg);
    }

	return NET_ERROR;
}

BOOL CAgentServer::MatchMsgProcess(MSG_LIST* pMsg)
{
	BOOL bRet = TRUE;

	if (!pMsg)
        return bRet;

	DWORD dwClient = pMsg->dwClient;
	NET_MSG_GENERIC* nmg = reinterpret_cast<NET_MSG_GENERIC*>(pMsg->Buffer);
	switch (nmg->Type())
	{
	case NET_MSG_HEARTBEAT_SERVER_ANS:
		m_pClientManager->SetHeartBeat(dwClient);
		break;

	case NET_MSG_HEARTBEAT_SERVER_REQ:
		MatchSendHeartbeat();        
		break;

	case NET_MSG_MATCH_REQUEST_RESULT_MAC:
		m_pGLAgentServer->MsgMatchResultMAC(nmg, dwClient, GAEAID_NULL);
		break;

	case NET_MSG_MATCH_CANCEL_CAM_FB:
		m_pGLAgentServer->MsgMatchCancelCAM_FB(nmg, dwClient, GAEAID_NULL);

	case NET_MSG_TRANSFER_START_MA:
		m_pGLAgentServer->MsgTransferStartMA(nmg, dwClient, GAEAID_NULL);
		break;
	
	default:
		bRet = FALSE;
		sc::writeLogDebug(sc::string::format("MatchMsgProcess: Message type %1% is passed to GLAgentServer", nmg->Type()));
		//
		// 여기서 처리되지 않은 메시지는 GLAgentServer::MsgProcess에서 처리된다.
		//
		break;
	}

	return bRet;
}

void CAgentServer::MatchSendHeartbeat()
{
	NET_HEARTBEAT_SERVER_ANS Msg;
	SendMatch(&Msg);
}
