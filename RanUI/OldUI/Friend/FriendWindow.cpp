#include "StdAfx.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../EngineLib/GUInterface/BasicLineBox.h"

#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"

#include "../../InnerInterface.h"

#include "FriendWindowBlockPage.h"
#include "FriendWindowNormalPage.h"
#include "InBoxWindow.h"
#include "SentWindow.h"
#include "ReceiveNoteWindow.h"
#include "WriteNoteWindow.h"
#include "FriendWindow.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CFriendWindow::CFriendWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
	: CUIWindowEx( pInterface, pEngineDevice )
	, m_pGaeaClient( pGaeaClient )
	, m_pNORMAL_TAB_BUTTON( NULL )
    , m_pBLOCK_TAB_BUTTON( NULL )
	, m_pRECEIVE_TAB_BUTTON( NULL )
	, m_pSEND_TAB_BUTTON( NULL )
	, m_pBlockPage( NULL )
	, m_pNormalPage( NULL )
	, m_pInBoxPage( NULL )
	, m_pSentPage( NULL )
{
}

CFriendWindow::~CFriendWindow()
{
}

void CFriendWindow::CreateSubControl()
{
	m_pNORMAL_TAB_BUTTON = CreateTextButton19( "FRIEND_NORMAL_TAB_BUTTON", FRIEND_NORMAL_TAB_BUTTON, (char*)ID2GAMEWORD("FRIEND_TAB_BUTTON",0) );
	m_pBLOCK_TAB_BUTTON	 = CreateTextButton19( "FRIEND_BLOCK_TAB_BUTTON", FRIEND_BLOCK_TAB_BUTTON, (char*)ID2GAMEWORD("FRIEND_TAB_BUTTON",1) );
	m_pRECEIVE_TAB_BUTTON = CreateTextButton19( "FRIEND_RECEIVE_TAB_BUTTON", RECEIVE_TAB_BUTTON, (char*)ID2GAMEWORD("FRIEND_TAB_BUTTON",2) );
	m_pRECEIVE_TAB_BUTTON->SetVisibleSingle( FALSE );
	m_pSEND_TAB_BUTTON	= CreateTextButton19( "FRIEND_SEND_TAB_BUTTON", SEND_TAB_BUTTON, (char*)ID2GAMEWORD("FRIEND_TAB_BUTTON",3) );
	m_pSEND_TAB_BUTTON->SetVisibleSingle( FALSE );

	{	// 텍스트 박스 배경
		CBasicLineBox* pBasicLineBox = new CBasicLineBox(m_pEngineDevice);
		pBasicLineBox->CreateSub( this, "BASIC_LINE_BOX_QUEST_LIST", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pBasicLineBox->CreateBaseBoxQuestList( "FRIEND_LINEBOX" );
		RegisterControl( pBasicLineBox );		
	}

	m_pNormalPage = new CFriendWindowNormalPage( m_pGaeaClient, m_pInterface, m_pEngineDevice );
	m_pNormalPage->CreateSub( this, "FRIEND_NORMAL_PAGE", UI_FLAG_DEFAULT, FRIEND_NORMAL_PAGE );
	m_pNormalPage->CreateSubControl();
	RegisterControl( m_pNormalPage );

	m_pBlockPage = new CFriendWindowBlockPage( m_pGaeaClient, m_pInterface, m_pEngineDevice );
	m_pBlockPage->CreateSub( this, "FRIEND_BLOCK_PAGE", UI_FLAG_DEFAULT, FRIEND_BLOCK_PAGE );
	m_pBlockPage->CreateSubControl();
	RegisterControl( m_pBlockPage );

	m_pInBoxPage = new CInBoxWindow(m_pEngineDevice);
	m_pInBoxPage->CreateSub( this, "FRIEND_INBOX_PAGE", UI_FLAG_DEFAULT, FRIEND_INBOX_PAGE );
	m_pInBoxPage->CreateSubControl();
	RegisterControl( m_pInBoxPage );

	m_pSentPage = new CSentWindow(m_pEngineDevice);
	m_pSentPage->CreateSub( this, "FRIEND_SENT_PAGE", UI_FLAG_DEFAULT, FRIEND_SENT_PAGE );
	m_pSentPage->CreateSubControl();
	RegisterControl( m_pSentPage );

	ChangePage( FRIEND_NORMAL_TAB_BUTTON );
}

CBasicTextButton* CFriendWindow::CreateTextButton19( char* szButton, UIGUID ControlID , char* szText )
{
	CBasicTextButton* pTextButton = new CBasicTextButton(m_pEngineDevice);
	pTextButton->CreateSub( this, "BASIC_TEXT_BUTTON191", UI_FLAG_XSIZE, ControlID );
	pTextButton->CreateBaseButton( szButton, CBasicTextButton::SIZE19_RECT, CBasicButton::RADIO_FLIP, szText );
	pTextButton->SetAlignFlag( UI_FLAG_BOTTOM );
	pTextButton->SetFlip( TRUE );
	RegisterControl( pTextButton );

	return pTextButton;
}

void CFriendWindow::TranslateUIMessage( UIGUID cID, DWORD dwMsg )
{
	CUIWindowEx::TranslateUIMessage( cID, dwMsg );

	switch ( cID )
	{
	case ET_CONTROL_TITLE:
	case ET_CONTROL_TITLE_F:
		{
			if ( (dwMsg & UIMSG_LB_DUP) && CHECK_MOUSE_IN( dwMsg ) )
			{
				m_pInterface->SetDefaultPosInterface( FRIEND_WINDOW );
			}
		}
		break;
	case FRIEND_NORMAL_TAB_BUTTON:
	case FRIEND_BLOCK_TAB_BUTTON:
	case RECEIVE_TAB_BUTTON:
	case SEND_TAB_BUTTON:
		{
			if ( CHECK_MOUSE_IN( dwMsg ) )
			{
				if ( UIMSG_LB_DOWN & dwMsg ) ChangePage( cID );
			}
		}
		break;
	}
}

void CFriendWindow::ChangePage( const UIGUID& cClickButton )
{
	{
		m_pNORMAL_TAB_BUTTON->SetFlip( FALSE );
		m_pBLOCK_TAB_BUTTON->SetFlip( FALSE );
		m_pRECEIVE_TAB_BUTTON->SetFlip( FALSE );
		m_pSEND_TAB_BUTTON->SetFlip( FALSE );
	}

	{
		m_pNormalPage->SetVisibleSingle( FALSE );
		m_pBlockPage->SetVisibleSingle( FALSE );
		m_pInBoxPage->SetVisibleSingle( FALSE );
		m_pSentPage->SetVisibleSingle( FALSE );
	}

	CBasicTextButton* pVisibleButton( NULL );
	CUIControl* pVisiblePage( NULL );

	switch ( cClickButton )
	{
	case FRIEND_NORMAL_TAB_BUTTON:
		{
			pVisibleButton = m_pNORMAL_TAB_BUTTON;
			pVisiblePage = m_pNormalPage;
		}
		break;

	case FRIEND_BLOCK_TAB_BUTTON:
		{
			pVisibleButton = m_pBLOCK_TAB_BUTTON;
			pVisiblePage = m_pBlockPage;
		}
		break;

	case RECEIVE_TAB_BUTTON:
		{
			pVisibleButton = m_pRECEIVE_TAB_BUTTON;
			pVisiblePage = m_pInBoxPage;
		}
		break;

	case SEND_TAB_BUTTON:
		{
			pVisibleButton = m_pSEND_TAB_BUTTON;
			pVisiblePage = m_pSentPage;
		}
		break;
	}

	if ( pVisibleButton && pVisiblePage )
	{
		pVisibleButton->SetFlip( TRUE );
		pVisiblePage->SetVisibleSingle( TRUE );
	}
}

void  CFriendWindow::LoadFriendList()
{
	if ( m_pNormalPage->IsVisible() )	m_pNormalPage->LoadFriendList();
	else								m_pBlockPage->LoadBlockList();
}

void CFriendWindow::ADD_FRIEND_NAME_TO_EDITBOX( const CString& strName )
{
	m_pNormalPage->ADD_FRIEND_NAME_TO_EDITBOX( strName );
	m_pBlockPage->ADD_NAME_TO_EDITBOX( strName );
}

void CFriendWindow::ADD_FRIEND( const CString& strName )
{
	m_pNormalPage->ADD_FRIEND( strName );
}

const CString& CFriendWindow::GET_FRIEND_NAME() const
{
	return m_pNormalPage->GET_FRIEND_NAME();
}

const CString& CFriendWindow::GET_BLOCK_NAME() const
{
	return m_pBlockPage->GET_BLOCK_NAME();
}

void CFriendWindow::EDIT_END()
{
	m_pNormalPage->EDIT_END();
	m_pBlockPage->EDIT_END();
}

void CFriendWindow::SetVisibleSingle( BOOL bVisible )
{
	CUIGroup::SetVisibleSingle( bVisible );

	if( bVisible )
	{
		//m_pGaeaClient->GetCharacter()->ReqFriendWindowOpen( true );

		if( m_pNormalPage->IsVisible() )		m_pNormalPage->SetVisibleSingle( TRUE );
		else if( m_pBlockPage->IsVisible() )	m_pBlockPage->SetVisibleSingle( TRUE );
		else if( m_pInBoxPage->IsVisible() )	m_pInBoxPage->SetVisibleSingle( TRUE );
		else if( m_pSentPage->IsVisible() )		m_pSentPage->SetVisibleSingle( TRUE );
	}
	else
	{
		//m_pGaeaClient->GetCharacter()->ReqFriendWindowOpen( false );
	}
}

std::tr1::shared_ptr<SFRIEND> CFriendWindow::GetFriendSMSInfo()
{ 
	return m_pNormalPage->GetFriendInfo(); 
}

MyFriendWindow::MyFriendWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
    : CFriendWindow( pGaeaClient, pInterface, pEngineDevice )
{
}

void MyFriendWindow::CreateUIWindowAndRegisterOwnership()
{
    CFriendWindow::Create( FRIEND_WINDOW, "BASIC_WINDOW", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
    CFriendWindow::CreateBaseWindowBlack( "FRIEND_WINDOW", (char*)ID2GAMEWORD("FRIEND_WINDOW_NAME_STATIC") );
    CFriendWindow::CreateSubControl();
    CFriendWindow::SetAlignFlag( UI_FLAG_CENTER_Y | UI_FLAG_RIGHT );
    CFriendWindow::m_pInterface->UiRegisterControl( this, true );
    CFriendWindow::m_pInterface->UiShowGroupFocus( FRIEND_WINDOW );
}

void MyFriendWindow::AddFriendName_To_EditBox( const UI::String& strName )
{
    CFriendWindow::ADD_FRIEND_NAME_TO_EDITBOX( UI::ToString( strName ) );
}

void MyFriendWindow::AddFriend( const UI::String& strName )
{
    CFriendWindow::ADD_FRIEND( UI::ToString( strName ) );
}

void MyFriendWindow::LoadFriendList()
{
    CFriendWindow::LoadFriendList();
}

UI::String MyFriendWindow::GetFriendName() const
{
    return UI::ToString( CFriendWindow::GET_FRIEND_NAME() );
}

UI::String MyFriendWindow::GetSelectBlockFriendName() const
{
    return UI::ToString( CFriendWindow::GET_BLOCK_NAME() );
}

void MyFriendWindow::EndEditFriendList()
{
    CFriendWindow::EDIT_END();
}

std::tr1::shared_ptr<SFRIEND> MyFriendWindow::GetFriendSMSInfo()
{
    return CFriendWindow::GetFriendSMSInfo();
}