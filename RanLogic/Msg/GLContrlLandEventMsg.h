#pragma once

#include "GLContrlBaseMsg.h"


//NET_MSG_GCTRL_LANDEVENT_SUMMONGATE_CREATE     = (NET_MSG_GCTRL + 1617), // ��ȯ�� ���� �̺�Ʈ ����
//NET_MSG_GCTRL_LANDEVENT_SUMMONGATE_CREATE_BRD = (NET_MSG_GCTRL + 1618), // ��ȯ�� ���� �̺�Ʈ ���� �˸�
//NET_MSG_GCTRL_LANDEVENT_SUMMONGATE_DROP       = (NET_MSG_GCTRL + 1619), // ��ȯ�� ���� �̺�Ʈ ���̱�

namespace GLMSG
{
    #pragma pack(1)

	//---------------------------------------------------------------------------NET

	struct SNETLANDEVENT_SUMMONGATE_CREATE : public NET_MSG_GENERIC
	{
		DWORD				dwGaeaID;
		SNATIVEID           sMapID;
		D3DXVECTOR3         vPos;

		FLOAT               fTimeCurrent; // �̺�Ʈ ���� �ð�
		FLOAT               fTimeEvent;   // �̺�Ʈ �ߵ� �ð�
		FLOAT               fTimeEnd;     // �̺�Ʈ ���� �ð�

		SNATIVEID           skill_id;        // �ش� ��ų ����
		WORD                skill_lev;       // �ش� ���� ����

		DWORD               dwOwnerID;       // ������ GUID

		STARGETID           sEvent;          // �̺�Ʈ ����
		STARGETID           sTarget;         // Ÿ�� ����

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
			LEVEL_LAYER_ACTIVE = 0,			// Ȱ��.
			LEVEL_LAYER_ONE_DEACTIVE = 1,	// 1�� ��Ȱ��.
			LEVEL_LAYER_ALL_DEACTIVE = 2,	// ��� ��Ȱ��.
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
