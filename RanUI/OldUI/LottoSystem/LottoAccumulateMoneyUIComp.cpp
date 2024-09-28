#include "stdafx.h"

#include "./LottoAccumulateMoneyUIComp.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/DxTools/d3dfont.h"

#include "../../../RanLogic/GLogicEx.h"
#include "../../../RanLogic/GLUseFeatures.h"

#include "../../InnerInterface.h"

#include "../../../SigmaCore/DebugInclude.h"

#include "../../../RanLogicClient/LottoSystem/GLLottoGlobalDataClient.h"
#include "../../../RanLogicClient/LottoSystem/GLLottoSystemManClient.h"

CLottoAccumulateMoneyUIComp::CLottoAccumulateMoneyUIComp( CInnerInterface* pInterface
														 , EngineDeviceMan* pEngineDevice )
														 : CUIGroupHelper( pEngineDevice )
														 , m_pInterface( pInterface )
{

}

CLottoAccumulateMoneyUIComp::~CLottoAccumulateMoneyUIComp() { }

void CLottoAccumulateMoneyUIComp::CreateSubControl()
{
	// Line Box;
	{
		CreateLineBox( "LOTTO_ACCUMULATE_MONEY_UI_COMP_REGION", "BLACK_LINE_DARKGRAY_BACKGROUND" );
	}

	// Text Box;
	{
		m_pAccumulateMoneyTextTextBox = new CBasicTextBox( m_pEngineDevice );
		m_pAccumulateMoneyTextTextBox->CreateSub ( this, "LOTTO_ACCUMULATE_MONEY_UI_COMP_TEXT_TEXTBOX", UI_FLAG_YSIZE );
		m_pAccumulateMoneyTextTextBox->SetFont ( DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_BOLD_FLAG ) );
		m_pAccumulateMoneyTextTextBox->SetTextAlign( TEXT_ALIGN_CENTER_Y|TEXT_ALIGN_RIGHT );
		m_pAccumulateMoneyTextTextBox->AddText(
			sc::string::format( ID2GAMEINTEXT( "LOTTO_MAIN_UI_ACCUMULATE_MONEY_TEXT" ), "" ).c_str(),
			NS_UITEXTCOLOR::BLACK );
		RegisterControl ( m_pAccumulateMoneyTextTextBox );

		m_pAccumulateMoneyTextBox = new CBasicTextBox( m_pEngineDevice );
		m_pAccumulateMoneyTextBox->CreateSub ( this, "LOTTO_ACCUMULATE_MONEY_UI_COMP_TEXTBOX", UI_FLAG_YSIZE );
		m_pAccumulateMoneyTextBox->SetFont ( DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 15, _DEFAULT_FONT_SHADOW_EX_FLAG ) );
		m_pAccumulateMoneyTextBox->SetTextAlign( TEXT_ALIGN_CENTER_Y|TEXT_ALIGN_RIGHT );
		m_pAccumulateMoneyTextBox->AddText(
			"0", NS_UITEXTCOLOR::WHITE );
		RegisterControl ( m_pAccumulateMoneyTextBox );
	}
}

void CLottoAccumulateMoneyUIComp::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroupHelper::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CLottoAccumulateMoneyUIComp::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	CUIGroupHelper::TranslateUIMessage( ControlID, dwMsg );
}

void CLottoAccumulateMoneyUIComp::UpdateTurnProc( const LottoSystem::CLottoSystemClient* pLottoSystem )
{
	if( !pLottoSystem )
		return;

	if( m_pAccumulateMoneyTextTextBox )
	{
		m_pAccumulateMoneyTextTextBox->SetOneLineText(
			sc::string::format( ID2GAMEINTEXT( "LOTTO_MAIN_UI_ACCUMULATE_MONEY_TEXT" ),
			sc::string::format( ID2GAMEINTEXT( "LOTTO_WIN_MAN_LIST_TURN_TEXT" ),
			pLottoSystem->GetCurTurnNum()+1 ) ).c_str(),
			NS_UITEXTCOLOR::BLACK );
	}
}

void CLottoAccumulateMoneyUIComp::UpdateAccumulateMoneyProc( const LottoSystem::CLottoSystemClient* pLottoSystem )
{
	if( !pLottoSystem )
		return;

	if( pLottoSystem->GetLottoSystemState() ==
		LottoSystem::LOTTO_STATE_GATHERING )
	{
		std::string strText = ID2GAMEINTEXT( "LOTTO_MAIN_UI_ACCUMULATE_MONEY_TEXT1" );
		m_pAccumulateMoneyTextBox->SetOneLineText( strText.c_str() );
	}
	else
	{
		LONGLONG accumulateMoney = pLottoSystem->GetAccumulateMoney();
		CString strText = NS_UITEXTCONTROL::MAKE_MONEY_FORMAT( accumulateMoney, 3, "," );
		m_pAccumulateMoneyTextBox->SetOneLineText( strText );
	}
}