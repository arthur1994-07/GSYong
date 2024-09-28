#pragma once

#include <queue>

#include "../../RanLogicClient/Crow/GLCrowRenList.h"
#include "../../RanLogicClient/Widget/GLWidgetScript.h"

namespace NSGFX_DISPLAY
{
	enum
	{
		// 클럽 마크 이미지 사이즈
		CLUB_MARK_SIZE_WIDTH	= 16,
		CLUB_MARK_SIZE_HEIGHT	= 11
	};

	// 선택한 타겟 HP 타입
	enum EMSELECT_HPTYPE
	{
		EMHP_NONE = -1,

		EMHP_RED = 0,	// 빨강
		EMHP_BLUE,		// 파랑
		EMHP_GREEN,		// 초록

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
		EMNDS_EMERGENCYBLINK = 0x004,	// 위급 상황시 깜빡임 효과;
		EMNDS_HPBAR_ALLY	 = 0x008,	// 이 플래그가 있으면 초록 HP, 아니면 빨간;
	};

	// 이름 디스플레이
	struct SNameDisplay
	{
		CROWREN			sDispName;	// 디스플레이 될 crow 정보
		GFx::Value		valDisplay;	// 디스플레이 무비클립

		D3DXVECTOR2		vPos;		// 이전 위치
		float			fOffsetX;	// X 오프셋
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

	// 이름 디스플레이 정렬용
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

	// 헤드챗 디스플레이
	struct SHeadChatDispaly
	{
		GFx::Value	valDisplay;		// 디스플레이 무비클립
		float		fLifeTime;		// 보여질 시간

		D3DXVECTOR2	vPos;			// 이전 위치
		float		fOffsetY;		// Y 오프셋

		SHeadChatDispaly()
			: fLifeTime( 0.0f )
			, fOffsetY( 0.0f )
		{
			vPos = D3DXVECTOR2( 0.0f, 0.0f );
		}
	};

	// HP 디스플레이
	struct SHpDisplay
	{
		GFx::Value	valDisplay;		// 디스플레이 무비클립
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

	// 데미지 디스플레이
	struct SDamageDisplay
	{
		GFx::Value	valDisplay;		// 디스플레이 무비클립
		float		fLifeTime;		// 리셋 시간

		SDamageDisplay()
			: fLifeTime( 0.0f )
		{
		}
	};

	// 개인상점 디스플레이
	struct SPrivateMarket
	{
		GFx::Value	valDisplay;
		std::string strCharName;
		DWORD		dwId;

		D3DXVECTOR2	vPos;			// 이전 위치
		float		fOffsetX;
		float		fOffsetY;		// Y 오프셋

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
	// 초기화 되었나?
	bool m_bInit;
	bool m_bOnceAddHpInit;
	// 이름 디스플레이 강제 업데이트
	bool m_bForceUpdateNameDisp;
	bool m_bForceUpdated;

	// 이름 디스플레이
	MAP_NAME	m_mapName;
	QUE_NAME	m_queDisableName;

	// 아이템 이름 디스플레이
	bool			m_bEnableItemNameClick;
	MAP_NAME		m_mapItemName;
	QUE_NAME		m_queDisableItemName;

	// 헤드쳇 디스플레이
	MAP_HEADCHAT	m_mapHeadChat;
	QUE_HEADCHAT	m_queDisableChat;

	// 개인상점 디스플레이
	MAP_PRIVATEMARKET	m_mapPrivateMarket;
	QUE_PRIVATEMARKET	m_queDisablePM;

	// 내 캐릭터 HP
	NSGFX_DISPLAY::SHpDisplay	m_MyHp;
	GLDWDATA					m_sHp;
	// 마우스 커서 올라간 타겟 HP
	NSGFX_DISPLAY::SHpDisplay	m_CursorTargetHp[ NSGFX_DISPLAY::EMHP_SIZE ];
	// 선택한 타겟 HP
	NSGFX_DISPLAY::SHpDisplay	m_SelectTargetHp[ NSGFX_DISPLAY::EMHP_SIZE ];

	// 데미지 디스플레이
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
	// 디스플레이 클리어
	void Clear();

	// 이름 디스플레이 옵션 체크
	bool CheckNameOption( const CROWREN& sDispName, const char* szOwnerName = NULL );

	// 이름 디스플레이 추가
	void AddName( const CROWREN& sDispName, const char* szOwnerName = NULL );
	// 아이템 이름 디스플레이 추가
	void AddItemName( const CROWREN& sDispName );

	// 헤드챗 디스플레이 추가
	void AddHeadChat( const std::string& strName, DWORD dwNameColor, 
		const std::string& strChat, DWORD dwColor, const D3DXVECTOR2& vPos );

	// HP 디스플레이 추가
	bool AddHp();

	// 데미지 디스플레이 추가
	void AddDamage( const D3DXVECTOR3& vPos, int nDamage, DWORD dwDamageFlag, bool bAttack );

	// 개인상점 디스플레이 추가
	void AddPrivateMarket( const CROWREN& sDispName );
	// 개인상점 디스플레이 삭제
	void DelPrivateMarket( const CROWREN& sDispName );
	void DelPrivateMarket( DWORD dwID );

	// 개인상점 디스플레이 토글 상태 끄기
	void ToggleOffPrivateMarket( DWORD dwID );
	// 개인상점 다른 디스플레이 토글 상태 끄기
	void ToggleOffOtherPrivateMarket( DWORD dwID );

	// 클럽 아이콘 변경
	void ChangeClubIcon( DWORD dwClubID );

	// 해당 포지션이 화면 안에 있는지 확인 및 화면 위치 계산
	bool IsVisible( const D3DXVECTOR3& vPos, D3DXVECTOR3& vScreen );

private :
	// 무비클립 visible 설정
	void SetVisibleDispInfo( GFx::Value& val, bool bVisible );

private :
	// 이름 디스플레이 visible off
	void AllNameVisibleOff();
	// 사용 안된 이름 디스플레이 제거
	void RemoveNotUsedName();
	

	// 해당 액터가 공격 가능한지 확인
	bool IsAllowAttack( ClientActor* pActor, const STARGETID& sTargetID );

	// 이름 디스플레이 업데이트
	void UpdateName( NSGFX_DISPLAY::SNameDisplay* pDisplay,
		const char* szOwnerName, const D3DXVECTOR3& vScreen );
	// 이름 디스플레이 HP 설정
	void UpdateNameHP( NSGFX_DISPLAY::SNameDisplay* pDisplay );
	// 헤드챗 디스플레이 업데이트
	void UpdateHeadChat( float fElspTime );
	//// 내 캐릭터 HP 디스플레이 업데이트
	//void UpdateMyHP();
	// 마우스 올라간 타겟 HP 디스플레이 업데이트
	//void UpdateCursorTargetHP();
	// 선택한 타겟 HP 디스플레이 업데이트
	//void UpdateSelectTargetHP();
	// 데미지 디스플레이 업데이트
	void UpdateDamage( float fElspTime );
	// 개인상점 디스플레이 업데이트
	void UpdatePrivateMarket();

public :
	bool GetItemNameDisplayName( DWORD dwID, EMCROW emCrow, CROWREN& dispName );
	void SetEnableItemClick( bool bEnable );
	bool CheckHeadChatDisplay( const char* pName );

};
