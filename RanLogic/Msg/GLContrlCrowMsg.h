#pragma once

#include "GLContrlBaseMsg.h"

//namespace GLMSG
//{
//	#pragma pack(1)
//
//
//	// Revert to default structure packing
//	#pragma pack()
//};

namespace GLMSG
{
//	#pragma pack(1)
	
	struct SNETCROW : public NET_MSG_GENERIC
	{
        DWORD dwGlobID;

		SNETCROW(DWORD _dwGlobID)
			: dwGlobID(_dwGlobID)
		{
		}
	};
	
	struct SNETCROW_MOVETO : public SNETCROW
	{
		DWORD				dwActState;
		D3DXVECTOR3			vCurPos;
		D3DXVECTOR3			vTarPos;
		DWORD				dwCellID;
        DWORD               dwTarID;
        EMCROW				emTarCrow;
		DWORD				dwFlags;	  // 추격을 위한 이동인지 유무

		SNETCROW_MOVETO(
            DWORD _dwGlobID,
            DWORD _dwActState,
            D3DXVECTOR3 _vCurPos,
            D3DXVECTOR3 _vTarPos,
            DWORD _dwCellID,
            DWORD _dwTarID,
            EMCROW _emTarCrow,
            BOOL _dwFlags)
			: SNETCROW(_dwGlobID)
            , dwActState(_dwActState)
			, vCurPos(_vCurPos)
			, vTarPos(_vTarPos)
			, dwCellID(_dwCellID)
            , dwTarID(_dwTarID)
            , emTarCrow(_emTarCrow)
			, dwFlags(_dwFlags)
		{
			dwSize = sizeof(SNETCROW_MOVETO);
			nType = NET_MSG_CROW_MOVETO;
			MIN_STATIC_ASSERT(sizeof(SNETCROW_MOVETO)<=NET_DATA_BUFSIZE);
		}
	};

	
	struct SNETCROW_ATTACK : public SNETCROW
	{
		EMCROW				emTarCrow;
		DWORD				dwTarID;
		DWORD				dwAType;
		D3DXVECTOR3			vPos;

		SNETCROW_ATTACK(
            DWORD _dwGlobID,
            EMCROW _emTarCrow,
            DWORD _dwTarID,
            DWORD _dwAType,
            D3DXVECTOR3 _vPos)
			: SNETCROW(_dwGlobID)
            , emTarCrow(_emTarCrow)
			, dwTarID(_dwTarID)
			, dwAType(_dwAType)
            , vPos(_vPos)
		{
			dwSize = sizeof(SNETCROW_ATTACK);
			nType = NET_MSG_CROW_ATTACK;
			MIN_STATIC_ASSERT(sizeof(SNETCROW_ATTACK)<=NET_DATA_BUFSIZE);
		}
	};
	
	struct SNETCROW_BLOCKING : public SNETCROW
	{
		const EMCROW emTarCrow;
		const DWORD dwTarID;
		const SNATIVEID SkillID;
		const WORD wSkillLevel;

		SNETCROW_BLOCKING(const DWORD _dwGlobID, const EMCROW _emTarCrow, const DWORD _dwTarID, const SNATIVEID& _SkillID, const WORD _wSkillLevel)
			: SNETCROW(_dwGlobID)
            , emTarCrow(_emTarCrow)
			, dwTarID(_dwTarID)
			, SkillID(_SkillID)
			, wSkillLevel(_wSkillLevel)
		{
			dwSize = sizeof(SNETCROW_BLOCKING);
			nType = NET_MSG_GCTRL_CROW_ATTACK_BLOCKING;
			MIN_STATIC_ASSERT(sizeof(SNETCROW_BLOCKING)<=NET_DATA_BUFSIZE);
		}
	};
	
	struct SNETCROW_AVOID : public SNETCROW
	{
		const EMCROW emTarCrow;
		const DWORD dwTarID;
		const SNATIVEID SkillID;
		const WORD wSkillLevel;

		SNETCROW_AVOID(const DWORD _dwGlobID, const EMCROW _emTarCrow, const DWORD _dwTarID, const SNATIVEID& _SkillID, const WORD _wSkillLevel)
			: SNETCROW(_dwGlobID)
            , emTarCrow(_emTarCrow)
			, dwTarID(_dwTarID)
			, SkillID(_SkillID)
			, wSkillLevel(_wSkillLevel)
		{
			dwSize = sizeof(SNETCROW_AVOID);
			nType = NET_MSG_CROW_AVOID;
			MIN_STATIC_ASSERT(sizeof(SNETCROW_AVOID)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETCROW_DAMAGE : public SNETCROW
	{
        EMCROW emHitCrow;   // 공격한 대상 타입
		EMCROW emTarCrow;
		DWORD dwTarID;
		int nDamage;
		DWORD dwNowHP;
		DWORD dwDamageFlag;
        

		SNETCROW_DAMAGE(
            DWORD _dwGlobID,
            EMCROW _emTarCrow,
            DWORD _dwTarID,
            int _nDamage,
            DWORD _dwNowHP,
            DWORD _dwDamageFlag)
			: SNETCROW(_dwGlobID)
            , emTarCrow(_emTarCrow)
			, dwTarID(_dwTarID)
			, nDamage(_nDamage)
			, dwNowHP(_dwNowHP)
			, dwDamageFlag(_dwDamageFlag)
		{
			dwSize = sizeof(SNETCROW_DAMAGE);
			nType = NET_MSG_CROW_DAMAGE;
			MIN_STATIC_ASSERT(sizeof(SNETCROW_DAMAGE)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETCROW_SKILL : public SNETCROW
	{
		DWORD				dwAType;
		SNATIVEID			skill_id;
		WORD				skill_lev;
	
		D3DXVECTOR3			vTARPOS;
		D3DXVECTOR3			vPos;
		WORD				wTARNUM;
		STARID				sTARIDS[EMTARGET_NET];

		SNETCROW_SKILL(DWORD _dwGlobID) 
			: SNETCROW(_dwGlobID)
			, dwAType(0)
			, skill_id(0,0)
			, skill_lev(0)
			, vTARPOS(0,0,0)
			, wTARNUM(0)
		{
			dwSize = sizeof(SNETCROW_SKILL);
			nType = NET_MSG_CROW_SKILL;
			MIN_STATIC_ASSERT(sizeof(SNETCROW_SKILL)<=NET_DATA_BUFSIZE);
		}

		void CALCSIZE ()
		{
			dwSize = sizeof(SNETCROW_SKILL) - ( sizeof(STARID) * (EMTARGET_NET - wTARNUM) );
		}

		BOOL ADDTARGET ( EMCROW emCrow, DWORD dwID )
		{
			if ( wTARNUM>=EMTARGET_NET )
			{
				return FALSE;
			}
			else
			{
				sTARIDS[wTARNUM].emCrow = emCrow;
				sTARIDS[wTARNUM].dwID = dwID;			
				wTARNUM++;
				CALCSIZE ();
				return TRUE;
			}
		}

		BOOL ADDTARGET ( const STARID &starget )
		{
			return ADDTARGET(starget.GETCROW(),starget.GETID());
		}
	};

	struct SNETCROW_SKILL_DEALY_ACTION : public SNETCROW
	{
		DWORD				dwAType;
		SNATIVEID			skill_id;
		WORD				skill_lev;

		D3DXVECTOR3			vTARPOS;
		D3DXVECTOR3			vPos;
		WORD				wTARNUM;
		STARID				sTARIDS[EMTARGET_NET];

		SNETCROW_SKILL_DEALY_ACTION(DWORD _dwGlobID) 
			: SNETCROW(_dwGlobID)
			, dwAType(0)
			, skill_id(0,0)
			, skill_lev(0)
			, vTARPOS(0,0,0)
			, wTARNUM(0)
		{
			dwSize = sizeof(SNETCROW_SKILL_DEALY_ACTION);
			nType = NET_MSG_CROW_SKILL_DEALY_ACTION;
			MIN_STATIC_ASSERT(sizeof(SNETCROW_SKILL_DEALY_ACTION)<=NET_DATA_BUFSIZE);
		}

		void CALCSIZE ()
		{
			dwSize = sizeof(SNETCROW_SKILL_DEALY_ACTION) - ( sizeof(STARID) * (EMTARGET_NET - wTARNUM) );
		}

		BOOL ADDTARGET ( EMCROW emCrow, DWORD dwID )
		{
			if ( wTARNUM>=EMTARGET_NET )
			{
				return FALSE;
			}
			else
			{
				sTARIDS[wTARNUM].emCrow = emCrow;
				sTARIDS[wTARNUM].dwID = dwID;			
				wTARNUM++;
				CALCSIZE ();
				return TRUE;
			}
		}

		BOOL ADDTARGET ( const STARID &starget )
		{
			return ADDTARGET(starget.GETCROW(),starget.GETID());
		}
	};

	struct SNETCROW_END_SKILL : public SNETCROW
	{
		const DWORD dwSkillfactIndex;
		SNETCROW_END_SKILL(const DWORD _dwGlobID, const DWORD _dwSkillfactIndex)
			:SNETCROW(_dwGlobID), dwSkillfactIndex(_dwSkillfactIndex)
		{
			dwSize = sizeof(SNETCROW_END_SKILL);
			nType = NET_MSG_CROW_END_SKILL;
			MIN_STATIC_ASSERT(sizeof(SNETCROW_END_SKILL)<=NET_DATA_BUFSIZE);
		}

	};

    struct SNETCROW_UPDATE_STATE : public SNETCROW
    {
		enum EMCHANGESTATUS_TYPE
		{
			EMCHANGESTATUS_TYPE_NONE,
			EMCHANGESTATUS_TYPE_OFFENSIVE_POWER,
			EMCHANGESTATUS_TYPE_DEFENSIVE_POWER,
			EMCHANGESTATUS_TYPE_FIRE_RESISTANCE,
			EMCHANGESTATUS_TYPE_ICE_RESISTANCE,
			EMCHANGESTATUS_TYPE_ELECTRIC_RESISTANCE,
			EMCHANGESTATUS_TYPE_POSITION_RESISTANCE,
			EMCHANGESTATUS_TYPE_SPIRIT_RESISTANCE,
			EMCHANGESTATUS_TYPE_ACCURACY_RATE,
			EMCHANGESTATUS_TYPE_AVOIDANCE_RATE,
			EMCHANGESTATUS_TYPE_HPRESTORATION_RATE,
			EMCHANGESTATUS_TYPE_MPRESTORATION_RATE,
			EMCHANGESTATUS_TYPE_SPRESTORATION_RATE,
			EMCHANGESTATUS_TYPE_HP_MULTIPLE,
			EMCHANGESTATUS_TYPE_ATTACK_MULTIPLE,
			EMCHANGESTATUS_TYPE_DEFENSE_MULTIPLE,
			EMCHANGESTATUS_TYPE_EXP_MULTIPLE,
			EMCHANGESTATUS_TYPE_DROP_MULTIPLE,
		};

        const EMACTIONTYPE emAction;
        const DWORD dwNowHP;

		EMCHANGESTATUS_TYPE emStatusType;
		union
		{
			float fStatusValue;
			DWORD dwStatusValue;
		};

        SNETCROW_UPDATE_STATE(const DWORD _dwGlobID, const DWORD _dwHP)
            : SNETCROW(_dwGlobID)
            , emAction(GLAT_SIZE)
            , dwNowHP(_dwHP)
			, emStatusType( EMCHANGESTATUS_TYPE_NONE )
			, dwStatusValue( 0 )
        {
            dwSize = sizeof(SNETCROW_UPDATE_STATE);
            nType = NET_MSG_CROW_UPDATE_STATE;
            MIN_STATIC_ASSERT(sizeof(SNETCROW_UPDATE_STATE)<=NET_DATA_BUFSIZE);
        }
    };

	// Revert to default structure packing
//	#pragma pack()
};

