#pragma once

#include "GLContrlBaseMsg.h"
#include "../Pet/GLPet.h"
//#include "GLSkill.h"

class GLPetField;

enum EMPET_USECARD_FB
{
	EMPET_USECARD_FB_OK			   = 0,
	EMPET_USECARD_FB_FAIL		   = 1,
	EMPET_USECARD_FB_INVALIDCARD   = 2,
	EMPET_USECARD_FB_NOITEM		   = 3,
	EMPET_USECARD_FB_NOTENOUGHFULL = 4,
    EMPET_USECARD_FB_INVALIDZONE   = 5,
    EMPET_USECARD_FB_ACTIONLIMIT   = 6,
	EMPET_USECARD_FB_NODATA		   = 7,
	EMPET_USECARD_FB_ERR_ACTIVITY		= 8,
	EMPET_USECARD_FB_ERR_CONTRIBUTION   = 9,
};

enum EMPET_REQ_GETRIGHTOFITEM_FB
{
	EMPET_REQ_GETRIGHTOFITEM_FB_OK   = 0,
	EMPET_REQ_GETRIGHTOFITEM_FB_FAIL = 1,
};

enum EMPET_PETSKINPACKOPEN_FB
{
	EMPET_PETSKINPACKOPEN_FB_FAIL			= 0,	//	일반오류.
	EMPET_PETSKINPACKOPEN_FB_PETCARD_FAIL	= 1,	//	펫카드오류.
	EMPET_PETSKINPACKOPEN_FB_OK			= 2,	//	성공.
	EMPET_PETSKINPACKOPEN_FB_END			= 3,	//	시간 종료
};

enum EMPET_REQ_GIVEFOOD_FB
{
	EMPET_REQ_GIVEFOOD_FB_OK		  = 0,
	EMPET_REQ_GIVEFOOD_FB_FAIL		  = 1,
	EMPET_REQ_GIVEFOOD_FB_INVALIDFOOD = 2,
	EMPET_REQ_GIVEFOOD_FB_INVALIDCARD = 3,
	EMPET_REQ_GETFULL_FROMDB_OK		  = 4,
	EMPET_REQ_GETFULL_FROMDB_FAIL	  = 5,
};

enum EMPET_REQ_RENAME_FB
{
	EMPET_REQ_RENAME_FB_FB_FAIL		= 0,		//	일반오류.
	EMPET_REQ_RENAME_FB_FB_OK		= 1,		//	성공.
	EMPET_REQ_RENAME_FB_FB_NOITEM	= 2,		//	아이템이 없음.
	EMPET_REQ_RENAME_FB_FB_BADITEM	= 3,		//	사용 불가능 아이템.
	EMPET_REQ_RENAME_FB_FB_LENGTH	= 4,		//	이름의 길이가 4자 미만
	EMPET_REQ_RENAME_FB_FROM_DB_OK	= 5,		//	성공.
	EMPET_REQ_RENAME_FB_FROM_DB_FAIL	= 6,	//	실패.(이름 중복)
	EMPET_REQ_RENAME_FB_FB_THAICHAR_ERROR = 7,  //  태국어 문자 조합 에러
	EMPET_REQ_RENAME_FB_FB_VNCHAR_ERROR = 8,	//  베트남 문자 조합 에러
	EMPET_REQ_RENAME_FB_BAD_NAME	= 9,
};

enum EMPET_REQ_STYLECOLOR_CHANGE_FB
{
	EMPET_REQ_STYLECOLOR_CHANGE_FB_FAIL		= 0,	//	일반오류.
	EMPET_REQ_STYLE_CHANGE_FB_OK			= 1,	//	성공.
	EMPET_REQ_COLOR_CHANGE_FB_OK			= 2,	//	성공.
	EMPET_REQ_STYLECOLOR_CHANGE_FB_NOITEM	= 3,	//	아이템이 없음.
	EMPET_REQ_STYLECOLOR_CHANGE_FB_BADITEM	= 4,	//	사용 불가능 아이템.
	EMPET_REQ_STYLECOLOR_CHANGE_FB_UNCHANGEABLE	= 5,	//	컬러변경이 불가능한 팻.
	EMPET_REQ_STYLECOLOR_CHANGE_FB_COOLTIME	= 6,	//	컬러변경이 불가능한 팻.
	EMPET_REQ_STYLECOLOR_CHANGE_FB_SKINPACK	= 7,	//	스킨팩 사용중
};

enum EMPET_REQ_SLOT_EX_HOLD_FB
{
	EMPET_REQ_SLOT_EX_HOLD_FB_FAIL		  = 0,		// 일반오류.
	EMPET_REQ_SLOT_EX_HOLD_FB_OK		  = 1,		// 아이템 교체 성공
	EMPET_REQ_SLOT_EX_HOLD_FB_NOMATCH	  = 2,		// 타입이 맞지 않는 아이템
	EMPET_REQ_SLOT_EX_HOLD_FB_INVALIDITEM = 3,		// 팻용 아이템이 아니다
};

enum EMPET_REQ_CHANGESKILL_FB
{
	EMPET_REQ_CHANGESKILL_FB_FAIL		= 0,		// 일반오류
	EMPET_REQ_CHANGESKILL_FB_OK			= 1,		// 성공
	EMPET_REQ_CHANGESKILL_FB_COOLTIME	= 2,		// 쿨타임 
};

enum EMPET_REQ_LEARNSKILL_FB
{
	EMPET_REQ_LEARNSKILL_FB_FAIL		= 0,		// 일반오류
	EMPET_REQ_LEARNSKILL_FB_OK			= 1,		// 성공
	EMPET_REQ_LEARNSKILL_FB_NOITEM		= 2,		// 아이템 없음
	EMPET_REQ_LEARNSKILL_FB_BADITEM		= 3,		// 사용 불가능 아이템
	EMPET_REQ_LEARNSKILL_FB_ALREADY		= 4,		// 이미 배운 스킬
	EMPET_REQ_LEARNSKILL_FB_INVALIDSTORAGE = 5,		// 유료하지 않은 창고
};

enum EMPET_REQ_SKILLSLOT_EXPANSION_FB
{
	EMPET_REQ_SKILLSLOT_EXPANSION_FB_FAIL		    = 0,		// 일반오류
	EMPET_REQ_SKILLSLOT_EXPANSION_FB_OK			    = 1,		// 성공
	EMPET_REQ_SKILLSLOT_EXPANSION_FB_NOITEM		    = 2,		// 아이템 없음
	EMPET_REQ_SKILLSLOT_EXPANSION_FB_BADITEM	    = 3,		// 사용 불가능 아이템
	EMPET_REQ_SKILLSLOT_EXPANSION_FB_ALREADY		= 4,		// 이미 배운 스킬
	EMPET_REQ_SKILLSLOT_EXPANSION_FB_INVALIDSTORAGE = 5,		// 유료하지 않은 창고
	EMPET_REQ_SKILLSLOT_EXPANSION_FB_MAX			= 6,		// 최대치에 도달
};

enum EMPET_REQ_REVIVE_FB
{
	EMPET_REQ_REVIVE_FB_FAIL			= 0,		// 일반오류
	EMPET_REQ_REVIVE_FB_OK				= 1,		// 성공
	EMPET_REQ_REVIVE_FB_DBFAIL			= 2,		// DB쿼리실패(부활실패)
};

enum EMPET_SKILLSLOT_EXPANSION_TYPE
{
	EMPET_SKILLSLOT_EXPANSION_TYPE_DUAL	= 0,
};

enum EMPET_CREATEPET_FB
{
    EMPET_CREATEPET_FB_FAIL = 0,
    EMPET_CREATEPET_FB_OK   = 1,
};

enum EMPET_GETPET_FROMDB_FB
{
    EMPET_GETPET_FROMDB_DBFAIL  = 0,
    EMPET_GETPET_FROMDB_NODATA  = 1,
};

namespace GLMSG
{
	#pragma pack(1)

    struct SNETPET_CREATEPET_FROMDB_FB : public NET_MSG_GENERIC
	{
        EMPET_CREATEPET_FB  emFB;
		DWORD				m_PetDbNum;
		WORD				wPosX;
		WORD				wPosY;

		enum { EMMSG_SIZE = sizeof(NET_MSG_GENERIC)+sizeof(EMPET_CREATEPET_FB)+sizeof(DWORD)*2 };
		char				m_cBUFFER[NET_DATA_BUFSIZE-EMMSG_SIZE];

		SNETPET_CREATEPET_FROMDB_FB ( EMPET_CREATEPET_FB _emFB, DWORD PetDbNum, WORD _wPosX, WORD _wPosY ) 
            : emFB(_emFB)
            , m_PetDbNum(PetDbNum)
            , wPosX(_wPosX)
            , wPosY(_wPosY)
		{
			dwSize = sizeof(SNETPET_CREATEPET_FROMDB_FB);
			nType = NET_MSG_CREATE_PET_FROMDB_FB;
			MIN_STATIC_ASSERT(sizeof(SNETPET_CREATEPET_FROMDB_FB)<=NET_DATA_BUFSIZE);
		}

        inline DWORD DbNum() const { return m_PetDbNum; }
	};
    
	struct SNETPET_GETPET_FROMDB_FB : public NET_MSG_GENERIC
	{
		GLPetField*			pPet;
        DWORD               dwClientID;
        DWORD               dwCharDBNum;
		DWORD				dwPetID;
		bool				bCardInfo;
		bool				bTrade;
		bool				bLMTItemCheck;

		enum { EMMSG_SIZE = sizeof(NET_MSG_GENERIC) + sizeof(PGLPET) + (sizeof(DWORD)*3) + (sizeof(bool)*3) };
		char				m_cBUFFER[NET_DATA_BUFSIZE-EMMSG_SIZE];

		explicit SNETPET_GETPET_FROMDB_FB(
            GLPetField* _pPet,
            DWORD _ClientSlot,
            DWORD _CharDBNum,
            DWORD _dwPetID,
            bool _bCardInfo,
            bool _bTrade,
            bool _bLMTItemCheck)
            : pPet(_pPet)
            , dwClientID(_ClientSlot)
            , dwCharDBNum(_CharDBNum)
            , dwPetID(_dwPetID)
            , bCardInfo(_bCardInfo)
            , bTrade(_bTrade)
            , bLMTItemCheck(_bLMTItemCheck)
		{
			dwSize = sizeof(SNETPET_GETPET_FROMDB_FB);
			nType = NET_MSG_GET_PET_FROMDB_FB;
			MIN_STATIC_ASSERT(sizeof(SNETPET_GETPET_FROMDB_FB)<=NET_DATA_BUFSIZE);
		}
	};

    struct SNETPET_GETPET_FROMDB_ERROR : public NET_MSG_GENERIC
	{
        EMPET_GETPET_FROMDB_FB  emFB;
		bool				bCardInfo;
		bool				bTrade;
		bool				bLMTItemCheck;
		WORD				wPosX;
		WORD				wPosY;

		enum { EMMSG_SIZE = sizeof(NET_MSG_GENERIC)+sizeof(EMPET_GETPET_FROMDB_FB)+sizeof(bool)*3+sizeof(WORD)+sizeof(WORD)};
		char				m_cBUFFER[NET_DATA_BUFSIZE-EMMSG_SIZE];

		SNETPET_GETPET_FROMDB_ERROR ( EMPET_GETPET_FROMDB_FB _emFB, bool _bCardInfo, bool _bTrade, bool _bLMTItemCheck, WORD _wPosX, WORD _wPosY )
            : emFB(_emFB)
            , bCardInfo(_bCardInfo)
            , bTrade(_bTrade)
            , bLMTItemCheck(_bLMTItemCheck)
            , wPosX(_wPosX)
            , wPosY(_wPosY)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GET_PET_FROMDB_ERROR;
			MIN_STATIC_ASSERT(sizeof(SNETPET_GETPET_FROMDB_ERROR)<=NET_DATA_BUFSIZE);
		}
	};


	struct SNETPET_REQ_USEPETCARD : public NET_MSG_GENERIC
	{
		WORD				wPosX;
		WORD				wPosY;

		SNETPET_REQ_USEPETCARD ( WORD _wPosX, WORD _wPosY ) 
            : wPosX(_wPosX)
            , wPosY(_wPosY)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_PET_REQ_USECARD;
			MIN_STATIC_ASSERT(sizeof(SNETPET_REQ_USEPETCARD)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPET_REQ_USEPETCARD_FB : public NET_MSG_GENERIC
	{
		char			m_szName[PETNAMESIZE+1];
		int			m_emTYPE;
		DWORD			m_dwGUID;
		SNATIVEID		m_sPetID;
		SNATIVEID		m_sActiveSkillID[MAX_PETSKILLSLOT];
		WORD			m_wMaxSkillSlot;
		PETSKILL		m_Skills[MAX_PETSKILL];
		DWORD			m_dwOwner;
		WORD			m_wStyle;
		WORD			m_wColor;
		float			m_fWalkSpeed;
		float			m_fRunSpeed;
		int				m_nFull;
		SITEMCUSTOM		m_PutOnItems[ACCETYPESIZE];
		MapID			m_sMapID;
		DWORD			m_dwCellID;
		WORD			m_wSkillNum;
		D3DXVECTOR3		m_vPos;
		D3DXVECTOR3		m_vDir;

		SPETSKINPACKDATA m_sPetSkinPackData;

		EMPET_USECARD_FB	emFB;

		DWORD			m_dwPetID;

		SNETPET_REQ_USEPETCARD_FB () :
			m_emTYPE(PETTYPE_NONE),
			m_dwGUID(UINT_MAX),
			m_sPetID(NATIVEID_NULL()),
			m_wMaxSkillSlot(0),
			m_dwOwner(0),
			m_wStyle(0),
			m_wColor(0),
			m_fWalkSpeed(0.0f),
			m_fRunSpeed(0.0f),
			m_nFull(0),
			m_wSkillNum(0),
			m_sMapID(NATIVEID_NULL()),
			m_dwCellID(0),
			m_vPos(0,0,0),
			m_vDir(0,0,0),
			emFB(EMPET_USECARD_FB_FAIL),
			m_dwPetID(0)
		{
			m_sPetSkinPackData.Init();
			memset ( m_szName, 0, PETNAMESIZE+1 );
			for ( int i=0; i<MAX_PETSKILLSLOT; ++i )
			{
				m_sActiveSkillID[i] = NATIVEID_NULL();
			}

			dwSize = sizeof(*this);
			nType = NET_MSG_PET_REQ_USECARD_FB;
			MIN_STATIC_ASSERT(sizeof(SNETPET_REQ_USEPETCARD_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPET_REQ_UNUSEPETCARD : public NET_MSG_GENERIC
	{
		DWORD				dwGUID;
		DWORD				dwPetID;

		SNETPET_REQ_UNUSEPETCARD ( DWORD _dwGUID, DWORD _dwPetID )
            : dwGUID(_dwGUID)
            , dwPetID(_dwPetID)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_PET_REQ_UNUSECARD;
			MIN_STATIC_ASSERT(sizeof(SNETPET_REQ_UNUSEPETCARD)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPET_REQ_UNUSEPETCARD_FB : public NET_MSG_GENERIC
	{
		DWORD				dwGUID;
		bool				bMoveMap;

		SNETPET_REQ_UNUSEPETCARD_FB ( DWORD _dwGUID, bool _bMoveMap )
            : dwGUID(_dwGUID)
            , bMoveMap(_bMoveMap)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_PET_REQ_UNUSECARD_FB;
			MIN_STATIC_ASSERT(sizeof(SNETPET_REQ_UNUSEPETCARD_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPET_REQ_PETREVIVEINFO : public NET_MSG_GENERIC
	{
		DWORD				dwCharID;

		SNETPET_REQ_PETREVIVEINFO ( DWORD _dwCharID )
            : dwCharID(_dwCharID)
		{
			dwSize = sizeof(*this);
			nType  = NET_MSG_PET_REQ_PETREVIVEINFO;
			MIN_STATIC_ASSERT(sizeof(SNETPET_REQ_PETREVIVEINFO)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPET_REQ_PETREVIVEINFO_FB : public NET_MSG_GENERIC
	{
		PETREVIVEINFO		arryPetReviveInfo[MAXPETREVINFO];
		WORD				wPetReviveNum;

		SNETPET_REQ_PETREVIVEINFO_FB () 
            : wPetReviveNum(0)
		{
			dwSize = sizeof(*this);
			nType  = NET_MSG_PET_REQ_PETREVIVEINFO_FB;
			MIN_STATIC_ASSERT(sizeof(SNETPET_REQ_PETREVIVEINFO_FB)<=NET_DATA_BUFSIZE);
		}
	};

    struct SNETPET_REQ_PETCARDINFO : public NET_MSG_GENERIC
	{
		DWORD m_PetDbNum;
		bool  bTrade;

		SNETPET_REQ_PETCARDINFO ( DWORD PetDbNum, bool _bTrade = false )
            : m_PetDbNum(PetDbNum)
            , bTrade(_bTrade)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_PET_REQ_PETCARDINFO;
			MIN_STATIC_ASSERT(sizeof(SNETPET_REQ_PETCARDINFO)<=NET_DATA_BUFSIZE);
		}

        inline DWORD PetDbNum() const { return m_PetDbNum; }
	};

	struct SNETPET_REQ_PETCARDINFO_FB : public NET_MSG_GENERIC
	{
		char				szName[PETNAMESIZE+1];
        DWORD				dwPetID;	
		int				emTYPE;
		int					nFull;

		WORD				wMaxSkillSlot;
		WORD				wSkillNum;

        bool				bTrade;

        SITEMCUSTOM			PutOnItems[ACCETYPESIZE];
        SNATIVEID			sActiveSkillID[MAX_PETSKILLSLOT];		
		PETSKILL			Skills[MAX_PETSKILL];		

		SNETPET_REQ_PETCARDINFO_FB(const char* _szName, DWORD _dwPetID, int _emType, int _nFull, WORD _wMaxSkillSlot, WORD _wSkillNum, bool _bTrade)
            : dwPetID(_dwPetID)
            , emTYPE(_emType)
            , nFull(_nFull)
            , wMaxSkillSlot(_wMaxSkillSlot)
            , wSkillNum(_wSkillNum)
			, bTrade(_bTrade)
		{
            StringCchCopy (szName, PETNAMESIZE+1, _szName );

			for ( WORD i=0; i<MAX_PETSKILLSLOT; ++i )
			{
				sActiveSkillID[i] = NATIVEID_NULL();
			}

			dwSize = sizeof(*this);
			nType = NET_MSG_PET_REQ_PETCARDINFO_FB;
			MIN_STATIC_ASSERT(sizeof(SNETPET_REQ_PETCARDINFO_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPET_REQ_DISAPPEAR : public NET_MSG_GENERIC
	{
		DWORD				dwGUID;

		SNETPET_REQ_DISAPPEAR () :
			dwGUID(UINT_MAX)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_PET_REQ_DISAPPEAR;
			MIN_STATIC_ASSERT(sizeof(SNETPET_REQ_DISAPPEAR)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPET_CREATE_ANYPET : public NET_MSG_GENERIC
	{
		SDROPPET			Data;

		SNETPET_CREATE_ANYPET ( SDROPPET _Data )
            : Data(_Data)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_CREATE_ANYPET;
			MIN_STATIC_ASSERT(sizeof(SNETPET_CREATE_ANYPET)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPET_DROP_PET : public NET_MSG_GENERIC
	{
		SDROPPET			Data;

		SNETPET_DROP_PET ()           
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_PET_DROPPET;
			MIN_STATIC_ASSERT(sizeof(SNETPET_DROP_PET)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPET_ATTACK : public NET_MSG_GENERIC
	{
		STARGETID			sTarID;

		SNETPET_ATTACK ( STARGETID _sTarID )
            : sTarID ( _sTarID )
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_PET_ATTACK;
			MIN_STATIC_ASSERT(sizeof(SNETPET_ATTACK)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPET_SAD : public NET_MSG_GENERIC
	{
		bool bReset;
		SNETPET_SAD ():bReset(false)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_PET_SAD;
			MIN_STATIC_ASSERT(sizeof(SNETPET_SAD)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPET_REQ_UPDATE_MOVESTATE : public NET_MSG_GENERIC
	{
		DWORD				dwFlag;

		SNETPET_REQ_UPDATE_MOVESTATE ( DWORD _dwFlag )
            : dwFlag(_dwFlag)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_PET_REQ_UPDATE_MOVE_STATE;
			MIN_STATIC_ASSERT(sizeof(SNETPET_REQ_UPDATE_MOVESTATE)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPET_REQ_UPDATE_MOVESTATE_FB : public NET_MSG_GENERIC
	{
		DWORD				dwFlag;

		SNETPET_REQ_UPDATE_MOVESTATE_FB ( DWORD _dwFlag )
            : dwFlag(_dwFlag)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_PET_REQ_UPDATE_MOVE_STATE_FB;
			MIN_STATIC_ASSERT(sizeof(SNETPET_REQ_UPDATE_MOVESTATE_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPET_UPDATE_CLIENT_PETFULL : public NET_MSG_GENERIC
	{
		int					nFull;

		SNETPET_UPDATE_CLIENT_PETFULL ( int _nFull )
            : nFull(_nFull)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_PET_UPDATECLIENT_FULL;
			MIN_STATIC_ASSERT(sizeof(SNETPET_UPDATE_CLIENT_PETFULL)<=NET_DATA_BUFSIZE);
		}
	};

    //! Pet Skill Change
	struct NET_PET_REQ_SKILL_CHANGE_CF : public NET_MSG_GENERIC
	{
		SNATIVEID sSkillID;
		WORD	  wSlot;

		NET_PET_REQ_SKILL_CHANGE_CF ( SNATIVEID _sSkillID, WORD _wSlot )
            : sSkillID(_sSkillID)
            , wSlot(_wSlot)
		{
			dwSize = sizeof(NET_PET_REQ_SKILL_CHANGE_CF);
			nType = NET_MSG_PET_REQ_SKILL_CHANGE_CF;
			MIN_STATIC_ASSERT(sizeof(NET_PET_REQ_SKILL_CHANGE_CF)<=NET_DATA_BUFSIZE);
		}
	};

	//! Pet skill change result
    struct SNETPET_REQ_SKILLCHANGE_FB : public NET_MSG_GENERIC
	{
		SNATIVEID					sSkillID;
		EMPET_REQ_CHANGESKILL_FB	emFB;

		SNETPET_REQ_SKILLCHANGE_FB ( SNATIVEID _sSkillID, EMPET_REQ_CHANGESKILL_FB _emFB )
            : sSkillID(_sSkillID)
            , emFB(_emFB)
			
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_PET_REQ_SKILLCHANGE_FB;
			MIN_STATIC_ASSERT(sizeof(SNETPET_REQ_SKILLCHANGE_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPET_ADD_SKILLFACT : public NET_MSG_GENERIC
	{
		struct SPETSKILLFACT
		{
			SNATIVEID			sNATIVEID;
			SKILL::EMTYPES		emTYPE;
			float				fMVAR;
			float				fAge;

		} sPETSKILLFACT[MAX_PETSKILLSLOT];

		SNETPET_ADD_SKILLFACT ()
		{
			for ( WORD i=0; i<MAX_PETSKILLSLOT; ++i )
			{
				sPETSKILLFACT[i].sNATIVEID = NATIVEID_NULL();
				sPETSKILLFACT[i].emTYPE    = SKILL::FOR_TYPE_SIZE;
				sPETSKILLFACT[i].fMVAR     = 0.0f;
				sPETSKILLFACT[i].fAge      = 0.0f;
			}

			dwSize = sizeof(*this);
			nType = NET_MSG_PET_ADD_SKILLFACT;
            MIN_STATIC_ASSERT(sizeof(SNETPET_ADD_SKILLFACT)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPET_REMOVE_SKILLFACT : public NET_MSG_GENERIC
	{
		WORD				wSlot;

		SNETPET_REMOVE_SKILLFACT ( WORD _wSlot ) 
            : wSlot (_wSlot)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_PET_REMOVE_SKILLFACT;
            MIN_STATIC_ASSERT(sizeof(SNETPET_REMOVE_SKILLFACT)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPET_REQ_GOTO : public NET_MSG_GENERIC
	{
		DWORD				dwGUID;
		D3DXVECTOR3			vCurPos;
		D3DXVECTOR3			vTarPos;
		DWORD				dwFlag;

		SNETPET_REQ_GOTO ( DWORD _dwGUID, D3DXVECTOR3 _vCurPos, D3DXVECTOR3 _vTarPos, DWORD _dwFlag ) 
            : dwGUID(_dwGUID)
            , vCurPos(_vCurPos)
            , vTarPos(_vTarPos)
            , dwFlag(_dwFlag)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_PET_REQ_GOTO;
			MIN_STATIC_ASSERT(sizeof(SNETPET_REQ_GOTO)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPET_REQ_GOTO_FB : public NET_MSG_GENERIC
	{
		DWORD				dwGUID;
		D3DXVECTOR3			vPos;

		SNETPET_REQ_GOTO_FB ( DWORD _dwGUID, D3DXVECTOR3 _vPos )
            : dwGUID(_dwGUID)
            , vPos(_vPos)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_PET_REQ_GOTO_FB;
			MIN_STATIC_ASSERT(sizeof(SNETPET_REQ_GOTO_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPET_REQ_STOP : public NET_MSG_GENERIC
	{
		DWORD				dwGUID;
		DWORD				dwFlag;
		D3DXVECTOR3			vPos;
		bool				bStopAttack;

		SNETPET_REQ_STOP ( DWORD _dwGUID, DWORD _dwFlag, D3DXVECTOR3 _vPos, bool _bStopAttack )
            : dwGUID(_dwGUID)
            , dwFlag(_dwFlag)
            , vPos(_vPos)
            , bStopAttack(_bStopAttack)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_PET_REQ_STOP;
			MIN_STATIC_ASSERT(sizeof(SNETPET_REQ_STOP)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPET_REQ_GETRIGHTOFITEM : public NET_MSG_GENERIC
	{
		DWORD				dwGUID;
		DWORD				dwGetFlag;

		SNETPET_REQ_GETRIGHTOFITEM ( DWORD _dwGUID, DWORD _dwGetFlag )
            : dwGUID(_dwGUID)
            , dwGetFlag(_dwGetFlag)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_PET_GETRIGHTOFITEM;
			MIN_STATIC_ASSERT(sizeof(SNETPET_REQ_GETRIGHTOFITEM)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPET_REQ_GETRIGHTOFITEM_FB : public NET_MSG_GENERIC
	{
		EMPET_REQ_GETRIGHTOFITEM_FB emFB;
		WORD						wSum;
		STARGETID					DroppedItems[MAXRIGHTOFITEM];

		SNETPET_REQ_GETRIGHTOFITEM_FB () :
			wSum(0),
			emFB(EMPET_REQ_GETRIGHTOFITEM_FB_OK)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_PET_GETRIGHTOFITEM_FB;
			MIN_STATIC_ASSERT(sizeof(SNETPET_REQ_GETRIGHTOFITEM_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPET_MSG_NOTENOUGHINVEN : public NET_MSG_GENERIC
	{
		SNETPET_MSG_NOTENOUGHINVEN ()
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_PET_NOTENOUGHINVEN;
			MIN_STATIC_ASSERT(sizeof(SNETPET_MSG_NOTENOUGHINVEN)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPET_REQ_GIVEFOOD : public NET_MSG_GENERIC
	{
		WORD						wPosX;
		WORD						wPosY;
		int						emPetTYPE;
        WORD                        wHoldPosX;
        WORD                        wHoldPosY;

		SNETPET_REQ_GIVEFOOD ( WORD _wPosX, WORD _wPosY, int _emPetTYPE)
            : wPosX(_wPosX)
            , wPosY(_wPosY)
            , emPetTYPE(_emPetTYPE)
            , wHoldPosX(0)
            , wHoldPosY(0)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_PET_REQ_GIVEFOOD;
			MIN_STATIC_ASSERT(sizeof(SNETPET_REQ_GIVEFOOD)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPET_REQ_GIVEFOOD_FB : public NET_MSG_GENERIC
	{
		DWORD						dwPetID;
		SNATIVEID					sNativeID;
		int							nFull;

		EMPET_REQ_GIVEFOOD_FB		emFB;

		SNETPET_REQ_GIVEFOOD_FB ( DWORD _dwPetID, SNATIVEID _sNativeID, int _nFull, EMPET_REQ_GIVEFOOD_FB _emFB )
            : dwPetID(_dwPetID)
            , sNativeID(_sNativeID)
            , nFull(_nFull)
            , emFB(_emFB)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_PET_REQ_GIVEFOOD_FB;
			MIN_STATIC_ASSERT(sizeof(SNETPET_REQ_GIVEFOOD_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPET_REQ_GETFULL_FROMDB_FB : public NET_MSG_GENERIC
	{
		DWORD						dwPetID;
		int							nFull;
		WORD						wPosX;
		WORD						wPosY;
		WORD						wCureVolume;
		BOOL						bRatio;
		SNATIVEID					sNativeID;
		EMPET_REQ_GIVEFOOD_FB		emFB;
		int						    emType;
        WORD                        wHoldPosX;
        WORD                        wHoldPosY;


		enum { EMMSG_SIZE = sizeof(NET_MSG_GENERIC)+sizeof(DWORD)+sizeof(int)+sizeof(WORD)*3+sizeof(BOOL)+
                            sizeof(SNATIVEID)+sizeof(EMPET_REQ_GIVEFOOD_FB)+sizeof(int) + sizeof(WORD)*2 };
		char				m_cBUFFER[NET_DATA_BUFSIZE-EMMSG_SIZE];

		SNETPET_REQ_GETFULL_FROMDB_FB () :
			dwPetID(0),
			nFull(0),
			wPosX(0),
			wPosY(0),
			wCureVolume(0),
			bRatio(false),
			sNativeID(NATIVEID_NULL()),
			emFB(EMPET_REQ_GETFULL_FROMDB_FAIL),
			emType(PETTYPE_NONE),
            wHoldPosX(0),
            wHoldPosY(0)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_PET_REQ_GETFULL_FROMDB_FB;
            MIN_STATIC_ASSERT(sizeof(SNETPET_REQ_GETFULL_FROMDB_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPET_REQ_RENAME : public NET_MSG_GENERIC
	{
		char					szName[PETNAMESIZE+1];
		WORD					wPosX;
		WORD					wPosY;

		SNETPET_REQ_RENAME ( const char* _szName, WORD _wPosX, WORD _wPosY ) 
            : wPosX(_wPosX)
            , wPosY(_wPosY)
		{
			dwSize = sizeof(*this);
            nType  = NET_MSG_PET_REQ_RENAME;
			MIN_STATIC_ASSERT(sizeof(SNETPET_REQ_RENAME)<=NET_DATA_BUFSIZE);
            
            StringCchCopy ( szName, PETNAMESIZE+1, _szName );			
		}
	};

	struct SNETPET_REQ_RENAME_FB : public NET_MSG_GENERIC
	{
		EMPET_REQ_RENAME_FB		emFB;
		char					szName[PETNAMESIZE+1];

		SNETPET_REQ_RENAME_FB ( EMPET_REQ_RENAME_FB _emFB )
            : emFB(_emFB)
		{
			dwSize = sizeof(*this);
			memset ( szName, 0, sizeof(char)*(PETNAMESIZE+1) );
			nType = NET_MSG_PET_REQ_RENAME_FB;
            MIN_STATIC_ASSERT(sizeof(SNETPET_REQ_RENAME_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPET_REQ_RENAME_FROMDB_FB : public NET_MSG_GENERIC
	{
		EMPET_REQ_RENAME_FB			emFB;
		WORD						wPosX;
		WORD						wPosY;
		char						szName[PETNAMESIZE+1];

		enum { EMMSG_SIZE = sizeof(NET_MSG_GENERIC)+sizeof(EMPET_REQ_RENAME_FB)+sizeof(WORD)+sizeof(WORD)+sizeof(char)*(PETNAMESIZE+1) };
		char				m_cBUFFER[NET_DATA_BUFSIZE-EMMSG_SIZE];

		SNETPET_REQ_RENAME_FROMDB_FB () :
			emFB(EMPET_REQ_RENAME_FB_FB_FAIL),
			wPosX(0),
			wPosY(0)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_PET_REQ_RENAME_FROMDB_FB;

			memset( szName, 0, sizeof(char)*(PETNAMESIZE+1) );
			memset( m_cBUFFER, 0, NET_DATA_BUFSIZE-EMMSG_SIZE );
            MIN_STATIC_ASSERT(sizeof(SNETPET_REQ_RENAME_FROMDB_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPET_REQ_CHANGE_COLOR : public NET_MSG_GENERIC
	{
		WORD					wColor;
		WORD					wPosX;
		WORD					wPosY;

		SNETPET_REQ_CHANGE_COLOR ( WORD _wColor, WORD _wPosX, WORD _wPosY ) 
            : wColor(_wColor)
            , wPosX(_wPosX)
            , wPosY(_wPosY)

		{
			dwSize = sizeof(*this);
			nType  = NET_MSG_PET_REQ_CHANGE_COLOR;
            MIN_STATIC_ASSERT(sizeof(SNETPET_REQ_CHANGE_COLOR)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPET_REQ_CHANGE_COLOR_FB : public NET_MSG_GENERIC
	{
		EMPET_REQ_STYLECOLOR_CHANGE_FB  emFB;
		WORD						    wColor;

		SNETPET_REQ_CHANGE_COLOR_FB ( EMPET_REQ_STYLECOLOR_CHANGE_FB _emFB, WORD _wColor )
            : emFB(_emFB)
            , wColor(_wColor)
			
		{
			dwSize = sizeof(*this);
			nType  = NET_MSG_PET_REQ_CHANGE_COLOR_FB;
            MIN_STATIC_ASSERT(sizeof(SNETPET_REQ_CHANGE_COLOR_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPET_REQ_CHANGE_STYLE : public NET_MSG_GENERIC
	{
		WORD					wStyle;
		WORD					wPosX;
		WORD					wPosY;

		SNETPET_REQ_CHANGE_STYLE ( WORD _wStyle, WORD _wPosX, WORD _wPosY )
            : wStyle(_wStyle)
            , wPosX(_wPosX)
            , wPosY(_wPosY)

		{
			dwSize = sizeof(*this);
			nType  = NET_MSG_PET_REQ_CHANGE_STYLE;
            MIN_STATIC_ASSERT(sizeof(SNETPET_REQ_CHANGE_STYLE)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPET_REQ_CHANGE_STYLE_FB : public NET_MSG_GENERIC
	{
		EMPET_REQ_STYLECOLOR_CHANGE_FB  emFB;
        WORD						    wStyle;
		WORD							wColor;
		


		SNETPET_REQ_CHANGE_STYLE_FB (EMPET_REQ_STYLECOLOR_CHANGE_FB _emFB, WORD _wStyle, WORD _wColor ) 
            : emFB(_emFB)
            , wStyle(_wStyle)
            , wColor(_wColor)
			
		{
			dwSize = sizeof(*this);
			nType  = NET_MSG_PET_REQ_CHANGE_STYLE_FB;
            MIN_STATIC_ASSERT(sizeof(SNETPET_REQ_CHANGE_STYLE_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPET_REQ_INVEN_EX_SLOT : public NET_MSG_GENERIC
	{
		EMSUIT				emSuit;

        WORD                wHoldPosX;
        WORD                wHoldPosY;

		SNETPET_REQ_INVEN_EX_SLOT ( EMSUIT _emSuit ) 
			: emSuit(_emSuit)
            , wHoldPosX(0)
            , wHoldPosY(0)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_PET_REQ_INVEN_EX_SLOT;
            MIN_STATIC_ASSERT(sizeof(SNETPET_REQ_INVEN_EX_SLOT)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPET_REQ_SLOT_EX_HOLD_FB : public NET_MSG_GENERIC
	{
		EMPET_REQ_SLOT_EX_HOLD_FB emFB;
		SITEMCUSTOM sHoldtoSlotItemCustom;
		EMSUIT					  emSuit;

		SNETPET_REQ_SLOT_EX_HOLD_FB ( EMPET_REQ_SLOT_EX_HOLD_FB _emFB, EMSUIT _emSuit ) 
			: emFB(_emFB)
            , emSuit(_emSuit)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_PET_REQ_SLOT_EX_HOLD_FB;
            MIN_STATIC_ASSERT(sizeof(SNETPET_REQ_SLOT_EX_HOLD_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPET_REQ_INVEN_TO_SLOT : public NET_MSG_GENERIC
	{
		EMSUIT				emSuit;

        WORD                wHoldPosX;
        WORD                wHoldPosY;

		SNETPET_REQ_INVEN_TO_SLOT ( EMSUIT _emSuit ) 
			: emSuit(_emSuit)
            , wHoldPosX(0)
            , wHoldPosY(0)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_PET_REQ_INVEN_TO_SLOT;
            MIN_STATIC_ASSERT(sizeof(SNETPET_REQ_INVEN_TO_SLOT)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPET_REQ_HOLD_TO_SLOT_FB : public NET_MSG_GENERIC
	{
		EMPET_REQ_SLOT_EX_HOLD_FB emFB;
		EMSUIT					  emSuit;
		SITEMCUSTOM				  sItemCustom;

		SNETPET_REQ_HOLD_TO_SLOT_FB ( EMPET_REQ_SLOT_EX_HOLD_FB _emFB, EMSUIT _emSuit ) 
			: emFB(_emFB)
            , emSuit(_emSuit)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_PET_REQ_HOLD_TO_SLOT_FB;
            MIN_STATIC_ASSERT(sizeof(SNETPET_REQ_HOLD_TO_SLOT_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPET_REQ_SLOT_TO_INVEN : public NET_MSG_GENERIC
	{
		EMSUIT				emSuit;

        WORD                wPosX;
        WORD                wPosY;

		SNETPET_REQ_SLOT_TO_INVEN ( EMSUIT _emSuit ) 
			: emSuit(_emSuit)
            , wPosX(0)
            , wPosY(0)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_PET_REQ_SLOT_TO_INVEN;
            MIN_STATIC_ASSERT(sizeof(SNETPET_REQ_SLOT_TO_INVEN)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPET_REQ_SLOT_TO_HOLD_FB : public NET_MSG_GENERIC
	{
		EMPET_REQ_SLOT_EX_HOLD_FB emFB;
		EMSUIT					  emSuit;

		SNETPET_REQ_SLOT_TO_HOLD_FB ( EMPET_REQ_SLOT_EX_HOLD_FB _emFB, EMSUIT _emSuit ) 
			: emFB(_emFB)
            , emSuit(_emSuit)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_PET_REQ_SLOT_TO_HOLD_FB;
            MIN_STATIC_ASSERT(sizeof(SNETPET_REQ_SLOT_TO_HOLD_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPET_REQ_REMOVE_SLOTITEM : public NET_MSG_GENERIC
	{
        EMSUIT  emSuit;
    
        SNETPET_REQ_REMOVE_SLOTITEM ( EMSUIT _emSuit ) 
			: emSuit(_emSuit)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_PET_REMOVE_SLOTITEM;
            MIN_STATIC_ASSERT(sizeof(SNETPET_REQ_REMOVE_SLOTITEM)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPET_REQ_REMOVE_SLOTITEM_FB : public NET_MSG_GENERIC
	{
		EMSUIT				emSuit;

		SNETPET_REQ_REMOVE_SLOTITEM_FB ( EMSUIT _emSuit ) 
			: emSuit(_emSuit)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_PET_REMOVE_SLOTITEM_FB;
            MIN_STATIC_ASSERT(sizeof(SNETPET_REQ_REMOVE_SLOTITEM_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPET_REQ_LEARNSKILL : public NET_MSG_GENERIC
	{
		WORD				wPosX;
		WORD				wPosY;
		bool				bStorage; // 창고여부
		DWORD				dwChannel;

		SNETPET_REQ_LEARNSKILL ( WORD _wPosX, WORD _wPosY, bool _bStorage = false, DWORD _dwChannel = 0  )
			: wPosX(_wPosX)
			, wPosY(_wPosY)
			, bStorage(_bStorage)
			, dwChannel(_dwChannel)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_PET_REQ_LEARNSKILL;
            MIN_STATIC_ASSERT(sizeof(SNETPET_REQ_LEARNSKILL)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPET_REQ_LEARNSKILL_FB : public NET_MSG_GENERIC
	{
		EMPET_REQ_LEARNSKILL_FB		emFB;
		WORD						wPosX;
		WORD						wPosY;

		SNETPET_REQ_LEARNSKILL_FB ( EMPET_REQ_LEARNSKILL_FB _emFB = EMPET_REQ_LEARNSKILL_FB_FAIL, WORD _wPosX = 0, WORD _wPosY = 0 )
			: emFB(_emFB)
			, wPosX(_wPosX)
			, wPosY(_wPosY)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_PET_REQ_LEARNSKILL_FB;
            MIN_STATIC_ASSERT(sizeof(EMPET_REQ_LEARNSKILL_FB_FAIL)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPET_REQ_FUNNY : public NET_MSG_GENERIC
	{
		WORD				wFunny;

		SNETPET_REQ_FUNNY ( WORD _wFunny = 1 )
			: wFunny(_wFunny)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_PET_REQ_FUNNY;
            MIN_STATIC_ASSERT(sizeof(SNETPET_REQ_FUNNY)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPET_REQ_REVIVE : public NET_MSG_GENERIC
	{
		DWORD				dwPetID;
		WORD				wPosX;
		WORD				wPosY;

        WORD                wHoldPosX;
        WORD                wHoldPosY;

		SNETPET_REQ_REVIVE ( DWORD _dwPetID = 0 , WORD _wPosX = 0 , WORD _wPosY = 0  )
			: dwPetID(_dwPetID)
			, wPosX(_wPosX)
			, wPosY(_wPosY)
            , wHoldPosX(0)
            , wHoldPosY(0)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_PET_REQ_REVIVE;
            MIN_STATIC_ASSERT(sizeof(SNETPET_REQ_REVIVE)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPET_REQ_REVIVE_FB : public NET_MSG_GENERIC
	{
        EMPET_REQ_REVIVE_FB emFB;
		DWORD				dwPetID;
		

		SNETPET_REQ_REVIVE_FB ( EMPET_REQ_REVIVE_FB _emFB = EMPET_REQ_REVIVE_FB_FAIL, DWORD _dwPetID = 0 )
			: emFB(_emFB)
            , dwPetID(_dwPetID)
			  
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_PET_REQ_REVIVE_FB;
            MIN_STATIC_ASSERT(sizeof(SNETPET_REQ_REVIVE_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPET_REQ_REVIVE_FROMDB_FB : public NET_MSG_GENERIC
	{
		DWORD				m_PetDbNum;
		WORD				wPosX;
		WORD				wPosY;

        WORD                wHoldPosX;
        WORD                wHoldPosY;


		EMPET_REQ_REVIVE_FB emFB;

		enum { EMMSG_SIZE = sizeof(NET_MSG_GENERIC) + sizeof(DWORD)*2+ sizeof(EMPET_REQ_REVIVE_FB) + sizeof(WORD)*2 };
		char				m_cBUFFER[NET_DATA_BUFSIZE-EMMSG_SIZE];

		SNETPET_REQ_REVIVE_FROMDB_FB( DWORD _dwPetID = 0, WORD _wPosX = 0, WORD _wPosY = 0, 
                                       EMPET_REQ_REVIVE_FB _emFB = EMPET_REQ_REVIVE_FB_FAIL )
			: m_PetDbNum(_dwPetID)
            , wPosX(_wPosX)
			, wPosY(_wPosY)
			, emFB(_emFB)
		{
			dwSize = sizeof(*this);
			nType  = NET_MSG_PET_REQ_REVIVE_FROMDB_FB;
            MIN_STATIC_ASSERT(sizeof(SNETPET_REQ_REVIVE_FROMDB_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPET_ACCESSORY_DELETE : public NET_MSG_GENERIC
	{
		ACCESSORYTYPE		accetype;

		SNETPET_ACCESSORY_DELETE ( ACCESSORYTYPE _accetype = ACCETYPESIZE )
			: accetype(_accetype)
		{
			dwSize = sizeof(*this);
			nType  = NET_MSG_PET_ACCESSORY_DELETE;
            MIN_STATIC_ASSERT(sizeof(SNETPET_ACCESSORY_DELETE)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPET_SKINPACKOPEN : public NET_MSG_GENERIC
	{
		WORD					wPosX;
		WORD					wPosY;
		SNATIVEID				sItemId;
        WORD                    wHoldPosX;
        WORD                    wHoldPosY;

		SNETPET_SKINPACKOPEN ( WORD _wPosX = 0, WORD _wPosY = 0, SNATIVEID _sItemId = NATIVEID_NULL() ) 
			: wPosX(_wPosX)
			, wPosY(_wPosY)
			, sItemId (_sItemId)
            , wHoldPosX(0)
            , wHoldPosY(0)
		{
			dwSize = sizeof(SNETPET_SKINPACKOPEN);
			nType = NET_MSG_PET_PETSKINPACKOPEN;
			MIN_STATIC_ASSERT(sizeof(SNETPET_SKINPACKOPEN)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPET_SKINPACKOPEN_FB : public NET_MSG_GENERIC
	{
		EMPET_PETSKINPACKOPEN_FB	emFB;
		SPETSKINPACKDATA			sPetSkinPackData;

		SNETPET_SKINPACKOPEN_FB ( EMPET_PETSKINPACKOPEN_FB _emFB = EMPET_PETSKINPACKOPEN_FB_FAIL ) 
			: emFB(_emFB)
		{
			sPetSkinPackData.Init();

			dwSize = sizeof(SNETPET_SKINPACKOPEN_FB);
			nType = NET_MSG_PET_PETSKINPACKOPEN_FB;
			MIN_STATIC_ASSERT(sizeof(SNETPET_SKINPACKOPEN_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPET_SKINPACKOPEN_BRD : public SNETPET_BROAD
	{
		SPETSKINPACKDATA		sPetSkinPackData;

		SNETPET_SKINPACKOPEN_BRD () 
		{
			sPetSkinPackData.Init();

			dwSize = sizeof(SNETPET_SKINPACKOPEN_BRD);
			nType = NET_MSG_PET_PETSKINPACKOPEN_BRD;
			MIN_STATIC_ASSERT(sizeof(SNETPET_SKINPACKOPEN_BRD)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPET_REQ_SKILLSLOT_EXPANSION : public NET_MSG_GENERIC
	{
		EMPET_SKILLSLOT_EXPANSION_TYPE	emType;
		WORD							wPosX;
		WORD							wPosY;
        bool							bStorage; // 창고여부
        DWORD							dwChannel;
		

		SNETPET_REQ_SKILLSLOT_EXPANSION ( EMPET_SKILLSLOT_EXPANSION_TYPE _emType = EMPET_SKILLSLOT_EXPANSION_TYPE_DUAL, 
                                          WORD _wPosX = 0, WORD _wPosY = 0, bool _bStorage = false, DWORD _dwChannel = 0  )
			: emType(_emType)
			, wPosX(_wPosX)
			, wPosY(_wPosY)
            , bStorage(_bStorage)
            , dwChannel(_dwChannel)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_PET_REQ_SKILLSLOT_EXPANSION;
            MIN_STATIC_ASSERT(sizeof(SNETPET_REQ_SKILLSLOT_EXPANSION)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPET_REQ_SKILLSLOT_EXPANSION_FB : public NET_MSG_GENERIC
	{
		EMPET_SKILLSLOT_EXPANSION_TYPE	 emType;
		EMPET_REQ_SKILLSLOT_EXPANSION_FB emFB;
		WORD							 wPosX;
		WORD							 wPosY;
		WORD							 wMaxSlot;

		SNETPET_REQ_SKILLSLOT_EXPANSION_FB ( EMPET_SKILLSLOT_EXPANSION_TYPE _emType = EMPET_SKILLSLOT_EXPANSION_TYPE_DUAL,
                                             EMPET_REQ_SKILLSLOT_EXPANSION_FB _emFB = EMPET_REQ_SKILLSLOT_EXPANSION_FB_FAIL,
                                             WORD _wPosX = 0, WORD _wPosY = 0, WORD _wMaxSlot = 0 )
			: emType(_emType)
			, emFB(_emFB)
			, wPosX(_wPosX)
			, wPosY(_wPosY)
			, wMaxSlot(_wMaxSlot)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_PET_REQ_SKILLSLOT_EXPANSION_FB;
            MIN_STATIC_ASSERT(sizeof(SNETPET_REQ_SKILLSLOT_EXPANSION_FB)<=NET_DATA_BUFSIZE);
		}
	};


//-----------------------------------------------------------------------------------------------------
	struct SNETPET_GOTO_BRD : public SNETPET_BROAD
	{
		D3DXVECTOR3			vCurPos;
		D3DXVECTOR3			vTarPos;
		DWORD				dwFlag;

		SNETPET_GOTO_BRD ( D3DXVECTOR3 _vCurPos, D3DXVECTOR3 _vTarPos, DWORD _dwFlag )
            : vCurPos(_vCurPos)
            , vTarPos(_vTarPos)
            , dwFlag(_dwFlag)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_PET_GOTO_BRD;
			MIN_STATIC_ASSERT(sizeof(SNETPET_GOTO_BRD)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPET_STOP_BRD : public SNETPET_BROAD
	{
		D3DXVECTOR3			vPos;
		DWORD				dwFlag;
		bool				bStopAttack;

		SNETPET_STOP_BRD ( D3DXVECTOR3 _vPos, DWORD _dwFlag, bool _bStopAttack = false ) 
            : vPos(_vPos)
            , dwFlag(_dwFlag)
            , bStopAttack(_bStopAttack)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_PET_STOP_BRD;
			MIN_STATIC_ASSERT(sizeof(SNETPET_STOP_BRD)<=NET_DATA_BUFSIZE);
		}
	};

    //! Pet Skill Change
	struct NET_PET_REQ_SKILL_CHANGE_FC : public SNETPET_BROAD
	{
		struct
		{
			SNATIVEID sSkillID;
			DWORD	  dwTarID;
		} sSKILL[MAX_PETSKILLSLOT];

		NET_PET_REQ_SKILL_CHANGE_FC()
		{
			for (WORD i=0; i<MAX_PETSKILLSLOT; ++i)
			{
				sSKILL[i].sSkillID  = NATIVEID_NULL();
				sSKILL[i].dwTarID   = 0;
			}

			dwSize = sizeof(NET_PET_REQ_SKILL_CHANGE_FC);
			nType = NET_MSG_PET_REQ_SKILL_CHANGE_FC;
			MIN_STATIC_ASSERT(sizeof(NET_PET_REQ_SKILL_CHANGE_FC)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPET_REQ_UPDATE_MOVESTATE_BRD : public SNETPET_BROAD
	{
		DWORD				dwFlag;

		SNETPET_REQ_UPDATE_MOVESTATE_BRD ( DWORD _dwFlag = 0 )
            : dwFlag(_dwFlag)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_PET_REQ_UPDATE_MOVE_STATE_BRD;
			MIN_STATIC_ASSERT(sizeof(SNETPET_REQ_UPDATE_MOVESTATE_BRD)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPET_ATTACK_BRD : public SNETPET_BROAD
	{
		STARGETID				sTarID;

		SNETPET_ATTACK_BRD ( STARGETID _sTarID )
            : sTarID ( _sTarID ) 
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_PET_ATTACK_BRD;
			MIN_STATIC_ASSERT(sizeof(SNETPET_ATTACK_BRD)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPET_SAD_BRD : public SNETPET_BROAD
	{
		DWORD					dwOwner;
		bool					bReset;
		SNETPET_SAD_BRD ( DWORD _dwOwner = UINT_MAX )
            : dwOwner(_dwOwner)
			, bReset(false)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_PET_SAD_BRD;
			MIN_STATIC_ASSERT(sizeof(SNETPET_SAD_BRD)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPET_REQ_RENAME_BRD : public SNETPET_BROAD
	{
		char 		szName[PETNAMESIZE+1];

		SNETPET_REQ_RENAME_BRD ( char* _szName )
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_PET_REQ_RENAME_BRD;
            MIN_STATIC_ASSERT(sizeof(SNETPET_REQ_RENAME_BRD)<=NET_DATA_BUFSIZE);

            StringCchCopy ( szName, PETNAMESIZE+1, _szName );
		}
	};

	struct SNETPET_REQ_CHANGE_COLOR_BRD  : public SNETPET_BROAD
	{
		WORD				    wColor;

		SNETPET_REQ_CHANGE_COLOR_BRD ( WORD _wColor = -1 )
            : wColor(_wColor)
		{
			dwSize = sizeof(*this);
			nType  = NET_MSG_PET_REQ_CHANGE_COLOR_BRD;
            MIN_STATIC_ASSERT(sizeof(SNETPET_REQ_CHANGE_COLOR_BRD)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPET_REQ_CHANGE_STYLE_BRD  : public SNETPET_BROAD
	{
		WORD				    wStyle;
		WORD					wColor;

		SNETPET_REQ_CHANGE_STYLE_BRD ( WORD _wStyle = -1, WORD _wColor = 0 )
            : wStyle(_wStyle)
            , wColor(_wColor)
		{
			dwSize = sizeof(*this);
			nType  = NET_MSG_PET_REQ_CHANGE_STYLE_BRD;
            MIN_STATIC_ASSERT(sizeof(SNETPET_REQ_CHANGE_STYLE_BRD)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPET_REQ_SLOT_EX_HOLD_BRD : public SNETPET_BROAD
	{
		SITEMCUSTOM			sItemCustom;
		EMSUIT				emSuit;

		SNETPET_REQ_SLOT_EX_HOLD_BRD ( SITEMCUSTOM _sItemCustom, EMSUIT _emSuit = SUIT_NSIZE ) 
			: sItemCustom(_sItemCustom)
            , emSuit(_emSuit)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_PET_REQ_SLOT_EX_HOLD_BRD;
            MIN_STATIC_ASSERT(sizeof(SNETPET_REQ_SLOT_EX_HOLD_BRD)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPET_REQ_LEARNSKILL_BRD : public SNETPET_BROAD
	{
		SNETPET_REQ_LEARNSKILL_BRD ()
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_PET_REQ_LEARNSKILL_BRD;
            MIN_STATIC_ASSERT(sizeof(SNETPET_REQ_LEARNSKILL_BRD)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPET_REQ_FUNNY_BRD : public SNETPET_BROAD
	{
		WORD				wFunny;

		SNETPET_REQ_FUNNY_BRD ( WORD _wFunny = 1)
			: wFunny(_wFunny)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_PET_REQ_FUNNY_BRD;
            MIN_STATIC_ASSERT(sizeof(SNETPET_REQ_FUNNY_BRD)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPET_REQ_REMOVE_SLOTITEM_BRD : public SNETPET_BROAD
	{
		EMSUIT			emSuit;

		SNETPET_REQ_REMOVE_SLOTITEM_BRD ( EMSUIT _emSuit = SUIT_NSIZE )
			: emSuit(_emSuit)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_PET_REMOVE_SLOTITEM_BRD;
            MIN_STATIC_ASSERT(sizeof(SNETPET_REQ_REMOVE_SLOTITEM_BRD)<=NET_DATA_BUFSIZE);
		}
	};
    
	struct SNETPET_ACCESSORY_DELETE_BRD : public SNETPET_BROAD
	{
		ACCESSORYTYPE		accetype;

		SNETPET_ACCESSORY_DELETE_BRD ( ACCESSORYTYPE _accetype = ACCETYPESIZE )
			: accetype(_accetype)
		{
			dwSize = sizeof(*this);
			nType  = NET_MSG_PET_ACCESSORY_DELETE_BRD;
            MIN_STATIC_ASSERT(sizeof(SNETPET_ACCESSORY_DELETE_BRD)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPET_REQ_SKILLSLOT_EXPANSION_BRD : public SNETPET_BROAD
	{
		WORD wMaxSlot;

		SNETPET_REQ_SKILLSLOT_EXPANSION_BRD ( WORD _wMaxSlot = 0 )
			: wMaxSlot(_wMaxSlot)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_PET_REQ_SKILLSLOT_EXPANSION_BRD;
            MIN_STATIC_ASSERT(sizeof(SNETPET_REQ_SKILLSLOT_EXPANSION_BRD)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPET_REQ_JUMP : public NET_MSG_GENERIC
	{
		DWORD				dwGUID;
		D3DXVECTOR3			vJumpPos;

		SNETPET_REQ_JUMP ( DWORD _dwGUID, D3DXVECTOR3 _vJumpPos ) 
			: dwGUID(_dwGUID), vJumpPos( _vJumpPos )
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GCTRL_PET_JUMP_POS;
			MIN_STATIC_ASSERT(sizeof(SNETPET_REQ_JUMP)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPET_REQ_JUMP_FB : public SNETPET_BROAD
	{
		D3DXVECTOR3			vJumpPos;

		SNETPET_REQ_JUMP_FB ( D3DXVECTOR3 _vJumpPos ) 
			: vJumpPos( _vJumpPos )
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GCTRL_PET_JUMP_POS_FB;
			MIN_STATIC_ASSERT(sizeof(SNETPET_REQ_JUMP_FB)<=NET_DATA_BUFSIZE);
		}
	};


	// Revert to default structure packing
	#pragma pack()
};