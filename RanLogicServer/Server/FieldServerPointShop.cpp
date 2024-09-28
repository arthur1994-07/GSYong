#include "../pch.h"
#include "../../RanLogic/Msg/PointShopMsg.h"
#include "../../RanLogic/Msg/CharacterMsg.h"
#include "../FieldServer/GLGaeaServer.h"
#include "./s_CFieldServer.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void CFieldServer::MsgPointShopDataStart(NET_MSG_GENERIC* pMsg)
{
    pointshop::NET_DATA_START_HS* pPacket = (pointshop::NET_DATA_START_HS*) pMsg;
    m_pGaeaServer->PointShopClear(pPacket->ShopUpdateDate());
}

void CFieldServer::MsgPointShopDataEnd(NET_MSG_GENERIC* pMsg)
{
    pointshop::NET_DATA_END_HS* pPacket = (pointshop::NET_DATA_END_HS*) pMsg;
    m_pGaeaServer->PointShopDataEnd();
}

void CFieldServer::MsgPointShopData(NET_MSG_GENERIC* pMsg)
{
    pointshop::NET_DATA_HS* pPacket = (pointshop::NET_DATA_HS*) pMsg;
    m_pGaeaServer->PointShopDataInsert(pPacket->Data());
}

void CFieldServer::MsgPointShopCharPointHF(NET_MSG_GENERIC* pMsg)
{
    NET_CHAR_POINT_HF* pPacket = (NET_CHAR_POINT_HF*) pMsg;    
    m_pGaeaServer->SetCharPointShopPoint(pPacket->ChaDbNum(), pPacket->Point());
}