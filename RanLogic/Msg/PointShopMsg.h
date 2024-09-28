#ifndef _GL_POINT_SHOP_MSG_H_
#define _GL_POINT_SHOP_MSG_H_

#pragma once

#include "../../SigmaCore/gassert.h"
#include "../s_NetGlobal.h"
#include "../PointShop/PointShopDefine.h"

namespace pointshop
{
    //! Database -> Server:Point Shop Data 시작
    struct NET_DATA_START_DS : public NET_MSG_GENERIC
    {
        __time64_t m_ShopUpdateDate;

        NET_DATA_START_DS(__time64_t ShopUpdateDate)
            : m_ShopUpdateDate(ShopUpdateDate)
        {
            dwSize = sizeof(NET_DATA_START_DS);
            nType = NET_MSG_PS_DATA_START_DS;
            MIN_STATIC_ASSERT(sizeof(NET_DATA_START_DS)<=NET_DATA_BUFSIZE);
        }

        inline __time64_t ShopUpdateDate() const { return m_ShopUpdateDate; }
    };

    //! Cache Server->Server
    struct NET_DATA_START_HS : public NET_DATA_START_DS
    {
        NET_DATA_START_HS(__time64_t ShopUpdateDate)
            : NET_DATA_START_DS(ShopUpdateDate)
        {
            dwSize = sizeof(NET_DATA_START_HS);
            nType = NET_MSG_PS_DATA_START_HS;
            MIN_STATIC_ASSERT(sizeof(NET_DATA_START_HS)<=NET_DATA_BUFSIZE);
        }
    };

    //! Agent->Client
    struct NET_DATA_START_AC : public NET_DATA_START_DS
    {
        NET_DATA_START_AC(__time64_t ShopUpdateDate)
            : NET_DATA_START_DS(ShopUpdateDate)
        {
            dwSize = sizeof(NET_DATA_START_AC);
            nType = NET_MSG_PS_DATA_START_AC;
            MIN_STATIC_ASSERT(sizeof(NET_DATA_START_AC)<=NET_DATA_BUFSIZE);
        }
    };

    //! Database -> Server:Point Shop Data 끝
    struct NET_DATA_END_DS : public NET_MSG_GENERIC
    {
        bool m_bReload;
        NET_DATA_END_DS(bool bReload)
            : m_bReload(bReload)
        {
            dwSize = sizeof(NET_DATA_END_DS);
            nType = NET_MSG_PS_DATA_END_DS;
            MIN_STATIC_ASSERT(sizeof(NET_DATA_END_DS)<=NET_DATA_BUFSIZE);
        }

        inline bool IsReload() const { return m_bReload; }
    };

    //! Cache Server->Server
    struct NET_DATA_END_HS : public NET_DATA_END_DS
    {
        NET_DATA_END_HS(bool bReload)
            : NET_DATA_END_DS(bReload)
        {
            dwSize = sizeof(NET_DATA_END_HS);
            nType = NET_MSG_PS_DATA_END_HS;
            MIN_STATIC_ASSERT(sizeof(NET_DATA_END_HS)<=NET_DATA_BUFSIZE);
        }
    };

    //! Agent Server->Client
    struct NET_DATA_END_AC : public NET_MSG_GENERIC
    {
        NET_DATA_END_AC()
        {
            dwSize = sizeof(NET_DATA_END_AC);
            nType = NET_MSG_PS_DATA_END_AC;
            MIN_STATIC_ASSERT(sizeof(NET_DATA_END_AC)<=NET_DATA_BUFSIZE);
        }
    };

    //! Database -> Server:Point Shop Data
    struct NET_DATA_DS : public NET_MSG_GENERIC
    {
        SellItem m_Data;
        NET_DATA_DS(SellItem Data)
            : m_Data(Data)
        {
            dwSize = sizeof(NET_DATA_DS);
            nType = NET_MSG_PS_DATA_DS;
            MIN_STATIC_ASSERT(sizeof(NET_DATA_DS)<=NET_DATA_BUFSIZE);
        }

        inline SellItem Data() const { return m_Data; }
    };

    //! Cache server->Server:Point Shop data
    struct NET_DATA_HS : public NET_DATA_DS
    {
        NET_DATA_HS(SellItem Data)
            : NET_DATA_DS(Data)
        {
            dwSize = sizeof(NET_DATA_HS);
            nType = NET_MSG_PS_DATA_HS;
            MIN_STATIC_ASSERT(sizeof(NET_DATA_HS)<=NET_DATA_BUFSIZE);
        }
    };

    //! Agent->Client:Point Shop data
    struct NET_DATA_AC : public NET_DATA_DS
    {
        NET_DATA_AC(SellItem Data)
            : NET_DATA_DS(Data)
        {
            dwSize = sizeof(NET_DATA_AC);
            nType = NET_MSG_PS_DATA_AC;
            MIN_STATIC_ASSERT(sizeof(NET_DATA_AC)<=NET_DATA_BUFSIZE);
        }
    };

    //! Agent, Field->Cache Server:Point Shop 정보 요청
    struct NET_REQ_DATA_SH : public NET_MSG_GENERIC
    {
        NET_REQ_DATA_SH()
        {
            dwSize = sizeof(NET_REQ_DATA_SH);
            nType = NET_MSG_PS_REQ_DATA_SH;
            MIN_STATIC_ASSERT(sizeof(NET_REQ_DATA_SH)<=NET_DATA_BUFSIZE);
        }
    };

    //! GM Client->Agent:Point Shop Reload 요청
    struct NET_GM_RELOAD_CA : public NET_MSG_GENERIC
    {
        NET_GM_RELOAD_CA()
        {
            dwSize = sizeof(NET_GM_RELOAD_CA);
            nType = NET_MSG_PS_GM_RELOAD_CA;
            MIN_STATIC_ASSERT(sizeof(NET_GM_RELOAD_CA)<=NET_DATA_BUFSIZE);
        }
    };

    //! Agent->Session:Point Shop Reload 요청
    struct NET_RELOAD_AS : public NET_MSG_GENERIC
    {
        NET_RELOAD_AS()
        {
            dwSize = sizeof(NET_RELOAD_AS);
            nType = NET_MSG_PS_RELOAD_AS;
            MIN_STATIC_ASSERT(sizeof(NET_RELOAD_AS)<=NET_DATA_BUFSIZE);
        }
    };

    //! Session->Server:Point Shop Reload 요청
    struct NET_RELOAD_SS : public NET_MSG_GENERIC
    {
        NET_RELOAD_SS()
        {
            dwSize = sizeof(NET_RELOAD_SS);
            nType = NET_MSG_PS_RELOAD_SS;
            MIN_STATIC_ASSERT(sizeof(NET_RELOAD_SS)<=NET_DATA_BUFSIZE);
        }
    };

    //! Client->Agent:Point Shop Update date 전송
    struct NET_UPDATE_DATE_CA : public NET_MSG_GENERIC
    {
        __time64_t m_UpdateDate;
        NET_UPDATE_DATE_CA(__time64_t UpdateDate)
            : m_UpdateDate(UpdateDate)
        {
            dwSize = sizeof(NET_UPDATE_DATE_CA);
            nType = NET_MSG_PS_UPDATE_DATE_CA;
            MIN_STATIC_ASSERT(sizeof(NET_UPDATE_DATE_CA)<=NET_DATA_BUFSIZE);
        }

        inline __time64_t UpdateDate() const { return m_UpdateDate; }
    };

    //! Client->Field:Point Shop 아이템 구매요청
    struct NET_PS_BUY_ITEM_CF : public NET_MSG_GENERIC
    {
        SNATIVEID m_ItemId; //! 구매할 Item
        NET_PS_BUY_ITEM_CF(const SNATIVEID& ItemId)
            : m_ItemId(ItemId)
        {
            dwSize = sizeof(NET_PS_BUY_ITEM_CF);
            nType = NET_MSG_PS_BUY_ITEM_CF;
            MIN_STATIC_ASSERT(sizeof(NET_PS_BUY_ITEM_CF)<=NET_DATA_BUFSIZE);
        }

        inline SNATIVEID ItemId() const { return m_ItemId; }
    };

    //! Field->Client:Point Shop 구매 결과
    struct NET_PS_BUY_ITEM_FC : public NET_MSG_GENERIC
    {
        SNATIVEID  m_BuyItem;
        BUY_RESULT m_BuyResult;
        
        NET_PS_BUY_ITEM_FC(const SNATIVEID& BuyItem, BUY_RESULT BuyResult=BR_OK)
            : m_BuyItem(BuyItem)
            , m_BuyResult(BuyResult)
        {
            dwSize = sizeof(NET_PS_BUY_ITEM_FC);
            nType = NET_MSG_PS_BUY_ITEM_CF;
            MIN_STATIC_ASSERT(sizeof(NET_PS_BUY_ITEM_FC)<=NET_DATA_BUFSIZE);
        }

        inline BUY_RESULT Result() const { return m_BuyResult; }
        inline void ResultSet(BUY_RESULT BuyResult) { m_BuyResult=BuyResult; }
        inline SNATIVEID BuyItem() const { return m_BuyItem; }
        inline void BuyItemSet(SNATIVEID  BuyItem) { m_BuyItem=BuyItem; }
    };

    //! Client->Field:Point 충전요청
    struct NET_PS_POINT_CARD_CF : public NET_MSG_GENERIC
    {
        WORD m_PosX; //! 인벤토리 위치 X
        WORD m_PosY; //! 인벤토리 위치 Y
        
        NET_PS_POINT_CARD_CF(WORD PosX, WORD PosY)
            : m_PosX(PosX)
            , m_PosY(PosY)
        {
            dwSize = sizeof(NET_PS_POINT_CARD_CF);
            nType = NET_MSG_PS_POINT_CARD_CF;
            MIN_STATIC_ASSERT(sizeof(NET_PS_POINT_CARD_CF)<=NET_DATA_BUFSIZE);
        }
        
        inline WORD PosX() const { return m_PosX; }
        inline WORD PosY() const { return m_PosY; }
    };

} // namespace pointshop


#endif // _GL_POINT_SHOP_MSG_H_