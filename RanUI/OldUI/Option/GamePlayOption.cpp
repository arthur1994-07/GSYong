#include "StdAfx.h"
#include "GamePlayOption.h"
#include "../../../EngineLib/GUInterface/BasicButton.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/BasicComboBox.h"
#include "../../../EngineLib/GUInterface/BasicComboBoxRollOver.h"

#include "../../../RanLogicClient/Tool/DxParamSet.h"
#include "../../../RanLogic/RANPARAM.h"
#include "../../../RanLogic/GLUseFeatures.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CBasicGamePlayOption::CBasicGamePlayOption ( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
    : CUIGroup(pEngineDevice)
	, m_pInterface(pInterface)
    , m_pFont9 ( NULL )
    , m_pConftButton ( NULL )
    , m_pTradeButton ( NULL )
    , m_pPartyButton ( NULL )
    , m_pSimpleHPButton ( NULL )
    , m_pFriendButton ( NULL )
    , m_pMovableOnChatButton ( NULL )
    , m_pShowTipButton ( NULL )
    , m_pForcedAttackButton ( NULL )
    , m_pNameDisplayButton ( NULL )
    , m_pSimpleSummonNameButton ( NULL )
    , m_pAutoCrowHPBarButton ( NULL )
	, m_pVehicleButton( NULL )
	, m_pCharInfoPrivateButton( NULL )
	, m_pCharInfoAdditionButton( NULL )
	, m_pGroupChatInvateButton ( NULL )
	, m_pAutoRebirthButton( NULL )
    , m_pComboBoxSimpleOpen ( NULL )
    , m_pComboBoxSimpleRollOver ( NULL )
    , m_pComboSkillTypeOpen( NULL )
    , m_pComboSkillTypeRollOver( NULL )
    , m_dwAroundDetail( 0 )
    , m_dwSkillTypeUI( 0 )
    , m_bConft( FALSE )
    , m_bTrade( FALSE )
    , m_bParty( FALSE )
    , m_bSimpleHP( FALSE )
    , m_bFriend( FALSE )
    , m_bVehicle( FALSE )
    , m_bMovableOnChat( FALSE )
    , m_bShowTip( FALSE )
    , m_bForcedAttack( FALSE )
    , m_bNameDisplay( FALSE )
    , m_bSimpleSummonName( FALSE )
    , m_bAutoCrowHPBar( FALSE )
    , m_RollOverID( NO_ID )
	, m_bCharInfoPrivate( FALSE )
	, m_bCharInfoAddition( FALSE )
	, m_bGroupChatInvite( FALSE )
{
}

CBasicGamePlayOption::~CBasicGamePlayOption ()
{
}

void CBasicGamePlayOption::CreateSubControl ()
{
	m_pFont9 = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	CBasicTextBox* pTextBox = NULL;

	{	//	기본 텍스트		
		DWORD dwFontColor = NS_UITEXTCOLOR::DEFAULT;
		int nAlign = TEXT_ALIGN_LEFT;

		//	자동 대련 거부
		pTextBox = CreateStaticControl ( "HWOPTION_GAMEPLAY_CONFT_STATIC", m_pFont9, dwFontColor, nAlign );
		pTextBox->SetOneLineText ( ID2GAMEWORD ( "HWOPTION_GAMEPLAY_OPTION", 0 ) );

		//	자동 거래 거부
		pTextBox = CreateStaticControl ( "HWOPTION_GAMEPLAY_TRADE_STATIC", m_pFont9, dwFontColor, nAlign );
		pTextBox->SetOneLineText ( ID2GAMEWORD ( "HWOPTION_GAMEPLAY_OPTION", 1 ) );

		//	자동 파티 거부
		pTextBox = CreateStaticControl ( "HWOPTION_GAMEPLAY_PARTY_STATIC", m_pFont9, dwFontColor, nAlign );
		pTextBox->SetOneLineText ( ID2GAMEWORD ( "HWOPTION_GAMEPLAY_OPTION", 2 ) );

		//	미니 HP 표시
		pTextBox = CreateStaticControl ( "HWOPTION_GAMEPLAY_SIMPLEHP_STATIC", m_pFont9, dwFontColor, nAlign );
		pTextBox->SetOneLineText ( ID2GAMEWORD ( "HWOPTION_GAMEPLAY_OPTION", 3 ) );

		//	자동 친구 거부
		pTextBox = CreateStaticControl ( "HWOPTION_GAMEPLAY_FRIEND_STATIC", m_pFont9, dwFontColor, nAlign );
		pTextBox->SetOneLineText ( ID2GAMEWORD ( "HWOPTION_GAMEPLAY_OPTION", 4 ) );

		//	게임창 기본 위치로
		pTextBox = CreateStaticControl ( "HWOPTION_GAMEPLAY_DEFAULTPOS_STATIC", m_pFont9, dwFontColor, nAlign );
		pTextBox->SetOneLineText ( ID2GAMEWORD ( "HWOPTION_GAMEPLAY_OPTION", 5 ) );

		//  채팅창 뚫기
		pTextBox = CreateStaticControl ( "HWOPTION_GAMEPLAY_MOVABLE_ON_CHAT_STATIC", m_pFont9, dwFontColor, nAlign );
		pTextBox->SetOneLineText ( ID2GAMEWORD ( "HWOPTION_GAMEPLAY_OPTION", 6 ) );

		//  팁 보기
		pTextBox = CreateStaticControl ( "HWOPTION_GAMEPLAY_SHOW_TIP_STATIC", m_pFont9, dwFontColor, nAlign );
		pTextBox->SetOneLineText ( ID2GAMEWORD ( "HWOPTION_GAMEPLAY_OPTION", 7 ) );

		//  강제 공격
		pTextBox = CreateStaticControl ( "HWOPTION_GAMEPLAY_FORCED_ATTACK_STATIC", m_pFont9, dwFontColor, nAlign );
		pTextBox->SetOneLineText ( ID2GAMEWORD ( "HWOPTION_GAMEPLAY_OPTION", 8 ) );

		//  이름 항상 표시
		pTextBox = CreateStaticControl ( "HWOPTION_GAMEPLAY_NAME_DISPLAY_STATIC", m_pFont9, dwFontColor, nAlign );
		pTextBox->SetOneLineText ( ID2GAMEWORD ( "HWOPTION_GAMEPLAY_OPTION", 9 ) );

		//  주변 캐릭터 단순화
		pTextBox = CreateStaticControl ( "HWOPTION_GAMEPLAY_SIMPLE_STATIC", m_pFont9, dwFontColor, nAlign );
		pTextBox->SetOneLineText ( ID2GAMEWORD ( "HWOPTION_GAMEPLAY_OPTION", 11 ) );

		// 소환수 이름 단순화
		pTextBox = CreateStaticControl ( "HWOPTION_GAMEPLAY_SIMPLE_SUMMON_NAME_STATIC", m_pFont9, dwFontColor, nAlign );
		pTextBox->SetOneLineText ( ID2GAMEWORD ( "HWOPTION_GAMEPLAY_OPTION", 12 ) );

		pTextBox = CreateStaticControl ( "HWOPTION_GAMEPLAY_SKILL_TYPE_STATIC", m_pFont9, dwFontColor, nAlign );
		pTextBox->SetOneLineText ( ID2GAMEWORD ( "HWOPTION_GAMEPLAY_OPTION", 13 ) );

		pTextBox = CreateStaticControl ( "HWOPTION_GAMEPLAY_VEHICLE_STATIC", m_pFont9, dwFontColor, nAlign );
		pTextBox->SetOneLineText ( ID2GAMEWORD ( "HWOPTION_GAMEPLAY_OPTION", 14 ) );

        // 몬스터 HP바 자동표시
        pTextBox = CreateStaticControl ( "HWOPTION_GAMEPLAY_AUTO_CROW_HPBAR_STATIC", m_pFont9, dwFontColor, nAlign );
        pTextBox->SetOneLineText ( ID2GAMEWORD ( "HWOPTION_GAMEPLAY_OPTION", 15 ) );

		// 내 정보 비공개
		if( GLUseFeatures::GetInstance().IsUsingReferChar() )
		{
			pTextBox = CreateStaticControl( "HWOPTION_GAMEPLAY_CHAR_INFO_PRIVATE", m_pFont9, dwFontColor, nAlign );
			pTextBox->SetOneLineText ( ID2GAMEWORD( "HWOPTION_GAMEPLAY_OPTION", 16 ) );
		}

		// 추가 정보 보기
		pTextBox = CreateStaticControl( "HWOPTION_GAMEPLAY_CHAR_INFO_ADDITIONAL", m_pFont9, dwFontColor, nAlign );
		pTextBox->SetOneLineText ( ID2GAMEWORD( "HWOPTION_GAMEPLAY_OPTION", 18 ) );

		// 자동 그룹채팅 거부
		if( GLUseFeatures::GetInstance().IsUsingGroupChat() )
		{
			pTextBox = CreateStaticControl( "HWOPTION_GAMEPLAY_GROUP_CHAT_STATIC", m_pFont9, dwFontColor, nAlign );
			pTextBox->SetOneLineText ( ID2GAMEWORD( "HWOPTION_GAMEPLAY_OPTION", 17 ) );
		}

		// 자동 부활;
		pTextBox = CreateStaticControl( "HWOPTION_GAMEPLAY_AUTO_REBIRTH", m_pFont9, dwFontColor, nAlign );
		pTextBox->SetOneLineText ( ID2GAMEINTEXT( "REBIRTH_WINDOW_AUTO_REBIRTH") );
	}

	{
		CBasicComboBox* pComboBox = NULL;
		pComboBox = new CBasicComboBox(m_pEngineDevice);
		pComboBox->CreateSub ( this, "BASIC_COMBOBOX", UI_FLAG_XSIZE, HWOPTION_GAMEPLAY_SIMPLE_COMBO_OPEN );
		pComboBox->CreateBaseComboBox ( "HWOPTION_GAMEPLAY_SIMPLE_COMBO_OPEN" );			
		RegisterControl ( pComboBox );
		m_pComboBoxSimpleOpen = pComboBox;
	}

	{
		CBasicComboBox* pComboBox = NULL;
		pComboBox = new CBasicComboBox(m_pEngineDevice);
		pComboBox->CreateSub ( this, "BASIC_COMBOBOX", UI_FLAG_XSIZE, HWOPTION_GAMEPLAY_SKILLTYPE_COMBO_OPEN );
		pComboBox->CreateBaseComboBox ( "HWOPTION_GAMEPLAY_SKILLTYPE_COMBO_OPEN" );			
		RegisterControl ( pComboBox );
		m_pComboSkillTypeOpen = pComboBox;
	}

	{	//	기능
		//	버튼
		m_pConftButton = CreateFlipButton( "HWOPTION_GAMEPLAY_CONFT_BUTTON", "HWOPTION_GAMEPLAY_CONFT_BUTTON_F", HWOPTION_GAMEPLAY_CONFT_BUTTON );
		m_pTradeButton = CreateFlipButton( "HWOPTION_GAMEPLAY_TRADE_BUTTON", "HWOPTION_GAMEPLAY_TRADE_BUTTON_F", HWOPTION_GAMEPLAY_TRADE_BUTTON );
		m_pPartyButton = CreateFlipButton( "HWOPTION_GAMEPLAY_PARTY_BUTTON", "HWOPTION_GAMEPLAY_PARTY_BUTTON_F", HWOPTION_GAMEPLAY_PARTY_BUTTON );
		m_pSimpleHPButton = CreateFlipButton( "HWOPTION_GAMEPLAY_SIMPLEHP_BUTTON", "HWOPTION_GAMEPLAY_SIMPLEHP_BUTTON_F", HWOPTION_GAMEPLAY_SIMPLEHP_BUTTON );
		m_pFriendButton = CreateFlipButton( "HWOPTION_GAMEPLAY_FRIEND_BUTTON", "HWOPTION_GAMEPLAY_FRIEND_BUTTON_F", HWOPTION_GAMEPLAY_FRIEND_BUTTON );
		m_pMovableOnChatButton = CreateFlipButton( "HWOPTION_GAMEPLAY_MOVABLE_ON_CHAT_BUTTON", "HWOPTION_GAMEPLAY_MOVABLE_ON_CHAT_BUTTON_F", HWOPTION_GAMEPLAY_MOVABLE_ON_CHAT_BUTTON );
		m_pShowTipButton = CreateFlipButton( "HWOPTION_GAMEPLAY_SHOW_TIP_BUTTON", "HWOPTION_GAMEPLAY_SHOW_TIP_BUTTON_F", HWOPTION_GAMEPLAY_SHOW_TIP_BUTTON );
		m_pForcedAttackButton = CreateFlipButton( "HWOPTION_GAMEPLAY_FORCED_ATTACK_BUTTON", "HWOPTION_GAMEPLAY_FORCED_ATTACK_BUTTON_F", HWOPTION_GAMEPLAY_FORCED_ATTACK_BUTTON );
		m_pNameDisplayButton = CreateFlipButton( "HWOPTION_GAMEPLAY_NAME_DISPLAY_BUTTON", "HWOPTION_GAMEPLAY_NAME_DISPLAY_BUTTON_F", HWOPTION_GAMEPLAY_NAME_DISPLAY_BUTTON );
		m_pSimpleSummonNameButton = CreateFlipButton( "HWOPTION_GAMEPLAY_SIMPLE_SUMMON_NAME_BUTTON", "HWOPTION_GAMEPLAY_SIMPLE_SUMMON_NAME_BUTTON_F", HWOPTION_GAMEPLAY_SIMPLE_SUMMON_NAME );
        m_pVehicleButton = CreateFlipButton( "HWOPTION_GAMEPLAY_VEHICLE_BUTTON", "HWOPTION_GAMEPLAY_VEHICLE_BUTTON_F", HWOPTION_GAMEPLAY_VEHICLE_BUTTON );
        m_pAutoCrowHPBarButton = CreateFlipButton( "HWOPTION_GAMEPLAY_AUTO_CROW_HPBAR_BUTTON", "HWOPTION_GAMEPLAY_AUTO_CROW_HPBAR_BUTTON_F", HWOPTION_GAMEPLAY_AUTO_CROW_HPBAR );

		// 내 정보 비공개
		if( GLUseFeatures::GetInstance().IsUsingReferChar() )
		{
			m_pCharInfoPrivateButton = CreateFlipButton( "HWOPTION_GAMEPLAY_CHAR_INFO_BUTTON", "HWOPTION_GAMEPLAY_CHAR_INFO_BUTTON_F", HWOPTION_GAMEPLAY_CHAR_INFO_PRIVATE );
		}

		// 추가 정보 보기
		m_pCharInfoAdditionButton = CreateFlipButton(  "HWOPTION_GAMEPLAY_CHAR_INFO_ADDITION_BUTTON", "HWOPTION_GAMEPLAY_CHAR_INFO_ADDITION_BUTTON_F", HWOPTION_GAMEPLAY_CHAR_INFO_ADDITION );

		// 자동 그룹채팅 거부
		if( GLUseFeatures::GetInstance().IsUsingGroupChat() )
		{
			m_pGroupChatInvateButton = CreateFlipButton( "HWOPTION_GAMEPLAY_GROUP_CHAT_BUTTON", "HWOPTION_GAMEPLAY_GROUP_CHAT_BUTTON_F", HWOPTION_GAMEPLAY_GROUP_CHAT_BUTTON );
		}

		// 자동 부활 시  부활하기로;
		m_pAutoRebirthButton = CreateFlipButton( "HWOPTION_GAMEPLAY_AUTO_REBIRTH_BUTTON", "HWOPTION_GAMEPLAY_AUTO_REBIRTH_BUTTON_F", HWOPTION_GAMEPLAY_AUTOREBIRTH );

		{
			CBasicButton* pButton = new CBasicButton(m_pEngineDevice);
			pButton->CreateSub ( this, "HWOPTION_GAMEPLAY_DEFAULTPOS_BUTTON", UI_FLAG_DEFAULT, HWOPTION_GAMEPLAY_DEFAULTPOS_BUTTON );
			pButton->CreateFlip ( "HWOPTION_GAMEPLAY_DEFAULTPOS_BUTTON_F", CBasicButton::CLICK_FLIP );				
			RegisterControl ( pButton );
		}
	}

	// 콤보 박스
	{
		

		CBasicComboBoxRollOver* pComboBoxRollOver = NULL;
		pComboBoxRollOver = new CBasicComboBoxRollOver(m_pEngineDevice);
		pComboBoxRollOver->CreateSub ( this, "BASIC_COMBOBOX_ROLLOVER", UI_FLAG_XSIZE | UI_FLAG_YSIZE, HWOPTION_GAMEPLAY_SIMPLE_COMBO_ROLLOVER );
		pComboBoxRollOver->CreateBaseComboBoxRollOver ( "HWOPTION_GAMEPLAY_SIMPLE_COMBO_ROLLOVER" );
		pComboBoxRollOver->SetVisibleSingle ( FALSE );
		RegisterControl ( pComboBoxRollOver );
		m_pComboBoxSimpleRollOver = pComboBoxRollOver;

		CString strTemp;
		m_pComboBoxSimpleRollOver->ClearText ();
		for ( int i = 0; i < EM_AROUND_SIZE; i++ )
		{
			strTemp = ID2GAMEWORD ( "HWOPTION_GAMEPLAY_SIMPLE", i );
			m_pComboBoxSimpleRollOver->AddText ( strTemp );
		}

		strTemp = ID2GAMEWORD ( "HWOPTION_GAMEPLAY_SIMPLE", 0 );
		m_pComboBoxSimpleOpen->SetText ( strTemp );
	}

	{
		CBasicComboBoxRollOver* pComboBoxRollOver = NULL;
		pComboBoxRollOver = new CBasicComboBoxRollOver(m_pEngineDevice);
		pComboBoxRollOver->CreateSub ( this, "BASIC_COMBOBOX_ROLLOVER", UI_FLAG_XSIZE | UI_FLAG_YSIZE, HWOPTION_GAMEPLAY_SKILLTYPE_COMBO_ROLLOVER );
		pComboBoxRollOver->CreateBaseComboBoxRollOver ( "HWOPTION_GAMEPLAY_SKILLTYPE_COMBO_ROLLOVER" );
		pComboBoxRollOver->SetVisibleSingle ( FALSE );
		RegisterControl ( pComboBoxRollOver );
		m_pComboSkillTypeRollOver = pComboBoxRollOver;


		CString strTemp;
		m_pComboSkillTypeRollOver->ClearText ();
		for ( int i = 0; i < EM_SKILLTYPE_SIZE; i++ )
		{
			strTemp = ID2GAMEWORD ( "HWOPTION_GAMEPLAY_SKILLTYPE", i );
			m_pComboSkillTypeRollOver->AddText ( strTemp );
		}

		strTemp = ID2GAMEWORD ( "HWOPTION_GAMEPLAY_SKILLTYPE", 0 );
		m_pComboSkillTypeOpen->SetText ( strTemp );
	}
}


CBasicButton* CBasicGamePlayOption::CreateFlipButton ( char* szButton, char* szButtonFlip, UIGUID ControlID )
{
	CBasicButton* pButton = new CBasicButton(m_pEngineDevice);
	pButton->CreateSub ( this, szButton, UI_FLAG_DEFAULT, ControlID );
	pButton->CreateFlip ( szButtonFlip, CBasicButton::RADIO_FLIP );
	pButton->SetControlNameEx ( szButton );
	RegisterControl ( pButton );
	return pButton;
}

CBasicTextBox* CBasicGamePlayOption::CreateStaticControl (const char* szControlKeyword, CD3DFontPar* pFont, D3DCOLOR dwColor, int nAlign )
{
	CBasicTextBox* pStaticText = new CBasicTextBox(m_pEngineDevice);
	pStaticText->CreateSub ( this, szControlKeyword );
	pStaticText->SetFont ( pFont );
	pStaticText->SetTextAlign ( nAlign );	
	RegisterControl ( pStaticText );
	return pStaticText;
}

void CBasicGamePlayOption::LoadConft ()
{
	m_bConft = DXPARAMSET::GetInstance().m_bDIS_CONFT;
	m_pConftButton->SetFlip ( m_bConft );
}

void CBasicGamePlayOption::LoadTrade ()
{
	m_bTrade = DXPARAMSET::GetInstance().m_bDIS_TRADE;
	m_pTradeButton->SetFlip ( m_bTrade );
}

void CBasicGamePlayOption::LoadParty ()
{
	m_bParty = DXPARAMSET::GetInstance().m_bDIS_PARTY;
	m_pPartyButton->SetFlip ( m_bParty );
}

void CBasicGamePlayOption::LoadSimpleHP ()
{
	m_bSimpleHP = DXPARAMSET::GetInstance().m_bSHOW_SIMPLEHP;
	m_pSimpleHPButton->SetFlip ( m_bSimpleHP );
}

void CBasicGamePlayOption::LoadFriend ()
{
	m_bFriend = RANPARAM::bDIS_FRIEND;
	m_pFriendButton->SetFlip ( m_bFriend );
}

void CBasicGamePlayOption::LoadVehicle ()
{
	m_bVehicle = RANPARAM::bDIS_VEHICLE;
	m_pVehicleButton->SetFlip ( m_bVehicle );
}

void CBasicGamePlayOption::LoadMovableOnChat ()
{
	m_bMovableOnChat = RANPARAM::bMOVABLE_ON_CHAT;
	m_pMovableOnChatButton->SetFlip ( m_bMovableOnChat );
}

void CBasicGamePlayOption::LoadShowTip ()
{
	m_bShowTip = RANPARAM::bSHOW_TIP;
	m_pShowTipButton->SetFlip ( m_bShowTip );
}

void CBasicGamePlayOption::LoadForcedAttack ()
{
	m_bForcedAttack = RANPARAM::bFORCED_ATTACK;
	m_pForcedAttackButton->SetFlip ( m_bForcedAttack );
}

void CBasicGamePlayOption::LoadNameDisplay ()
{
	m_bNameDisplay = RANPARAM::bNAME_DISPLAY;
	m_pNameDisplayButton->SetFlip ( m_bNameDisplay );
}

void CBasicGamePlayOption::LoadAroundSimple()
{
	m_dwAroundDetail = (int)RANPARAM::emAroundDetail;
	CString strTemp( ID2GAMEWORD ( "HWOPTION_GAMEPLAY_SIMPLE", m_dwAroundDetail ) );
	m_pComboBoxSimpleOpen->SetText ( strTemp );
}
 
void CBasicGamePlayOption::LoadSkillTypeUI()
{
	m_dwSkillTypeUI = (int)RANPARAM::emSkillTypeUI;
	CString strTemp( ID2GAMEWORD ( "HWOPTION_GAMEPLAY_SKILLTYPE", m_dwSkillTypeUI ) );
	m_pComboSkillTypeOpen->SetText ( strTemp );
}

void CBasicGamePlayOption::LoadSimpleSummonName ()
{
	m_bSimpleSummonName = RANPARAM::bSIMPLE_SUM_NAME;
	m_pSimpleSummonNameButton->SetFlip ( m_bSimpleSummonName );
}

void CBasicGamePlayOption::LoadAutoCrowHPBar ()
{
    m_bAutoCrowHPBar = RANPARAM::bAUTO_CROW_HP;
    m_pAutoCrowHPBarButton->SetFlip ( m_bAutoCrowHPBar );
}

void CBasicGamePlayOption::LoadCharInfoPrivate()
{
	if( GLUseFeatures::GetInstance().IsUsingReferChar() )
	{
		m_bCharInfoPrivate = RANPARAM::bCharInfoPrivate;
		m_pCharInfoPrivateButton->SetFlip( m_bCharInfoPrivate );
	}
}

void CBasicGamePlayOption::LoadCharInfoAddition()
{
	if( GLCONST_CHAR::bAdditional_ability )
	{
		m_bCharInfoAddition = RANPARAM::bCharInfoAddition;
		m_pCharInfoAdditionButton->SetFlip( m_bCharInfoAddition );
	}
	else
	{
		m_bCharInfoAddition = RANPARAM::bCharInfoAddition = GLCONST_CHAR::bAdditional_ability;
		m_pCharInfoAdditionButton->SetFlip( m_bCharInfoAddition );
	}
}

void CBasicGamePlayOption::LoadGroupChatInvite()
{
	if( GLUseFeatures::GetInstance().IsUsingGroupChat() )
	{
		m_bGroupChatInvite = RANPARAM::bGroupChatInvite;
		m_pGroupChatInvateButton->SetFlip( m_bGroupChatInvite );
	}
}

void CBasicGamePlayOption::LoadRebirthOption()
{
	//m_pAutoRebirthButton->SetText(ID2GAMEINTEXT("REBIRTH_WINDOW_AUTO_REBIRTH"));
	m_bAutoRebirth = RANPARAM::bCheckAutoRebirth;
	m_pAutoRebirthButton->SetFlip(RANPARAM::bCheckAutoRebirth);
}

void CBasicGamePlayOption::LoadCurrentOption()
{
	LoadConft();
	LoadTrade();
	LoadParty();
	LoadSimpleHP();
	LoadFriend();
	LoadMovableOnChat();
	LoadShowTip();
	LoadForcedAttack();
	LoadNameDisplay();
	LoadAroundSimple();
	LoadSimpleSummonName ();
    LoadAutoCrowHPBar();
	LoadSkillTypeUI();
	LoadVehicle();
	LoadCharInfoPrivate();
	LoadCharInfoAddition();
	LoadGroupChatInvite();
	LoadRebirthOption();
}

void CBasicGamePlayOption::SetVisibleSingle( BOOL bVisible )
{
	CUIGroup::SetVisibleSingle ( bVisible );

	if( bVisible )
	{
		LoadCurrentOption();
	}
}

void CBasicGamePlayOption::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	if ( m_RollOverID == NO_ID )
	{
		CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
	}
	else
	{
		ResetMessageEx ();

		CUIControl::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

		CUIControl* pControl = m_ControlContainer.FindControl ( m_RollOverID );
		if ( !pControl )
		{
			GASSERT ( 0 && "심각한 오류, 노드가 널입니다." );
			return ;
		}

		pControl->Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

		DWORD dwControlMsg = pControl->GetMessageEx ();
		NS_UIDEBUGSET::BlockBegin ();
		if ( dwControlMsg ) TranslateUIMessage ( m_RollOverID, dwControlMsg );
		NS_UIDEBUGSET::BlockEnd ();

		//	스크롤바에 관계된 메시지가 아니고		
		if ( !(dwControlMsg & UI_MSG_COMBOBOX_ROLLOVER_SCROLL) )
		{
			DWORD dwMsg = GetMessageEx ();
			if ( dwMsg & UIMSG_LB_UP )
			{
				if ( !m_bFirstLBUP )
				{
					m_RollOverID = NO_ID;
					pControl->SetVisibleSingle ( FALSE );					
				}
				m_bFirstLBUP = FALSE;
			}
		}
	}

	//CDebugSet::ToView ( 1, 18, "%d", m_RollOverID );
}