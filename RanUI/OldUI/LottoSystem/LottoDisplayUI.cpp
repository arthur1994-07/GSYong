#include "stdafx.h"

#include "./LottoDisplayUI.h"

#include "./GLLottoSystemManUI.h"

#include "../../../SigmaCore/String/StringFormat.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/UITextUtil.h"
#include "../../../EngineLib/GUInterface/InterfaceBaseDefine.h"
#include "../../../EngineLib/GUInterface/SwapImage.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"

#include "../Util/UIMoveableTextBox.h"

#include "../../InnerInterface.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

// Logic;
#include "../../../RanLogicClient/LottoSystem/GLLottoSystemManClient.h"
#include "../../../RanLogicClient/LottoSystem/GLLottoGlobalDataClient.h"

CLottoDisplayUI::CLottoDisplayUI( CInnerInterface* pInterface
								 , EngineDeviceMan* pEngineDevice )
								 : CUIGroupHelperNonClose( pEngineDevice
								 , pInterface )
								 , m_pMoveableTextBox( NULL )
								 , m_fTime( 0.f )
{
}

CLottoDisplayUI::~CLottoDisplayUI () { }

void CLottoDisplayUI::CreateSubControl ()
{
	CreateLineBox( "LOTTO_DISPLAY_UI_REGION", "BLACK_LINE_DARKGRAY_BACKGROUND" );

	m_pMoveableTextBox = new CMoveableTextBox(
		m_pInterface, m_pEngineDevice );
	m_pMoveableTextBox->CreateSub( this, "LOTTO_DISPLAY_UI_REGION", UI_FLAG_YSIZE );
	m_pMoveableTextBox->CreateSubControl(
		"LOTTO_DISPLAY_UI_REGION",
		DxFontMan::GetInstance().LoadDxFont( _DEFAULT_FONT, 15, _DEFAULT_FONT_SHADOW_EX_FLAG ),
		TEXT_ALIGN_CENTER_Y|TEXT_ALIGN_RIGHT );
	m_pMoveableTextBox->SetText( "" );
	m_pMoveableTextBox->SetTextColor( NS_UITEXTCOLOR::WHITE );
	m_pMoveableTextBox->SetTextSpeed( 0.2f );
	RegisterControl( m_pMoveableTextBox );
}

void CLottoDisplayUI::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroupHelperNonClose::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	unsigned int tabIdx =
		GLLottoSystemManUI::Instance()->GetAccumulateMoneyLED();

	LottoSystem::CLottoSystemClient* pLottoSystem =
		LottoSystem::GLLottoSystemManClient::Instance()->GetLottoSystemUsingTabIdx( tabIdx );
	if( pLottoSystem )
	{
		LottoSystem::LOTTO_SYSTEM_CLIENT_VALUE sValue =
			LottoSystem::GLLottoGlobalDataClient::Instance()
			->GetLottoSystemValue( pLottoSystem->GetLottoSystemId() );

		std::string strAccumulateMoney =
			NS_UITEXTCONTROL::MAKE_MONEY_FORMAT(
			pLottoSystem->GetAccumulateMoney(), 3, "," );

		std::string strWinNum;
		LottoSystem::LOTTO_SYSTEM_LOTTO_NUM winNum =
			pLottoSystem->GetWinnerNum();
		for( unsigned int i=0; i<sValue.nDrawingTotal; ++i )
		{
			if( winNum.lottoNum[ i ] == 0 )
			{
				strWinNum = "-";
				break;
			}

			if( i == sValue.nDrawingMain )
				strWinNum += "| ";

			strWinNum += sc::string::format( "%d ", winNum.lottoNum[ i ] );
		}

		std::string outputString;
		switch( pLottoSystem->GetLottoSystemState() )
		{
		case LottoSystem::LOTTO_STATE_GATHERING:
			{
				outputString = sc::string::format(
					ID2GAMEINTEXT("LOTTO_DISPLAY_UI_EXIT_STATE_TEXT"),
					pLottoSystem->GetCurTurnNum(),
					pLottoSystem->GetCurTurnNum()+1,
					strWinNum );
			}
			break;
		case LottoSystem::LOTTO_STATE_BASIC:
			{	
				outputString = sc::string::format(
					ID2GAMEINTEXT("LOTTO_DISPLAY_UI_BASIC_STATE_TEXT"),
					pLottoSystem->GetCurTurnNum()+1,
					pLottoSystem->GetCurTurnNum(),
					strAccumulateMoney,
					strWinNum );
			}
			break;
		case LottoSystem::LOTTO_STATE_WAIT:
			{
				outputString = sc::string::format(
					ID2GAMEINTEXT("LOTTO_DISPLAY_UI_WAIT_STATE_TEXT"),
					pLottoSystem->GetCurTurnNum()+1, strAccumulateMoney );
			}
			break;
		case LottoSystem::LOTTO_STATE_DRAWING:
			{
				outputString = sc::string::format(
					ID2GAMEINTEXT("LOTTO_DISPLAY_UI_DRAWING_STATE_TEXT"),
					pLottoSystem->GetCurTurnNum()+1, strAccumulateMoney );
			}
			break;
		}

		if( m_pMoveableTextBox )
			m_pMoveableTextBox->SetText( outputString );
	}

	if( GLLottoSystemManUI::Instance()->GetAccumulateMoneyLED()
		== 0xffffffff )
		m_pInterface->UiHideGroup( LOTTO_DISPLAY_UI );
}

void CLottoDisplayUI::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{	
	CUIGroupHelperNonClose::TranslateUIMessage( ControlID, dwMsg );
}

void CLottoDisplayUI::UpdateAccumulateMoney( const LottoSystem::CLottoSystemClient* pLottoSystem )
{
	if( !pLottoSystem )
		return;

	unsigned int ledTabIdx =
		GLLottoSystemManUI::Instance()->GetAccumulateMoneyLED();

	bool bOk = false;

	LottoSystem::LOTTO_SYSTEM_CLIENT_VALUE sValue = 
		LottoSystem::GLLottoGlobalDataClient::Instance()
		->GetLottoSystemValue( static_cast<unsigned int>(
		pLottoSystem->GetLottoSystemId() ) );
	int tabIdx = sValue.useIndex;

	if( ledTabIdx == static_cast<unsigned int>( tabIdx ) )
		bOk = true;

	if( bOk )
	{
		std::string strAccumulateMoney =
			NS_UITEXTCONTROL::MAKE_MONEY_FORMAT(
			pLottoSystem->GetAccumulateMoney(), 3, "," );

		std::string strWinNum;
		LottoSystem::LOTTO_SYSTEM_LOTTO_NUM winNum =
			pLottoSystem->GetWinnerNum();
		for( unsigned int i=0; i<sValue.nDrawingTotal; ++i )
		{
			if( winNum.lottoNum[ i ] == 0 )
			{
				strWinNum = "-";
				break;
			}

			if( i == sValue.nDrawingMain )
				strWinNum += "| ";

			strWinNum += sc::string::format( "%d ", winNum.lottoNum[ i ] );
		}

		std::string outputString;
		switch( pLottoSystem->GetLottoSystemState() )
		{
		case LottoSystem::LOTTO_STATE_GATHERING:
			{
				outputString = sc::string::format(
					ID2GAMEINTEXT("LOTTO_DISPLAY_UI_EXIT_STATE_TEXT"),
					pLottoSystem->GetCurTurnNum(),
					pLottoSystem->GetCurTurnNum()+1,
					strWinNum );
			}
			break;
		case LottoSystem::LOTTO_STATE_BASIC:
			{	
				outputString = sc::string::format(
					ID2GAMEINTEXT("LOTTO_DISPLAY_UI_BASIC_STATE_TEXT"),
					pLottoSystem->GetCurTurnNum()+1,
					pLottoSystem->GetCurTurnNum(),
					strAccumulateMoney,
					strWinNum );
			}
			break;
		case LottoSystem::LOTTO_STATE_WAIT:
			{
				outputString = sc::string::format(
					ID2GAMEINTEXT("LOTTO_DISPLAY_UI_WAIT_STATE_TEXT"),
					pLottoSystem->GetCurTurnNum()+1, strAccumulateMoney );
			}
			break;
		case LottoSystem::LOTTO_STATE_DRAWING:
			{
				outputString = sc::string::format(
					ID2GAMEINTEXT("LOTTO_DISPLAY_UI_DRAWING_STATE_TEXT"),
					pLottoSystem->GetCurTurnNum()+1, strAccumulateMoney );
			}
			break;
		}

		if( m_pMoveableTextBox )
			m_pMoveableTextBox->SetText( outputString );
	}
}

void CLottoDisplayUI::ResetTextAnimation()
{
	if( m_pMoveableTextBox )
		m_pMoveableTextBox->ResetTextAnimation();
}

MyLottoDisplayUI::MyLottoDisplayUI( CInnerInterface* pInterface
								   , EngineDeviceMan* pEngineDevice )
								   : CLottoDisplayUI( pInterface
								   , pEngineDevice )
{

}

void MyLottoDisplayUI::CreateUIWindowAndRegisterOwnership()
{
	CLottoDisplayUI::Create( LOTTO_DISPLAY_UI, "LOTTO_DISPLAY_UI", UI_FLAG_RIGHT|UI_FLAG_TOP );
	CLottoDisplayUI::CreateLightGrayMove( "LOTTO_DISPLAY_UI_REGION" );
	CLottoDisplayUI::CreateSubControl();
	CLottoDisplayUI::m_pInterface->UiRegisterControl( this );
	CLottoDisplayUI::m_pInterface->UiShowGroupFocus( LOTTO_DISPLAY_UI );
}

bool MyLottoDisplayUI::IsVisible()
{
	return CLottoDisplayUI::IsVisible();
}

void MyLottoDisplayUI::UpdateAccumulateMoney( const LottoSystem::CLottoSystemClient* pLottoSystem )
{
	CLottoDisplayUI::UpdateAccumulateMoney( pLottoSystem );
}

void MyLottoDisplayUI::ResetTextAnimation()
{
	CLottoDisplayUI::ResetTextAnimation();
}