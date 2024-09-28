#include "stdafx.h"

#include "./LottoBuyLottoPopupUIComp.h"

#include "./LottoUIDateType.h"

#include "./GLLottoSystemManUI.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/InterfaceBaseDefine.h"
#include "../../../EngineLib/GUInterface/SwapImage.h"
#include "../../../EngineLib/GUInterface/UITextUtil.h"
#include "../../../EngineLib/GUInterface/UIEditBox.h"
#include "../../../EngineLib/GUInterface/UIEditBoxMan.h"
#include "../../../EngineLib/GUInterface/UIKeyCheck.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"

#include "../../InnerInterface.h"

#include "../Util/RnButton.h"

#include "../../../SigmaCore/DebugInclude.h"

// Logic;
#include "../../../RanLogicClient/LottoSystem/LottoSystemClient.h"
#include "../../../RanLogicClient/LottoSystem/GLLottoSystemManClient.h"
#include "../../../RanLogicClient/LottoSystem/GLLottoGlobalDataClient.h"

#include <boost/bind.hpp>

CLottoBuyLottoPopupUIComp::CLottoBuyLottoPopupUIComp( CInnerInterface* pInterface
													 , EngineDeviceMan* pEngineDevice )
													 : CUIGroupHelperMessageBox( pEngineDevice )
													 , m_pInterface( pInterface )
													 , m_pBuyNumTextBox( NULL )
													 , m_pTicketMoneyTextBox( NULL )
													 , m_pHaveMoneyTextBox( NULL )
													 , m_pEditBoxMan( NULL )
													 , m_pBuyButton( NULL )
{

}

CLottoBuyLottoPopupUIComp::~CLottoBuyLottoPopupUIComp() { }

void CLottoBuyLottoPopupUIComp::CreateSubControl()
{
	// Line Box;
	{
		CreateLineBox( "LOTTO_BUY_LOTTO_UI_OUTLINE_REGION", "NO_LINE_WHITE_BACKGROUND" );
		CreateLineBox( "LOTTO_BUY_LOTTO_UI_REGION", "RAN_UI_BASIC_BACKGROUND_1" );
		CreateLineBox( "LOTTO_BUY_LOTTO_UI_TICKET_MONEY_TEXTBOX", "WHITE_ROUNDED_LINE_DARKGRAY_BACKGROUND" );
		CreateLineBox( "LOTTO_BUY_LOTTO_UI_MY_MONEY_TEXTBOX", "WHITE_ROUNDED_LINE_DARKGRAY_BACKGROUND" );
		CreateLineBox( "LOTTO_BUY_LOTTO_UI_INPUT_OUTLINE_REGION", "NO_LINE_GRAY_BACKGROUND" );
		CreateLineBox( "LOTTO_BUY_LOTTO_UI_INPUT_REGION", "WHITE_ROUNDED_LINE_DARKGRAY_BACKGROUND" );
	}

	// Text Box;
	{
		{
			CBasicTextBox* pTextBox = new CBasicTextBox( m_pEngineDevice );
			pTextBox->CreateSub( this, "LOTTO_BUY_LOTTO_UI_TICKET_MONEY_TEXT_TEXTBOX", UI_FLAG_YSIZE );
			pTextBox->SetFont( DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_BOLD_FLAG ) );
			pTextBox->SetTextAlign( TEXT_ALIGN_CENTER_Y|TEXT_ALIGN_RIGHT );
			pTextBox->AddText(
				ID2GAMEINTEXT( "LOTTO_TICKET_MONEY_TEXT" ),
				NS_UITEXTCOLOR::BLACK );
			RegisterControl( pTextBox );
		}

		{
			CBasicTextBox* pTextBox = new CBasicTextBox( m_pEngineDevice );
			pTextBox->CreateSub( this, "LOTTO_BUY_LOTTO_UI_MY_MONEY_TEXT_TEXTBOX", UI_FLAG_YSIZE );
			pTextBox->SetFont( DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_BOLD_FLAG ) );
			pTextBox->SetTextAlign( TEXT_ALIGN_CENTER_Y|TEXT_ALIGN_RIGHT );
			pTextBox->AddText(
				ID2GAMEINTEXT( "LOTTO_MY_MONEY_TEXT" ),
				NS_UITEXTCOLOR::BLACK );
			RegisterControl( pTextBox );
		}
	}

	// Editable Text Box;
	{
		m_pBuyNumTextBox = new CBasicTextBox( m_pEngineDevice );
		m_pBuyNumTextBox->CreateSub( this, "LOTTO_BUY_LOTTO_UI_BUY_NUM_TEXTBOX", UI_FLAG_YSIZE );
		m_pBuyNumTextBox->SetFont( DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_BOLD_FLAG ) );
		m_pBuyNumTextBox->SetTextAlign( TEXT_ALIGN_CENTER_Y|TEXT_ALIGN_LEFT );
		m_pBuyNumTextBox->AddText(
			sc::string::format( ID2GAMEINTEXT( "LOTTO_BUY_LOTTO_INFO_TEXT" ), 6, 1, 45 ).c_str(),
			NS_UITEXTCOLOR::BLACK );
		RegisterControl( m_pBuyNumTextBox );

		//UIRECT tempRect;

		m_pTicketMoneyTextBox = new CBasicTextBox( m_pEngineDevice );
		m_pTicketMoneyTextBox->CreateSub( this, "LOTTO_BUY_LOTTO_UI_TICKET_MONEY_TEXTBOX", UI_FLAG_YSIZE );
		m_pTicketMoneyTextBox->SetFont( DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_BOLD_FLAG ) );
		m_pTicketMoneyTextBox->SetTextAlign( TEXT_ALIGN_CENTER_Y|TEXT_ALIGN_RIGHT );
		m_pTicketMoneyTextBox->AddText(
			"", NS_UITEXTCOLOR::WHITE );
		RegisterControl( m_pTicketMoneyTextBox );

		m_pHaveMoneyTextBox = new CBasicTextBox( m_pEngineDevice );
		m_pHaveMoneyTextBox->CreateSub( this, "LOTTO_BUY_LOTTO_UI_MY_MONEY_TEXTBOX", UI_FLAG_YSIZE );
		m_pHaveMoneyTextBox->SetFont( DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_BOLD_FLAG ) );
		m_pHaveMoneyTextBox->SetTextAlign( TEXT_ALIGN_CENTER_Y|TEXT_ALIGN_RIGHT );
		m_pHaveMoneyTextBox->AddText(
			"", NS_UITEXTCOLOR::WHITE );
		RegisterControl( m_pHaveMoneyTextBox );
	}

	// Image Box;
	{
		CSwapImage* pImage = new CSwapImage( m_pEngineDevice );
		pImage->CreateSub ( this, "LOTTO_BUY_LOTTO_UI_LOTTO_IMAGE", UI_FLAG_DEFAULT );
		pImage->SetImage( "LOTTO_ICON" );
		RegisterControl( pImage );
	}

	// Num Slot;
	{
		static std::string strSlotSizeId[ LottoSystem::LOTTO_SLOT_NUM_CONT ] = {
			"LOTTO_BUY_LOTTO_UI_INPUT_EDITBOX1",
			"LOTTO_BUY_LOTTO_UI_INPUT_EDITBOX2",
			"LOTTO_BUY_LOTTO_UI_INPUT_EDITBOX3",
			"LOTTO_BUY_LOTTO_UI_INPUT_EDITBOX4",
			"LOTTO_BUY_LOTTO_UI_INPUT_EDITBOX5",
			"LOTTO_BUY_LOTTO_UI_INPUT_EDITBOX6",
		};

		// Num Slot Line;
		for( unsigned int i=0; i<LottoSystem::LOTTO_SLOT_NUM_CONT; ++i )
		{	
			CSwapImage* pImage = new CSwapImage( m_pEngineDevice );
			pImage->CreateSub ( this, strSlotSizeId[ i ].c_str(), UI_FLAG_DEFAULT );
			pImage->SetImage( "BLACK_LINE_DARKGRAY_BACKGROUND" );
			RegisterControl( pImage );
		}

		// Num Slot Edit Box;
		m_pEditBoxMan = new CUIEditBoxMan( m_pEngineDevice );
		m_pEditBoxMan->CreateSub( this, "LOTTO_BUY_LOTTO_UI" );
		m_pEditBoxMan->AddConfirmFunc(
			boost::bind( boost::mem_fn(
			&CLottoBuyLottoPopupUIComp::ConfirmInputLottoNum ), this, _1 ) );

		for( unsigned int i=0; i<LottoSystem::LOTTO_SLOT_NUM_CONT; ++i )
		{	
			CUIEditBox* pEditBox = new CUIEditBox( m_pEngineDevice );
			pEditBox->CreateSub( this, strSlotSizeId[ i ].c_str(), UI_FLAG_DEFAULT, EEditBoxIdx+i );
			pEditBox->CreateCarrat( "LOTTO_BUY_LOTTO_UI_WRITE_NOTE_CARRAT", TRUE, UINT_MAX );
			pEditBox->SetFont( m_pEngineDevice->LoadFont(
				_DEFAULT_FONT, 15, _DEFAULT_FONT_SHADOW_FLAG ) );
			pEditBox->SetLimitInput( 2 );
			pEditBox->DoUSE_DECIMALNUMBER( true );
			pEditBox->SetMinusSign( TRUE );
			pEditBox->SetAlign( EDIT_CENTER_XY_ALIGN );
			pEditBox->SetTextColor( NS_UITEXTCOLOR::WHITE );

			m_pEditBoxMan->AddEditBox( pEditBox );
			m_pEditBoxs.push_back( pEditBox );
		}

		RegisterControl( m_pEditBoxMan );
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
				pButton->CreateSub( this, "LOTTO_BUY_LOTTO_UI_AUTO_INPUT_BUTTON", UI_FLAG_DEFAULT, EAutoInputButton );
				arg.text = ID2GAMEWORD( "LOTTO_BUY_LOTTO_UI_BUTTON_TEXT", 0 );
				pButton->CreateSubControl( arg );
				pButton->SetPressedEventTime( eventPressedTime );
				RegisterControl( pButton );
			}
		}

		{
			m_pBuyButton = new RnButton( m_pEngineDevice );
			if( m_pBuyButton )
			{
				m_pBuyButton->CreateSub( this, "LOTTO_BUY_LOTTO_UI_BUY_BUTTON", UI_FLAG_DEFAULT, EBuyButton );
				arg.text = ID2GAMEWORD( "LOTTO_BUY_LOTTO_UI_BUTTON_TEXT", 1 );
				m_pBuyButton->CreateSubControl( arg );
				m_pBuyButton->SetPressedEventTime( eventPressedTime );
				RegisterControl( m_pBuyButton );
			}
		}

		{
			RnButton* pButton = new RnButton( m_pEngineDevice );
			if( pButton )
			{
				pButton->CreateSub( this, "LOTTO_BUY_LOTTO_UI_CLOSE_BUTTON", UI_FLAG_DEFAULT, ECloseButton );
				arg.text = ID2GAMEWORD( "LOTTO_BUY_LOTTO_UI_BUTTON_TEXT", 2 );
				pButton->CreateSubControl( arg );
				pButton->SetPressedEventTime( eventPressedTime );
				RegisterControl( pButton );
			}
		}
	}

	CreateMessageBox();
}

void CLottoBuyLottoPopupUIComp::Update( int x
									   , int y
									   , BYTE LB
									   , BYTE MB
									   , BYTE RB
									   , int nScroll
									   , float fElapsedTime
									   , BOOL bFirstControl )
{
	LottoSystem::CLottoSystemClient* pLottoSystem =
		LottoSystem::GLLottoSystemManClient::Instance()
		->GetLottoSystemUsingTabIdx( GLLottoSystemManUI::Instance()->GetCurTabIndex() );

	if( pLottoSystem )
	{
		LottoSystem::CLottoSystemClient::BUY_LOTTO_FLAG buyFlag = 
			pLottoSystem->BuyLottoTicket(
			GLLottoSystemManUI::Instance()->GetLottoNum(),
			GLLottoSystemManUI::Instance()->GetBuyType(),
			false );

		if( buyFlag == LottoSystem::CLottoSystemClient::BUY_LOTTO_FLAG_OK )
			m_pBuyButton->SetEnable( true );
		else
			m_pBuyButton->SetEnable( false );
	}

	CUIGroupHelperMessageBox::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	//	탭 이동;
	if( m_pEditBoxMan )
	{
		CUIControl* pParent = m_pEditBoxMan->GetTopParent ();
		if( !pParent )	pParent = this;	//	만약 이클래스가 최상위 컨트롤인 경우;
		BOOL bFocus = ( pParent->IsFocusControl() );

		if( bFocus )
		{
			if( UIKeyCheck::GetInstance()->Check( DIK_TAB, DXKEY_DOWN ) )
			{
				UIGUID ControlID = m_pEditBoxMan->GetBeginEditBox();

				if( ConfirmInputLottoNum( ControlID ) )
					m_pEditBoxMan->GoNextTab();
			}
		}
	}
}

void CLottoBuyLottoPopupUIComp::TranslateUIMessage( UIGUID ControlID
												   , DWORD dwMsg )
{
	CUIGroupHelperMessageBox::TranslateUIMessage( ControlID, dwMsg );

	LottoSystem::CLottoSystemClient* pLottoSystem =
		LottoSystem::GLLottoSystemManClient::Instance()
		->GetLottoSystemUsingTabIdx( GLLottoSystemManUI::Instance()->GetCurTabIndex() );

	switch( ControlID )
	{
	case EAutoInputButton:
		if( CHECK_MOUSEIN_LBUPLIKE(dwMsg) )
		{
			SetLottoNum( pLottoSystem->MakeAutoLottoNum(),
				LottoSystem::AUTO_BUY );
		}
		break;
	case EBuyButton:
		if( CHECK_MOUSEIN_LBUPLIKE(dwMsg) )
		{
			if( m_pBuyButton->IsEnable() )
			{
				// 로또 구입 예외 확인;
				LottoSystem::CLottoSystemClient::BUY_LOTTO_FLAG buyFlag
					= pLottoSystem->BuyLottoTicket(
					GLLottoSystemManUI::Instance()->GetLottoNum(),
					GLLottoSystemManUI::Instance()->GetBuyType(),
					false );

				switch( buyFlag )
				{
				case LottoSystem::CLottoSystemClient::BUY_LOTTO_FLAG_OK:
					// 구입 확인 팝업;
					OpenMessageBox( CMessageBox::TITLE_ANNOUNCEMENT, 
						ID2GAMEINTEXT("LOTTO_BUY_LOTTO_CONFIRM_TEXT"),
						CMessageBox::TYPE_YESNO,
						CLottoBuyLottoPopupUIComp::MESSAGEBOX_CONFIRM_BUY_TICKET );
					break;
				}
			}
		}
		break;
	case ECloseButton:
		if( CHECK_MOUSEIN_LBUPLIKE(dwMsg) )
		{
			m_pInterface->UiHideGroup( LOTTO_BUY_LOTTO_UI );
		}
		break;
	}
}

void CLottoBuyLottoPopupUIComp::TranslateMessageBox( unsigned int nIdentity
													, DWORD dwMsg )
{
	LottoSystem::CLottoSystemClient* pLottoSystem =
		LottoSystem::GLLottoSystemManClient::Instance()
		->GetLottoSystemUsingTabIdx( GLLottoSystemManUI::Instance()
		->GetCurTabIndex() );

	if( !pLottoSystem )
		return;

	switch( nIdentity )
	{
	case MESSAGEBOX_CONFIRM_BUY_TICKET:
		if( dwMsg & UIMSG_MESSAGEBOX_POSITIVE )
		{
			// 로또 구입 예외 확인;
			LottoSystem::CLottoSystemClient::BUY_LOTTO_FLAG buyFlag
				= pLottoSystem->BuyLottoTicket(
				GLLottoSystemManUI::Instance()->GetLottoNum(),
				GLLottoSystemManUI::Instance()->GetBuyType(),
				true );

			m_pInterface->UiHideGroup( LOTTO_BUY_LOTTO_UI );
		}
		break;
	case MESSAGEBOX_WARN_INPUT_ZERO:
	case MESSAGEBOX_WARN_FAILED_NUM:
	case MESSAGEBOX_WARN_INPUT_DUPLICATE:
		break;
	}
}

void CLottoBuyLottoPopupUIComp::SetVisibleSingle( BOOL bVisible )
{
	CUIGroupHelperMessageBox::SetVisibleSingle( bVisible );
}

void CLottoBuyLottoPopupUIComp::UpdateHaveMoney( const LottoSystem::CLottoSystemClient* pLottoSystem )
{
	if( !pLottoSystem )
		return;

	LottoSystem::LOTTO_SYSTEM_CLIENT_VALUE sValue =
		LottoSystem::GLLottoGlobalDataClient::Instance()
		->GetLottoSystemValue( pLottoSystem->GetLottoSystemId() );

	if( m_pTicketMoneyTextBox )
	{
		m_pTicketMoneyTextBox->SetOneLineText(
			sc::string::format( "%d ", NS_UITEXTCONTROL::MAKE_MONEY_FORMAT(
			sValue.lottoTicketMoney, 3, "," ) ).c_str(),
			NS_UITEXTCOLOR::WHITE );

		m_pBuyNumTextBox->SetOneLineText(
			sc::string::format(
			ID2GAMEINTEXT( "LOTTO_BUY_LOTTO_INFO_TEXT" ),
			sValue.nDrawingMain, 1, sValue.nLottoTotal ).c_str(),
			NS_UITEXTCOLOR::BLACK );
	}

	if( m_pHaveMoneyTextBox )
	{
		D3DCOLOR textColor;
		if( sValue.buyInvenMoney > pLottoSystem->GetHaveMoney() )
			textColor = NS_UITEXTCOLOR::RED;
		else
			textColor = NS_UITEXTCOLOR::WHITE;

		m_pHaveMoneyTextBox->SetOneLineText(
			sc::string::format( "%d ", NS_UITEXTCONTROL::MAKE_MONEY_FORMAT(
			pLottoSystem->GetHaveMoney(), 3, "," ) ).c_str(),
			textColor );
	}
}

void CLottoBuyLottoPopupUIComp::SetLottoNum( const LottoSystem::LOTTO_SYSTEM_LOTTO_NUM& lottoNum
								   , LottoSystem::EMLOTTO_BUY_TYPE buyType )
{
	GLLottoSystemManUI::Instance()->SetLottoNum( lottoNum );
	GLLottoSystemManUI::Instance()->SetLottoBuyType( buyType );

	for( unsigned int i=0; i<m_pEditBoxs.size(); ++i )
	{
		if( lottoNum.lottoNum[ i ] == 0 )
		{
			m_pEditBoxs.at( i )->SetEditString( "" );
		}
		else
		{
			m_pEditBoxs.at( i )->SetEditString(
				sc::string::format( "%d", lottoNum.lottoNum[ i ] ).c_str() );
		}
	}
}

void CLottoBuyLottoPopupUIComp::ClearLottoNum()
{
	GLLottoSystemManUI::Instance()->ClearLottoNum();

	for( unsigned int i=0; i<m_pEditBoxs.size(); ++i )
		m_pEditBoxs.at( i )->SetEditString( "" );
}

bool CLottoBuyLottoPopupUIComp::ConfirmInputLottoNum( UIGUID ControlID )
{
	static LottoSystem::LOTTO_GLOBAL_VALUE gValue =
		LottoSystem::GLLottoGlobalDataClient::Instance()
		->GetLottoGlobalValue();

	LottoSystem::LOTTO_SYSTEM_CLIENT_VALUE sValue =
		LottoSystem::GLLottoGlobalDataClient::Instance()
		->GetLottoSystemValue(
		GLLottoSystemManUI::Instance()->GetCurTabIndex() );

	LottoSystem::LOTTO_SYSTEM_LOTTO_NUM& lottoNum
		= GLLottoSystemManUI::Instance()->GetLottoNum();

	// 현재 Edit의 Num 가져오기;
	int idx = ControlID-EEditBoxIdx;
	if( idx >= 0 && idx < static_cast<int>( m_pEditBoxs.size() ) )
	{
		CUIEditBox* pEditBox = m_pEditBoxs[ idx ];
		if( pEditBox )
		{
			std::string numStr =
				pEditBox->GetEditString().GetString();

			if( numStr == "" )
			{
				lottoNum.lottoNum[ ControlID-EEditBoxIdx ] = 0;
				return false;
			}

			// Num 검사 후 예외 처리;
			unsigned int num = atoi( numStr.c_str() );

			// 0일 경우;
			if( num == 0 )
			{
				m_pEditBoxs.at( ControlID-EEditBoxIdx )
					->SetEditString( "" );

				lottoNum.lottoNum[ ControlID-EEditBoxIdx ] = 0;

				// 팝업;
				OpenMessageBox( CMessageBox::TITLE_ANNOUNCEMENT, 
					sc::string::format(
					ID2GAMEINTEXT("LOTTO_BUY_LOTTO_CONFIRM_TEXT1"),
					sValue.nLottoTotal ).c_str(),
					CMessageBox::TYPE_OK,
					CLottoBuyLottoPopupUIComp::MESSAGEBOX_WARN_INPUT_ZERO );

				return false;
			}

			// 숫자 범위가 벗어난 경우;
			if( num < 0 ||
				num > sValue.nLottoTotal )
			{
				m_pEditBoxs.at( ControlID-EEditBoxIdx )
					->SetEditString( "" );

				lottoNum.lottoNum[ ControlID-EEditBoxIdx ] = 0;

				// 팝업;
				OpenMessageBox( CMessageBox::TITLE_ANNOUNCEMENT, 
					sc::string::format(
					ID2GAMEINTEXT("LOTTO_BUY_LOTTO_CONFIRM_TEXT1"),
					sValue.nLottoTotal ).c_str(),
					CMessageBox::TYPE_OK,
					CLottoBuyLottoPopupUIComp::MESSAGEBOX_WARN_FAILED_NUM );

				return false;
			}

			// 중복일 경우;
			for( unsigned int i=0; i<m_pEditBoxs.size(); ++i )
			{
				CUIEditBox* pEditBox = m_pEditBoxs.at( i );
				unsigned int curIdx = ControlID-EEditBoxIdx;

				if( pEditBox && ( i != curIdx ) )
				{
					unsigned int tempNum =
						atoi( pEditBox->GetEditString().GetString() );
					if( num == tempNum )
					{
						m_pEditBoxs.at( curIdx )
							->SetEditString( "" );

						lottoNum.lottoNum[ ControlID-EEditBoxIdx ] = 0;

						// 팝업;
						OpenMessageBox( CMessageBox::TITLE_ANNOUNCEMENT, 
							sc::string::format(
							ID2GAMEINTEXT("LOTTO_BUY_LOTTO_CONFIRM_TEXT2"),
							sValue.nDrawingMain ).c_str(),
							CMessageBox::TYPE_OK,
							CLottoBuyLottoPopupUIComp::MESSAGEBOX_WARN_INPUT_DUPLICATE );

						return false;
					}
				}
			}

			// 숫자 넣기;
			lottoNum.lottoNum[ ControlID-EEditBoxIdx ] = num;
			GLLottoSystemManUI::Instance()->SetLottoBuyType( LottoSystem::MANUAL_BUY );
		}
		else return false;
	}
	else return false;

	

	return true;
}