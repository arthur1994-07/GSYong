#include "stdafx.h"

#include "./LottoNotifyButton.h"

#include "./GLLottoSystemManUI.h"

#include "../../../SigmaCore/String/StringFormat.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/InterfaceBaseDefine.h"
#include "../../../EngineLib/GUInterface/SwapImage.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"

#include "../../InnerInterface.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

// Logic;
#include "../../../RanLogicClient/LottoSystem/GLLottoSystemManClient.h"
#include "../../../RanLogicClient/LottoSystem/GLLottoGlobalDataClient.h"

const float CLottoNotifyButton::fDESC_ANIMATION_SPEED_DELAY = 0.5f;

CLottoNotifyButton::CLottoNotifyButton( CInnerInterface* pInterface
									   , EngineDeviceMan* pEngineDevice )
									   : CUIGroup( pEngineDevice )
									   , m_pInterface( pInterface )
									   , m_pLottoImage( NULL )
									   , m_pStateImage( NULL )
									   , m_pCountTextBoxBackgroundImage( NULL )
									   , m_pCountTextBox( NULL )
									   , m_type( ENOTIFY_TYPE_NONE )
									   , m_bRead( false )
									   , m_curLottoSystemId( 0 )
{

}

CLottoNotifyButton::~CLottoNotifyButton () { }

void CLottoNotifyButton::CreateSubControl ()
{
	// 기본 버튼 UI;
	m_pLottoImage = new CSwapImage( m_pEngineDevice );
	m_pLottoImage->CreateSub ( this, "LOTTO_NOTIFY_BUTTON_REGION", UI_FLAG_DEFAULT, ENotifyButton );
	m_pLottoImage->SetImage( "LOTTO_MAINICON_GRAY" );
	m_pLottoImage->SetVisibleSingle( TRUE );
	RegisterControl( m_pLottoImage );

	// 대기 상태에 표시될 UI;
	m_pStateImage = new CSwapImage( m_pEngineDevice );
	m_pStateImage->CreateSub ( this, "LOTTO_NOTIFY_BUTTON_REGION", UI_FLAG_DEFAULT );
	m_pStateImage->SetImage( "LOTTO_TIME" );
	m_pStateImage->SetVisibleSingle( TRUE );
	RegisterControl( m_pStateImage );

	// 확인 안한 이전 구매리스트를 위한 UI;
	m_pCountTextBoxBackgroundImage = new CSwapImage( m_pEngineDevice );
	m_pCountTextBoxBackgroundImage->CreateSub ( this, "POSTNOTIFY_BUTTON_BACKGROUND_IMAGE", UI_FLAG_DEFAULT );
	m_pCountTextBoxBackgroundImage->SetImage( "POSTNOTIFY_BUTTON_BACKGROUND_IMAGE_RED" );
	RegisterControl ( m_pCountTextBoxBackgroundImage );

	m_pCountTextBox = new CBasicTextBox( m_pEngineDevice );
	m_pCountTextBox->CreateSub ( this, "POSTNOTIFY_BUTTON_TEXT", UI_FLAG_DEFAULT );
	m_pCountTextBox->SetFont ( m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 8, _DEFAULT_FONT_FLAG ) );
	m_pCountTextBox->SetTextAlign ( TEXT_ALIGN_CENTER_X|TEXT_ALIGN_CENTER_Y );	
	RegisterControl ( m_pCountTextBox );
}

void CLottoNotifyButton::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	// 확인하지 않은 이전 구매리스트의 개수 표시;
	int nNonCofirmBuyList =
		LottoSystem::GLLottoSystemManClient::Instance()
		->GetConfirmBuyListCount();

	if( nNonCofirmBuyList > 0 )
		m_bRead = true;
	else
		m_bRead = false;

	if( m_bRead )
	{
		m_pCountTextBox->SetOneLineText(
			sc::string::format( "%d", nNonCofirmBuyList ).c_str() );

		m_pLottoImage->SetImage( "LOTTO_MAINICON_COLOR" );
		m_pCountTextBox->SetVisibleSingle( TRUE );
		m_pCountTextBoxBackgroundImage->SetVisibleSingle( TRUE );
	}
	else
	{
		m_pLottoImage->SetImage( "LOTTO_MAINICON_GRAY" );
		m_pCountTextBox->SetVisibleSingle( FALSE );
		m_pCountTextBoxBackgroundImage->SetVisibleSingle( FALSE );
	}

	// 각 상태별로 다른 Image 표시;
	unsigned int lottoSystemCount =LottoSystem::GLLottoSystemManClient::Instance()
		->GetLottoSystemCount();
	LottoSystem::EMLOTTO_STATE state = LottoSystem::LOTTO_STATE_BASIC;
	for( unsigned int i=0; i<lottoSystemCount; ++i )
	{
		LottoSystem::CLottoSystemClient* pLottoSystem =
			LottoSystem::GLLottoSystemManClient::Instance()
			->GetLottoSystem( i );
		if( pLottoSystem )
		{
			if( state < pLottoSystem->GetLottoSystemState() &&
				pLottoSystem->GetLottoSystemState() !=
				LottoSystem::LOTTO_STATE_GATHERING )
				m_curLottoSystemId = pLottoSystem->GetLottoSystemId();
		}
	}

	LottoSystem::CLottoSystemClient* pLottoSystem =
		LottoSystem::GLLottoSystemManClient::Instance()
		->GetLottoSystem( m_curLottoSystemId );
	unsigned int turnNum = 0;
	unsigned int minute = 0;
	if( pLottoSystem )
	{
		state = pLottoSystem->GetLottoSystemState();
		turnNum = pLottoSystem->GetCurTurnNum();
		LottoSystem::LOTTO_DRAWING_DATE progressState = 
			pLottoSystem->GetCurProgressTime();
		minute = progressState.GetMinute();
	}
	
	static float fTime = 0.f;
	switch( state )
	{
	case LottoSystem::LOTTO_STATE_GATHERING:
	case LottoSystem::LOTTO_STATE_BASIC:
		m_type = ENOTIFY_TYPE_NONE;
		m_pStateImage->SetVisibleSingle( FALSE );

		// 툴팁 세팅;
		m_strTooltip = sc::string::format(
			ID2GAMEINTEXT( "LOTTO_NOTIFY_BUTTON_UI_BASIC_STATE_TEXT" ),
			turnNum+1 );
		break;
	case LottoSystem::LOTTO_STATE_WAIT:
		m_type = ENOTIFY_TYPE_WAIT_STATE;

		m_pStateImage->SetVisibleSingle( TRUE );

		// 툴팁 세팅;
		m_strTooltip = sc::string::format(
			ID2GAMEINTEXT( "LOTTO_NOTIFY_BUTTON_UI_WAIT_STATE_TEXT" ),
			turnNum+1, minute );
		break;
	case LottoSystem::LOTTO_STATE_DRAWING:
		m_type = ENOTIFY_TYPE_DRAWING_STATE;

		fTime += fElapsedTime;
		if( fTime >= fDESC_ANIMATION_SPEED_DELAY )
		{
			fTime = 0.f;

			if( m_bRead )
			{
				m_pStateImage->SetImage( "LOTTO_MAINICON_COLOR_LINE" );
				m_pStateImage->SetVisibleSingle( !m_pStateImage->IsVisible() );
			}
			else
			{
				m_pStateImage->SetImage( "LOTTO_MAINICON_GRAY_LINE" );
				m_pStateImage->SetVisibleSingle( !m_pStateImage->IsVisible() );
			}
		}

		// 툴팁 세팅;
		m_strTooltip = sc::string::format(
			ID2GAMEINTEXT( "LOTTO_NOTIFY_BUTTON_UI_DRAWING_STATE_TEXT" ),
			turnNum+1 );
		break;
	}

	// 확인 안한 Ticket이 있을 경우;
	if( m_bRead )
	{
		m_strTooltip =
			ID2GAMEINTEXT( "LOTTO_MAIN_UI_BUY_BUTTON_EXCEPTION_TEXT2" );
	}
}

void CLottoNotifyButton::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	CUIGroup::TranslateUIMessage( ControlID, dwMsg );

	switch ( ControlID )
	{
	case ENotifyButton:
		if ( CHECK_MOUSE_IN ( dwMsg ) )
		{
			m_pInterface->SHOW_COMMON_LINEINFO(
				m_strTooltip.c_str(),
				NS_UITEXTCOLOR::WHITE,
				TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP );         

			if ( CHECK_LB_UP_LIKE ( dwMsg ) )
			{
				if( m_pInterface->UiIsVisibleGroup( LOTTO_MAIN_UI ) )
					m_pInterface->UiHideGroup( LOTTO_MAIN_UI );
				else
					m_pInterface->UiShowGroupFocus( LOTTO_MAIN_UI );
			}
		}
		break;
	}
}

MyLottoNotifyButton::MyLottoNotifyButton( CInnerInterface* pInterface
										 , EngineDeviceMan* pEngineDevice )
										 : CLottoNotifyButton( pInterface
										 , pEngineDevice )
{

}

void MyLottoNotifyButton::CreateUIWindowAndRegisterOwnership()
{
	CLottoNotifyButton::Create( LOTTO_NOTIFY_BUTTON, "LOTTO_NOTIFY_BUTTON", UI_FLAG_RIGHT | UI_FLAG_BOTTOM );
	CLottoNotifyButton::CreateSubControl();
	CLottoNotifyButton::m_pInterface->UiRegisterControl( this );
	CLottoNotifyButton::m_pInterface->UiShowGroupBottom( LOTTO_NOTIFY_BUTTON );
}

bool MyLottoNotifyButton::IsVisible()
{
	return CLottoNotifyButton::IsVisible();
}