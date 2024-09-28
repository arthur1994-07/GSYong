#pragma once

#include <queue>

#include "../../RanLogicClient/Crow/GLCrowRenList.h"
#include "../../RanLogicClient/Widget/GLWidgetScript.h"

namespace NSGFX_DISPLAY
{
	enum
	{
		// Ŭ�� ��ũ �̹��� ������
		CLUB_MARK_SIZE_WIDTH	= 16,
		CLUB_MARK_SIZE_HEIGHT	= 11
	};

	// ������ Ÿ�� HP Ÿ��
	enum EMSELECT_HPTYPE
	{
		EMHP_NONE = -1,

		EMHP_RED = 0,	// ����
		EMHP_BLUE,		// �Ķ�
		EMHP_GREEN,		// �ʷ�

		EMHP_SIZE
	};

	enum EMTARGET_TYPE
	{
		EMDPTARGET_NULL = 0,
		EMDPTARGET_NORMAL,
		EMDPTARGET_CTF_AUTH,
	};

	enum EMNAMEDISPLAY_STATE
	{
		// Flag
		EMNDS_NONE = 0,
		EMNDS_USED = 0x001,
		EMNDS_UPDATECLUBICON = 0x002,
		EMNDS_EMERGENCYBLINK = 0x004,	// ���� ��Ȳ�� ������ ȿ��;
		EMNDS_HPBAR_ALLY	 = 0x008,	// �� �÷��װ� ������ �ʷ� HP, �ƴϸ� ����;
	};

	// �̸� ���÷���
	struct SNameDisplay
	{
		CROWREN			sDispName;	// ���÷��� �� crow ����
		GFx::Value		valDisplay;	// ���÷��� ����Ŭ��

		D3DXVECTOR2		vPos;		// ���� ��ġ
		float			fOffsetX;	// X ������
		// EMNAMEDISPLAY_STATE
		DWORD			dwNDPState;

		SNameDisplay()
			: fOffsetX( 0.0f )
			, dwNDPState(EMNDS_NONE)
		{
			vPos = D3DXVECTOR2( 0.0f, 0.0f );
		}

		void AddState(DWORD dwState)	{ dwNDPState |= dwState; }
		void RemoveState(DWORD dwState)	{ dwNDPState &= ~dwState; }
		bool IsState(DWORD dwState) const {	return dwNDPState&dwState; }
	};

	// �̸� ���÷��� ���Ŀ�
	struct SNameDisplaySort
	{
		SNameDisplay*	pDisplay;
		WORD			wOverlaped;

		SNameDisplaySort()
			: pDisplay( NULL )
			, wOverlaped( 0 )
		{
		}

		SNameDisplaySort( SNameDisplay* p )
			: pDisplay( p )
			, wOverlaped( 0 )
		{
		}
	};

	// ���ê ���÷���
	struct SHeadChatDispaly
	{
		GFx::Value	valDisplay;		// ���÷��� ����Ŭ��
		float		fLifeTime;		// ������ �ð�

		D3DXVECTOR2	vPos;			// ���� ��ġ
		float		fOffsetY;		// Y ������

		SHeadChatDispaly()
			: fLifeTime( 0.0f )
			, fOffsetY( 0.0f )
		{
			vPos = D3DXVECTOR2( 0.0f, 0.0f );
		}
	};

	// HP ���÷���
	struct SHpDisplay
	{
		GFx::Value	valDisplay;		// ���÷��� ����Ŭ��
		// EMNAMEDISPLAY_STATE
		DWORD			dwNDPState;

		SHpDisplay()
			: dwNDPState(EMNDS_NONE)
		{
		}

		void AddState(DWORD dwState)	{ dwNDPState |= dwState; }
		void RemoveState(DWORD dwState)	{ dwNDPState &= ~dwState; }
		bool IsState(DWORD dwState) const {	return dwNDPState&dwState; }
	};

	// ������ ���÷���
	struct SDamageDisplay
	{
		GFx::Value	valDisplay;		// ���÷��� ����Ŭ��
		float		fLifeTime;		// ���� �ð�

		SDamageDisplay()
			: fLifeTime( 0.0f )
		{
		}
	};

	// ���λ��� ���÷���
	struct SPrivateMarket
	{
		GFx::Value	valDisplay;
		std::string strCharName;
		DWORD		dwId;

		D3DXVECTOR2	vPos;			// ���� ��ġ
		float		fOffsetX;
		float		fOffsetY;		// Y ������

		SPrivateMarket()
			: strCharName( "" )
			, dwId( NATIVEID_NULL().dwID )
			, fOffsetX( 0.0f )
			, fOffsetY( 0.0f )
		{
			vPos = D3DXVECTOR2( 0.0f, 0.0f );
		}
	};
}

class GfxDisplay
{
private :
	GfxDisplay();
	GfxDisplay( const GfxDisplay& value );
	~GfxDisplay();

public :
	static GfxDisplay& GetInstance();

private :
	enum EMLayer
	{
		Layer_Bottom = 0,
		Layer_Middle,
		Layer_Top,
		Layer_Item,
	};

private :
	typedef std::pair< EMCROW, DWORD >									PAIR_NAME;
	typedef std::map< PAIR_NAME, NSGFX_DISPLAY::SNameDisplay* >			MAP_NAME;
	typedef MAP_NAME::iterator											MAP_NAME_ITER;
	typedef std::queue< NSGFX_DISPLAY::SNameDisplay* >					QUE_NAME;

	typedef std::map< std::string, NSGFX_DISPLAY::SHeadChatDispaly* >	MAP_HEADCHAT;
	typedef MAP_HEADCHAT::iterator										MAP_HEADCHAT_ITER;
	typedef std::queue< NSGFX_DISPLAY::SHeadChatDispaly* >				QUE_HEADCHAT;

	typedef std::list< NSGFX_DISPLAY::SDamageDisplay* >					LIST_DMG;
	typedef LIST_DMG::iterator											LIST_DMG_ITER;
	typedef std::queue < NSGFX_DISPLAY::SDamageDisplay* >				QUE_DMG;

	typedef std::map< DWORD, NSGFX_DISPLAY::SPrivateMarket* >			MAP_PRIVATEMARKET;
	typedef MAP_PRIVATEMARKET::iterator									MAP_PRIVATEMARKET_ITER;
	typedef std::queue< NSGFX_DISPLAY::SPrivateMarket* >				QUE_PRIVATEMARKET;

private :
	// �ʱ�ȭ �Ǿ���?
	bool m_bInit;
	bool m_bOnceAddHpInit;
	// �̸� ���÷��� ���� ������Ʈ
	bool m_bForceUpdateNameDisp;
	bool m_bForceUpdated;

	// �̸� ���÷���
	MAP_NAME	m_mapName;
	QUE_NAME	m_queDisableName;

	// ������ �̸� ���÷���
	bool			m_bEnableItemNameClick;
	MAP_NAME		m_mapItemName;
	QUE_NAME		m_queDisableItemName;

	// ����� ���÷���
	MAP_HEADCHAT	m_mapHeadChat;
	QUE_HEADCHAT	m_queDisableChat;

	// ���λ��� ���÷���
	MAP_PRIVATEMARKET	m_mapPrivateMarket;
	QUE_PRIVATEMARKET	m_queDisablePM;

	// �� ĳ���� HP
	NSGFX_DISPLAY::SHpDisplay	m_MyHp;
	GLDWDATA					m_sHp;
	// ���콺 Ŀ�� �ö� Ÿ�� HP
	NSGFX_DISPLAY::SHpDisplay	m_CursorTargetHp[ NSGFX_DISPLAY::EMHP_SIZE ];
	// ������ Ÿ�� HP
	NSGFX_DISPLAY::SHpDisplay	m_SelectTargetHp[ NSGFX_DISPLAY::EMHP_SIZE ];

	// ������ ���÷���
	LIST_DMG	m_listDamage;
	QUE_DMG		m_queDisableDamage;

	NSGFX_DISPLAY::EMTARGET_TYPE		m_emDPCursorTarget;
	NSGFX_DISPLAY::EMTARGET_TYPE		m_emDPSelectTarget;

	GFx::Value m_valUpdateClubIconRT;

public :
	GFx::Value m_funcAddDisplay;
	GFx::Value m_funcChangeLayer;
	GFx::Value m_funcSetDamage;
	GFx::Value m_funcSortItemLayer;

public :
	void Init();
	void Reset();

	void ForceUpdateNameDisplay() { m_bForceUpdateNameDisp = true; }

public :
	void OnResetDevice();
	void OnFrameMove( float fElspTime );

public :
	// ���÷��� Ŭ����
	void Clear();

	// �̸� ���÷��� �ɼ� üũ
	bool CheckNameOption( const CROWREN& sDispName, const char* szOwnerName = NULL );

	// �̸� ���÷��� �߰�
	void AddName( const CROWREN& sDispName, const char* szOwnerName = NULL );
	// ������ �̸� ���÷��� �߰�
	void AddItemName( const CROWREN& sDispName );

	// ���ê ���÷��� �߰�
	void AddHeadChat( const std::string& strName, DWORD dwNameColor, 
		const std::string& strChat, DWORD dwColor, const D3DXVECTOR2& vPos );

	// HP ���÷��� �߰�
	bool AddHp();

	// ������ ���÷��� �߰�
	void AddDamage( const D3DXVECTOR3& vPos, int nDamage, DWORD dwDamageFlag, bool bAttack );

	// ���λ��� ���÷��� �߰�
	void AddPrivateMarket( const CROWREN& sDispName );
	// ���λ��� ���÷��� ����
	void DelPrivateMarket( const CROWREN& sDispName );
	void DelPrivateMarket( DWORD dwID );

	// ���λ��� ���÷��� ��� ���� ����
	void ToggleOffPrivateMarket( DWORD dwID );
	// ���λ��� �ٸ� ���÷��� ��� ���� ����
	void ToggleOffOtherPrivateMarket( DWORD dwID );

	// Ŭ�� ������ ����
	void ChangeClubIcon( DWORD dwClubID );

	// �ش� �������� ȭ�� �ȿ� �ִ��� Ȯ�� �� ȭ�� ��ġ ���
	bool IsVisible( const D3DXVECTOR3& vPos, D3DXVECTOR3& vScreen );

private :
	// ����Ŭ�� visible ����
	void SetVisibleDispInfo( GFx::Value& val, bool bVisible );

private :
	// �̸� ���÷��� visible off
	void AllNameVisibleOff();
	// ��� �ȵ� �̸� ���÷��� ����
	void RemoveNotUsedName();
	

	// �ش� ���Ͱ� ���� �������� Ȯ��
	bool IsAllowAttack( ClientActor* pActor, const STARGETID& sTargetID );

	// �̸� ���÷��� ������Ʈ
	void UpdateName( NSGFX_DISPLAY::SNameDisplay* pDisplay,
		const char* szOwnerName, const D3DXVECTOR3& vScreen );
	// �̸� ���÷��� HP ����
	void UpdateNameHP( NSGFX_DISPLAY::SNameDisplay* pDisplay );
	// ���ê ���÷��� ������Ʈ
	void UpdateHeadChat( float fElspTime );
	//// �� ĳ���� HP ���÷��� ������Ʈ
	//void UpdateMyHP();
	// ���콺 �ö� Ÿ�� HP ���÷��� ������Ʈ
	//void UpdateCursorTargetHP();
	// ������ Ÿ�� HP ���÷��� ������Ʈ
	//void UpdateSelectTargetHP();
	// ������ ���÷��� ������Ʈ
	void UpdateDamage( float fElspTime );
	// ���λ��� ���÷��� ������Ʈ
	void UpdatePrivateMarket();

public :
	bool GetItemNameDisplayName( DWORD dwID, EMCROW emCrow, CROWREN& dispName );
	void SetEnableItemClick( bool bEnable );
	bool CheckHeadChatDisplay( const char* pName );

};
