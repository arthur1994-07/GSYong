#include "StdAfx.h"
#include "SMSSendWindow.h"
#include "FriendWindow.h"


#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../EngineLib/GUInterface/UIMultiEditBox.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicLineBox.h"
#include "../../../EngineLib/DxTools/d3dfont.h"
#include "../../InnerInterface.h"

#include "../../../RanLogic/Msg/GLContrlCharJoinMsg.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"
#include "../../../RanLogicClient/GLGaeaClient.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CSMSSendWindow::CSMSSendWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
	: CUIWindowEx ( pInterface, pEngineDevice )
	, m_pGaeaClient ( pGaeaClient )
	, m_pMultiEditBox(NULL)
	, m_pNameTextBox(NULL)
{
}

CSMSSendWindow::~CSMSSendWindow(void)
{
}

void CSMSSendWindow::CreateSubControl()
{
	CD3DFontPar* pFont9 = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	{
		CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
		pTextBox->CreateSub( this, "SMS_SEND_TO_TEXTBOX" );
		pTextBox->SetFont( pFont9 );
		pTextBox->AddText( (char*)ID2GAMEWORD("FRIEND_BUTTON",14) );
		RegisterControl( pTextBox );

		pTextBox = new CBasicTextBox(m_pEngineDevice);
		pTextBox->CreateSub( this, "SMS_SEND_TO_NAME_TEXTBOX" );
		pTextBox->SetFont( pFont9 );		
		RegisterControl( pTextBox );
		m_pNameTextBox = pTextBox;
	}

	{
		CBasicLineBox* pBasicLineBox = new CBasicLineBox(m_pEngineDevice);
		pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_QUEST_LIST", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pBasicLineBox->CreateBaseBoxQuestList ( "SMS_SEND_LINEBOX" );
		RegisterControl ( pBasicLineBox );		

		CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
		pTextBox->CreateSub ( this, "SMS_SEND_STATIC" );
		pTextBox->SetFont ( pFont9 );
		RegisterControl ( pTextBox );
		pTextBox->AddText( ID2GAMEINTEXT( "SMS_SEND_STATIC" ) );
	}

	{
		CBasicLineBox* pLineBox = new CBasicLineBox(m_pEngineDevice);
		pLineBox->CreateSub( this, "BASIC_LINE_BOX_MINIPARTY", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pLineBox->CreateBaseBoxSystemMessageUp( "SMS_SEND_LINE_TEXTBOX" );
		pLineBox->ResetAlignFlag ();
		RegisterControl( pLineBox );
	}

	{
		CUIMultiEditBox* pMultiEditBox = new CUIMultiEditBox(m_pEngineDevice);
		pMultiEditBox->CreateSub( this, "SMS_SEND_EDITBOX" );
		pMultiEditBox->CreateEditBox( SMS_SEND_EDITBOX_L01, "SMS_SEND_EDITBOX_L01", "WRITE_NOTE_CARRAT", TRUE, UINT_MAX, pFont9 );
		pMultiEditBox->CreateEditBox( SMS_SEND_EDITBOX_L02, "SMS_SEND_EDITBOX_L02", "WRITE_NOTE_CARRAT", TRUE, UINT_MAX, pFont9 );
		RegisterControl( pMultiEditBox );
		m_pMultiEditBox = pMultiEditBox;
	}

	{
		CBasicTextButton* pButton = new CBasicTextButton(m_pEngineDevice);
		pButton->CreateSub( this, "BASIC_TEXT_BUTTON14", UI_FLAG_XSIZE, SMS_SEND_BUTTON );
		pButton->CreateBaseButton( "SMS_SEND_BUTTON", CBasicTextButton::SIZE14, CBasicButton::CLICK_FLIP, (char*)ID2GAMEWORD("FRIEND_BUTTON",12) );
		RegisterControl( pButton );

		pButton = new CBasicTextButton(m_pEngineDevice);
		pButton->CreateSub( this, "BASIC_TEXT_BUTTON14", UI_FLAG_XSIZE, SMS_CANCEL_BUTTON );
		pButton->CreateBaseButton( "SMS_CANCEL_BUTTON", CBasicTextButton::SIZE14, CBasicButton::CLICK_FLIP, (char*)ID2GAMEWORD("FRIEND_BUTTON",13) );
		RegisterControl( pButton );
	}
}

void CSMSSendWindow::TranslateUIMessage( UIGUID cID, DWORD dwMsg )
{
	CUIWindowEx::TranslateUIMessage( cID, dwMsg );

	switch ( cID )
	{
	case SMS_SEND_BUTTON:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				CString strNotice;
				m_pMultiEditBox->GetText( strNotice );

				//std::tr1::shared_ptr<SFRIEND> sFriend = m_pInterface->GetFriendWindow()->GetFriendSMSInfo();
				//m_pGaeaClient->GetCharacter()->ReqSendSMS( sFriend.nCharID, sFriend.szPhoneNumber, strNotice );

				m_pInterface->UiHideGroup( GetWndID() );
			}
		}
		break;

	case SMS_CANCEL_BUTTON:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				m_pInterface->UiHideGroup( GetWndID() );
			}
		}
		break;
	}
}

void CSMSSendWindow::EDIT_BEGIN()
{
	m_pMultiEditBox->ClearEdit();
	m_pMultiEditBox->Init ();
	m_pMultiEditBox->EndEdit ();
	m_pMultiEditBox->BeginEdit ();
}

void CSMSSendWindow::EDIT_END()
{
	m_pMultiEditBox->EndEdit();
}

void CSMSSendWindow::SetVisibleSingle( BOOL bVisible )
{
	CUIWindowEx::SetVisibleSingle( bVisible );

	if( bVisible )
	{
		const CString& strName = UI::ToString( m_pInterface->GetFriendWindow()->GetFriendName() );
		m_pNameTextBox->AddText( strName );

		EDIT_BEGIN();
	}
	else
	{
		m_pNameTextBox->ClearText();

		EDIT_END();
	}
}