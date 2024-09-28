#include "StdAfx.h"
#include "../../../SigmaCore/String/StringFormat.h"
#include "../../../SigmaCore/String/StringUtils.h"

#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/DxTools/DxInputDevice.h"
#include "../../../EngineLib/GUInterface/BasicTextBoxEx.h"
#include "../../../EngineLib/GUInterface/BasicScrollBarEx.h"
#include "../../../EngineLib/GUInterface/BasicScrollThumbFrame.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/DxTools/DxViewPort.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/Common/DXInputString.h"
#include "../../../EngineLib/GUInterface/BasicButton.h"
#include "../../../EngineLib/GUInterface/UIKeyCheck.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../EngineLib/DxResponseMan.h"

#include "../../../RanLogicClient/Friend/GLFriendClient.h"
#include "../../../RanLogicClient/Stage/DxGlobalStage.h"
#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Command/dxincommand.h"
#include "../../../RanLogicClient/Party/GLPartyClient.h"

#include "../../../MfcExLib/RanFilter.h"

#include "../../InnerInterface.h"
#include "../Display/HeadChatDisplayMan.h"
#include "../Interface/SystemMessageDisplay.h"
#include "BasicChatRightBody.h"
#include "BasicTextBoxExLink.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

const int CBasicChatRightBody::nLIMITCHAR = CHAT_MSG_SIZE - 10;
const int CBasicChatRightBody::nSTARTLINE = 0;
const int CBasicChatRightBody::nLIMITLINE = 200;
const D3DCOLOR	CBasicChatRightBody::dwCLICKCOLOR = D3DCOLOR_ARGB(140,255,255,255);

CBasicChatRightBody::CBasicChatRightBody(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice) :
    CUIGroup( pEngineDevice ),
    m_pGaeaClient(pGaeaClient),
	m_pInterface ( pInterface ),
	m_wDisplayState ( CHAT_CONST::CHAT_ALL ),
	m_NamePos ( 0 ),
	m_dwChatType ( CHAT_CONST::CHAT_NORMAL ),
	m_nFRIENDSIZE_BACK ( 0 ),
	m_ChatPos ( 0 ),
	m_ChatCountBack ( 0 ),
	m_NameCountBack ( 0 ),
	m_bIgnoreState ( false ),
	m_wDISPLAYTYPE ( CHAT_CHANNEL_TOTAL ),
	m_bFREEZE ( false ),
	m_fLifeTime( 0.0f ),
	m_nInputCount( 0 ),
	m_nTimeCount( 0 ),
	m_bPapering( FALSE ),
	m_bChatLog ( false ),
	m_nChatLogType ( 0 )
    , m_pEditBoxBack(NULL)
	, m_pEditBox ( NULL )
	, m_pTextBox_ALL(NULL)
	, m_pScrollBar_ALL(NULL)
	, m_pTextBox_PARTY(NULL)
	, m_pScrollBar_PARTY(NULL)
	, m_pTextBox_CLUB(NULL)
	, m_pScrollBar_CLUB(NULL)
	, m_pTextBox_ALLIANCE(NULL)
	, m_pScrollBar_ALLIANCE(NULL)
	, m_pTextBox_CHANNEL(NULL)
	, m_pScrollBar_CHANNEL(NULL)
	, m_pTextBox_SYSTEM(NULL)
	, m_pScrollBar_SYSTEM(NULL)
	, m_pScrollBarDummy(NULL)
	, m_pNativeButton(NULL)
	, m_pEnglishButton(NULL)
	, m_pChatTotalButton(NULL)
	, m_pChatPartyButton(NULL)
	, m_pChatClubButton(NULL)
	, m_pChatAllianceButton(NULL)
	, m_pChatChannelButton(NULL)
	, m_pChatSystemButton(NULL)
	, m_pChatTypeBox(NULL)
{
}

CBasicChatRightBody::~CBasicChatRightBody ()
{
}

void CBasicChatRightBody::CreateSubControl ( CD3DFontPar*	pFont )
{
	CUIControl* pTopBar = new CUIControl(m_pEngineDevice);
	pTopBar->CreateSub ( this, "CHAT_RIGHT_BODY_TOP_BAR" );
	pTopBar->SetProtectSizePoint ();
	pTopBar->SetNoUpdate ( true );
	RegisterControl ( pTopBar );

	CUIControl* pMain = new CUIControl(m_pEngineDevice);
	pMain->CreateSub ( this, "CHAT_RIGHT_BODY_MAIN", UI_FLAG_YSIZE );
	pMain->SetProtectSizePoint ();
	pMain->SetNoUpdate ( true );
	RegisterControl ( pMain );

	CUIControl* pUnder = new CUIControl(m_pEngineDevice);
	pUnder->CreateSub ( this, "CHAT_RIGHT_BODY_UNDER", UI_FLAG_BOTTOM );
	pUnder->SetProtectSizePoint ();
	pUnder->SetNoUpdate ( true );
	RegisterControl ( pUnder );

	CUIControl* pRightBar = new CUIControl(m_pEngineDevice);
	pRightBar->CreateSub ( this, "CHAT_RIGHT_BODY_RIGHT_BAR", UI_FLAG_YSIZE | UI_FLAG_RIGHT );
	pRightBar->SetProtectSizePoint ();
	pRightBar->SetNoUpdate ( true );
	RegisterControl ( pRightBar );

	CUIControl*	pChannelBack = new CUIControl(m_pEngineDevice);
	pChannelBack->CreateSub ( this, "CHAT_CHANNEL_BACK", UI_FLAG_BOTTOM );
	pChannelBack->SetProtectSizePoint ();
	pChannelBack->SetNoUpdate ( true );
	RegisterControl ( pChannelBack );

	
	m_pChatTotalButton = CreateTextButton ( "CHAT_CHANNEL_TOTAL", CHAT_CHANNEL_TOTAL, const_cast<char*>(ID2GAMEWORD("CHAT_CHANNEL_BUTTON",0)) );
	m_pChatPartyButton = CreateTextButton ( "CHAT_CHANNEL_PARTY", CHAT_CHANNEL_PARTY, const_cast<char*>(ID2GAMEWORD("CHAT_CHANNEL_BUTTON",2)) );
	m_pChatClubButton = CreateTextButton ( "CHAT_CHANNEL_CLUB", CHAT_CHANNEL_CLUB, const_cast<char*>(ID2GAMEWORD("CHAT_CHANNEL_BUTTON",3)) );
	m_pChatSystemButton = CreateTextButton ( "CHAT_CHANNEL_SYSTEM", CHAT_CHANNEL_SYSTEM, const_cast<char*>(ID2GAMEWORD("CHAT_CHANNEL_BUTTON",4)) );
	m_pChatAllianceButton = CreateTextButton ( "CHAT_CHANNEL_ALLIANCE", CHAT_CHANNEL_ALLIANCE, const_cast<char*>(ID2GAMEWORD("CHAT_CHANNEL_BUTTON",5)) );
	// 원래 채널탭인데 귓속말만 사용해서 이름만 바꿨다...
	m_pChatChannelButton = CreateTextButton ( "CHAT_CHANNEL_CHANNEL", CHAT_CHANNEL_CHANNEL, const_cast<char*>(ID2GAMEWORD("CHAT_CHANNEL_BUTTON",1)) );	


	int nTotalLine(0);
	int nVisibleLine(0);

	{	//	전체
		m_pTextBox_ALL = CreateTextBoxExLink ( pFont, CHAT_TEXTBOX_ALL );
		nTotalLine = m_pTextBox_ALL->GetTotalLine ();
		nVisibleLine = m_pTextBox_ALL->GetVisibleLine ();
		
		m_pScrollBar_ALL = CreateScrollBarEx ( nTotalLine, nVisibleLine );	
	}

	{	// 채널

		m_pTextBox_CHANNEL = CreateTextBoxExLink ( pFont, CHAT_TEXTBOX_CHANNEL );
		nTotalLine = m_pTextBox_CHANNEL->GetTotalLine ();
		nVisibleLine = m_pTextBox_CHANNEL->GetVisibleLine ();

		m_pScrollBar_CHANNEL = CreateScrollBarEx ( nTotalLine, nVisibleLine );
	}

	{	//	클럽
		m_pTextBox_CLUB = CreateTextBoxExLink ( pFont, CHAT_TEXTBOX_CLUB );
		nTotalLine = m_pTextBox_CLUB->GetTotalLine ();
		nVisibleLine = m_pTextBox_CLUB->GetVisibleLine ();

		m_pScrollBar_CLUB = CreateScrollBarEx ( nTotalLine, nVisibleLine );
	}

	{	//	파티
		m_pTextBox_PARTY = CreateTextBoxExLink ( pFont, CHAT_TEXTBOX_PARTY );
		nTotalLine = m_pTextBox_PARTY->GetTotalLine ();
		nVisibleLine = m_pTextBox_PARTY->GetVisibleLine ();

		m_pScrollBar_PARTY = CreateScrollBarEx ( nTotalLine, nVisibleLine );	
	}

	{	//	동맹
		m_pTextBox_ALLIANCE = CreateTextBoxExLink ( pFont, CHAT_TEXTBOX_ALLIANCE );
		nTotalLine = m_pTextBox_ALLIANCE->GetTotalLine ();
		nVisibleLine = m_pTextBox_ALLIANCE->GetVisibleLine ();

		m_pScrollBar_ALLIANCE = CreateScrollBarEx ( nTotalLine, nVisibleLine );	
	}

	{	//	시스템
		m_pTextBox_SYSTEM = CreateTextBoxExLink ( pFont, CHAT_TEXTBOX_SYSTEM );
		nTotalLine = m_pTextBox_SYSTEM->GetTotalLine ();
		nVisibleLine = m_pTextBox_SYSTEM->GetVisibleLine ();

		m_pScrollBar_SYSTEM = CreateScrollBarEx ( nTotalLine, nVisibleLine );	
	}

	{
		CUIControl* pScrollBarDummy = new CUIControl(m_pEngineDevice);
		pScrollBarDummy->CreateSub ( this, "BASIC_CHAT_SCROLLBAR_DUMMY" );
		pScrollBarDummy->SetVisibleSingle ( FALSE );
		RegisterControl ( pScrollBarDummy );
		m_pScrollBarDummy = pScrollBarDummy;
	}	

	m_pEditBoxBack = new CUIControl(m_pEngineDevice);
	m_pEditBoxBack->CreateSub ( this, "CHAT_EDIT_BOX_BACK", UI_FLAG_BOTTOM );
	m_pEditBoxBack->SetUseRender ( TRUE );
	m_pEditBoxBack->SetDiffuse ( D3DCOLOR_ARGB(70,255,255,255) );
	RegisterControl ( m_pEditBoxBack );

	m_pEditBox = new CUIEditBoxLink(m_pEngineDevice);
	m_pEditBox->CreateSub ( this, "CHAT_EDIT_BOX", UI_FLAG_BOTTOM, CHAT_EDITBOX );
	m_pEditBox->CreateCarrat ( "CHAT_CARRAT", TRUE, UINT_MAX );
	m_pEditBox->SetLimitInput ( nLIMITCHAR );
	m_pEditBox->SetFont ( pFont );	
//	m_pEditBox->SetProtectSizePoint ();
	RegisterControl ( m_pEditBox );

	m_pChatTypeBox = new CBasicTextBox(m_pEngineDevice);
	m_pChatTypeBox->CreateSub ( this, "CHAT_TYPE_TEXT", UI_FLAG_BOTTOM, CHAT_TYPE_TEXT );
	m_pChatTypeBox->SetFont ( pFont );
	RegisterControl ( m_pChatTypeBox );

	{	//	한글 버튼
		m_pNativeButton = new CBasicButton(m_pEngineDevice);
		m_pNativeButton->CreateSub ( this, "CHAT_NATIVE_BUTTON", UI_FLAG_BOTTOM | UI_FLAG_RIGHT, CHAT_NATIVE_BUTTON );
		m_pNativeButton->CreateFlip ( "CHAT_NATIVE_BUTTON_F", CBasicButton::MOUSEIN_FLIP );
		m_pNativeButton->SetUseGlobalAction ( TRUE );
		RegisterControl ( m_pNativeButton );

		m_pEnglishButton = new CBasicButton(m_pEngineDevice);
		m_pEnglishButton->CreateSub ( this, "CHAT_ENGLISH_BUTTON", UI_FLAG_BOTTOM | UI_FLAG_RIGHT, CHAT_ENGLISH_BUTTON );
		m_pEnglishButton->CreateFlip ( "CHAT_ENGLISH_BUTTON_F", CBasicButton::MOUSEIN_FLIP );
		m_pEnglishButton->SetUseGlobalAction ( TRUE );
		RegisterControl ( m_pEnglishButton );
	}

	SetCHATTYPE( CHAT_CONST::CHAT_NORMAL );


}

CBasicTextBoxEx*	CBasicChatRightBody::CreateTextBoxEx( CD3DFontPar* pFont, UIGUID uID )
{
	CBasicTextBoxEx* pTextBox = new CBasicTextBoxEx(m_pEngineDevice);
	pTextBox->CreateSub ( this, "CHAT_BOX_TEXT_BOX", UI_FLAG_YSIZE, uID );
	pTextBox->SetFont ( pFont );
	pTextBox->SetTextAlign ( TEXT_ALIGN_BOTH_X | TEXT_ALIGN_BOTTOM );
	pTextBox->SetLimitLine ( nLIMITLINE );
	pTextBox->SetLineInterval ( 2.0f );
	pTextBox->SetPartInterval ( 2.0f );
	pTextBox->SetProtectSizePoint ();
	RegisterControl ( pTextBox );

	return pTextBox;
}

CBasicTextBoxExLink* CBasicChatRightBody::CreateTextBoxExLink( CD3DFontPar* pFont, UIGUID uID )
{
	CBasicTextBoxExLink* pTextBox = new CBasicTextBoxExLink(m_pEngineDevice);
	pTextBox->CreateSub ( this, "CHAT_BOX_TEXT_BOX", UI_FLAG_YSIZE, uID );
	pTextBox->SetFont ( pFont );
	pTextBox->SetTextAlign ( TEXT_ALIGN_BOTH_X | TEXT_ALIGN_BOTTOM );
	pTextBox->SetLimitLine ( nLIMITLINE );
	pTextBox->SetLineInterval ( 2.0f );
	pTextBox->SetPartInterval ( 2.0f );
	pTextBox->SetProtectSizePoint ();
	RegisterControl ( pTextBox );

	return pTextBox;
}


CBasicScrollBarEx*	CBasicChatRightBody::CreateScrollBarEx ( const int& nTotalLine, const int& nVisibleLine )
{
	CBasicScrollBarEx* pScrollBar = new CBasicScrollBarEx(m_pEngineDevice);
	pScrollBar->CreateSub ( this, "BASIC_SCROLLBAR", UI_FLAG_RIGHT | UI_FLAG_YSIZE );
	pScrollBar->CreateBaseScrollBar ( "CHAT_SCROLLBAR" );
	pScrollBar->SetVisibleSingle( FALSE );
	pScrollBar->GetThumbFrame()->SetState ( nTotalLine, nVisibleLine );	
	pScrollBar->SetProtectSizePoint ();
	RegisterControl ( pScrollBar );

	return pScrollBar;
}

void CBasicChatRightBody::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{	
	const DWORD dwCHATTYPE_BACK = m_dwChatType;

	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	DecreaseTime( fElapsedTime );

	UPDATE_CHAT_PAGE ( bFirstControl );


    //if ( UIKeyCheck::GetInstance()->Check(DIK_Z,DXKEY_DOWN) )
    //{
    //    CUIControl::AddMessageEx ( UIMSG_KEY_FOCUSED );
    //    m_pInterface->UiShowGroupFocus(PTOPMENU_WINDOW);
    //    m_pInterface->GetPtoPWindow()->showGroupChat();
    //}

// Focus 및 Update문제로 주석처리
//	if ( bFirstControl ) 
	{
		if ( IsEDIT_BEGIN() )
		{
			if (UIKeyCheck::GetInstance()->Check(DIK_RETURN,DXKEY_DOWN) ||
				UIKeyCheck::GetInstance()->Check(DIK_NUMPADENTER,DXKEY_DOWN))
			{
				DXInputString* pInputString = m_pEngineDevice->GetInputString();
				if (pInputString->CheckEnterKeyDown())
				{
					CUIControl::AddMessageEx(UIMSG_KEY_FOCUSED);

					//	친구 리스트에 등록된 목록을
					//	귓속말 목록에 추가하는 부분
					ADD_FRIEND_LIST();
					SEND_CHAT_MESSAGE();
				}
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

            CLASSIFY_CHATTYPE ();
			UPDATE_SHORTKEY ();
		}
	}

	//	한/영 모드 보여주기
	BOOL bNative = (m_pEditBox->IsMODE_NATIVE ())?TRUE:FALSE;
	m_pNativeButton->SetVisibleSingle ( bNative );
	m_pEnglishButton->SetVisibleSingle ( !bNative );

	if ( IsEDIT_BEGIN () )
	{
		m_pEditBoxBack->SetVisibleSingle ( TRUE );
		EditBoxControlCopy();
		EditBoxControlPaste();
	}
	else
	{
		m_pEditBoxBack->SetVisibleSingle ( FALSE );
	}
}

void	CBasicChatRightBody::UPDATE_CHAT_PAGE ( const BOOL& bFirstControl )
{
	m_bMINIMUM_SIZE_TEXT = false;
	m_bMINIMUM_SIZE_SCROLL = false;

	if ( m_pTextBox_ALL && m_pScrollBar_ALL )
	{
		//	텍스트박스 보이지 않게 조절
		{			
			const UIRECT& rcGlobalPos = m_pTextBox_ALL->GetGlobalPos ();
			const float fOneLine = m_pTextBox_ALL->CalcMaxHEIGHT(0,0);
			if ( rcGlobalPos.sizeY <= ((fOneLine*2)+3) )
			{
				m_bMINIMUM_SIZE_TEXT = true;
			}
		}

		//	스크롤바 보이지 않게 조절
		{
			const UIRECT& rcGlobalPos = m_pScrollBar_ALL->GetGlobalPos ();
			const float fProtectY = m_pScrollBarDummy->GetGlobalPos ().sizeY;
			if ( rcGlobalPos.sizeY <= fProtectY )
			{
				m_bMINIMUM_SIZE_SCROLL = true;
			}
		}
	}

//	if ( bFirstControl )
	{
		if ( !m_bMINIMUM_SIZE_TEXT )
		{	
			if ( m_pTextBox_ALL->IsVisible () )				UPDATE_CHAT_PAGE_TEXTBOX ( m_pTextBox_ALL, m_pScrollBar_ALL );
			else if ( m_pTextBox_PARTY->IsVisible () )		UPDATE_CHAT_PAGE_TEXTBOX ( m_pTextBox_PARTY, m_pScrollBar_PARTY );
			else if ( m_pTextBox_CLUB->IsVisible () )		UPDATE_CHAT_PAGE_TEXTBOX ( m_pTextBox_CLUB, m_pScrollBar_CLUB );
			else if ( m_pTextBox_SYSTEM->IsVisible () )		UPDATE_CHAT_PAGE_TEXTBOX ( m_pTextBox_SYSTEM, m_pScrollBar_SYSTEM );
			else if ( m_pTextBox_ALLIANCE->IsVisible () )	UPDATE_CHAT_PAGE_TEXTBOX ( m_pTextBox_ALLIANCE, m_pScrollBar_ALLIANCE );
			else if ( m_pTextBox_CHANNEL->IsVisible () )	UPDATE_CHAT_PAGE_TEXTBOX ( m_pTextBox_CHANNEL, m_pScrollBar_CHANNEL );
		}
	}

	SELECT_CHAT_PAGE ();
}

void	CBasicChatRightBody::UPDATE_CHAT_PAGE_TEXTBOX ( CBasicTextBoxEx* pTextBox, CBasicScrollBarEx* pScrollBar )
{
	//	스크롤바 조절
	CBasicScrollThumbFrame* const pThumbFrame = pScrollBar->GetThumbFrame ();	

	const int nTotalLine = pTextBox->GetTotalLine ();
	const int nLinePerOneView = pTextBox->GetVisibleLine ();

	pThumbFrame->SetState ( nTotalLine, nLinePerOneView );	
	if ( nLinePerOneView < nTotalLine )
	{
		const int nMovableLine = nTotalLine - nLinePerOneView;
		float fPercent = pThumbFrame->GetPercent ();
		int nPos = (int)(fPercent * nMovableLine + 0.5f);
		pTextBox->SetCurLine ( nPos );
	}
}

void	CBasicChatRightBody::ADD_CHAT_PAGE_TEXTBOX ( const CString& strTemp, const D3DCOLOR& dwColor, CBasicTextBoxEx* pTextBox, CBasicScrollBarEx* pScrollBar )
{
	GASSERT( pTextBox && pScrollBar );

	if ( !pTextBox || !pScrollBar )
		return;

    std::string strType = "";
    std::string strName = "";
    std::string strContent = "";

    GetSeparateCharName( strTemp, strType, strName, strContent );	

	int nIndex = pTextBox->AddText( strType.c_str(), dwColor );	
    
    pTextBox->AddString( nIndex, strName.c_str(), NS_UITEXTCOLOR::CHAT_CHAR_NAME );
    pTextBox->AddString( nIndex, strContent.c_str(), dwColor );

	//pTextBox->AddText ( strTemp, dwColor );

	CBasicScrollThumbFrame* const pThumbFrame = pScrollBar->GetThumbFrame ();
	if ( !pThumbFrame )
	{
		GASSERT ( 0 && "썸이 만들어지지 않았습니다.");
		return ;
	}

	int nTotal, nView;
	pThumbFrame->GetState ( nTotal, nView );	

	const int nTotalLine = pTextBox->GetTotalLine ();
	pThumbFrame->SetState ( nTotalLine, nView );
	pThumbFrame->SetPercent ( CBasicScrollThumbFrame::fHUNDRED_PERCENT );
}

void CBasicChatRightBody::ADD_CHAT_PAGE_TEXTBOX_LINK ( const CString& strTemp, const D3DCOLOR& dwColor, CBasicTextBoxExLink* pTextBox, 
													  CBasicScrollBarEx* pScrollBar, VEC_LINK_TEXT_DATA& vecLinkTextData, WORD wChatType, bool bGM )
{
	GASSERT( pTextBox && pScrollBar );

	if ( !pTextBox || !pScrollBar )
		return;

    std::string strType = "";
    std::string strName = "";
    std::string strContent = "";

    VEC_LINK_TEXT_DATA vecLinkTextDataCopy = vecLinkTextData;
    GetSeparateCharName( wChatType, strTemp, vecLinkTextDataCopy, strType, strName, strContent );

	int nIndex = pTextBox->AddText( strType.c_str(), dwColor );
    
	if( bGM )
		pTextBox->AddString( nIndex, strName.c_str(), NS_UITEXTCOLOR::CHAT_GM );
	else
		pTextBox->AddString( nIndex, strName.c_str(), NS_UITEXTCOLOR::CHAT_CHAR_NAME );

    pTextBox->AddStringLink( nIndex, strContent.c_str(), vecLinkTextDataCopy, dwColor );

	//pTextBox->AddTextLink ( strTemp, vecLinkTextData, dwColor );

	CBasicScrollThumbFrame* const pThumbFrame = pScrollBar->GetThumbFrame ();
	if ( !pThumbFrame )
	{
		GASSERT ( 0 && "썸이 만들어지지 않았습니다.");
		return ;
	}

	int nTotal, nView;
	pThumbFrame->GetState ( nTotal, nView );	

	const int nTotalLine = pTextBox->GetTotalLine ();
	pThumbFrame->SetState ( nTotalLine, nView );
	pThumbFrame->SetPercent ( CBasicScrollThumbFrame::fHUNDRED_PERCENT );

}

void CBasicChatRightBody::ChangePrivateName ()
{
	if ( !IsEDIT_BEGIN () ) return ;

	const CString& strOrigin = m_pEditBox->GetEditString();
	const int nEditLength = m_pEditBox->GetEditLength ();
	const int nSize = (int)m_NameList.size ();

	if ( nSize != m_NameCountBack )
	{
		m_NamePos = 0;
	}

	m_NameCountBack = nSize;

	if (nSize)
	{
        DXInputString* pInputString = m_pEngineDevice->GetInputString();
		//	문자 조합중이 아닐 경우에만 귓속말 상대 선택이 가능.
		if (!pInputString->CheckComposition())
		{
			BOOL bKEYDOWN = FALSE;
			if ( UIKeyCheck::GetInstance()->Check ( DIK_UP, DXKEY_DOWN) )
			{							
				m_NamePos++;
				if ( nSize <= m_NamePos ) m_NamePos = nSize - 1;
				bKEYDOWN = TRUE;
			}
			else if ( UIKeyCheck::GetInstance()->Check ( DIK_DOWN, DXKEY_DOWN ) )
			{
				m_NamePos--;
				if ( m_NamePos < 0 ) m_NamePos = 0;
				bKEYDOWN = TRUE;
			}

			if ( bKEYDOWN )
			{
				NAMELIST_ITER iter = m_NameList.begin ();
				NAMELIST_ITER iter_end = m_NameList.end ();
				for ( int i = 0; i < m_NamePos && iter != iter_end; ++i, ++iter ) ;
		        
				CString strName = (*iter);
				m_strPrivateName = strName;
				SetCHATTYPE( CHAT_CONST::CHAT_PRIVATE );
			}
		}
	}
	m_strChatBack = strOrigin;
}

bool CBasicChatRightBody::IS_FRIEND ( CString strName )
{
	std::tr1::shared_ptr<SFRIEND> psFriend = m_pGaeaClient->GetFriendClient()->GetData ( strName.GetString () );
	if (psFriend)
        return true;
	else
        return false;
}


CString CBasicChatRightBody::GET_ONLINE_FRIEND ( CString strName )
{
	const friendlogic::FRIENDMAP& mapFriend = m_pGaeaClient->GetFriendClient()->GetList();
	friendlogic::FRIENDMAP_CITER iter = mapFriend.find ( strName.GetString () );	
	if ( iter==mapFriend.end() )
        return strName;

	std::tr1::shared_ptr<SFRIEND> sFriend = iter->second;	
	if ( sFriend->m_Online )
        return strName;

	const int nSize = (int)m_NameList.size ();

	++iter;
	++m_NamePos;
	if ( nSize <= m_NamePos ) m_NamePos = nSize - 1;
	while ( iter!=mapFriend.end() )
	{		
		std::tr1::shared_ptr<SFRIEND> sFriend = iter->second;
		if ( sFriend->m_Online )
            return sFriend->m_ChaName.c_str();
		++iter;
		++m_NamePos;
		if ( nSize <= m_NamePos )
            m_NamePos = nSize - 1;
	}

	return strName;
}

void CBasicChatRightBody::ChangeLastChat ()
{
	if (!IsEDIT_BEGIN())
        return;

	BOOL bKEYDOWN = FALSE;
	//	채팅이 갱신되었으면, 채팅 포지션을 -1로 맞춘다.
	int nSize = (int) m_ChatLog.size();
	if (nSize != m_ChatCountBack)
		m_ChatPos = OUT_OF_RANGE;
	m_ChatCountBack = nSize;

	if (!nSize)
        return;

	// 문자 조합중이 아닐 경우에만 귓속말 상대 선택이 가능.
    DXInputString* pInputString = m_pEngineDevice->GetInputString();
	if (pInputString->CheckComposition())
        return;

	if ( UIKeyCheck::GetInstance()->Check ( DIK_UP, DXKEY_DOWN) )
	{							
		if ( m_ChatPos < (nSize-1) )
		{
			m_ChatPos++;
			bKEYDOWN = TRUE;
		}
	}
	else if ( UIKeyCheck::GetInstance()->Check ( DIK_DOWN, DXKEY_DOWN ) )
	{
		if ( OUT_OF_RANGE < m_ChatPos )
		{
			m_ChatPos--;			
			bKEYDOWN = TRUE;
		}
	}

	if ( bKEYDOWN )
	{
		if ( m_ChatPos == OUT_OF_RANGE )
		{
			m_pEditBox->ClearEdit ();
			return ;
		}
		else
		{
			NORMAL_CHAT_LOG_ITER iter = m_ChatLog.begin ();
			NORMAL_CHAT_LOG_ITER iter_end = m_ChatLog.end ();
			for ( int i = 0; i < m_ChatPos && iter != iter_end; ++i, ++iter ) ;
			
			CString strChat = (*iter);
			m_pEditBox->SetEditString ( strChat );
		}
	}
}

void	CBasicChatRightBody::ChangeChatType ()
{
	if ( !IsEDIT_BEGIN () ) return ;
	if ( UIKeyCheck::GetInstance()->Check ( DIK_ESCAPE, DXKEY_DOWN ) )
	{
		EDIT_END ();
	}

	if ( UIKeyCheck::GetInstance()->Check ( DIK_TAB, DXKEY_DOWN ) )
	{
		const int nCHATTYPE_BACK = GetCHATTYPE();
		EDIT_END ();


		
		switch ( nCHATTYPE_BACK )
		{
		case CHAT_CONST::CHAT_NORMAL:		SetCHATTYPE( CHAT_CONST::CHAT_PRIVATE );		break;
		case CHAT_CONST::CHAT_PRIVATE:		SetCHATTYPE( CHAT_CONST::CHAT_PARTY );			break;
		case CHAT_CONST::CHAT_PARTY:		SetCHATTYPE( CHAT_CONST::CHAT_GUILD );			break;
		case CHAT_CONST::CHAT_GUILD:		SetCHATTYPE( CHAT_CONST::CHAT_ALLIANCE );		break;
		case CHAT_CONST::CHAT_ALLIANCE:	
			{
				if ( GLCONST_CHAR::bCHAT_EXTEND ) SetCHATTYPE( CHAT_CONST::CHAT_AREA );
				else SetCHATTYPE( CHAT_CONST::CHAT_TOALL );
			}
			break;
		case CHAT_CONST::CHAT_AREA:
			{
				if ( GLCONST_CHAR::bCHAT_EXTEND ) SetCHATTYPE( CHAT_CONST::CHAT_PARTY_RECRUIT );
				else SetCHATTYPE( CHAT_CONST::CHAT_TOALL );				
			}
			break;
		case CHAT_CONST::CHAT_PARTY_RECRUIT:SetCHATTYPE( CHAT_CONST::CHAT_FACTION );		break;
		case CHAT_CONST::CHAT_FACTION:		SetCHATTYPE( CHAT_CONST::CHAT_TOALL );			break;
		case CHAT_CONST::CHAT_TOALL:		SetCHATTYPE( CHAT_CONST::CHAT_NORMAL );			break;
		case CHAT_CONST::CHAT_SYSTEM:		SetCHATTYPE( CHAT_CONST::CHAT_NORMAL );			break;
		}

		EDIT_BEGIN();
	}	
}

void	CBasicChatRightBody::CLASSIFY_CHATTYPE ()
{	

	const int nEditLength = m_pEditBox->GetEditLength ();
	if( nEditLength )
	{
		CString strOrigin = m_pEditBox->GetEditString();
		strOrigin.TrimLeft( CHAT_CONST::CHAT_SIMBOL_BLANK );	//	좌측 공백 무시
		if( !strOrigin.GetLength() )	return;

		STRUTIL::ClearSeparator ();
		STRUTIL::RegisterSeparator ( " " );

		CStringArray strArray ;
		STRUTIL::StringSeparate ( strOrigin, strArray );
		if ( strArray.GetSize()==0 )	return;

		CString strCOMMAND = strArray.GetAt(0);
		
		//	마지막 공백 확인
		if ( strOrigin.GetAt( strOrigin.GetLength()-1 ) != CHAT_CONST::CHAT_SIMBOL_BLANK ) 
			return;

		DWORD chatType = CHATTYPE_SIMBOL_CHECK( strCOMMAND );

		if ( chatType != CHAT_CONST::CHAT_NO )
		{
			if ( chatType == CHAT_CONST::CHAT_PRIVATE )
			{
				if ( strArray.GetSize() != 2 ) return;

				m_strPrivateName = strArray.GetAt(1);

				//	귓속말 캐릭터 설정
			}

			EDIT_CLEAR();
			SetCHATTYPE( chatType );
		}
		else
		{
			if( strCOMMAND == CHAT_CONST::CHAT_TYPE_SYMBOL[CHAT_CONST::CHAT_SIMBOL_REPLAY] ||
				strCOMMAND == CHAT_CONST::CHAT_TYPE_SYMBOL_EX[CHAT_CONST::CHAT_SIMBOL_REPLAY] || 
				strCOMMAND == ID2GAMEWORD( "CHAT_TYPE_SYMBOL_LOCAL", CHAT_CONST::CHAT_SIMBOL_REPLAY ) )
			{

				m_strPrivateName = m_strRePrivateName;						
				EDIT_CLEAR();
				SetCHATTYPE( CHAT_CONST::CHAT_PRIVATE );
			}
			else if ( strCOMMAND == ID2GAMEWORD( "CHAT_LINK_PARTY_RECRUIT", 0 ) )
			{
				SLINK_DATA_BASIC sLinkDataBasic;
				sLinkDataBasic.sLinkType.wMType = PARTY_LINK;
                sLinkDataBasic.sLinkType.wSType = PARTY_LINK_RECRUIT;
                sLinkDataBasic.dwData1          = m_pGaeaClient->GetActiveMapID().getBaseMapID().Id();

                EDIT_CLEAR();
				m_pInterface->ChatToLink(sLinkDataBasic);
			}
		}
	}	
}

void	CBasicChatRightBody::UPDATE_SHORTKEY ()
{
	ChangeChatType ();

	switch ( m_dwChatType )
	{
	case CHAT_CONST::CHAT_NORMAL:
		{
			ChangeLastChat ();
		}
		break;

	case CHAT_CONST::CHAT_PRIVATE:
		{
			ChangePrivateName ();
		}
		break;

	case CHAT_CONST::CHAT_PARTY:
		{
		}
		break;

	case CHAT_CONST::CHAT_GUILD:
		{
		}
		break;

	case CHAT_CONST::CHAT_ALLIANCE:
		{
		}
		break;

	default:
		{
		}
		break;
	}
}

void CBasicChatRightBody::SELECT_CHAT_PAGE ()
{
	m_pTextBox_ALL->SetVisibleSingle ( FALSE );
	m_pScrollBar_ALL->SetVisibleSingle ( FALSE );

	m_pTextBox_PARTY->SetVisibleSingle ( FALSE );
	m_pScrollBar_PARTY->SetVisibleSingle ( FALSE );

	m_pTextBox_CLUB->SetVisibleSingle ( FALSE );
	m_pScrollBar_CLUB->SetVisibleSingle ( FALSE );

	m_pTextBox_ALLIANCE->SetVisibleSingle ( FALSE );
	m_pScrollBar_ALLIANCE->SetVisibleSingle ( FALSE );

	m_pTextBox_SYSTEM->SetVisibleSingle ( FALSE );
	m_pScrollBar_SYSTEM->SetVisibleSingle ( FALSE );
	
	m_pTextBox_CHANNEL->SetVisibleSingle ( FALSE );
	m_pScrollBar_CHANNEL->SetVisibleSingle ( FALSE );

	m_pChatTotalButton->SetFlip ( FALSE );
	m_pChatPartyButton->SetFlip ( FALSE );
	m_pChatClubButton->SetFlip ( FALSE );
	m_pChatAllianceButton->SetFlip ( FALSE );
	m_pChatSystemButton->SetFlip ( FALSE );
	m_pChatChannelButton->SetFlip ( FALSE );

	switch ( m_wDISPLAYTYPE )
	{
	case CHAT_CHANNEL_TOTAL:
		{
			m_pChatTotalButton->SetFlip ( TRUE );			

			if ( !m_bMINIMUM_SIZE_TEXT ) m_pTextBox_ALL->SetVisibleSingle ( TRUE );
			if ( !m_bMINIMUM_SIZE_SCROLL )
			{
				int nTotal = m_pTextBox_ALL->GetTotalLine();
				int nVisible = m_pTextBox_ALL->GetVisibleLine();

				if( nTotal > nVisible ) m_pScrollBar_ALL->SetVisibleSingle ( TRUE );
			}
		}
		break;

	case CHAT_CHANNEL_CHANNEL:
		{
			m_pChatChannelButton->SetFlip ( TRUE );

			if ( !m_bMINIMUM_SIZE_TEXT ) m_pTextBox_CHANNEL->SetVisibleSingle ( TRUE );
			if ( !m_bMINIMUM_SIZE_SCROLL )
			{
				int nTotal = m_pTextBox_CHANNEL->GetTotalLine();
				int nVisible = m_pTextBox_CHANNEL->GetVisibleLine();

				if( nTotal > nVisible ) m_pScrollBar_CHANNEL->SetVisibleSingle ( TRUE );
			}
		}
		break;

	case CHAT_CHANNEL_PARTY:
		{
			m_pChatPartyButton->SetFlip ( TRUE );

			if ( !m_bMINIMUM_SIZE_TEXT ) m_pTextBox_PARTY->SetVisibleSingle ( TRUE );
			if ( !m_bMINIMUM_SIZE_SCROLL )
			{
				int nTotal = m_pTextBox_PARTY->GetTotalLine();
				int nVisible = m_pTextBox_PARTY->GetVisibleLine();

				if( nTotal > nVisible ) m_pScrollBar_PARTY->SetVisibleSingle ( TRUE );
			}
		}
		break;

	case CHAT_CHANNEL_CLUB:
		{
			m_pChatClubButton->SetFlip ( TRUE );

			if ( !m_bMINIMUM_SIZE_TEXT ) m_pTextBox_CLUB->SetVisibleSingle ( TRUE );
			if ( !m_bMINIMUM_SIZE_SCROLL )
			{
				int nTotal = m_pTextBox_CLUB->GetTotalLine();
				int nVisible = m_pTextBox_CLUB->GetVisibleLine();

				if( nTotal > nVisible ) m_pScrollBar_CLUB->SetVisibleSingle ( TRUE );
			}
		}
		break;

	case CHAT_CHANNEL_ALLIANCE:
		{
			m_pChatAllianceButton->SetFlip ( TRUE );

			if ( !m_bMINIMUM_SIZE_TEXT ) m_pTextBox_ALLIANCE->SetVisibleSingle ( TRUE );
			if ( !m_bMINIMUM_SIZE_SCROLL )
			{
				int nTotal = m_pTextBox_ALLIANCE->GetTotalLine();
				int nVisible = m_pTextBox_ALLIANCE->GetVisibleLine();

				if( nTotal > nVisible ) m_pScrollBar_ALLIANCE->SetVisibleSingle ( TRUE );
			}
		}
		break;

	case CHAT_CHANNEL_SYSTEM:
		{
			m_pChatSystemButton->SetFlip ( TRUE );

			if ( !m_bMINIMUM_SIZE_TEXT ) m_pTextBox_SYSTEM->SetVisibleSingle ( TRUE );
			if ( !m_bMINIMUM_SIZE_SCROLL )
			{
				int nTotal = m_pTextBox_SYSTEM->GetTotalLine();
				int nVisible = m_pTextBox_SYSTEM->GetVisibleLine();

				if( nTotal > nVisible ) m_pScrollBar_SYSTEM->SetVisibleSingle ( TRUE );
			}


		}
		break;
	}
}

CBasicTextButton*	 CBasicChatRightBody::CreateTextButton ( char* szButton, UIGUID ControlID, char* szText )
{
	CBasicTextButton* pTextButton = new CBasicTextButton(m_pEngineDevice);
	pTextButton->CreateSub ( this, "BASIC_TEXT_BUTTON191", UI_FLAG_XSIZE, ControlID );
	pTextButton->CreateBaseButton ( szButton, 
									CBasicTextButton::SIZE19_RECT, 
									CBasicButton::RADIO_FLIP, 
									szText, 
									_DEFAULT_FONT_SHADOW_EX_FLAG );
	pTextButton->SetAlignFlag ( UI_FLAG_BOTTOM );
	pTextButton->SetFlip ( TRUE );
	RegisterControl ( pTextButton );

	return pTextButton;
}

void CBasicChatRightBody::AddChat ( const CString& strName, const CString& strMsg, const WORD& wType )
{
	AddStringToChatEx ( strMsg,wType );

	switch ( wType )
	{
	case CHAT_CONST::CHAT_PRIVATE:
		{
			m_strRePrivateName = strName;
			ADD_FRIEND ( strName );
			break;
		}
		break;
	}
}

void CBasicChatRightBody::AddStringToNORMALEx ( CString strTemp, D3DCOLOR dwColor, bool bGM )
{
	AddStringToNORMAL( strTemp, dwColor, bGM );
}

void CBasicChatRightBody::SET_PRIVATE_NAME ( const CString& strName )
{
	m_strRePrivateName = strName;
	ADD_FRIEND ( strName );
}

void CBasicChatRightBody::AddChatMacro(CString strMsg)
{
	strMsg.TrimLeft ( CHAT_CONST::CHAT_SIMBOL_BLANK );	//	좌측 공백 무시
	if( !strMsg.GetLength() )	return;

	DWORD dwType;
	CString ChatType( strMsg );

	STRUTIL::ClearSeparator ();
	STRUTIL::RegisterSeparator ( " " );

	CStringArray strArray ;
	STRUTIL::StringSeparate ( ChatType, strArray );
	if ( strArray.GetSize()==0 )	return;
	
	CString strCOMMAND = strArray.GetAt(0);

	dwType = CHATTYPE_SIMBOL_CHECK( strCOMMAND );
	if ( dwType != CHAT_CONST::CHAT_NO ) 
	{
		SetCHATTYPE ( dwType );

		strMsg.TrimLeft( strCOMMAND + CHAT_CONST::CHAT_SIMBOL_BLANK );
		SEND_CHAT_MESSAGE(strMsg);
	}
	else
	{
		SEND_CHAT_MESSAGE(strMsg);
	}
}

void CBasicChatRightBody::SetVisibleChatButton( BOOL bVisible )
{
	m_pChatTotalButton->SetVisibleSingle( bVisible );
	m_pChatPartyButton->SetVisibleSingle( bVisible );
	m_pChatClubButton->SetVisibleSingle( bVisible );
	m_pChatSystemButton->SetVisibleSingle( bVisible );
	m_pChatAllianceButton->SetVisibleSingle( bVisible );
	m_pChatChannelButton->SetVisibleSingle( bVisible );	
}

void CBasicChatRightBody::SetCHATTYPE( DWORD dwChatType )
{
	m_dwChatType = dwChatType;


	CString strText;
	CString strTemp;
	DWORD dwColor = CHAT_INFOR_COLOR::CHAT_NORMAL;

	switch ( m_dwChatType ) 
	{
	case CHAT_CONST::CHAT_NORMAL: 
		strText.Format( "[%s]", ID2GAMEWORD("CHAT_TYPE", 0) );
		dwColor	= CHAT_INFOR_COLOR::CHAT_NORMAL;
		break;
	case CHAT_CONST::CHAT_PRIVATE: 
		strTemp = sc::string::format( ID2GAMEWORD("CHAT_TYPE", 1), m_strPrivateName ).c_str();
//		strTemp = m_strPrivateName;
//		strTemp.Append(ID2GAMEWORD("CHAT_TYPE", 1));
		strText.Format( "[%s]", strTemp );
		dwColor	= CHAT_INFOR_COLOR::CHAT_PRIVATE;
		break;
	case CHAT_CONST::CHAT_PARTY: 
		strText.Format( "[%s]", ID2GAMEWORD("CHAT_TYPE", 2) );
		dwColor	= CHAT_INFOR_COLOR::CHAT_PARTY;
		break;
	case CHAT_CONST::CHAT_GUILD: 
		strText.Format( "[%s]", ID2GAMEWORD("CHAT_TYPE", 3) );
		dwColor	= CHAT_INFOR_COLOR::CHAT_GUILD;
		break;
	case CHAT_CONST::CHAT_ALLIANCE: 
		strText.Format( "[%s]", ID2GAMEWORD("CHAT_TYPE", 4) );
		dwColor	= CHAT_INFOR_COLOR::CHAT_ALLIANCE;
		break;
	case CHAT_CONST::CHAT_AREA: 
		strText.Format( "[%s]", ID2GAMEWORD("CHAT_TYPE", 5) );
		dwColor	= CHAT_INFOR_COLOR::CHAT_AREA;
		break;
	case CHAT_CONST::CHAT_PARTY_RECRUIT: 
		strText.Format( "[%s]", ID2GAMEWORD("CHAT_TYPE", 6) );
		dwColor	= CHAT_INFOR_COLOR::CHAT_PARTY_RECRUIT;
		break;
	case CHAT_CONST::CHAT_TOALL: 
		strText.Format( "[%s]", ID2GAMEWORD("CHAT_TYPE", 7) );
		dwColor	= CHAT_INFOR_COLOR::CHAT_TOALL;
		break;
	case CHAT_CONST::CHAT_SYSTEM: 
		strText.Format( "[%s]", ID2GAMEWORD("CHAT_TYPE", 8) );
		dwColor	= CHAT_INFOR_COLOR::CHAT_SYSTEM;
		break;
	case CHAT_CONST::CHAT_FACTION:
		strText.Format( "[%s]", ID2GAMEWORD("CHAT_TYPE", 10) );
		dwColor = CHAT_INFOR_COLOR::CHAT_FACTION;
	}

	//	에디트 박스와 사이즈를 변경시킨다.
	CD3DFontPar* pFont = m_pChatTypeBox->GetFont();
	SIZE nTextSize;
	pFont->GetTextExtent( strText, nTextSize );


	const UIRECT& rcGlobalPos = m_pChatTypeBox->GetGlobalPos ();
	const UIRECT rcGlobalNewPos ( rcGlobalPos.left, rcGlobalPos.top, static_cast<float>(nTextSize.cx), rcGlobalPos.sizeY );
	const UIRECT& rcLocalPos = m_pChatTypeBox->GetLocalPos ();
	const UIRECT rcLocalNewPos ( rcLocalPos.left, rcLocalPos.top, static_cast<float>(nTextSize.cx), rcLocalPos.sizeY );


	m_pChatTypeBox->AlignSubControl( rcGlobalPos, rcGlobalNewPos );
	m_pChatTypeBox->SetGlobalPos( rcGlobalNewPos ); 
	m_pChatTypeBox->SetLocalPos( rcLocalNewPos ); 

	const UIRECT& rcGlobalPos2 = m_pEditBox->GetGlobalPos ();
	const UIRECT rcGlobalNewPos2( rcGlobalNewPos.right+2, rcGlobalPos2.top, rcGlobalPos2.right-(rcGlobalNewPos.right+2), rcGlobalPos2.sizeY );

	const UIRECT& rcLocalPos2 = m_pEditBox->GetLocalPos ();
	const UIRECT rcLocalNewPos2 ( rcLocalNewPos.right+2, rcLocalPos2.top, rcLocalPos2.right-(rcLocalNewPos.right+2), rcLocalPos2.sizeY );
	
	m_pEditBox->AlignSubControl( rcGlobalPos2, rcGlobalNewPos2 ) ;
	m_pEditBox->SetGlobalPos( rcGlobalNewPos2 ); 
	m_pEditBox->SetLocalPos( rcLocalNewPos2 );

	m_pChatTypeBox->SetText( strText, dwColor );
	m_pEditBox->SetTextColor( dwColor );

}

DWORD CBasicChatRightBody::CHATTYPE_SIMBOL_CHECK( CString strOrigin )
{
	strOrigin = _strlwr( strOrigin.GetBuffer() );

	if( strOrigin == CHAT_CONST::CHAT_TYPE_SYMBOL[CHAT_CONST::CHAT_SIMBOL_NORMAL] ||
		strOrigin == CHAT_CONST::CHAT_TYPE_SYMBOL_EX[CHAT_CONST::CHAT_SIMBOL_NORMAL] ||
		strOrigin == ID2GAMEWORD( "CHAT_TYPE_SYMBOL_LOCAL", CHAT_CONST::CHAT_SIMBOL_NORMAL ) )		
	{	
		return CHAT_CONST::CHAT_NORMAL;
	}
	else if( strOrigin == CHAT_CONST::CHAT_TYPE_SYMBOL[CHAT_CONST::CHAT_SIMBOL_PRIVATE] ||
		strOrigin == CHAT_CONST::CHAT_TYPE_SYMBOL_EX[CHAT_CONST::CHAT_SIMBOL_PRIVATE] ||
		strOrigin == CHAT_CONST::CHAT_TYPE_SYMBOL_OLD[CHAT_CONST::CHAT_SIMBOL_PRIVATE] ||
		strOrigin == ID2GAMEWORD( "CHAT_TYPE_SYMBOL_LOCAL", CHAT_CONST::CHAT_SIMBOL_PRIVATE ) )		
	{
		return CHAT_CONST::CHAT_PRIVATE;		
	}
	else if( strOrigin == CHAT_CONST::CHAT_TYPE_SYMBOL[CHAT_CONST::CHAT_SIMBOL_PARTY] ||
		strOrigin == CHAT_CONST::CHAT_TYPE_SYMBOL_EX[CHAT_CONST::CHAT_SIMBOL_PARTY] ||
		strOrigin == CHAT_CONST::CHAT_TYPE_SYMBOL_OLD[CHAT_CONST::CHAT_SIMBOL_PARTY] ||
		strOrigin == ID2GAMEWORD( "CHAT_TYPE_SYMBOL_LOCAL", CHAT_CONST::CHAT_SIMBOL_PARTY ) )		
	{
		return CHAT_CONST::CHAT_PARTY;		
	}
	else if( strOrigin == CHAT_CONST::CHAT_TYPE_SYMBOL[CHAT_CONST::CHAT_SIMBOL_GUILD] ||
		strOrigin == CHAT_CONST::CHAT_TYPE_SYMBOL_EX[CHAT_CONST::CHAT_SIMBOL_GUILD] ||
		strOrigin == CHAT_CONST::CHAT_TYPE_SYMBOL_OLD[CHAT_CONST::CHAT_SIMBOL_GUILD] ||
		strOrigin == ID2GAMEWORD( "CHAT_TYPE_SYMBOL_LOCAL", CHAT_CONST::CHAT_SIMBOL_GUILD ) )		
	{
		return CHAT_CONST::CHAT_GUILD;		
	}		
	else if( strOrigin == CHAT_CONST::CHAT_TYPE_SYMBOL[CHAT_CONST::CHAT_SIMBOL_ALLIANCE] ||
		strOrigin == CHAT_CONST::CHAT_TYPE_SYMBOL_EX[CHAT_CONST::CHAT_SIMBOL_ALLIANCE] ||
		strOrigin == CHAT_CONST::CHAT_TYPE_SYMBOL_OLD[CHAT_CONST::CHAT_SIMBOL_ALLIANCE] ||
		strOrigin == ID2GAMEWORD( "CHAT_TYPE_SYMBOL_LOCAL", CHAT_CONST::CHAT_SIMBOL_ALLIANCE ) )		
	{
		return CHAT_CONST::CHAT_ALLIANCE;		
	}
	else if( strOrigin == CHAT_CONST::CHAT_TYPE_SYMBOL[CHAT_CONST::CHAT_SIMBOL_AREA] ||
		strOrigin == CHAT_CONST::CHAT_TYPE_SYMBOL_EX[CHAT_CONST::CHAT_SIMBOL_AREA] )
	{
		if ( GLCONST_CHAR::bCHAT_EXTEND ) return CHAT_CONST::CHAT_AREA;		
		else 	return CHAT_CONST::CHAT_NO;
	}
	else if( strOrigin == CHAT_CONST::CHAT_TYPE_SYMBOL[CHAT_CONST::CHAT_SIMBOL_PARTY_RECRUIT] ||
		strOrigin == CHAT_CONST::CHAT_TYPE_SYMBOL_EX[CHAT_CONST::CHAT_SIMBOL_PARTY_RECRUIT] )
	{
		if ( GLCONST_CHAR::bCHAT_EXTEND ) return CHAT_CONST::CHAT_PARTY_RECRUIT;
		else 	return CHAT_CONST::CHAT_NO;		
	}
	else if( strOrigin == CHAT_CONST::CHAT_TYPE_SYMBOL[CHAT_CONST::CHAT_SIMBOL_TOALL] ||
		strOrigin == CHAT_CONST::CHAT_TYPE_SYMBOL_EX[CHAT_CONST::CHAT_SIMBOL_TOALL] ||
		strOrigin == CHAT_CONST::CHAT_TYPE_SYMBOL_OLD[CHAT_CONST::CHAT_SIMBOL_TOALL] ||
		strOrigin == ID2GAMEWORD( "CHAT_TYPE_SYMBOL_LOCAL", CHAT_CONST::CHAT_SIMBOL_TOALL ) )		
	{
		return CHAT_CONST::CHAT_TOALL;
	}	
	else if( strOrigin == CHAT_CONST::CHAT_TYPE_SYMBOL[CHAT_CONST::CHAT_SIMBOL_SYSTEM] ||
		strOrigin == CHAT_CONST::CHAT_TYPE_SYMBOL_EX[CHAT_CONST::CHAT_SIMBOL_SYSTEM] ||
		strOrigin == CHAT_CONST::CHAT_TYPE_SYMBOL_OLD[CHAT_CONST::CHAT_SIMBOL_SYSTEM] ||
		strOrigin == ID2GAMEWORD( "CHAT_TYPE_SYMBOL_LOCAL", CHAT_CONST::CHAT_SIMBOL_SYSTEM ) )		
	{
		// 시스템 타입은 GM만 사용할수 있다.
		DWORD dwUSERLVL = m_pGaeaClient->GetCharacter()->UserLevel();
		if ( dwUSERLVL < USER_USER_GM )
            return CHAT_CONST::CHAT_NO;
		
		return CHAT_CONST::CHAT_SYSTEM;		
	}
	else if( strOrigin == CHAT_CONST::CHAT_TYPE_SYMBOL[CHAT_CONST::CHAT_SIMBOL_FACTION] ||
		strOrigin == CHAT_CONST::CHAT_TYPE_SYMBOL_EX[CHAT_CONST::CHAT_SIMBOL_FACTION] ||
		strOrigin == ID2GAMEWORD( "CHAT_TYPE_SYMBOL_LOCAL", CHAT_CONST::CHAT_SIMBOL_FACTION ) )		
	{	
		return CHAT_CONST::CHAT_FACTION;
	}

	return CHAT_CONST::CHAT_NO;
}

void CBasicChatRightBody::ChatToLink( const SLINK_DATA_BASIC& sLinkDataBasic, const CString& strLinkName, DWORD dwColor )
{
	if ( m_dwChatType == CHAT_CONST::CHAT_SYSTEM )
		return;

	if ( m_pEditBox->IsBegin() )
	{
		m_pEditBox->ChatToLink ( sLinkDataBasic, strLinkName, dwColor );
	}
	else
	{
		m_pEditBox->BeginEdit();
		m_pEditBox->ChatToLink ( sLinkDataBasic, strLinkName, dwColor );
	}
}

void CBasicChatRightBody::SendReplyAwayState( const std::string& strOrigin )
{
    SEND_PRIVATE_MESSAGE( strOrigin.c_str(), VEC_LINK_TEXT_DATA_EX(), false );
    EDIT_CLEAR();
    EDIT_END();
}
