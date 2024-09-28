#pragma once

#include "../../SigmaCore/gassert.h"
#include "../s_NetGlobal.h"
//#include "../Character/GLCharDefine.h"
#include "../Market/GLConsignmentSaleDefine.h"
#include "../../SigmaCore/String/MinGuid.h"
namespace GLMSG
{
    // ConsignmentSale ( 위탁판매 )
    // cwback
    //! REQ = Request     (요청)     
    //! ACK = Acknowledge (응답)
    //NET_MSG_CONSIGNMENT_ADD_ITEM_REQ,               // 아이템 등록 요청
    //NET_MSG_CONSIGNMENT_ADD_ITEM_ACK,               // 아이템 등록 응답
    //NET_MSG_CONSIGNMENT_SALE_ITEM_LIST_REQ,         // 아이템 목록 요청
    //NET_MSG_CONSIGNMENT_SALE_ITEM_LIST_ACK,         // 아이템 목록 응답
    //NET_MSG_CONSIGNMENT_SALE_ITEM_SENDBACK_REQ,     // 아이템 반환 요청
    //NET_MSG_CONSIGNMENT_SALE_ITEM_SENDBACK_ACK,     // 아이템 반환 응답
    //NET_MSG_CONSIGNMENT_SALE_PURCHASE_ACK,          // 아이템 구매요청에 대한 DB응답


    //////////////////////////////////////////////////////////////////////////

    //---------------------------------------------------------------------------NET
    struct NET_CONSIGNMENT_ADD_ITEM_REQ
    {
        //NET_MSG_CONSIGNMENT_ADD_ITEM_REQ

        DWORD m_dwChaDbNum;
        WORD m_nInvenPosX;
        WORD m_nInvenPosY;
        SNATIVEID m_NativeID;

        DWORD m_wTurnNum;
        WORD m_wPeriodIndex;
        LONGLONG m_llPrice;
        BYTE m_bySellType;


        MSGPACK_DEFINE( m_dwChaDbNum, m_nInvenPosX, m_nInvenPosY, m_NativeID, m_wTurnNum, 
                        m_wPeriodIndex, m_llPrice, m_bySellType );
    };

    struct NET_CONSIGNMENT_ADD_ITEM_ACK
    {
        //NET_MSG_CONSIGNMENT_ADD_ITEM_ACK

        bool bAddSucess;
        private_market::ConsignmentSaleItem SaleItem;

        MSGPACK_DEFINE(bAddSucess, SaleItem);
    };

    struct NET_CONSIGNMENT_ITEM_REGIST_REQ
    {
        //NET_MSG_CONSIGNMENT_ITEM_REGIST_REQ

        private_market::ConsignmentSaleItem SaleItem;
        float fCommissionRate;

        MSGPACK_DEFINE( SaleItem, fCommissionRate );
    };


    struct NET_CONSIGNMENT_SALE_ITEM_LIST_REQ
    {
        //NET_MSG_CONSIGNMENT_SALE_ITEM_LIST_REQ,

        DWORD   m_dwChaDbNum;
        MSGPACK_DEFINE(m_dwChaDbNum);

        NET_CONSIGNMENT_SALE_ITEM_LIST_REQ() 
            : m_dwChaDbNum(0) {}
        NET_CONSIGNMENT_SALE_ITEM_LIST_REQ( const DWORD& dwChaDbNum ) 
            : m_dwChaDbNum(dwChaDbNum) {}
    };

    struct NET_CONSIGNMENT_SALE_ITEM_LIST_ACK
    {
        //NET_MSG_CONSIGNMENT_SALE_ITEM_LIST_ACK,
       // 너무 많은 양을 담을 경우 문제가 생긴다.
        private_market::SALE_ITEMS_VECTOR vecSaleItems;
        bool    m_bIsLast;

        MSGPACK_DEFINE(vecSaleItems, m_bIsLast);

        NET_CONSIGNMENT_SALE_ITEM_LIST_ACK() : m_bIsLast(FALSE) {}
    };

    struct NET_CONSIGNMENT_SALE_SLOT_EXPIRATION_DATE_ACK
    {
        //NET_MSG_CONSIGNMENT_SALE_SLOT_EXPIRATION_DATE_ACK

        __time64_t          ExpirationDate; // 물품 만료시간
        bool                bItemUsed;

        MSGPACK_DEFINE(ExpirationDate, bItemUsed);

        NET_CONSIGNMENT_SALE_SLOT_EXPIRATION_DATE_ACK() : ExpirationDate(0), bItemUsed(0) {}
    };


    struct NET_CONSIGNMENT_SALE_ITEM_SENDBACK_REQ
    {
        //NET_MSG_CONSIGNMENT_SALE_ITEM_SENDBACK_REQ,
        std::string strGUID;
        DWORD       dwChaDbNum;
		int nServerGroup;
        MSGPACK_DEFINE( strGUID, dwChaDbNum, nServerGroup );
    };

    struct NET_CONSIGNMENT_SALE_ITEM_SENDBACK_ACK
    {
        //NET_MSG_CONSIGNMENT_SALE_ITEM_SENDBACK_ACK,
        std::string strGUID;
        DWORD       dwChaDbNum;
		int			nResult;
 
        MSGPACK_DEFINE(strGUID, dwChaDbNum, nResult);
    };

    struct NET_CONSIGNMENT_SALE_ITEM_SENDBACK_ACK_DB
    {
        //NET_MSG_CONSIGNMENT_SALE_ITEM_SENDBACK_ACK_DB,
        private_market::ConsignmentSaleItem SaleItem;
        MSGPACK_DEFINE(SaleItem);
    };


    struct NET_CONSIGNMENT_SALE_PURCHAS_REQ
    {
        //NET_MSG_CONSIGNMENT_SALE_PURCHASE_REQ,
        std::string strBuyerName;
        DWORD       dwBuyerDbNum;
        std::string strSellerName;
        DWORD       dwSellerDbNum;

        std::string strGUID;
        int         nPurchaseCount;     // 구매 개수
        LONGLONG    llItemPriceTotal;   // 총 가격( 포인트 or 게임머니 )
        int         nSellType;          // private_market::eSellType

        MSGPACK_DEFINE( strBuyerName, dwBuyerDbNum, strSellerName, dwSellerDbNum, strGUID, nPurchaseCount, llItemPriceTotal, nSellType );

    };

	// 아이템 구매 처리 에러가 발생했다.
	struct NET_CONSIGNMENT_SALE_PURCHASE_FB
	{
		// NET_MSG_CONSIGNMENT_SALE_PURCHASE_ACK_DB,
		std::string strBuyerName;
		DWORD       dwBuyerDbNum;
		std::string strSellerName;
		DWORD       dwSellerDbNum;

		std::string strGUID;
		int         nPurchaseCount;     // 구매 개수
		LONGLONG    llItemPriceTotal;   // 총 가격( 포인트 or 게임머니 )
		int         nSellType;          // private_market::eSellType

		MSGPACK_DEFINE( strBuyerName, dwBuyerDbNum, strSellerName, dwSellerDbNum, strGUID, nPurchaseCount, llItemPriceTotal, nSellType );

	};

	struct NET_CONSIGNMENT_SALE_PAY_MONEY
	{
		// NET_MSG_CONSIGNMENT_SALE_PAY_MONEY_REQ,
		std::string strBuyerName;
		DWORD       dwBuyerDbNum;
		std::string strSellerName;
		DWORD       dwSellerDbNum;

		std::string strGUID;
		int         nPurchaseCount;     // 구매 개수
		LONGLONG    llItemPriceTotal;   // 총 가격( 포인트 or 게임머니 )
		int         nSellType;          // private_market::eSellType
		bool		bSameServerGroup;
		DWORD		dwBuyerFieldSlot;

		MSGPACK_DEFINE( strBuyerName, dwBuyerDbNum, strSellerName, dwSellerDbNum, strGUID, nPurchaseCount, llItemPriceTotal, nSellType, bSameServerGroup, dwBuyerFieldSlot );
	};

	struct NET_CONSIGNMENT_SALE_PAY_MONEY_ACK
	{
		// NET_MSG_CONSIGNMENT_SALE_PAY_MONEY_ACK,
		std::string strBuyerName;
		DWORD       dwBuyerDbNum;
		std::string strSellerName;
		DWORD       dwSellerDbNum;

		std::string strGUID;
		int         nPurchaseCount;     // 구매 개수
		LONGLONG    llItemPriceTotal;   // 총 가격( 포인트 or 게임머니 )
		int         nSellType;          // private_market::eSellType
		bool		bSameServerGroup;
		DWORD		dwBuyerFieldSlot;

		MSGPACK_DEFINE( strBuyerName, dwBuyerDbNum, strSellerName, dwSellerDbNum, strGUID, nPurchaseCount, llItemPriceTotal, nSellType, bSameServerGroup, dwBuyerFieldSlot );
	};

	struct NET_CONSIGNMENT_SALE_UPDATE_ITEM_ACK
	{
		// NET_MSG_CONSIGNMENT_SALE_UPDATE_ITEM_ACK,
		std::string strBuyerName;
		DWORD       dwBuyerDbNum;
		std::string strSellerName;
		DWORD       dwSellerDbNum;

		std::string strGUID;
		int         nPurchaseCount;     // 구매 개수
		LONGLONG    llItemPriceTotal;   // 총 가격( 포인트 or 게임머니 )
		int         nSellType;          // private_market::eSellType
		int			Result;				// 0 success, -1 구매대금 돌려줘야 -2 구매대금이랑 아이템도 돌려줘야
		SITEMCUSTOM	Attach;
		DWORD		dwBuyerFieldSlot;

		MSGPACK_DEFINE( strBuyerName, dwBuyerDbNum, strSellerName, dwSellerDbNum, strGUID, nPurchaseCount, llItemPriceTotal, nSellType, Result, Attach, dwBuyerFieldSlot );
	};

	struct NET_CONSIGNMENT_SALE_SEND_POST_REQ
	{
		// NET_MSG_CONSIGNMENT_SALE_SEND_POST_AND_LOG_REQ,
		std::string strBuyerName;
		DWORD       dwBuyerDbNum;
		std::string strSellerName;
		DWORD       dwSellerDbNum;

		std::string strGUID;
		int         nPurchaseCount;     // 구매 개수
		LONGLONG    llItemPriceTotal;   // 총 가격( 포인트 or 게임머니 )
		int         nSellType;          // private_market::eSellType
		SITEMCUSTOM	Attach;

		MSGPACK_DEFINE( strBuyerName, dwBuyerDbNum, strSellerName, dwSellerDbNum, strGUID, nPurchaseCount, llItemPriceTotal, nSellType, Attach );
	};

    struct NET_CONSIGNMENT_SALE_PURCHASE_ACK
    {
        //NET_MSG_CONSIGNMENT_SALE_PURCHASE_ACK,
        std::string strGUID;
        int         nPurchaseCount;     // 구매 개수

        MSGPACK_DEFINE(  strGUID, nPurchaseCount );

    };






} // namespace GLMSG