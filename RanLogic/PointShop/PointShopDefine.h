#ifndef _POINT_SHOP_DEFINE_H_
#define _POINT_SHOP_DEFINE_H_

#pragma once

#include <vector>
#include "../../EngineLib/G-Logic/TypeDefine.h"
#include "../Item/GLItemDef.h"

namespace pointshop
{

//! �Ǹ� ������
struct SellItem
{
    SNATIVEID m_ItemId; //! ������ ��ȣ
    __int64 m_Price; //! ������ ����

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

//! ���İ����� �׸�
enum SEARCH_ORDER_TYPE
{
    ITEM_MID,
    ITEM_SID,
    ITEM_PRICE,
    ITEM_TYPE,
    ITEM_SUIT,
    ITEM_NAME,
};

//! ���ļ��� 
enum SEARCH_ORDER_DES_ASC
{
    SQL_ASC, //! ��������
    SQL_DESC, //! ��������
};

struct SEARCH_ORDER
{
    SEARCH_ORDER_TYPE m_Type; //! ���� �׸�
    SEARCH_ORDER_DES_ASC m_Degree; //! ��������/��������

    SEARCH_ORDER(SEARCH_ORDER_TYPE Type, SEARCH_ORDER_DES_ASC Degree=SQL_ASC)
        : m_Type(Type)
        , m_Degree(Degree)
    {
    }

    //! ���� �׸�
    inline SEARCH_ORDER_TYPE Type() const { return m_Type; }

    //! ��������/��������
    inline SEARCH_ORDER_DES_ASC Degree() const { return m_Degree; }
};

//! PointShop Point �߰�
enum EM_POINT_CHANGE_TYPE
{
	GM_COMMAND,             //! GM ��ɾ�
	BUY_ITEM,				//! Item ����
	POINT_CARD,             //! Point Card
	CONSIGNMENT_SALE,        //! ����Ʈ �ŷ�
	LOTTO_SYSTEM,			//! �ζ� �ý���
	HOLDEM_SYSTEM,			//! Ȧ�� �ý���
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

//! PointShop ���Ű��
enum BUY_RESULT
{
    BR_OK,                   //! ���ż���
    BR_NOT_ENOUGH_POINT,     //! Point �� ������
    BR_CAN_NOT_MAKE_ITEM,    //! Item �� ������ �� ����
    BR_NOT_SELL_ITEM,        //! �Ǹ����� �ʴ� Item
    NOT_EXIST_ITEM,          //! �������� �ʴ� Item
    NOT_ENOUGH_INVEN_SPACE,  //! �κ��丮�� ������ ����
};

//! �Ǹŵ� ������ ���
struct SELL_ITEM_STATISTICS
{
    SNATIVEID m_ItemId;     //! �Ǹ� ������
    __int64   m_TotalPrice; //! �Ǹ� �ݾ�
    DWORD     m_SellCount;  //! �ǸŰ���

    SELL_ITEM_STATISTICS(const SNATIVEID& ItemId,  __int64 TotalPrice)
        : m_ItemId(ItemId)
        , m_TotalPrice(TotalPrice)
        , m_SellCount(1)
    {
    }
};


} // namespace pointshop
#endif // #ifndef _POINT_SHOP_DEFINE_H_