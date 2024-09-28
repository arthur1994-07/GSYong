#pragma once

#include "GLContrlBaseMsg.h"


//NET_MSG_GCTRL_LANDEVENT_SUMMONGATE_CREATE     = (NET_MSG_GCTRL + 1617), // 소환문 랜드 이벤트 생성
//NET_MSG_GCTRL_LANDEVENT_SUMMONGATE_CREATE_BRD = (NET_MSG_GCTRL + 1618), // 소환문 랜드 이벤트 생성 알림
//NET_MSG_GCTRL_LANDEVENT_SUMMONGATE_DROP       = (NET_MSG_GCTRL + 1619), // 소환문 랜드 이벤트 보이기

namespace GLMSG
{
    #pragma pack(1)

	//---------------------------------------------------------------------------NET

	struct SNETLANDEVENT_SUMMONGATE_CREATE : public NET_MSG_GENERIC
	{
		DWORD				dwGaeaID;
		SNATIVEID           sMapID;
		D3DXVECTOR3         vPos;

		FLOAT               fTimeCurrent; // 이벤트 현재 시간
		FLOAT               fTimeEvent;   // 이벤트 발동 시간
		FLOAT               fTimeEnd;     // 이벤트 종료 시간

		SNATIVEID           skill_id;        // 해당 스킬 정보
		WORD                skill_lev;       // 해당 레벨 정보

		DWORD               dwOwnerID;       // 주인의 GUID

		STARGETID           sEvent;          // 이벤트 정보
		STARGETID           sTarget;         // 타겟 정보

		SNETLANDEVENT_SUMMONGATE_CREATE ()
			: dwGaeaID(UINT_MAX)
            , sMapID(false)
            , vPos(0.0f,0.0f,0.0f)
            , fTimeCurrent(0.0f)
            , fTimeEvent(0.0f)
            , fTimeEnd(0.0f)	
            , skill_id(false)
            , skill_lev(0)
            , dwOwnerID(UINT_MAX)
		{
			dwSize = sizeof(SNETLANDEVENT_SUMMONGATE_CREATE); 
			nType  = NET_MSG_GCTRL_LANDEVENT_SUMMONGATE_CREATE;
            MIN_STATIC_ASSERT(sizeof(SNETLANDEVENT_SUMMONGATE_CREATE)<=NET_DATA_BUFSIZE);
		}
	};



	//---------------------------------------------------------------------------NET

    struct SNETLANDEVENT_REMOVE : public NET_MSG_GENERIC
    {
        DWORD dwGaeaID;

        SNETLANDEVENT_REMOVE(DWORD GaeaId)
            : dwGaeaID(GaeaId)
            //: dwGaeaID (UINT_MAX)
        {
            dwSize = sizeof(SNETLANDEVENT_REMOVE); 
            nType  = NET_MSG_GCTRL_LANDEVENT_REMOVE;
            MIN_STATIC_ASSERT(sizeof(SNETLANDEVENT_REMOVE)<=NET_DATA_BUFSIZE);
        }
    };



	// Revert to default structure packing
#pragma pack()



	//---------------------------------------------------------------------------

	struct SNET_LEVEL_LAYER_FC : public NET_MSG_GENERIC
	{
		enum EMLEVEL_LAYER_ACTIVE
		{
			LEVEL_LAYER_ACTIVE = 0,			// 활성.
			LEVEL_LAYER_ONE_DEACTIVE = 1,	// 1개 비활성.
			LEVEL_LAYER_ALL_DEACTIVE = 2,	// 모두 비활성.
		};
		EMLEVEL_LAYER_ACTIVE	m_emActionType;
		UINT					m_unLayerIndex;

		SNET_LEVEL_LAYER_FC(EMLEVEL_LAYER_ACTIVE emActionType, UINT unLayerIndex)
			: m_emActionType(emActionType)
			, m_unLayerIndex (unLayerIndex)
		{
			dwSize = sizeof(SNET_LEVEL_LAYER_FC); 
			nType  = NET_MSG_GCTRL_LEVEL_LAYER_FC;
			MIN_STATIC_ASSERT(sizeof(SNET_LEVEL_LAYER_FC)<=NET_DATA_BUFSIZE);
		}
	};
};
