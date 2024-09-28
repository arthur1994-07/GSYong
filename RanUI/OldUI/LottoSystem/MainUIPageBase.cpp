#include "stdafx.h"

#include "./MainUIPageBase.h"

#include "./LottoMainUI.h"

#include "./LottoAccumulateMoneyUIComp.h"
#include "./LottoBasicBuyListUIComp.h"
#include "./LottoDrawingDateUIComp.h"
#include "./LottoMachineViewerUIComp.h"

#include "./LottoMainUI.h"
#include "./LottoBuyLottoUI.h"
#include "./LottoBuyListInfo.h"
#include "./LottoWinManListInfo.h"

#include "./GLLottoSystemManUI.h"

#include "../../../SigmaCore/Json/MinJson.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicButton.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/DxTools/d3dfont.h"

#include "../../../RanLogic/GLogicEx.h"
#include "../../../RanLogic/GLUseFeatures.h"

#include "../../InnerInterface.h"

#include "../Util/RnButton.h"
#include "../Util/CheckBox.h"

#include "../../../SigmaCore/DebugInclude.h"

// Logic;
#include "../../../RanLogicClient/LottoSystem/GLLottoSystemManClient.h"
#include "../../../RanLogicClient/LottoSystem/GLLottoGlobalDataClient.h"

CMainUIPageBase::CMainUIPageBase( CInnerInterface* pInterface
								 , EngineDeviceMan* pEngineDevice )
								 : CUIGroupHelperMessageBox( pEngineDevice )
								 , m_pInterface( pInterface )
								 , m_systemId( 0 )
								 , m_pCheckBox( NULL )
								 , m_pUpdateButton( NULL )
								 , m_pConfirmWinButton( NULL )
								 , m_pAccumulateMoneyUIComp( NULL )
								 , m_pBuyListUIComp( NULL )
								 , m_pDrawingDateUIComp( NULL )
								 , m_pMachineViewerUIComp( NULL )
{
}

CMainUIPageBase::~CMainUIPageBase() { }

void CMainUIPageBase::CreateSubControl()
{
	// Text Box;
	{		
		CreateText(
			sc::string::format( ID2GAMEINTEXT( "LOTTO_MAIN_UI_DRAWING_DATE_TEXT" ), "<", ">" ).c_str(),
			"LOTTO_MAIN_UI_VIEW_DRAWING_DATE_TEXTBOX",
			TEXT_ALIGN_LEFT|TEXT_ALIGN_TOP );
	}

	// 누적 당첨금 항상 보기 Check Box;
	{
		CreateText( ID2GAMEINTEXT( "LOTTO_MAIN_UI_VIEW_ACCUMULATE_MONEY_TEXT" ),
			"LOTTO_MAIN_UI_VIEW_ACCUMULATE_CHECK_TEXTBOX",
			TEXT_ALIGN_LEFT|TEXT_ALIGN_TOP );

		m_pCheckBox = new CCheckBox( m_pEngineDevice ); 
		m_pCheckBox->CreateSub( this, "LOTTO_MAIN_UI_VIEW_ACCUMULATE_CHECK_BOX" , UI_FLAG_DEFAULT , EViewAccumulateMoneyCheckBox );
		m_pCheckBox->CreateSubControl ( "RAN_CHECK_BOX_ACTIVE", "RAN_CHECK_BOX_INACTIVE" );
		m_pCheckBox->SetCheck( FALSE );
		m_pCheckBox->SetVisibleSingle( TRUE );
		RegisterControl( m_pCheckBox );
	}

	// Button;
	{
		const float eventPressedTime = 0.05f;

		{	
			RnButton::CreateArg arg;
			arg.defaultTextureKey = "RAN_UPDATE_BUTTON_DEFAULT_TEXTURE";
			arg.mouseOverTextureKey = "RAN_UPDATE_BUTTON_OVER_TEXTURE";
			arg.mouseClickTextureKey = "RAN_UPDATE_BUTTON_CLICK_TEXTURE";
			arg.pFont = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 8, _DEFAULT_FONT_SHADOW_FLAG );

			// 누적 당첨금 갱신 버튼;
			{
				m_pUpdateButton = new RnButton( m_pEngineDevice );
				if( m_pUpdateButton )
				{
					m_pUpdateButton->CreateSub( this, "LOTTO_MAIN_UI_UPDATE_ACCUMULATE_BUTTON", UI_FLAG_DEFAULT, EUpdateAccumulateMoneyButton );
					m_pUpdateButton->CreateSubControl( arg );
					m_pUpdateButton->SetPressedEventTime( eventPressedTime );
					RegisterControl( m_pUpdateButton );
				}
			}
		}

		{	
			RnButton::CreateArg arg;
			arg.defaultTextureKey = "RAN_RNBUTTON_DEFAULT_TEXTURE";
			arg.mouseOverTextureKey = "RAN_RNBUTTON_OVER_TEXTURE";
			arg.mouseClickTextureKey = "RAN_RNBUTTON_CLICK_TEXTURE";
			arg.pFont = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 8, _DEFAULT_FONT_SHADOW_FLAG );

			// 이전 회 결과 보기 버튼;
			{
				RnButton* pButton = new RnButton( m_pEngineDevice );
				if( pButton )
				{
					pButton->CreateSub( this, "LOTTO_MAIN_UI_RESULT_BUTTON", UI_FLAG_DEFAULT, EViewWinManListButton );
					arg.text = ID2GAMEWORD( "LOTTO_MAIN_UI_BUTTON_TEXT", 0 );
					pButton->CreateSubControl( arg );
					pButton->SetPressedEventTime( eventPressedTime );
					RegisterControl( pButton );
				}
			}

			// 당첨 확인 버튼;
			{
				m_pConfirmWinButton = new RnButton( m_pEngineDevice );
				if( m_pConfirmWinButton )
				{
					m_pConfirmWinButton->CreateSub( this, "LOTTO_MAIN_UI_CONFIRM_BUTTON", UI_FLAG_DEFAULT, EConfirmMyResultButton );
					arg.text = ID2GAMEWORD( "LOTTO_MAIN_UI_BUTTON_TEXT", 1 );
					m_pConfirmWinButton->CreateSubControl( arg );
					m_pConfirmWinButton->SetPressedEventTime( eventPressedTime );
					RegisterControl( m_pConfirmWinButton );
				}
			}

			// 닫기 버튼;
			{
				RnButton* pButton = new RnButton( m_pEngineDevice );
				if( pButton )
				{
					pButton->CreateSub( this, "LOTTO_MAIN_UI_CLOSE_BUTTON", UI_FLAG_DEFAULT, ECloseButton );
					arg.text = ID2GAMEWORD( "LOTTO_MAIN_UI_BUTTON_TEXT", 2 );
					pButton->CreateSubControl( arg );
					pButton->SetPressedEventTime( eventPressedTime );
					RegisterControl( pButton );
				}
			}
		}
	}

	// Accumlate Money UI;
	{
		m_pAccumulateMoneyUIComp = new CLottoAccumulateMoneyUIComp(
			m_pInterface, m_pEngineDevice );
		if( m_pAccumulateMoneyUIComp )
		{
			m_pAccumulateMoneyUIComp->CreateSub( this, "LOTTO_MAIN_UI_ACCUMULATE_MONEY_REGION", UI_FLAG_DEFAULT );
			m_pAccumulateMoneyUIComp->CreateSubControl();
			m_pAccumulateMoneyUIComp->SetVisibleSingle( TRUE );
			m_pAccumulateMoneyUIComp->SetParent( this );
			RegisterControl( m_pAccumulateMoneyUIComp );
		}
	}

	// Drawing Date UI;
	{
		m_pDrawingDateUIComp = new CLottoDrawingDateUIComp(
			m_pInterface, m_pEngineDevice );
		if( m_pDrawingDateUIComp )
		{
			m_pDrawingDateUIComp->CreateSub( this, "LOTTO_MAIN_UI_DRAWING_DATE_REGION", UI_FLAG_DEFAULT );
			m_pDrawingDateUIComp->CreateSubControl();
			m_pDrawingDateUIComp->SetVisibleSingle( TRUE );
			m_pDrawingDateUIComp->SetParent( this );
			RegisterControl( m_pDrawingDateUIComp );
		}
	}

	// Buy List UI;
	{
		m_pBuyListUIComp = new CLottoBasicBuyListUIComp(
			m_pInterface, m_pEngineDevice );
		if( m_pBuyListUIComp )
		{
			m_pBuyListUIComp->SetLottoSystemId( m_systemId );
			m_pBuyListUIComp->CreateSub( this, "LOTTO_MAIN_UI_BUY_LIST_REGION", UI_FLAG_DEFAULT );
			m_pBuyListUIComp->CreateSubControl();
			m_pBuyListUIComp->SetVisibleSingle( TRUE );
			RegisterControl( m_pBuyListUIComp );
		}
	}

	// Lotto Machine Viewer;
	{
		m_pMachineViewerUIComp = new CLottoMachineViewerUIComp(
			m_pInterface, m_pEngineDevice );
		if( m_pMachineViewerUIComp )
		{
			m_pMachineViewerUIComp->CreateSub( this, "LOTTO_MAIN_UI_MACHINE_REGION", UI_FLAG_DEFAULT );
			m_pMachineViewerUIComp->CreateSubControl();
			RegisterControl( m_pMachineViewerUIComp );
		}
	}

	CreateMessageBox();
}

void CMainUIPageBase::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	if( GLLottoSystemManUI::Instance()->IsBuyLottoUIOpen() )
		return;

	CUIGroupHelperMessageBox::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	LottoSystem::CLottoSystemClient* pLottoSystem =
		LottoSystem::GLLottoSystemManClient::Instance()
		->GetLottoSystemUsingTabIdx( GLLottoSystemManUI::Instance()->GetCurTabIndex() );

	if( pLottoSystem )
	{
		if( pLottoSystem->GetConfirmBuyCount() )
			ActiveConfrimButton();
		else
			InactiveConfrimButton();
	}
}

void CMainUIPageBase::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	LottoSystem::CLottoSystemClient* pLottoSystem =
		LottoSystem::GLLottoSystemManClient::Instance()->GetLottoSystem( m_systemId );

	CUIGroupHelperMessageBox::TranslateUIMessage( ControlID, dwMsg );

	switch( ControlID )
	{
	case EViewAccumulateMoneyCheckBox:
		if( CHECK_MOUSEIN_LBUPLIKE(dwMsg) )
		{
			if( m_pCheckBox->IsChecked() )
			{
				LottoSystem::LOTTO_SYSTEM_CLIENT_VALUE sValue = 
					LottoSystem::GLLottoGlobalDataClient::Instance()
					->GetLottoSystemValue( static_cast<unsigned int>(
					pLottoSystem->GetLottoSystemId() ) );
				int tabIdx = sValue.useIndex;

				// 어떤 탭의 체크박스가 선택되어 있는지 저장;
				GLLottoSystemManUI::Instance()->SetAccumulateMoneyLED(
					tabIdx );

				ILottoDisplayUI* pDisplayUI = m_pInterface->GetLottoDisplayUI();
				if( pDisplayUI )
				{
					pDisplayUI->UpdateAccumulateMoney( pLottoSystem );
					pDisplayUI->ResetTextAnimation();
				}
				
				m_pInterface->UiShowGroupFocus( LOTTO_DISPLAY_UI );
			}
			else
			{
				// 이벤트는 들어오는데 체크가 안되어있는 경우는;
				// 체크가 풀리는 즉, 아무곳에도 체크가 되어있지 않은;
				// 경우와 같다;
				GLLottoSystemManUI::Instance()->SetAccumulateMoneyLED(
					0xffffffff );
			}
		}
		break;
	case EUpdateAccumulateMoneyButton:
		if( CHECK_MOUSE_IN( dwMsg ) )
		{
			m_pInterface->SHOW_COMMON_LINEINFO(
				ID2GAMEINTEXT( "LOTTO_MAIN_UI_ACCUMULATE_MONEY_TOOLTIP_TEXT" ),
				NS_UITEXTCOLOR::YELLOW,
				TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP );

			if( CHECK_MOUSEIN_LBUPLIKE(dwMsg) )
			{
				pLottoSystem->UpdateAccumulateMoney();
			}
		}
		break;
	case EViewWinManListButton:
		if( CHECK_MOUSEIN_LBUPLIKE(dwMsg) )
		{
			if( !m_pInterface->IsVisible(LOTTO_WIN_MAN_LIST_INFO ) )
				m_pInterface->UiShowGroupFocus( LOTTO_WIN_MAN_LIST_INFO );
			else
				m_pInterface->UiHideGroup( LOTTO_WIN_MAN_LIST_INFO );
		}
		break;
	case EConfirmMyResultButton:
		ConfirmMyResultButtonProc( ControlID, dwMsg );
		break;
	case ECloseButton:
		if( CHECK_MOUSEIN_LBUPLIKE(dwMsg) )
		{
			m_pInterface->UiHideGroup( LOTTO_MAIN_UI );
		}
		break;
	}
}

void CMainUIPageBase::SetVisibleSingle( BOOL bVisible )
{
	CUIGroupHelperMessageBox::SetVisibleSingle( bVisible );

	if( m_pMachineViewerUIComp )
		m_pMachineViewerUIComp->SetVisibleSingle( bVisible );

	if( bVisible )
	{
		LottoSystem::CLottoSystemClient* pLottoSystem =
			LottoSystem::GLLottoSystemManClient::Instance()
			->GetLottoSystem( m_systemId );

		LottoSystem::LOTTO_SYSTEM_CLIENT_VALUE sValue = 
			LottoSystem::GLLottoGlobalDataClient::Instance()
			->GetLottoSystemValue( static_cast<unsigned int>(
			m_systemId ) );
		int tabIdx = sValue.useIndex;

		if( GLLottoSystemManUI::Instance()->GetAccumulateMoneyLED()
			== tabIdx )
			m_pCheckBox->SetCheckOn();
		else
			m_pCheckBox->SetCheckOff();

		if( GLLottoSystemManUI::Instance()->GetCurTabIndex() == tabIdx &&
			pLottoSystem )
		{
			ILottoBuyListInfo* pLottoBuyListInfo = 
				m_pInterface->GetLottoBuyListInfo();

			const unsigned int nNonCofirmBuyList =
				pLottoSystem->GetConfirmBuyCount();
			if( pLottoBuyListInfo && nNonCofirmBuyList )
			{
				pLottoBuyListInfo->UpdateWinNumProc( pLottoSystem );
				pLottoBuyListInfo->UpdateChangeState( pLottoSystem );
				pLottoBuyListInfo->UpdateBuyList( pLottoSystem );
			}
		}
	}
}

void CMainUIPageBase::ConfirmMyResultButtonProc( UIGUID ControlID, DWORD dwMsg )
{
	LottoSystem::CLottoSystemClient* pLottoSystem =
		LottoSystem::GLLottoSystemManClient::Instance()->GetLottoSystem( m_systemId );

	if( !pLottoSystem )
		return;

	switch( pLottoSystem->GetLottoSystemState() )
	{
	case LottoSystem::LOTTO_STATE_GATHERING:
	case LottoSystem::LOTTO_STATE_BASIC:
		if( CHECK_MOUSEIN_LBUPLIKE(dwMsg) )
		{
			if( !m_pInterface->IsVisible(LOTTO_BUY_LIST_INFO ) )
				m_pInterface->UiShowGroupFocus( LOTTO_BUY_LIST_INFO );
			else
				m_pInterface->UiHideGroup( LOTTO_BUY_LIST_INFO );
		}
		break;
	case LottoSystem::LOTTO_STATE_WAIT:
	case LottoSystem::LOTTO_STATE_DRAWING:
		break;
	}
}

void CMainUIPageBase::InactiveConfrimButton()
{
	if( m_pConfirmWinButton )
		m_pConfirmWinButton->SetVisibleSingle( FALSE );
}

void CMainUIPageBase::ActiveConfrimButton()
{
	if( m_pConfirmWinButton )
		m_pConfirmWinButton->SetVisibleSingle( TRUE );
}

void CMainUIPageBase::SetSystemID( LottoSystem::LOTTO_SYSTEM_ID systemId )
{
	m_systemId = systemId;
}

void CMainUIPageBase::UpdateAccumulateMoneyTurnProc( const LottoSystem::CLottoSystemClient* pLottoSystem )
{
	if( m_pAccumulateMoneyUIComp )
		m_pAccumulateMoneyUIComp->UpdateTurnProc( pLottoSystem );
}

void CMainUIPageBase::UpdateDrawingDateStateProc( const LottoSystem::CLottoSystemClient* pLottoSystem )
{
	if( m_pDrawingDateUIComp )
		m_pDrawingDateUIComp->UpdateStateProc( pLottoSystem );
}

void CMainUIPageBase::UpdateBuyListStateProc( const LottoSystem::CLottoSystemClient* pLottoSystem )
{
	if( m_pBuyListUIComp )
		m_pBuyListUIComp->UpdateChangeState( pLottoSystem );
}

void CMainUIPageBase::UpdateDrawingDateProc( const LottoSystem::CLottoSystemClient* pLottoSystem )
{
	if( m_pDrawingDateUIComp )
		m_pDrawingDateUIComp->UpdateDrawingDateProc( pLottoSystem );
}

void CMainUIPageBase::UpdateProgressTimeProc( const LottoSystem::CLottoSystemClient* pLottoSystem )
{
	if( m_pDrawingDateUIComp )
		m_pDrawingDateUIComp->UpdateProgressTimeProc( pLottoSystem );
}

void CMainUIPageBase::UpdateMachineTurn( const LottoSystem::CLottoSystemClient* pLottoSystem )
{
	if( m_pMachineViewerUIComp )
		m_pMachineViewerUIComp->UpdateTurn( pLottoSystem );
}

void CMainUIPageBase::UpdateMachine( const LottoSystem::CLottoSystemClient* pLottoSystem
									, int drawingNum )
{
	if( m_pMachineViewerUIComp )
		m_pMachineViewerUIComp->UpdateMachine( pLottoSystem, drawingNum );
}

void CMainUIPageBase::UpdateWinNum( const LottoSystem::CLottoSystemClient* pLottoSystem )
{
	if( m_pBuyListUIComp )
		m_pBuyListUIComp->UpdateWinNum( pLottoSystem );
}

void CMainUIPageBase::UpdateBuyList( const LottoSystem::CLottoSystemClient* pLottoSystem
									, int idx )
{
	if( m_pBuyListUIComp )
		m_pBuyListUIComp->UpdateBuyList( pLottoSystem, idx );
}

void CMainUIPageBase::UpdateBuyListCount( const LottoSystem::CLottoSystemClient* pLottoSystem, int count )
{
	if( m_pBuyListUIComp )
		m_pBuyListUIComp->UpdateBuyListCount( pLottoSystem, count );
}

void CMainUIPageBase::UpdateAccumulateMoney( const LottoSystem::CLottoSystemClient* pLottoSystem )
{
	if( m_pAccumulateMoneyUIComp )
		m_pAccumulateMoneyUIComp->UpdateAccumulateMoneyProc( pLottoSystem );
}

void CMainUIPageBase::UpdateMachineAnimation( float fElapsedTime )
{
	if( m_pMachineViewerUIComp )
		m_pMachineViewerUIComp->UpdateAnimation( fElapsedTime );
}