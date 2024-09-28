#include "stdafx.h"

#include "./LottoBuyListUIComp.h"
#include "./LottoWinNumUIComp.h"

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

CLottoBuyListUIComp::CLottoBuyListUIComp( CInnerInterface* pInterface
										 , EngineDeviceMan* pEngineDevice )
										 : CUIGroupHelper( pEngineDevice )
										 , m_pInterface( pInterface )
										 , m_pWinNum( NULL )
										 , m_pBuyListTextBox( NULL )
										 , m_pWinNumTextBox( NULL )
										 , m_lottoSystemId( 0 )
{
}

CLottoBuyListUIComp::~CLottoBuyListUIComp() { }

void CLottoBuyListUIComp::CreateSubControl()
{
	// Line Box;
	{
		CreateLineBox( "LOTTO_BUY_LIST_COMP_BUY_LIST_TEXT_REGION", "WHITE_ROUNDED_LINE_DARKGRAY_BACKGROUND" );
		CreateLineBox( "LOTTO_BUY_LIST_COMP_BUY_LIST_REGION", "WHITE_ROUNDED_LINE_DARKGRAY_BACKGROUND" );
		CreateLineBox( "LOTTO_BUY_LIST_COMP_MAIN_NUM_TEXT_REGION", "WHITE_ROUNDED_LINE_DARKGRAY_BACKGROUND" );
		CreateLineBox( "LOTTO_BUY_LIST_COMP_BONUS_NUM_TEXT_REGION", "WHITE_ROUNDED_LINE_DARKGRAY_BACKGROUND" );
		CreateLineBox( "LOTTO_BUY_LIST_COMP_MAIN_NUM_REGION", "WHITE_ROUNDED_LINE_DARKGRAY_BACKGROUND" );
		CreateLineBox( "LOTTO_BUY_LIST_COMP_BONUS_NUM_REGION", "WHITE_ROUNDED_LINE_DARKGRAY_BACKGROUND" );
	}

	// Text Box;
	{
		CBasicTextBox* pTextBox = new CBasicTextBox( m_pEngineDevice );
		pTextBox->CreateSub ( this, "LOTTO_BUY_LIST_COMP_BONUS_TEXTBOX", UI_FLAG_YSIZE );
		pTextBox->SetFont ( DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 8, _DEFAULT_FONT_BOLD_FLAG ) );
		pTextBox->SetTextAlign( TEXT_ALIGN_CENTER_Y|TEXT_ALIGN_LEFT );
		pTextBox->AddText(
			ID2GAMEINTEXT( "LOTTO_BONUS_TEXT" ),
			NS_UITEXTCOLOR::WHITE );
		RegisterControl ( pTextBox );
	}

	// Changable Text Box;
	{
		m_pBuyListTextBox = new CBasicTextBox( m_pEngineDevice );
		m_pBuyListTextBox->CreateSub ( this, "LOTTO_BUY_LIST_COMP_BUY_LIST_TEXTBOX", UI_FLAG_YSIZE );
		m_pBuyListTextBox->SetFont ( DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 8, _DEFAULT_FONT_BOLD_FLAG ) );
		m_pBuyListTextBox->SetTextAlign( TEXT_ALIGN_CENTER_Y|TEXT_ALIGN_LEFT );
		m_pBuyListTextBox->AddText(
			sc::string::format( ID2GAMEINTEXT( "LOTTO_MY_BUY_LIST_TEXT" ), 0, 10 ).c_str(),
			NS_UITEXTCOLOR::WHITE );
		RegisterControl ( m_pBuyListTextBox );

		m_pWinNumTextBox = new CBasicTextBox( m_pEngineDevice );
		m_pWinNumTextBox->CreateSub ( this, "LOTTO_BUY_LIST_COMP_WIN_NUM_TEXTBOX", UI_FLAG_YSIZE );
		m_pWinNumTextBox->SetFont ( DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 8, _DEFAULT_FONT_BOLD_FLAG ) );
		m_pWinNumTextBox->SetTextAlign( TEXT_ALIGN_CENTER_Y|TEXT_ALIGN_CENTER_X );
		m_pWinNumTextBox->AddText(
			sc::string::format( ID2GAMEINTEXT( "LOTTO_WIN_NUM_TEXT" ), 1 ).c_str(),
			NS_UITEXTCOLOR::WHITE );
		RegisterControl ( m_pWinNumTextBox );
	}

	// Win Num UI Component;
	{
		m_pWinNum = new CLottoWinNumUIComp( m_pInterface, m_pEngineDevice );
		if( m_pWinNum )
		{
			m_pWinNum->CreateSub( this, "LOTTO_BUY_LIST_COMP_WIN_NUM_REGION", UI_FLAG_DEFAULT );
			m_pWinNum->CreateSubControl();
			RegisterControl( m_pWinNum );
		}
	}
}

void CLottoBuyListUIComp::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroupHelper::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CLottoBuyListUIComp::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	CUIGroupHelper::TranslateUIMessage( ControlID, dwMsg );
}