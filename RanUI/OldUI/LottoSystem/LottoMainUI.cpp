#include "stdafx.h"

#include "./LottoMainUI.h"

#include "./LottoBuyLottoUI.h"
#include "./LottoBuyListInfo.h"
#include "./LottoWinManListInfo.h"

#include "./MainUIPageBase.h"

#include "./GLLottoSystemManUI.h"

#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/InterfaceBaseDefine.h"

#include "../../InnerInterface.h"

#include "../../../SigmaCore/DebugInclude.h"

// Logic;
#include "../../../RanLogicClient/LottoSystem/GLLottoSystemManClient.h"
#include "../../../RanLogicClient/LottoSystem/GLLottoGlobalDataClient.h"

CLottoMainUI::CLottoMainUI( CInnerInterface* pInterface
						   , EngineDeviceMan* pEngineDevice )
						   : CUIWindowEx( pInterface, pEngineDevice )
						   , m_pCurrentPage( NULL )
						   , m_pHelpBox( NULL )
{
	for( unsigned int i=0; i<LottoSystem::LOTTO_MAX_PAGE_SIZE; ++i )
	{
		m_pTab[ i ] = NULL;
		m_pPage[ i ] = NULL;
	}

	for( unsigned int i=0; i<LottoSystem::HELP_TEXT_COUNT; ++i )
		m_pHelpText[ i ] = NULL;
}

void CLottoMainUI::CreateSubControl()
{
	CreateLineBox( "LOTTO_MAIN_UI_OUTLINE_REGION", "RAN_UI_BASIC_BACKGROUND_1" );

	static std::string strTabKeywords[ LottoSystem::LOTTO_MAX_PAGE_SIZE ] =
	{
		"LOTTO_MAIN_UI_GAME_MONEY_TAB",
		"LOTTO_MAIN_UI_RAN_POINT_TAB",
	};

	// 탭 생성;
	for( unsigned int i=0; i<LottoSystem::LOTTO_MAX_PAGE_SIZE; ++i )
	{
		m_pTab[ i ] = new STAPBUTTON( m_pEngineDevice );
		if( m_pTab[ i ] )
		{
			m_pTab[ i ]->CreateSub( this, strTabKeywords[ i ].c_str(), UI_FLAG_DEFAULT, ETabID+i );
			m_pTab[ i ]->CreateSubControl(
				ID2GAMEWORD( "LOTTO_MAIN_UI_TAB_NAME", 0 ),
				"LOTTO_MAIN_UI_TAB_BASE", "RAN_TAB_BUTTON_ACTIVE",
				"LOTTO_MAIN_UI_TAB_BASE", "RAN_TAB_BUTTON_INACTIVE",
				"LOTTO_MAIN_UI_TAB_BASE" );
			m_pTab[ i ]->SetVisibleSingle( FALSE );
			RegisterControl( m_pTab[ i ] );
			RegisterTapButton( m_pTab[ i ] );

			// 페이지 생성;
			m_pPage[ i ] = new CMainUIPageBase(
				m_pInterface,
				m_pEngineDevice );
			if( m_pPage[ i ] )
			{
				m_pPage[ i ]->CreateSub( this, "LOTTO_MAIN_UI_REGION", UI_FLAG_DEFAULT, EPageID+i );
				m_pPage[ i ]->CreateSubControl();
				m_pPage[ i ]->SetVisibleSingle( FALSE );
				m_pPage[ i ]->SetParent( this );
				RegisterControl( m_pPage[ i ] );
			}
		}
	}

	if( LottoSystem::LOTTO_MAX_PAGE_SIZE > 0 )
		EventSelectedTap( ETabID );

	{
		// 도움말 아이콘;
		RnButton::CreateArg arg;
		arg.defaultTextureKey = "RAN_HELP_BUTTON_DEFAULT_TEXTURE";
		arg.mouseOverTextureKey = "RAN_HELP_BUTTON_OVER_TEXTURE";
		arg.mouseClickTextureKey = "RAN_HELP_BUTTON_DEFAULT_TEXTURE";
		arg.dwColor = NS_UITEXTCOLOR::WHITE;

		RnButton* pHelpButton = new RnButton( m_pEngineDevice );
		pHelpButton->CreateSub(
			this,
			"LOTTO_MAIN_UI_HELP_BUTTON",
			UI_FLAG_DEFAULT,
			EViewHelpButton );
		pHelpButton->CreateSubControl( arg );
		RegisterControl( pHelpButton );

		// 도움말 윈도우 타이틀;
		m_pHelpBox = new CBasicLineBoxSmart( m_pEngineDevice );
		m_pHelpBox->CreateSub( this, "LOTTO_MAIN_UI_HELP_TEXTBOX", UI_FLAG_DEFAULT);
		m_pHelpBox->CreateSubControl( "BLUE_LINE_BACKGROUND" );
		m_pHelpBox->SetVisibleSingle( FALSE );
		RegisterControl( m_pHelpBox );

		// 도움말 텍스트;
		static const std::string strHelpTextUIId[ LottoSystem::HELP_TEXT_COUNT ] = {
			"LOTTO_MAIN_UI_HELP_TEXT_1",
			"LOTTO_MAIN_UI_HELP_TEXT_2",
			"LOTTO_MAIN_UI_HELP_TEXT_3"
		};

		static const std::string strHelpTextId[ LottoSystem::HELP_TEXT_COUNT ] = {
			"LOTTO_MAIN_UI_HELP_TEXT1",
			"LOTTO_MAIN_UI_HELP_TEXT2",
			"LOTTO_MAIN_UI_HELP_TEXT3"
		};

		for( unsigned int i=0; i<LottoSystem::HELP_TEXT_COUNT; ++i )
		{
			m_pHelpText[ i ] = new CBasicTextBox( m_pEngineDevice );
			m_pHelpText[ i ]->CreateSub( this, strHelpTextUIId[ i ].c_str(), UI_FLAG_DEFAULT );
			m_pHelpText[ i ]->SetFont ( m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG ) );	
			m_pHelpText[ i ]->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP );
			m_pHelpText[ i ]->SetVisibleSingle( FALSE );
			RegisterControl ( m_pHelpText[ i ] );

			CString strText = ID2GAMEINTEXT( strHelpTextId[ i ].c_str() );
			m_pHelpText[ i ]->SetText( strText, NS_UITEXTCOLOR::WHITE );
		}
	}
}

void CLottoMainUI::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIWindowEx::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CLottoMainUI::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	// MainUI로 부터 작동되는 UI가 켜지면 이벤트를 받지 않는다;
	EMTAPSELECTORRESULT emResult = EMTAPSELECTORRESULT_NONE;
	TapSelectorUIMessage( ControlID, dwMsg, emResult );
	CUIWindowEx::TranslateUIMessage( ControlID, dwMsg );

	switch( ControlID )
	{
	case ET_CONTROL_TITLE:
	case ET_CONTROL_TITLE_F:
		{
			if( ( dwMsg & UIMSG_LB_DUP ) && CHECK_MOUSE_IN( dwMsg ) )
			{
				m_pInterface->SetDefaultPosInterface( LOTTO_MAIN_UI );
			}
		}
		break;
	case EViewHelpButton:
		if( CHECK_MOUSE_IN( dwMsg ) )
		{
			m_pHelpBox->SetVisibleSingle( TRUE );
			for( unsigned int i=0; i<LottoSystem::HELP_TEXT_COUNT; ++i )
				m_pHelpText[ i ]->SetVisibleSingle( TRUE );
		}
		else
		{
			m_pHelpBox->SetVisibleSingle( FALSE );
			for( unsigned int i=0; i<LottoSystem::HELP_TEXT_COUNT; ++i )
				m_pHelpText[ i ]->SetVisibleSingle( FALSE );
		}
		break;
	}
}

void CLottoMainUI::SetVisibleSingle( BOOL bVisible )
{
	CUIWindowEx::SetVisibleSingle( bVisible );

	if( m_pCurrentPage )
		m_pCurrentPage->SetVisibleSingle( bVisible );
}

void CLottoMainUI::SetFocusControl()
{
	CUIWindowEx::SetFocusControl();

	if( GLLottoSystemManUI::Instance()->IsBuyLottoUIOpen() )
		m_pInterface->UiShowGroupFocus( LOTTO_BUY_LOTTO_UI );
}

void CLottoMainUI::EventSelectedTap( UIGUID controlID )
{
	if( m_pCurrentPage )
		m_pCurrentPage->SetVisibleSingle( FALSE );
	
	int tabIdx = static_cast<int>( controlID - ETabID );
	if( tabIdx >= 0 && tabIdx < LottoSystem::LOTTO_MAX_PAGE_SIZE )
	{
		m_pCurrentPage = m_pPage[ tabIdx ];
		GLLottoSystemManUI::Instance()->ChangeTabIndex( tabIdx );
	}

	if( m_pCurrentPage )
		m_pCurrentPage->SetVisibleSingle( TRUE );
}

void CLottoMainUI::InitializeUI( const LottoSystem::CLottoSystemClient* pLottoSystem )
{
	LottoSystem::LOTTO_SYSTEM_CLIENT_VALUE sValue = 
		LottoSystem::GLLottoGlobalDataClient::Instance()
		->GetLottoSystemValue( static_cast<unsigned int>(
		pLottoSystem->GetLottoSystemId() ) );

	int tabIdx = sValue.useIndex;
	if( tabIdx >= 0 && tabIdx < LottoSystem::LOTTO_MAX_PAGE_SIZE )
	{
		int wordIdx = sValue.moneyType;

		STAPBUTTON*	pTab = m_pTab[ tabIdx ];
		if( pTab )
		{
			pTab->SetTabLabel( ID2GAMEWORD( "LOTTO_MAIN_UI_TAB_NAME", wordIdx ) );
			pTab->SetVisibleSingle( TRUE );
		}

		CMainUIPageBase* pPage = m_pPage[ tabIdx ];
		if( pPage )
			pPage->SetSystemID( pLottoSystem->GetLottoSystemId() );
	}	
}

void CLottoMainUI::UpdateAccumulateMoneyTurnProc( const LottoSystem::CLottoSystemClient* pLottoSystem )
{
	if( !pLottoSystem )
		return;

	LottoSystem::LOTTO_SYSTEM_CLIENT_VALUE sValue = 
		LottoSystem::GLLottoGlobalDataClient::Instance()
		->GetLottoSystemValue( static_cast<unsigned int>(
		pLottoSystem->GetLottoSystemId() ) );

	int tabIdx = sValue.useIndex;
	if( tabIdx >= 0 && tabIdx < LottoSystem::LOTTO_MAX_PAGE_SIZE )
	{
		if( m_pPage[ tabIdx ] )
			m_pPage[ tabIdx ]->UpdateAccumulateMoneyTurnProc( pLottoSystem );
	}
}

void CLottoMainUI::UpdateDrawingDateStateProc( const LottoSystem::CLottoSystemClient* pLottoSystem )
{
	if( !pLottoSystem )
		return;

	LottoSystem::LOTTO_SYSTEM_CLIENT_VALUE sValue = 
		LottoSystem::GLLottoGlobalDataClient::Instance()
		->GetLottoSystemValue( static_cast<unsigned int>(
		pLottoSystem->GetLottoSystemId() ) );

	int tabIdx = sValue.useIndex;
	if( tabIdx >= 0 && tabIdx < LottoSystem::LOTTO_MAX_PAGE_SIZE )
	{
		if( m_pPage[ tabIdx ] )
			m_pPage[ tabIdx ]->UpdateDrawingDateStateProc( pLottoSystem );
	}
}

void CLottoMainUI::UpdateBuyListStateProc( const LottoSystem::CLottoSystemClient* pLottoSystem )
{
	if( !pLottoSystem )
		return;

	LottoSystem::LOTTO_SYSTEM_CLIENT_VALUE sValue = 
		LottoSystem::GLLottoGlobalDataClient::Instance()
		->GetLottoSystemValue( static_cast<unsigned int>(
		pLottoSystem->GetLottoSystemId() ) );

	int tabIdx = sValue.useIndex;
	if( tabIdx >= 0 && tabIdx < LottoSystem::LOTTO_MAX_PAGE_SIZE )
	{
		if( m_pPage[ tabIdx ] )
			m_pPage[ tabIdx ]->UpdateBuyListStateProc( pLottoSystem );
	}
}

void CLottoMainUI::UpdateDrawingDateProc( const LottoSystem::CLottoSystemClient* pLottoSystem )
{
	if( !pLottoSystem )
		return;

	LottoSystem::LOTTO_SYSTEM_CLIENT_VALUE sValue = 
		LottoSystem::GLLottoGlobalDataClient::Instance()
		->GetLottoSystemValue( static_cast<unsigned int>(
		pLottoSystem->GetLottoSystemId() ) );

	int tabIdx = sValue.useIndex;
	if( tabIdx >= 0 && tabIdx < LottoSystem::LOTTO_MAX_PAGE_SIZE )
	{
		if( m_pPage[ tabIdx ] )
			m_pPage[ tabIdx ]->UpdateDrawingDateProc( pLottoSystem );
	}
}

void CLottoMainUI::UpdateProgressTimeProc( const LottoSystem::CLottoSystemClient* pLottoSystem )
{
	if( !pLottoSystem )
		return;

	LottoSystem::LOTTO_SYSTEM_CLIENT_VALUE sValue = 
		LottoSystem::GLLottoGlobalDataClient::Instance()
		->GetLottoSystemValue( static_cast<unsigned int>(
		pLottoSystem->GetLottoSystemId() ) );

	int tabIdx = sValue.useIndex;
	if( tabIdx >= 0 && tabIdx < LottoSystem::LOTTO_MAX_PAGE_SIZE )
	{
		if( m_pPage[ tabIdx ] )
			m_pPage[ tabIdx ]->UpdateProgressTimeProc( pLottoSystem );
	}
}

void CLottoMainUI::UpdateMachineTurn( const LottoSystem::CLottoSystemClient* pLottoSystem )
{
	if( !pLottoSystem )
		return;

	LottoSystem::LOTTO_SYSTEM_CLIENT_VALUE sValue = 
		LottoSystem::GLLottoGlobalDataClient::Instance()
		->GetLottoSystemValue( static_cast<unsigned int>(
		pLottoSystem->GetLottoSystemId() ) );

	int tabIdx = sValue.useIndex;
	if( tabIdx >= 0 && tabIdx < LottoSystem::LOTTO_MAX_PAGE_SIZE )
	{
		if( m_pPage[ tabIdx ] )
			m_pPage[ tabIdx ]->UpdateMachineTurn( pLottoSystem );
	}
}

void CLottoMainUI::UpdateMachine( const LottoSystem::CLottoSystemClient* pLottoSystem
								 , int drawingNum )
{
	if( !pLottoSystem )
		return;

	LottoSystem::LOTTO_SYSTEM_CLIENT_VALUE sValue = 
		LottoSystem::GLLottoGlobalDataClient::Instance()
		->GetLottoSystemValue( static_cast<unsigned int>(
		pLottoSystem->GetLottoSystemId() ) );

	int tabIdx = sValue.useIndex;
	if( tabIdx >= 0 && tabIdx < LottoSystem::LOTTO_MAX_PAGE_SIZE )
	{
		if( m_pPage[ tabIdx ] )
			m_pPage[ tabIdx ]->UpdateMachine( pLottoSystem, drawingNum );
	}
}

void CLottoMainUI::UpdateWinNum( const LottoSystem::CLottoSystemClient* pLottoSystem )
{
	if( !pLottoSystem )
		return;

	LottoSystem::LOTTO_SYSTEM_CLIENT_VALUE sValue = 
		LottoSystem::GLLottoGlobalDataClient::Instance()
		->GetLottoSystemValue( static_cast<unsigned int>(
		pLottoSystem->GetLottoSystemId() ) );

	int tabIdx = sValue.useIndex;
	if( tabIdx >= 0 && tabIdx < LottoSystem::LOTTO_MAX_PAGE_SIZE )
	{
		if( m_pPage[ tabIdx ] )
			m_pPage[ tabIdx ]->UpdateWinNum( pLottoSystem );
	}
}

void CLottoMainUI::UpdateBuyList( const LottoSystem::CLottoSystemClient* pLottoSystem
								 , int idx )
{
	if( !pLottoSystem )
		return;

	LottoSystem::LOTTO_SYSTEM_CLIENT_VALUE sValue = 
		LottoSystem::GLLottoGlobalDataClient::Instance()
		->GetLottoSystemValue( static_cast<unsigned int>(
		pLottoSystem->GetLottoSystemId() ) );

	int tabIdx = sValue.useIndex;
	if( tabIdx >= 0 && tabIdx < LottoSystem::LOTTO_MAX_PAGE_SIZE )
	{
		if( m_pPage[ tabIdx ] )
			m_pPage[ tabIdx ]->UpdateBuyList( pLottoSystem, idx );
	}
}

void CLottoMainUI::UpdateBuyListCount( const LottoSystem::CLottoSystemClient* pLottoSystem
									  , int count )
{
	if( !pLottoSystem )
		return;

	LottoSystem::LOTTO_SYSTEM_CLIENT_VALUE sValue = 
		LottoSystem::GLLottoGlobalDataClient::Instance()
		->GetLottoSystemValue( static_cast<unsigned int>(
		pLottoSystem->GetLottoSystemId() ) );

	int tabIdx = sValue.useIndex;
	if( tabIdx >= 0 && tabIdx < LottoSystem::LOTTO_MAX_PAGE_SIZE )
	{
		if( m_pPage[ tabIdx ] )
			m_pPage[ tabIdx ]->UpdateBuyListCount( pLottoSystem, count );
	}
}

void CLottoMainUI::UpdateAccumulateMoney( const LottoSystem::CLottoSystemClient* pLottoSystem )
{
	if( !pLottoSystem )
		return;

	LottoSystem::LOTTO_SYSTEM_CLIENT_VALUE sValue = 
		LottoSystem::GLLottoGlobalDataClient::Instance()
		->GetLottoSystemValue( static_cast<unsigned int>(
		pLottoSystem->GetLottoSystemId() ) );

	int tabIdx = sValue.useIndex;
	if( tabIdx >= 0 && tabIdx < LottoSystem::LOTTO_MAX_PAGE_SIZE )
	{
		if( m_pPage[ tabIdx ] )
			m_pPage[ tabIdx ]->UpdateAccumulateMoney( pLottoSystem );

	}
}

void CLottoMainUI::UpdateMachineAnimation(
	const LottoSystem::CLottoSystemClient* pLottoSystem,
	float fElapsedTime )
{
	if( !pLottoSystem )
		return;

	LottoSystem::LOTTO_SYSTEM_CLIENT_VALUE sValue = 
		LottoSystem::GLLottoGlobalDataClient::Instance()
		->GetLottoSystemValue( static_cast<unsigned int>(
		pLottoSystem->GetLottoSystemId() ) );

	int tabIdx = sValue.useIndex;
	if( tabIdx >= 0 && tabIdx < LottoSystem::LOTTO_MAX_PAGE_SIZE )
	{
		if( m_pPage[ tabIdx ] )
			m_pPage[ tabIdx ]->UpdateMachineAnimation( fElapsedTime );

	}
}

void CLottoMainUI::OpenMessageBox( const CMessageBox::BOXTITLE eBoxTitle
								  , const std::string& strMessage
								  , const CMessageBox::BOXTYPE eBoxType
								  , unsigned int nIdentity )
{
	if( m_pCurrentPage )
		m_pCurrentPage->OpenMessageBox( eBoxTitle, strMessage, eBoxType, nIdentity );
}

void CLottoMainUI::InactiveConfirmButton()
{
	if( m_pCurrentPage )
		m_pCurrentPage->InactiveConfrimButton();
}

void CLottoMainUI::ActiveConfirmButton()
{
	if( m_pCurrentPage )
		m_pCurrentPage->ActiveConfrimButton();
}

MyLottoMainUI::MyLottoMainUI( CInnerInterface* pInterface
							 , EngineDeviceMan* pEngineDevice )
							 : CLottoMainUI( pInterface
							 , pEngineDevice )
{

}

void MyLottoMainUI::CreateUIWindowAndRegisterOwnership()
{
	CLottoMainUI::Create( LOTTO_MAIN_UI, "BASIC_WINDOW", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	CLottoMainUI::CreateBaseWindowLightGray( "LOTTO_MAIN_UI", (char*)ID2GAMEINTEXT("LOTTO_MAIN_UI_TITLE_TEXT") );
	CLottoMainUI::CreateSubControl();
	CLottoMainUI::SetAlignFlag( UI_FLAG_CENTER_Y | UI_FLAG_RIGHT );
	CLottoMainUI::m_pInterface->UiRegisterControl( this, true );
	CLottoMainUI::m_pInterface->UiShowGroupFocus( LOTTO_MAIN_UI );
}

bool MyLottoMainUI::IsVisible()
{
	return CLottoMainUI::IsVisible();
}

void MyLottoMainUI::InitializeUI( const LottoSystem::CLottoSystemClient* pLottoSystem )
{
	CLottoMainUI::InitializeUI( pLottoSystem );
}

void MyLottoMainUI::UpdateAccumulateMoneyTurnProc( const LottoSystem::CLottoSystemClient* pLottoSystem )
{
	CLottoMainUI::UpdateAccumulateMoneyTurnProc( pLottoSystem );
}

void MyLottoMainUI::UpdateDrawingDateStateProc( const LottoSystem::CLottoSystemClient* pLottoSystem )
{
	CLottoMainUI::UpdateDrawingDateStateProc( pLottoSystem );
}

void MyLottoMainUI::UpdateBuyListStateProc( const LottoSystem::CLottoSystemClient* pLottoSystem )
{
	CLottoMainUI::UpdateBuyListStateProc( pLottoSystem );
}

void MyLottoMainUI::UpdateDrawingDateProc( const LottoSystem::CLottoSystemClient* pLottoSystem )
{
	CLottoMainUI::UpdateDrawingDateProc( pLottoSystem );
}

void MyLottoMainUI::UpdateProgressTimeProc( const LottoSystem::CLottoSystemClient* pLottoSystem )
{
	CLottoMainUI::UpdateProgressTimeProc( pLottoSystem );
}

void MyLottoMainUI::UpdateMachineTurn( const LottoSystem::CLottoSystemClient* pLottoSystem )
{
	CLottoMainUI::UpdateMachineTurn( pLottoSystem );
}

void MyLottoMainUI::UpdateMachine( const LottoSystem::CLottoSystemClient* pLottoSystem
								  , int drawingNum )
{
	CLottoMainUI::UpdateMachine( pLottoSystem, drawingNum );
}

void MyLottoMainUI::UpdateWinNum( const LottoSystem::CLottoSystemClient* pLottoSystem )
{
	CLottoMainUI::UpdateWinNum( pLottoSystem );
}

void MyLottoMainUI::UpdateBuyList( const LottoSystem::CLottoSystemClient* pLottoSystem
								  , int idx )
{
	CLottoMainUI::UpdateBuyList( pLottoSystem, idx );
}

void MyLottoMainUI::UpdateBuyListCount( const LottoSystem::CLottoSystemClient* pLottoSystem
									   , int count )
{
	CLottoMainUI::UpdateBuyListCount( pLottoSystem, count );
}

void MyLottoMainUI::UpdateAccumulateMoney( const LottoSystem::CLottoSystemClient* pLottoSystem )
{
	CLottoMainUI::UpdateAccumulateMoney( pLottoSystem );
}

void MyLottoMainUI::UpdateMachineAnimation(
	const LottoSystem::CLottoSystemClient* pLottoSystem,
	float fElapsedTime )
{
	CLottoMainUI::UpdateMachineAnimation( pLottoSystem, fElapsedTime );
}

void MyLottoMainUI::OpenMessageBox( const CMessageBox::BOXTITLE eBoxTitle
								   , const std::string& strMessage
								   , const CMessageBox::BOXTYPE eBoxType
								   , unsigned int nIdentity )
{
	CLottoMainUI::OpenMessageBox( eBoxTitle, strMessage, eBoxType, nIdentity );
}

void MyLottoMainUI::InactiveConfirmButton()
{
	CLottoMainUI::InactiveConfirmButton();
}

void MyLottoMainUI::ActiveConfirmButton()
{
	CLottoMainUI::ActiveConfirmButton();
}