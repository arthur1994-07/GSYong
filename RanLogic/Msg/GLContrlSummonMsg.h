#pragma once

#include "../Summon/GLSummon.h"
#include "GLContrlBaseMsg.h"

enum EMUSE_SUMMON_FB
{
	EMUSE_SUMMON_FB_OK				  = 0, // 성공
	EMUSE_SUMMON_FB_FAIL			  = 1, // 실패
	EMUSE_SUMMON_FB_FAIL_INVALIDZONE  = 2, // 소환할 수 없는 지역
	EMUSE_SUMMON_FB_FAIL_VEHICLE	  = 3, // 탈것을 타고 있어서 실패
	EMUSE_SUMMON_FB_FAIL_CONFRONT	  = 4, // 대련중 실패
	EMUSE_SUMMON_FB_FAIL_NODATA		  = 5, // 데이터가 없을경우
};

enum EMSUMMON_REQ_SLOT_EX_HOLD_FB
{
	EMSUMMON_REQ_SLOT_EX_HOLD_FB_FAIL		  = 0,		// 일반오류.
	EMSUMMON_REQ_SLOT_EX_HOLD_FB_OK			  = 1,		// 아이템 교체 성공
	EMSUMMON_REQ_SLOT_EX_HOLD_FB_NOMATCH	  = 2,		// 타입이 맞지 않는 아이템
	EMSUMMON_REQ_SLOT_EX_HOLD_FB_INVALIDITEM  = 3,		// 탈것용이 아니다.
};

enum EMHIRE_SUMMOM_FC
{
	EMFAILED_HIRE_SUMMON_OK							= 0,
	EMFAILED_HIRE_SUMMON_FAIL						= 1,
	EMFAILED_HIRE_SUMMON_DONT_LEARN_PASSIVE			= 2,
	EMFAILED_HIRE_SUMMON_INVALID_CROWDATA			= 3,
	EMFAILED_HIRE_SUMMON_CROWLEVEL_OVERFLOW			= 4,
};

enum EMHIRE_FAILED_DROP_SUMMOM_FC
{
	EMFAILED_FAILED_DROP_SUMMON_NONHIRE				= 0,
	EMFAILED_FAILED_OVERFLOW_CROWLEVEL				= 1,
};

namespace GLMSG
{
	#pragma pack(1)

    struct SNET_SUMMON : public NET_MSG_GENERIC
	{
		DWORD				dwGUID;

		SNET_SUMMON ( DWORD _dwGUID )//= UINT_MAX )
            : dwGUID(_dwGUID)
		{
		}
	};

    struct SNETPC_REQ_USE_SUMMON : public NET_MSG_GENERIC
	{
		SNATIVEID			sSummonID;
		SUMMON_TYPE			emTYPE;

		SNETPC_REQ_USE_SUMMON ( SNATIVEID _sSummonID = NATIVEID_NULL(), SUMMON_TYPE _emTYPE = SUMMON_TYPE_NONE ) 
            : sSummonID(_sSummonID)
            , emTYPE(_emTYPE)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_REQ_USE_SUMMON;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_USE_SUMMON)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_REQ_USE_SUMMON_EVENT_SKILL_CF : public NET_MSG_GENERIC
	{
		UINT iPage;
		UINT iIdx;

		SNETPC_REQ_USE_SUMMON_EVENT_SKILL_CF ( UINT _iIdx )
			: iIdx( _iIdx )
		{
			dwSize = sizeof( *this );
			nType = NET_MSG_REQ_USE_SUMMON_EVENT_SKILL;
			MIN_STATIC_ASSERT( sizeof( SNETPC_REQ_USE_SUMMON_EVENT_SKILL_CF ) <= NET_DATA_BUFSIZE );
		}
	};

	struct SNETPC_REQ_HIRE_BASICSUMMON_CF : public NET_MSG_GENERIC
	{
		WORD wMainID;
		WORD wSubID;

		SNETPC_REQ_HIRE_BASICSUMMON_CF ( WORD _wMainID, WORD _wSubID )
			: wMainID( _wMainID )
			, wSubID( _wSubID )
		{
			dwSize = sizeof( *this );
			nType = NET_MSG_HIRE_BASICSUMMON_CF;
			MIN_STATIC_ASSERT( sizeof( SNETPC_REQ_HIRE_BASICSUMMON_CF ) <= NET_DATA_BUFSIZE );
		}
	};

	struct SNETPC_REQ_HIRE_BASICSUMMON_FC : public NET_MSG_GENERIC
	{
		EMHIRE_SUMMOM_FC emFC;

		SNETPC_REQ_HIRE_BASICSUMMON_FC ()
			: emFC ( EMFAILED_HIRE_SUMMON_OK )
		{
			dwSize = sizeof( *this );
			nType = NET_MSG_HIRE_BASICSUMMON_FC;
			MIN_STATIC_ASSERT( sizeof( SNETPC_REQ_HIRE_BASICSUMMON_FC ) <= NET_DATA_BUFSIZE );
		}
	};

	struct SNETPC_REQ_USE_SUMMON_FB : public NET_MSG_GENERIC
	{
		SUMMON_TYPE			emTYPE;
		DWORD				dwGUID;
		SNATIVEID			sSummonID;
		DWORD				dwOwner;	// 주인 GaeaID		
		SNATIVEID			sMapID;
		DWORD				dwCellID;
		DWORD				dwSummonID;
		EMUSE_SUMMON_FB		emFB;
		D3DXVECTOR3			vPos;
		D3DXVECTOR3			vDir;
		STARGETID           sTarget;    // 타겟정보

		DWORD				dwNowHP;	//	생명량.
		WORD				wNowMP;		//	정신량.

		SNETPC_REQ_USE_SUMMON_FB () :
		emTYPE(SUMMON_TYPE_NONE),
			dwGUID(UINT_MAX),
			sSummonID(NATIVEID_NULL()),
			dwOwner(0),
			sMapID(NATIVEID_NULL()),
			dwCellID(0),
			dwSummonID(0),
			vPos(0,0,0),
			vDir(0,0,0),
			emFB(EMUSE_SUMMON_FB_FAIL),
			dwNowHP(0),
			wNowMP(0)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_REQ_USE_SUMMON_FB;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_USE_SUMMON_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_SUMMON_FAILED_DROP_SUMMON_FC : public SNET_SUMMON
	{
		EMHIRE_FAILED_DROP_SUMMOM_FC emFC;

		SNET_SUMMON_FAILED_DROP_SUMMON_FC ()
			: SNET_SUMMON ( UINT_MAX )
			, emFC ( EMFAILED_FAILED_DROP_SUMMON_NONHIRE )
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GCTRL_FAILED_DROP_SUMMON;
			MIN_STATIC_ASSERT(sizeof(SNET_SUMMON_FAILED_DROP_SUMMON_FC)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_SUMMON_DROP_SUMMON : public SNET_SUMMON
	{
		SDROPSUMMON	Data;

		SNET_SUMMON_DROP_SUMMON () 
            : SNET_SUMMON ( UINT_MAX ) 
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GCTRL_DROP_SUMMON;
			MIN_STATIC_ASSERT(sizeof(SNET_SUMMON_DROP_SUMMON)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_SUMMON_GOTO : public SNET_SUMMON
	{
		STARGETID           sTarID;
		D3DXVECTOR3			vCurPos;
		D3DXVECTOR3			vTarPos;
		FLOAT				fSpeed;
		DWORD				dwFlag;

		SNET_SUMMON_GOTO ( DWORD _dwGUID, STARGETID _sTarID, D3DXVECTOR3 _vCurPos, D3DXVECTOR3 _vTarPos, FLOAT _fSpeed = 0.0f, DWORD _dwFlag = 0 )
            : SNET_SUMMON (_dwGUID)
            , sTarID(_sTarID)
            , vCurPos(_vCurPos)
            , vTarPos(_vTarPos)
            , fSpeed(_fSpeed)
            , dwFlag(_dwFlag)
		{
			dwSize = sizeof(*this);
			nType  = NET_MSG_GCTRL_SUMMON_GOTO;
			MIN_STATIC_ASSERT(sizeof(SNET_SUMMON_GOTO)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_SUMMON_ATTACK : public SNET_SUMMON
	{
		STARGETID			sTarID;
		DWORD				dwAType;

		SNET_SUMMON_ATTACK ( DWORD _dwGUID, STARGETID _sTarID, DWORD _dwAType = 0 ) 
            : SNET_SUMMON(_dwGUID)
            , sTarID(_sTarID)
            , dwAType(_dwAType)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GCTRL_SUMMON_ATTACK;
			MIN_STATIC_ASSERT(sizeof(SNET_SUMMON_ATTACK)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_SUMMON_ATTACK_BLOCKING : public SNET_SUMMON
	{
		const EMCROW emTarCrow;
		const DWORD dwTarID;
		const SNATIVEID SkillID;
		const WORD wSkillLevel;

		SNET_SUMMON_ATTACK_BLOCKING(const DWORD _dwGUID, const EMCROW _emTarCrow, const DWORD _dwTarID, const SNATIVEID& _SkillID, const WORD _wSkillLevel)
            : SNET_SUMMON(_dwGUID)
            , emTarCrow(_emTarCrow)
            , dwTarID(_dwTarID)
			, SkillID(_SkillID)
			, wSkillLevel(_wSkillLevel)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GCTRL_SUMMON_ATTACK_BLOCKING;
			MIN_STATIC_ASSERT(sizeof(SNET_SUMMON_ATTACK_BLOCKING)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_SUMMON_ATTACK_AVOID : public SNET_SUMMON
	{
		const EMCROW emTarCrow;
		const DWORD dwTarID;
		const SNATIVEID SkillID;
		const WORD wSkillLevel;

		SNET_SUMMON_ATTACK_AVOID(const DWORD _dwGUID, const EMCROW _emTarCrow, const DWORD _dwTarID, const SNATIVEID& _SkillID, const WORD _wSkillLevel)
			: SNET_SUMMON(_dwGUID)
			, emTarCrow(_emTarCrow)
			, dwTarID(_dwTarID)
			, SkillID(_SkillID)
			, wSkillLevel(_wSkillLevel)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GCTRL_SUMMON_ATTACK_AVOID;
			MIN_STATIC_ASSERT(sizeof(SNET_SUMMON_ATTACK_AVOID)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_SUMMON_ATTACK_DAMAGE : public SNET_SUMMON
	{
		EMCROW				emTarCrow;
		DWORD				dwTarID;
		int					nDamage;
		DWORD				dwDamageFlag;
        WORD                wSchool;        //mjeon.CaptureTheField

		SNET_SUMMON_ATTACK_DAMAGE ( DWORD _dwGUID, EMCROW _emTarCrow = CROW_MOB, DWORD _dwTarID = 0, 
                                    int _nDamage = 0, DWORD _dwDamageFlag = DAMAGE_TYPE_NONE, WORD _wSchool = SCHOOL_NONE )
            : SNET_SUMMON(_dwGUID)
            , emTarCrow(_emTarCrow)
            , dwTarID(_dwTarID)
            , nDamage(_nDamage)
            , dwDamageFlag( _dwDamageFlag )
            , wSchool(_wSchool)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GCTRL_SUMMON_ATTACK_DAMAGE;
			MIN_STATIC_ASSERT(sizeof(SNET_SUMMON_ATTACK_DAMAGE)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_SUMMON_SKILL : public SNET_SUMMON
	{
		STARGETID           sTarID;
		DWORD				dwAType;
		bool				bUsingEventSkill;
		SNATIVEID			skill_id;
		WORD				skill_lev;

		D3DXVECTOR3			vTARPOS;
		WORD				wTARNUM;
		STARID				sTARIDS[EMTARGET_NET];

		SNET_SUMMON_SKILL ( DWORD _dwGUID
			, STARGETID _sTarID
			, DWORD _dwAType = 0
			, bool _bUsingEventSkill = false
			, SNATIVEID _skill_id = NATIVEID_NULL()
			, WORD _skill_lev = 0 
			, D3DXVECTOR3 _vTARPOS = D3DXVECTOR3(0,0,0)
			, WORD _wTARNUM = 0 )
            : SNET_SUMMON(_dwGUID)
            , sTarID(_sTarID)
            , dwAType(_dwAType)
			, bUsingEventSkill(_bUsingEventSkill)
			, skill_id(_skill_id)
			, skill_lev(_skill_lev)
			, vTARPOS(_vTARPOS)
			, wTARNUM(_wTARNUM)
		{
			dwSize = sizeof(SNETCROW_SKILL);
			nType = NET_MSG_GCTRL_SUMMON_SKILL;
			MIN_STATIC_ASSERT(sizeof(SNET_SUMMON_SKILL)<=NET_DATA_BUFSIZE);
		}

		void CALCSIZE ()
		{
			dwSize = sizeof(SNET_SUMMON_SKILL) - ( sizeof(STARID) * (EMTARGET_NET - wTARNUM) );
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

	struct SNET_SUMMON_END_SKILL : public SNET_SUMMON
	{
		const DWORD dwSkillfactIndex;

		SNET_SUMMON_END_SKILL ( const DWORD _dwGlobID
			, const DWORD _dwSkillfactIndex )
			: SNET_SUMMON ( _dwGlobID )
			, dwSkillfactIndex ( _dwSkillfactIndex )
		{
			dwSize = sizeof( SNET_SUMMON_END_SKILL );
			nType = NET_MSG_GCTRL_SUMMON_END_SKILL;
			MIN_STATIC_ASSERT( sizeof( SNET_SUMMON_END_SKILL ) <= NET_DATA_BUFSIZE );
		}
	};

	struct SNET_SUMMON_EFFECT : public SNET_SUMMON
	{
		SNATIVEID sSkillID;

		SNET_SUMMON_EFFECT ( DWORD _dwGUID )
			: SNET_SUMMON ( _dwGUID )
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GCTRL_SUMMON_EFFECT;
			MIN_STATIC_ASSERT(sizeof(SNET_SUMMON_EFFECT)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_SUMMON_UPDATE_CURSKILLPAGE : public SNET_SUMMON
	{
		DWORD dwCurSkillPage;

		SNET_SUMMON_UPDATE_CURSKILLPAGE ( DWORD _dwGUID
			, DWORD _dwCurSkillPage )
			: SNET_SUMMON ( _dwGUID )
			, dwCurSkillPage ( _dwCurSkillPage )
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GCTRL_SUMMON_UPDATE_CUR_SKILLPAGE;
			MIN_STATIC_ASSERT(sizeof(SNET_SUMMON_UPDATE_CURSKILLPAGE)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_SUMMON_UPDATE_STATE : public SNET_SUMMON
	{
		DWORD				  dwActionFlag;
		DWORD                 dwHPNow;
		EMSUMMONGESTURESTATE  emGesture;
		float				  fSpeed;

		SNET_SUMMON_UPDATE_STATE ( DWORD _dwGUID, DWORD _dwActionFlag = 0 , DWORD _dwHPNow = 0, 
                                   EMSUMMONGESTURESTATE _emGesture = EM_SUMMONGESTURE_NONE, float _fSpeed = 0.0f )
            : SNET_SUMMON(_dwGUID)
            , dwActionFlag(_dwActionFlag)
            , dwHPNow(_dwHPNow)
            , fSpeed(_fSpeed)
            , emGesture(_emGesture)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GCTRL_SUMMON_UPDATE_STATE;
			MIN_STATIC_ASSERT(sizeof(SNET_SUMMON_UPDATE_STATE)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_SUMMON_LANDEFFECT_UPDATE : public SNET_SUMMON
	{
        SLANDEFFECT_VALUE landEffect;
		bool bADD;

		SNET_SUMMON_LANDEFFECT_UPDATE ( DWORD _dwGUID, SLANDEFFECT_VALUE _landEffect, bool _bADD = false )
            : SNET_SUMMON(_dwGUID)
            , landEffect(_landEffect)
			, bADD ( _bADD ) 
		{
			dwSize = sizeof(SNET_SUMMON_LANDEFFECT_UPDATE);
			nType = NET_MSG_GCTRL_SUMMON_LANDEFFECT_UPDATE;
            MIN_STATIC_ASSERT(sizeof(SNET_SUMMON_LANDEFFECT_UPDATE)<=NET_DATA_BUFSIZE);
		}

	};

	struct SNET_SUMMON_MAPEFFECT_UPDATE : public SNET_SUMMON
	{
        SMAP_EFFECT sMapEffect;
		bool bADD;

		SNET_SUMMON_MAPEFFECT_UPDATE ( DWORD _dwGUID, SMAP_EFFECT _sMapEffect, bool _bADD = false )
			: SNET_SUMMON(_dwGUID)
            , sMapEffect(_sMapEffect)
			, bADD ( _bADD )
		{
			dwSize = sizeof(SNET_SUMMON_MAPEFFECT_UPDATE);
			nType = NET_MSG_GCTRL_SUMMON_MAPEFFECT_UPDATE;
            MIN_STATIC_ASSERT(sizeof(SNET_SUMMON_MAPEFFECT_UPDATE)<=NET_DATA_BUFSIZE);
		}

	};

	// Revert to default structure packing
	#pragma pack()
};