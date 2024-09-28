#include "StdAfx.h"
#include "ChatOptionWindow.h"
#include "../../../EngineLib/GUInterface/BasicLineBox.h"
#include "../../../EngineLib/GUInterface/BasicButton.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../Interface/ChatType.h"
#include "../../InnerInterface.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CChatOptionWindow::CChatOptionWindow( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
	: CUIWindowEx( pInterface, pEngineDevice )
	, m_pNormalButton(NULL)
	, m_pPrivateButton(NULL)
	, m_pPartyButton(NULL)
	, m_pClubButton(NULL)
	, m_pSystemButton(NULL)
	, m_pAllianceButton(NULL)
	, m_pToAllButton(NULL)
	, m_pAreaButton(NULL)
	, m_pPartyRecruitButton(NULL)
	, m_pNormalTextBox(NULL)
	, m_pPrivateTextBox(NULL)
	, m_pPartyTextBox(NULL)
	, m_pClubTextBox(NULL)
	, m_pSystemTextBox(NULL)
	, m_pAllianceTextBox(NULL)
	, m_pToAllTextBox(NULL)
	, m_pAreaTextBox(NULL)
	, m_pPartyRecruitTextBox(NULL)
	, m_dwChatFlag(CHAT_CONST::CHAT_ALL)
{
}

CChatOptionWindow::~CChatOptionWindow()
{
}

void	CChatOptionWindow::CreateSubControl()
{
	{	//	뒷 배경
		CBasicLineBox* pBasicLineBox = new CBasicLineBox(m_pEngineDevice);
		pBasicLineBox->CreateSub( this, "BASIC_LINE_BOX_QUEST_LIST", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pBasicLineBox->CreateBaseBoxQuestList( "CHAT_OPTION_WINDOW_BACK" );
//		pBasicLineBox->ResetAlignFlagLine();
		pBasicLineBox->ResetAlignFlag();
		RegisterControl( pBasicLineBox );		
	}

	const int nBUTTONSIZE = CBasicTextButton::SIZE19;

	CreateTextBox( "CHAT_NORMAL_STATIC", ID2GAMEWORD("CHAT_OPTION_WINDOW_STATIC",1), CHAT_NORMAL_STATIC );
	CreateTextBox( "CHAT_CHANNEL_STATIC", ID2GAMEWORD("CHAT_OPTION_WINDOW_STATIC",2), CHAT_CHANNEL_STATIC );

	//	일반
	m_pNormalButton = CreateButton( "CHAT_NORMAL_CHECK", "CHAT_NORMAL_CHECK_F", CHAT_NORMAL_CHECK );
	m_pNormalButton->SetFlip( TRUE );
	m_pNormalTextBox = CreateTextBox( "CHAT_NORMAL_CHECK_TEXT", ID2GAMEWORD("CHAT_OPTION",0), CHAT_NORMAL_TEXTBOX );
	m_pNormalTextBox->SetUseTextColor( 0, TRUE );

	//	귓속말
	m_pPrivateButton = CreateButton( "CHAT_PRIVATE_CHECK", "CHAT_PRIVATE_CHECK_F", CHAT_PRIVATE_CHECK );
	m_pPrivateButton->SetFlip( TRUE );
	m_pPrivateTextBox = CreateTextBox( "CHAT_PRIVATE_CHECK_TEXT", ID2GAMEWORD("CHAT_OPTION",1), CHAT_PRIVATE_TEXTBOX );
	m_pPrivateTextBox->SetUseTextColor( 0, TRUE );

	//	파티
	m_pPartyButton = CreateButton( "CHAT_PARTY_CHECK", "CHAT_PARTY_CHECK_F", CHAT_PARTY_CHECK );
	m_pPartyButton->SetFlip( TRUE );
	m_pPartyTextBox = CreateTextBox( "CHAT_PARTY_CHECK_TEXT", ID2GAMEWORD("CHAT_OPTION",2), CHAT_PARTY_TEXTBOX );
	m_pPartyTextBox->SetUseTextColor( 0, TRUE );

	//	클럽
	m_pClubButton = CreateButton( "CHAT_CLUB_CHECK", "CHAT_CLUB_CHECK_F", CHAT_CLUB_CHECK );
	m_pClubButton->SetFlip( TRUE );
	m_pClubTextBox = CreateTextBox( "CHAT_CLUB_CHECK_TEXT", ID2GAMEWORD("CHAT_OPTION",3), CHAT_CLUB_TEXTBOX );
	m_pClubTextBox->SetUseTextColor( 0, TRUE );

	// 동맹
	m_pAllianceButton = CreateButton( "CHAT_ALLIANCE_CHECK", "CHAT_ALLIANCE_CHECK_F", CHAT_ALLIANCE_CHECK );
	m_pAllianceButton->SetFlip( TRUE );
	m_pAllianceTextBox = CreateTextBox( "CHAT_ALLIANCE_CHECK_TEXT", ID2GAMEWORD("CHAT_OPTION",4), CHAT_ALLIANCE_TEXTBOX );
	m_pAllianceTextBox->SetUseTextColor( 0, TRUE );

	//	지역채팅
	m_pAreaButton = CreateButton( "CHAT_AREA_CHECK", "CHAT_AREA_CHECK_F", CHAT_AREA_CHECK );
	m_pAreaButton->SetFlip( TRUE );
	m_pAreaTextBox = CreateTextBox( "CHAT_AREA_CHECK_TEXT", ID2GAMEWORD("CHAT_OPTION",5), CHAT_AREA_TEXTBOX );
	m_pAreaTextBox->SetUseTextColor( 0, TRUE );

	// 파티찾기
	m_pPartyRecruitButton = CreateButton( "CHAT_PARTY_RECRUIT_CHECK", "CHAT_PARTY_RECRUIT_CHECK_F", CHAT_PARTY_RECRUIT_CHECK );
	m_pPartyRecruitButton->SetFlip( TRUE );
	m_pPartyRecruitTextBox = CreateTextBox( "CHAT_PARTY_RECRUIT_CHECK_TEXT", ID2GAMEWORD("CHAT_OPTION",6), CHAT_PARTY_RECRUIT_TEXTBOX );
	m_pPartyRecruitTextBox->SetUseTextColor( 0, TRUE );

	//	시스템
	m_pSystemButton = CreateButton( "CHAT_SYSTEM_CHECK", "CHAT_SYSTEM_CHECK_F", CHAT_SYSTEM_CHECK );
	m_pSystemButton->SetFlip( TRUE );
	m_pSystemTextBox = CreateTextBox( "CHAT_SYSTEM_CHECK_TEXT", ID2GAMEWORD("CHAT_OPTION",8), CHAT_SYSTEM_TEXTBOX );
	m_pSystemTextBox->SetUseTextColor( 0, TRUE );

	// 확성기
	if ( GLUseFeatures::GetInstance().IsUsingMegaphoneControl() )
	{
		m_pToAllButton = CreateButton( "CHAT_TOALL_CHECK", "CHAT_TOALL_CHECK_F", CHAT_TOALL_CHECK );
	}
	else
	{
		m_pToAllButton = CreateButton( "CHAT_TOALL_CHECK", "CHAT_TOALL_CHECK_F_GRAY", CHAT_TOALL_CHECK );
	}
	m_pToAllButton->SetFlip( TRUE );
	m_pToAllTextBox = CreateTextBox( "CHAT_TOALL_CHECK_TEXT", ID2GAMEWORD("CHAT_OPTION",7), CHAT_TOALL_TEXTBOX );
	m_pToAllTextBox->SetUseTextColor( 0, TRUE );


	CBasicTextButton* pOKButton = new CBasicTextButton(m_pEngineDevice);
	pOKButton->CreateSub( this, "BASIC_TEXT_BUTTON19", UI_FLAG_XSIZE, CHAT_OK_BUTTON );
	pOKButton->CreateBaseButton( "CHAT_OK_BUTTON", nBUTTONSIZE, CBasicButton::CLICK_FLIP, (char*)ID2GAMEWORD( "CHAT_OK_BUTTON", 0 ) );
	RegisterControl( pOKButton );

	CBasicTextButton* pCancelButton = new CBasicTextButton(m_pEngineDevice);
	pCancelButton->CreateSub( this, "BASIC_TEXT_BUTTON19", UI_FLAG_XSIZE, CHAT_CANCEL_BUTTON );
	pCancelButton->CreateBaseButton( "CHAT_CANCEL_BUTTON", nBUTTONSIZE, CBasicButton::CLICK_FLIP, (char*)ID2GAMEWORD( "CHAT_OK_BUTTON", 1 ) );
	RegisterControl( pCancelButton );

	if ( !GLCONST_CHAR::bCHAT_EXTEND ) 
	{
		m_pAreaButton->SetVisibleSingle( FALSE );
		m_pAreaTextBox->SetVisibleSingle( FALSE );
		m_pPartyRecruitButton->SetVisibleSingle( FALSE );
		m_pPartyRecruitTextBox->SetVisibleSingle( FALSE );
	}

	UpdateChatFlag();
}

CBasicButton*	CChatOptionWindow::CreateButton( const CString& strKeyword, const CString& strFlipKeyword, const UIGUID& cID )
{
	CBasicButton* pButton = new CBasicButton(m_pEngineDevice);
	pButton->CreateSub( this, const_cast<CString&>(strKeyword).GetString(), UI_FLAG_DEFAULT, cID );
	pButton->CreateFlip( const_cast<CString&>(strFlipKeyword).GetString(), CBasicButton::RADIO_FLIP );
	RegisterControl( pButton );
	return pButton;
}

CBasicTextBox*	CChatOptionWindow::CreateTextBox( const CString& strKeyword, const CString& strText, const UIGUID& cID )
{
	CD3DFontPar* pFont = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	CBasicTextBox*	pTextBox = new CBasicTextBox(m_pEngineDevice);
	pTextBox->CreateSub( this, const_cast<CString&>(strKeyword).GetString(), UI_FLAG_DEFAULT, cID );
	pTextBox->SetFont( pFont );
	pTextBox->AddText( strText );
	RegisterControl( pTextBox );
	return pTextBox;
}

void CChatOptionWindow::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	if ( !IsVisible() ) return ;

	CUIWindowEx::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
/*
	//	원래는 필요없는 코드이나,
	//	CBasicButton에 버그로 인해,
	//	고치기전까지의 사용으로 임시적용
	{
		m_pNormalButton->SetVisibleSingle ( TRUE );
		m_pPrivateButton->SetVisibleSingle ( TRUE );
		m_pPartyButton->SetVisibleSingle ( TRUE );
		m_pClubButton->SetVisibleSingle ( TRUE );
		m_pAllianceButton->SetVisibleSingle( TRUE );
		m_pSystemButton->SetVisibleSingle ( TRUE );
		m_pToAllButton->SetVisibleSingle ( TRUE );
		m_pAreaButton->SetVisibleSingle ( TRUE );
		m_pPartyRecruitButton->SetVisibleSingle ( TRUE );
	}
*/

}

void CChatOptionWindow::SetVisibleSingle( BOOL bVisible )
{
	CUIWindowEx::SetVisibleSingle( bVisible );
	if ( bVisible ) 
	{
		InitFlipButton();
	}
	
}


void CChatOptionWindow::InitFlipButton()
{
	D3DCOLOR dwColorOn = NS_UITEXTCOLOR::WHITESMOKE;
	D3DCOLOR dwColorOFF = NS_UITEXTCOLOR::DARKGRAY;

	BOOL bFlip = FALSE;
	
	bFlip = ( m_dwChatFlag & CHAT_CONST::CHAT_NORMAL );	
	m_pNormalButton->SetFlip ( bFlip );
	if ( bFlip ) m_pNormalTextBox->SetTextColor ( 0, dwColorOn );
	else m_pNormalTextBox->SetTextColor ( 0, dwColorOFF );

	bFlip = ( m_dwChatFlag & CHAT_CONST::CHAT_PRIVATE );	
	m_pPrivateButton->SetFlip ( bFlip );
	if ( bFlip ) m_pPrivateTextBox->SetTextColor ( 0, dwColorOn );
	else m_pPrivateTextBox->SetTextColor ( 0, dwColorOFF );

	bFlip = ( m_dwChatFlag & CHAT_CONST::CHAT_PARTY );	
	m_pPartyButton->SetFlip ( bFlip );
	if ( bFlip ) m_pPartyTextBox->SetTextColor ( 0, dwColorOn );
	else m_pPartyTextBox->SetTextColor ( 0, dwColorOFF );

	bFlip = ( m_dwChatFlag & CHAT_CONST::CHAT_GUILD );	
	m_pClubButton->SetFlip ( bFlip );
	if ( bFlip ) m_pClubTextBox->SetTextColor ( 0, dwColorOn );
	else m_pClubTextBox->SetTextColor ( 0, dwColorOFF );

	bFlip = ( m_dwChatFlag & CHAT_CONST::CHAT_ALLIANCE );	
	m_pAllianceButton->SetFlip ( bFlip );
	if ( bFlip ) m_pAllianceTextBox->SetTextColor ( 0, dwColorOn );
	else m_pAllianceTextBox->SetTextColor ( 0, dwColorOFF );

	bFlip = ( m_dwChatFlag & CHAT_CONST::CHAT_AREA );	
	m_pAreaButton->SetFlip ( bFlip );
	if ( bFlip ) m_pAreaTextBox->SetTextColor ( 0, dwColorOn );
	else m_pAreaTextBox->SetTextColor ( 0, dwColorOFF );

	bFlip = ( m_dwChatFlag & CHAT_CONST::CHAT_PARTY_RECRUIT );	
	m_pPartyRecruitButton->SetFlip ( bFlip );
	if ( bFlip ) m_pPartyRecruitTextBox->SetTextColor ( 0, dwColorOn );
	else m_pPartyRecruitTextBox->SetTextColor ( 0, dwColorOFF );

	bFlip = ( m_dwChatFlag & CHAT_CONST::CHAT_TOALL );	
	m_pToAllButton->SetFlip ( bFlip );
	if ( bFlip ) m_pToAllTextBox->SetTextColor ( 0, dwColorOn );
	else m_pToAllTextBox->SetTextColor ( 0, dwColorOFF );

	bFlip = ( m_dwChatFlag & CHAT_CONST::CHAT_SYSTEM );	
	m_pSystemButton->SetFlip ( bFlip );
	if ( bFlip ) m_pSystemTextBox->SetTextColor ( 0, dwColorOn );
	else m_pSystemTextBox->SetTextColor ( 0, dwColorOFF );	
}

void CChatOptionWindow::UpdateChatFlag()
{
	m_dwChatFlag = CHAT_CONST::CHAT_NO;
	
	if ( m_pNormalButton->IsFlip() )		m_dwChatFlag |= CHAT_CONST::CHAT_NORMAL;
	if ( m_pPrivateButton->IsFlip() )		m_dwChatFlag |= CHAT_CONST::CHAT_PRIVATE;
	if ( m_pPartyButton->IsFlip() )			m_dwChatFlag |= CHAT_CONST::CHAT_PARTY;
	if ( m_pClubButton->IsFlip() )			m_dwChatFlag |= CHAT_CONST::CHAT_GUILD;
	if ( m_pAllianceButton->IsFlip() )		m_dwChatFlag |= CHAT_CONST::CHAT_ALLIANCE;
	if ( m_pSystemButton->IsFlip() )		m_dwChatFlag |= CHAT_CONST::CHAT_SYSTEM;
	if ( m_pToAllButton->IsFlip() )			m_dwChatFlag |= CHAT_CONST::CHAT_TOALL;
	if ( m_pAreaButton->IsFlip() )			m_dwChatFlag |= CHAT_CONST::CHAT_AREA;
	if ( m_pPartyRecruitButton->IsFlip() )	m_dwChatFlag |= CHAT_CONST::CHAT_PARTY_RECRUIT;
}

void CChatOptionWindow::TranslateUIMessage( UIGUID cID, DWORD dwMsg )
{
	CUIWindowEx::TranslateUIMessage( cID, dwMsg );

	switch ( cID )
	{
	case CHAT_NORMAL_TEXTBOX:
	case CHAT_NORMAL_CHECK:
		{
			if ( CHECK_MOUSE_IN( dwMsg ) )
			{
				if ( UIMSG_LB_DOWN & dwMsg )
				{
					BOOL bFlip = !m_pNormalButton->IsFlip();
					m_pNormalButton->SetFlip( bFlip );

					D3DCOLOR dwColor = NS_UITEXTCOLOR::DARKGRAY;
					if ( bFlip ) dwColor = NS_UITEXTCOLOR::WHITESMOKE;					

					m_pNormalTextBox->SetTextColor( 0, dwColor );
				}
			}
		}
		break;

	case CHAT_PRIVATE_TEXTBOX:
	case CHAT_PRIVATE_CHECK:
		{
			if ( CHECK_MOUSE_IN( dwMsg ) )
			{
				if ( UIMSG_LB_DOWN & dwMsg )
				{
					BOOL bFlip = !m_pPrivateButton->IsFlip();
					m_pPrivateButton->SetFlip( bFlip );

					D3DCOLOR dwColor = NS_UITEXTCOLOR::DARKGRAY;
					if ( bFlip ) dwColor = NS_UITEXTCOLOR::WHITESMOKE;	

					m_pPrivateTextBox->SetTextColor( 0, dwColor );
				}
			}
		}
		break;
		
	case CHAT_PARTY_TEXTBOX:
	case CHAT_PARTY_CHECK:
		{
			if ( CHECK_MOUSE_IN( dwMsg ) )
			{
				if ( UIMSG_LB_DOWN & dwMsg )
				{
					BOOL bFlip = !m_pPartyButton->IsFlip();
					m_pPartyButton->SetFlip( bFlip );

					D3DCOLOR dwColor = NS_UITEXTCOLOR::DARKGRAY;
					if ( bFlip ) dwColor = NS_UITEXTCOLOR::WHITESMOKE;					

					m_pPartyTextBox->SetTextColor( 0, dwColor );
				}
			}
		}
		break;

	case CHAT_CLUB_TEXTBOX:
	case CHAT_CLUB_CHECK:
		{
			if ( CHECK_MOUSE_IN( dwMsg ) )
			{
				if ( UIMSG_LB_DOWN & dwMsg )
				{
					BOOL bFlip = !m_pClubButton->IsFlip();
					m_pClubButton->SetFlip( bFlip );

					D3DCOLOR dwColor = NS_UITEXTCOLOR::DARKGRAY;
					if ( bFlip ) dwColor = NS_UITEXTCOLOR::WHITESMOKE;					

					m_pClubTextBox->SetTextColor( 0, dwColor );
				}
			}
		}
		break;

	case CHAT_ALLIANCE_CHECK:
	case CHAT_ALLIANCE_TEXTBOX:
		{
			if ( CHECK_MOUSE_IN( dwMsg ) )
			{
				if ( UIMSG_LB_DOWN & dwMsg )
				{
					BOOL bFlip = !m_pAllianceButton->IsFlip();
					m_pAllianceButton->SetFlip( bFlip );

					D3DCOLOR dwColor = NS_UITEXTCOLOR::DARKGRAY;
					if ( bFlip ) dwColor = NS_UITEXTCOLOR::WHITESMOKE;					

					m_pAllianceTextBox->SetTextColor( 0, dwColor );
				}
			}
		}
		break;

	case CHAT_SYSTEM_CHECK:
	case CHAT_SYSTEM_TEXTBOX:
		{
			if ( CHECK_MOUSE_IN( dwMsg ) )
			{
				if ( UIMSG_LB_DOWN & dwMsg )
				{
					BOOL bFlip = !m_pSystemButton->IsFlip();
					m_pSystemButton->SetFlip( bFlip );

					D3DCOLOR dwColor = NS_UITEXTCOLOR::DARKGRAY;
					if ( bFlip ) dwColor = NS_UITEXTCOLOR::WHITESMOKE;					

					m_pSystemTextBox->SetTextColor( 0, dwColor );
				}
			}
		}
		break;
	case CHAT_TOALL_CHECK:
	case CHAT_TOALL_TEXTBOX:
		{
			if ( !GLUseFeatures::GetInstance().IsUsingMegaphoneControl() )
				break;

			if ( CHECK_MOUSE_IN( dwMsg ) )
			{
				if ( UIMSG_LB_DOWN & dwMsg )
				{
					BOOL bFlip = !m_pToAllButton->IsFlip();
					m_pToAllButton->SetFlip( bFlip );

					D3DCOLOR dwColor = NS_UITEXTCOLOR::DARKGRAY;
					if ( bFlip ) dwColor = NS_UITEXTCOLOR::WHITESMOKE;					

					m_pToAllTextBox->SetTextColor( 0, dwColor );
				}
			}
		}
		break;
	case CHAT_AREA_CHECK:
	case CHAT_AREA_TEXTBOX:
		{
			if ( CHECK_MOUSE_IN( dwMsg ) )
			{
				if ( UIMSG_LB_DOWN & dwMsg )
				{
					BOOL bFlip = !m_pAreaButton->IsFlip();
					m_pAreaButton->SetFlip( bFlip );

					D3DCOLOR dwColor = NS_UITEXTCOLOR::DARKGRAY;
					if ( bFlip ) dwColor = NS_UITEXTCOLOR::WHITESMOKE;					

					m_pAreaTextBox->SetTextColor( 0, dwColor );
				}
			}
		}
		break;
	case CHAT_PARTY_RECRUIT_CHECK:
	case CHAT_PARTY_RECRUIT_TEXTBOX:
		{
			if ( CHECK_MOUSE_IN( dwMsg ) )
			{
				if ( UIMSG_LB_DOWN & dwMsg )
				{
					BOOL bFlip = !m_pPartyRecruitButton->IsFlip();
					m_pPartyRecruitButton->SetFlip( bFlip );

					D3DCOLOR dwColor = NS_UITEXTCOLOR::DARKGRAY;
					if ( bFlip ) dwColor = NS_UITEXTCOLOR::WHITESMOKE;					

					m_pPartyRecruitTextBox->SetTextColor( 0, dwColor );
				}
			}
		}
		break;
	case CHAT_OK_BUTTON:
		{
			if (CHECK_MOUSEIN_LBUPLIKE(dwMsg))
			{
				UpdateChatFlag();
                WORD wChatFlag = static_cast<WORD>(m_dwChatFlag);
				m_pInterface->SetChatOption(wChatFlag);
				m_pInterface->UiHideGroup(GetWndID());
			}
		}
		break;
	case CHAT_CANCEL_BUTTON:
	case ET_CONTROL_BUTTON:
		{	
			if ( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			{
				m_pInterface->UiHideGroup( GetWndID() );
			}
		}
		break;
	}
}

MyChatOptionWindow::MyChatOptionWindow( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
    : CChatOptionWindow( pInterface, pEngineDevice )
{
}

void MyChatOptionWindow::CreateUIWindowAndRegisterOwnership()
{
    CChatOptionWindow::Create( CHAT_OPTION_WINDOW, "BASIC_WINDOW", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
    CChatOptionWindow::CreateBaseWindowBlack( "CHAT_OPTION_WINDOW", (char*)ID2GAMEWORD("CHAT_OPTION_WINDOW_STATIC",0) );
    CChatOptionWindow::CreateSubControl();
    CChatOptionWindow::SetAlignFlag( UI_FLAG_BOTTOM | UI_FLAG_LEFT );
    CChatOptionWindow::m_pInterface->UiRegisterControl( this, true );
    CChatOptionWindow::m_pInterface->UiShowGroupFocus( CHAT_OPTION_WINDOW );
}