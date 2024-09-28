#include "../pch.h"
#include "../../SigmaCore/Log/LogMan.h"
#include "../../SigmaCore/String/StringFormat.h"

#include "../../RanLogic/Item/GLItemMan.h"
#include "../../RanLogic/GLogicDataMan.h"
#include "../../RanLogic/Msg/CharacterMsg.h"
#include "../../RanLogic/Msg/PointShopMsg.h"

#include "../Util/GLItemLimit.h"
#include "../PointShop/PointShopField.h"
#include "./GLGaeaServer.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void GLGaeaServer::PointShopClear(__time64_t UpdateTime)
{
    m_pPointShop->Clear();
    m_pPointShop->SetUpdateDate(UpdateTime);

#ifndef _RELEASE
    sc::writeLogInfo(
        sc::string::format(
            "PointShopClear %1%", UpdateTime));
#endif
}

void GLGaeaServer::PointShopDataEnd()
{
    sc::writeLogInfo(
		sc::string::format("PointShopDataEnd Item Size %1%", m_pPointShop->Size()));
}

void GLGaeaServer::PointShopDataInsert(const pointshop::SellItem& Item)
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

void GLGaeaServer::SetCharPointShopPoint(DWORD ChaDbNum, const CHARACTER_POINT& Point)
{
    GLChar* pChar = GetCharByDbNum(ChaDbNum);
    if (pChar)
        pChar->PointShopPointSet(Point);
}

BOOL GLGaeaServer::MsgPointShopGmCharPointAddCF(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* pMsg)
{
    GLChar* pChar = GetChar(dwGaeaID);
    if (!pChar)
        return FALSE;

    if (pChar->UserLevel() < USER_GM3)
        return FALSE;

    NET_GM_CHAR_POINT_ADD_CF* pPacket = (NET_GM_CHAR_POINT_ADD_CF*) pMsg;
    __int64 AddPoint = pPacket->AddPoint();
    if (AddPoint <= 0)
        return FALSE;

    pChar->PointShopPointAdd(0, AddPoint, pointshop::GM_COMMAND);
    return TRUE;
}

//! Client->Field:PointShop ������ ���� ��û
BOOL GLGaeaServer::MsgPointShopBuyItemCF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* pMsg )
{    
    GLChar* pChar = GetChar( dwGaeaID );
    if ( !pChar )
        return FALSE;

    pointshop::NET_PS_BUY_ITEM_CF* pPacket = ( pointshop::NET_PS_BUY_ITEM_CF* ) pMsg;
    SNATIVEID BuyItemId = pPacket->ItemId();

    pointshop::NET_PS_BUY_ITEM_FC MsgFC( BuyItemId );

    // Check Item
    const SITEM* pItem = GLogicData::GetInstance().GetItem( BuyItemId );
    if ( !pItem )
    {
        MsgFC.ResultSet( pointshop::NOT_EXIST_ITEM );
        SENDTOCLIENT( dwClientID, &MsgFC );
        return FALSE;
    }

    std::tr1::tuple< bool, __int64 > Result = m_pPointShop->Price( BuyItemId );
    if ( false == std::tr1::get< 0 >( Result ) )
    {
        // PointShop not exist item
        MsgFC.ResultSet( pointshop::BR_NOT_SELL_ITEM );
        SENDTOCLIENT( dwClientID, &MsgFC );
        return FALSE;
    }

    __int64 ItemPrice = std::tr1::get< 1 >( Result );
    __int64 ChaPoint = pChar->PointShopPoint();

    if ( ChaPoint < ItemPrice )
    {
        // Point �� ���ڶ�. ���źҰ�
        MsgFC.ResultSet( pointshop::BR_NOT_ENOUGH_POINT );
        SENDTOCLIENT( dwClientID, &MsgFC );
        return FALSE;
    }
    
    // ������ ����
    SITEMCUSTOM BuyItem;
    if ( !NewItemMake( BuyItem, BuyItemId, EMGEN_POINTSHOP ) )
    {
        // ������ ���� �Ұ�
        MsgFC.ResultSet( pointshop::BR_CAN_NOT_MAKE_ITEM );
        SENDTOCLIENT( dwClientID, &MsgFC );
        return FALSE;
    }

	BuyItem.GenerateBasicStat( false );
	BuyItem.GenerateLinkSkill();
	BuyItem.GenerateAddOption();

    if ( !pChar->InvenInsertItem( BuyItem, true, 189, true, false, true ) )
    {
        // �κ��丮�� �ֱ� ����
        MsgFC.ResultSet( pointshop::NOT_ENOUGH_INVEN_SPACE );
        SENDTOCLIENT( dwClientID, &MsgFC );
        return FALSE;
    }

    // Point ����
    // �̸� �˻��߱� ������ �ݵ�� �����ؾ� �Ѵ�
    pChar->PointShopBuyItem( BuyItemId, ItemPrice, BuyItem );
    SENDTOCLIENT( dwClientID, &MsgFC );
    return TRUE;
}

bool GLGaeaServer::NewItemMake( SITEMCUSTOM& NewItem, const SNATIVEID& ItemId, EMITEMGEN GenType )
{
    m_ItemMakeLock.lockOn();
    const SITEM* pItemData = GLogicData::GetInstance().GetItem( ItemId );
    if ( !pItemData )
    {
        m_ItemMakeLock.lockOff();
        return false;
    }
    
    CTime Time( CTime::GetCurrentTime() );

	// ���ʿ��ϰ� �������� �ʵ��� ������
	NewItem.Initialize( ItemId );
	NewItem.BornTimeSet( Time.GetTime() );
	NewItem.TurnNumSet( pItemData->GETAPPLYNUM() );
	NewItem.GenTypeSet( GenType );
	// NewItem.ChannelNumSet( static_cast< BYTE >( m_nServerChannel ) );
	// NewItem.FieldNumSet( static_cast< BYTE >( m_dwFieldSvrID ) );
	sc::SeqUniqueGUID::Instance()->getGUID( NewItem.guid );
    
    m_ItemMakeLock.lockOff();
    return true;
}

//! Client->Feild:PointShop ī�� ����
BOOL GLGaeaServer::MsgPointShopPointCardCF(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* pMsg)
{
    GLChar* pChar = GetChar(dwGaeaID);
    if (!pChar)
        return FALSE;

    // Point Card ����
    pointshop::NET_PS_POINT_CARD_CF* pPacket = (pointshop::NET_PS_POINT_CARD_CF*) pMsg;
    const SINVENITEM* pInvenItem = pChar->InvenGetItem( pPacket->PosX(), pPacket->PosY() );
    if (!pInvenItem)
        return FALSE;
    
    const SITEM* pItemData = GLogicData::GetInstance().GetItem(pInvenItem->GetNativeID());
    if (!pItemData)
        return FALSE;

    EMITEM_TYPE ItemType = pItemData->BasicType();
    if (ItemType != ITEM_POINT_CARD_REFUND && ItemType != ITEM_POINT_CARD_NOT_REFUND)
        return FALSE;

	// Note : ���� ���� Ȯ��
	//
	if ( !pChar->SIMPLE_CHECK_ITEM( pInvenItem->sItemCustom ) )
	{
		return FALSE;
	}

	// ������ ���� ���� �Ѵ�.
	if ( false == pChar->InvenDeleteItem( pPacket->PosX(), pPacket->PosY(), true, FALSE, 1, true ) )
	{
		return FALSE;
	}

    // Point ����
    DWORD PointCharge = pItemData->GetPointCharge();
    if (ItemType == ITEM_POINT_CARD_REFUND)
        pChar->PointShopPointAdd(PointCharge, 0, pointshop::POINT_CARD);

    if (ItemType == ITEM_POINT_CARD_NOT_REFUND)
        pChar->PointShopPointAdd(0, PointCharge, pointshop::POINT_CARD);    
    
    // ������ ���� �α�
	// ��� �� �α� ����
    m_pItemLimit->LogItemExchange(
        pInvenItem->sItemCustom,
        gamelog::ID_CHAR, pChar->CharDbNum(),
        gamelog::ID_CHAR, 0,
        ITEM_ROUTE_DELETE, pInvenItem->TurnNum(), 
		( pChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pChar->GetCurrentMap(), LOG_ITEM_DEL_POINT_CARD, 
		false);

    // Ŭ���̾�Ʈ�� Point ������ �˸�
    NET_CHAR_POINT_ADD_FC MsgFC(PointCharge);
    SENDTOCLIENT(dwClientID, &MsgFC);
    
    return TRUE;
}

void GLGaeaServer::ReloadLogItem()
{
    if (m_pItemLimit)
        m_pItemLimit->ReadLogItem();
}