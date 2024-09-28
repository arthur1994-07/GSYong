#include "stdafx.h"

#include "./LottoWinManListInfo.h"

#include "./LottoWinNumUIComp.h"
#include "./LottoWinManListUIComp.h"

#include "./GLLottoSystemManUI.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/InterfaceBaseDefine.h"
#include "../../../EngineLib/GUInterface/SwapImage.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"

#include "../../InnerInterface.h"

#include "../Util/RnButton.h"
#include "../Util/SmartComboBoxEx.h"

#include "../../../SigmaCore/DebugInclude.h"

// Logic;
#include "../../../RanLogicClient/LottoSystem/GLLottoSystemManClient.h"
#include "../../../RanLogicClient/LottoSystem/GLLottoGlobalDataClient.h"

CLottoWinManListInfo::CLottoWinManListInfo( CInnerInterface* pInterface
										   , EngineDeviceMan* pEngineDevice )
										   : CUIWindowEx( pInterface
										   , pEngineDevice )
										   , m_pAccumulateMoneyTextBox( NULL )
										   , m_pWinNum( NULL )
										   , m_pWinManListComp( NULL )
										   , m_pComboBox( NULL )
{

}

void CLottoWinManListInfo::CreateSubControl()
{
	// Line Box;
	{
		CreateLineBox( "LOTTO_WIN_MAN_LIST_INFO_REGION", "WHITE_LINE_GRAY_BACKGROUND" );
		CreateLineBox( "LOTTO_WIN_MAN_LIST_INFO_ACCUMULTATE_MONEY_TEXTBOX_REGION", "WHITE_ROUNDED_LINE_DARKGRAY_BACKGROUND" );
	}

	// Text Box;
	{
		{
			CBasicTextBox* pTextBox = new CBasicTextBox( m_pEngineDevice );
			pTextBox->CreateSub ( this, "LOTTO_WIN_MAN_LIST_INFO_ACCUMULTATE_MONEY_TEXT_TEXTBOX", UI_FLAG_YSIZE );
			pTextBox->SetFont ( DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_BOLD_FLAG ) );
			pTextBox->SetTextAlign( TEXT_ALIGN_CENTER_Y|TEXT_ALIGN_RIGHT );
			pTextBox->AddText(
				sc::string::format( ID2GAMEINTEXT( "LOTTO_MAIN_UI_ACCUMULATE_MONEY_TEXT" ), "" ).c_str(),
				NS_UITEXTCOLOR::BLACK );
			RegisterControl ( pTextBox );
		}

		m_pAccumulateMoneyTextBox = new CBasicTextBox( m_pEngineDevice );
		m_pAccumulateMoneyTextBox->CreateSub ( this, "LOTTO_WIN_MAN_LIST_INFO_ACCUMULTATE_MONEY_TEXTBOX", UI_FLAG_YSIZE );
		m_pAccumulateMoneyTextBox->SetFont ( DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 13, _DEFAULT_FONT_BOLD_FLAG ) );
		m_pAccumulateMoneyTextBox->SetTextAlign( TEXT_ALIGN_CENTER_Y|TEXT_ALIGN_RIGHT );
		m_pAccumulateMoneyTextBox->AddText(
			"0", NS_UITEXTCOLOR::WHITE );
		RegisterControl ( m_pAccumulateMoneyTextBox );
	}

	// Win Num UI Component;
	{
		m_pWinNum = new CLottoWinNumUIComp( m_pInterface, m_pEngineDevice );
		if( m_pWinNum )
		{
			m_pWinNum->CreateSub( this, "LOTTO_WIN_MAN_LIST_INFO_WINNUM_REGION", UI_FLAG_DEFAULT );
			m_pWinNum->CreateSubControl();
			RegisterControl( m_pWinNum );
		}
	}

	// Win Man List;
	{
		m_pWinManListComp = new CLottoWinManListUIComp( m_pInterface, m_pEngineDevice );
		if( m_pWinManListComp )
		{
			m_pWinManListComp->CreateSub( this, "LOTTO_WIN_MAN_LIST_INFO_WIN_MAN_LIST_REGION", UI_FLAG_DEFAULT );
			m_pWinManListComp->CreateSubControl();
			RegisterControl( m_pWinManListComp );
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
				pButton->CreateSub( this, "LOTTO_WIN_MAN_LIST_INFO_CLOSE_BUTTON", UI_FLAG_DEFAULT, ECloseButton );
				arg.text = ID2GAMEWORD( "LOTTO_WIN_MAIN_LIST_INFO_BUTTON_TEXT", 0 );
				pButton->CreateSubControl( arg );
				pButton->SetPressedEventTime( eventPressedTime );
				RegisterControl( pButton );
			}
		}
	}

	// Combo Box;
	{
		m_pComboBox = new CSmartComboBoxBasicEx( m_pEngineDevice );
		m_pComboBox->CreateSub ( this, "BASIC_LINE_BOX_SMART"
			, UI_FLAG_XSIZE | UI_FLAG_YSIZE, ETurnComboBox );
		m_pComboBox->CreateSmartComboBoxCustom(
			"LOTTO_WIN_MAN_LIST_INFO_COMBOBOX", 
			"RAN_COMBO_BOX_BACKGROUND",
			"MARKET_SEARCH_PAGE_BASIC_WEAR_CATEGORY_COMBO_BUTTON",
			"RAN_COMBO_BOX_BUTTON_DEFAULT_TEXTURE",
			"RAN_COMBO_BOX_BUTTON_OVER_TEXTURE",
			"RAN_COMBO_BOX_BUTTON_CLICK_TEXTURE" );
		//m_pComboBox->SetText( m_pComboBox->GetRollOverText( 0 ) );

		RegisterControl( m_pComboBox );
	}
}

void CLottoWinManListInfo::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIWindowEx::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CLottoWinManListInfo::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	CUIWindowEx::TranslateUIMessage( ControlID, dwMsg );

	switch( ControlID )
	{
	case ECloseButton:
		if( CHECK_MOUSEIN_LBUPLIKE(dwMsg) )
		{
			m_pInterface->UiHideGroup( LOTTO_WIN_MAN_LIST_INFO );
		}
		break;

	case ETurnComboBox:
		{
			LottoSystem::CLottoSystemClient* pLottoSystem =
				LottoSystem::GLLottoSystemManClient::Instance()->GetLottoSystemUsingTabIdx(
				GLLottoSystemManUI::Instance()->GetCurTabIndex() );

			// 콤보박스 선택이 변경되면;
			if( dwMsg & UI_MSG_COMBOBOX_ROLLOVER_CHANGE )
			{
				int idx = m_pComboBox->GetSelectIndex();
				if( idx >= 0 &&
					idx < static_cast<int>( m_pComboBoxMemberTurnNum.size() ) )
				{
					unsigned int turnNum = m_pComboBoxMemberTurnNum.at( idx );
					pLottoSystem->ReqPreWinManList( turnNum );
					pLottoSystem->ReqPreAccumulateMoney( turnNum );
					pLottoSystem->ReqPreWinNum( turnNum );
				}
			}
		}
	}
}

void CLottoWinManListInfo::SetVisibleSingle( BOOL bVisible )
{
	CUIWindowEx::SetVisibleSingle( bVisible );

	if( !bVisible && m_pComboBox )
		m_pComboBox->SetVisibleSingleRollOver( bVisible );

	if( bVisible && m_pComboBox )
	{
		LottoSystem::CLottoSystemClient* pLottoSystem =
			LottoSystem::GLLottoSystemManClient::Instance()->GetLottoSystemUsingTabIdx(
			GLLottoSystemManUI::Instance()->GetCurTabIndex() );

		if( pLottoSystem )
		{
			// Update WinManList;
			UpdateWinManList( pLottoSystem );
			UpdateWinNumProc( pLottoSystem );

			// Combo 박스를 갱신한다;
			UpdateTurnProc( pLottoSystem );
			m_pComboBox->SetSelectIndex( 0 );

			// DB에서 즉석해서 가져오는 Data 이므로 업데이트하지 않고 바로 요청한다;
			int preTurn = pLottoSystem->GetCurTurnNum()-1;
			pLottoSystem->ReqPreWinManList( preTurn );
			pLottoSystem->ReqPreAccumulateMoney( preTurn );
			pLottoSystem->ReqPreWinNum( preTurn );
		}
	}
}

void CLottoWinManListInfo::SetFocusControl()
{
	CUIWindowEx::SetFocusControl();

	if( m_pComboBox )
		m_pComboBox->SetVisibleSingleRollOver( FALSE );
}

void CLottoWinManListInfo::ResetFocusControl()
{
	CUIWindowEx::ResetFocusControl();

	if( m_pComboBox )
		m_pComboBox->SetVisibleSingleRollOver( FALSE );
}

void CLottoWinManListInfo::ChangeMainTab()
{
	LottoSystem::CLottoSystemClient* pLottoSystem =
		LottoSystem::GLLottoSystemManClient::Instance()->GetLottoSystemUsingTabIdx(
		GLLottoSystemManUI::Instance()->GetCurTabIndex() );

	// Update WinManList;
	UpdateWinManList( pLottoSystem );
	UpdateWinNumProc( pLottoSystem );

	// Combo 박스를 갱신한다;
	UpdateTurnProc( pLottoSystem );
	m_pComboBox->SetSelectIndex( 0 );

	// DB에서 즉석해서 가져오는 Data 이므로 업데이트하지 않고 바로 요청한다;
	int preTurn = pLottoSystem->GetCurTurnNum()-1;
	pLottoSystem->ReqPreWinManList( preTurn );
	pLottoSystem->ReqPreAccumulateMoney( preTurn );
	pLottoSystem->ReqPreWinNum( preTurn );
}

void CLottoWinManListInfo::UpdateWinNumProc( const LottoSystem::CLottoSystemClient* pLottoSystem )
{
	if( !pLottoSystem )
		return;

	if( m_pWinNum )
	{
		LottoSystem::LOTTO_SYSTEM_CLIENT_VALUE sValue =
			LottoSystem::GLLottoGlobalDataClient::Instance()->GetLottoSystemValue(
			pLottoSystem->GetLottoSystemId() );

		LottoSystem::LOTTO_SYSTEM_LOTTO_NUM winNum = 
			pLottoSystem->GetTurnWinnerNum();

		if( sValue.bUseSort )
		{
			std::vector<unsigned int> vTempDrawingNum;
			for( unsigned int j=0; j<LottoSystem::MAX_DRAWING_NUM_LIST; ++j )
			{
				if( j >= 0 && j < sValue.nDrawingMain )
					vTempDrawingNum.push_back( winNum.lottoNum[ j ] );
			}

			sort( vTempDrawingNum.begin(), vTempDrawingNum.end(), LottoSystem::CLottoSystemClient::CompareLottoNum );

			for( unsigned int j=0; j<LottoSystem::MAX_DRAWING_NUM_LIST; ++j )
			{
				if( j >= 0 && j < sValue.nDrawingMain )
					winNum.lottoNum[ j ] = vTempDrawingNum.at( j );
			}
		}

		for( unsigned int i=0; i<sValue.nDrawingTotal; ++i )
		{
			if( i >= 0 && i < LottoSystem::MAX_DRAWING_NUM_LIST )
				m_pWinNum->SetNumImage( i, winNum.lottoNum[ i ] );
		}
	}
}

void CLottoWinManListInfo::UpdateAccumulateMoneyProc( const LottoSystem::CLottoSystemClient* pLottoSystem )
{
	if( !pLottoSystem )
		return;

	LONGLONG accumulateMoney = pLottoSystem->GetTurnAccumulateMoney();
	CString strText = NS_UITEXTCONTROL::MAKE_MONEY_FORMAT( accumulateMoney, 3, "," );
	m_pAccumulateMoneyTextBox->SetOneLineText( strText );
}

void CLottoWinManListInfo::UpdateWinManList( const LottoSystem::CLottoSystemClient* pLottoSystem )
{
	if( !pLottoSystem )
		return;

	if( m_pWinManListComp )
		m_pWinManListComp->UpdateWinManList( pLottoSystem );
}

void CLottoWinManListInfo::UpdateTurnProc( const LottoSystem::CLottoSystemClient* pLottoSystem )
{
	if( !pLottoSystem )
		return;

	static LottoSystem::LOTTO_GLOBAL_VALUE gValue =
		LottoSystem::GLLottoGlobalDataClient::Instance()
		->GetLottoGlobalValue();

	m_pComboBox->ClearText();
	m_pComboBoxMemberTurnNum.clear();

	int preTurn = pLottoSystem->GetCurTurnNum()-1;
	if( preTurn >= 0 )
	{
		int minTurn = preTurn-(gValue.nViewList-1);
		if( minTurn < 0 )
			minTurn = 0;

		for( int i=preTurn; i>=minTurn; --i )
		{
			std::string tempStr = sc::string::format(
				ID2GAMEINTEXT( "LOTTO_WIN_MAN_LIST_TURN_TEXT" ), i+1 );
			m_pComboBox->SetRollOverAddText( tempStr.c_str(), i );
			m_pComboBoxMemberTurnNum.push_back( i );
		}
	}
}

MyLottoWinManListInfo::MyLottoWinManListInfo( CInnerInterface* pInterface
											 , EngineDeviceMan* pEngineDevice )
											 : CLottoWinManListInfo( pInterface
											 , pEngineDevice )
{

}

void MyLottoWinManListInfo::CreateUIWindowAndRegisterOwnership()
{
	CLottoWinManListInfo::Create( LOTTO_WIN_MAN_LIST_INFO, "BASIC_WINDOW", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	CLottoWinManListInfo::CreateBaseWindowLightGray( "LOTTO_WIN_MAN_LIST_INFO", (char*)ID2GAMEINTEXT("LOTTO_WIN_MAN_LIST_TITLE_TEXT") );
	CLottoWinManListInfo::CreateSubControl();
	CLottoWinManListInfo::SetAlignFlag( UI_FLAG_CENTER_Y | UI_FLAG_RIGHT );
	CLottoWinManListInfo::m_pInterface->UiRegisterControl( this, true );
	CLottoWinManListInfo::m_pInterface->UiShowGroupFocus( LOTTO_WIN_MAN_LIST_INFO );
}

bool MyLottoWinManListInfo::IsVisible()
{
	return CLottoWinManListInfo::IsVisible();
}

void MyLottoWinManListInfo::ChangeMainTab()
{
	CLottoWinManListInfo::ChangeMainTab();
}

void MyLottoWinManListInfo::UpdateWinNumProc( const LottoSystem::CLottoSystemClient* pLottoSystem )
{
	CLottoWinManListInfo::UpdateWinNumProc( pLottoSystem );
}

void MyLottoWinManListInfo::UpdateAccumulateMoneyProc( const LottoSystem::CLottoSystemClient* pLottoSystem )
{
	CLottoWinManListInfo::UpdateAccumulateMoneyProc( pLottoSystem );
}

void MyLottoWinManListInfo::UpdateWinManList( const LottoSystem::CLottoSystemClient* pLottoSystem )
{
	CLottoWinManListInfo::UpdateWinManList( pLottoSystem );
}

void MyLottoWinManListInfo::UpdateTurnProc( const LottoSystem::CLottoSystemClient* pLottoSystem )
{
	CLottoWinManListInfo::UpdateTurnProc( pLottoSystem );
}