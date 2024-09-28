#include "stdafx.h"

#include "./LottoBuyListInfo.h"

#include "./LottoConfirmBuyListUIComp.h"
#include "./GLLottoSystemManUI.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/InterfaceBaseDefine.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"

#include "../../InnerInterface.h"

#include "../Util/RnButton.h"

#include "../../../SigmaCore/DebugInclude.h"

// Logic;
#include "../../../RanLogicClient/LottoSystem/GLLottoSystemManClient.h"
#include "../../../RanLogicClient/LottoSystem/GLLottoGlobalDataClient.h"

CLottoBuyListInfo::CLottoBuyListInfo( CInnerInterface* pInterface
									 , EngineDeviceMan* pEngineDevice )
									 : CUIWindowEx( pInterface, pEngineDevice )
									 , m_pBuyListUIComp( NULL )
{

}

CLottoBuyListInfo::~CLottoBuyListInfo() { }

void CLottoBuyListInfo::CreateSubControl()
{
	CreateLineBox( "LOTTO_BUY_LIST_INFO_REGION", "RAN_UI_BASIC_BACKGROUND_1" );

	// Buy List UI;
	{
		m_pBuyListUIComp = new CLottoConfirmBuyListUIComp(
			m_pInterface, m_pEngineDevice );
		if( m_pBuyListUIComp )
		{
			m_pBuyListUIComp->CreateSub( this, "LOTTO_BUY_LIST_INFO_BUY_LIST_REGION", UI_FLAG_DEFAULT );
			m_pBuyListUIComp->CreateSubControl();
			m_pBuyListUIComp->SetVisibleSingle( TRUE );
			RegisterControl( m_pBuyListUIComp );
		}
	}

	// Button;
	{
		const float eventPressedTime = 0.05f;

		RnButton::CreateArg arg;
		arg.defaultTextureKey = "RAN_RNBUTTON_DEFAULT_TEXTURE";
		arg.mouseOverTextureKey = "RAN_RNBUTTON_OVER_TEXTURE";
		arg.mouseClickTextureKey = "RAN_RNBUTTON_CLICK_TEXTURE";
		arg.pFont = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 8, _DEFAULT_FONT_SHADOW_FLAG );

		{
			RnButton* pButton = new RnButton( m_pEngineDevice );
			if( pButton )
			{
				pButton->CreateSub( this, "LOTTO_BUY_LIST_INFO_CONFIRM_BUTTON", UI_FLAG_DEFAULT, EConfirmButton );
				arg.text = ID2GAMEWORD( "LOTTO_BUY_LIST_INFO_BUTTON_TEXT", 0 );
				pButton->CreateSubControl( arg );
				pButton->SetPressedEventTime( eventPressedTime );
				RegisterControl( pButton );
			}
		}

		{
			RnButton* pButton = new RnButton( m_pEngineDevice );
			if( pButton )
			{
				pButton->CreateSub( this, "LOTTO_BUY_LIST_INFO_CLOSE_BUTTON", UI_FLAG_DEFAULT, ECloseButton );
				arg.text = ID2GAMEWORD( "LOTTO_BUY_LIST_INFO_BUTTON_TEXT", 1 );
				pButton->CreateSubControl( arg );
				pButton->SetPressedEventTime( eventPressedTime );
				RegisterControl( pButton );
			}
		}
	}
}

void CLottoBuyListInfo::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIWindowEx::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CLottoBuyListInfo::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	CUIWindowEx::TranslateUIMessage( ControlID, dwMsg );

	switch( ControlID )
	{
	case EConfirmButton:
		if( CHECK_MOUSEIN_LBUPLIKE(dwMsg) )
		{
			if( m_pBuyListUIComp )
				m_pBuyListUIComp->ConfirmRanking();
		}
		break;
	case ECloseButton:
		if( CHECK_MOUSEIN_LBUPLIKE(dwMsg) )
			m_pInterface->UiHideGroup( LOTTO_BUY_LIST_INFO );
		break;
	}
}

void CLottoBuyListInfo::SetVisibleSingle( BOOL bVisible )
{
	CUIWindowEx::SetVisibleSingle( bVisible );

	if( !bVisible )
	{
		if( m_pBuyListUIComp )
			m_pBuyListUIComp->CloseUI();
	}

	if( bVisible )
	{
		LottoSystem::CLottoSystemClient* pLottoSystem =
			LottoSystem::GLLottoSystemManClient::Instance()->GetLottoSystemUsingTabIdx(
			GLLottoSystemManUI::Instance()->GetCurTabIndex() );

		if( pLottoSystem )
		{
			UpdateBuyList( pLottoSystem );
			UpdateWinNumProc( pLottoSystem );
			UpdateChangeState( pLottoSystem );
		}
	}
}

void CLottoBuyListInfo::ChangeMainTab()
{
	LottoSystem::CLottoSystemClient* pLottoSystem =
		LottoSystem::GLLottoSystemManClient::Instance()->GetLottoSystemUsingTabIdx(
		GLLottoSystemManUI::Instance()->GetCurTabIndex() );

	UpdateBuyList( pLottoSystem );
	UpdateWinNumProc( pLottoSystem );
	UpdateChangeState( pLottoSystem );

	if( m_pBuyListUIComp )
		m_pBuyListUIComp->ConfirmStateOff();

	m_pInterface->UiHideGroup( LOTTO_BUY_LIST_INFO );
}

void CLottoBuyListInfo::UpdateBuyList( const LottoSystem::CLottoSystemClient* pLottoSystem
									  , int idx )
{
	if( !pLottoSystem )
		return;

	if( m_pBuyListUIComp )
		m_pBuyListUIComp->UpdateBuyList( pLottoSystem, idx );
}

void CLottoBuyListInfo::UpdateWinNumProc( const LottoSystem::CLottoSystemClient* pLottoSystem )
{
	if( !pLottoSystem )
		return;

	if( m_pBuyListUIComp )
		m_pBuyListUIComp->UpdateWinNum( pLottoSystem );
}

void CLottoBuyListInfo::UpdateChangeState( const LottoSystem::CLottoSystemClient* pLottoSystem )
{
	if( !pLottoSystem )
		return;

	if( m_pBuyListUIComp )
		m_pBuyListUIComp->UpdateChangeState( pLottoSystem );
}

void CLottoBuyListInfo::ConfirmRanking()
{
	if( m_pBuyListUIComp )
		m_pBuyListUIComp->ConfirmRanking();
}

void CLottoBuyListInfo::ConfirmStateOff()
{
	if( m_pBuyListUIComp )
		m_pBuyListUIComp->ConfirmStateOff();
}

MyLottoBuyListInfo::MyLottoBuyListInfo( CInnerInterface* pInterface
									   , EngineDeviceMan* pEngineDevice )
									   : CLottoBuyListInfo( pInterface
									   , pEngineDevice )
{
}

void MyLottoBuyListInfo::CreateUIWindowAndRegisterOwnership()
{
	CLottoBuyListInfo::Create( LOTTO_BUY_LIST_INFO, "BASIC_WINDOW", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	CLottoBuyListInfo::CreateBaseWindowLightGray( "LOTTO_BUY_LIST_INFO", (char*)ID2GAMEINTEXT("LOTTO_BUY_LIST_TITLE_TEXT") );
	CLottoBuyListInfo::CreateSubControl();
	CLottoBuyListInfo::SetAlignFlag( UI_FLAG_CENTER_Y | UI_FLAG_RIGHT );
	CLottoBuyListInfo::m_pInterface->UiRegisterControl( this, true );
	CLottoBuyListInfo::m_pInterface->UiShowGroupFocus( LOTTO_BUY_LIST_INFO );
}

bool MyLottoBuyListInfo::IsVisible()
{
	return CLottoBuyListInfo::IsVisible();
}

void MyLottoBuyListInfo::ChangeMainTab()
{
	CLottoBuyListInfo::ChangeMainTab();
}

void MyLottoBuyListInfo::UpdateBuyList( const LottoSystem::CLottoSystemClient* pLottoSystem )
{
	CLottoBuyListInfo::UpdateBuyList( pLottoSystem );
}

void MyLottoBuyListInfo::UpdateWinNumProc( const LottoSystem::CLottoSystemClient* pLottoSystem )
{
	CLottoBuyListInfo::UpdateWinNumProc( pLottoSystem );
}

void MyLottoBuyListInfo::UpdateChangeState( const LottoSystem::CLottoSystemClient* pLottoSystem )
{
	CLottoBuyListInfo::UpdateChangeState( pLottoSystem );
}

void MyLottoBuyListInfo::ConfirmRanking()
{
	CLottoBuyListInfo::ConfirmRanking();
}

void MyLottoBuyListInfo::ConfirmStateOff()
{
	CLottoBuyListInfo::ConfirmStateOff();
}