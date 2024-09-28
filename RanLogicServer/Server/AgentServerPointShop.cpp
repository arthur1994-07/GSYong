#include "../pch.h"
#include "../../RanLogic/Msg/PointShopMsg.h"
#include "../Client/NetAgentClientMan.h"
#include "./AgentServer.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void CAgentServer::MsgPointShopDataStart(NET_MSG_GENERIC* pMsg)
{
    pointshop::NET_DATA_START_HS* pPacket = (pointshop::NET_DATA_START_HS*) pMsg;
    m_pGLAgentServer->PointShopClear(pPacket->ShopUpdateDate());
}

void CAgentServer::MsgPointShopDataEnd(NET_MSG_GENERIC* pMsg)
{
    pointshop::NET_DATA_END_HS* pPacket = (pointshop::NET_DATA_END_HS*) pMsg;
    m_pGLAgentServer->PointShopDataEnd(pPacket->IsReload());   
}

void CAgentServer::MsgPointShopData(NET_MSG_GENERIC* pMsg)
{
    pointshop::NET_DATA_HS* pPacket = (pointshop::NET_DATA_HS*) pMsg;
    m_pGLAgentServer->PointShopDataInsert(pPacket->Data());
}

void CAgentServer::MsgPointShopGmReloadCA(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* pMsg)
{
    pointshop::NET_GM_RELOAD_CA* pPacket = (pointshop::NET_GM_RELOAD_CA*) pMsg;
    if (m_pClientManager->GetUserType(dwClient) >= USER_GM3)
    {
        // Session 서버로 알림
        pointshop::NET_RELOAD_AS MsgAS;
        SendSession(&MsgAS);
    }
}