#include "StdAfx.h"
#include "RnBlockTab.h"

#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicLineBoxSmart.h"
#include "../../../enginelib/GUInterface/UIEditBoxMan.h"
#include "../../../enginelib/GUInterface/BasicScrollBarEx.h"
#include "../../../enginelib/GUInterface/BasicScrollThumbFrame.h"

#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/DxTools/d3dfont.h"

#include "../../../RanLogic/Friend/FriendDefine.h"

#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Friend/GLFriendClient.h"


// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace
{
    const char* strAddBlock = "차단추가";
    const char* strDelBlock = "차단해제";
    const char* strListCount = "등록인원";
}

RnBlockTab::CItem::CItem( EngineDeviceMan* pEngineDevice )
    : CUIGroupHelper( pEngineDevice )
    , m_pText_CharName( NULL )
{
    for( int i=0 ; i<GLCI_NUM_TRICKER ; i++)
    {
        m_pClassIcon[i] = NULL;
    }
}

void RnBlockTab::CItem::CreateSubControl()
{
    CD3DFontPar* pFont9 = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_SHADOW_FLAG );

    CreateLineBox( "RNFRIEND_FRIEND_ROW_SIZE", "RNFRIEND_TEXTURE_NOLINE_GRAY_BG" );

    {
        // 캐릭터 직업 아이콘
        CString strClassOff[GLCI_NUM_TRICKER] =
        {
            "gyouk_msg_lgc",
            "gum_msg_lgc",
            "yang_wsg_lgc",
            "gi_wsg_lgc",
            "guk_msg_lgc",
            "guk_wsg_lgc",
            "gyouk_wsg_lgc",
            "gum_msg_lgc",
            "yang_wsg_lgc",
            "gi_msg_lgc",
            "gua_msg_lgc",
            "gua_wsg_lgc",
            "nin_msg_lgc",
            "nin_wsg_lgc",
			"tri_msg_lgc",
			"tri_wsg_lgc",
        };

        for( int i=0 ; i<GLCI_NUM_TRICKER ; i++)
        {
            CBasicLineBoxSmart* m_pIcon = new CBasicLineBoxSmart( GetEngineDeviceMan() );
            m_pIcon->CreateSub( this, "RNFRIEND_ITEM_CLASS", UI_FLAG_DEFAULT );
            m_pIcon->CreateSubControl( strClassOff[i].GetString() );
            m_pIcon->SetVisibleSingle( FALSE );
            RegisterControl(m_pIcon);

            m_pClassIcon[i] = m_pIcon;
        }
    }
    {
        // 캐릭터 명
        m_pText_CharName = new CBasicTextBox(m_pEngineDevice);
        m_pText_CharName->CreateSub ( this, "RNFRIEND_ITEM_BLOK_CHARNAME", UI_FLAG_DEFAULT );
        m_pText_CharName->SetFont ( pFont9 );
        m_pText_CharName->SetTextAlign ( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );	
        RegisterControl ( m_pText_CharName );
    }

}

void RnBlockTab::CItem::SetData( const SFRIEND* pFriend )
{
    Reset();
    if( pFriend == NULL )
        return;

    //m_pIcon_Class->SetVisibleSingle( TRUE );
    int nIConIndex = GetGLCCToGLCI( (EMCHARCLASS)pFriend->m_ChaClass );
    m_pClassIcon[nIConIndex]->SetVisibleSingle( TRUE );

    m_pText_CharName->SetText( pFriend->m_ChaName.c_str(), NS_UITEXTCOLOR::DARKGRAY );
}

void RnBlockTab::CItem::Reset()
{
    for( int i=0 ; i<GLCI_NUM_TRICKER ; i++)
    {
        m_pClassIcon[i]->SetVisibleSingle( FALSE );
    }
    m_pText_CharName->ClearText();
}

EMCHARINDEX RnBlockTab::CItem::GetGLCCToGLCI( EMCHARCLASS eGLCC )
{
    unsigned int eGLCI = 0;
    switch( eGLCC )
    {
    case GLCC_ETC_W:
        eGLCI++;
    case GLCC_ETC_M:
        eGLCI++;
	case GLCC_TRICKER_W:
		eGLCI++;
	case GLCC_TRICKER_M:
		eGLCI++;
    case GLCC_ASSASSIN_W:
        eGLCI++;
    case GLCC_ASSASSIN_M:
        eGLCI++;
    case GLCC_SCIENTIST_W:
        eGLCI++;
    case GLCC_SCIENTIST_M:
        eGLCI++;
    case GLCC_SPIRIT_M:
        eGLCI++;
    case GLCC_ARCHER_M:
        eGLCI++;
    case GLCC_ARMS_W:
        eGLCI++;
    case GLCC_FIGHTER_W:
        eGLCI++;
    case GLCC_EXTREME_W:
        eGLCI++;
    case GLCC_EXTREME_M:
        eGLCI++;
    case GLCC_SPIRIT_W:
        eGLCI++;
    case GLCC_ARCHER_W:
        eGLCI++;
    case GLCC_ARMS_M:
        eGLCI++;
    case GLCC_FIGHTER_M:
        break;
    default:
        return GLCI_FIGHTER_M;
    }

    return (EMCHARINDEX)eGLCI;
}


RnBlockTab::RnBlockTab( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
    : CUIGroupHelperMessageBox( pEngineDevice )
    , m_pGaeaClient( pGaeaClient )
    , m_pInterface( pInterface )
    , m_pText_ListCount( NULL )
    , m_pEditBox( NULL )
    , m_pScrollBar( NULL )
    , m_nViewPos( 0 )
    , m_pYLine( NULL )
    , m_nSelectedPos( -1 )
{
    for( int nRow = 0 ; nRow < LIST_Y ; nRow++ )
    {
        m_pSlot[nRow] = NULL;
    }
    strAddBlock     = ID2GAMEWORD( "RNFRIEND_BLOCKTAB", 0);
    strDelBlock     = ID2GAMEWORD( "RNFRIEND_BLOCKTAB", 1);

    strListCount    = ID2GAMEWORD( "RNFRIEND_FRIENDTAB", 0);

}

void RnBlockTab::CreateSubControl()
{
    CD3DFontPar* pFont9 = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );
    CD3DFontPar* pFont10 = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 10, _DEFAULT_FONT_SHADOW_FLAG );

    CreateLineBox( "RNFRIEND_BLOCK_TAB", "GRAY_BACKGROUND_BLACK_LINE_TEXTURE" );
    CreateLineBox( "RNFRIEND_BLOCK_ROW_TOP", "GRAY_BACKGROUND_BLACK_LINE_TEXTURE" )->SetDiffuseAlpha(90);

    // 등록인원
    CBasicTextBox* pText_ListCount = new CBasicTextBox(m_pEngineDevice);
    pText_ListCount->CreateSub ( this, "RNFRIEND_FRIEND_LISTCOUNT", UI_FLAG_DEFAULT );
    pText_ListCount->SetFont ( pFont10 );
    pText_ListCount->SetTextAlign ( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );	
    RegisterControl ( pText_ListCount );
    m_pText_ListCount = pText_ListCount;


    //순서있음. UIRnFriendWindow.xml
    const char* strListRow[LIST_Y] = 
    {
        "RNFRIEND_FRIEND_ROW_0",
        "RNFRIEND_FRIEND_ROW_1",
        "RNFRIEND_FRIEND_ROW_2",
        "RNFRIEND_FRIEND_ROW_3",
        "RNFRIEND_FRIEND_ROW_4",
        "RNFRIEND_FRIEND_ROW_5",
        "RNFRIEND_FRIEND_ROW_6",
        "RNFRIEND_FRIEND_ROW_7",
        "RNFRIEND_FRIEND_ROW_8",
        "RNFRIEND_FRIEND_ROW_9",
        "RNFRIEND_FRIEND_ROW_10",
    };

    for( int nIndex = 0; nIndex < LIST_Y ; nIndex++ )
    {
        m_pSlot[nIndex] = new CItem( m_pEngineDevice );
        m_pSlot[nIndex]->CreateSub( this, strListRow[nIndex], UI_FLAG_DEFAULT, ID_LIST_CONTROL_START + nIndex );
        m_pSlot[nIndex]->CreateSubControl();
        RegisterControl( m_pSlot[nIndex] );
    }

    CreateLineBox( "RNFRIEND_BLOCK_TEXTEDIT_POS_BG", "GRAY_BACKGROUND_BLACK_LINE_TEXTURE", ID_TEXTBOX_BACKGROUND );

    CUIEditBoxMan* pEditBoxMan = new CUIEditBoxMan(m_pEngineDevice);
    pEditBoxMan->CreateSub( this, "RNFRIEND_BLOCK_TEXTEDIT_SIZE", UI_FLAG_DEFAULT , ID_TEXTBOX_BOX );
    pEditBoxMan->CreateEditBox( ID_TEXTBOX_EDIT, "RNFRIEND_BLOCK_TEXTEDIT", "DEFAULT_CARRAT", TRUE, UINT_MAX, pFont9, 16 );
    RegisterControl( pEditBoxMan );
    m_pEditBox = pEditBoxMan;
//     

    RnButton::CreateArg arg;
    arg.defaultTextureKey = "RNBUTTON_DEFAULT_TEXTURE";
    arg.mouseOverTextureKey = "RNBUTTON_OVER_TEXTURE";
    arg.mouseClickTextureKey = "RNBUTTON_CLICK_TEXTURE";
    arg.pFont = pFont10;
    arg.dwColor = NS_UITEXTCOLOR::WHITE;

    RnButton* pRnButton = NULL;
    pRnButton = new RnButton( m_pEngineDevice );
    pRnButton->CreateSub(this,"RNFRIEND_BUTTON_BLOCK",UI_FLAG_DEFAULT, ID_BTN_BLACK );
    arg.text = strAddBlock;
    pRnButton->CreateSubControl(arg);
    RegisterControl(pRnButton);

    pRnButton = new RnButton( m_pEngineDevice );
    pRnButton->CreateSub(this,"RNFRIEND_BUTTON_DELETE",UI_FLAG_DEFAULT, ID_BTN_DEL );
    arg.text = strDelBlock;
    pRnButton->CreateSubControl(arg);
    RegisterControl(pRnButton);

    { // 스크롤바
        CBasicScrollBarEx* pScrollBar = new CBasicScrollBarEx(m_pEngineDevice);
        pScrollBar->CreateSub( this, "BASIC_SCROLLBAR", UI_FLAG_RIGHT | UI_FLAG_YSIZE );
        pScrollBar->CreateBaseScrollBar( "RNFRIEND_FRIEND_SCROLL" );
        pScrollBar->GetThumbFrame()->SetScrollParent( this );
        RegisterControl ( pScrollBar );
        m_pScrollBar = pScrollBar;
    }

    //테두리[노란선]
    m_pYLine = CreateLineBox( "RNFRIEND_FRIEND_ROW_SELECTEDBOX_SIZE", "LINEBOX_Y" );
    m_pYLine->SetVisibleSingle( FALSE );
    m_rcSeleted = m_pYLine->GetGlobalPos();


    CreateMessageBox();
}

void RnBlockTab::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    CUIGroupHelperMessageBox::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

    unsigned int nTotal = (int) m_vecFriendPtr.size();

    m_pScrollBar->GetThumbFrame()->SetState( nTotal, LIST_Y );
    int nVisibleLine = LIST_Y;


    CBasicScrollThumbFrame* const pThumbFrame = m_pScrollBar->GetThumbFrame();

    const int nMovableLine = nTotal - nVisibleLine;
    float fPercent = pThumbFrame->GetPercent();


    m_nViewPos = (int)floor( fPercent * nMovableLine );

    if( m_nViewPos < 0 )
        m_nViewPos = 0;


    for( unsigned int nRow=0 ; nRow < LIST_Y ; nRow++ )
    {
        if( m_nViewPos+nRow < m_vecFriendPtr.size() )
        {
            SFRIEND* pFriend = m_vecFriendPtr[m_nViewPos+nRow];
            m_pSlot[ nRow ]->SetData( pFriend );
        }
        else
        {
            m_pSlot[ nRow ]->SetData( NULL );
        }

    }

    SFRIEND* pFriend = FindFirend( m_nSelectedPos );
    if( pFriend == NULL )
        return;

    int nIndex = m_nSelectedPos - m_nViewPos;
    if( 0 <= nIndex && nIndex < LIST_Y )
    {
        UIRECT rcSelectBoxPos = m_pSlot[nIndex]->GetGlobalPos();
        float fLeftMargine = rcSelectBoxPos.sizeX - m_rcSeleted.sizeX;
        float fTopMargine = rcSelectBoxPos.sizeY - m_rcSeleted.sizeY;
        m_pYLine->SetGlobalPos( D3DXVECTOR2( rcSelectBoxPos.left + fLeftMargine/2, rcSelectBoxPos.top + fTopMargine/2 ) );
        m_pYLine->SetVisibleSingle( TRUE );
    }
    else
    {
        m_pYLine->SetVisibleSingle( FALSE );
    }
}

void RnBlockTab::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
    CUIGroupHelperMessageBox::TranslateUIMessage( ControlID, dwMsg );

    //커서 지우기
    if( ControlID == ID_TEXTBOX_BACKGROUND )
        if( !CHECK_MOUSE_IN( dwMsg ) && CHECK_LB_UP_LIKE( dwMsg ) )
            m_pEditBox->EndEdit();

    if( IsOpenMessageBox() == TRUE ) // 메시지 박스가 떠있다면 메시지를 무시한다.
        return;


    switch( ControlID )
    {
    case ID_BTN_BLACK:
        if( CHECK_MOUSE_IN( dwMsg ) && CHECK_LB_UP_LIKE( dwMsg ) )
        {
            const std::string str = GetEditBoxString();
            
            if( str.empty() == FALSE )
            {
                std::string strMsgBoxString = sc::string::format( ID2GAMEINTEXT("FRIEND_BLOCK_ADD"), str );
                OpenMessageBox( CMessageBox::TITLE_QUESTION, strMsgBoxString, CMessageBox::TYPE_YESNO, IDENTI_ADDBLOCK );
            }

            m_strInputData = str;
            m_pEditBox->ClearEdit( ID_TEXTBOX_EDIT );
        }
        break;
    case ID_BTN_DEL:
        if( CHECK_MOUSE_IN( dwMsg ) && CHECK_LB_UP_LIKE( dwMsg ) )
        {
            SFRIEND* pFriend = FindFirend( m_nSelectedPos );

            if( pFriend!= NULL )
            {
                std::string strMsgBoxString = sc::string::format( ID2GAMEINTEXT("RNFRIEND_FRIEND_DELBLOCK"), pFriend->m_ChaName );
                OpenMessageBox( CMessageBox::TITLE_QUESTION, strMsgBoxString, CMessageBox::TYPE_YESNO, IDENTI_DELBLOCK );
            }
        }
        break;
    case ID_TEXTBOX_BOX:
        if( CHECK_MOUSE_IN( dwMsg ) && CHECK_LB_UP_LIKE( dwMsg ) )
        {
            
        }
        break;
    }


    //////////////////////////////////////////////////////////////////////////
    // 내부 일괄 처리
    if( ID_LIST_CONTROL_START <= ControlID && ControlID <= ID_LIST_CONTROL_END )
    {
        if( CHECK_MOUSE_IN( dwMsg ) && CHECK_LB_UP_LIKE( dwMsg ) )
        {
            unsigned int nIndex = ControlID - ID_LIST_CONTROL;
            m_nSelectedPos = nIndex + m_nViewPos;
        }
    }
    
}

void RnBlockTab::TranslateMessageBox( unsigned int nIdentity, DWORD dwMsg )
{
    switch( nIdentity )
    {
    case IDENTI_ADDBLOCK:
        if( dwMsg & UIMSG_MESSAGEBOX_POSITIVE )
        {
            if( m_strInputData.empty() == FALSE )
                m_pInterface->ADD_FRIEND_BLOCK( m_strInputData.c_str(), true );
                //m_pGaeaClient->GetFriendClient()->ReqFriendBlock( m_strInputData.c_str(), true );

            m_strInputData.clear();
        }
        break;
    case IDENTI_DELBLOCK:
        if( dwMsg & UIMSG_MESSAGEBOX_POSITIVE )
        {
            SFRIEND* pFriend = FindFirend( m_nSelectedPos );
            if( pFriend != NULL )
			{
				m_pGaeaClient->GetFriendClient()->ReqFriendDel ( pFriend->m_ChaName.c_str() );
				m_nSelectedPos = -1;
				m_pYLine->SetVisibleSingle( FALSE );
			}
        }
        break;
    }
}

void RnBlockTab::Refresh()
{
    m_vecFriendPtr.clear();
    const friendlogic::FRIENDMAP& FriendMap = m_pGaeaClient->GetFriendClient()->GetList();

    BOOST_FOREACH( const friendlogic::FRIENDMAP_VALUE& it, FriendMap )
    {
        SFRIEND* sFriend = it.second.get();
        
        if( sFriend->IsBLOCK() == true )
        {
            m_vecFriendPtr.push_back( sFriend );
        }            
    }

    CString strListCount;
    strListCount.Format( "%s %d/%d", strListCount, m_vecFriendPtr.size(), GLCONST_CHAR::nMAX_FRIEND_BLOCK );
    
    m_pText_ListCount->SetText( strListCount );
}

SFRIEND* RnBlockTab::FindFirend( int nIndex )
{
    if( (unsigned)nIndex < m_vecFriendPtr.size() )
        return m_vecFriendPtr[nIndex];
 
    return NULL;
}

std::string RnBlockTab::GetEditBoxString()
{
    std::string str = m_pEditBox->GetEditString();
    if( str.empty() == true )
    {
        OpenMessageBox( CMessageBox::TITLE_WARNING, ID2GAMEINTEXT("RNFRIEND_FRIEND_INPUTEMPTY"), CMessageBox::TYPE_OK );
        return "";
    }

    sc::string::trimLeft( str );
    sc::string::trimRight( str );
    
    return str;
}
