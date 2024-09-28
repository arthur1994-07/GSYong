#include "StdAfx.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicTextBoxEx.h"
#include "../../../EngineLib/GUInterface/BasicScrollBarEx.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicLineBox.h"
#include "../../../EngineLib/GUInterface/BasicLineBoxSmart.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/BasicScrollThumbFrame.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/DxTools/DebugSet.h"
#include "../../../EngineLib/GUInterface/BasicButton.h"



#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"

#include "../../InnerInterface.h"
#include "MinChatWindow.h"
#include "../util/RnButton.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CMinChatWindow::CMinChatWindow ( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
: CUIWindowEx ( pInterface, pEngineDevice )
, m_pGaeaClient ( pGaeaClient )
, m_pMinChatRoomNameBox(NULL)
, m_pMinChatRoomExitButton(NULL)
, bButtonState(TRUE)
, m_pUpTextBoxLine (NULL)
, bMouseIn(FALSE)
, m_dwKey (INVALID_CHAT_KEY)
{
}

namespace
{
    const char* ID2GAMEWORD_TITLE;
    const char* ID2GAMEWORD_RESET;
    const char* ID2GAMEWORD_CONFIRM;
}



CMinChatWindow::~CMinChatWindow ()
{
}

void CMinChatWindow::CreateMinChatRoom ( CString strRoomName )
{
    CD3DFontPar* pFont = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );


	{    
		CUIControl* pRnButton = new CUIControl(m_pEngineDevice);
		pRnButton->CreateSub(this, "GROUP_CHATTING_ROOM", UI_FLAG_DEFAULT);
		RegisterControl(pRnButton);
		m_pMinChatRoomBG_NOMAL = pRnButton;
		m_pMinChatRoomBG_NOMAL->SetVisibleSingle(TRUE);
	}
	{
		CUIControl* pRnButton = new CUIControl(m_pEngineDevice);
		pRnButton->CreateSub(this, "GROUP_CHATTING_ROOM_OPEN", UI_FLAG_DEFAULT);
		RegisterControl(pRnButton);
		m_pMinChatRoomBG_OPEN = pRnButton;
		m_pMinChatRoomBG_OPEN->SetVisibleSingle(FALSE);
	}
	{
		CUIControl* pRnButton = new CUIControl(m_pEngineDevice);
		pRnButton->CreateSub(this, "GROUP_CHATTING_ROOM_MESSAGE", UI_FLAG_DEFAULT);
		RegisterControl(pRnButton);
		m_pMinChatRoomBG_MSG = pRnButton;
		m_pMinChatRoomBG_MSG->SetVisibleSingle(FALSE);
	}

    //	텍스트 박스
    {
        CBasicTextBoxEx* pTextBox = new CBasicTextBoxEx(m_pEngineDevice);
        pTextBox->CreateSub ( this, "MIN_GROUP_CHAT_ROOM_NAME", UI_FLAG_XSIZE | UI_FLAG_YSIZE, MIN_GROUP_CHAT_TEXTBOX );
        pTextBox->SetFont ( pFont );
        pTextBox->SetText( strRoomName );
        pTextBox->SetTextAlign( TEXT_ALIGN_CENTER_Y | TEXT_ALIGN_CENTER_X );
        pTextBox->SetSensitive ( true );
        
        RegisterControl ( pTextBox );
        m_pMinChatRoomNameBox = pTextBox;
    }

    //{
    //    CBasicLineBoxSmart* pLinBoxSmart = new CBasicLineBoxSmart(m_pEngineDevice);
    //    pLinBoxSmart->CreateSub( this, "MIN_GROUP_CHAT_ROOM_OUTLINE_POS", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
    //    pLinBoxSmart->CreateSubControl( "POINT_SHOP_UP_TEXT_BOX_BACK_LINE" );
    //    RegisterControl( pLinBoxSmart );
    //    m_pUpTextBoxLine = pLinBoxSmart;
    //}


    //EXIT 버튼
    {
        CBasicButton* pButton = new CBasicButton(m_pEngineDevice);
        pButton->CreateSub ( this, "MIN_GROUP_CHAT_ROOM_BUTTON", UI_FLAG_XSIZE | UI_FLAG_YSIZE, MIN_GROUP_CHAT_EXIT_BUTTON );
        pButton->CreateFlip ( "MIN_GROUP_CHAT_ROOM_BUTTON_F", CBasicButton::MOUSEIN_FLIP );	
        RegisterControl ( pButton );
        m_pMinChatRoomExitButton = pButton;
    }

}

void CMinChatWindow::SetMinChatRoomName ( CString strRoomName )
{
    m_pMinChatRoomNameBox->SetText( strRoomName );
}

CString CMinChatWindow::GetMinChatRoomName ()
{
    return m_pMinChatRoomNameBox->GetText(0);
}

BOOL CMinChatWindow::IsMouseIn()
{
    if ( bMouseIn )
        return TRUE;
    else
        return FALSE;
}

void CMinChatWindow::SetRoomState( ROOM_STATE emState )
{
	switch (emState)
	{
	case ROOM_STATE_NOMAL:
		{
			m_pMinChatRoomBG_NOMAL->SetVisibleSingle(TRUE);
			m_pMinChatRoomBG_OPEN->SetVisibleSingle(FALSE);
			m_pMinChatRoomBG_MSG->SetVisibleSingle(FALSE);
		}
		break;

	case ROOM_STATE_OPEN:
		{
			m_pMinChatRoomBG_NOMAL->SetVisibleSingle(FALSE);
			m_pMinChatRoomBG_OPEN->SetVisibleSingle(TRUE);
			m_pMinChatRoomBG_MSG->SetVisibleSingle(FALSE);
		}
		break;

	case ROOM_STATE_MSG:
		{
			m_pMinChatRoomBG_NOMAL->SetVisibleSingle(FALSE);
			m_pMinChatRoomBG_OPEN->SetVisibleSingle(FALSE);
			m_pMinChatRoomBG_MSG->SetVisibleSingle(TRUE);
		}
		break;
	}
}

void CMinChatWindow::TranslateUIMessage ( UIGUID cID, DWORD dwMsg )
{
    CUIWindowEx::TranslateUIMessage ( cID, dwMsg );

    switch ( cID )
    {
    //    AddMessageEx(UIMSG_USER1);
    //case MIN_GROUP_CHAT_TEXTBOX:
    //    {
    //        if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
    //        {
				//SetRoomState(ROOM_STATE_OPEN);
    //        }
    //    }
    //    break;

    case MIN_GROUP_CHAT_EXIT_BUTTON:
        {
            if ( CHECK_MOUSE_IN ( dwMsg ) )
            {
                bMouseIn = FALSE;
                if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
                {
                    //채팅 종료 메시지
					GLMSG::SNET_GCHAT_QUIT_CA msg;
					msg.dwKey = m_dwKey;
					m_pGaeaClient->NETSENDTOAGENT(&msg);
                    bButtonState = FALSE;
                }
            }
            else
                bMouseIn = TRUE;
        }
        break;
    }
}

void	CMinChatWindow::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    if ( !IsVisible () ) return ;

    CUIWindowEx::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

    {
        //if ( 0 <= m_nSelectMap && m_nSelectMap < m_pMapList->GetCount () )
        //{
        //    m_pMapList->SetUseTextColor ( m_nSelectMap, TRUE );
        //    m_pMapList->SetTextColor ( m_nSelectMap, NS_UITEXTCOLOR::ORANGE );
        //}

        //{
        //    CBasicScrollThumbFrame* const pThumbFrame = m_pMapListScrollBar->GetThumbFrame ();

        //    const int nTotalLine = m_pMapList->GetTotalLine ();
        //    const int nLinePerOneView = m_pMapList->GetVisibleLine ();
        //    CDebugSet::ToView ( 1, 10, "보이는 라인 %d", nLinePerOneView );
        //    pThumbFrame->SetState ( nTotalLine, nLinePerOneView );
        //    if ( nLinePerOneView < nTotalLine )
        //    {
        //        const int nMovableLine = nTotalLine - nLinePerOneView;
        //        float fPercent = pThumbFrame->GetPercent ();
        //        int nPos = (int)floor(fPercent * nMovableLine);
        //        m_pMapList->SetCurLine ( nPos );			
        //    }
        //}
    }

}

