#include "StdAfx.h"
#include "ChatMacroWindow.h"
#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/BasicLineBox.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/UIEditBox.h"
#include "../../InnerInterface.h"
#include "../../../RanLogic/RANPARAM.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CChatMacroWindow::CChatMacroWindow( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
    : CUIWindowEx( pInterface, pEngineDevice )
{
}

CChatMacroWindow::~CChatMacroWindow()
{
}

void CChatMacroWindow::SetChatMacro()
{	
	for (int i = 0; i<CHAT_MACRO_SIZE; ++i)
	{
		RANPARAM::ChatMacro[i] = m_pEditBox[i]->GetEditString();
	}
}


void CChatMacroWindow::GetChatMacro(){
	
	for(int i = 0; i< CHAT_MACRO_SIZE; ++i){
		m_ChatMacro[i] = RANPARAM::ChatMacro[i];
		m_pEditBox[i]->SetEditString( m_ChatMacro[i] );	
	}
}

void CChatMacroWindow::CreateSubControl()
{
	const int nBUTTONSIZE = CBasicTextButton::SIZE19;
	CUIEditBox* pEditBox = NULL;
	CBasicLineBox* pBasicLineBox = NULL;
	CBasicTextBox* pTextBox = NULL;

	m_pFont9 = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );
	
	DWORD dwFontColor = NS_UITEXTCOLOR::DEFAULT;
	int nAlign = TEXT_ALIGN_LEFT;

	//	배경
	pBasicLineBox = new CBasicLineBox(m_pEngineDevice);
	pBasicLineBox->CreateSub( this, "BASIC_LINE_BOX_OPTION", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pBasicLineBox->CreateBaseBoxOption( "CHATMACRO_WINDOW_BACK_BOX" );
	RegisterControl( pBasicLineBox );


	// 확인 버튼 
	CBasicTextButton* pOKButton = new CBasicTextButton(m_pEngineDevice);
	pOKButton->CreateSub( this, "BASIC_TEXT_BUTTON19", UI_FLAG_XSIZE, CHAT_MACRO_OK );
	pOKButton->CreateBaseButton( "CHATMACRO_OKBUTTON", nBUTTONSIZE, CBasicButton::CLICK_FLIP, (char*)ID2GAMEWORD ( "CHATMACRO_OKBUTTON", 0 ) );
	RegisterControl( pOKButton );	


	int i = 0;

	char strTemp[50];
	
	// 채팅 매크로
	for(i = 0; i < CHAT_MACRO_SIZE; ++i){

		_stprintf_s( strTemp, 50, "CHAT_MACRO_KEYTEXT%d", i );
		pTextBox = CreateStaticControl( strTemp, m_pFont9, dwFontColor, nAlign );
		pTextBox->SetOneLineText( ID2GAMEWORD ( "CHATMACRO_KEYTEXT", i ) );
	}

	// EditBox배경 생성
	for(i = 0; i < CHAT_MACRO_SIZE; ++i){
		_stprintf_s( strTemp, 50, "CHAT_MACRO_EDIT_BACK%d", i );
		pBasicLineBox = new CBasicLineBox(m_pEngineDevice);
		pBasicLineBox->CreateSub( this, "BASIC_LINE_BOX_EDIT", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pBasicLineBox->CreateBaseBoxEditBox( strTemp);
		RegisterControl( pBasicLineBox );        
	}

    // EditBox 생성
	for(i = 0; i < CHAT_MACRO_SIZE; ++i){
		_stprintf_s( strTemp, 50, "CHAT_MACRO_EDIT%d", i );
		pEditBox = new CUIEditBox(m_pEngineDevice);
		pEditBox->CreateSub( this, strTemp, UI_FLAG_XSIZE | UI_FLAG_BOTTOM, CHAT_MACRO_EDIT0+i );
		pEditBox->CreateCarrat( "CHATMACRO_CARRAT", TRUE, UINT_MAX );
		pEditBox->SetFont( m_pFont9 );
		pEditBox->SetLimitInput( CHATMACRO_EDIT_LIMIT );
		RegisterControl( pEditBox );
		m_pEditBox[i] = pEditBox;
	}
}


void CChatMacroWindow::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	// EditBox
	int result = 0;
	CString strTemp;
	if( ControlID >= CHAT_MACRO_EDIT0 && ControlID <= CHAT_MACRO_EDIT9)
	{
		if ( CHECK_MOUSE_IN( dwMsg ) )
		{
			if ( UIMSG_LB_UP & dwMsg )
			{
				CString strName = m_pEditBox[ControlID - CHAT_MACRO_EDIT0]->GetEditString();
				EDIT_BEGIN(ControlID - CHAT_MACRO_EDIT0, strName);				
			}
		}
		else
		{
			if ( UIMSG_LB_DOWN & dwMsg || UIMSG_LB_UP & dwMsg )
				EDIT_END(ControlID - CHAT_MACRO_EDIT0);
		}
	}
	else
	{
		switch ( ControlID )
		{
		case ET_CONTROL_TITLE:
		case ET_CONTROL_TITLE_F:
			{
				if ( (dwMsg & UIMSG_LB_DUP) && CHECK_MOUSE_IN( dwMsg ) )
				{
					m_pInterface->SetDefaultPosInterface( CHATMACRO_WINDOW );
				}
				break;
			}			
		case CHAT_MACRO_OK:
			{
				if( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
				{
					SetChatMacro();
					m_pInterface->UiHideGroup( GetWndID() );
				}
				break;
			}		
		case ET_CONTROL_BUTTON:
			{
				if ( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
				{
					m_pInterface->UiHideGroup( GetWndID() );
				}
				break;
			}
		}
	}
	
	CUIWindowEx::TranslateUIMessage ( ControlID, dwMsg );
}
void CChatMacroWindow::EDIT_BEGIN(int num, const CString& strName)
{	
	if( m_pEditBox[num])
	{
		m_pEditBox[num]->EndEdit();
		m_pEditBox[num]->ClearEdit();

		m_pEditBox[num]->SetEditString(strName);
		m_pEditBox[num]->BeginEdit();	
	}
}

void CChatMacroWindow::EDIT_END(int num)
{
	if( m_pEditBox[num] ){
		m_ChatMacro[num] = m_pEditBox[num]->GetEditString();				
		m_pEditBox[num]->EndEdit();
	}
}


MyChatMacroWindow::MyChatMacroWindow( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
    : CChatMacroWindow( pInterface, pEngineDevice )
{
}

void MyChatMacroWindow::CreateUIWindowAndRegisterOwnership()
{
    CChatMacroWindow::Create( CHATMACRO_WINDOW, "BASIC_WINDOW", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
    CChatMacroWindow::CreateBaseWindowBlack( "CHATMACRO_WINDOW", (char*)ID2GAMEWORD("CHATMACRO_WINDOW_NAME_STATIC") );
    CChatMacroWindow::CreateSubControl();
    CChatMacroWindow::SetControlNameEx( "채팅매크로" );
    CChatMacroWindow::SetAlignFlag( UI_FLAG_CENTER_Y | UI_FLAG_CENTER_X );
    CChatMacroWindow::m_pInterface->UiRegisterControl( this, true );
    CChatMacroWindow::m_pInterface->UiShowGroupFocus( CHATMACRO_WINDOW );
}

void MyChatMacroWindow::GetChatMacro()
{
    CChatMacroWindow::GetChatMacro();
}