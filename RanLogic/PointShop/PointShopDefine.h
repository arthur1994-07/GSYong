#ifndef _POINT_SHOP_DEFINE_H_
#define _POINT_SHOP_DEFINE_H_

#pragma once

#include <vector>
#include "../../EngineLib/G-Logic/TypeDefine.h"
#include "../Item/GLItemDef.h"

namespace pointshop
{

//! 판매 아이템
struct SellItem
{
    SNATIVEID m_ItemId; //! 아이템 번호
    __int64 m_Price; //! 아이템 가격

    SellItem()
        : m_Price(0)
    {
    }

    SellItem(WORD Mid, WORD Sid, __int64 _Price)
        : m_ItemId(Mid, Sid)
        , m_Price(_Price)
    {
    }

    SellItem(SNATIVEID _ItemId, __int64 _Price)
        : m_ItemId(_ItemId)
        , m_Price(_Price)
    {
    }

    inline SNATIVEID ItemId() const { return m_ItemId; }
    inline WORD ItemMid() const { return m_ItemId.Mid(); }
    inline WORD ItemSid() const { return m_ItemId.Sid(); }
    inline __int64 Price() const { return m_Price; }
};

struct SellItemClient : public SellItem
{
    EMITEM_TYPE m_Type;
    EMSUIT m_Suit;
    std::string m_ItemName;

    SellItemClient(WORD Mid, WORD Sid, __int64 Price,
        EMITEM_TYPE Type, EMSUIT Suit, const std::string& ItemName)
        : SellItem(Mid, Sid, Price)
        , m_Type(Type)
        , m_Suit(Suit)
        , m_ItemName(ItemName)
    {
    }

    inline EMITEM_TYPE Type() const { return m_Type; }
    inline EMSUIT Suit() const { return m_Suit; }
    inline std::string Name() const { return m_ItemName; }
};

//! 정렬가능한 항목
enum SEARCH_ORDER_TYPE
{
    ITEM_MID,
    ITEM_SID,
    ITEM_PRICE,
    ITEM_TYPE,
    ITEM_SUIT,
    ITEM_NAME,
};

//! 정렬순서 
enum SEARCH_ORDER_DES_ASC
{
    SQL_ASC, //! 오름차순
    SQL_DESC, //! 내림차순
};

struct SEARCH_ORDER
{
    SEARCH_ORDER_TYPE m_Type; //! 정렬 항목
    SEARCH_ORDER_DES_ASC m_Degree; //! 오름차순/내림차순

    SEARCH_ORDER(SEARCH_ORDER_TYPE Type, SEARCH_ORDER_DES_ASC Degree=SQL_ASC)
        : m_Type(Type)
        , m_Degree(Degree)
    {
    }

    //! 정렬 항목
    inline SEARCH_ORDER_TYPE Type() const { return m_Type; }

    //! 오름차순/내림차순
    inline SEARCH_ORDER_DES_ASC Degree() const { return m_Degree; }
};

//! PointShop Point 추가
enum EM_POINT_CHANGE_TYPE
{
	GM_COMMAND,             //! GM 명령어
	BUY_ITEM,				//! Item 구매
	POINT_CARD,             //! Point Card
	CONSIGNMENT_SALE,        //! 포인트 거래
	LOTTO_SYSTEM,			//! 로또 시스템
	HOLDEM_SYSTEM,			//! 홀덤 시스템
	POST_SYSTEM,
	NPC_EXCHANGE_ITEM, 
	CONSIGNMENT_SALE_ERROR, 
};

enum EM_POINT_SUB_TYPE
{
	POINT_SUB_TYPE_EXCHANGE_NPC = 1,
	POINT_SUB_TYPE_GRABAGE = 2,
	POINT_SUB_TYPE_HOLDEM = 3,
	POINT_SUB_TYPE_SHOP_BUY = 4,
	POINT_SUB_TYPE_CHARGE_POINT = 5,
	POINT_SUB_TYPE_LOTTO = 6,
	POINT_SUB_TYPE_POST = 7,
	POINT_SUB_TYPE_CONSIGNMENT_ADD = 8,
	POINT_SUB_TYPE_CINSIGNMENT_BUY = 9,
};


struct UI_OPTION
{
    std::string m_OptionName;
    std::vector<EMITEM_TYPE> m_TypeVec;

    UI_OPTION()
    {
    }

    inline std::string OptionName() const { return m_OptionName; }
    inline const std::vector<EMITEM_TYPE>& TypeVec() const { return m_TypeVec; }
};

//! PointShop 구매결과
enum BUY_RESULT
{
    BR_OK,                   //! 구매성공
    BR_NOT_ENOUGH_POINT,     //! Point 가 부족함
    BR_CAN_NOT_MAKE_ITEM,    //! Item 을 생성할 수 없음
    BR_NOT_SELL_ITEM,        //! 판매하지 않는 Item
    NOT_EXIST_ITEM,          //! 존재하지 않는 Item
    NOT_ENOUGH_INVEN_SPACE,  //! 인벤토리에 공간이 없음
};

//! 판매된 아이템 통계
struct SELL_ITEM_STATISTICS
{
    SNATIVEID m_ItemId;     //! 판매 아이템
    __int64   m_TotalPrice; //! 판매 금액
    DWORD     m_SellCount;  //! 판매개수

    SELL_ITEM_STATISTICS(const SNATIVEID& ItemId,  __int64 TotalPrice)
        : m_ItemId(ItemId)
        , m_TotalPrice(TotalPrice)
        , m_SellCount(1)
    {
    }
};


} // namespace pointshop
#endif // #ifndef _POINT_SHOP_DEFINE_H_