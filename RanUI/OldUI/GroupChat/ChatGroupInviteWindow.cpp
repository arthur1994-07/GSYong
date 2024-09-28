#include "StdAfx.h"
#include "ChatGroupInviteWindow.h"

//#include "RnFriendTab.h"
//#include "RnBlockTab.h"

#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../enginelib/GUInterface/BasicLineBoxSmart.h"
#include "../../../EngineLib/GUInterface/BasicScrollBarEx.h"
#include "../../../EngineLib/GUInterface/BasicScrollThumbFrame.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/UIKeyCheck.h"

#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Friend/GLFriendClient.h"
#include "../../../RanLogicClient/Club/GLClubClient.h"
#include "../../../RanLogicClient/Char/CharNameDbNumCache.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace
{
	const char* ID2GAMEWORD_TITLE = "그룹채팅 초대";
	const char* ID2GAMEWORD_FRIEND = "친구목록";
	const char* ID2GAMEWORD_BLOCK = "클럽목록";
	const char* strTextt ="초대하기";
}

ChatGroupInviteWindow::ChatGroupInviteWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
: CUIWindowEx( pInterface, pEngineDevice )
, m_pGaeaClient( pGaeaClient )
, m_pInterface( pInterface )
, m_pCurrentPage( NULL )
, m_pFriendPage( NULL )
, m_pClubPage( NULL )
, m_PageSelect (FRIEND_PAGE)
, m_dwInviteDbnum ( INVALID_CHAT_KEY )
, m_bDbNumRegWaite ( TRUE )
, m_fWaiteTime ( 0 )
{
	//ID2GAMEWORD_TITLE   = ID2GAMEWORD("RNFRIEND_WINDOW", 0);
	//ID2GAMEWORD_FRIEND  = ID2GAMEWORD("RNFRIEND_WINDOW", 1);
	//ID2GAMEWORD_BLOCK   = ID2GAMEWORD("RNFRIEND_WINDOW", 2);
	ID2GAMEWORD_TITLE = ID2GAMEWORD("INVITE_GROUP_CHAT_WINDOW_TITLE",0);
	strTextt = ID2GAMEWORD("MAX_GROUP_CHAT_INVITE",0);
}

void ChatGroupInviteWindow::CreateSubControl()
{
	CD3DFontPar* pFont = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_SHADOW_FLAG );
	CreateBaseWindowLightGray( "INVITE_GROUP_CHAT_WINDOW",  ID2GAMEWORD("INVITE_GROUP_CHAT_WINDOW_TITLE") );

	CBasicLineBoxSmart* pLineBox = new CBasicLineBoxSmart(GetEngineDeviceMan());
	pLineBox->CreateSub( GetUIGroupThis(), "INVITE_GROUP_CHAT_WINDOW_WHITE" );
	pLineBox->CreateSubControl( "ANOTHER_CHAR_REGION_WINDOW_TEXTUREINFO" );
	RegisterControl( pLineBox );

	{
		m_pNameBoxBack = new CUIControl(m_pEngineDevice);
		m_pNameBoxBack->CreateSub ( this, "INVITE_GROUP_CHAT_NAME_LIST_BG",UI_FLAG_DEFAULT );
		m_pNameBoxBack->SetUseRender ( TRUE );
		m_pNameBoxBack->SetDiffuse ( D3DCOLOR_ARGB(180,0,0,0) );
		RegisterControl ( m_pNameBoxBack );
	}

	{
		CBasicTextBoxExLink* pTextBox = new CBasicTextBoxExLink(m_pEngineDevice);
		pTextBox->CreateSub ( this, "INVITE_GROUP_CHAT_NAME_LIST", UI_FLAG_DEFAULT, ID_NAME_LIST_BOX );
		pTextBox->SetFont ( pFont );		
		pTextBox->SetLineInterval ( 5.0f );
		pTextBox->SetSensitive ( true );

		pTextBox->SetMouseOverColor(NS_UITEXTCOLOR::AQUA);
		pTextBox->CreateMouseClick(NS_UITEXTCOLOR::AQUA);


		RegisterControl ( pTextBox );
		m_pNameListBox = pTextBox;

		int nTotalLine = pTextBox->GetVisibleLine ();

		//	스크롤바
		CBasicScrollBarEx* pScrollBar = new CBasicScrollBarEx(m_pEngineDevice);
		pScrollBar->CreateSub ( this, "BASIC_SCROLLBAR",  UI_FLAG_RIGHT | UI_FLAG_YSIZE, ID_NAME_LIST_SCROLLBAR);
		pScrollBar->CreateBaseScrollBar ( "INVITE_GROUP_CHAT_NAME_SCROLLBAR" );
		pScrollBar->GetThumbFrame()->SetState ( 1, nTotalLine );
		RegisterControl ( pScrollBar );
		m_pNameListScrollBar = pScrollBar;
		m_pNameListScrollBar->GetThumbFrame ()->SetScrollParent(m_pNameListBox);

		RnButton::CreateArg arg;
		//////////////////////////////////////////////////////////////////////////
		arg.defaultTextureKey = "RNBUTTON_DEFAULT_TEXTURE";
		arg.mouseOverTextureKey = "RNBUTTON_OVER_TEXTURE";
		arg.mouseClickTextureKey = "RNBUTTON_CLICK_TEXTURE";
		//////////////////////////////////////////////////////////////////////////
		arg.dwColor = NS_UITEXTCOLOR::WHITE;

		// 초대하기 버튼
		CString strText = strTextt;
		//strText.Format( "%s%s", "   ", ID2GAMEWORD_RESET );
		arg.text = strText.GetString();
		RnButton* pReset = new RnButton(m_pEngineDevice);
		pReset->CreateSub(this, "INVITE__GROUP_CHAT_INVITE_BUTTON_POS", UI_FLAG_DEFAULT, ID__INVITE_BUTTON);
		pReset->CreateSubControl(arg);
		RegisterControl(pReset);
		m_pInviteButton = pReset;
	}

	{   // 친구 페이지
		CUIGroup* pFriendPage = new CUIGroup( m_pEngineDevice );
		//CUIGroup* pFriendPage = new CUIGroup( m_pGaeaClient, m_pInterface, m_pEngineDevice );
		pFriendPage->CreateSub(this,"RNFRIEND_FRIEND_TAB",UI_FLAG_DEFAULT, ID_FRIEND_PAGE);
		//pFriendPage->CreateSubControl();
		pFriendPage->SetUseRender(TRUE);
		pFriendPage->SetDiffuse ( D3DCOLOR_ARGB(70,255,0,0) );
		pFriendPage->SetVisibleSingle( FALSE );
		RegisterControl( pFriendPage );
		m_pFriendPage = pFriendPage;
	} 

	{   // 클럽 페이지
		CUIGroup* pClubPage = new CUIGroup(m_pEngineDevice );
		//CUIGroup* pFriendPage = new CUIGroup( m_pGaeaClient, m_pInterface, m_pEngineDevice );
		pClubPage->CreateSub(this,"RNFRIEND_BLOCK_TAB",UI_FLAG_DEFAULT, ID_CLUB_PAGE);
		//pBlockPage->CreateSubControl();
		pClubPage->SetUseRender(TRUE);
		pClubPage->SetDiffuse ( D3DCOLOR_ARGB(70,0,0,255) );
		pClubPage->SetVisibleSingle( FALSE );
		RegisterControl( pClubPage );
		m_pClubPage = pClubPage;
	}

	{   // 친구 탭 버튼
		STAPBUTTON* pFriend = new STAPBUTTON(m_pEngineDevice);
		pFriend->CreateSub( this, "RNFRIEND_FRIEND_BUTTON", UI_FLAG_DEFAULT, ID_FRIEND_TAB );
		pFriend->CreateSubControl( ID2GAMEWORD("INVITE_GROUP_CHAT_WINDOW_FRIEND"),
			"CHARACTER_WINDOW_TAP_BASE","CHARACTER_WINDOW_TAP_ACTIVE",
			"CHARACTER_WINDOW_TAP_BASE","CHARACTER_WINDOW_TAP_INACTIVE",
			"CHARACTER_WINDOW_TAP_BASE");
		RegisterControl ( pFriend );
		RegisterTapButton( pFriend );
	}

	{   // 클럽 탭 버튼
		STAPBUTTON* Club = new STAPBUTTON(m_pEngineDevice);
		Club->CreateSub ( this, "RNFRIEND_BLOCK_BUTTON", UI_FLAG_DEFAULT, ID_CLUB_TAB );
		Club->CreateSubControl( ID2GAMEWORD("INVITE_GROUP_CHAT_WINDOW_CLUB") ,
			"CHARACTER_WINDOW_TAP_BASE","CHARACTER_WINDOW_TAP_ACTIVE",
			"CHARACTER_WINDOW_TAP_BASE","CHARACTER_WINDOW_TAP_INACTIVE",
			"CHARACTER_WINDOW_TAP_BASE");
		RegisterControl ( Club );
		RegisterTapButton( Club );
	}

	{
		m_pEditBoxBack = new CUIControl(m_pEngineDevice);
		m_pEditBoxBack->CreateSub ( this, "INVITE__GROUP_CHAT_EDIT_BOX_BG",UI_FLAG_XSIZE | UI_FLAG_BOTTOM );
		m_pEditBoxBack->SetUseRender ( TRUE );
		m_pEditBoxBack->SetDiffuse ( D3DCOLOR_ARGB(180,0,0,0) );
		RegisterControl ( m_pEditBoxBack );
	}
	{
		m_pEditBox = new CUIEditBoxLink(m_pEngineDevice);
		m_pEditBox->CreateSub ( this, "INVITE__GROUP_CHAT_EDIT_BOX",UI_FLAG_XSIZE | UI_FLAG_BOTTOM, ID_EDIT_BOX );
		m_pEditBox->CreateCarrat ( "MAX_GROUP_CHAT_TALK_INPUT_BOX_CARRAT", TRUE, UINT_MAX );
		m_pEditBox->SetLimitInput ( CHAT_MSG_SIZE - 10 );
		m_pEditBox->SetFont ( pFont );
		RegisterControl ( m_pEditBox );
	}

	EventSelectedTap( ID_FRIEND_TAB );
}

void ChatGroupInviteWindow::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	EMTAPSELECTORRESULT emResult = EMTAPSELECTORRESULT_NONE;
	TapSelectorUIMessage( ControlID, dwMsg, emResult );
	CUIWindowEx::TranslateUIMessage( ControlID, dwMsg );

	switch ( ControlID )
	{
	case ID_NAME_LIST_BOX:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( UIMSG_LB_DOWN & dwMsg )
				{
					m_strSelectName = m_pNameListBox->GetSelectWordText();
					m_pEditBox->ClearEdit();
					m_pEditBox->SetEditString(m_strSelectName);
					//m_pGaeaClient->ReqChaDbNum((std::string)m_strSelectName);
				}
			}
		}
		break;

	case ID__INVITE_BUTTON:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( UIMSG_LB_UP & dwMsg )
				{
					m_strSelectName = m_pEditBox->GetEditString();
					if (m_strSelectName.GetLength())
					{
						std::string strTemp = sc::string::format( ID2GAMEWORD("INVITE_GROUP_CHAT_WINDOW_INVITE_MSG") , (std::string)m_strSelectName ).c_str();
						m_dwInviteDbnum = m_pGaeaClient->FindChaDbNumAtCache(std::string(m_strSelectName.GetString()));
						if (m_dwInviteDbnum != 0)
						{
							m_pInterface->DoChatGroupModal( strTemp, UI::MODAL_CHAT_GROUP_REQ, EMPARTY_MODE_PARTY, m_dwInviteDbnum );
							m_dwInviteDbnum = 0;
						}
						else
						{
							if ( m_bDbNumRegWaite == FALSE )
								return;

							m_bDbNumRegWaite = FALSE;
							m_pGaeaClient->ReqChaDbNum((std::string)m_strSelectName);
							//m_mbDbNumRegKey = m_pGaeaClient->ReqChaDbNum((std::string)m_strSelectName);
						}
					}
				}
			}
		}
		break;

	case ID_EDIT_BOX:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( UIMSG_LB_DOWN & dwMsg )
				{
					EDIT_BEGIN ();
				}
			}
			else
			{
				if ( m_pEditBox->IsBegin () )
				{
					if ((UIMSG_LB_DOWN & dwMsg) || 
						(UIMSG_MB_DOWN & dwMsg) || 
						(UIMSG_RB_DOWN & dwMsg) || 
						UIKeyCheck::GetInstance()->Check( DIK_ESCAPE, DXKEY_DOWN ) )
					{
						if ( !(DxInputDevice::GetInstance().GetKeyState ( DIK_LCONTROL ) & DXKEY_PRESSED) )
							m_pEditBox->EndEdit();
					}
				}
			}
		}
		break;
	}

}

void ChatGroupInviteWindow::EDIT_BEGIN ( const CString& strMessage )
{
	if ( strMessage.GetLength () ) m_pEditBox->SetEditString ( strMessage );

	m_pEditBox->BeginEdit ();
}

void ChatGroupInviteWindow::SetVisibleSingle( BOOL bVisible )
{
	if( m_pCurrentPage != NULL )
		m_pCurrentPage->SetVisibleSingle( bVisible );

	CUIWindowEx::SetVisibleSingle( bVisible );
}

void ChatGroupInviteWindow::SetScroolBarUpDate(CBasicTextBoxEx* pBasicTextBoxEx)
{
	CBasicScrollThumbFrame* const pThumbFrame = m_pNameListScrollBar->GetThumbFrame ();
	if ( !pThumbFrame )
	{
		GASSERT ( 0 && "썸이 만들어지지 않았습니다.");
		return ;
	}

	int nTotal, nView;
	pThumbFrame->GetState ( nTotal, nView );	

	const int nTotalLine = pBasicTextBoxEx->GetTotalLine();
	pThumbFrame->SetState ( nTotalLine, nView );
	pThumbFrame->SetPercent ( CBasicScrollThumbFrame::fHUNDRED_PERCENT );
}

void ChatGroupInviteWindow::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIWindowEx::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	if (!m_pNameListBox->GetTotalLine() && (m_PageSelect == FRIEND_PAGE))
	{
		LoadFriend();
	}

	if ( m_bDbNumRegWaite == FALSE )
	{
		m_fWaiteTime += fElapsedTime;
			m_dwInviteDbnum = m_pGaeaClient->FindChaDbNumAtCache(std::string(m_strSelectName.GetString()));
			if ( m_dwInviteDbnum != 0 )
			{
				std::string strTemp = sc::string::format( ID2GAMEWORD("INVITE_GROUP_CHAT_WINDOW_INVITE_MSG") , (std::string)m_strSelectName ).c_str();
				m_pInterface->DoChatGroupModal( strTemp, UI::MODAL_CHAT_GROUP_REQ, EMPARTY_MODE_PARTY, m_dwInviteDbnum );
				m_bDbNumRegWaite = TRUE;
				m_dwInviteDbnum = 0;
			}

			if ( m_fWaiteTime > 5.0f )
			{
				m_fWaiteTime = 0.0f;
				m_bDbNumRegWaite = TRUE;
				m_dwInviteDbnum = 0;
			}
	}

}

CUIGroup* ChatGroupInviteWindow::GetTapWindow( UIGUID controlID )
{
	switch ( controlID )
	{
	case ID_FRIEND_TAB:
		return m_pFriendPage;
	case ID_CLUB_TAB:
		return m_pClubPage;
	}

	return NULL;
}

void ChatGroupInviteWindow::EventSelectedTap( UIGUID controlID )
{
	if( controlID != ID_FRIEND_TAB && controlID != ID_CLUB_TAB )
		return;

	
	switch ( controlID )
	{
	case ID_FRIEND_TAB:
		{
			m_pNameListBox->ClearText();
			m_PageSelect = FRIEND_PAGE;
			LoadFriend();
		}
		break;
	case ID_CLUB_TAB:
		{
			m_pNameListBox->ClearText();
			m_PageSelect = CLUB_PAGE;
			LoadClubMember();
		}
		break;
	}
}

void ChatGroupInviteWindow::Refresh()
{
	if( m_pCurrentPage == NULL )
		return;

	m_pCurrentPage->Refresh();
}

void ChatGroupInviteWindow::LoadFriend()
{
	// 온 오프라인 분리
	std::vector< const std::tr1::shared_ptr<SFRIEND> > vecTempOnlineFriend;
	std::vector< const std::tr1::shared_ptr<SFRIEND> > vecTempOfflineFriend;

	const friendlogic::FRIENDMAP& FriendMap = m_pGaeaClient->GetFriendClient()->GetList();

	BOOST_FOREACH( const friendlogic::FRIENDMAP_VALUE itFriend, FriendMap )
	{
		const std::string& strGroupName = itFriend.first;      
		const std::tr1::shared_ptr<SFRIEND> spFriend = itFriend.second;

		if( spFriend->IsBLOCK() )
			continue;

		if( spFriend->m_Online && spFriend->IsVALID() )
			m_pNameListBox->AddText(strGroupName.c_str(), NS_UITEXTCOLOR::WHITE);
	}

}

void ChatGroupInviteWindow::LoadClubMember()
{
	GLClubClient* pMyClub = m_pGaeaClient->GetMyClub();
	if (!pMyClub->IsValid())
		return;

	std::vector<club::ALIGN_ORDER> MemberAlign;

	club::ALIGN_ORDER Align( m_OrderType, m_OrderASC_DSC );
	MemberAlign.push_back(Align);

	m_vecMembers.clear();
	m_pGaeaClient->GetClubMember(m_vecMembers, MemberAlign, true);
	m_pGaeaClient->GetClubMember(m_vecMembers, MemberAlign, false);

	std::string strMyname(m_pGaeaClient->GetCharacter()->GetName());

	for (size_t i=0; i<m_vecMembers.size(); ++i)
	{
		club::GLCLUBMEMBER_CLIENT& sClubMember = m_vecMembers[i];
		std::string strMemberyname(sClubMember.GetName());

		if ( sClubMember.m_Online && strcmp(strMemberyname.c_str(), strMyname.c_str()) )
			m_pNameListBox->AddText(sClubMember.GetName(), NS_UITEXTCOLOR::WHITE);
	}
}


void MyChatGroupInviteWindow::CreateUIWindowAndRegisterOwnership()
{
	ChatGroupInviteWindow::Create( CHAT_GROUP_INVITE_WINDOW, "BASIC_WINDOW", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	ChatGroupInviteWindow::CreateSubControl();
	ChatGroupInviteWindow::SetAlignFlag( UI_FLAG_CENTER_Y | UI_FLAG_RIGHT );
	ChatGroupInviteWindow::m_pInterface->UiRegisterControl( this, true );
	ChatGroupInviteWindow::m_pInterface->UiShowGroupFocus( CHAT_GROUP_INVITE_WINDOW );
}

void MyChatGroupInviteWindow::AddFriendName_To_EditBox( const UI::String& strName )
{
}

void MyChatGroupInviteWindow::AddFriend( const UI::String& strName )
{
	if( strName.empty() == FALSE )
        m_pInterface->ADD_FRIEND( strName.c_str() );
		//m_pGaeaClient->GetFriendClient()->ReqFriendAdd( strName.c_str() );
}

void MyChatGroupInviteWindow::LoadFriendList()
{
	m_pCurrentPage->Refresh();
}

UI::String MyChatGroupInviteWindow::GetFriendName() const
{
	return "";
}

UI::String MyChatGroupInviteWindow::GetSelectBlockFriendName() const
{
	return "";
}

void MyChatGroupInviteWindow::EndEditFriendList()
{
	int i = 0;
}

std::tr1::shared_ptr<SFRIEND> MyChatGroupInviteWindow::GetFriendSMSInfo()
{
	return std::tr1::shared_ptr<SFRIEND>();
}

