#include "StdAfx.h"

#include "../../../EngineLib/DxTools/d3dfont.h"

#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/BasicButton.h"

#include "../../InnerInterface.h"

#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicLineBox.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"

#include "PostBoxWindowMain.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CPostBoxWindowMain::CPostBoxWindowMain ( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
	: CPostBoxWindowObject ( pInterface, pEngineDevice )
	, m_pButton_Close ( NULL )
{

}

CPostBoxWindowMain::~CPostBoxWindowMain ()
{
	// Blank
}

void CPostBoxWindowMain::CreateSubControlEx ()
{	
	// Note : 페이지 프레임
	{
		CreatePageFrame( "POSTBOX_WINDOW_MAIN_PAGE" );
	}

	// Note : 탭 버튼
	{ 
		CTapSelector::CreateTapButton ( this, "BASIC_TEXT_BUTTON19", "POSTBOX_WINDOW_MAIN_BUTTON_TAP_RECEIVE", (char*)ID2GAMEWORD( "POSTBOX_BUTTON", 0 ),
			CBasicTextButton::SIZE19, POSTBOX_WINDOW_MAIN_BUTTON_TAP_RECEIVE, m_pEngineDevice );

		CTapSelector::CreateTapButton ( this, "BASIC_TEXT_BUTTON19", "POSTBOX_WINDOW_MAIN_BUTTON_TAP_SEND",	   (char*)ID2GAMEWORD( "POSTBOX_BUTTON", 1 ),
			CBasicTextButton::SIZE19, POSTBOX_WINDOW_MAIN_BUTTON_TAP_SEND, m_pEngineDevice );

		CTapSelector::CreateTapButton ( this, "BASIC_TEXT_BUTTON19", "POSTBOX_WINDOW_MAIN_BUTTON_TAP_HISTORY", (char*)ID2GAMEWORD( "POSTBOX_BUTTON", 2 ),
			CBasicTextButton::SIZE19, POSTBOX_WINDOW_MAIN_BUTTON_TAP_HISTORY, m_pEngineDevice );
/*
		CBasicTextButton* pButton;
		m_pButton_Tap[TAP_RECEIVE] = pButton = new CBasicTextButton(m_pRenderDevice);
		pButton->CreateSub( this, "BASIC_TEXT_BUTTON19", UI_FLAG_XSIZE, POSTBOX_WINDOW_MAIN_BUTTON_TAP_RECEIVE );
		pButton->CreateBaseButton( "POSTBOX_WINDOW_MAIN_BUTTON_TAP_RECEIVE", CBasicTextButton::SIZE19, 
			CBasicButton::RADIO_FLIP, (char*)ID2GAMEWORD( "POSTBOX_BUTTON", 0 ) );
		RegisterControl( pButton );

		m_pButton_Tap[TAP_SEND] = pButton = new CBasicTextButton(m_pRenderDevice);
		pButton->CreateSub( this, "BASIC_TEXT_BUTTON19", UI_FLAG_XSIZE, POSTBOX_WINDOW_MAIN_BUTTON_TAP_SEND );
		pButton->CreateBaseButton( "POSTBOX_WINDOW_MAIN_BUTTON_TAP_SEND", CBasicTextButton::SIZE19, 
			CBasicButton::RADIO_FLIP, (char*)ID2GAMEWORD( "POSTBOX_BUTTON", 1 ) );
		RegisterControl( pButton );

		m_pButton_Tap[TAP_HISTORY] = pButton = new CBasicTextButton(m_pRenderDevice);
		pButton->CreateSub( this, "BASIC_TEXT_BUTTON19", UI_FLAG_XSIZE, POSTBOX_WINDOW_MAIN_BUTTON_TAP_HISTORY );
		pButton->CreateBaseButton( "POSTBOX_WINDOW_MAIN_BUTTON_TAP_HISTORY", CBasicTextButton::SIZE19, 
			CBasicButton::RADIO_FLIP, (char*)ID2GAMEWORD( "POSTBOX_BUTTON", 2 ) );
		RegisterControl( pButton );
*/

		CTapSelector::TapSelect ( TAP_DEFAULT );
	}

	// Note : 닫기 버튼
	{
		CBasicTextButton* pButton;
		m_pButton_Close = pButton = new CBasicTextButton(m_pEngineDevice);
		pButton->CreateSub( this, "BASIC_TEXT_BUTTON19", UI_FLAG_XSIZE, POSTBOX_WINDOW_MAIN_BUTTON_CLOSE );
		pButton->CreateBaseButton( "POSTBOX_WINDOW_MAIN_BUTTON_CLOSE", CBasicTextButton::SIZE19, 
			CBasicButton::CLICK_FLIP, (char*)ID2GAMEWORD( "POSTBOX_BUTTON", 3 ) );
		RegisterControl( pButton );
	}
}

void CPostBoxWindowMain::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	switch ( ControlID )
	{
	case POSTBOX_WINDOW_MAIN_BUTTON_TAP_RECEIVE :
	case POSTBOX_WINDOW_MAIN_BUTTON_TAP_SEND    :
	case POSTBOX_WINDOW_MAIN_BUTTON_TAP_HISTORY :
		{
			CTapSelector::EMTAPSELECTORRESULT emResult = CTapSelector::EMTAPSELECTORRESULT_NONE;
			CTapSelector::TapSelectorUIMessage( ControlID, dwMsg, emResult );

			if ( emResult == CTapSelector::EMTAPSELECTORRESULT_SELECT )
			{
				AddMessageEx ( UIMSG_POSTBOX_WINDOW_MAIN_TAP_CHANGE );
			}
		}
		break;

	case POSTBOX_WINDOW_MAIN_BUTTON_CLOSE :
		{
			if ( CHECK_KEYFOCUSED( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			{
				AddMessageEx ( UIMSG_POSTBOX_WINDOW_OBJECT_CLOSE );
			}
		}
		break;
	}

	CPostBoxWindowObject::TranslateUIMessage ( ControlID, dwMsg );
}

void CPostBoxWindowMain::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{	
	CPostBoxWindowObject::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}
