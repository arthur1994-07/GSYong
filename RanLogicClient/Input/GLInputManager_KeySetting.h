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

// KeySettingWindow ���� ���̺� ����Ű ������ �ٸ��� �����Ǿ� �־ �ʿ�
int					KeySettingToRanparam[QUICK_MENU_NUM] = { SHOTCUT_INVEN, SHOTCUT_CHAR, SHOTCUT_SKILL, SHOTCUT_PARTY,
SHOTCUT_QUEST, SHOTCUT_CLUB, SHOTCUT_FRIEND, SHOTCUT_MAP,
SHOTCUT_CHATMACRO, SHOTCUT_ITEMBANK, SHOTCUT_POINT_SHOP, SHOTCUT_RUN,
SHOTCUT_HELP, SHOTCUT_PET, SHOTCUT_ATTACKMODE, SHOTCUT_PKMODE, SHOTCUT_QBOX};

// BasicMenu ���� ���̺� ����Ű ������ �ٸ��� �����Ǿ� �־ �ʿ�
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


// ��Ʈ�� ��� Ÿ��
enum EM_CONTROLTYPE
{
	EM_CONTROLTYPE_A = 0,		// ���� ���;
	EM_CONTROLTYPE_B,			// �� ���;
	EM_CONTROLTYPE_FLYCAMERA,	// ī�޶� ���� �̵� ���;

	EM_CONTROLTYPE_SIZE,
	EM_CONTROLTYPE_NULL = EM_CONTROLTYPE_SIZE,

	EM_CONTROLTYPE_DEFAULT = EM_CONTROLTYPE_A,
};

// ŰŸ��
enum EMKEYTYPE
{
	EMKEYTYPE_NULL	    = 0, // ����
	EMKEYTYPE_KEYBOARD,		 // Ű����
	EMKEYTYPE_MOUSE,		 // ���콺

	EMKEYTYPE_RESERVED_0,	 // ����
	EMKEYTYPE_RESERVED_1,	 // ����
	EMKEYTYPE_RESERVED_2,	 // ����

	//! Ư���뵵
	EMKEYTYPE_AND,				 // AND ������
	EMKEYTYPE_OR,				 // OR ������
	EMKEYTYPE_BRACKET_CLOSE,	 // )
	EMKEYTYPE_BRACKET_OPEN,		 // (
	EMKEYTYPE_NEGATIVE_OPERATOR, // " " -> �� ���� ���� Ű ������ �˸��� (�����ڰ� �ƴ�)
};

// GetKeyState �ɼ�
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

// ����Ű Ÿ�� (UI�� ���õ� ����Ű Ÿ�� ������ WidgetDef.lua ���ϵ� ���� ����)
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

	EMSHORTCUTS_SKILLTRAYTOGGLE, // �� ��ų ���� ����/���

	EMSHORTCUTS_ARMSSLOTSWAP, // �ذ��� ����, ���� ����

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
	EMSHORTCUTS_CAMERA_UP,				// ī�޶� ��
	EMSHORTCUTS_CAMERA_DOWN,			// ī�޶� �ٿ�
	EMSHORTCUTS_CAMERA_LEFT,			// ī�޶� ����Ʈ
	EMSHORTCUTS_CAMERA_RIGHT,			// ī�޶� ����Ʈ
	EMSHORTCUTS_CAMERA_ZOOMIN,			// ī�޶� ����
	EMSHORTCUTS_CAMERA_ZOOMOUT,			// ī�޶� �ܾƿ�
	EMSHORTCUTS_CAMERA_ZOOM,			// ī�޶� ��
	EMSHORTCUTS_CAMERA_DIRECTIONMODE,	// ī�޶� ���� ���� ���

	EMSHORTCUTS_ACTIONCANCEL,			// �ൿ ���Ű

	EMSHORTCUTS_ITEMDROP,		// ������ ������
	EMSHORTCUTS_ITEMDROP_USE,	// ������ �����鼭 ���(ex. ����)

	EMSHORTCUTS_MOVING,			// �̵�Ű
	EMSHORTCUTS_RUNSKILL,		// ��ų ���Ű
	EMSHORTCUTS_ACTION_SELECT,	// �׼�Ű(������ ���)
	EMSHORTCUTS_ACTION_CURSOR,	// �׼�Ű(Ŀ���� �ִ� ���)

	EMSHORTCUTS_DOMINATE_CONTROL,	// ���� ��� ����; ( �� ���۵� ����������? )
	EMSHORTCUTS_CONTROL,					// control-key;

	EMSHORTCUTS_TAR_P2PMENU,	// P2P�޴�
	EMSHORTCUTS_TAR_ONLYITEM,	// ������ Ŭ��

	EMSHORTCUTS_TARGET_SELECT,			// Ÿ�� ����
	EMSHORTCUTS_TARGET_SELECT_AROUND,	// �ֺ� Ÿ�� ����

	// ī�޶� �̵�;
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

	EMSHORTCUTS_CONFIRM,				// ����Ű Ȯ��, ä��â ��Ŀ�� ��
	EMSHORTCUTS_INTERACT_DROPITEM,		// ��� ������ �̸� Ŭ������ �ݱ� Ȱ��

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
//! ����Ű ������

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

			//! EMKEYTYPE �� �������� �켱������ �� ����.
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
	//! ������ Ű������ �Ľ� ( ex. Ctrl+X )
	void ParseFormula( const std::string& strKey );
	//! ����ǥ�⸦ ����ǥ������� ����� ( ex. Ctrl+X => Ctrl X + )
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
	UINT nMinimumKeyNum; // �ּ� ��� Ű ����
	bool bHide;

private:
	VEC_KEY		vecKey;
	std::string strKey;

};

//----------------------------------------------------------------------------------//
//! ���ڿ� <--> Ű�� ������ ��ȯ ���ε�

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
//! ����Ű ����

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
