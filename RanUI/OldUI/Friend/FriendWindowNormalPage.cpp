#include "StdAfx.h"
#include "../../../SigmaCore/String/StringFormat.h"

#include "../../../EngineLib/GUInterface/BasicTextBoxEx.h"
#include "../../../EngineLib/GUInterface/BasicScrollBarEx.h"
#include "../../../EngineLib/GUInterface/BasicScrollThumbFrame.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/DxTools/DebugSet.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../EngineLib/GUInterface/UIEditBox.h"
#include "../../../EngineLib/GUInterface/BasicLineBox.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/UIKeyCheck.h"
#include "../../../EngineLib/Common/DXInputString.h"
#include "../../../EngineLib/DxResponseMan.h"

#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Party/GLPartyClient.h"
#include "../../../RanLogicClient/Stage/DxGlobalStage.h"
#include "../../../RanLogicClient/Tutorial/GLBaseTutorial.h"

#include "../Interface/BasicChat.h"
#include "../Interface/ChatType.h"
#include "../../InnerInterface.h"
#include "FriendWindowNormalPage.h"
#include "FriendWindow.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

const int CFriendWindowNormalPage::nLIMIT_CHAR = 16;

CFriendWindowNormalPage::CFriendWindowNormalPage(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
	: CUIGroup(pEngineDevice)
    , m_pInterface(pInterface)
	, m_pGaeaClient ( pGaeaClient )
	, m_nSelectIndex ( -1 )
	, m_bINIT( FALSE )
	, m_pListText(NULL)
	, m_pListScrollBar(NULL)
	, m_pADD(NULL)
	, m_pDEL(NULL)
	, m_pMOVE(NULL)
	, m_pPARTY(NULL)
	//, m_pNote(NULL)
	, m_pEditBox(NULL)
	, m_pBLOCK(NULL)
{
}

CFriendWindowNormalPage::~CFriendWindowNormalPage ()
{
}

HRESULT CFriendWindowNormalPage::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	GASSERT( pd3dDevice );

	HRESULT hr = S_OK;
	hr = CUIGroup::InitDeviceObjects ( pd3dDevice );
	if ( FAILED ( hr ) ) return hr;

	m_bINIT = TRUE;

	LoadFriendList();

	return S_OK;
}

void CFriendWindowNormalPage::CreateSubControl ()
{
	CD3DFontPar* pFont9 = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	//	텍스트 박스
	{
		CBasicTextBoxEx* pTextBox = new CBasicTextBoxEx(m_pEngineDevice);
		pTextBox->CreateSub ( this, "FRIEND_LIST_TEXTBOX", UI_FLAG_DEFAULT, FRIEND_LIST_TEXTBOX );
		pTextBox->SetFont ( pFont9 );		
		pTextBox->SetLineInterval ( 3.0f );
		pTextBox->SetSensitive ( true );
		pTextBox->SetLimitLine ( 10000 );
		RegisterControl ( pTextBox );
		m_pListText = pTextBox;

		int nTotalLine = pTextBox->GetVisibleLine ();

		//	스크롤바
		CBasicScrollBarEx* pScrollBar = new CBasicScrollBarEx(m_pEngineDevice);
		pScrollBar->CreateSub ( this, "BASIC_SCROLLBAR", UI_FLAG_RIGHT | UI_FLAG_YSIZE, FRIEND_LIST_SCROLLBAR );
		pScrollBar->CreateBaseScrollBar ( "FRIEND_LIST_SCROLLBAR" );
		pScrollBar->GetThumbFrame()->SetState ( 1, nTotalLine );
		RegisterControl ( pScrollBar );
		m_pListScrollBar = pScrollBar;
	}

	m_pADD = CreateTextButton14 ( "FRIEND_LIST_ADDBUTTON", FRIEND_LIST_ADDBUTTON, (char*)ID2GAMEWORD("FRIEND_BUTTON",0) );
	m_pADD->SetShortcutKey ( DIK_RETURN );
	m_pDEL = CreateTextButton14 ( "FRIEND_LIST_DELBUTTON", FRIEND_LIST_DELBUTTON, (char*)ID2GAMEWORD("FRIEND_BUTTON",1) );
	m_pMOVE = CreateTextButton14 ( "FRIEND_LIST_MOVEBUTTON", FRIEND_LIST_MOVEBUTTON, (char*)ID2GAMEWORD("FRIEND_BUTTON",2) );
//#if defined(KRT_PARAM)
//	m_pBLOCK = CreateTextButton14 ( "FRIEND_LIST_BLOCKBUTTON", FRIEND_LIST_BLOCKBUTTON, (char*)ID2GAMEWORD("FRIEND_BUTTON",15) );
//#else
	m_pBLOCK = CreateTextButton14 ( "FRIEND_LIST_BLOCKBUTTON", FRIEND_LIST_BLOCKBUTTON, (char*)ID2GAMEWORD("FRIEND_BUTTON",4) );
//#endif
	//m_pNote = CreateTextButton14 ( "FRIEND_LIST_NOTEBUTTON", FRIEND_LIST_NOTEBUTTON, (char*)ID2GAMEWORD("FRIEND_TAB_BUTTON",4) );
	//m_pNote->SetVisibleSingle( FALSE );

	m_pMOVE = CreateTextButton14 ( "FRIEND_LIST_PARTYBUTTON", FRIEND_LIST_PARTYBUTTON, (char*)ID2GAMEWORD("FRIEND_BUTTON",16) );
	

	CBasicLineBox* pBasicLineBox = new CBasicLineBox(m_pEngineDevice);
	pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_EDIT", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pBasicLineBox->CreateBaseBoxEditBox ( "FRIEND_EDIT_BOX_BACK" );
	RegisterControl ( pBasicLineBox );

	CUIEditBox* pEditBox = new CUIEditBox(m_pEngineDevice);
	pEditBox->CreateSub ( this, "FRIEND_EDIT_BOX", UI_FLAG_XSIZE | UI_FLAG_BOTTOM, FRIEND_EDITBOX );
	pEditBox->CreateCarrat ( "FRIEND_CARRAT", TRUE, UINT_MAX );
	pEditBox->SetLimitInput ( nLIMIT_CHAR );
	pEditBox->SetFont ( pFont9 );		
	RegisterControl ( pEditBox );
	m_pEditBox = pEditBox;
}

CBasicTextButton* CFriendWindowNormalPage::CreateTextButton14 ( char* szButton, UIGUID ControlID , char* szText )
{
	const int nBUTTONSIZE = CBasicTextButton::SIZE14;
	CBasicTextButton* pButton = new CBasicTextButton(m_pEngineDevice);
	pButton->CreateSub ( this, "BASIC_TEXT_BUTTON14", UI_FLAG_XSIZE, ControlID );
	pButton->CreateBaseButton ( szButton, nBUTTONSIZE, CBasicButton::CLICK_FLIP, szText );
	RegisterControl ( pButton );
	return pButton;
}

void CFriendWindowNormalPage::LoadFriendList ()
{
	if( !m_bINIT ) return; // InitDeviceObject() 호출 후에 실행 되어야 한다.

	//	친구 목록을 갱신한다.
	m_pListText->ClearText ();

	const friendlogic::FRIENDMAP& FriendMap = m_pGaeaClient->GetFriendClient()->GetList();
	for (friendlogic::FRIENDMAP_CITER iter=FriendMap.begin(); iter != FriendMap.end(); ++iter)
	{
		std::tr1::shared_ptr<SFRIEND> sFriend = iter->second;
		if (sFriend->IsBLOCK())
            continue;
		if (sFriend->m_Online)
            LoadFriend(sFriend);
	}

	for (friendlogic::FRIENDMAP_CITER iter=FriendMap.begin(); iter != FriendMap.end(); ++iter)
	{
		std::tr1::shared_ptr<SFRIEND> sFriend = iter->second;
		if (sFriend->IsBLOCK())
            continue;
		if (!sFriend->m_Online)
            LoadFriend(sFriend);
	}

	m_pListText->SetCurLine ( 0 );

	const int nTotal = m_pListText->GetCount ();
	const int nViewPerPage = m_pListText->GetVisibleLine ();	
	m_pListScrollBar->GetThumbFrame()->SetState ( nTotal, nViewPerPage );
	m_pListScrollBar->GetThumbFrame()->SetPercent ( 0.0f );
}

void CFriendWindowNormalPage::LoadFriend(std::tr1::shared_ptr<SFRIEND> sFriend)
{
    if (!sFriend)
        return;

	const CString& strName = sFriend->m_ChaName.c_str();
	bool bOnline = sFriend->m_Online;

	static CString strKeyword[] =
	{
		"FRIEND_ONLINE",
		"FRIEND_OFFLINE"
	};

	D3DCOLOR dwTextColor;
	int nIcon;

	if ( bOnline ) //&& sFriend->m_Flag != EMFRIEND_BETRAY)
	{
		dwTextColor = NS_UITEXTCOLOR::WHITE;
		nIcon = ONLINE;
	}
	else
	{
		dwTextColor = NS_UITEXTCOLOR::DARKGRAY;
		nIcon = OFFLINE;
	}

// 	if( sFriend->m_Flag == EMFRIEND_BETRAY ) // Note : 상대방이 나를 친구에서 삭제한 경우
// 	{
// 		dwTextColor = NS_UITEXTCOLOR::FRIEND_OFF;
// 		nIcon = OFFLINE;
// 		bOnline = FALSE;
// 	}

	CString strTemp;
	
	if( m_pInterface->IsCHANNEL () && bOnline )
	{
//#if defined(KRT_PARAM)
//		CString strBlank( ' ', 16 - strName.GetLength() );
//		strTemp.Format( "[%d]%s%s%s", sFriend.m_Channel, strName, strBlank, sFriend.szPhoneNumber );
//#else
		strTemp.Format( "[%d]%s", sFriend->m_Channel, strName );
//#endif
	}
	else
	{
//#if defined(KRT_PARAM)
//		CString strBlank( ' ', 19 - strName.GetLength() );
//		strTemp.Format( "%s%s%s", strName, strBlank, sFriend.szPhoneNumber );
//#else
		strTemp.Format( "%s", strName );
//#endif
	}

	int nIndex = m_pListText->AddText ( strTemp, dwTextColor );	
	m_pListText->SetTextImage ( nIndex, strKeyword[nIcon] );	
	m_pListText->SetTextData ( nIndex, sFriend->m_ChaDbNum );
}

BOOL CFriendWindowNormalPage::GetFriend ( const int nCharID, CString & strName )
{
	const friendlogic::FRIENDMAP& FriendMap = m_pGaeaClient->GetFriendClient()->GetList();	
	for (friendlogic::FRIENDMAP_CITER iter=FriendMap.begin(); iter != FriendMap.end(); ++iter)
	{
		std::tr1::shared_ptr<SFRIEND> sFriend = iter->second;
		if (sFriend->m_ChaDbNum == nCharID)
		{
			strName = sFriend->m_ChaName.c_str();
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CFriendWindowNormalPage::SetFriendInfo( const int nCharID )
{
	const friendlogic::FRIENDMAP& FriendMap = m_pGaeaClient->GetFriendClient()->GetList();
	for (friendlogic::FRIENDMAP_CITER iter=FriendMap.begin(); iter != FriendMap.end(); ++iter)
	{
        std::tr1::shared_ptr<SFRIEND> spFriend = iter->second;
		if (spFriend->m_ChaDbNum == nCharID)
		{			
			m_sFriend.reset();
            m_sFriend = spFriend;
			return TRUE;
		}
	}
	return FALSE;
}

void CFriendWindowNormalPage::TranslateUIMessage ( UIGUID cID, DWORD dwMsg )
{
	CUIGroup::TranslateUIMessage ( cID, dwMsg );

	switch ( cID )
	{
	case FRIEND_LIST_TEXTBOX:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				const int nIndex = m_pListText->GetSelectPos ();
				if ( nIndex < 0 || m_pListText->GetCount () <= nIndex ) return ;

				m_pListText->SetUseOverColor ( TRUE );
				m_pListText->SetOverColor ( nIndex, NS_UITEXTCOLOR::DARKORANGE );

				if( UIMSG_LB_UP & dwMsg )
				{
					m_nSelectIndex = nIndex;

					int nCharID = m_pListText->GetTextData ( m_nSelectIndex );
					if( !GetFriend( nCharID, m_strSelectName ) ) m_strSelectName.Empty();
				}
				
				if ( UIMSG_LB_DUP & dwMsg )
				{
					std::tr1::shared_ptr<SFRIEND> pFriend = m_pGaeaClient->GetFriendClient()->GetData ( m_strSelectName.GetString() );
					if ( pFriend && pFriend->m_Online )
					{
						//m_pInterface->PostUIMessage ( FRIEND_WINDOW, UIMSG_ADDNAME_TO_CHATBOX );
						GetParent()->AddMessageEx( UIMSG_ADDNAME_TO_CHATBOX );
					}
				}
			}
		}
		break;

	case FRIEND_EDITBOX:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( UIMSG_LB_UP & dwMsg )
				{
					CString strName = m_pEditBox->GetEditString ();
					EDIT_BEGIN ( strName );				
				}
			}
			else
			{
				if ( UIMSG_LB_DOWN & dwMsg || UIMSG_LB_UP & dwMsg )
					EDIT_END ();
			}
		}
		break;

	case FRIEND_LIST_ADDBUTTON:
		{
			bool bKEY_FOCUSED = false;
			if ( CHECK_KEYFOCUSED ( dwMsg ) )
			{
				if ( !m_pEditBox->IsBegin () )
				{
					//	RETURN 무시
					UIKeyCheck::GetInstance()->RemoveCheck ( DIK_RETURN, DXKEY_DOWN );

					dwMsg &= ~UIMSG_KEY_FOCUSED;
					SetMessageEx ( dwMsg );
					return ;
				}

				bKEY_FOCUSED = true;
			}

			if (bKEY_FOCUSED || CHECK_MOUSEIN_LBUPLIKE(dwMsg))
			{
                DXInputString* pInputString = m_pEngineDevice->GetInputString();
				if (CHECK_KEYFOCUSED(dwMsg) && !pInputString->CheckEnterKeyDown())
                    return;

				if (!m_pEditBox->GetEditLength())
                    return;

				CString strName = m_pEditBox->GetEditString();
				ADD_FRIEND(strName);
				
				SetMessageEx(dwMsg &= ~UIMSG_KEY_FOCUSED); // Focus 문제로 메세지 삭제

				//m_pInterface->PostUIMessage ( GetWndID (), UIMSG_EDIT_END );
				GetParent()->AddMessageEx(UIMSG_EDIT_END);
			}
		}
		break;

	case FRIEND_LIST_DELBUTTON:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				if ( m_nSelectIndex < 0 )
				{
					m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::RED, ID2GAMEINTEXT("FRIEND_DEL_INCORRECT") );
					return ;
				}

				int nCharID = m_pListText->GetTextData ( m_nSelectIndex );

				CString strName;
				if( GetFriend( nCharID, strName ) ) DEL_FRIEND ( strName );
			}
		}
		break;

	case FRIEND_LIST_MOVEBUTTON:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				if ( m_pGaeaClient->GetTutorial()->IsTutorial() ) // 튜토리얼 중에는 친구에게 이동 금지.
				{
					m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("TUTORIAL_CANNOT_USE") );
				}
				else
				{
					if ( m_nSelectIndex < 0 ) return ;

					int nCharID = m_pListText->GetTextData ( m_nSelectIndex );

					CString strName;
					if( GetFriend( nCharID, strName ) ) MOVETO_FRIEND ( strName ); //	친구에게 이동
				}
			}
		}
		break;

	case FRIEND_LIST_BLOCKBUTTON:
		{
			if( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				if( m_nSelectIndex < 0 ) return ;

				int nCharID = m_pListText->GetTextData( m_nSelectIndex );

				CString strName;
				if( GetFriend( nCharID, strName ) )
				{
//#if defined(KRT_PARAM)
//					if( SetFriendInfo( nCharID ) )
//					{
//						GetParent()->AddMessageEx( UIMSG_WRITE_NOTE_VISIBLE );
//      			}
//#else
					ADD_BLOCK( strName );
//#endif
				}
			}
		}
		break;
	case FRIEND_LIST_PARTYBUTTON:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				if ( m_nSelectIndex < 0 ) return ;

				int nCharID = m_pListText->GetTextData ( m_nSelectIndex );

				CString strName;
				if( GetFriend( nCharID, strName ) ) PARTY_FRIEND ( strName ); //	친구에게 이동
			}
		}
		break;

	//case FRIEND_LIST_NOTEBUTTON:
	//	{
	//		if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
	//		{
	//			if ( m_nSelectIndex < 0 ) return ;
	//			GetParent()->AddMessageEx( UIMSG_WRITE_NOTE_VISIBLE );
	//		}
	//	}
	//	break;
	}
}

void CFriendWindowNormalPage::ADD_FRIEND ( const CString& strName )
{
	if ( !strName.GetLength () ) return ;

	m_strSelectName = strName;

	std::tr1::shared_ptr<SFRIEND> pFriend = m_pGaeaClient->GetFriendClient()->GetData (m_strSelectName.GetString());
	if ( pFriend && !pFriend->IsBLOCK () )
	{
		m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, sc::string::format( ID2GAMEINTEXT("FRIEND_ADD_ALREADY"), strName ) );
		return ;
	}

	INT nTotalLine = static_cast<INT>(m_pGaeaClient->GetFriendClient()->Size( EMFRIEND_VALID ));
	//nTotalLine += static_cast<INT>(m_pGaeaClient->GetFriendClient()->Size( EMFRIEND_BETRAY ));
	if( nTotalLine >= GLCONST_CHAR::nMAX_FRIEND_NUMBER )
	{
		m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, sc::string::format( ID2GAMEINTEXT("FRIEND_ADD_MAX"), GLCONST_CHAR::nMAX_FRIEND_NUMBER ) );
		return ;
	}

	m_pInterface->DoModal ( sc::string::format( ID2GAMEINTEXT("FRIEND_ADD"), strName ), UI::MODAL_QUESTION, UI::YESNO, UI::MODAL_FRIEND_ADD );
}

void CFriendWindowNormalPage::DEL_FRIEND ( const CString& strName )
{
	if ( !strName.GetLength () ) return ;

	m_strSelectName = strName;

	m_pInterface->DoModal ( sc::string::format( ID2GAMEINTEXT("FRIEND_DEL"), strName ), UI::MODAL_QUESTION, UI::YESNO, UI::MODAL_FRIEND_DEL );
}

void CFriendWindowNormalPage::MOVETO_FRIEND ( const CString& strName )
{
	if ( !strName.GetLength () ) return ;

	m_strSelectName = strName;

	const friendlogic::FRIENDMAP& FriendMap = m_pGaeaClient->GetFriendClient()->GetList();

	//	온라인이 아니면 처리하지 않음	
	for (friendlogic::FRIENDMAP_CITER iter=FriendMap.begin(); iter != FriendMap.end(); ++iter)
	{
		std::tr1::shared_ptr<SFRIEND> sFriend = iter->second;
		if (!sFriend->m_ChaName.compare(m_strSelectName.GetString()))
        {
			if (!sFriend->m_Online)
                return;
		}
	}
	m_pInterface->DoModal ( sc::string::format( ID2GAMEINTEXT("FRIEND_MOVE"), strName ), UI::MODAL_QUESTION, UI::YESNO, UI::MODAL_FRIEND_MOVE );
}

void CFriendWindowNormalPage::PARTY_FRIEND ( const CString& strName )
{
	
	if ( !strName.GetLength () )
		return ;

	m_strSelectName = strName;

	const friendlogic::FRIENDMAP& FriendMap = m_pGaeaClient->GetFriendClient()->GetList();

	//	온라인이 아니면 처리하지 않음
	std::string strFind = m_strSelectName.GetString();

	friendlogic::FRIENDMAP_CITER iter = FriendMap.find(strFind);
	friendlogic::FRIENDMAP_CITER iter_end = FriendMap.end ();	

	if ( iter == iter_end )
        return;
	
	std::tr1::shared_ptr<SFRIEND> sFriend = iter->second;
	if (!sFriend->m_Online)
        return;

    DxGlobalStage* pGlobalStage = m_pGaeaClient->GetGlobalStage();
    if (sFriend->m_Channel != pGlobalStage->GetChannel())
        return;
    
	GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
	if ( !pCharacter )
		return;

	//	파티장이 아니면 지원하지 않는다.
	if ( m_pGaeaClient->GetMyPartyClient()->isMaster(pCharacter->GetGaeaID()) == false )
		return;

	VEC_LINK_DATA_BASIC vecLinkDataBasic;

	CString strChatMsg, strTemp;

	SLINK_DATA_BASIC sLinkDataBasic;

	sLinkDataBasic.sLinkType.wMType = PARTY_LINK;
	sLinkDataBasic.sLinkType.wSType = PARTY_LINK_RECRUIT;

	strTemp.Format( "%s%d%d%s", strLINK_LEFT_SIMBOL, PARTY_LINK, PARTY_LINK_RECRUIT, strLINK_RIGHT_SIMBOL );		
	strChatMsg += strTemp;

	strTemp = sc::string::format( ID2GAMEWORD( "CHAT_LINK_PARTY_RECRUIT", 5 ), m_strSelectName ).c_str();
	strChatMsg += strTemp;

	vecLinkDataBasic.push_back(sLinkDataBasic);

	pCharacter->ReqSendChatMsg ( CHAT_CONST::CHAT_PRIVATE, m_strSelectName.GetString(), strChatMsg.GetString(), vecLinkDataBasic );


}

void CFriendWindowNormalPage::EDIT_BEGIN ( const CString& strName )
{
	if( m_pEditBox )
	{
		m_pEditBox->EndEdit ();
		m_pEditBox->ClearEdit ();

		m_pEditBox->SetEditString ( strName );
		m_pEditBox->BeginEdit ();	
	}
}

void CFriendWindowNormalPage::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	m_pListText->SetUseOverColor ( FALSE );
	if ( 0 <= m_nSelectIndex && m_nSelectIndex < m_pListText->GetCount () )
		m_pListText->SetUseTextColor ( m_nSelectIndex, FALSE );

	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );


	if ( 0 <= m_nSelectIndex && m_nSelectIndex < m_pListText->GetCount () )
	{
		m_pListText->SetUseTextColor ( m_nSelectIndex, TRUE );
		m_pListText->SetTextColor ( m_nSelectIndex, NS_UITEXTCOLOR::ORANGE );
	}

	{
		CBasicScrollThumbFrame* const pThumbFrame = m_pListScrollBar->GetThumbFrame ();

		const int nTotalLine = m_pListText->GetTotalLine ();
		const int nLinePerOneView = m_pListText->GetVisibleLine ();
		CDebugSet::ToView ( 1, 10, "보이는 라인 %d", nLinePerOneView );
		pThumbFrame->SetState ( nTotalLine, nLinePerOneView );
		if ( nLinePerOneView < nTotalLine )
		{
			const int nMovableLine = nTotalLine - nLinePerOneView;
			float fPercent = pThumbFrame->GetPercent ();
			int nPos = (int)floor(fPercent * nMovableLine);
			m_pListText->SetCurLine ( nPos );			
		}
	}
}

void CFriendWindowNormalPage::SetVisibleSingle ( BOOL bVisible )
{
	CUIGroup::SetVisibleSingle ( bVisible );

	if( bVisible )
	{
		LoadFriendList ();

		m_nSelectIndex = -1;

		if( GLCONST_CHAR::bBATTLEROYAL )
		{
			m_pADD->SetVisibleSingle( FALSE );
		}
	}
	else
	{
		EDIT_END ();
	}
}

void CFriendWindowNormalPage::EDIT_END ()
{
	if( m_pEditBox ) m_pEditBox->EndEdit();
}

void CFriendWindowNormalPage::ADD_FRIEND_NAME_TO_EDITBOX ( const CString& strName )
{
	if( m_pEditBox ) m_pEditBox->SetEditString( strName );
}

void CFriendWindowNormalPage::ADD_BLOCK ( const CString& strName )
{
	if ( !strName.GetLength () ) return ;

	m_strSelectName = strName;

	std::tr1::shared_ptr<SFRIEND> pFriend = m_pGaeaClient->GetFriendClient()->GetData (m_strSelectName.GetString());
	if ( pFriend && pFriend->IsBLOCK () )
	{
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, sc::string::format( ID2GAMEINTEXT("FRIEND_BLOCK_ADD_ALREADY"), strName ) );
		return ;
	}

	INT nTotalLine = static_cast<INT>(m_pGaeaClient->GetFriendClient()->Size( EMFRIEND_BLOCK ));
	if( nTotalLine >= GLCONST_CHAR::nMAX_FRIEND_NUMBER )
	{
		m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, sc::string::format( ID2GAMEINTEXT("FRIEND_ADD_MAX"), GLCONST_CHAR::nMAX_FRIEND_NUMBER ) );
		return ;
	}

	m_pInterface->DoModal ( sc::string::format( ID2GAMEINTEXT("FRIEND_BLOCK_ADD"), strName ), UI::MODAL_QUESTION, UI::YESNO, UI::MODAL_FRIEND_BLOCK_ADD_N );
}
