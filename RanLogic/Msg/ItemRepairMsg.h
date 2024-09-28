#pragma once
#include "../../SigmaCore/gassert.h"

#include "GLContrlBaseMsg.h"
#include "../Character/GLCharDefine.h"
#include "../Item/GLItemDef.h"


namespace GLMSG
{
	#pragma pack(1)

// 	//! 착용아이템 변화
// 	struct SNETPC_PUTON_UPDATE //: public NET_MSG_GENERIC
// 	{
// 		BYTE        m_SlotRelease; // EMSLOT
// 		BYTE        m_Slot; // EMSLOT
// 		SITEMCUSTOM m_Item;
// 
//         MSGPACK_DEFINE(m_SlotRelease, m_Slot, m_Item);
// 
// 		SNETPC_PUTON_UPDATE()
// 			: m_SlotRelease(SLOT_TSIZE)
// 			, m_Slot(SLOT_HEADGEAR)
// 		{
//             MIN_STATIC_ASSERT(sizeof(SNETPC_PUTON_UPDATE)<=NET_DATA_BUFSIZE);
// 			//dwSize = sizeof(SNETPC_PUTON_UPDATE);
// 			//nType = NET_MSG_GCTRL_PUTON_UPDATE;
// 		}
// 	};

    
    enum EMACK_ITEM_REPAIR
    {
        REPAIR_FAIL = 0,
        REPAIR_OK,
    };

    //! 아이템 수리 요청
	struct NET_ITEM_REPAIR_REQ
	{
		DWORD m_dwChaDbNum;   // 캐릭터 번호
        std::vector<BYTE> m_vecSlot;         // EMSLOT

        MSGPACK_DEFINE( m_dwChaDbNum, m_vecSlot );
        //nType = NET_MSG_ITEM_REPAIR_REQ

		NET_ITEM_REPAIR_REQ()
			: m_dwChaDbNum(0)
		{}
	};

    //! 아이템 수리 응답
    struct NET_ITEM_REPAIR_ACK
    {
        DWORD m_dwChaDbNum;    // 캐릭터 번호
        std::vector<BYTE> m_vecSlot;         // EMSLOT

        MSGPACK_DEFINE( m_dwChaDbNum, m_vecSlot );
        //nType = NET_MSG_ITEM_REPAIR_ACK

        NET_ITEM_REPAIR_ACK()
            : m_dwChaDbNum(0)
        {}
    };

    //! 아이템 수리 카드 사용 요청 Client to Field
    struct NET_ITEM_REPAIR_CARD_USE_REQ
    {
        DWORD   m_dwChaDbNum;
        int     m_nInvecPosX;
        int     m_nInvecPosY;

        MSGPACK_DEFINE(m_dwChaDbNum, m_nInvecPosX, m_nInvecPosY);
        //NET_MSG_ITEM_REPAIR_CARD_REQ

        NET_ITEM_REPAIR_CARD_USE_REQ()
            : m_dwChaDbNum(0)
            , m_nInvecPosX(0)
            , m_nInvecPosY(0)
        {}
    };

    //! 아이템 수리 카드 사용 응답 Field to Client
    struct NET_ITEM_REPAIR_CARD_USE_ACK
    {
        DWORD   m_dwChaDbNum;

        MSGPACK_DEFINE(m_dwChaDbNum);
        //NET_MSG_ITEM_REPAIR_CARD_ACK

        NET_ITEM_REPAIR_CARD_USE_ACK()
            : m_dwChaDbNum(0)
        {}
    };

    enum DURABILITY_TYPE
    {
        DDT_NOT_USE = 0,
        DDT_HIT,
        DDT_SKILL_USE,
        DDT_DEAD,
    };

    //! 내구도 감소
    struct NET_DURABILITY_DOWN
    {
        DWORD   m_dwChaDbNum;
        BYTE    byDurabilityType;
        unsigned int nDamage;

        MSGPACK_DEFINE(m_dwChaDbNum, byDurabilityType, nDamage);
        //NET_MSG_DURABILITY_DOWN_BRD

        NET_DURABILITY_DOWN()
            : m_dwChaDbNum(0)
        {}
    };


	// Revert to default structure packing
	#pragma pack()
};
