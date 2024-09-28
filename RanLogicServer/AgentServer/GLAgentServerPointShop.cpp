#include "../pch.h"
#include "../../SigmaCore/Log/LogMan.h"
#include "../../SigmaCore/String/StringFormat.h"
#include "../../RanLogic/Item/GLItemMan.h"
#include "../../RanLogic/Msg/PointShopMsg.h"
#include "../../RanLogic/Msg/CharacterMsg.h"
#include "../../RanLogic/GLogicDataMan.h"

#include "../PointShop/PointShopAgent.h"
#include "./GLAgentServer.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void GLAgentServer::PointShopClear(__time64_t UpdateTime)
{
    m_pPointShop->Clear();
    m_pPointShop->SetUpdateDate(UpdateTime);

#ifndef _RELEASE
    sc::writeLogInfo(
        sc::string::format(
            "PointShopClear %1%", UpdateTime));
#endif
}

void GLAgentServer::PointShopDataEnd(bool bReload)
{    
    if (bReload)
    {
        // 모든 Client 에 Point Shop 정보 재전송
        SendPointShopInfoToClient(0, true);
    }
#ifndef _RELEASE
    sc::writeLogInfo(std::string("PointShopDataEnd"));
#endif
}

void GLAgentServer::PointShopDataInsert(const pointshop::SellItem& Item)
{
    const SITEM* pItem = GLogicData::GetInstance().GetItem(Item.ItemId());
    if (!pItem)
    {
        sc::writeLogInfo(
            sc::string::format(
            "PointShopDataInsert Item not exist. item data (%1%/%2%)",
            Item.ItemId().Mid(),
            Item.ItemId().Sid()));
    }
    else
    {
        m_pPointShop->Insert(Item);
    }

#ifndef _RELEASE
	/*
    sc::writeLogInfo(
        sc::string::format(
            "PointShopDataInsert %1% %2% %3%",
            Item.ItemId().Mid(),
            Item.ItemId().Sid(),
            Item.Price()));
	*/
#endif
}

BOOL GLAgentServer::MsgPointShopUpdateDateCA(NET_MSG_GENERIC* pMsg, DWORD ClientSlot, DWORD GaeaID)
{
    const GLCharAG* pChar = GetChar(GaeaID);
    if (!pChar)
        return FALSE;

    pointshop::NET_UPDATE_DATE_CA* pPacket = (pointshop::NET_UPDATE_DATE_CA*) pMsg;
    if (m_pPointShop->UpdateDate() != pPacket->UpdateDate())
        SendPointShopInfoToClient(ClientSlot, false);
/*
#ifndef _RELEASE
    sc::writeLogInfo(
        sc::string::format(
            "MsgPointShopUpdateDateCA %1%",
            pPacket->UpdateDate()));
#endif
*/
    return TRUE;
}

void GLAgentServer::SendPointShopInfoToClient(DWORD ClientSlot, bool bAllClient)
{
    // 시작
    pointshop::NET_DATA_START_AC MsgStart(m_pPointShop->UpdateDate());
    if (bAllClient)
        m_pMsgServer->SendAllClient(&MsgStart);
    else
        m_pMsgServer->SendClient(ClientSlot, &MsgStart);
    
    // Data
    const pointshop::PointShop::PS_DATA& SellList = m_pPointShop->GetSellData();     
    for (pointshop::PointShop::PS_DATA_CITER iter=SellList.begin(); iter!=SellList.end(); ++iter)
    {        
        pointshop::NET_DATA_AC MsgData(iter->second);
        if (bAllClient)
            m_pMsgServer->SendAllClient(&MsgData);
        else
            m_pMsgServer->SendClient(ClientSlot, &MsgData);
    }

    // 끝
    pointshop::NET_DATA_END_AC MsgEnd;
    if (bAllClient)
        m_pMsgServer->SendAllClient(&MsgEnd);
    else
        m_pMsgServer->SendClient(ClientSlot, &MsgEnd);
}

BOOL GLAgentServer::MsgPointShopCharPointFA(NET_MSG_GENERIC* pMsg, DWORD ClientSlot, DWORD GaeaID)
{
	NET_CHAR_POINT_FA* pPacket = (NET_CHAR_POINT_FA*) pMsg;

	sc::writeLogInfo(sc::string::format("[TRACE POINT] PointShopPointSet(MsgPointShopCharPointFA) [Refundable %1% / NotRefundable %2%]",pPacket->Point().m_PointRefundable, pPacket->Point().m_PointNotRefundable));

	GLCharAG* pChar = GetCharByDbNum(pPacket->ChaDbNum());
	if (pChar)
	{
		NET_CHAR_POINT_AC MsgAC(pPacket->Point());
		SENDTOCLIENT(pChar->ClientSlot(), &MsgAC);
		return TRUE;
	}
	else
	{
		sc::writeLogError(
			sc::string::format(
			"MsgPointShopCharPointFA Char not found. db num(%1%)",
			pPacket->ChaDbNum()));
		return FALSE;
	}
}

void GLAgentServer::ReloadLogItem()
{
    if (m_pItemLimit)
        m_pItemLimit->ReadLogItem();
}