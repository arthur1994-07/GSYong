#include "pch.h"
#include "../SigmaCore/String/StringFormat.h"

#include "../EngineLib/GUInterface/GameTextControl.h"

#include "../RanLogic/Msg/PointShopMsg.h"
#include "../RanLogic/Msg/CharacterMsg.h"
#include "../RanLogic/Item/GLItemMan.h"
#include "../RanLogic/GLogicDataMan.h"

#include "./Char/GLCharacter.h"
#include "./PointShop/PointShopClient.h"
#include "./GLGaeaClient.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void GLGaeaClient::PointShopGmReload()
{    
    if (GetCharacter()->UserLevel() < USER_GM3)
        return;
    pointshop::NET_GM_RELOAD_CA MsgCA;
    NETSENDTOAGENT(&MsgCA);
}

void GLGaeaClient::PointShopCacheLoad()
{
    m_pPointShop->Load();
}

void GLGaeaClient::SendPointShopUpdateDate()
{
    pointshop::NET_UPDATE_DATE_CA MsgCA(m_pPointShop->UpdateDate());
    NETSENDTOAGENT(&MsgCA);
}

void GLGaeaClient::PointShopGmView()
{
    if (GetCharacter()->UserLevel() < USER_GM3)
        return;

    std::vector<pointshop::SellItemClient> ItemList;
    std::string SearchName("");
    std::vector<EMITEM_TYPE> TypeList;
    std::vector<pointshop::SEARCH_ORDER> OrderList;

    m_pPointShop->GetList(ItemList, SearchName, TypeList, OrderList);

    for (size_t i=0; i<ItemList.size(); ++i)
    {
        const pointshop::SellItemClient& Item = ItemList[i];        
		if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
		{
			PrintConsoleText(
				sc::string::format(
				"(%1%/%2%) Price:%3% %4%/%5%/%6%",
				Item.ItemMid(),
				Item.ItemSid(),
				Item.Price(),
				Item.Name(),
				Item.Type() < ITEM_NSIZE ? CGameTextMan::GetInstance().GetCommentText("ITEMTYPE", Item.Type()).GetString() : "",
				Item.Type() < SUIT_NSIZE ? CGameTextMan::GetInstance().GetCommentText("ITEMSUIT", Item.Suit()).GetString() : ""));
		}
		else
		{
			PrintConsoleText(
				sc::string::format(
				"(%1%/%2%) Price:%3% %4%/%5%/%6%",
				Item.ItemMid(),
				Item.ItemSid(),
				Item.Price(),
				Item.Name(),
				COMMENT::ItemTypeName(Item.Type()),
				COMMENT::ItemSuitName(Item.Suit())));
		}
    }
}

void GLGaeaClient::MsgPointShopDataStartAC(NET_MSG_GENERIC* pMsg)
{
    pointshop::NET_DATA_START_AC* pPacket = (pointshop::NET_DATA_START_AC*) pMsg;
    // Point Shop UI 가 활성화된 상태이면, 안내메시지와 함께 닫는다.
    m_pPointShop->Clear();
    m_pPointShop->SetUpdateDate(pPacket->ShopUpdateDate());

// #ifndef NDEBUG
//     PrintConsoleText(
//         sc::string::format(
//             "MsgPointShopDataStartAC %1%", pPacket->ShopUpdateDate()));
// #endif

	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_PointShop_Close );
}

void GLGaeaClient::MsgPointShopDataAC(NET_MSG_GENERIC* pMsg)
{
    pointshop::NET_DATA_AC* pPacket = (pointshop::NET_DATA_AC*) pMsg;
    m_pPointShop->InsertItem(pPacket->Data());

// #ifndef NDEBUG
//     const pointshop::SellItem& Item = pPacket->Data();
//     PrintConsoleText(
//         sc::string::format(
//             "MsgPointShopDataAC %1%/%2% %3%",
//             Item.ItemMid(),
//             Item.ItemSid(),
//             Item.Price()));
// #endif
}

void GLGaeaClient::MsgPointShopEndAC(NET_MSG_GENERIC* pMsg)
{
    pointshop::NET_DATA_END_AC* pPacket = (pointshop::NET_DATA_END_AC*) pMsg;
    m_pPointShop->Save();

// #ifndef NDEBUG
//     PrintConsoleText(std::string("MsgPointShopEndAC"));
// #endif
	if ( GLUseFeatures::GetInstance().IsUsingPointShop() )
	{
		GLWidgetScript::GetInstance().LuaCallFunc< void >(
			NSWIDGET_SCRIPT::g_strFunc_PointShop_Open );
	}
}

void GLGaeaClient::MsgPointShopCharPointAC(NET_MSG_GENERIC* pMsg)
{
    NET_CHAR_POINT_AC* pPacket = (NET_CHAR_POINT_AC*) pMsg;
    const CHARACTER_POINT& Point = pPacket->Point();
    m_pCharacter->PointShopPointSet(Point);

// #ifndef NDEBUG
//     PrintConsoleText(
//         sc::string::format(
//             "MsgPointShopCharPointFC %1% %2%",
//             Point.Refundable(),
//             Point.NotRefundable()));
// #endif

	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_Inventory_UpdatePoint,
		"-l", m_pCharacter->PointShopPoint() );
}

void GLGaeaClient::PointShopAddPoint(__int64 AddPoint)
{
    if (GetCharacter()->UserLevel() < USER_GM3 || AddPoint <= 0)
        return;

    NET_GM_CHAR_POINT_ADD_CF MsgCF(AddPoint);
    NETSENDTOFIELD(&MsgCF);
}

//! Point Shop 에서 구매가능한 Character Point
__int64 GLGaeaClient::PointShopChaPoint() const
{
    return m_pCharacter->PointShopPoint();
}

//! Point Shop 에서 Item 구매
bool GLGaeaClient::PointShopBuyItem(const SNATIVEID& ItemId)
{
    SITEM* pItem = GLogicData::GetInstance().GetItem(ItemId);
    if (!pItem)
    {
        // 존재하지 않는 아이템
        PrintConsoleText(
            sc::string::format(
                ID2GAMEINTEXT("POINT_SHOP_NO_EXIST_ITEM"),
                ItemId.Mid(),
                ItemId.Sid()));
        return false;
    }

    std::tr1::tuple<bool, __int64> ItemExistPrice = m_pPointShop->ExistItem(ItemId);
    if (false == std::tr1::get<0> (ItemExistPrice))
    {
        // 판매하지 않는 아이템이다
        PrintConsoleText(
            sc::string::format(
                ID2GAMEINTEXT("POINT_SHOP_NO_SELL_ITEM"),
                ItemId.Mid(),
                ItemId.Sid(),
                pItem->GetName()));
        return false;
    }

    // 캐릭터 Point 체크
    __int64 ItemPrice = std::tr1::get<1> (ItemExistPrice);
    __int64 ChaPoint = m_pCharacter->PointShopPoint();
    if (ChaPoint < ItemPrice)
    {
        PrintConsoleText(
            sc::string::format(
                ID2GAMEINTEXT("POINT_SHOP_NOT_ENOUGH_POINT"),
                ItemPrice,
                ChaPoint));
        return false;
    }

    if (m_pCharacter->InvenAmountEmpty(false) < 1)
    {
        PrintConsoleText(
            std::string(
                ID2GAMEINTEXT("POINT_SHOP_NOT_ENOUGH_INVENTORY")));
        return false;
    }

    pointshop::NET_PS_BUY_ITEM_CF MsgCF(ItemId);
    NETSENDTOFIELD(&MsgCF);
    return true;
}

void GLGaeaClient::MsgPointShopBuyItemCF(NET_MSG_GENERIC* pMsg)
{
    pointshop::NET_PS_BUY_ITEM_FC* pPacket = (pointshop::NET_PS_BUY_ITEM_FC*) pMsg;

    std::string Message;
    switch (pPacket->Result())
    {
    case pointshop::BR_OK: // 구매성공
        Message.append(
            sc::string::format(
                ID2GAMEINTEXT("POINT_SHOP_FC_BUY_ITEM"),
                GLogicData::GetInstance().GetItemName(pPacket->BuyItem())));
        break;

    case pointshop::BR_NOT_ENOUGH_POINT:  // Point 가 부족함
        Message.append(
            std::string(
                ID2GAMEINTEXT("POINT_SHOP_FC_NOT_ENOUGH_POINT")));
        break;

    case pointshop::BR_CAN_NOT_MAKE_ITEM: // Item 을 생성할 수 없음
        Message.append(
            std::string(
                ID2GAMEINTEXT("POINT_SHOP_FC_NOT_CREATE_ITEM")));
        break;

    case pointshop::BR_NOT_SELL_ITEM: // 판매하지 않는 Item
        Message.append(
            std::string(
                ID2GAMEINTEXT("POINT_SHOP_FC_NOT_SELL_ITEM")));
        break;

    case pointshop::NOT_EXIST_ITEM: // 존재하지 않는 Item
        Message.append(
            std::string(
                ID2GAMEINTEXT("POINT_SHOP_FC_NOT_EXIST_ITEM")));
        break;

    case pointshop::NOT_ENOUGH_INVEN_SPACE: // 인벤토리에 공간이 없음
        Message.append(
            std::string(
                ID2GAMEINTEXT("POINT_SHOP_FC_NOT_ENOUGH_INVENTORY")));
        break;

    default:
        Message.append(
            std::string(
                ID2GAMEINTEXT("POINT_SHOP_FC_UNKNOWN_ERROR")));
        break;
    };

    PrintConsoleText(Message);
}

//! Point Card 충전요청
void GLGaeaClient::ReqInvenDrugPointCard(WORD PosX, WORD PosY)
{
    pointshop::NET_PS_POINT_CARD_CF MsgCF(PosX, PosY);
    NETSENDTOFIELD(&MsgCF);
}

//! Field->Client:Character Point 충전됨
void GLGaeaClient::MsgPointShopAddPointFC(NET_MSG_GENERIC* pMsg)
{
    NET_CHAR_POINT_ADD_FC* pPacket = (NET_CHAR_POINT_ADD_FC*) pMsg;
    
    std::string Message(
        sc::string::format(
            ID2GAMEINTEXT("POINT_SHOP_FC_ADD_POINT"),
            pPacket->AddPoint()));
    PrintConsoleText(Message);
}
