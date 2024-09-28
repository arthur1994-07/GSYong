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
#include "./ChatPopMenu.h"

#include "../../InnerInterface.h"
#include "ChatPopUpWindow.h"
#include "../util/RnButton.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


const int nLIMITCHAR = CHAT_MSG_SIZE - 10;
const int MENU_GAP = 30;

CChatPopUpWindow::CChatPopUpWindow ( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
: CUIWindowEx ( pInterface, pEngineDevice )
, m_pGaeaClient ( pGaeaClient )
, m_strName ( "" )
, m_strCharName( "" )
, m_pPopBGMemberLine ( NULL )
, m_pPopBGMasterLine ( NULL )
, m_pPopBGMember ( NULL )
, m_pPopBGMaster ( NULL )
, m_dwKey( INVALID_CHAT_KEY )
{
 	m_pPopMenu.clear();
}

CChatPopUpWindow::~CChatPopUpWindow ()
{
}

CUIControl* CChatPopUpWindow::CreatePopMenuBG(CString strButtonPos, 
												EM_MAX_GROUP_CHAT enMENU)
{
	CD3DFontPar* pFont = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	{
		CUIControl* pPopMenuBG = new CUIControl(m_pEngineDevice);
		pPopMenuBG->CreateSub ( this, strButtonPos,UI_FLAG_XSIZE | UI_FLAG_BOTTOM );
		pPopMenuBG->SetUseRender ( TRUE );
		pPopMenuBG->SetDiffuse ( D3DCOLOR_ARGB(200,0,0,0) );
		RegisterControl ( pPopMenuBG );
		return pPopMenuBG;
	}
}

void CChatPopUpWindow::CreatePopMenu ( CString strRoomName,const char* strButtonPos, UIGUID nID)
{
	ChatPopUpWindowMenu* pPopMenu = new ChatPopUpWindowMenu( m_pGaeaClient, m_pInterface, m_pEngineDevice );
	pPopMenu->CreateSub( this,strButtonPos,UI_FLAG_DEFAULT, nID);
	pPopMenu->CreatePopMenuSub( strRoomName,"POP_UP_MENU_BASIC");
	pPopMenu->SetVisibleSingle(FALSE);
	RegisterControl ( pPopMenu );
	m_pPopMenu.push_back(pPopMenu);
}

void CChatPopUpWindow::CreateSubControl ()
{
    CD3DFontPar* pFont = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	{
		CBasicLineBoxSmart* pLinBoxSmart = new CBasicLineBoxSmart(m_pEngineDevice);
		pLinBoxSmart->CreateSub( this, "POP_UP_MENU_BG_MEMBER", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pLinBoxSmart->CreateSubControl( "TEXTURE_EMPTYBG_BLACKLINE" );
		RegisterControl( pLinBoxSmart );
		m_pPopBGMemberLine = pLinBoxSmart;
		m_pPopBGMemberLine->SetVisibleSingle( FALSE );
	}
	{
		CBasicLineBoxSmart* pLinBoxSmart = new CBasicLineBoxSmart(m_pEngineDevice);
		pLinBoxSmart->CreateSub( this, "POP_UP_MENU_BG_MASTER", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pLinBoxSmart->CreateSubControl( "TEXTURE_EMPTYBG_BLACKLINE" );
		RegisterControl( pLinBoxSmart );
		m_pPopBGMasterLine = pLinBoxSmart;
		m_pPopBGMasterLine->SetVisibleSingle( FALSE );
	}

	m_pPopBGMember = CreatePopMenuBG( "POP_UP_MENU_BG_MEMBER", POP_UP_MENU_BG_MEMBER );
	m_pPopBGMaster = CreatePopMenuBG( "POP_UP_MENU_BG_MASTER", POP_UP_MENU_BG_MASTER );

    CreatePopMenu( ID2GAMEWORD("CHAT_GROUP_POP_UP_MENU_INFO"), 
		"POP_UP_MENU_INFO", POP_UP_MENU_INFO );
    CreatePopMenu( ID2GAMEWORD("CHAT_GROUP_POP_UP_MENU_FRIEND_REGISTER"), 
		"POP_UP_MENU_FRIEND_REGISTER" ,POP_UP_MENU_FRIEND_REGISTER);
    CreatePopMenu( ID2GAMEWORD("CHAT_GROUP_POP_UP_MENU_BLOCK_REGISTER"), 
		"POP_UP_MENU_BLOCK_REGISTER", POP_UP_MENU_BLOCK_REGISTER );
    CreatePopMenu( ID2GAMEWORD("CHAT_GROUP_POP_UP_MENU_BLOCK_RELEASE"), 
		"POP_UP_MENU_BLOCK_RELEASE", POP_UP_MENU_BLOCK_RELEASE );
    CreatePopMenu( ID2GAMEWORD("CHAT_GROUP_POP_UP_MENU_INVITE"), 
		"POP_UP_MENU_INVITE", POP_UP_MENU_INVITE );
    CreatePopMenu( ID2GAMEWORD("CHAT_GROUP_POP_UP_MENU_GET_OUT_ROOM"), 
		"POP_UP_MENU_GET_OUT_ROOM", POP_UP_MENU_GET_OUT_ROOM );

}

void	CChatPopUpWindow::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    CUIWindowEx::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}



void CChatPopUpWindow::TranslateUIMessage ( UIGUID cID, DWORD dwMsg )
{
    CUIWindowEx::TranslateUIMessage ( cID, dwMsg );


    switch ( cID )
    {
	case POP_UP_MENU_INFO:
		{
			if( CHECK_MOUSE_IN( dwMsg ) )
			{
				if( dwMsg & UIMSG_LB_DOWN )
				{
					m_pInterface->ReferCharWindowOpen( (std::string)m_strCharName );
					break;
				}
			}
		}
		break;
    case POP_UP_MENU_INVITE:
		{
			if( CHECK_MOUSE_IN( dwMsg ) )
			{
				if ( dwMsg & UIMSG_LB_DOWN )
				{
					std::string strTemp = sc::string::format( ID2GAMEINTEXT("PARTY_QUESTION"), m_strCharName ).c_str();
					DWORD dwDbnum = m_pGaeaClient->FindChaDbNumAtCache(std::string(m_strCharName.GetString()));
					if ( dwDbnum == m_pGaeaClient->GetCharacter()->GetCharID() )
						break;
					m_pInterface->DoPartyModal( strTemp, UI::MODAL_PARTY_REQ_DBNUM, EMPARTY_MODE_PARTY, dwDbnum );
				}
				break;
			}
		}
		break;
    case POP_UP_MENU_FRIEND_REGISTER:
		{
			if( CHECK_MOUSE_IN( dwMsg ) )
			{			
				if ( dwMsg & UIMSG_LB_DOWN )
				{
					m_pInterface->ADD_FRIEND( m_strCharName );
					//m_pGaeaClient->GetFriendClient()->ReqFriendAdd( m_strCharName  );
					break;
				}
			}

		}
		break;

    case POP_UP_MENU_BLOCK_REGISTER:
		{
			if( CHECK_MOUSE_IN( dwMsg ) )
			{			
				if ( dwMsg & UIMSG_LB_DOWN )
				{
                    m_pInterface->ADD_FRIEND_BLOCK( m_strCharName, true );
					//m_pGaeaClient->GetFriendClient()->ReqFriendBlock( m_strCharName, true );
					break;
				}
			}

			break;

		}

    case POP_UP_MENU_BLOCK_RELEASE:
		{
			if( CHECK_MOUSE_IN( dwMsg ) )
			{
				if ( dwMsg & UIMSG_LB_DOWN )
				{
					m_pGaeaClient->GetFriendClient()->ReqFriendDel ( m_strCharName );
					break;
				}
			}

		}
		break;

    case POP_UP_MENU_GET_OUT_ROOM:
		{
			if( CHECK_MOUSE_IN( dwMsg ) )
			{
				if( dwMsg & UIMSG_LB_DOWN )
				{
						GLMSG::SNET_GCHAT_KICK_CA msg;
						msg.dwChaNum = m_dwKey;
						m_pGaeaClient->NETSENDTOAGENT(&msg);
					break;
				}
			}
		}
		break;
    }

	if ( CHECK_LB_UP_LIKE ( dwMsg ) || CHECK_LB_DOWN_LIKE ( dwMsg )  )
		{
			SetVisibleSingle(FALSE);
		}

}

void CChatPopUpWindow::ShowPopUpWindow(int nMousePosX, int nMousePosY)
{

    UIRECT rcOldPos = GetGlobalPos();
    WORD wAlignFlag = GetAlignFlag ();
    SetAlignFlag ( UI_FLAG_DEFAULT );
    {
        UIRECT rcNewPos(static_cast<float>(nMousePosX) + MENU_GAP, static_cast<float>(nMousePosY), 
            rcOldPos.sizeX , rcOldPos.sizeY );
        AlignSubControl( rcOldPos, rcNewPos );
        SetGlobalPos( rcNewPos );
    }
    SetAlignFlag( wAlignFlag );


	std::string srtTargetName(m_pGaeaClient->GetCharacter()->GetName());
	DWORD dwDbnumMaster = m_pGaeaClient->FindChaDbNumAtCache(std::string(m_strName.GetString()));
	DWORD dwDbnumTarget = m_pGaeaClient->FindChaDbNumAtCache(srtTargetName);
	
	// 방장메뉴에만 강퇴 표시
    if ( dwDbnumMaster == dwDbnumTarget )
    {
		m_pPopBGMaster->SetVisibleSingle(TRUE);
		m_pPopBGMember->SetVisibleSingle(FALSE);
		m_pPopMenu[0]->SetVisibleSingle(TRUE);
		m_pPopMenu[1]->SetVisibleSingle(TRUE);
		m_pPopMenu[2]->SetVisibleSingle(TRUE);
		m_pPopMenu[3]->SetVisibleSingle(TRUE);
		m_pPopMenu[4]->SetVisibleSingle(TRUE);
		m_pPopMenu[5]->SetVisibleSingle(TRUE);
		m_pPopBGMemberLine->SetVisibleSingle(FALSE);
		m_pPopBGMasterLine->SetVisibleSingle(TRUE);

    }
    else
    {
		m_pPopBGMaster->SetVisibleSingle(FALSE);
		m_pPopBGMember->SetVisibleSingle(TRUE);
		m_pPopMenu[0]->SetVisibleSingle(TRUE);
		m_pPopMenu[1]->SetVisibleSingle(TRUE);
		m_pPopMenu[2]->SetVisibleSingle(TRUE);
		m_pPopMenu[3]->SetVisibleSingle(TRUE);
		m_pPopMenu[4]->SetVisibleSingle(TRUE);
		m_pPopMenu[5]->SetVisibleSingle(FALSE);
		m_pPopBGMemberLine->SetVisibleSingle(TRUE);
		m_pPopBGMasterLine->SetVisibleSingle(FALSE);
    }

    SetVisibleSingle(TRUE);


}
void CChatPopUpWindow::HidePopUpWindow()
{

    //SetVisibleSingle(FALSE);

    //for ( int i = 0; i < 6; i ++)
    //{
    //    m_pPopMenu[i]->SetText(
    //        m_pPopMenu[i]->GetText().c_str(),NS_UITEXTCOLOR::WHITESMOKE);
    //}

}

void CChatPopUpWindow::SetRoomName( CString srtName )
{
	m_strName = srtName;
}

void CChatPopUpWindow::SetName( CString srtCharmName )
{
	m_strCharName = srtCharmName;
}

void CChatPopUpWindow::SetDwKey( DWORD dwkey )
{
	m_dwKey = dwkey;
}




