#include "StdAfx.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/UIControl.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/BasicLineBox.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/UIEditBox.h"

#include "../../OuterInterface.h"

#include "CreateCharacterWindow.h"
#include "CreateCharacterMenu.h"
#include "CreateCharacterButtonPage.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CCreateChatacterButtonPage::CCreateChatacterButtonPage(COuterInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
    , m_pInterface(pInterface)
    , m_pNewChar( NULL )
    , m_pServerText( NULL )
{
}

CCreateChatacterButtonPage::~CCreateChatacterButtonPage()
{
}

void CCreateChatacterButtonPage::CreateSubControl ()
{
	const int nBUTTONSIZE = CBasicTextButton::SIZE40;

	{
		CBasicLineBox* pBasicLineBox = new CBasicLineBox(m_pEngineDevice);
		pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_OUTER_FAT", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pBasicLineBox->CreateBaseBoxOuterFat ( "CREATE_CHARACTER_BUTTON_WINDOW_BACK" );
		RegisterControl ( pBasicLineBox );		
	}

	{
		CBasicLineBox* pBasicLineBox = new CBasicLineBox(m_pEngineDevice);
		pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_OUTER_FAT", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pBasicLineBox->CreateBaseBoxOuterFat ( "CREATE_CHARACTER_SERVER_TEXT_BACK" );
		RegisterControl ( pBasicLineBox );		
	}

	{
		CD3DFontPar* pFont9 = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9 , _DEFAULT_FONT_FLAG );
		m_pServerText = CreateStaticControl ( "CREATE_CHARACTER_SERVER_TEXT", pFont9, NS_UITEXTCOLOR::DEFAULT, TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );
	}

	{
		CBasicTextButton* pButton = new CBasicTextButton(m_pEngineDevice);
		pButton->CreateSub ( this, "BASIC_TEXT_BUTTON40", UI_FLAG_XSIZE, CREATE_CHARACTER_CANCEL );
		pButton->CreateBaseButton ( "CREATE_CHARACTER_CANCEL", nBUTTONSIZE, CBasicButton::CLICK_FLIP, (char*)ID2GAMEWORD ( "ESC_MENU", 0 ) );
		RegisterControl ( pButton );
	}

	{
		CBasicTextButton* pButton = new CBasicTextButton(m_pEngineDevice);
		pButton->CreateSub ( this, "BASIC_TEXT_BUTTON40", UI_FLAG_XSIZE, CREATE_CHARACTER_NEW );
		pButton->CreateBaseButton ( "CREATE_CHARACTER_NEW", nBUTTONSIZE, CBasicButton::CLICK_FLIP, (char*)ID2GAMEWORD ( "SELECT_CHARACTER_PAGE_BUTTON", 0 ) );		
		RegisterControl ( pButton );
		m_pNewChar = pButton;
	}

	{
		CBasicTextButton* pButton = new CBasicTextButton(m_pEngineDevice);
		pButton->CreateSub ( this, "BASIC_TEXT_BUTTON40", UI_FLAG_XSIZE, CREATE_CHARACTER_CHANGE_SERVER );
		pButton->CreateBaseButton ( "CREATE_CHARACTER_CHANGE_SERVER", nBUTTONSIZE, CBasicButton::CLICK_FLIP, (char*)ID2GAMEWORD ( "CHANGE_SERVER", 0 ) );
		pButton->SetShortcutKey ( DIK_ESCAPE );
		RegisterControl ( pButton );
	}

	{
		CBasicTextButton* pButton = new CBasicTextButton(m_pEngineDevice);
		pButton->CreateSub ( this, "BASIC_TEXT_BUTTON40", UI_FLAG_XSIZE, CREATE_CHARACTER_OK );
		pButton->CreateBaseButton ( "CREATE_CHARACTER_GAME_START", nBUTTONSIZE, CBasicButton::CLICK_FLIP, (char*)ID2GAMEWORD ( "CREATE_COMPLETE", 0 ) );
		RegisterControl ( pButton );
	}
}

CUIControl*	CCreateChatacterButtonPage::CreateControl( CString strControl, WORD wAlignFlag, UIGUID WndID )
{
	CUIControl* pControl = new CUIControl(m_pEngineDevice);
	pControl->CreateSub ( this, strControl.GetString(), wAlignFlag, WndID );
	RegisterControl ( pControl );
	return pControl;
}

CBasicTextBox* CCreateChatacterButtonPage::CreateStaticControl (const char* szControlKeyword, CD3DFontPar* pFont, D3DCOLOR dwColor, int nAlign )
{
	CBasicTextBox* pStaticText = new CBasicTextBox(m_pEngineDevice);
	pStaticText->CreateSub ( this, szControlKeyword );
	pStaticText->SetFont ( pFont );
	pStaticText->SetTextAlign ( nAlign );
	RegisterControl ( pStaticText );
	return pStaticText;
}

void CCreateChatacterButtonPage::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	const USHORT uCharRemain = m_pInterface->GetCharRemain();
	CString strTemp;
	strTemp.Format ( "%s(%u)", (char*)ID2GAMEWORD ( "SELECT_CHARACTER_PAGE_BUTTON", 0 ), uCharRemain );
	m_pNewChar->SetOneLineText( strTemp );

	INT nServerInfo, nChannelInfo;
	CString strCombine;
	m_pInterface->GetConnectServerInfo( nServerInfo, nChannelInfo );
	strCombine.Format( _T( "%s - %d%s" ), ID2GAMEEXTEXT("SERVER_NAME",nServerInfo), nChannelInfo, ID2GAMEEXTEXT("SERVER_CHANNEL") );
	m_pServerText->SetOneLineText( strCombine );
}

void CCreateChatacterButtonPage::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	CUIGroup::TranslateUIMessage ( ControlID, dwMsg );

	switch ( ControlID )
	{
	case CREATE_CHARACTER_CANCEL:
		{
			if ( CHECK_KEYFOCUSED ( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				if( !m_pInterface->GetCreateCharacterWindow()->IsCancelButton() )
					return;

				m_pInterface->DoModal ( ID2GAMEEXTEXT ("CHARACTERSTAGE_13"), UI::MODAL_INFOMATION, UI::OKCANCEL, OUTER_MODAL_CHARACTER_SELECT );
			}
		}
		break;

	case CREATE_CHARACTER_NEW:
		{
			if ( CHECK_KEYFOCUSED ( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
			}
		}
		break;

	case CREATE_CHARACTER_CHANGE_SERVER:
		{
			if ( CHECK_KEYFOCUSED ( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				if( !m_pInterface->GetCreateCharacterWindow()->IsCancelButton() )
					return;

				m_pInterface->DoModal ( ID2GAMEEXTEXT ("CHARACTERSTAGE_6"), UI::MODAL_INFOMATION, UI::OKCANCEL, OUTER_MODAL_LOGOUT );
			}
		}
		break;

	case CREATE_CHARACTER_OK:
		{
			if ( CHECK_KEYFOCUSED ( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				INT nSchool;
				INT nSex;
				INT nClass;
				CCreateCharacterMenu* pMenu = m_pInterface->GetCreateCharacterWindow()->GetMenu();
				if( pMenu )
				{
					pMenu->GetSelectInfo( nSchool, nSex, nClass );

					if( nSchool == -1 )
					{
						m_pInterface->DoModal ( ID2GAMEEXTEXT ( "NEW_CHAR_SELECT_SCHOOL_ERROR" ) );
						return;
					}
					else if( nSex == -1 )
					{
						m_pInterface->DoModal ( ID2GAMEEXTEXT ( "NEW_CHAR_SELECT_SEX_ERROR" ) );
						return;
					}
					else if( nClass == -1 )
					{
						m_pInterface->DoModal ( ID2GAMEEXTEXT ( "NEW_CHAR_SELECT_CLASS_ERROR" ) );
						return;
					}

					CUIEditBox* pEditBox = m_pInterface->GetCreateCharacterWindow()->GetMenu()->GetEditBox();
					if ( !pEditBox )											return;
					if ( !pEditBox->GetEditLength() )	
					{
						m_pInterface->DoModal ( ID2GAMEEXTEXT ( "CREATESTAGE_2" ) );
						return;
					}

					pMenu->CreateCharProcess();
				}
			}
		}
		break;
	}
}