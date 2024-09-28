#include "StdAfx.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicTextBoxEx.h"
#include "../../../EngineLib/GUInterface/BasicScrollBarEx.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicLineBox.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/BasicScrollThumbFrame.h"
#include "../../../EngineLib/GUInterface/BasicLineBoxSmart.h"
#include "../../../EngineLib/GUInterface/UIMan.h"
#include "../../../EngineLib/GUInterface/UIKeyCheck.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/DxTools/DebugSet.h"
#include "../../../EngineLib/GUInterface/BasicButton.h"
#include "../../../EngineLib/Common/DXInputString.h"
#include "../../../EngineLib/GUInterface/BasicVarTextBox.h"

#include "../../../RanLogic/GLogicDataMan.h"
#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"
#include "../../../RanLogicClient/GroupChat/GroupChatClient.h"
#include "../../../RanLogicClient/Char/CharNameDbNumCache.h"
#include "../../../RanLogicClient/Friend/GLFriendClient.h"
#include "../../../RanLogicClient/Club/GLClubClient.h"
#include "../../../RanLogic/GLUseFeatures.h"

#include "../../../MfcExLib/RanFilter.h"


#include "../../InnerInterface.h"
#include "MaxChatWindow.h"
#include "ChatPopUpWindow.h"
#include "ChatRoomList.h"
#include "../Post/PostBoxPage.h"
#include "../Post/PostBoxWindowNaked.h"
#include "../Post/PostBoxWindowObject.h"
#include "../Post/PostBoxFriendFindPage.h"
#include "../util/RnButton.h"
#include "../Util/PopupMenuComponent.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


//설정 파일로 이동 할것 bjkim
const int nLIMITCHAR = CHAT_MSG_SIZE - 10;
//const int MAX_ROOM_MEMBER = 50;
//const CString strTextt ="초대하기";
//const std::string strRoomBoss ="님의 방";
const int SMALL_CHAT_UI_SIZE_X = 220;
const int SMALL_CHAT_UI_SIZE_Y = 100;


CMaxChatWindow::CMaxChatWindow ( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice, LPDIRECT3DDEVICEQ pd3dDevice )
: CUIWindowEx ( pInterface, pEngineDevice )
, m_pInterface ( pInterface )
, m_pGaeaClient ( pGaeaClient )
, m_nSelectMap ( -1 )
, m_nSelectStop( -1 )
, m_pNameList ( NULL )
, m_pMapListScrollBar ( NULL )
, m_pTalkList ( NULL )
, m_pStopListScrollBar ( NULL )
, m_pMoveButton ( NULL )
, m_pTitle ( NULL )
, m_pStopTitle ( NULL )
, m_bFirstGap ( FALSE )
, m_pMaxChatWindowSizeSetButton ( NULL )
, m_bFlag ( FALSE )
, m_pEditBox ( NULL )
, m_link (FALSE )
, m_bRoomFree(TRUE)
, m_pd3dDevice ( NULL )
, m_bNewMsg ( FALSE )
, m_dwKey ( INVALID_CHAT_KEY )
, m_nNameListIndex (-1)
, m_dwTargetDbNum ( INVALID_CHAT_KEY )
{
    ResetExclusiveControl();
}

namespace
{
    const char* ID2GAMEWORD_TITLE;
    const char* ID2GAMEWORD_RESET;
    const char* ID2GAMEWORD_CONFIRM;
}

CMaxChatWindow::~CMaxChatWindow ()
{
}

void CMaxChatWindow::CreateSubControl ()
{
    CD3DFontPar* pFont = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );
    {
		CD3DFontPar* pFontTitle = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_SHADOW_EX_FLAG );
        CBasicTextBox* pTextBox = new CBasicTextBoxEx(m_pEngineDevice);
        pTextBox->CreateSub ( this, "MAX_GROUP_CHAT_WINDOW_TITLE_POS", UI_FLAG_DEFAULT);
        pTextBox->SetFont ( pFontTitle );
        pTextBox->SetTextAlign( TEXT_ALIGN_CENTER_Y );
        RegisterControl ( pTextBox );
        m_pTitle = pTextBox;
    }
 
    {
        CBasicLineBox* pBasicLineBox = new CBasicLineBox(m_pEngineDevice);
		pBasicLineBox->CreateSub ( this, "MAX_GROUP_CHAT_NAME_LIST", UI_FLAG_DEFAULT );
        pBasicLineBox->CreateBaseBoxQuestList ( "MAX_GROUP_CHAT_NAME_LIST" );
        RegisterControl ( pBasicLineBox );

		//이름창 텍스트 박스
        CBasicTextBoxExLink* pTextBox = new CBasicTextBoxExLink(m_pEngineDevice);
        pTextBox->CreateSub ( this, "MAX_GROUP_CHAT_NAME_LIST", UI_FLAG_YSIZE, MAX_GROUP_CHAT_USERS_NAME_TEXT_BOX );
        pTextBox->SetFont ( pFont );		
        pTextBox->SetLineInterval ( 5.0f );
        pTextBox->SetSensitive ( true );
        RegisterControl ( pTextBox );
        m_pNameList = pTextBox;

        int nTotalLine = pTextBox->GetVisibleLine ();

        //이름창 스크롤바
        CBasicScrollBarEx* pScrollBar = new CBasicScrollBarEx(m_pEngineDevice);
        pScrollBar->CreateSub ( this, "BASIC_SCROLLBAR",  UI_FLAG_YSIZE, MAX_GROUP_CHAT_ROOM_NAME_SCROLLBAR);
        pScrollBar->CreateBaseScrollBar ( "MAX_GROUP_CHAT_NAME_SCROLLBAR" );
        pScrollBar->GetThumbFrame()->SetState ( 1, nTotalLine );
        RegisterControl ( pScrollBar );
        m_pMapListScrollBar = pScrollBar;
        m_pMapListScrollBar->GetThumbFrame ()->SetScrollParent(m_pNameList);

        RnButton::CreateArg arg;
        //////////////////////////////////////////////////////////////////////////
        arg.defaultTextureKey = "RNBUTTON_DEFAULT_TEXTURE";
        arg.mouseOverTextureKey = "RNBUTTON_OVER_TEXTURE";
        arg.mouseClickTextureKey = "RNBUTTON_CLICK_TEXTURE";
        //////////////////////////////////////////////////////////////////////////
        arg.dwColor = NS_UITEXTCOLOR::WHITE;

        // 초대하기 버튼
        CString strText;
        strText.Format( "%s", ID2GAMEWORD("MAX_GROUP_CHAT_INVITE") );
        arg.text = strText.GetString();
        RnButton* pReset = new RnButton(m_pEngineDevice);
        pReset->CreateSub(this, "MAX_GROUP_CHAT_INVITE_BUTTON_POS", UI_FLAG_LEFT | UI_FLAG_BOTTOM, MAX_GROUP_CHAT_INVITE_BUTTON);
        pReset->CreateSubControl(arg);
        RegisterControl(pReset);
        m_pInviteButton = pReset;
    }


    {
		//	대화 내용창 텍스트 박스
		CBasicTextBoxExLink* pTextBox = new CDynamicBasicTextBoxExLink(m_pEngineDevice);
		pTextBox->CreateSub ( this, "MAX_GROUP_CHAT_TALK_LIST", UI_FLAG_XSIZE | UI_FLAG_YSIZE, MAX_GROUP_CHAT_USERS_TALK_TEXT_BOX );
		pTextBox->SetFont ( pFont );
		pTextBox->SetTextAlign ( TEXT_ALIGN_BOTTOM );
		pTextBox->SetLineInterval ( 5.0f );
		pTextBox->SetPartInterval ( 2.0f );
		RegisterControl ( pTextBox );
		m_pTalkList = pTextBox;

		int nTotalLine = pTextBox->GetVisibleLine ();

		//	대화 내용창 스크롤바
		CBasicScrollBarEx* pScrollBar = new CBasicScrollBarEx(m_pEngineDevice);
		pScrollBar->CreateSub ( this, "BASIC_SCROLLBAR", UI_FLAG_RIGHT | UI_FLAG_YSIZE, MAX_GROUP_CHAT_USERS_TALK_SCROLLBAR );
		pScrollBar->CreateBaseScrollBar ( "MAX_GROUP_CHAT_TALK_SCROLLBAR" );
		pScrollBar->GetThumbFrame()->SetState ( 1, nTotalLine );
		RegisterControl ( pScrollBar );
		m_pStopListScrollBar = pScrollBar;
		m_pStopListScrollBar->GetThumbFrame ()->SetScrollParent(m_pTalkList);
		
    }
	{	//에디트 박스 평상시 배경화면
		m_pEditBoxBack_ENDEDIT = new CUIControl(m_pEngineDevice);
		m_pEditBoxBack_ENDEDIT->CreateSub ( this, "MAX_GROUP_CHAT_TALK_INPUT_BOX_BG",UI_FLAG_XSIZE | UI_FLAG_BOTTOM );
		m_pEditBoxBack_ENDEDIT->SetUseRender ( TRUE );
		m_pEditBoxBack_ENDEDIT->SetDiffuse ( D3DCOLOR_ARGB(150,0,0,0) );
		RegisterControl ( m_pEditBoxBack_ENDEDIT );
	}
	{	//대화내용 입력중 에디트 박스 배경화면
		m_pEditBoxBack_BEGINEDIT = new CUIControl(m_pEngineDevice);
		m_pEditBoxBack_BEGINEDIT->CreateSub ( this, "MAX_GROUP_CHAT_TALK_INPUT_BOX_BG",UI_FLAG_XSIZE | UI_FLAG_BOTTOM );
		m_pEditBoxBack_BEGINEDIT->SetUseRender ( TRUE );
		m_pEditBoxBack_BEGINEDIT->SetDiffuse ( D3DCOLOR_ARGB(70,255,255,255) );
		RegisterControl ( m_pEditBoxBack_BEGINEDIT );
	}
	{	//에디트 박스 아웃라인
		m_pEditBoxLine = new CBasicLineBoxSmart(m_pEngineDevice);
		m_pEditBoxLine->CreateSub( this, "MAX_GROUP_CHAT_TALK_INPUT_BOX_BG", UI_FLAG_XSIZE | UI_FLAG_BOTTOM );
		m_pEditBoxLine->CreateSubControl( "TEXTURE_EMPTYBG_BLACKLINE" );
		RegisterControl( m_pEditBoxLine );
	}
    {	//에디트 박스
        m_pEditBox = new CUIEditBoxLink(m_pEngineDevice);
        m_pEditBox->CreateSub ( this, "MAX_GROUP_CHAT_TALK_INPUT_BOX",UI_FLAG_XSIZE | UI_FLAG_BOTTOM, MAX_GROUP_CHAT_TALK_INPUT_EDIT_BOX );
        m_pEditBox->CreateCarrat ( "MAX_GROUP_CHAT_TALK_INPUT_BOX_CARRAT", TRUE, UINT_MAX );
        m_pEditBox->SetLimitInput ( CHAT_MSG_SIZE - 10 );//에디트박스 최대 입력 내용
        m_pEditBox->SetFont ( pFont );
        RegisterControl ( m_pEditBox );
    }
    {	//맥스 그룹채팅창 사이즈 조절 버튼
        CBasicButton* pButton = new CBasicButton(m_pEngineDevice);
        pButton->CreateSub ( this, "MAX_GROUP_CHAT_RESIZE_BUTTON", UI_FLAG_RIGHT | UI_FLAG_BOTTOM,MAX_GROUP_CHAT_RESIZE_BUTTON );
        pButton->CreateFlip ( "MAX_GROUP_CHAT_RESIZE_BUTTON_F", CBasicButton::MOUSEIN_FLIP );	
        RegisterControl ( pButton );
        m_pMaxChatWindowSizeSetButton = pButton;
    }
	{	//친구 관계 표시 창
		// 그룹채팅창내 이름창내 이름위에 커서를 올리면
		// 커서 옆에 표시 창이 나타난다
		CBasicVarTextBox* pInfo = new CBasicVarTextBox(m_pEngineDevice);
		pInfo->CreateSub( this, "BASIC_VAR_TEXT", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pInfo->CreateSubControl();
		pInfo->SetVisibleSingle( FALSE );
		RegisterControl( pInfo );
		m_pInfo = pInfo;
	}

}


void CMaxChatWindow::EDIT_BEGIN ( const CString& strMessage )
{
    if ( strMessage.GetLength () ) m_pEditBox->SetEditString ( strMessage );

    m_pEditBox->BeginEdit ();
}

void CMaxChatWindow::ChatToLink( const SLINK_DATA_BASIC& sLinkDataBasic, const CString& strLinkName, DWORD dwColor, const SITEMCUSTOM *pItemCustom )
{
	if ( !m_pEditBox->IsBegin() )
	{
		m_pEditBox->BeginEdit();
	}

	m_pEditBox->ChatToLink ( sLinkDataBasic, strLinkName, dwColor, pItemCustom );

    m_dwColor = dwColor;
    m_link = TRUE;
}

void CMaxChatWindow::SetMaxChatWindoWTitel( CString strRoomName )
{
	m_strRoomName = strRoomName;
    m_pTitle->ClearText();
	m_nMaxRoomMemberCount = m_pGaeaClient->GetChatGroup()->GetMaxGroupChatTO();
	
	std::string strRoomTitle = stringformat(ID2GAMEWORD("MAX_GROUP_CHAT_ROOM_MASTER"), 
		strRoomName, m_dwRoomMemberCount, m_nMaxRoomMemberCount);
	m_pTitle->AddText( strRoomTitle.c_str(),NS_UITEXTCOLOR::WHITE );
	
}



CBasicTextBoxExLink* CMaxChatWindow::GetTalkBox()
{
	return m_pTalkList;
}


void CMaxChatWindow::StrTagChange(std::string strTag, std::string strItemname, std::string &strChat)
{
	std::string::size_type offset = 0;
	int count = 0;
	while( count < HOW_MANY_ITEM_LINK_PER_CHAT_MSG )    
	{
		count++;
		offset = strChat.find( strTag, offset );

		if( std::string::npos == offset )
			break;                 
		else
		{
			strChat.replace( offset, strTag.length(), strItemname );
			break;
		}
	}     
}

void CMaxChatWindow::SetScroolBarUpDate(CBasicTextBoxExLink* pBasicTextBoxExLink)
{
	CBasicScrollThumbFrame* const pThumbFrame = m_pStopListScrollBar->GetThumbFrame ();
	if ( !pThumbFrame )
	{
		GASSERT ( 0 && "썸이 만들어지지 않았습니다.");
		return ;
	}

	int nTotal, nView;
	pThumbFrame->GetState ( nTotal, nView );	

	const int nTotalLine = pBasicTextBoxExLink->GetTotalLine();
	pThumbFrame->SetState ( nTotalLine, nView );
	pThumbFrame->SetPercent ( CBasicScrollThumbFrame::fHUNDRED_PERCENT );

}

void CMaxChatWindow::AddStringToNORMAL ( CString strTemp, D3DCOLOR dwColor, VEC_LINK_TEXT_DATA& vecLinkTextData, bool bGM )
{
	if ( m_pTalkList && m_pStopListScrollBar )
	{

		std::string strType = "";
		std::string strName = "";
		std::string strContent = "";

		GetSeparateCharName( strTemp, vecLinkTextData, strType, strName, strContent );

		int nIndex = m_pTalkList->AddText( strType.c_str(), dwColor );

		if( bGM )
			m_pTalkList->AddString( nIndex, strName.c_str(), NS_UITEXTCOLOR::CHAT_GM );
		else
			m_pTalkList->AddString( nIndex, strName.c_str(), NS_UITEXTCOLOR::CHAT_CHAR_NAME );

		m_pTalkList->AddStringLink( nIndex, strContent.c_str(), vecLinkTextData, dwColor );

	}
}

void CMaxChatWindow::SetNewMsgState( BOOL bState )
{
	m_bNewMsg = bState;
}

BOOL CMaxChatWindow::ISNewMsgState()
{
	return m_bNewMsg;
}

void CMaxChatWindow::GetSeparateCharName( const CString& strText, VEC_LINK_TEXT_DATA& vecLinkTextData, std::string& strOutType, std::string& strOutName, std::string& strOutContent )
{
	CString strType = "";
	CString strName = "";
	CString strContent = "";

	int nBegin = 0;
	int nEnd = 0;

	nBegin = strText.Find( _T('('));
	if ( nBegin >= 0 ) 
	{
		nEnd = strText.Find( _T("):") );
		if ( nEnd > 0 ) 
		{
			int nCnt =  nEnd - nBegin - 1; 

			strName = strText.Mid( nBegin+1, nCnt );

			strType = strText.Mid( 0, nBegin+1 );
		}
		else
		{
			nBegin = 0;
			nEnd = 0;
		}
	}
	else
	{
		nBegin = 0;
		nEnd = 0;
	}

	strContent = strText.Mid( nEnd );

	strOutType = stringformat( "%1%", strType );
	strOutName = stringformat( "%1%", strName );
	strOutContent = stringformat( "%1%", strContent );

	for ( DWORD i = 0; i < vecLinkTextData.size(); ++i )
	{
		vecLinkTextData[i].sLinkPos.nStart -= nEnd;
		vecLinkTextData[i].sLinkPos.nEnd -= nEnd;
	}
}

void CMaxChatWindow::InfoMenu( int ClickIndex  /* = -1 */ )
{
	m_pInterface->ReferCharWindowOpen( (std::string)m_strCharName );
}

void CMaxChatWindow::FriendRegisterMenu( int ClickIndex  /* = -1 */ )
{
	m_pInterface->ADD_FRIEND( m_strCharName );
}

void CMaxChatWindow::BlockRegisterMenu( int ClickIndex  /* = -1 */ )
{
	m_pInterface->ADD_FRIEND_BLOCK( m_strCharName, true );
}

void CMaxChatWindow::BLockReleaseMenu( int ClickIndex  /* = -1 */ )
{
	m_pGaeaClient->GetFriendClient()->ReqFriendDel ( m_strCharName );
}

void CMaxChatWindow::InviteMenu( int ClickIndex  /* = -1 */ )
{
	std::string strTemp = sc::string::format( ID2GAMEINTEXT("PARTY_QUESTION"), m_strCharName ).c_str();
	DWORD dwDbnum = m_pGaeaClient->FindChaDbNumAtCache(std::string(m_strCharName.GetString()));
	if ( dwDbnum == m_pGaeaClient->GetCharacter()->GetCharID() )
		return;
	m_pInterface->DoPartyModal( strTemp, UI::MODAL_PARTY_REQ_DBNUM, EMPARTY_MODE_PARTY, dwDbnum );
}

void CMaxChatWindow::GetoutRoomMenu( int ClickIndex  /* = -1 */ )
{
	GLMSG::SNET_GCHAT_KICK_CA msg;
	msg.dwChaNum = m_dwTargetDbNum;
	m_pGaeaClient->NETSENDTOAGENT(&msg);
}

void CMaxChatWindow::EditBoxControlCopy()
{
	if (DxInputDevice::GetInstance().IsCtrlDown() && (DxInputDevice::GetInstance().GetKeyState(DIK_C) & DXKEY_DOWN))
	{                        
		if (m_pEditBox->IsBegin())
		{            
			const CString& EditStr = m_pEditBox->GetEditString();
			sc::string::stringToClipboard(EditStr.GetString());
		}
	}
}

void CMaxChatWindow::EditBoxControlPaste()
{
	if (DxInputDevice::GetInstance().IsCtrlDown() && (DxInputDevice::GetInstance().GetKeyState(DIK_V) & DXKEY_DOWN))
	{                        
		if (m_pEditBox->IsBegin())
		{
			std::string ClipBoardText;
			sc::string::clipboardToString2(NULL, ClipBoardText);
			m_pEditBox->InsertString(ClipBoardText.c_str());
		}
	}
}

void	CMaxChatWindow::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    m_MousePosX = x;
    m_MousePosY = y;

    if ( !IsVisible () )
        return ;

	//대화 참가자 이름 갱신
	//이름 표시창의 이름을 지운다
    m_pNameList->ClearText ();
	std::vector<CHAT_MEMBER> member;

	//방의 DB번호와 캐릭터의 DB번호를 넣으면
	// 케릭터의 이름을 넘겨준다
	m_pGaeaClient->GetChatGroup()->GetMembername(member,m_dwKey);

	//현재 방의 맴버 수를 구한다
	int memberCount = static_cast<int>(member.size());
	DWORD dwMydbNum = m_pGaeaClient->GetCharacter()->GetCharID();
	SetRoomMemberCount(memberCount);
	SetMaxChatWindoWTitel(m_strRoomName);

	//나와의 관계를 아이콘으로 표시
	for ( int i = 0; i < memberCount; i++ )
	{
		m_pNameList->AddText ( member[i].szChaName, NS_UITEXTCOLOR::WHITE );
		std::tr1::shared_ptr<SFRIEND> spFriend = m_pGaeaClient->GetFriendClient()->GetData( member[i].dwChaNum );
		bool bIsMember = m_pGaeaClient->GetMyClub()->IsMember(member[i].dwChaNum);

		if (dwMydbNum == member[i].dwChaNum)// 자신이면
		{
			m_pNameList->SetTextImage( i, "GROUP_CHAT_STATUS_ICON_FRIENDSHIP");
			continue;
		}

		if (spFriend != NULL && spFriend->IsBLOCK())//내가 상대를 차단
		{
			m_pNameList->SetUseTextColor( i, TRUE );
			m_pNameList->SetTextColor( i, NS_UITEXTCOLOR::RED );
			m_pNameList->SetTextImage( i, "GROUP_CHAT_STATUS_ICON_BLOCK");
			continue;
		}

		if ( bIsMember ) // 클럽원이면
		{
			m_pNameList->SetTextImage( i, "GROUP_CHAT_STATUS_ICON_FRIENDSHIP");
			continue;
		}

		if (spFriend != NULL && spFriend->IsVALID())//친구
		{
			m_pNameList->SetTextImage( i, "GROUP_CHAT_STATUS_ICON_FRIENDSHIP");
			continue;
		}
				
		if (spFriend != NULL && spFriend->IsOFF())// 친구가 아님
		{
			m_pNameList->SetTextImage( i, "GROUP_CHAT_STATUS_ICON_NORMAL");
			continue;
		}

// 		if (spFriend != NULL && spFriend->IsBETRAY())// 상대가 나를 차단
// 		{
// 			m_pNameList->SetTextImage( i, "GROUP_CHAT_STATUS_ICON_NORMAL");
// 			continue;
// 		}

		//아무 관계도 아닐때
		m_pNameList->SetTextImage( i, "GROUP_CHAT_STATUS_ICON_NORMAL");
	}

	if( m_nNameListIndex > -1 )
	{
		if( m_pNameList->GetSelectPos() > -1 )
		{
			m_pNameList->SetUseTextColor( m_pNameList->GetSelectPos(), TRUE );
			m_pNameList->SetTextColor( m_pNameList->GetSelectPos(), NS_UITEXTCOLOR::YELLOW );
		}

	}
	else
	{
		if( m_pNameList->GetSelectPos() > -1 )
		{
			m_pNameList->SetUseTextColor( m_pNameList->GetSelectPos(), TRUE );
			m_pNameList->SetTextColor( m_pNameList->GetSelectPos(), NS_UITEXTCOLOR::WHITE );
		}
	}

	//MaxChat창 크기 조절
    if( m_pMaxChatWindowSizeSetButton->IsExclusiveControl()  )
    {
        D3DXVECTOR2 vDELTA;
        const UIRECT& rcGlobalPos = GetGlobalPos ();

        vDELTA.x = static_cast<float>(x) - rcGlobalPos.left;			
        vDELTA.y = static_cast<float>(y) - rcGlobalPos.top;

        if ( vDELTA.x > SMALL_CHAT_UI_SIZE_X || vDELTA.y > SMALL_CHAT_UI_SIZE_Y )
        {        
            WORD wAlignFlag = GetAlignFlag ();
            SetAlignFlag ( UI_FLAG_XSIZE | UI_FLAG_YSIZE );
            {
                UIRECT rcNewPos(rcGlobalPos.left, rcGlobalPos.top
                    /* +6 마우스 커서가 버튼 모서리로 가서 가장자리로 이동시킴*/
                    , vDELTA.x > SMALL_CHAT_UI_SIZE_X ? vDELTA.x + 6 : rcGlobalPos.sizeX                     
                    , vDELTA.y > SMALL_CHAT_UI_SIZE_Y ? vDELTA.y + 6 : rcGlobalPos.sizeY  );
                AlignSubControl( rcGlobalPos, rcNewPos );
                SetGlobalPos( rcNewPos );
            }
            SetAlignFlag( wAlignFlag );
        }
    }

	//대화입력창(EDITBOX) 처리부분
    {
        if ( !m_pEditBox->IsBegin() )
        {
            if ( UIKeyCheck::GetInstance()->Check(DIK_RETURN,DXKEY_DOWN) ||
                UIKeyCheck::GetInstance()->Check(DIK_NUMPADENTER,DXKEY_DOWN) )
            {
                CUIControl::AddMessageEx ( UIMSG_KEY_FOCUSED );

                //EDIT_BEGIN ();
                m_pEditBox->BeginEdit ();
            }
        }
        else
        {
            if (UIKeyCheck::GetInstance()->Check(DIK_RETURN,DXKEY_DOWN) ||
                UIKeyCheck::GetInstance()->Check(DIK_NUMPADENTER,DXKEY_DOWN))
            {
				CString strTemp;

                if ( m_link )
                {
                    m_link = FALSE;
										
					GLMSG::SNET_GCHAT_MESSAGE_CAC msg;								
					VEC_LINK_TEXT_DATA_EX vecLinkTextData = m_pEditBox->m_vecLinkTextData;
					int nItemNum = static_cast<int>(vecLinkTextData.size());

					CT2CA CStringTOStdstring( m_pEditBox->GetEditString() );
					std::string strFullChat( CStringTOStdstring );

					for ( int i = 0; i < nItemNum; i ++)
					{
						SITEMCUSTOM sItemCustom = vecLinkTextData[i].sItemcustom;
						std::string tagNew = m_pGaeaClient->GetChatGroup()->m_LinkMan.GetFreeTag();
						std::string itemName = std::string("<") +GLogicData::GetInstance().GetItemName(sItemCustom.Id())+ std::string(">");

						StrTagChange(itemName, tagNew, strFullChat);

						if ( !(msg.AddLink( sItemCustom )) )
							break;
					}

					strTemp =  strFullChat.c_str();
					strTemp.TrimLeft ( _T(" ") );	//	좌측 공백 무시

					// 금지어 필터링
					if ( CRanFilter::GetInstance().ChatFilter( strTemp ) )
					{
						strTemp = CRanFilter::GetInstance().GetProverb();
					}

					wsprintfA( msg.szChat, "%s", strTemp);
					msg.dwChaSay = m_pGaeaClient->GetCharacter()->CharDbNum();
					msg.dwKey = m_dwKey;
					m_pGaeaClient->NETSENDTOAGENT(&msg);
                }
                else
				{
					//에디트 박스가 공백이 아닐경우만 
					if ( m_pEditBox->GetEditString().GetLength() )
					{
						GLMSG::SNET_GCHAT_MESSAGE_CAC msg;

						strTemp =  m_pEditBox->GetEditString();
						strTemp.TrimLeft ( _T(" ")  );	//	좌측 공백 무시

						// 금지어 필터링
						if ( CRanFilter::GetInstance().ChatFilter( strTemp ) )
						{
							strTemp = CRanFilter::GetInstance().GetProverb();
						}

						wsprintf( msg.szChat, "%s", strTemp);
						msg.dwChaSay = m_pGaeaClient->GetCharacter()->CharDbNum();
						msg.dwKey = m_dwKey;                        
						m_pGaeaClient->NETSENDTOAGENT(&msg);
					}
				}

                m_pEditBox->ClearEdit ();
                m_pEditBox->EndEdit ();
				SetScroolBarUpDate( m_pTalkList );
            }
        }

        if ( m_pEditBox->IsBegin () )
        {
            static bool bVALID_INPUT_BACK = true;

            //if ( bVALID_INPUT_BACK && !m_pEditBox->IsVALID_INPUT () )
            //{
            //	AddStringToSystemMessage ( ID2GAMEINTEXT("INVALID_KEYBOARD_INPUT"), NS_UITEXTCOLOR::RED );				
            //}
            bVALID_INPUT_BACK = m_pEditBox->IsVALID_INPUT ();
        }
    }


	//스클롤바 처리
    CUIWindowEx::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

        {
            CBasicScrollThumbFrame* const pThumbFrame = m_pMapListScrollBar->GetThumbFrame ();

            const int nTotalLine = m_pNameList->GetTotalLine ();
            const int nLinePerOneView = m_pNameList->GetVisibleLine ();
            CDebugSet::ToView ( 1, 10, "보이는 라인 %d", nLinePerOneView );
            pThumbFrame->SetState ( nTotalLine, nLinePerOneView );
            if ( nLinePerOneView < nTotalLine )
            {
                const int nMovableLine = nTotalLine - nLinePerOneView;
                float fPercent = pThumbFrame->GetPercent ();
                int nPos = (int)floor(fPercent * nMovableLine);
                m_pNameList->SetCurLine ( nPos );			
            }
        }
     

        {
            CBasicScrollThumbFrame* const pThumbFrame = m_pStopListScrollBar->GetThumbFrame ();

            const int nTotalLine = m_pTalkList->GetTotalLine ();
            const int nLinePerOneView = m_pTalkList->GetVisibleLine ();
            CDebugSet::ToView ( 1, 10, "보이는 라인 %d", nLinePerOneView );
            pThumbFrame->SetState ( nTotalLine, nLinePerOneView );
            if ( nLinePerOneView < nTotalLine )
            {
                const int nMovableLine = nTotalLine - nLinePerOneView;
                float fPercent = pThumbFrame->GetPercent ();
                int nPos = (int)floor(fPercent * nMovableLine);
                m_pTalkList->SetCurLine ( nPos );		
            }
        }

		if ( m_pEditBox->IsBegin() )
		{
			m_pEditBoxBack_BEGINEDIT->SetVisibleSingle ( TRUE );
			m_pEditBoxBack_ENDEDIT->SetVisibleSingle ( FALSE );
			m_pEditBoxLine->SetVisibleSingle( TRUE );
			EditBoxControlCopy();
			EditBoxControlPaste();
		}
		else
		{
			m_pEditBoxBack_BEGINEDIT->SetVisibleSingle ( FALSE );
			m_pEditBoxBack_ENDEDIT->SetVisibleSingle ( TRUE );
			m_pEditBoxLine->SetVisibleSingle( FALSE );
		}

}

void CMaxChatWindow::TranslateMegThumb( UIGUID cID, DWORD dwMsg, CUIControl * pUIControl )
{
    GASSERT( pUIControl );

    if ( CHECK_MOUSE_IN ( dwMsg ) )					//	컨트롤 내부에 마우스가 위치하고,
    {
        if ( CHECK_LB_DOWN_LIKE ( dwMsg ) )			//	마우스를 좌클릭으로 눌렀을 때,
        {
            pUIControl->SetExclusiveControl();		//	단독 컨트롤로 등록하고,
            if ( !m_bFirstGap )						//	최초 포지션 갭을 기록한다.
            {
                UIRECT rcPos = pUIControl->GetGlobalPos ();
                m_vGap.x = m_PosX - rcPos.left;
                m_vGap.y = m_PosY - rcPos.top;

                m_bFirstGap = TRUE;
            }
        }
        else if ( CHECK_LB_UP_LIKE ( dwMsg ) )		//	좌클릭했던것을 놓으면
        {
            pUIControl->ResetExclusiveControl();	//	단독 컨트롤을 해제하고
            m_bFirstGap = FALSE;					//	최초 포지션갭을 해제한다.
        }
    }
    else if ( CHECK_LB_UP_LIKE ( dwMsg ) )			//	컨트롤 영역밖에서 버튼을 떼는 경우가
    {												//	발생하더라도
        pUIControl->ResetExclusiveControl();		//	단독 컨트롤을 해제하고,
        m_bFirstGap = FALSE;						//	최초 포지션갭을 해제한다.
    }
}

void CMaxChatWindow::ScrollMeg( UIGUID cID, DWORD dwMsg, CUIControl * pUIControl )
{
    GASSERT( pUIControl );

    if ( CHECK_MOUSE_IN ( dwMsg ) )					//	컨트롤 내부에 마우스가 위치하고,
        pUIControl->SetExclusiveControl();		//	단독 컨트롤로 등록하고,
    else
        pUIControl->ResetExclusiveControl();
}

void CMaxChatWindow::TranslateUIMessage ( UIGUID cID, DWORD dwMsg )
{
    CUIWindowEx::TranslateUIMessage ( cID, dwMsg );

    switch ( cID )
    {
    case MAX_GROUP_CHAT_RESIZE_BUTTON:
		{
        TranslateMegThumb( cID, dwMsg, m_pMaxChatWindowSizeSetButton );
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( UIMSG_LB_UP & dwMsg )
				{
				}
			}
		}
        break;

    case MAX_GROUP_CHAT_TALK_INPUT_EDIT_BOX:
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
							if ( !(DxInputDevice::GetInstance().GetKeyState ( DIK_LCONTROL ) & DXKEY_PRESSED  ) )
								m_pEditBox->EndEdit();
						}
					}
				}
		}
		break;

    case MAX_GROUP_CHAT_USERS_NAME_TEXT_BOX:
        {
           if ( CHECK_MOUSE_IN ( dwMsg ) )
            {
				const int nIndex = m_pNameList->GetSelectPos();
				if( nIndex < 0 || m_pNameList->GetCount() <= nIndex )
				{
					m_pInfo->SetVisibleSingle( FALSE );
					return;
				}


				m_strCharName = m_pNameList->GetSelectWordText();
				m_nTempNum = m_pGaeaClient->FindChaDbNumAtCache((std::string)m_strCharName);

				if ( m_nTempNum == 0 )
				{
					m_pGaeaClient->ReqChaDbNum((std::string)m_strCharName);
					break;
				}

				//채팅방의 캐릭터의 이름위에 커서를 올리면 나와의 관계를 텍스트로 표시
				CString strISFriend;

				DWORD dwMydbNum = m_pGaeaClient->GetCharacter()->GetCharID();
				std::tr1::shared_ptr<SFRIEND> spFriend = m_pGaeaClient->GetFriendClient()->GetData( m_nTempNum );
				bool bIsMember = m_pGaeaClient->GetMyClub()->IsMember(m_nTempNum);

				strISFriend = ID2GAMEWORD("MAX_GROUP_CHAT_STATE_NORMAL");

				if ( bIsMember )
					strISFriend = ID2GAMEWORD("MAX_GROUP_CHAT_STATE_CLUBMEMBER");

				if( spFriend )
				{
					if (spFriend->IsVALID())//친구
						strISFriend = ID2GAMEWORD("MAX_GROUP_CHAT_STATE_FRIEND");

					if (spFriend->IsBLOCK())//내가 상대를 차단
						strISFriend = ID2GAMEWORD("MAX_GROUP_CHAT_STATE_BLOCK");
				}

				m_pInfo->SetTextNoSplit( strISFriend, NS_UITEXTCOLOR::SILVER );
				m_pInfo->SetGlobalPos(D3DXVECTOR2(static_cast<float>(m_MousePosX)
												, static_cast<float>(m_MousePosY)));
				
				if ( dwMydbNum == m_nTempNum )
					m_pInfo->SetVisibleSingle( FALSE );
				else
					m_pInfo->SetVisibleSingle( TRUE );

                if ( UIMSG_RB_UP & dwMsg )
                {
					m_dwTargetDbNum = m_pGaeaClient->FindChaDbNumAtCache((std::string)m_strCharName);
					m_nNameListIndex = m_pNameList->GetSelectPos();
					m_strCharName = m_pNameList->GetSelectWordText();

					DWORD dwMydbNum = m_pGaeaClient->GetCharacter()->GetCharID();
					{
						popupmenu::SP_POPUP_MENU PopupMenu = popupmenu::CreateRoot();
                        if(GLUseFeatures::GetInstance().IsUsingReferChar())
						    PopupMenu->addComponent(CREATEPOPUPITEM( ID2GAMEWORD("CHAT_GROUP_POP_UP_MENU_INFO"), CMaxChatWindow, InfoMenu ));
						PopupMenu->addComponent(CREATEPOPUPITEM( ID2GAMEWORD("CHAT_GROUP_POP_UP_MENU_FRIEND_REGISTER"), CMaxChatWindow, FriendRegisterMenu ));
						PopupMenu->addComponent(CREATEPOPUPITEM( ID2GAMEWORD("CHAT_GROUP_POP_UP_MENU_BLOCK_REGISTER"), CMaxChatWindow, BlockRegisterMenu ));
						PopupMenu->addComponent(CREATEPOPUPITEM( ID2GAMEWORD("CHAT_GROUP_POP_UP_MENU_BLOCK_RELEASE"), CMaxChatWindow, BLockReleaseMenu ));
						PopupMenu->addComponent(CREATEPOPUPITEM( ID2GAMEWORD("CHAT_GROUP_POP_UP_MENU_INVITE"), CMaxChatWindow, InviteMenu));
						PopupMenu->addComponent(CREATEPOPUPITEM2( ID2GAMEWORD("CHAT_GROUP_POP_UP_MENU_GET_OUT_ROOM"), CMaxChatWindow, GetoutRoomMenu, dwMydbNum == m_dwKey ));

						m_pInterface->OpenPopupMenu( PopupMenu );
					}
						
                }
            }
           else
			   m_pInfo->SetVisibleSingle( FALSE );

        }
        break;

    case MAX_GROUP_CHAT_USERS_TALK_TEXT_BOX:
            if ( CHECK_MOUSE_IN ( dwMsg ) )
            {
                if ( UIMSG_LB_UP & dwMsg )
                {
                    int nLinkIndex = m_pTalkList->GetSelectWordData();
                    if ( nLinkIndex != -1 ) 
                    {
                        //	링크 처리
                        CDebugSet::ToView( 7,1, "Select Data m_pTextBox_ALL : %d", nLinkIndex );

						SITEMCUSTOM sItemcustom;
						m_pGaeaClient->GetChatGroup()->m_LinkMan.GetItem(nLinkIndex, sItemcustom);
						m_pInterface->SHOW_LINK_ITEM_INFO( sItemcustom );
						
                        return;
                    }
                }
                //m_pChatRoolList->SetVisibleSingle(TRUE);
            }
            //else
            //    m_pChatRoolList->SetVisibleSingle(FALSE);

        break;

	case MAX_GROUP_CHAT_INVITE_BUTTON:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( UIMSG_LB_UP & dwMsg )
				{
					if( m_dwKey == m_pGaeaClient->GetCharacter()->GetCharID())
					{
						m_pInterface->UiShowGroupFocus( CHAT_GROUP_INVITE_WINDOW );
					}
					else
					{
						m_pGaeaClient->PrintMsgTextDlg( NS_UITEXTCOLOR::DISABLE, ID2GAMEWORD("MAX_GROUP_CHAT_INVITE_BUTTON_MSG"));
					}

				}
			}
		}
		break;


    default:
        {
            if ( CHECK_LB_UP_LIKE ( dwMsg ) || CHECK_LB_DOWN_LIKE ( dwMsg )  )
            {
				m_nNameListIndex = -1;
				m_pInfo->SetVisibleSingle( FALSE );
            }

			if ( CHECK_RB_UP_LIKE ( dwMsg ) || CHECK_RB_DOWN_LIKE ( dwMsg )  )
			{
				m_pInfo->SetVisibleSingle( FALSE );
			}

        }
        break;
    }
}
  

