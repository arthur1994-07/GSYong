#include "StdAfx.h"
#include "./CTFResultWindow.h"
#include "./CTFResultRanking.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/GUInterface/NSGUI.h"

//#include "../../../SigmaCore/String/StringFormat.h"
//
#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"

#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../EngineLib/GUInterface/BasicLineBoxSmart.h"

#include "../../../EngineLib/DxTools/d3dfont.h"

#include "../../../RanLogicClient/CapturedTheField/GLCaptureTheFieldClient.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CCTFResultWindow::CCTFResultWindow ( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
: CUIWindowEx( pInterface, pEngineDevice )
, m_pGaeaClient(pGaeaClient)
, m_pSortButton_Total(NULL)
, m_pSortButton_SM(NULL)
, m_pSortButton_HA(NULL)
, m_pSortButton_BH(NULL)
, m_pText_Time(NULL)
, m_pCloseButton(NULL)
{
	//Blank
}

CCTFResultWindow::~CCTFResultWindow ()
{
	//Blank
}

void CCTFResultWindow::CreateSubControl ()
{
	//! 정렬버튼
	{
		m_pSortButton_Total = new SORTBUTTON(m_pEngineDevice);
		m_pSortButton_Total->CreateSub( this, "CTF_RESULT_SORTBUTTON_TOTAL", UI_FLAG_DEFAULT, CTF_RESULT_SORTBUTTON_TOTAL);
		m_pSortButton_Total->CreateSubControl( ID2GAMEWORD("CTF_RESULT_BUTTON", 0) );
		RegisterControl( m_pSortButton_Total );

		m_pSortButton_SM = new SORTBUTTON(m_pEngineDevice);
		m_pSortButton_SM->CreateSub( this, "CTF_RESULT_SORTBUTTON_SM", UI_FLAG_DEFAULT, CTF_RESULT_SORTBUTTON_SM);
		m_pSortButton_SM->CreateSubControl( ID2GAMEWORD("CTF_RESULT_BUTTON", 1) );
		RegisterControl( m_pSortButton_SM );

		m_pSortButton_HA = new SORTBUTTON(m_pEngineDevice);
		m_pSortButton_HA->CreateSub( this, "CTF_RESULT_SORTBUTTON_HA", UI_FLAG_DEFAULT, CTF_RESULT_SORTBUTTON_HA);
		m_pSortButton_HA->CreateSubControl( ID2GAMEWORD("CTF_RESULT_BUTTON", 2) );
		RegisterControl( m_pSortButton_HA );

		m_pSortButton_BH = new SORTBUTTON(m_pEngineDevice);
		m_pSortButton_BH->CreateSub( this, "CTF_RESULT_SORTBUTTON_BH", UI_FLAG_DEFAULT, CTF_RESULT_SORTBUTTON_BH);
		m_pSortButton_BH->CreateSubControl( ID2GAMEWORD("CTF_RESULT_BUTTON", 3) );
		RegisterControl( m_pSortButton_BH );
	}

	//! 랭킹
	{
		m_pRanking = new CCTFResultRanking(m_pGaeaClient, m_pInterface, m_pEngineDevice);
		m_pRanking->CreateSub( this, "CTF_RESULT_RANKING", UI_FLAG_DEFAULT, CTF_RESULT_RANKING);
		m_pRanking->CreateSubControl();
		RegisterControl( m_pRanking );
	}

	//! 남은시간
	{
		CD3DFontPar* pFont_Shadow = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_SHADOW_FLAG );
		const int nAlignCenter = TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y;	

		m_pText_Time = new CBasicTextBox(m_pEngineDevice);
		m_pText_Time->CreateSub ( this, "CTF_RESULT_TEXT_TIME", UI_FLAG_DEFAULT );
		m_pText_Time->SetFont ( pFont_Shadow );
		m_pText_Time->SetTextAlign ( nAlignCenter );	
		RegisterControl ( m_pText_Time );
	}

	//! 나가기 버튼
	{
		m_pCloseButton = new CBasicTextButton(m_pEngineDevice);
		m_pCloseButton->CreateSub(this, "BASIC_TEXT_BUTTON19", UI_FLAG_XSIZE, CTF_RESULT_BUTTON_CLOSE);
		m_pCloseButton->CreateBaseButton( "CTF_RESULT_BUTTON_CLOSE", CBasicTextButton::SIZE19, 
			CBasicButton::CLICK_FLIP, ID2GAMEWORD("CTF_RESULT_BUTTON", 4) );
		RegisterControl( m_pCloseButton );
	}
}

void CCTFResultWindow::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	NSGUI::SetCharMoveAllBlock ();

	CUIWindowEx::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	UpdateTime();
}

void CCTFResultWindow::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	switch ( ControlID )
	{
	case CTF_RESULT_SORTBUTTON_TOTAL:
	case CTF_RESULT_SORTBUTTON_SM:
	case CTF_RESULT_SORTBUTTON_HA:
	case CTF_RESULT_SORTBUTTON_BH:
		{
			if ( CHECK_KEYFOCUSED( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			{
				SelectSortButton(ControlID);
			}
		}
		break;

	case CTF_RESULT_BUTTON_CLOSE:	
		{
			if( CHECK_KEYFOCUSED( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			{
				m_pGaeaClient->ReqCaptureTheFieldQuit();
			}
		}
	};

	CUIWindowEx::TranslateUIMessage( ControlID, dwMsg );
}

void CCTFResultWindow::SelectSortButton( const UIGUID cID )
{
	GASSERT( m_pRanking );
	GASSERT( m_pSortButton_Total );
	GASSERT( m_pSortButton_SM );
	GASSERT( m_pSortButton_HA );
	GASSERT( m_pSortButton_BH );

	m_pSortButton_Total->SetEnable( false );
	m_pSortButton_SM->SetEnable( false );
	m_pSortButton_HA->SetEnable( false );
	m_pSortButton_BH->SetEnable( false );

	switch ( cID )
	{
	case CTF_RESULT_SORTBUTTON_TOTAL:
		{
			m_pSortButton_Total->SetEnable( true );
			m_pRanking->Update_SortTotal();
		}
		break;

	case CTF_RESULT_SORTBUTTON_SM:
		{
			m_pSortButton_SM->SetEnable( true );
			m_pRanking->Update_SortSM();
		}
		break;

	case CTF_RESULT_SORTBUTTON_HA:
		{
			m_pSortButton_HA->SetEnable( true );
			m_pRanking->Update_SortHA();
		}
		break;

	case CTF_RESULT_SORTBUTTON_BH:
		{
			m_pSortButton_BH->SetEnable( true );
			m_pRanking->Update_SortBH();
		}
		break;

	};

}

void CCTFResultWindow::Show()
{
	SetVisibleSingle( TRUE );
	SelectSortButton( CTF_RESULT_SORTBUTTON_TOTAL );
}

void CCTFResultWindow::Update()
{
	m_pRanking->Update();
	UpdateTime();
}

void CCTFResultWindow::UpdateTime()
{
	m_pText_Time->ClearText();
	m_pText_Time->AddText(ID2GAMEWORD("CTF_RESULT_TEXT", 0), NS_UITEXTCOLOR::WHITE );
	m_pText_Time->AddString(0, " ", NS_UITEXTCOLOR::WHITE );

	using namespace PVP;
	GLCaptureTheFieldClient* pCTFClient = m_pGaeaClient->GetCaptureTheField();
	GLCaptureTheFieldClient::CTFGLOBALSTATE& sState = pCTFClient->GetCurrentGlobalState();

	if ( sState.IsNULL() )
	{
		m_pText_Time->AddString(0, sc::string::format(ID2GAMEWORD("CTF_RESULT_TEXT", 1), 0, 0).c_str(), NS_UITEXTCOLOR::ORANGE);
		m_pText_Time->AddString(0, ">", NS_UITEXTCOLOR::WHITE);
		return;
	}

	CTime tCurTime( m_pGaeaClient->GetCurrentTime() );

	//	서버 타임으로 변환( TimeZone 계산 )
	m_pGaeaClient->GetConvertServerTime( tCurTime );

	CTimeSpan sTimeLeft( sState.tSTOPTIME - tCurTime.GetTime() );

	if ( sTimeLeft < 0 )
	{
		sTimeLeft = 0;
	}

	m_pText_Time->AddString(0, sc::string::format(ID2GAMEWORD("CTF_RESULT_TEXT", 1), sTimeLeft.GetMinutes(), sTimeLeft.GetSeconds() ).c_str(), NS_UITEXTCOLOR::ORANGE );
	m_pText_Time->AddString(0, ">", NS_UITEXTCOLOR::WHITE);
}

CBasicLineBoxSmart* CCTFResultWindow::CreateLineBox ( const std::string& strControlName, const std::string& strTexInfo, const UIGUID cID )
{
	CBasicLineBoxSmart* pLineBox = new CBasicLineBoxSmart(m_pEngineDevice);
	pLineBox->CreateSub( this, strControlName.c_str(), UI_FLAG_DEFAULT, cID );
	pLineBox->CreateSubControl( strTexInfo.c_str() );
	RegisterControl( pLineBox );
	return pLineBox;
}

//----------------------------------------------------------------------------------------------------//

void CCTFResultWindow::SORTBUTTON::CreateSubControl ( const std::string& strLabel )
{
	CD3DFontPar* pFont = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	ACTIVE = new CBasicLineBoxSmart(m_pEngineDevice);
	ACTIVE->CreateSub( this, "CTF_RESULT_SORTBUTTON_BASE" );
	ACTIVE->CreateSubControl( "CTF_RESULT_SORTBUTTON_TEXINFO_ACTIVE" );
	RegisterControl( ACTIVE );

	INACTIVE = new CBasicLineBoxSmart(m_pEngineDevice);
	INACTIVE->CreateSub( this, "CTF_RESULT_SORTBUTTON_BASE" );
	INACTIVE->CreateSubControl( "CTF_RESULT_SORTBUTTON_TEXINFO_INACTIVE" );
	RegisterControl( INACTIVE );

	TEXTBOX = new CBasicTextBox(m_pEngineDevice);
	TEXTBOX->CreateSub( this, "CTF_RESULT_SORTBUTTON_BASE" );
	TEXTBOX->SetFont( pFont );
	TEXTBOX->SetTextAlign( TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );
	TEXTBOX->SetText( strLabel.c_str() );
	TEXTBOX->SetUseTextColor( 0, TRUE );
	RegisterControl( TEXTBOX );
}

void CCTFResultWindow::SORTBUTTON::SetEnable ( const bool bEnable )
{
	ENABLE = bEnable;

	if ( bEnable )
	{
		ACTIVE->SetVisibleSingle( TRUE );
		INACTIVE->SetVisibleSingle( FALSE );
	}
	else
	{
		ACTIVE->SetVisibleSingle( FALSE );
		INACTIVE->SetVisibleSingle( TRUE );
	}
}

//----------------------------------------------------------------------------------------------------//

MyCTFResultWindow::MyCTFResultWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
    : CCTFResultWindow( pGaeaClient, pInterface, pEngineDevice )
{
}

void MyCTFResultWindow::CreateUIWindowAndRegisterOwnership()
{
     CCTFResultWindow::Create( CTF_RESULT_WINDOW, "BASIC_WINDOW", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
     CCTFResultWindow::CreateBaseWindowBodyLightGray( "CTF_RESULT_WINDOW" );
     CCTFResultWindow::CreateSubControl();
     CCTFResultWindow::SetAlignFlag( UI_FLAG_CENTER_X );
     CCTFResultWindow::m_pInterface->UiRegisterControl( this, true );
     CCTFResultWindow::m_pInterface->UiShowGroupFocus( CTF_RESULT_WINDOW );
}

void MyCTFResultWindow::Show()
{
	CCTFResultWindow::Show();
}

void MyCTFResultWindow::Update()
{
	CCTFResultWindow::Update();
}
