#include "StdAfx.h"
#include "PassKeyboard.h"

#include "../../../EngineLib/GUInterface/BasicButton.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"

#include "../Util/ModalWindow.h" // 2차비번

#include "LoginPage.h"
#include "SecPassCheckPage.h"
#include "SecPassSetPage.h"
#include "../../OuterInterface.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CPassKeyboard::CPassKeyboard(COuterInterface* pInterface, EngineDeviceMan* pEngineDevice)
	: CUIGroup(pEngineDevice)
    , m_pInterface(pInterface)
	, m_pModalWindow(NULL) // 2차비번
	, m_pPassKeyboardUpChar(NULL)
	, m_pPassKeyboardLowChar(NULL)
	, m_pKeyCapsLockLed(NULL)
	, m_bUpperCase(FALSE)
{
}

// 2차비번
CPassKeyboard::CPassKeyboard(CModalWindow* pModalWindow, EngineDeviceMan* pEngineDevice)
	: CUIGroup(pEngineDevice)
    , m_pInterface(NULL)
	, m_pModalWindow(pModalWindow)
	, m_pPassKeyboardUpChar(NULL)
	, m_pPassKeyboardLowChar(NULL)
	, m_pKeyCapsLockLed(NULL)
	, m_bUpperCase(FALSE)
{
}
///////////////////////////////////////////////////

CPassKeyboard::~CPassKeyboard()
{
}

void CPassKeyboard::CreateSubControl ()
{
	CUIControl* pPassKeyboard = new CUIControl(m_pEngineDevice);
	pPassKeyboard->CreateSub( this, "PASS_KEYBOARD_FORM", UI_FLAG_DEFAULT, PASS_KEYBOARD_FORM );		
	RegisterControl( pPassKeyboard );

	CString strCombine;
	for( int i=0; i<10; ++i )
	{
		strCombine.Format( "KEY_BUTTON_%d", i );
		CreateKeyButton( strCombine.GetString(), "KEY_BUTTON_FLIP", KEY_BUTTON_0 + i );
	}

	for( int i=0; i<26; ++i )
	{
		strCombine.Format( "KEY_BUTTON_%c", KEY_BUTTON_A + i );
		CreateKeyButton( strCombine.GetString(), "KEY_BUTTON_FLIP", KEY_BUTTON_A + i );
	}

	{ // Note : 기능키
		CreateKeyButton( "KEY_BUTTON_BACK", "KEY_BUTTON_FLIP", KEY_BUTTON_BACK );
		CreateKeyButton( "KEY_BUTTON_TAB", "KEY_BUTTON_L_FLIP", KEY_BUTTON_TAB );
		CreateKeyButton( "KEY_BUTTON_CAPSL", "KEY_BUTTON_L_FLIP", KEY_BUTTON_CAPSL );
	}

	{ // Note : Caps Lock LED
		CBasicButton* pButton = new CBasicButton(m_pEngineDevice);
		pButton->CreateSub( this, "KEY_BUTTON_CAPSL_LED", UI_FLAG_DEFAULT, KEY_BUTTON_CAPSL_LED );
		pButton->CreateFlip( "KEY_BUTTON_CAPSL_LED_FLIP", CBasicButton::RADIO_FLIP );
		RegisterControl( pButton );
		m_pKeyCapsLockLed = pButton;
	}

	{ // Note : 문자 스킨
		pPassKeyboard = new CUIControl(m_pEngineDevice);
		pPassKeyboard->CreateSub( this, "PASS_KEYBOARD_NUMBER", UI_FLAG_DEFAULT, PASS_KEYBOARD_NUMBER );		
		RegisterControl( pPassKeyboard );

		pPassKeyboard = new CUIControl(m_pEngineDevice);
		pPassKeyboard->CreateSub( this, "PASS_KEYBOARD_LOW_CHAR", UI_FLAG_DEFAULT, PASS_KEYBOARD_LOW_CHAR );
		pPassKeyboard->SetVisibleSingle( FALSE );
		RegisterControl( pPassKeyboard );
		m_pPassKeyboardLowChar = pPassKeyboard;

		pPassKeyboard = new CUIControl(m_pEngineDevice);
		pPassKeyboard->CreateSub( this, "PASS_KEYBOARD_UP_CHAR", UI_FLAG_DEFAULT, PASS_KEYBOARD_UP_CHAR );
		pPassKeyboard->SetVisibleSingle( FALSE );
		RegisterControl( pPassKeyboard );
		m_pPassKeyboardUpChar = pPassKeyboard;
	}
}

void CPassKeyboard::CreateKeyButton( const char * szControlKey, const char * szFlipKey, UIGUID nID )
{
	CBasicButton* pButton = new CBasicButton(m_pEngineDevice);
	pButton->CreateSub( this, szControlKey, UI_FLAG_DEFAULT, nID );
	pButton->CreateFlip( szFlipKey, CBasicButton::MOUSEIN_FLIP );
	RegisterControl( pButton );		
}

void CPassKeyboard::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	if( (m_bUpperCase && !(::GetKeyState(VK_CAPITAL) & 1)) ||
		(!m_bUpperCase && (::GetKeyState(VK_CAPITAL) & 1)) )
	{
		m_bUpperCase = !m_bUpperCase;
		SetVisibleKeyChar( m_bUpperCase );
	}
}

void CPassKeyboard::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	CUIGroup::TranslateUIMessage( ControlID, dwMsg );

	if( ControlID >= KEY_BUTTON_0 && ControlID <= KEY_BUTTON_9 )
	{
		TranslateKeyMsg( ControlID, dwMsg, TRUE );
	}
	else if( ControlID >= KEY_BUTTON_A && ControlID <= KEY_BUTTON_Z )
	{
		TranslateKeyMsg( ControlID, dwMsg );
	}
	else
	{
		switch( ControlID )
		{
		case KEY_BUTTON_BACK:
			{
				if( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
				{
					TranslateBackKey();					
				}
			}
			break;

		case KEY_BUTTON_TAB:
			{
				if( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
				{
					TranslateTabKey();
				}
			}
			break;

		case KEY_BUTTON_CAPSL:
			{
				if( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
				{
					m_bUpperCase = !m_bUpperCase;
					SetVisibleKeyChar( m_bUpperCase );

					if( (m_bUpperCase && !(::GetKeyState(VK_CAPITAL) & 1)) ||
						(!m_bUpperCase && (::GetKeyState(VK_CAPITAL) & 1)) )
					{
						keybd_event( VK_CAPITAL, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0 );
						keybd_event( VK_CAPITAL, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0 );
					}
				}
			}
			break;
		}
	}
}

void CPassKeyboard::TranslateKeyMsg( UIGUID ControlID, DWORD dwMsg, BOOL bNumber )
{

	TCHAR Key; 

	if( m_bUpperCase || bNumber ) // Note : 대문자 or 숫자
	{
		Key =  _TCHAR(ControlID);
	}
	else 
	{
		Key =   _TCHAR(ControlID + 32);
	}


	if( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
	{
		// 2차비번
		if(m_pModalWindow)
		{
			CUIEditBox *pEditBox = m_pModalWindow->GetEditBox();

			if( !pEditBox ) return;
			
			CString strTemp;
			strTemp = pEditBox->GetEditString();
			strTemp += Key;
			pEditBox->SetEditString( strTemp );
			
			return; 
		}
		////////////////////////////////

		if( m_pInterface->UiIsVisibleGroup( LOGIN_PAGE ) )
		{
			CLoginPage * pLoginPage = m_pInterface->GetLoginPage();
			if( !pLoginPage ) return;
			pLoginPage->GoThisTab();
			pLoginPage->SetCharToEditBox(Key);

		}
		else if( m_pInterface->UiIsVisibleGroup( SECPASS_SETPAGE ) )
		{
			CSecPassSetPage * pSecPassSetPage = m_pInterface->GetSecPassSetPage();
			if( !pSecPassSetPage ) return;
			pSecPassSetPage->GoThisTab();
			pSecPassSetPage->SetCharToEditBox(Key);
		}
		else if( m_pInterface->UiIsVisibleGroup( SECPASS_CHECKPAGE ) )
		{
			CSecPassCheckPage * pSecPassCheckPage = m_pInterface->GetSecPassCheckPage();
			if( !pSecPassCheckPage ) return;
			pSecPassCheckPage->GoThisTab();
			pSecPassCheckPage->SetCharToEditBox(Key);
		}

		////////////////////
		if( m_pInterface->UiIsVisibleGroup( SECPASS_SETPAGE ) )
		{
			CSecPassSetPage * pSecPassSetPage = m_pInterface->GetSecPassSetPage();
			if( !pSecPassSetPage ) return;
			pSecPassSetPage->GoThisTab();
			pSecPassSetPage->SetCharToEditBox(Key);
		}
		else if( m_pInterface->UiIsVisibleGroup( SECPASS_CHECKPAGE ) )
		{
			CSecPassCheckPage * pSecPassCheckPage = m_pInterface->GetSecPassCheckPage();
			if( !pSecPassCheckPage ) return;
			pSecPassCheckPage->GoThisTab();
			pSecPassCheckPage->SetCharToEditBox(Key);
		}
	}
}

void CPassKeyboard::SetVisibleKeyChar( BOOL bCase )
{
	m_pPassKeyboardUpChar->SetVisibleSingle( FALSE );
	m_pPassKeyboardLowChar->SetVisibleSingle( FALSE );

	if( bCase )	m_pPassKeyboardUpChar->SetVisibleSingle( TRUE );
	else		m_pPassKeyboardLowChar->SetVisibleSingle( TRUE );

	m_pKeyCapsLockLed->SetFlip( bCase );
}

void CPassKeyboard::SetVisibleSingle ( BOOL bVisible )
{
	CUIGroup::SetVisibleSingle( bVisible );

	if( bVisible )
	{
		if( ::GetKeyState(VK_CAPITAL) & 1 )	m_bUpperCase = TRUE;
		else								m_bUpperCase = FALSE;

		SetVisibleKeyChar( m_bUpperCase );
	}
}

void CPassKeyboard::TranslateBackKey()
{
	// 2차비번
	// 비번 입력 이외에는 다른 입력을 할 수 없도록 막음
	if(m_pModalWindow) return; 
	////////////////////////////////

	if( m_pInterface->UiIsVisibleGroup( LOGIN_PAGE ) )
	{
		CLoginPage * pLoginPage = m_pInterface->GetLoginPage();
		if( !pLoginPage ) return;
		pLoginPage->GoThisTab();
		pLoginPage->DelCharToEditBox();
	}
	else if( m_pInterface->UiIsVisibleGroup( SECPASS_SETPAGE ) )
	{
		CSecPassSetPage * pSecPassSetPage = m_pInterface->GetSecPassSetPage();
		if( !pSecPassSetPage ) return;
		pSecPassSetPage->GoThisTab();
		pSecPassSetPage->DelCharToEditBox();
	}
	else if( m_pInterface->UiIsVisibleGroup( SECPASS_CHECKPAGE ) )
	{
		CSecPassCheckPage * pSecPassCheckPage = m_pInterface->GetSecPassCheckPage();
		if( !pSecPassCheckPage ) return;
		pSecPassCheckPage->GoThisTab();
		pSecPassCheckPage->DelCharToEditBox();
	}

	if( m_pInterface->UiIsVisibleGroup( SECPASS_SETPAGE ) )
	{
		CSecPassSetPage * pSecPassSetPage = m_pInterface->GetSecPassSetPage();
		if( !pSecPassSetPage ) return;
		pSecPassSetPage->GoThisTab();
		pSecPassSetPage->DelCharToEditBox();
	}
	else if( m_pInterface->UiIsVisibleGroup( SECPASS_CHECKPAGE ) )
	{
		CSecPassCheckPage * pSecPassCheckPage = m_pInterface->GetSecPassCheckPage();
		if( !pSecPassCheckPage ) return;
		pSecPassCheckPage->GoThisTab();
		pSecPassCheckPage->DelCharToEditBox();
	}
}

void CPassKeyboard::TranslateTabKey()
{
	// 2차비번
	// 비번 입력 이외에는 다른 입력을 할 수 없도록 막음
	if(m_pModalWindow) return; 
	////////////////////////////////

	if( m_pInterface->UiIsVisibleGroup( LOGIN_PAGE ) )
	{
		CLoginPage * pLoginPage = m_pInterface->GetLoginPage();
		if( !pLoginPage ) return;
		pLoginPage->GoNextTab();
	}
	else if( m_pInterface->UiIsVisibleGroup( SECPASS_SETPAGE ) )
	{
		CSecPassSetPage * pSecPassSetPage = m_pInterface->GetSecPassSetPage();
		if( !pSecPassSetPage ) return;
		pSecPassSetPage->GoNextTab();
	}
	else if( m_pInterface->UiIsVisibleGroup( SECPASS_CHECKPAGE ) )
	{
		CSecPassCheckPage * pSecPassCheckPage = m_pInterface->GetSecPassCheckPage();
		if( !pSecPassCheckPage ) return;
		pSecPassCheckPage->GoNextTab();
	}

	if( m_pInterface->UiIsVisibleGroup( SECPASS_SETPAGE ) )
	{
		CSecPassSetPage * pSecPassSetPage = m_pInterface->GetSecPassSetPage();
		if( !pSecPassSetPage ) return;
		pSecPassSetPage->GoNextTab();
	}
	else if( m_pInterface->UiIsVisibleGroup( SECPASS_CHECKPAGE ) )
	{
		CSecPassCheckPage * pSecPassCheckPage = m_pInterface->GetSecPassCheckPage();
		if( !pSecPassCheckPage ) return;
		pSecPassCheckPage->GoNextTab();
	}
}
