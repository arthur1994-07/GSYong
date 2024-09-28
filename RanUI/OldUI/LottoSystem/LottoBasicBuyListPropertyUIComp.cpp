#include "stdafx.h"

#include "./LottoBasicBuyListPropertyUIComp.h"

#include "./GLLottoSystemManUI.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicButton.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/SwapImage.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/DxTools/d3dfont.h"

#include "../../../RanLogic/GLogicEx.h"
#include "../../../RanLogic/GLUseFeatures.h"

#include "../../InnerInterface.h"

#include "../Util/RnButton.h"

#include "../../../SigmaCore/DebugInclude.h"

// Logic;
#include "../../../RanLogicClient/LottoSystem/GLLottoSystemManClient.h"
#include "../../../RanLogicClient/LottoSystem/GLLottoGlobalDataClient.h"

CLottoBasicBuyListPropertyUIComp::CLottoBasicBuyListPropertyUIComp( CInnerInterface* pInterface
																   , EngineDeviceMan* pEngineDevice )
																   : CLottoBuyListPropertyUIComp( pInterface
																   , pEngineDevice )
																   , m_pBuyButton( NULL )
{

}

CLottoBasicBuyListPropertyUIComp::~CLottoBasicBuyListPropertyUIComp() { }

void CLottoBasicBuyListPropertyUIComp::CreateSubControl()
{
	CLottoBuyListPropertyUIComp::CreateSubControl();

	// Button;
	{
		const float eventPressedTime = 0.05f;

		RnButton::CreateArg arg;
		arg.defaultTextureKey = "LOTTO_ICON";
		arg.mouseOverTextureKey = "LOTTO_ICON";
		arg.mouseClickTextureKey = "LOTTO_ICON";

		m_pBuyButton = new RnButton( m_pEngineDevice );
		if( m_pBuyButton )
		{
			m_pBuyButton->CreateSub( this, "LOTTO_BUY_LIST_COMP_PROPERTY_BUY_BUTTON", UI_FLAG_DEFAULT, EBuyBuyyon );
			m_pBuyButton->CreateSubControl( arg );
			m_pBuyButton->SetPressedEventTime( eventPressedTime );
			//m_pBuyButton->SetDiffuse( NS_UITEXTCOLOR::RED );
			m_pBuyButton->SetVisibleSingle( FALSE );
			RegisterControl( m_pBuyButton );
		}
	}
}

void CLottoBasicBuyListPropertyUIComp::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CLottoBuyListPropertyUIComp::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	LottoSystem::CLottoSystemClient* pLottoSystem =
		LottoSystem::GLLottoSystemManClient::Instance()->GetLottoSystemUsingTabIdx(
		GLLottoSystemManUI::Instance()->GetCurTabIndex() );
	if( pLottoSystem )
	{
		LottoSystem::EMLOTTO_STATE state = pLottoSystem->GetLottoSystemState();
		switch( state )
		{
		case LottoSystem::LOTTO_STATE_GATHERING:
		case LottoSystem::LOTTO_STATE_BASIC:
			if( GLLottoSystemManUI::Instance()->GetConfirmState() &&
				!pLottoSystem->GetLockBuyState() )
			{
				if( m_pBuyButton )
					m_pBuyButton->SetDiffuse( NS_UITEXTCOLOR::WHITE );
			}
			else
			{
				if( m_pBuyButton )
					m_pBuyButton->SetDiffuse( NS_UITEXTCOLOR::RED );
			}
			break;
		case LottoSystem::LOTTO_STATE_WAIT:
		case LottoSystem::LOTTO_STATE_DRAWING:
			if( m_pBuyButton )
				m_pBuyButton->SetDiffuse( NS_UITEXTCOLOR::RED );
			break;
		}
	}
}

void CLottoBasicBuyListPropertyUIComp::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	CLottoBuyListPropertyUIComp::TranslateUIMessage( ControlID, dwMsg );

	switch( ControlID )
	{
	case EBuyBuyyon:
		if( CHECK_MOUSEIN_LBUPLIKE(dwMsg) )
		{
			LottoSystem::CLottoSystemClient* pLottoSystem =
				LottoSystem::GLLottoSystemManClient::Instance()->GetLottoSystemUsingTabIdx(
				GLLottoSystemManUI::Instance()->GetCurTabIndex() );
			if( pLottoSystem )
			{
				LottoSystem::EMLOTTO_STATE state = pLottoSystem->GetLottoSystemState();
				switch( state )
				{
				case LottoSystem::LOTTO_STATE_BASIC:
				case LottoSystem::LOTTO_STATE_GATHERING:
					if( GLLottoSystemManUI::Instance()->GetConfirmState() )
					{
						LottoSystem::LOTTO_SYSTEM_CLIENT_VALUE sValue =
							LottoSystem::GLLottoGlobalDataClient::Instance()
							->GetLottoSystemValue(
							GLLottoSystemManUI::Instance()->GetCurTabIndex() );

						// 예외 파악;
						LottoSystem::CLottoSystemClient::BUY_LOTTO_FLAG buyFlag
							= pLottoSystem->BuyLottoTicket(
							GLLottoSystemManUI::Instance()->GetLottoNum(),
							GLLottoSystemManUI::Instance()->GetBuyType(),
							false );

						switch( buyFlag )
						{
						case LottoSystem::CLottoSystemClient::BUY_LOTTO_FLAG_LEVEL_FAIL:
							GLLottoSystemManUI::Instance()->OpenMessageBoxMainUI(
								CMessageBox::TITLE_ANNOUNCEMENT,
								sc::string::format( ID2GAMEINTEXT( "LOTTO_MAIN_UI_BUY_BUTTON_EXCEPTION_TEXT3" ),
								sValue.buyLevel ) );
							break;
						case LottoSystem::CLottoSystemClient::BUY_LOTTO_FLAG_MONEY_FAIL:
							GLLottoSystemManUI::Instance()->OpenMessageBoxMainUI(
								CMessageBox::TITLE_ANNOUNCEMENT,
								sc::string::format( ID2GAMEINTEXT( "LOTTO_MAIN_UI_BUY_BUTTON_EXCEPTION_TEXT4" ),
								NS_UITEXTCONTROL::MAKE_MONEY_FORMAT( sValue.buyInvenMoney, 3, "," ) ) );
							break;
						case LottoSystem::CLottoSystemClient::BUY_LOTTO_FLAG_LOCK_FAIL:
							GLLottoSystemManUI::Instance()->OpenMessageBoxMainUI(
								CMessageBox::TITLE_ANNOUNCEMENT,
								ID2GAMEINTEXT( "LOTTO_MAIN_UI_BUY_BUTTON_EXCEPTION_TEXT5" ) );
							break;
						default:
							// Show 로또 구매 UI;
							m_pInterface->UiShowGroupFocus( LOTTO_BUY_LOTTO_UI );
						}
					}
					else
					{
						GLLottoSystemManUI::Instance()->OpenMessageBoxMainUI(
							CMessageBox::TITLE_ANNOUNCEMENT,
							ID2GAMEINTEXT( "LOTTO_MAIN_UI_BUY_BUTTON_EXCEPTION_TEXT2" ) );
					}
					break;
				case LottoSystem::LOTTO_STATE_WAIT:
					GLLottoSystemManUI::Instance()->OpenMessageBoxMainUI(
						CMessageBox::TITLE_ANNOUNCEMENT,
						ID2GAMEINTEXT( "LOTTO_MAIN_UI_BUY_BUTTON_EXCEPTION_TEXT" ) );
					break;
				case LottoSystem::LOTTO_STATE_DRAWING:
					GLLottoSystemManUI::Instance()->OpenMessageBoxMainUI(
						CMessageBox::TITLE_ANNOUNCEMENT,
						ID2GAMEINTEXT( "LOTTO_MAIN_UI_BUY_BUTTON_EXCEPTION_TEXT1" ) );

					/*m_pGaeaClient->PrintMsgText(
						NS_UITEXTCOLOR::WHITE,
						ID2GAMEINTEXT( "LOTTO_MAIN_UI_BUY_BUTTON_EXCEPTION_TEXT" ) );*/
					break;
				}
			}
		}
		break;
	}
}

void CLottoBasicBuyListPropertyUIComp::SetVisibleSingle( BOOL bVisible )
{
	if( bVisible )
	{
		LottoSystem::CLottoSystemClient* pLottoSystem =
			LottoSystem::GLLottoSystemManClient::Instance()->GetLottoSystemUsingTabIdx(
			GLLottoSystemManUI::Instance()->GetCurTabIndex() );
		if( pLottoSystem )
		{
			LottoSystem::EMLOTTO_STATE state = pLottoSystem->GetLottoSystemState();
			switch( state )
			{
			case LottoSystem::LOTTO_STATE_BASIC:
			case LottoSystem::LOTTO_STATE_GATHERING:
				if( GLLottoSystemManUI::Instance()->GetConfirmState() &&
					!pLottoSystem->GetLockBuyState() )
				{
					if( m_pBuyButton )
						m_pBuyButton->SetDiffuse( NS_UITEXTCOLOR::WHITE );
				}
				else
				{
					if( m_pBuyButton )
						m_pBuyButton->SetDiffuse( NS_UITEXTCOLOR::RED );
				}
				break;
			case LottoSystem::LOTTO_STATE_WAIT:
			case LottoSystem::LOTTO_STATE_DRAWING:
				if( m_pBuyButton )
					m_pBuyButton->SetDiffuse( NS_UITEXTCOLOR::RED );
				break;
			}
		}
	}

	CLottoBuyListPropertyUIComp::SetVisibleSingle( bVisible );
}

void CLottoBasicBuyListPropertyUIComp::SetPropertyState( CLottoBuyListPropertyUIComp::PROPERTY_STATE state )
{
	CLottoBuyListPropertyUIComp::SetPropertyState( state );

	m_pBuyButton->SetVisibleSingle( FALSE );

	switch( m_state )
	{
	case ESTATE_ACTIVE:
		break;
	case ESTATE_INACTIVE:
		m_pBuyButton->SetVisibleSingle( TRUE );
		break;
	}
}