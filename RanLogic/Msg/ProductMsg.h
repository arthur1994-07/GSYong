#pragma once

#include "../../SigmaCore/gassert.h"
#include "../s_NetGlobal.h"
#include "../Character/GLCharDefine.h"

#include "../Product/GLProductDataType.h"

namespace GLMSG
{
    //---------------------------------------------------------------------------NET
    struct NET_PRODUCT_BOOK_LIST_REQ
    {
        DWORD   m_dwChaDbNum;
        MSGPACK_DEFINE(m_dwChaDbNum);
        //NET_MSG_REQ_PRODUCTBOOK,            //! 배운 조합서 목록 요청
        NET_PRODUCT_BOOK_LIST_REQ()
            : m_dwChaDbNum(m_dwChaDbNum)
        {}
    };
    struct NET_PRODUCT_BOOK_LIST_ACK
    {
        DWORD   m_dwChaDbNum;
        std::vector<DWORD> m_vecBookList;
        MSGPACK_DEFINE(m_dwChaDbNum, m_vecBookList);
        //NET_MSG_PRODUCTBOOK,          //! 조합서 습득 목록
        NET_PRODUCT_BOOK_LIST_ACK()
            : m_dwChaDbNum(m_dwChaDbNum)
        {}
    };



    struct SNETPC_REQ_LEARN_PRODUCTBOOK : public NET_MSG_GENERIC
    {
        DWORD m_dwChaDbNum;
        DWORD nItemID;
        WORD nPosX;
        WORD nPosY;

        SNETPC_REQ_LEARN_PRODUCTBOOK()
            : m_dwChaDbNum(0)
            , nPosX( 0 )
            , nPosY( 0 )
        {
            dwSize = sizeof( SNETPC_REQ_LEARN_PRODUCTBOOK );
            nType = NET_MSG_REQ_LEARN_PRODUCTBOOK;
        }
    };

    struct SNETPC_REQ_LEARN_PRODUCTBOOK_FB : public NET_MSG_GENERIC
    {
        DWORD dwChaDbNum;
        DWORD nItemID;

        SNETPC_REQ_LEARN_PRODUCTBOOK_FB()
            : nItemID( UINT_MAX )
        {
            dwSize = sizeof( SNETPC_REQ_LEARN_PRODUCTBOOK_FB );
            nType = NET_MSG_REQ_LEARN_PRODUCTBOOK_FB;
        }
    };

    struct SNET_PRODUCT_REQ : public NET_MSG_GENERIC
    {
        DWORD dwChaDbNum;
        DWORD nKey;
        WORD nCount;
		bool bOngoing;

        SNET_PRODUCT_REQ()
            : dwChaDbNum(0)
            , nKey( UINT_MAX )
            , nCount( 1 )
			, bOngoing( false )
        {
            dwSize = sizeof( SNET_PRODUCT_REQ );
            nType = NET_MSG_PRODUCT_REQ;
            MIN_STATIC_ASSERT( sizeof( SNET_PRODUCT_REQ ) <= NET_DATA_BUFSIZE );
        }
    };

    struct SNET_PRODUCT_FB : public NET_MSG_GENERIC
    {
        DWORD dwChaDbNum;
        DWORD nKey;
        WORD nCount;
        BYTE nState;
        DWORD dwCharGaeaID;

        SNET_PRODUCT_FB()
            : dwChaDbNum( 0 )
            , nKey( UINT_MAX )
            , nCount( 1 )
            , nState( 0 )
        {
            dwSize = sizeof( SNET_PRODUCT_FB );
            nType = NET_MSG_PRODUCT_FB;
            MIN_STATIC_ASSERT( sizeof( SNET_PRODUCT_FB ) <= NET_DATA_BUFSIZE );
        }
    };

	struct SNET_PRODUCT_FA : public NET_MSG_GENERIC
	{
		DWORD dwChaDbNum;
		DWORD nKey;
		__int64 iResultItemMakeNum;
		WORD nCount;
		BYTE nState;
		DWORD dwCharGaeaID;
		LONGLONG llMoney;

		SNET_PRODUCT_FA()
			: dwChaDbNum( 0 )
			, nKey( UINT_MAX )
			, iResultItemMakeNum( 0 )
			, nCount( 1 )
			, nState( 0 )
			, dwCharGaeaID( 0 )
			, llMoney( 0 )
		{
			dwSize = sizeof( SNET_PRODUCT_FA );
			nType = NET_MSG_PRODUCT_FA;
			MIN_STATIC_ASSERT( sizeof( SNET_PRODUCT_FA ) <= NET_DATA_BUFSIZE );
		}
	};

	struct NET_PRODUCT_EXCHANGE_LOG_FA : public NET_MSG_GENERIC
	{
		DWORD m_dwDbNum;
		PRODUCT_EXCHANGE_LOG m_exchangeLog;

		NET_PRODUCT_EXCHANGE_LOG_FA()
			: m_dwDbNum( 0 )
		{
			dwSize = sizeof(NET_PRODUCT_EXCHANGE_LOG_FA);
			nType = NET_MSG_PRODUCT_EXCHANGE_LOG_FA;
			MIN_STATIC_ASSERT( sizeof(NET_PRODUCT_EXCHANGE_LOG_FA) <= NET_DATA_BUFSIZE );
		}

		NET_PRODUCT_EXCHANGE_LOG_FA( DWORD dwDbNum, const PRODUCT_EXCHANGE_LOG& exchangeLog )
			: m_dwDbNum( dwDbNum ) 
			, m_exchangeLog( exchangeLog )
		{
			dwSize = sizeof(NET_PRODUCT_EXCHANGE_LOG_FA);
			nType = NET_MSG_PRODUCT_EXCHANGE_LOG_FA;
			MIN_STATIC_ASSERT( sizeof(NET_PRODUCT_EXCHANGE_LOG_FA) <= NET_DATA_BUFSIZE );
		}
	};

    
    // cwback
    // REQ = Request     (요청)
    // ACK = Acknowledge (응답)
    struct NET_PRODUCT_TYPE_LIST_REQ
    {
        DWORD   m_dwChaDbNum;
        MSGPACK_DEFINE(m_dwChaDbNum);
        //NET_MSG_PRODUCT_TYPE_LIST_REQ,
        NET_PRODUCT_TYPE_LIST_REQ()
            : m_dwChaDbNum(m_dwChaDbNum)
        {}
    };

    struct NET_PRODUCT_TYPE_LIST_ACK
    {
        DWORD               m_dwChaDbNum;
        std::vector<WORD>   m_vecType;
        MSGPACK_DEFINE(m_dwChaDbNum, m_vecType);
        //NET_MSG_PRODUCT_TYPE_LIST_ACK,
        NET_PRODUCT_TYPE_LIST_ACK()
            : m_dwChaDbNum(0)
        {}
    };

    struct NET_PRODUCT_TYPE_ADD_REQ
    {
        DWORD   m_dwChaDbNum;
        WORD    m_nProductType;
        MSGPACK_DEFINE(m_dwChaDbNum, m_nProductType);
        //NET_MSG_PRODUCT_TYPE_ADD_REQ,       //! 조합 분야 추가 요청
        NET_PRODUCT_TYPE_ADD_REQ()
            : m_dwChaDbNum(0)
            , m_nProductType(0)
        {}
    };

    struct NET_PRODUCT_TYPE_ADD_ACK
    {
        DWORD   m_dwChaDbNum;
        WORD    m_nProductType;
        bool    m_bOK;
        MSGPACK_DEFINE(m_dwChaDbNum, m_nProductType, m_bOK);
        //NET_MSG_PRODUCT_TYPE_ADD_ACK,       //! 조합 분야 추가 응답
        NET_PRODUCT_TYPE_ADD_ACK()
            : m_dwChaDbNum(0)
            , m_nProductType(0)
            , m_bOK(0)
        {}
    };

    struct NET_PRODUCT_TYPE_DEL_REQ
    {
        DWORD   m_dwChaDbNum;
        std::vector<WORD> m_vecProductType;
        int     m_nInvecPosX;
        int     m_nInvecPosY;

        MSGPACK_DEFINE(m_dwChaDbNum, m_vecProductType, m_nInvecPosX, m_nInvecPosY);
        //NET_MSG_PRODUCT_TYPE_DEL_REQ,       //! 조합 분야 삭제 요청   
        NET_PRODUCT_TYPE_DEL_REQ()
            : m_dwChaDbNum(0)
            , m_nInvecPosX(0)
            , m_nInvecPosY(0)
        {}
    };

    struct NET_PRODUCT_TYPE_DEL_ACK
    {
        DWORD   m_dwChaDbNum;
        std::vector<WORD> m_vecProductType;
        bool    m_bOK;
        MSGPACK_DEFINE(m_dwChaDbNum, m_vecProductType, m_bOK);
        //NET_MSG_PRODUCT_TYPE_DEL_ACK,       //! 조합 분야 삭제 요청
        NET_PRODUCT_TYPE_DEL_ACK()
            : m_dwChaDbNum(0)
            , m_bOK(0)
        {}
    };

    struct NET_PRODUCT_ISGETEXP_LIST_REQ
    {
        DWORD       m_dwChaDbNum;
        MSGPACK_DEFINE(m_dwChaDbNum);
        //NET_MSG_PRODUCT_ISGETEXP_LIST_REQ,  //! 조합으로 숙련도를 습득한 목록 요청
        NET_PRODUCT_ISGETEXP_LIST_REQ()
            : m_dwChaDbNum(0)
        {}
    };

    struct NET_PRODUCT_ISGETEXP_LIST_ACK
    {
        DWORD               m_dwChaDbNum;
		bool	m_bFirst;
		bool	m_bLast;
        std::vector<WORD>  m_vecIsGetExp;
        MSGPACK_DEFINE(m_dwChaDbNum, m_bFirst, m_bLast, m_vecIsGetExp);
        //NET_MSG_PRODUCT_ISGETEXP_LIST_ACK,  //! 조합으로 숙련도를 습득한 목록 응답
        NET_PRODUCT_ISGETEXP_LIST_ACK()
            : m_dwChaDbNum(0)
			, m_bFirst(false)
			, m_bLast(false)
        {}
    };

} // namespace GLMSG