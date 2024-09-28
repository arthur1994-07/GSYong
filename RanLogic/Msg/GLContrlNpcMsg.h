#pragma once

#include "GLContrlBaseMsg.h"

enum EMQUEST_START_FB
{
	EMQUEST_START_FB_OK,	//	성공.
	EMQUEST_START_FB_FAIL,	//	실패.
	EMQUEST_START_FB_MINIMUM_PLAYER, // 인원 부족.
	EMQUEST_START_FB_MAXIMUM_PLAYER, // 인원 초과.
	EMQUEST_START_FB_ALREADY,	//	이미 수행중.
	EMQUEST_START_FB_DONAGAIN,	//	재시도 불가능.
	EMQUEST_START_FB_DONREPEAT,	//	재시도 불가능.
	EMQUEST_START_FB_CONDITION,	//	조건에 부적합.
	EMQUEST_START_FB_NEEDMONEY,	//	금액이 부족.
};

namespace GLMSG
{
	//#pragma pack(1)

	//---------------------------------------------------------------------------NET
	struct SNETPC_REQ_REGEN_GATE : public NET_MSG_CHARACTER//NET_MSG_GENERIC
	{
		DWORD				dwNpcGlobID;

		SNETPC_REQ_REGEN_GATE (DWORD _dwNpcGlobID) 
			: dwNpcGlobID(_dwNpcGlobID)
		{
			dwSize = sizeof(SNETPC_REQ_REGEN_GATE);
			nType = NET_MSG_GCTRL_REGEN_GATE;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_REGEN_GATE)<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_REQ_REGEN_GATE_FB : public NET_MSG_CHARACTER//NET_MSG_GENERIC
	{
		EMREGEN_GATE_FB		emFB;

		SNATIVEID			sMapID;
		DWORD				dwGateID;

		SNETPC_REQ_REGEN_GATE_FB (EMREGEN_GATE_FB _emFB, SNATIVEID& _sMapID, DWORD _dwGateID) 
			: emFB(_emFB)
			, sMapID(_sMapID)
			, dwGateID(0)
		{
			dwSize = sizeof(SNETPC_REQ_REGEN_GATE_FB);
			nType = NET_MSG_GCTRL_REGEN_GATE_FB;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_REGEN_GATE_FB)<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_REQ_CURE : public NET_MSG_GENERIC
	{
		DWORD				dwTalkID;
		DWORD				dwNpcGlobID;

		SNETPC_REQ_CURE (DWORD _dwTalkID, DWORD _dwNpcGlobID) 
			: dwNpcGlobID(_dwNpcGlobID)
			, dwTalkID(_dwTalkID)
		{
			dwSize = sizeof(SNETPC_REQ_CURE);
			nType = NET_MSG_GCTRL_CURE;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_CURE)<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_REQ_CURE_FB : public NET_MSG_GENERIC
	{
		EMREGEN_CURE_FB		emFB;
		DWORD				dwCUREFLAG;
		GLDWDATA			sHP;
        GLDWDATA			sMP;
        GLDWDATA			sSP;

		SNETPC_REQ_CURE_FB (EMREGEN_CURE_FB _emFB, DWORD _dwCUREFLAG,
            GLDWDATA& _sHP, GLDWDATA& _sMP, GLDWDATA& _sSP) 
			: emFB(_emFB)
			, dwCUREFLAG(_dwCUREFLAG)
            , sHP(_sHP)
            , sMP(_sMP)
            , sSP(_sSP)
		{
			dwSize = sizeof(SNETPC_REQ_CURE_FB);
			nType = NET_MSG_GCTRL_CURE_FB;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_CURE_FB)<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_REQ_CHARRESET : public NET_MSG_GENERIC
	{
		DWORD dwNpcGlobID;

		SNETPC_REQ_CHARRESET (DWORD _dwNpcGlobID) 
			: dwNpcGlobID(_dwNpcGlobID)
		{
			dwSize = sizeof(SNETPC_REQ_CHARRESET);
			nType = (EMNET_MSG)NET_MSG_GCTRL_CHARRESET;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_CHARRESET)<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_REQ_CHARRESET_FB : public NET_MSG_GENERIC
	{
		EMREGEN_CHARRESET_FB	emFB;
		DWORD					dwSTATS_P;
		DWORD					dwSKILL_P;

		SNETPC_REQ_CHARRESET_FB (EMREGEN_CHARRESET_FB _emFB, DWORD _dwSTATS_P, DWORD _dwSKILL_P) 
			: emFB(_emFB)
			, dwSTATS_P(_dwSTATS_P)
			, dwSKILL_P(_dwSKILL_P)
		{
			dwSize = sizeof(SNETPC_REQ_CHARRESET_FB);
			nType = (EMNET_MSG)NET_MSG_GCTRL_CHARRESET_FB;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_CHARRESET_FB)<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_REQ_NPC_ITEM_TRADE : public NET_MSG_GENERIC
	{
		DWORD				dwNpcGlobID;
		DWORD				dwTalkID;

		SNETPC_REQ_NPC_ITEM_TRADE (DWORD _dwNpcGlobID, DWORD _dwTalkID) 
			: dwNpcGlobID(_dwNpcGlobID)
			, dwTalkID(_dwTalkID)
		{

			dwSize = sizeof(SNETPC_REQ_NPC_ITEM_TRADE);
			nType = (EMNET_MSG)NET_MSG_GCTRL_NPC_ITEM_TRADE;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_NPC_ITEM_TRADE)<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_REQ_NPC_ITEM_TRADE_FB : public NET_MSG_GENERIC
	{
		EMNPC_ITEM_TRADE_FB	emFB;

		SNETPC_REQ_NPC_ITEM_TRADE_FB (EMNPC_ITEM_TRADE_FB _emFB) 
			: emFB(_emFB)
		{
			dwSize = sizeof(SNETPC_REQ_NPC_ITEM_TRADE_FB);
			nType = (EMNET_MSG)NET_MSG_GCTRL_NPC_ITEM_TRADE_FB;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_NPC_ITEM_TRADE_FB)<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_REQ_QUEST_START : public NET_MSG_GENERIC
	{
		DWORD				dwNpcGlobID;
		DWORD				dwTalkID;
		DWORD				dwQuestID;

		SNETPC_REQ_QUEST_START (DWORD _dwNpcGlobID, DWORD _dwTalkID, DWORD _dwQuestID) 
			: dwNpcGlobID(_dwNpcGlobID)
			, dwTalkID(_dwTalkID)
			, dwQuestID(_dwQuestID)
		{
			dwSize = sizeof(SNETPC_REQ_QUEST_START);
			nType = (EMNET_MSG)NET_MSG_GCTRL_REQ_QUEST_START;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_QUEST_START)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_REQ_QUEST_START_WITHOUT_NPC : public NET_MSG_GENERIC
	{
		DWORD				dwQuestID;

		SNETPC_REQ_QUEST_START_WITHOUT_NPC (DWORD _dwQuestID) 
			: dwQuestID(_dwQuestID)
		{
			dwSize = sizeof(SNETPC_REQ_QUEST_START_WITHOUT_NPC);
			nType = (EMNET_MSG)NET_MSG_GCTRL_REQ_QUEST_START_WITHOUT_NPC;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_QUEST_START_WITHOUT_NPC)<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_REQ_QUEST_START_FB : public NET_MSG_GENERIC
	{
		EMQUEST_START_FB	emFB;
		DWORD dwVar;

		SNETPC_REQ_QUEST_START_FB (EMQUEST_START_FB _emFB) 
			: emFB(_emFB)
		{
			dwSize = sizeof(SNETPC_REQ_QUEST_START_FB);
			nType = (EMNET_MSG)NET_MSG_GCTRL_REQ_QUEST_START_FB;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_QUEST_START_FB)<=NET_DATA_BUFSIZE);
		}
	};

    // 재매입 리스트에 추가. Field -> Agent
    struct SNET_ADD_REBUY_ITEM_FA : public NET_MSG_CHARACTER
    {
        SITEMCUSTOM sItemCustom;
        LONGLONG Price;

        SNET_ADD_REBUY_ITEM_FA(const SITEMCUSTOM& _sItemCustom, LONGLONG _Price )
            : sItemCustom(_sItemCustom)
            , Price(_Price)
        {
            dwSize = sizeof( SNET_ADD_REBUY_ITEM_FA );
            nType = NET_MSG_ADD_REBUY_ITEM_FA;
            MIN_STATIC_ASSERT( sizeof(SNET_ADD_REBUY_ITEM_FA)<= NET_DATA_BUFSIZE );
        }
    };

    // 재매입 리스트에 추가 결과. Agent -> Client
    struct SNET_ADD_REBUY_ITEM_AC : public NET_MSG_GENERIC
    {
        SITEMCUSTOM sItemCustom;
        LONGLONG Price;

        SNET_ADD_REBUY_ITEM_AC(const SITEMCUSTOM& _sItemCustom, LONGLONG _Price )
            : sItemCustom(_sItemCustom)
            , Price(_Price)
        {
            dwSize = sizeof( SNET_ADD_REBUY_ITEM_AC );
            nType = NET_MSG_ADD_REBUY_ITEM_AC;
            MIN_STATIC_ASSERT( sizeof(SNET_ADD_REBUY_ITEM_AC)<= NET_DATA_BUFSIZE );
        }
    };

    // 아이템 재매입. Client -> Agent
    struct SNET_BUY_REBUY_ITEM_CA : public NET_MSG_GENERIC
    {
        enum { MAGIC_NUM=9283748, };
        int m_MagicNum;
        SITEMCUSTOM sItemCustom;
        LONGLONG Price;
        
        SNET_BUY_REBUY_ITEM_CA(const SITEMCUSTOM& _sItemCustom, LONGLONG _Price)
            : sItemCustom(_sItemCustom)
            , Price(_Price)
            , m_MagicNum(MAGIC_NUM)
        {
            dwSize = sizeof(SNET_BUY_REBUY_ITEM_CA);
            nType = NET_MSG_BUY_REBUY_ITEM_CA;
            MIN_STATIC_ASSERT( sizeof(SNET_BUY_REBUY_ITEM_CA)<= NET_DATA_BUFSIZE );
        }
    };

    // 아이템 재매입. Agent -> Field
    struct SNET_BUY_REBUY_ITEM_AF : public NET_MSG_GENERIC
    {
        enum { MAGIC_NUMBER=8279323, };
        unsigned int MagicNum;
        DWORD m_ChaDbNum;
        SITEMCUSTOM sItemCustom;
        LONGLONG Price;
        
        SNET_BUY_REBUY_ITEM_AF(DWORD ChaDbNum, const SITEMCUSTOM& _sItemCustom, LONGLONG _Price )
            : m_ChaDbNum(ChaDbNum)
            , sItemCustom(_sItemCustom)
            , Price(_Price)
            , MagicNum(MAGIC_NUMBER)
        {
            dwSize = sizeof( SNET_BUY_REBUY_ITEM_AF );
            nType = NET_MSG_BUY_REBUY_ITEM_AF;
            MIN_STATIC_ASSERT( sizeof(SNET_BUY_REBUY_ITEM_AF)<= NET_DATA_BUFSIZE );
        }
    };

    // 아이템 재매입 결과. Field -> Clinet
    struct SNET_BUY_REBUY_ITEM_FC : public NET_MSG_GENERIC
    {

        EMBUYREBUYITEM emFB;

        SITEMCUSTOM sItemCustom;
        LONGLONG Price;

        SNET_BUY_REBUY_ITEM_FC(const SITEMCUSTOM& _sItemCustom, LONGLONG _Price )
            : emFB(EMBUYREBUYITEM_FAILED)
            , sItemCustom(_sItemCustom)
            , Price(_Price)
        {
            dwSize = sizeof( SNET_BUY_REBUY_ITEM_FC );
            nType = NET_MSG_BUY_REBUY_ITEM_FC;
            MIN_STATIC_ASSERT( sizeof(SNET_BUY_REBUY_ITEM_FC)<= NET_DATA_BUFSIZE );
        }
    };

    // 아이템 재매입 결과. Field -> Agent
    struct SNET_BUY_REBUY_ITEM_FA : public NET_MSG_CHARACTER//NET_MSG_GENERIC
    {
        EMBUYREBUYITEM emFB;

        SITEMCUSTOM sItemCustom;
        LONGLONG Price;

        SNET_BUY_REBUY_ITEM_FA(const SITEMCUSTOM& _sItemCustom, LONGLONG _Price )
            : emFB(EMBUYREBUYITEM_FAILED)
            , sItemCustom(_sItemCustom)
            , Price(_Price)
        {
            dwSize = sizeof( SNET_BUY_REBUY_ITEM_FA );
            nType = NET_MSG_BUY_REBUY_ITEM_FA;
            MIN_STATIC_ASSERT( sizeof(SNET_BUY_REBUY_ITEM_FA)<= NET_DATA_BUFSIZE );
        }
    };

    // NPC 재매입 ui에서 소모품 아이템 체크.
    struct SNET_CHECK_REBUY_SUPPLIES : public NET_MSG_GENERIC
    {
        bool m_bCheck;

        SNET_CHECK_REBUY_SUPPLIES( bool bCheck )
            : m_bCheck(bCheck)
        {
            dwSize = sizeof( SNET_CHECK_REBUY_SUPPLIES );
            nType = NET_MSG_CHECK_REBUY_SUPPLIES;
            MIN_STATIC_ASSERT( sizeof(SNET_CHECK_REBUY_SUPPLIES)<= NET_DATA_BUFSIZE );
        }
    };

	// Revert to default structure packing
	//#pragma pack()
};
