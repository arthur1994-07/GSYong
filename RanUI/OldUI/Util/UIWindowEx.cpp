#include "StdAfx.h"
#include "../Util/UIWindowEx.h"

#include "../../InnerInterface.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/BasicButton.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/DeviceMan.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CUIWindowEx::CUIWindowEx(CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
	: CUIWindow(pEngineDevice)
    , m_pInterface(pInterface)
	, m_pMessageBox(NULL)
	, bOnMouse(false)
{
}

CUIWindowEx::~CUIWindowEx ()
{
}

void CUIWindowEx::CreateMessageBox()
{
	CMessageBox* pMessageBox = new CMessageBox( m_pEngineDevice );
	pMessageBox->CreateSub( this, "UTILE_MESSAGEBOX_SIZE", UI_FLAG_XSIZE | UI_FLAG_YSIZE, ET_CONTROL_MESSAGEBOX );
	pMessageBox->CreateSubControl();
	RegisterControl( pMessageBox );
	m_pMessageBox = pMessageBox;
}

void CUIWindowEx::OpenMessageBox( const std::string& strTitle, const std::string& strMessage, const CMessageBox::BOXTYPE eBoxType /*= CMessageBox::TYPE_OKCANCEL*/, unsigned int nIdentity /*= 0 */ )
{
	if ( !m_pMessageBox )
	{
		GASSERT( 0 && "CreateMessageBox()를 호출하십시오.");
		return;
	}

	m_pMessageBox->OpenMessageBox( strTitle, strMessage, eBoxType, nIdentity );
}

void CUIWindowEx::OpenMessageBox( const CMessageBox::BOXTITLE eBoxTitle, const std::string& strMessage, const CMessageBox::BOXTYPE eBoxType /*= CMessageBox::TYPE_OKCANCEL*/, unsigned int nIdentity /*= 0 */ )
{
	if ( !m_pMessageBox )
	{
		GASSERT( 0 && "CreateMessageBox()를 호출하십시오.");
		return;
	}

	m_pMessageBox->OpenMessageBox( eBoxTitle, strMessage, eBoxType, nIdentity );
}

void CUIWindowEx::OpenMessageBoxMultiLine( const CMessageBox::BOXTITLE eBoxTitle, const std::vector<std::string>& vecMessage, const CMessageBox::BOXTYPE eBoxType /*= CMessageBox::TYPE_OKCANCEL*/, unsigned int nIdentity /*= 0 */ )
{
	if ( !m_pMessageBox )
	{
		GASSERT( 0 && "CreateMessageBox()를 호출하십시오.");
		return;
	}

	m_pMessageBox->OpenMessageBoxMultiLine( eBoxTitle, vecMessage, eBoxType, nIdentity );
}

void CUIWindowEx::OpenMessageBoxMultiLine( const std::string& strTitle, const std::vector<std::string>& vecMessage, const CMessageBox::BOXTYPE eBoxType /*= CMessageBox::TYPE_OKCANCEL*/, unsigned int nIdentity /*= 0 */ )
{
	if ( !m_pMessageBox )
	{
		GASSERT( 0 && "CreateMessageBox()를 호출하십시오.");
		return;
	}

	m_pMessageBox->OpenMessageBoxMultiLine( strTitle, vecMessage, eBoxType, nIdentity );
}

void CUIWindowEx::SetVisibleSingle( BOOL bVisible )
{
	CUIWindow::SetVisibleSingle( bVisible );

	if ( FALSE == bVisible )
	{
		if ( m_pMessageBox )
			m_pMessageBox->CloseMessageBox();
	}
}

void CUIWindowEx::CloseMessageBox()
{
	if ( m_pMessageBox )
		m_pMessageBox->CloseMessageBox();
}

void CUIWindowEx::CreateBaseWindowEmpty ( char* szWindowKeyword )
{
    ResizeControl ( szWindowKeyword );

    WORD wFlag = GetAlignFlag ();
    wFlag &= ~UI_FLAG_XSIZE;
    wFlag &= ~UI_FLAG_YSIZE;
    SetAlignFlag ( wFlag );
}

void CUIWindowEx::CreateBaseWindowBody ( char* szWindowKeyword )
{
	CreateTitle ( "BASIC_WINDOW_TITLE", "BASIC_WINDOW_TITLE_LEFT", "BASIC_WINDOW_TITLE_MID", "BASIC_WINDOW_TITLE_RIGHT", "BASIC_WINDOW_TEXTBOX", NULL );
	CreateBody ( "BASIC_WINDOW_BODY", "BASIC_WINDOW_BODY_LEFT", "BASIC_WINDOW_BODY_UP", "BASIC_WINDOW_BODY_MAIN_BLACK", "BASIC_WINDOW_BODY_DOWN", "BASIC_WINDOW_BODY_RIGHT" );
	ResizeControl ( szWindowKeyword );

	WORD wFlag = GetAlignFlag ();
	wFlag &= ~UI_FLAG_XSIZE;
	wFlag &= ~UI_FLAG_YSIZE;
	SetAlignFlag ( wFlag );
}

void CUIWindowEx::CreateBaseWindowBodyLightGray ( char* szWindowKeyword )
{
	CreateTitle ( "BASIC_WINDOW_TITLE", "BASIC_WINDOW_TITLE_LEFT", "BASIC_WINDOW_TITLE_MID", "BASIC_WINDOW_TITLE_RIGHT", "BASIC_WINDOW_TEXTBOX", NULL );
	CreateBody ( "BASIC_WINDOW_BODY", "BASIC_WINDOW_BODY_LEFT", "BASIC_WINDOW_BODY_UP", "BASIC_WINDOW_BODY_MAIN_LIGHTGRAY", "BASIC_WINDOW_BODY_DOWN", "BASIC_WINDOW_BODY_RIGHT" );
	ResizeControl ( szWindowKeyword );

	WORD wFlag = GetAlignFlag ();
	wFlag &= ~UI_FLAG_XSIZE;
	wFlag &= ~UI_FLAG_YSIZE;
	SetAlignFlag ( wFlag );
}

void CUIWindowEx::CreateBaseWindowBlack ( char* szWindowKeyword, char* szWindowName )
{
	CreateTitle ( "BASIC_WINDOW_TITLE", "BASIC_WINDOW_TITLE_LEFT", "BASIC_WINDOW_TITLE_MID", "BASIC_WINDOW_TITLE_RIGHT", "BASIC_WINDOW_TEXTBOX", szWindowName );
    EMSERVICE_PROVIDER ServiceProvider = m_pEngineDevice->GetServiceProvider();

    if (ServiceProvider == SP_KOREA ||
        ServiceProvider == SP_KOREA_TEST ||
        ServiceProvider == SP_OFFICE_TEST ||
        ServiceProvider == SP_TAIWAN ||
        ServiceProvider == SP_HONGKONG ||
        ServiceProvider == SP_JAPAN ||
        ServiceProvider == SP_CHINA ||
        ServiceProvider == SP_INDONESIA ||
        ServiceProvider == SP_WORLD_BATTLE)
    {
	    CreateTitleFocus ( "BASIC_WINDOW_TITLE", "BASIC_WINDOW_TITLE_LEFT_F", "BASIC_WINDOW_TITLE_MID_F", "BASIC_WINDOW_TITLE_RIGHT_F", "BASIC_WINDOW_TEXTBOX", szWindowName );
    }

	CreateCloseButton ( "BASIC_WINDOW_CLOSE", "BASIC_WINDOW_CLOSE_F", "BASIC_WINDOW_CLOSE_OVER" );
	CreateBody ( "BASIC_WINDOW_BODY", "BASIC_WINDOW_BODY_LEFT", "BASIC_WINDOW_BODY_UP", "BASIC_WINDOW_BODY_MAIN_BLACK", "BASIC_WINDOW_BODY_DOWN", "BASIC_WINDOW_BODY_RIGHT" );
	ResizeControl ( szWindowKeyword );

    WORD wFlag = GetAlignFlag ();
	wFlag &= ~UI_FLAG_XSIZE;
	wFlag &= ~UI_FLAG_YSIZE;
	SetAlignFlag ( wFlag );
}

void CUIWindowEx::CreateBaseWindowLightGray ( char* szWindowKeyword,const char* szWindowName )
{
	CreateTitle ( "BASIC_WINDOW_TITLE", "BASIC_WINDOW_TITLE_LEFT", "BASIC_WINDOW_TITLE_MID", "BASIC_WINDOW_TITLE_RIGHT", "BASIC_WINDOW_TEXTBOX", szWindowName );

    EMSERVICE_PROVIDER ServiceProvider = m_pEngineDevice->GetServiceProvider();
    if (ServiceProvider == SP_KOREA ||
        ServiceProvider == SP_KOREA_TEST ||
        ServiceProvider == SP_OFFICE_TEST ||
        ServiceProvider == SP_TAIWAN ||
        ServiceProvider == SP_HONGKONG ||
        ServiceProvider == SP_JAPAN ||
        ServiceProvider == SP_CHINA ||
        ServiceProvider == SP_INDONESIA ||
        ServiceProvider == SP_WORLD_BATTLE)
    {
	    CreateTitleFocus ( "BASIC_WINDOW_TITLE", "BASIC_WINDOW_TITLE_LEFT_F", "BASIC_WINDOW_TITLE_MID_F", "BASIC_WINDOW_TITLE_RIGHT_F", "BASIC_WINDOW_TEXTBOX", szWindowName );
    }

	CreateCloseButton ( "BASIC_WINDOW_CLOSE", "BASIC_WINDOW_CLOSE_F", "BASIC_WINDOW_CLOSE_OVER" );
	CreateBody ( "BASIC_WINDOW_BODY", "BASIC_WINDOW_BODY_LEFT", "BASIC_WINDOW_BODY_UP", "BASIC_WINDOW_BODY_MAIN_LIGHTGRAY", "BASIC_WINDOW_BODY_DOWN", "BASIC_WINDOW_BODY_RIGHT" );
	ResizeControl ( szWindowKeyword );

    WORD wFlag = GetAlignFlag ();
	wFlag &= ~UI_FLAG_XSIZE;
	wFlag &= ~UI_FLAG_YSIZE;
	SetAlignFlag ( wFlag );
}

void CUIWindowEx::CreateBaseWindowLightGrayNoClose( char* szWindowKeyword,const char* szWindowName )
{
    CreateTitle ( "BASIC_WINDOW_TITLE", "BASIC_WINDOW_TITLE_LEFT", "BASIC_WINDOW_TITLE_MID", "BASIC_WINDOW_TITLE_RIGHT", "BASIC_WINDOW_TEXTBOX", szWindowName );

    EMSERVICE_PROVIDER ServiceProvider = m_pEngineDevice->GetServiceProvider();
    if (ServiceProvider == SP_KOREA ||
        ServiceProvider == SP_KOREA_TEST ||
        ServiceProvider == SP_OFFICE_TEST ||
        ServiceProvider == SP_TAIWAN ||
        ServiceProvider == SP_HONGKONG ||
        ServiceProvider == SP_JAPAN ||
        ServiceProvider == SP_CHINA ||
        ServiceProvider == SP_INDONESIA ||
        ServiceProvider == SP_WORLD_BATTLE)
    {
        CreateTitleFocus ( "BASIC_WINDOW_TITLE", "BASIC_WINDOW_TITLE_LEFT_F", "BASIC_WINDOW_TITLE_MID_F", "BASIC_WINDOW_TITLE_RIGHT_F", "BASIC_WINDOW_TEXTBOX", szWindowName );
    }

    CreateBody ( "BASIC_WINDOW_BODY", "BASIC_WINDOW_BODY_LEFT", "BASIC_WINDOW_BODY_UP", "BASIC_WINDOW_BODY_MAIN_LIGHTGRAY", "BASIC_WINDOW_BODY_DOWN", "BASIC_WINDOW_BODY_RIGHT" );
    ResizeControl ( szWindowKeyword );

    WORD wFlag = GetAlignFlag ();
    wFlag &= ~UI_FLAG_XSIZE;
    wFlag &= ~UI_FLAG_YSIZE;
    SetAlignFlag ( wFlag );
}


void CUIWindowEx::CreateBaseWindowMiniParty ( char* szWindowKeyword, char* szWindowName )
{
	CreateTitle ( "MINIPARTY_WINDOW_TITLE", "MINIPARTY_WINDOW_TITLE_LEFT", "MINIPARTY_WINDOW_TITLE_MID", "MINIPARTY_WINDOW_TITLE_RIGHT", "MINIPARTY_WINDOW_TEXTBOX", szWindowName );
	CreateCloseButton ( "MINIPARTY_WINDOW_CLOSE", "MINIPARTY_WINDOW_CLOSE_F", "MINIPARTY_WINDOW_CLOSE_OVER" );
	CreateBody ( "MINIPARTY_WINDOW_BODY", "MINIPARTY_WINDOW_BODY_LEFT", "MINIPARTY_WINDOW_BODY_UP", "MINIPARTY_WINDOW_BODY_MAIN", "MINIPARTY_WINDOW_BODY_DOWN", "MINIPARTY_WINDOW_BODY_RIGHT" );
	ResizeControl ( szWindowKeyword );

    WORD wFlag = GetAlignFlag ();
	wFlag &= ~UI_FLAG_XSIZE;
	wFlag &= ~UI_FLAG_YSIZE;
	SetAlignFlag ( wFlag );
}

void CUIWindowEx::CreateBaseWindowLargeMap ( char* szWindowKeyword, char* szWindowName )
{
	CreateTitle ( "BASIC_WINDOW_TITLE", "BASIC_WINDOW_TITLE_LEFT", "BASIC_WINDOW_TITLE_MID", "BASIC_WINDOW_TITLE_RIGHT", "BASIC_WINDOW_TEXTBOX", szWindowName );

    EMSERVICE_PROVIDER ServiceProvider = m_pEngineDevice->GetServiceProvider();
    if (ServiceProvider == SP_KOREA ||
        ServiceProvider == SP_KOREA_TEST || 
        ServiceProvider == SP_OFFICE_TEST ||
        ServiceProvider == SP_TAIWAN ||
        ServiceProvider == SP_HONGKONG || 
        ServiceProvider == SP_JAPAN ||
        ServiceProvider == SP_CHINA ||
        ServiceProvider == SP_INDONESIA ||
        ServiceProvider == SP_WORLD_BATTLE)
    {
	    CreateTitleFocus ( "BASIC_WINDOW_TITLE", "BASIC_WINDOW_TITLE_LEFT_F", "BASIC_WINDOW_TITLE_MID_F", "BASIC_WINDOW_TITLE_RIGHT_F", "BASIC_WINDOW_TEXTBOX", szWindowName );
    }

    CBasicTextButton* pButton = new CBasicTextButton(m_pEngineDevice);
	pButton->CreateSub ( this, "BASIC_TEXT_BUTTON16", UI_FLAG_RIGHT, ET_CONTROL_BUTTON );
	pButton->CreateBaseButton ( "LARGEMAP_CLOSE_BUTTON", CBasicTextButton::SIZE16, CBasicButton::CLICK_FLIP, (char*)ID2GAMEWORD ( "CLOSE_BUTTON", 0 ) );
	RegisterControl ( pButton );
	CreateBody ( "BASIC_WINDOW_BODY", "BASIC_WINDOW_BODY_LEFT", "BASIC_WINDOW_BODY_UP", "BASIC_WINDOW_BODY_MAIN_BLACK", "BASIC_WINDOW_BODY_DOWN", "BASIC_WINDOW_BODY_RIGHT" );
	ResizeControl ( szWindowKeyword );

	WORD wFlag = GetAlignFlag ();
	wFlag &= ~UI_FLAG_XSIZE;
	wFlag &= ~UI_FLAG_YSIZE;
	SetAlignFlag ( wFlag );
}

void CUIWindowEx::CreateBaseWindowBlackNoClose ( char* szWindowKeyword, char* szWindowName )
{
	CreateTitle ( "BASIC_WINDOW_TITLE", "BASIC_WINDOW_TITLE_LEFT", "BASIC_WINDOW_TITLE_MID", "BASIC_WINDOW_TITLE_RIGHT", "BASIC_WINDOW_TEXTBOX", szWindowName );
	EMSERVICE_PROVIDER ServiceProvider = m_pEngineDevice->GetServiceProvider();

	if (ServiceProvider == SP_KOREA ||
		ServiceProvider == SP_KOREA_TEST ||
		ServiceProvider == SP_OFFICE_TEST ||
		ServiceProvider == SP_TAIWAN ||
		ServiceProvider == SP_HONGKONG ||
		ServiceProvider == SP_JAPAN ||
		ServiceProvider == SP_CHINA ||
		ServiceProvider == SP_INDONESIA ||
		ServiceProvider == SP_WORLD_BATTLE)
	{
		CreateTitleFocus ( "BASIC_WINDOW_TITLE", "BASIC_WINDOW_TITLE_LEFT_F", "BASIC_WINDOW_TITLE_MID_F", "BASIC_WINDOW_TITLE_RIGHT_F", "BASIC_WINDOW_TEXTBOX", szWindowName );
	}

	CreateBody ( "BASIC_WINDOW_BODY", "BASIC_WINDOW_BODY_LEFT", "BASIC_WINDOW_BODY_UP", "BASIC_WINDOW_BODY_MAIN_BLACK", "BASIC_WINDOW_BODY_DOWN", "BASIC_WINDOW_BODY_RIGHT" );
	ResizeControl ( szWindowKeyword );

	WORD wFlag = GetAlignFlag ();
	wFlag &= ~UI_FLAG_XSIZE;
	wFlag &= ~UI_FLAG_YSIZE;
	SetAlignFlag ( wFlag );
}

void CUIWindowEx::CreateBaseWindowMaxGroupChat ( char* szWindowKeyword, char* szWindowName )
{
	CreateTitle ( "BASIC_WINDOW_TITLE", "BASIC_WINDOW_TITLE_LEFT", "BASIC_WINDOW_TITLE_MID", "BASIC_WINDOW_TITLE_RIGHT", "BASIC_WINDOW_TEXTBOX", szWindowName );
	EMSERVICE_PROVIDER ServiceProvider = m_pEngineDevice->GetServiceProvider();

	if (ServiceProvider == SP_KOREA ||
		ServiceProvider == SP_KOREA_TEST ||
		ServiceProvider == SP_OFFICE_TEST ||
		ServiceProvider == SP_TAIWAN ||
		ServiceProvider == SP_HONGKONG ||
		ServiceProvider == SP_JAPAN ||
		ServiceProvider == SP_CHINA ||
		ServiceProvider == SP_INDONESIA ||
		ServiceProvider == SP_WORLD_BATTLE)
	{
		CreateTitleFocus ( "BASIC_WINDOW_TITLE", "BASIC_WINDOW_TITLE_LEFT_F", "BASIC_WINDOW_TITLE_MID_F", "BASIC_WINDOW_TITLE_RIGHT_F", "BASIC_WINDOW_TEXTBOX", szWindowName );
	}
	CreateCloseButton ( "MINIPARTY_WINDOW_CLOSE", "MINIPARTY_WINDOW_CLOSE_F", "MINIPARTY_WINDOW_CLOSE_OVER" );

	CreateBody ( "BASIC_WINDOW_BODY", "BASIC_WINDOW_BODY_LEFT", "BASIC_WINDOW_BODY_UP", "BASIC_WINDOW_BODY_MAIN_BLACK", "BASIC_WINDOW_BODY_DOWN", "BASIC_WINDOW_BODY_RIGHT" );
	ResizeControl ( szWindowKeyword );

	WORD wFlag = GetAlignFlag ();
	wFlag &= ~UI_FLAG_XSIZE;
	wFlag &= ~UI_FLAG_YSIZE;
	SetAlignFlag ( wFlag );
}

void CUIWindowEx::ResizeControl ( char* szWindowKeyword )
{
    CUIControl TempControl(m_pEngineDevice); 
	TempControl.Create ( 1, szWindowKeyword ); // MEMO
	const UIRECT& rcParentOldPos = GetLocalPos ();
	const UIRECT& rcParentNewPos = TempControl.GetLocalPos ();
	AlignSubControl ( rcParentOldPos, rcParentNewPos );

	SetLocalPos ( D3DXVECTOR2 ( rcParentNewPos.left, rcParentNewPos.top ) );
}

// CUIControl*	CUIWindowEx::CreateControl ( const char* szControl, const UIGUID& cID )
// {
// 	CUIControl* pControl = new CUIControl(m_pEngineDevice);
// 	pControl->CreateSub ( this, szControl, UI_FLAG_DEFAULT, cID );
// 	RegisterControl ( pControl );
// 	return pControl;
// }
// 
// CBasicButton* CUIWindowEx::CreateFlipButton ( char* szButton, char* szButtonFlip, UIGUID ControlID, WORD wFlipType )
// {
// 	CBasicButton* pButton = new CBasicButton(m_pEngineDevice);
// 	pButton->CreateSub ( this, szButton, UI_FLAG_DEFAULT, ControlID );
// 	pButton->CreateFlip ( szButtonFlip, wFlipType );
// 	RegisterControl ( pButton );
// 	return pButton;
// }
// 
// CBasicTextBox* CUIWindowEx::CreateStaticControl ( char* szControlKeyword, CD3DFontPar* pFont, D3DCOLOR dwColor, int nAlign )
// {
// 	CBasicTextBox* pStaticText = new CBasicTextBox(m_pEngineDevice);
// 	pStaticText->CreateSub ( this, szControlKeyword );
// 	pStaticText->SetFont ( pFont );
// 	pStaticText->SetTextAlign ( nAlign );
// 	RegisterControl ( pStaticText );
// 	return pStaticText;
// }

void CUIWindowEx::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	if ( !m_pMessageBox )
	{
		CUIWindow::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
		return;
	}

	if ( FALSE == m_pMessageBox->IsVisible() )
	{
		ActiveUIMessage();
	}
	else
	{
		if ( m_pMessageBox )
		{
			InactiveUIMessage();
			m_pMessageBox->ActiveUIMessage();

			m_pMessageBox->Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
			DWORD dwMsg = m_pMessageBox->GetMessageEx();
			TranslateMessageBox( m_pMessageBox->GetIdentifier(), dwMsg);
		}
	}

	CUIWindow::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CUIWindowEx::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	if ( !m_pMessageBox )
	{
		CUIWindow::TranslateUIMessage( ControlID, dwMsg );

		switch ( ControlID )
		{
		case ET_CONTROL_BUTTON:
			{
				if( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
				{
					m_pInterface->UiHideGroup( GetWndID() );
				}
			}
			break;
		}

		return;
	}

	if ( ControlID == ET_CONTROL_MESSAGEBOX )
		bOnMouse = true;
	else
		bOnMouse = false;

	if ( FALSE == m_pMessageBox->IsVisible() )
	{
		TranslateMessageBox( m_pMessageBox->GetIdentifier(), dwMsg);
		CUIWindow::TranslateUIMessage( ControlID, dwMsg );

		switch ( ControlID )
		{
		case ET_CONTROL_BUTTON:
			{
				if( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
				{
					m_pInterface->UiHideGroup( GetWndID() );
				}
			}
			break;
		}
	}
}

BOOL CUIWindowEx::IsOpenMessageBox()
{
	if ( !m_pMessageBox )
		return false;

	return m_pMessageBox->IsVisible();
}

BOOL CUIWindowEx::IsOnMouse()
{
	return bOnMouse;
}

std::string CUIWindowEx::GetInputString()
{
	if ( !m_pMessageBox )
	{
		GASSERT( 0 && "CreateMessageBox()를 호출하십시오.");
		return "";
	}

	return m_pMessageBox->GetInputString();
}

void CUIWindowEx::CreateBaseWindowBodyLightGrayNoTitle( char* szWindowKeyword )
{    
    CreateBody ( "BASIC_WINDOW_BODY", "BASIC_WINDOW_BODY_LEFT", "BASIC_WINDOW_BODY_UP", "BASIC_WINDOW_BODY_MAIN_LIGHTGRAY", "BASIC_WINDOW_BODY_DOWN", "BASIC_WINDOW_BODY_RIGHT" );
    ResizeControl ( szWindowKeyword );

    WORD wFlag = GetAlignFlag ();
    wFlag &= ~UI_FLAG_XSIZE;
    wFlag &= ~UI_FLAG_YSIZE;
    SetAlignFlag ( wFlag );
}
