#pragma once

#include "../../EngineLib/G-Logic/GLDefine.h"

#include <vector>
#include <string>
#include <map>
#include <boost/noncopyable.hpp>
#include <boost/tr1/memory.hpp>

//----------------------------------------------------------------------

class GLCharacter;
class gltexfile;

namespace NSKeySettingScriptMan
{
	class SQKeySettingScriptMan;
};

typedef std::tr1::shared_ptr<NSKeySettingScriptMan::SQKeySettingScriptMan> sp_SqKeySettingScript;

/*----------------------------------------------------------------------
int					QuickSlot[QUICK_SLOT_NUM]		= { DIK_Q, DIK_W, DIK_E, DIK_A, DIK_S, DIK_D };

int					SkillSlot[QUICK_SKILL_NUM]		= { DIK_1, DIK_2, DIK_3, DIK_4, DIK_5, DIK_6,
DIK_7, DIK_8, DIK_9, DIK_0};

int					MenuShotcut[QUICK_MENU_NUM]		= { DIK_I, DIK_C, DIK_K, DIK_P, DIK_T, DIK_G, 
DIK_F, DIK_M, DIK_B, DIK_R, DIK_H, DIK_L,
DIK_X, DIK_Z ,DIK_BACKSLASH, DIK_J, DIK_O, DIK_SLASH };

int					DefaultShotcutKey[QUICK_ALL_NUM]	= { DIK_Q, DIK_W, DIK_E, DIK_A, DIK_S, DIK_D,
DIK_1, DIK_2, DIK_3, DIK_4, DIK_5, DIK_6,
DIK_7, DIK_8, DIK_9, DIK_0,	DIK_I, DIK_C,
DIK_K, DIK_P, DIK_T, DIK_G, DIK_F, DIK_M, 
DIK_B, DIK_R, DIK_H, DIK_L, DIK_X, DIK_Z,
DIK_BACKSLASH, DIK_J, DIK_O, DIK_SLASH };

// KeySettingWindow 참조 테이블 단축키 순서가 다르게 설정되어 있어서 필요
int					KeySettingToRanparam[QUICK_MENU_NUM] = { SHOTCUT_INVEN, SHOTCUT_CHAR, SHOTCUT_SKILL, SHOTCUT_PARTY,
SHOTCUT_QUEST, SHOTCUT_CLUB, SHOTCUT_FRIEND, SHOTCUT_MAP,
SHOTCUT_CHATMACRO, SHOTCUT_ITEMBANK, SHOTCUT_POINT_SHOP, SHOTCUT_RUN,
SHOTCUT_HELP, SHOTCUT_PET, SHOTCUT_ATTACKMODE, SHOTCUT_PKMODE, SHOTCUT_QBOX};

// BasicMenu 참조 테이블 단축키 순서가 다르게 설정되어 있어서 필요
int					BasicMenuToRanparam[BASICMENU_NUM] = { SHOTCUT_INVEN, SHOTCUT_CHAR,	SHOTCUT_SKILL, SHOTCUT_PARTY,
SHOTCUT_QUEST, SHOTCUT_CLUB, SHOTCUT_FRIEND, SHOTCUT_MAP,
SHOTCUT_ITEMBANK, SHOTCUT_RUN, SHOTCUT_POINT_SHOP, SHOTCUT_CHATMACRO,
SHOTCUT_SNS
};
----------------------------------------------------------------------*/

#define IMPLEMENT_SHORTCUTS(shortcuts, key)				\
	m_mShortcuts[shortcuts]  = SSHORTCUTS(key,false);	\
	m_mName[#shortcuts]		 = shortcuts

#define IMPLEMENT_HIDESHORTCUTS(shortcuts, key)			\
	m_mShortcuts[shortcuts]  = SSHORTCUTS(key,true);	\
	m_mName[#shortcuts]		 = shortcuts


// 컨트롤 방식 타입
enum EM_CONTROLTYPE
{
	EM_CONTROLTYPE_A = 0,		// 기존 방식;
	EM_CONTROLTYPE_B,			// 새 방식;
	EM_CONTROLTYPE_FLYCAMERA,	// 카메라 자유 이동 방식;

	EM_CONTROLTYPE_SIZE,
	EM_CONTROLTYPE_NULL = EM_CONTROLTYPE_SIZE,

	EM_CONTROLTYPE_DEFAULT = EM_CONTROLTYPE_A,
};

// 키타입
enum EMKEYTYPE
{
	EMKEYTYPE_NULL	    = 0, // 없음
	EMKEYTYPE_KEYBOARD,		 // 키보드
	EMKEYTYPE_MOUSE,		 // 마우스

	EMKEYTYPE_RESERVED_0,	 // 예약
	EMKEYTYPE_RESERVED_1,	 // 예약
	EMKEYTYPE_RESERVED_2,	 // 예약

	//! 특수용도
	EMKEYTYPE_AND,				 // AND 연산자
	EMKEYTYPE_OR,				 // OR 연산자
	EMKEYTYPE_BRACKET_CLOSE,	 // )
	EMKEYTYPE_BRACKET_OPEN,		 // (
	EMKEYTYPE_NEGATIVE_OPERATOR, // " " -> 이 묶음 안이 키 값임을 알린다 (연산자가 아님)
};

// GetKeyState 옵션
enum FLAG_GETKEYSTATE
{
	FLAG_GETKEYSTATE_NULL			= 0x0000,
	FLAG_GETKEYSTATE_BLOCK_KEYBOARD = 0x0001,
	FLAG_GETKEYSTATE_BLOCK_MOUSE	= 0x0002,

	FLAG_GETKEYSTATE_BLOCK_ALL = 
		(FLAG_GETKEYSTATE_BLOCK_KEYBOARD | FLAG_GETKEYSTATE_BLOCK_MOUSE),

	FLAG_GETKEYSTATE_ONLY_KEYBOARD = 
		(FLAG_GETKEYSTATE_BLOCK_ALL & ~FLAG_GETKEYSTATE_BLOCK_KEYBOARD),

	FLAG_GETKEYSTATE_ONLY_MOUSE = 
		(FLAG_GETKEYSTATE_BLOCK_ALL & ~FLAG_GETKEYSTATE_BLOCK_MOUSE),
};

// 단축키 타입 (UI와 관련된 단축키 타입 수정시 WidgetDef.lua 파일도 같이 수정)
enum EMSHORTCUTS
{
	EMSHORTCUTS_NULL = 0,

	EMSHORTCUTS_QUICKSLOT0,
	EMSHORTCUTS_QUICKSLOT1,
	EMSHORTCUTS_QUICKSLOT2,
	EMSHORTCUTS_QUICKSLOT3,
	EMSHORTCUTS_QUICKSLOT4,
	EMSHORTCUTS_QUICKSLOT5,

	EMSHORTCUTS_SKILLSLOT0,
	EMSHORTCUTS_SKILLSLOT1,
	EMSHORTCUTS_SKILLSLOT2,
	EMSHORTCUTS_SKILLSLOT3,
	EMSHORTCUTS_SKILLSLOT4,
	EMSHORTCUTS_SKILLSLOT5,
	EMSHORTCUTS_SKILLSLOT6,
	EMSHORTCUTS_SKILLSLOT7,
	EMSHORTCUTS_SKILLSLOT8,
	EMSHORTCUTS_SKILLSLOT9,

	EMSHORTCUTS_SKILLTAB0,
	EMSHORTCUTS_SKILLTAB1,
	EMSHORTCUTS_SKILLTAB2,
	EMSHORTCUTS_SKILLTAB3,

	EMSHORTCUTS_SKILLTRAYTOGGLE, // 퀵 스킬 슬롯 접기/펴기

	EMSHORTCUTS_ARMSSLOTSWAP, // 극강부 전용, 무기 스왑

	EMSHORTCUTS_CHATMACRO0,
	EMSHORTCUTS_CHATMACRO1,
	EMSHORTCUTS_CHATMACRO2,
	EMSHORTCUTS_CHATMACRO3,
	EMSHORTCUTS_CHATMACRO4,
	EMSHORTCUTS_CHATMACRO5,
	EMSHORTCUTS_CHATMACRO6,
	EMSHORTCUTS_CHATMACRO7,
	EMSHORTCUTS_CHATMACRO8,
	EMSHORTCUTS_CHATMACRO9,
 
	EMSHORTCUTS_INVENTORY,
	EMSHORTCUTS_CHARACTER,
	EMSHORTCUTS_SKILL,
	EMSHORTCUTS_PARTY,
	EMSHORTCUTS_QUEST,
	EMSHORTCUTS_CLUB,
	EMSHORTCUTS_FRIEND,
	EMSHORTCUTS_ITEMBANK,
	EMSHORTCUTS_MINIMAP,
	EMSHORTCUTS_POINTSHOP,
	EMSHORTCUTS_CHATMACRO,
	EMSHORTCUTS_SNS,
	EMSHORTCUTS_HELP,
	EMSHORTCUTS_QBOX,
	EMSHORTCUTS_PET,
	EMSHORTCUTS_VEHICLE,
	EMSHORTCUTS_PRODUCT,
	EMSHORTCUTS_MARKETSEARCHBUY,

	EMSHORTCUTS_SCHOOLRECORD,
	EMSHORTCUTS_BATTLEGROUND,

	EMSHORTCUTS_PICKUPITEM,
	EMSHORTCUTS_FOURCEDPKMODE,
	EMSHORTCUTS_RUNMODE,
	EMSHORTCUTS_PEACEMODE,
	EMSHORTCUTS_VEHICLEACTIVE,
	EMSHORTCUTS_VEHICLEBOOST,

//--
	EMSHORTCUTS_CAMERA_UP,				// 카메라 업
	EMSHORTCUTS_CAMERA_DOWN,			// 카메라 다운
	EMSHORTCUTS_CAMERA_LEFT,			// 카메라 레프트
	EMSHORTCUTS_CAMERA_RIGHT,			// 카메라 라이트
	EMSHORTCUTS_CAMERA_ZOOMIN,			// 카메라 줌인
	EMSHORTCUTS_CAMERA_ZOOMOUT,			// 카메라 줌아웃
	EMSHORTCUTS_CAMERA_ZOOM,			// 카메라 줌
	EMSHORTCUTS_CAMERA_DIRECTIONMODE,	// 카메라 방향 설정 모드

	EMSHORTCUTS_ACTIONCANCEL,			// 행동 취소키

	EMSHORTCUTS_ITEMDROP,		// 아이템 버리기
	EMSHORTCUTS_ITEMDROP_USE,	// 아이템 버리면서 사용(ex. 폭죽)

	EMSHORTCUTS_MOVING,			// 이동키
	EMSHORTCUTS_RUNSKILL,		// 스킬 사용키
	EMSHORTCUTS_ACTION_SELECT,	// 액션키(선택한 대상)
	EMSHORTCUTS_ACTION_CURSOR,	// 액션키(커서에 있는 대상)

	EMSHORTCUTS_DOMINATE_CONTROL,	// 지배 대상 조작; ( 펫 조작도 가능할지도? )
	EMSHORTCUTS_CONTROL,					// control-key;

	EMSHORTCUTS_TAR_P2PMENU,	// P2P메뉴
	EMSHORTCUTS_TAR_ONLYITEM,	// 아이템 클릭

	EMSHORTCUTS_TARGET_SELECT,			// 타겟 선택
	EMSHORTCUTS_TARGET_SELECT_AROUND,	// 주변 타겟 선택

	// 카메라 이동;
	EMSHORTCUTS_CAMERA_MOVE_FRONT,
	EMSHORTCUTS_CAMERA_MOVE_BACK,
	EMSHORTCUTS_CAMERA_MOVE_LEFT,
	EMSHORTCUTS_CAMERA_MOVE_RIGHT,
	EMSHORTCUTS_CAMERA_MOVE_UP,
	EMSHORTCUTS_CAMERA_MOVE_DOWN,

	EMSHORTCUTS_FLYCAMERACONTROL,

	EMSHORTCUTS_CAMERA_INCREASE_MOVESPEED,
	EMSHORTCUTS_CAMERA_DECREASE_MOVESPEED,
	EMSHORTCUTS_CAMERA_INCREASE_ROTATESPEED,
	EMSHORTCUTS_CAMERA_DECREASE_ROTATESPEED,
	EMSHORTCUTS_CAMERA_TARGET_FIXEDMODE,

	EMSHORTCUTS_CAMERA_INCREASE_INTERPOLATION_MOVETIME,
	EMSHORTCUTS_CAMERA_DECREASE_INTERPOLATION_MOVETIME,
	EMSHORTCUTS_CAMERA_CLEAR_INTERPOLATION_TIME,
	EMSHORTCUTS_CAMERA_INCREASE_INTERPOLATION_ROTATETIME,
	EMSHORTCUTS_CAMERA_DECREASE_INTERPOLATION_ROTATETIME,

	EMSHORTCUTS_CAMERA_INCREASE_INTERPOLATION_TARGETSELECTTIME,
	EMSHORTCUTS_CAMERA_DECREASE_INTERPOLATION_TARGETSELECTTIME,

	EMSHORTCUTS_CAMERA_PLAY,
	EMSHORTCUTS_CAMERA_STOP,
	EMSHORTCUTS_CAMERA_REWIND_PLAY,

	EMSHORTCUTS_CONFIRM,				// 엔터키 확인, 채팅창 포커스 등
	EMSHORTCUTS_INTERACT_DROPITEM,		// 드랍 아이템 이름 클릭으로 줍기 활성

	EMSHORTCUTS_CHANGE_CHARSLOT0,
	EMSHORTCUTS_CHANGE_CHARSLOT1,
	EMSHORTCUTS_CHANGE_CHARSLOT2,
	EMSHORTCUTS_CHANGE_CHARSLOT3,
	EMSHORTCUTS_CHANGE_CHARSLOT4,
	EMSHORTCUTS_CHANGE_CHARSLOT5,
	EMSHORTCUTS_CHANGE_CHARSLOT6,
	EMSHORTCUTS_CHANGE_CHARSLOT7,
	EMSHORTCUTS_CHANGE_CHARSLOT8,
	EMSHORTCUTS_CHANGE_CHARSLOT9,
};

//----------------------------------------------------------------------------------//
//! 단축키 데이터

typedef std::set<EMSHORTCUTS>				SET_BLOCK_SHORTCUTS;
typedef SET_BLOCK_SHORTCUTS::iterator		SET_BLOCK_SHORTCUTS_ITER;
typedef SET_BLOCK_SHORTCUTS::const_iterator	SET_BLOCK_SHORTCUTS_CITER;

struct SSHORTCUTS
{
	struct KEY
	{
		EMKEYTYPE emTYPE;
		UINT	  nVALUE;

		KEY()
			: emTYPE(EMKEYTYPE_NULL)
			, nVALUE(NULL)
		{
		}

		KEY( const EMKEYTYPE _emTYPE, const UINT _nVALUE )
			: emTYPE(_emTYPE)
			, nVALUE(_nVALUE)
		{
		}

		const bool operator == ( const KEY& rhs ) const
		{
			if ( emTYPE != rhs.emTYPE )
			{
				return false;
			}

			if ( nVALUE != rhs.nVALUE )
			{
				return false;
			}

			return true;
		}

		const bool operator < ( const KEY& rhs ) const
		{
			if ( emTYPE == rhs.emTYPE )
			{
				return nVALUE < rhs.nVALUE;
			}

			//! EMKEYTYPE 은 낮을수록 우선순위가 더 높다.
			return rhs.emTYPE < emTYPE;
		}

		SSHORTCUTS operator & ( const KEY& rhs ) const
		{
			SSHORTCUTS sShortcuts((*this), false);
			sShortcuts.AND_SynthesisKey( rhs );

			return sShortcuts;
		}

		SSHORTCUTS operator | ( const KEY& rhs ) const
		{
			SSHORTCUTS sShortcuts((*this), false);
			sShortcuts.OR_SynthesisKey( rhs );

			return sShortcuts;
		}

		const bool IsKeyType() const
		{
			switch(emTYPE)
			{
			case EMKEYTYPE_KEYBOARD:
			case EMKEYTYPE_MOUSE:
				{
					return true;
				}
			};

			return false;
		}

		const bool IsNull() const
		{
			if ( emTYPE == EMKEYTYPE_NULL && nVALUE == NULL )
			{
				return true;
			}
			
			return false;
		}
	};

	struct KEYBOARD : public KEY
	{
		KEYBOARD( const UINT _nVALUE )
			: KEY(EMKEYTYPE_KEYBOARD, _nVALUE)
		{
		}
	};

	struct MOUSE : public KEY
	{
		MOUSE( const UINT _nVALUE )
			: KEY(EMKEYTYPE_MOUSE, _nVALUE)
		{
		}
	};

	struct OPERATOR_AND : public KEY
	{
		OPERATOR_AND()
			: KEY(EMKEYTYPE_AND, NULL)
		{
		}
	};

	struct OPERATOR_OR : public KEY
	{
		OPERATOR_OR()
			: KEY(EMKEYTYPE_OR, NULL)
		{
		}
	};

	struct OPERATOR_BRACKET_OPEN : public KEY
	{
		OPERATOR_BRACKET_OPEN()
			: KEY(EMKEYTYPE_BRACKET_OPEN, NULL)
		{
		}
	};

	struct OPERATOR_BRACKET_CLOSE : public KEY
	{
		OPERATOR_BRACKET_CLOSE()
			: KEY(EMKEYTYPE_BRACKET_CLOSE, NULL)
		{
		}
	};

	struct OPERATOR_NEGATIVE_OPERATOR : public KEY
	{
		OPERATOR_NEGATIVE_OPERATOR()
			: KEY(EMKEYTYPE_NEGATIVE_OPERATOR, NULL)
		{
		}
	};

public:
	typedef std::set<KEY>						SET_BLOCK_KEYS;
	typedef SET_BLOCK_KEYS::iterator			SET_BLOCK_KEYS_ITER;
	typedef SET_BLOCK_KEYS::const_iterator		SET_BLOCK_KEYS_CITER;

	typedef std::vector<KEY>					VEC_KEY;
	typedef VEC_KEY::iterator					VEC_KEY_ITER;
	typedef VEC_KEY::const_iterator				VEC_KEY_CITER;
	typedef VEC_KEY::reverse_iterator			VEC_KEY_RITER;

	SSHORTCUTS()
		: bHide(false)
		, nMinimumKeyNum(0)
	{
	}

	SSHORTCUTS( const KEY& sKey, const bool _bHide )
		: bHide(_bHide)
		, nMinimumKeyNum(0)
	{
		PushKey(sKey);
		BuildString();
	}

	explicit SSHORTCUTS( const SSHORTCUTS& sShortCuts, const bool _bHide )
		: bHide(false)
		, nMinimumKeyNum(0)
	{
		(*this) = sShortCuts;
		bHide	= _bHide;
	}

	explicit SSHORTCUTS( const std::string& strFormulaKey, const bool _bHide )
		: bHide(_bHide)
		, nMinimumKeyNum(0)
	{
		ParseFormula(strFormulaKey);
	}

	SSHORTCUTS& operator & ( const KEY& rhs )
	{
		AND_SynthesisKey(rhs);
		return (*this);
	}

	SSHORTCUTS& operator | ( const KEY& rhs )
	{
		OR_SynthesisKey(rhs);
		return (*this);
	}

	SSHORTCUTS& operator & ( const SSHORTCUTS& rhs )
	{
		AND_SynthesisShortcuts(rhs);
		return (*this);
	}

	SSHORTCUTS& operator | ( const SSHORTCUTS& rhs )
	{
		OR_SynthesisShortcuts(rhs);
		return (*this);
	}

private:
	//! 수식을 키값으로 파싱 ( ex. Ctrl+X )
	void ParseFormula( const std::string& strKey );
	//! 중위표기를 후위표기법으로 만든다 ( ex. Ctrl+X => Ctrl X + )
	std::string InfixToPostfixNotation( IN const std::string& strInfixKey );

private:
	void PushKey( const SSHORTCUTS::KEY& sKey );
	void BuildString();

public:
	void AND_SynthesisKey( const SSHORTCUTS::KEY& sKey );
	void OR_SynthesisKey( const SSHORTCUTS::KEY& sKey );
	void AND_SynthesisShortcuts( const SSHORTCUTS& rhs );
	void OR_SynthesisShortcuts( const SSHORTCUTS& rhs );

public:
	const bool IsHide() const
	{
		return bHide;
	}

	const bool IsNull() const
	{
		if ( vecKey.empty() )
		{
			return true;
		}

		VEC_KEY_CITER iter = vecKey.begin();
		for ( ; iter!=vecKey.end(); ++iter )
		{
			const KEY& sKey = (*iter);

			if ( sKey.emTYPE == EMKEYTYPE_NULL )
			{
				return true;
			}
		}

		return false;
	}

public:
	const UINT  GetMinimumKeyNum () const { return nMinimumKeyNum; }

public:
	const DWORD GetKeyState( DWORD dwOption=FLAG_GETKEYSTATE_NULL, const SET_BLOCK_KEYS* pBlockKeys=NULL ) const;
	const int   GetKeyAmount( const SET_BLOCK_KEYS* pBlockKeys=NULL ) const;

public:
	const std::string& GetString() const { return strKey; }
	const UINT GetSize() const { return static_cast<UINT>(vecKey.size()); }

public:
	VEC_KEY_CITER IterBegin() const { return vecKey.begin(); }
	VEC_KEY_CITER IterEnd()   const { return vecKey.end(); }

private:
	const DWORD GetKeyStateAND ( DWORD dwState1, DWORD dwState2 ) const;
	const DWORD GetKeyStateOR  ( DWORD dwState1, DWORD dwState2 ) const;

private:
	UINT nMinimumKeyNum; // 최소 사용 키 갯수
	bool bHide;

private:
	VEC_KEY		vecKey;
	std::string strKey;

};

//----------------------------------------------------------------------------------//
//! 문자열 <--> 키값 사이의 변환 바인딩

struct SKEYSTRINGBINDER : public boost::noncopyable
{
public:
	typedef std::map<SSHORTCUTS::KEY, std::string> MAP_KEYTOSTRING;
	typedef MAP_KEYTOSTRING::iterator			   MAP_KEYTOSTRING_ITER;

	typedef std::map<std::string, SSHORTCUTS::KEY> MAP_STRINGTOKEY;
	typedef MAP_STRINGTOKEY::iterator			   MAP_STRINGTOKEY_ITER;

private:
	SKEYSTRINGBINDER();

private:
	MAP_KEYTOSTRING KeyToString;
	MAP_STRINGTOKEY StringToKey;

public:
	const std::string& GetNullKey()
	{
		static std::string strNull = "(None)";
		return strNull;
	}

	const std::string& GetString( const SSHORTCUTS::KEY& sKey )
	{
		MAP_KEYTOSTRING_ITER finditer = KeyToString.find(sKey);
		if ( finditer == KeyToString.end() )
		{
			return GetNullKey();
		}

		return finditer->second;
	}

	const SSHORTCUTS::KEY& GetKey( const std::string& strKey )
	{
		MAP_STRINGTOKEY_ITER finditer = StringToKey.find(strKey);
		if ( finditer == StringToKey.end() )
		{
			static const SSHORTCUTS::KEY sNotFound(EMKEYTYPE_NULL,NULL);
			return sNotFound;
		}

		return finditer->second;
	}

public:
	static SKEYSTRINGBINDER& GetInstance();

};

//----------------------------------------------------------------------------------//
//! 단축키 세팅

class CKeySetting
{
public:
	enum { VERSION = 0x001, };

public:
	typedef std::map<EMSHORTCUTS,SSHORTCUTS>			MAP_KEYBINDING_SHORTCUTS;
	typedef MAP_KEYBINDING_SHORTCUTS::iterator			MAP_KEYBINDING_SHORTCUTS_ITER;
	typedef MAP_KEYBINDING_SHORTCUTS::const_iterator	MAP_KEYBINDING_SHORTCUTS_CITER;

	typedef std::map<std::string, EMSHORTCUTS>			MAP_KEYBINDING_NAME;
	typedef MAP_KEYBINDING_NAME::iterator				MAP_KEYBINDING_NAME_ITER;
	typedef MAP_KEYBINDING_NAME::const_iterator			MAP_KEYBINDING_NAME_CITER;

public:
	CKeySetting();
	~CKeySetting();

public:
	const bool LoadSquirrel( const std::string& strFilePath );

	const bool Load( gltexfile& cFile );
	const bool Save( FILE* pFile );
    const bool Load120( gltexfile& cFile );

public:
	const EMSHORTCUTS GetShortCutsType( const std::string& strName ) const
	{
		MAP_KEYBINDING_NAME_CITER finditer = m_mName.find( strName );
		if ( finditer == m_mName.end() )
		{
			return EMSHORTCUTS_NULL;
		}

		return finditer->second;
	}

	const SSHORTCUTS& GetShortcuts( const EMSHORTCUTS emShortcuts ) const
	{
		MAP_KEYBINDING_SHORTCUTS_CITER finditer = m_mShortcuts.find( emShortcuts );
		if ( finditer == m_mShortcuts.end() )
		{
			static SSHORTCUTS sNull( SSHORTCUTS::KEY(EMKEYTYPE_NULL,NULL), false );
			return sNull;
		}

		return finditer->second;
	}

	const SSHORTCUTS& GetShortcuts( const std::string& strName ) const
	{
		EMSHORTCUTS emShortCuts = GetShortCutsType( strName );

		if ( emShortCuts == EMSHORTCUTS_NULL )
		{
			static SSHORTCUTS sNull( SSHORTCUTS::KEY(EMKEYTYPE_NULL,NULL), false );
			return sNull;
		}

		return GetShortcuts(emShortCuts);
	}

	void SetShortcuts( const EMSHORTCUTS emShortcuts, const SSHORTCUTS& sShortcuts )
	{
		MAP_KEYBINDING_SHORTCUTS_CITER finditer = m_mShortcuts.find( emShortcuts );

		if ( finditer != m_mShortcuts.end() )
		{
			if ( finditer->second.IsHide() != sShortcuts.IsHide() )
			{
				return;
			}
		}

		m_mShortcuts[emShortcuts] = sShortcuts;
	}

	void SetCameraMoveShortcuts();

public:
	const std::string& GetString( const EMSHORTCUTS emShortcuts ) const
	{
		MAP_KEYBINDING_SHORTCUTS_CITER finditer = m_mShortcuts.find( emShortcuts );
		if ( finditer == m_mShortcuts.end() )
		{
			return SKEYSTRINGBINDER::GetInstance().GetNullKey();
		}

		return (finditer->second).GetString();
	}

public:
	void BlockKeyState( const EMSHORTCUTS emShortcuts );
	void ResetBlockKeyState( const EMSHORTCUTS emShortcuts );
	const bool IsBlockKeyState ( const EMSHORTCUTS emShortcuts ) const;

	const DWORD GetKeyState ( const EMSHORTCUTS emShortcuts, DWORD dwOption=FLAG_GETKEYSTATE_NULL ) const;
	const int   GetKeyAmount( const EMSHORTCUTS emShortcuts ) const;
	void GetMouseMove( OUT int& dx, OUT int& dy, OUT int& dz );

public:
	const STARGETID DoLogic_SelectAround( GLCharacter* pCharacter );

private:
	const STARGETID DoLogic_SelectAround_Squirrel( GLCharacter* pCharacter );

public:
	MAP_KEYBINDING_SHORTCUTS_ITER BeginIterShortcuts()	{ return m_mShortcuts.begin(); }
	MAP_KEYBINDING_SHORTCUTS_ITER EndIterShortcuts()	{ return m_mShortcuts.end();   }

public:
	const UINT GetNumManagedSelectTarget() const;

private:
	SSHORTCUTS::SET_BLOCK_KEYS	m_setBlockKeys;
	SET_BLOCK_SHORTCUTS			m_setBlockShortcuts;
	MAP_KEYBINDING_SHORTCUTS	m_mShortcuts;
	MAP_KEYBINDING_NAME			m_mName;

private:
	sp_SqKeySettingScript	 m_spSqScript;

};
