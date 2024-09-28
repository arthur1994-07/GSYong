#include "StdAfx.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicTextBoxEx.h"
#include "../../../EngineLib/GUInterface/BasicScrollBarEx.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicLineBox.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/BasicScrollThumbFrame.h"
#include "../../../EngineLib/GUInterface/UIMan.h"
#include "../../../EngineLib/GUInterface/UIKeyCheck.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/DxTools/DebugSet.h"
#include "../../../EngineLib/GUInterface/BasicButton.h"
#include "../../../EngineLib/Common/DXInputString.h"
#include "../../../EngineLib/GUInterface/BasicLineBoxSmart.h"
#include "../../../EngineLib/GUInterface/BasicTextBoxEx.h"



#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"
#include "../../../RanLogicClient/GroupChat/GroupChatClient.h"
#include "../../../RanLogicClient/Friend/GLFriendClient.h"
#include "../../../RanLogicClient/Char/CharNameDbNumCache.h"
#include "./MaxChatWindow.h"

#include "../../InnerInterface.h"
#include "../util/RnButton.h"
#include "ChatPopMenu.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------




ChatPopUpWindowMenu::ChatPopUpWindowMenu ( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
: CUIWindowEx ( pInterface, pEngineDevice )
, m_pGaeaClient ( pGaeaClient )
, m_pInterface ( pInterface )
, m_pPopBGMemberLine ( NULL )
, m_pMenu ( NULL )
{

}

ChatPopUpWindowMenu::~ChatPopUpWindowMenu ()
{
}

void ChatPopUpWindowMenu::CreatePopMenuSub(CString strRoomName,CString strButtonPos)
{
	CD3DFontPar* pFont9 = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	
	{
		CBasicLineBoxSmart* pPopBGMemberLine = new CBasicLineBoxSmart(m_pEngineDevice);
		pPopBGMemberLine->CreateSub( this, strButtonPos, UI_FLAG_XSIZE | UI_FLAG_BOTTOM, ID_LINEBOX );
		pPopBGMemberLine->CreateSubControl( "LINEBOX_Y" );
		RegisterControl( pPopBGMemberLine );
		m_pPopBGMemberLine = pPopBGMemberLine;
		m_pPopBGMemberLine->SetVisibleSingle(FALSE);
	}

	{
		CBasicTextBoxExLink* pMenu = new CBasicTextBoxExLink(m_pEngineDevice);
		pMenu->CreateSub ( this, strButtonPos, UI_FLAG_DEFAULT, ID_MENUBOX);
		pMenu->SetFont ( pFont9 );		
		pMenu->SetLineInterval ( 5.0f );
		pMenu->SetSensitive ( true );
		pMenu->AddText(strRoomName,NS_UITEXTCOLOR::WHITE);
		pMenu->SetTextAlign(TEXT_ALIGN_CENTER_X|TEXT_ALIGN_CENTER_Y);
		RegisterControl ( pMenu );
		m_pMenu = pMenu;
		m_pMenu->SetVisibleSingle(FALSE);
	}
	
}


void	ChatPopUpWindowMenu::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIWindowEx::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}



void ChatPopUpWindowMenu::TranslateUIMessage ( UIGUID cID, DWORD dwMsg )
{
	CUIWindowEx::TranslateUIMessage ( cID, dwMsg );

	switch ( cID )
	{

	case ID_MENUBOX:
		{
			if( CHECK_MOUSE_IN( dwMsg ) )
			{
				m_pPopBGMemberLine->SetVisibleSingle(TRUE);
				m_pMenu->SetUseTextColor( 0, TRUE );
				m_pMenu->SetTextColor( 0, NS_UITEXTCOLOR::YELLOW );
			}
			else
			{
				m_pPopBGMemberLine->SetVisibleSingle(FALSE);
				m_pMenu->SetUseTextColor( 0, TRUE );
				m_pMenu->SetTextColor( 0, NS_UITEXTCOLOR::WHITE );
			}

		}
		break;
	}
}





