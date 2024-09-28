#include "StdAfx.h"
#include "RnFriendTab.h"

#include "../../../SigmaCore/String/StringUtil.h"

#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicLineBoxSmart.h"
#include "../../../EngineLib/GUInterface/BasicTextBoxEx.h"
#include "../../../enginelib/GUInterface/BasicScrollBarEx.h"
#include "../../../enginelib/GUInterface/BasicScrollThumbFrame.h"
#include "../../../enginelib/GUInterface/SmartComboBoxRollOver.h"
#include "../../../EngineLib/GUInterface/SmartComboBox.h"
#include "../../../EngineLib/GUInterface/SmartComboBoxRollOver.h"
#include "../../../enginelib/GUInterface/UIEditBoxMan.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/DxTools/d3dfont.h"

#include "../../../RanLogic/Friend/FriendDefine.h"
#include "../../../RanLogic/Party/GLPartyDefine.h"

#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"
#include "../../../RanLogicClient/Friend/GLFriendClient.h"
#include "../../../RanLogicClient/Party/PartyCacheClient.h"

#include "../Message/WaitServerDialogue.h"

#include "../Util/MessageBox.h"
#include "../Util/PopupMenuComponent.h"
#include "../../../RanLogic/GLUseFeatures.h"

#include <boost/lambda/lambda.hpp>

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace
{
    const char* strListCount = "등록인원";
    const char* strGroupCount = "그룹";
    const char* strView = "보기";

    const char* strBtnAdd = "친구추가";
    const char* strBtnMakeGroup = "그룹생성";
    const char* strBtnAddParty = "파티초대";
    const char* strBtnWhisper = "귓속말";
    const char* strBtnBlock = "차단";
    const char* strBtnDelete = "삭제";

    const char* strComboTextAll = "전체";
    const char* strComboTextOnline = "온라인";

    const char* strTooltip0 = "해당 그룹은 변경하거나 삭제할 수 없습니다.";
    const char* strTooltip1 = "등록수/최대등록수";
    const char* strTooltip2 = "생성수/최대생성수";

    // 팝업
    const char* strGroupNameNORMAL = "일반";

    const char* strWhisper      = "귓속말";
    const char* strCharInfo     = "캐릭터정보";
    const char* strPartyAdd     = "파티초대";
    const char* strGroupChatAdd = "그룹채팅 초대";
    const char* strGroupEdit    = "그룹편집";
    const char* strMoveToFriend = "친구에게이동(카드사용)";
    const char* strBlock        = "차단하기";
    const char* strDelete       = "삭제하기";

    const char* strGRename       = "이름변경";
    const char* strGDelete       = "그룹삭제";
}


RnFriendTab::RnFriendTab( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
    : CUIGroupHelperMessageBox( pEngineDevice )
    , m_pInterface( pInterface )
    , m_pGaeaClient( pGaeaClient )
    , m_fUpdateTime( UPDATE_TIME )
    , m_bInit( false )
    , m_nCurrentFilter( FILTER_ALL )
{
    strListCount    = ID2GAMEWORD( "RNFRIEND_FRIENDTAB", 0);
    strGroupCount   = ID2GAMEWORD( "RNFRIEND_FRIENDTAB", 1);
    strView         = ID2GAMEWORD( "RNFRIEND_FRIENDTAB", 2);

    strBtnAdd       = ID2GAMEWORD( "RNFRIEND_FRIENDTAB_BTN", 0);
    strBtnMakeGroup = ID2GAMEWORD( "RNFRIEND_FRIENDTAB_BTN", 1);
    strBtnAddParty  = ID2GAMEWORD( "RNFRIEND_FRIENDTAB_BTN", 2);
    strBtnWhisper   = ID2GAMEWORD( "RNFRIEND_FRIENDTAB_BTN", 3);
    strBtnBlock     = ID2GAMEWORD( "RNFRIEND_FRIENDTAB_BTN", 4);
    strBtnDelete    = ID2GAMEWORD( "RNFRIEND_FRIENDTAB_BTN", 5);

    strComboTextAll     = ID2GAMEWORD( "RNFRIEND_FRIENDTAB_COMBO", 0);
    strComboTextOnline  = ID2GAMEWORD( "RNFRIEND_FRIENDTAB_COMBO", 1);

    strTooltip0         = ID2GAMEINTEXT( "RNFRIEND_FRIEND_TOOLTIP_0" );
    strTooltip1         = ID2GAMEINTEXT( "RNFRIEND_FRIEND_TOOLTIP_1" );
    strTooltip2         = ID2GAMEINTEXT( "RNFRIEND_FRIEND_TOOLTIP_2" );

    strGroupNameNORMAL  = ID2GAMEWORD("RNFRIEND_FRIENDTAB_SLOT_NORMAL", 0);

    strWhisper          = ID2GAMEWORD("RNFRIEND_FRIENDTAB_POPUP", 0);
    strCharInfo         = ID2GAMEWORD("RNFRIEND_FRIENDTAB_POPUP", 1);
    strPartyAdd         = ID2GAMEWORD("RNFRIEND_FRIENDTAB_POPUP", 2);
    strGroupChatAdd     = ID2GAMEWORD("INVITE_GROUP_CHAT_WINDOW_TITLE", 0);
    strGroupEdit        = ID2GAMEWORD("RNFRIEND_FRIENDTAB_POPUP", 3);
    strMoveToFriend     = ID2GAMEWORD("RNFRIEND_FRIENDTAB_POPUP", 4);
    strBlock            = ID2GAMEWORD("RNFRIEND_FRIENDTAB_POPUP", 5);
    strDelete           = ID2GAMEWORD("RNFRIEND_FRIENDTAB_POPUP", 6);

    strGRename          = ID2GAMEWORD("RNFRIEND_FRIENDTAB_GPOPUP", 0);
    strGDelete          = ID2GAMEWORD("RNFRIEND_FRIENDTAB_GPOPUP", 1);

    for( int nRow = 0 ; nRow < LIST_Y ; nRow++ )
    {
        m_pSlot[nRow] = NULL;
    }
    
}

void RnFriendTab::CreateSubControl()
{
    CD3DFontPar* pFont9 = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );
    CD3DFontPar* pFont10 = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 10, _DEFAULT_FONT_SHADOW_FLAG );

    CreateLineBox( "RNFRIEND_FRIEND_TAB", "GRAY_BACKGROUND_BLACK_LINE_TEXTURE" );
    CreateLineBox( "RNFRIEND_FRIEND_ROW_TOP", "GRAY_BACKGROUND_BLACK_LINE_TEXTURE" )->SetDiffuseAlpha(190);
    CreateLineBox( "RNFRIEND_FRIEND_ROW_BOTTOM_0", "TEXTURE_EMPTYBG_BLACKLINE" )->SetDiffuseAlpha(75);

    // 등록인원
    m_pText_ListCount = new CBasicTextBox(m_pEngineDevice);
    m_pText_ListCount->CreateSub ( this, "RNFRIEND_FRIEND_LISTCOUNT", UI_FLAG_DEFAULT, ID_LISTCOUNT );
    m_pText_ListCount->SetFont ( pFont10 );
    m_pText_ListCount->SetTextAlign ( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );	
    RegisterControl( m_pText_ListCount );

    // 그룹 개수
    m_pText_GroupCount = new CBasicTextBox(m_pEngineDevice);
    m_pText_GroupCount->CreateSub ( this, "RNFRIEND_FRIEND_GROUPCOUNT", UI_FLAG_DEFAULT, ID_GROUPCOUNT );
    m_pText_GroupCount->SetFont ( pFont10 );
    m_pText_GroupCount->SetTextAlign ( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );	
    RegisterControl( m_pText_GroupCount );

    // 보기
    CBasicTextBox* pText_ViewText = new CBasicTextBox(m_pEngineDevice);
    pText_ViewText->CreateSub ( this, "RNFRIEND_FRIEND_VIEWTEXT", UI_FLAG_DEFAULT );
    pText_ViewText->SetFont ( pFont10 );
    pText_ViewText->SetTextAlign ( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );	
    RegisterControl( pText_ViewText );


    // 친구목록 구성
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
    };

    for( int nIndex = 0; nIndex < LIST_Y ; nIndex++ )
    {
        m_pSlot[nIndex] = new RnFriendSlot( m_pGaeaClient, m_pEngineDevice );
        m_pSlot[nIndex]->CreateSub( this, strListRow[nIndex], UI_FLAG_DEFAULT, ID_LIST_CONTROL_START + nIndex );
        m_pSlot[nIndex]->CreateSubControl();
        RegisterControl( m_pSlot[nIndex] );
    }

    CreateLineBox( "RNFRIEND_FRIEND_TEXTEDIT_POS_BG", "GRAY_BACKGROUND_BLACK_LINE_TEXTURE", ID_TEXTBOX_BACKGROUND );

    CUIEditBoxMan* pEditBoxMan = new CUIEditBoxMan(m_pEngineDevice);
    pEditBoxMan->CreateSub( this, "RNFRIEND_FRIEND_TEXTEDIT_SIZE", UI_FLAG_DEFAULT, ID_TEXTBOX_BOXMAN );
    pEditBoxMan->CreateEditBox( ID_TEXTBOX_EDIT, "RNFRIEND_FRIEND_TEXTEDIT", "DEFAULT_CARRAT", TRUE, UINT_MAX, pFont9, 16 );
    RegisterControl( pEditBoxMan );
    m_pEditBox = pEditBoxMan;
    
    

    
    RnButton::CreateArg arg;
    arg.defaultTextureKey = "RNBUTTON_DEFAULT_TEXTURE";
    arg.mouseOverTextureKey = "RNBUTTON_OVER_TEXTURE";
    arg.mouseClickTextureKey = "RNBUTTON_CLICK_TEXTURE";
    arg.pFont = pFont10;
    arg.dwColor = NS_UITEXTCOLOR::WHITE;

    {
        RnButton* pRnButton = new RnButton( m_pEngineDevice );
        pRnButton->CreateSub(this,"RNFRIEND_BUTTON_ADD",UI_FLAG_DEFAULT, ID_BTN_ADD );
        arg.text = strBtnAdd;
        pRnButton->CreateSubControl(arg);
        RegisterControl(pRnButton);
    }

    {
        RnButton* pRnButton = new RnButton( m_pEngineDevice );
        pRnButton->CreateSub(this,"RNFRIEND_BUTTON_ADDGROUP",UI_FLAG_DEFAULT, ID_BTN_ADDGROUP );
        arg.text = strBtnMakeGroup;
        pRnButton->CreateSubControl(arg);
        RegisterControl(pRnButton);
    }

    {
        RnButton* pRnButton = new RnButton( m_pEngineDevice );
        pRnButton->CreateSub(this,"RNFRIEND_BUTTON_ADDPARTY",UI_FLAG_DEFAULT, ID_BTN_ADDPARTY );
        arg.text = strBtnAddParty;
        pRnButton->CreateSubControl(arg);
        RegisterControl(pRnButton);
    }

    {
        RnButton* pRnButton = new RnButton( m_pEngineDevice );
        pRnButton->CreateSub(this,"RNFRIEND_BUTTON_WHISPER",UI_FLAG_DEFAULT, ID_BTN_WHISPER );
        arg.text = strBtnWhisper;
        pRnButton->CreateSubControl(arg);
        RegisterControl(pRnButton);
    }

    {
        RnButton* pRnButton = new RnButton( m_pEngineDevice );
        pRnButton->CreateSub(this,"RNFRIEND_BUTTON_BLOCK",UI_FLAG_DEFAULT, ID_BTN_BLOCK );
        arg.text = strBtnBlock;
        pRnButton->CreateSubControl(arg);
        RegisterControl(pRnButton);
    }

    {
        RnButton* pRnButton = new RnButton( m_pEngineDevice );
        pRnButton->CreateSub(this,"RNFRIEND_BUTTON_DELETE",UI_FLAG_DEFAULT, ID_BTN_DELETE );
        arg.text = strBtnDelete;
        pRnButton->CreateSubControl(arg);
        RegisterControl(pRnButton);
    }

    //테두리[노란선]
    m_pYLine = CreateLineBox( "RNFRIEND_FRIEND_ROW_SELECTEDBOX_SIZE", "LINEBOX_Y" );
    m_pYLine->SetVisibleSingle( true );
    m_rcSeleted = m_pYLine->GetGlobalPos();

    { // 스크롤바
        CBasicScrollBarEx* pScrollBar = new CBasicScrollBarEx(m_pEngineDevice);
        pScrollBar->CreateSub( this, "BASIC_SCROLLBAR", UI_FLAG_RIGHT | UI_FLAG_YSIZE );
        pScrollBar->CreateBaseScrollBar( "RNFRIEND_FRIEND_SCROLL" );
        pScrollBar->GetThumbFrame()->SetScrollParent( this );
        pScrollBar->GetThumbFrame()->SetScrollMoveLine( 1 );
        RegisterControl ( pScrollBar );
        m_pScrollBar = pScrollBar;
    }

    //////////////////////////////////////////////////////////////////////////

    pText_ViewText->SetText( strView );



    { // 콤보박스 
        // 콤보박스 
        // 배경
        CreateLineBox( "RNFRIEND_FRIEND_VIEWCOMBO", "PRODUCT_WINDOW_BASE_SMARTLINEBOX_GRAY" );

        // 텍스트 영역
        CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
        pTextBox->CreateSub( this, "RNFRIEND_FRIEND_VIEWCOMBO", UI_FLAG_DEFAULT, ID_COMBO_TEXT );
        pTextBox->SetFont( pFont9 );		
        pTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y  );
        RegisterControl( pTextBox );
        m_pComboBoxText = pTextBox;

        //버튼
        RnButton* pRnButton = NULL;
        RnButton::CreateArg arg;
        //////////////////////////////////////////////////////////////////////////
        arg.defaultTextureKey = "ANOTHER_CHARACTER_COMBOBOX_BUTTON_NORMAL";
        arg.mouseOverTextureKey = "ANOTHER_CHARACTER_COMBOBOX_BUTTON_OVER";
        arg.mouseClickTextureKey = "ANOTHER_CHARACTER_COMBOBOX_BUTTON_DOWN";
        //////////////////////////////////////////////////////////////////////////

        pRnButton = new RnButton(m_pEngineDevice);
        pRnButton->CreateSub(this, "RNFRIEND_FRIEND_VIEWCOMBO_BUTTON", UI_FLAG_DEFAULT, ID_COMBO_BUTTON );
        pRnButton->CreateSubControl(arg);
        RegisterControl(pRnButton);


        CBasicComboBoxRollOver* pComboBoxRollOver;
        pComboBoxRollOver =  new CBasicComboBoxRollOver(m_pEngineDevice);
        pComboBoxRollOver->CreateSub( this, "BASIC_COMBOBOX_ROLLOVER", UI_FLAG_XSIZE | UI_FLAG_YSIZE, ID_COMBO_ROLLOVER );
        pComboBoxRollOver->CreateBaseComboBoxRollOver( "RNFRIEND_FRIEND_VIEWCOMBO_ROLLOVER");
        pComboBoxRollOver->SetVisibleSingle( FALSE );
        pComboBoxRollOver->SetVisibleThumb( FALSE );
        RegisterControl( pComboBoxRollOver );
        m_pComboBoxRollOver = pComboBoxRollOver;

        CString strTemp;
        strTemp.Format(" %s", strComboTextAll ); // 공백 삽입
        pComboBoxRollOver->AddText( strTemp.GetString() );
        strTemp.Format(" %s", strComboTextOnline ); // 공백 삽입
        pComboBoxRollOver->AddText(  strTemp.GetString() );

        strTemp = m_pComboBoxRollOver->GetSelectText( (int)m_nCurrentFilter );	
        m_pComboBoxText->SetText( strTemp );
    }

    CreateMessageBox();
}

void RnFriendTab::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    CUIGroupHelperMessageBox::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

    if( m_bInit == false ) // 초기화 작업
    {
        Refresh();
        m_bInit = true;
    }
// 
//     m_fUpdateTime += fElapsedTime;
//     if( UPDATE_TIME < m_fUpdateTime )
//     {
//         LoadFriend();
//         m_fUpdateTime = 0;
//     }
//     

    // 전체 갯수를 파악한다.
    unsigned int nTotal = 0;
    BOOST_FOREACH( const GROUP_FRIEND_MAP_VALUE& it, m_mapGroupFirend )
    {
        const SGROUP& sGroup = it.second;
        
        nTotal++;
        if( sGroup.m_bOpen == true )
        {
            BOOST_FOREACH( const std::tr1::shared_ptr<SFRIEND> pFriend, sGroup.m_vecFriendPtr)
            {
                if( pFriend == NULL )
                    continue;

                if( pFriend->m_Online == FALSE && m_nCurrentFilter == FILTER_ONLINE ) 
                    continue;

                nTotal++;

            }
        }
        
    }

    m_pScrollBar->GetThumbFrame()->SetState( nTotal, LIST_Y );
    int nVisibleLine = LIST_Y;

    // 스크롤 설정
    CBasicScrollThumbFrame* const pThumbFrame = m_pScrollBar->GetThumbFrame();
    const int nMovableLine = nTotal - nVisibleLine;
    float fPercent = pThumbFrame->GetPercent();

    m_nViewPos = (int)floor( fPercent * nMovableLine );

    if( m_nViewPos < 0 )
        m_nViewPos = 0;

    unsigned int nRow=0;
    bool bVisibleSelectLine = false;
    UIRECT rcSelectBoxPos;
    BOOST_FOREACH( const GROUP_FRIEND_MAP_VALUE& it, m_mapGroupFirend )
    {
        const WORD wGroupNum            = it.first;
        const SGROUP& sGroup            = it.second;

        RnFriendSlot::SDATA_POS sDataPos = RnFriendSlot::SDATA_POS(wGroupNum, 0);
        if(m_nViewPos <= nRow && nRow-m_nViewPos < LIST_Y)
        {
            m_pSlot[ nRow-m_nViewPos ]->SetData( sDataPos, sGroup.m_vecFriendPtr, sGroup.m_bOpen );

            // 그룹 선택
            if( m_sSelectedData.nDataIndex == 0 && m_sSelectedData.wGroupId == wGroupNum )
            {
                bVisibleSelectLine = true;
                rcSelectBoxPos = m_pSlot[ nRow-m_nViewPos ]->GetGlobalPos();

                // 추가된 예외 일반(0번 그룹)그룹은 선택할수 없음. (편집이나 삭제 불능)
                if( m_sSelectedData.wGroupId == 0 )
                    bVisibleSelectLine = false;
            }
        }
        
        nRow++;
        
        if( sGroup.m_bOpen == true )
        {
            for( unsigned int nIndex = 0 ; nIndex < sGroup.m_vecFriendPtr.size() ; nIndex++ )
            {
                const std::tr1::shared_ptr<SFRIEND> pFriend = sGroup.m_vecFriendPtr[nIndex];
                if( pFriend == NULL || pFriend->m_Online == FALSE && m_nCurrentFilter == FILTER_ONLINE ) 
                    continue;

                sDataPos.nDataIndex++;

                if( m_nViewPos <= nRow && nRow-m_nViewPos < LIST_Y )
                {

                    m_pSlot[ nRow-m_nViewPos ]->SetData( sDataPos, pFriend );

                    // 일반 선택
                    if( m_sSelectedData.wGroupId == wGroupNum 
                        && m_sSelectedData.nDataIndex == nIndex+1 )
                    {
                        bVisibleSelectLine = true;
                        rcSelectBoxPos = m_pSlot[ nRow-m_nViewPos ]->GetGlobalPos();
                    }

                }
    

                nRow++;
            }


            int nDataIndex = 1;
            BOOST_FOREACH( const std::tr1::shared_ptr<SFRIEND> pFriend, sGroup.m_vecFriendPtr)
            {

            } // BOOST_FOREACH( const std::tr1::shared_ptr<SFRIEND> pFriend, sGroup.m_vecFriendPtr)
        } // if( sGroup.m_bOpen == true )
    }

    while( nRow-m_nViewPos < LIST_Y )
    {
        m_pSlot[ nRow-m_nViewPos ]->Reset();
        nRow++;
    }

    if ( bVisibleSelectLine == TRUE )
    {
        float fLeftMargine = rcSelectBoxPos.sizeX - m_rcSeleted.sizeX;
        float fTopMargine = rcSelectBoxPos.sizeY - m_rcSeleted.sizeY;

        m_pYLine->SetGlobalPos( 
            D3DXVECTOR2( rcSelectBoxPos.left +fLeftMargine/2, rcSelectBoxPos.top +fTopMargine/2 ) 
            );
        m_pYLine->SetVisibleSingle( bVisibleSelectLine );
    }
    else
    {
        m_pYLine->SetVisibleSingle( FALSE );
    }
   
}

void RnFriendTab::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
    CUIGroupHelperMessageBox::TranslateUIMessage( ControlID, dwMsg );

    if( ControlID == ID_TEXTBOX_BACKGROUND )
        if ( CHECK_MOUSE_IN( dwMsg ) == FALSE && CHECK_LB_DOWN_LIKE( dwMsg ) )
            m_pEditBox->EndEdit();

    // 메시지 박스가 떠있다면 메시지를 무시한다.
    if( IsOpenMessageBox() == TRUE ) 
        return;

    switch( ControlID )
    {
    case ID_TEXTBOX_BOXMAN:
    case ID_TEXTBOX_EDIT:
        break;
    case ID_LISTCOUNT:
        if(  CHECK_MOUSE_IN( dwMsg ) )
        {
            m_pInterface->SHOW_COMMON_LINEINFO(
                strTooltip1,
                D3DCOLOR_XRGB( 0x50, 0x82, 0xC0 ),
                TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );
        }
        break;
    case ID_GROUPCOUNT:
        if(  CHECK_MOUSE_IN( dwMsg ) )
        {
            m_pInterface->SHOW_COMMON_LINEINFO(
                strTooltip2,
                D3DCOLOR_XRGB( 0x50, 0x82, 0xC0 ),
                TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );
        }
        break;
    case ID_BTN_ADD:
        if( CHECK_MOUSE_IN( dwMsg ) && CHECK_LB_UP_LIKE( dwMsg )  )
        {
/*            if( m_nFriendListSize < GLCONST_CHAR::nMAX_FRIEND_LIST )*/
            {
                const std::string str = GetEditBoxString();
                if( str.empty() == true )
                    return;

                OpenMessageBox( CMessageBox::TITLE_QUESTION
                    , sc::string::format( ID2GAMEINTEXT("FRIEND_ADD"), str )
                    , CMessageBox::TYPE_YESNO, IDENTI_ADDFRIEND );
                m_strInputData = str;
                m_pEditBox->ClearEdit( ID_TEXTBOX_EDIT );
            }
//             else
//             {
//                 OpenMessageBox( CMessageBox::TITLE_WARNING
//                     , ID2GAMEINTEXT("RNFRIEND_FRIEND_DONOTADD" )
//                     , CMessageBox::TYPE_OK, IDENTI_ADDFRIEND );
// 
//             }
            

        }
        break;
    case ID_BTN_ADDGROUP:
        if( CHECK_MOUSE_IN( dwMsg ) && CHECK_LB_UP_LIKE( dwMsg ) )
        {
            const std::string str = GetEditBoxString();
            if( str.empty() == true )
                return;

            std::wstring strUnicode = sc::string::ansiToUnicode( str.c_str() );
            if( sc::string::haveSymbolCha( strUnicode ) )
            {
                OpenMessageBox( CMessageBox::TITLE_WARNING
                    , ID2GAMEINTEXT("RNFRIEND_FRIEND_GROUPEDIT_ERROR")
                    , CMessageBox::TYPE_OK );
                m_pEditBox->ClearEdit( ID_TEXTBOX_EDIT );
                return;
            }

            std::string strTest = sc::string::format( ID2GAMEINTEXT("RNFRIEND_FRIEND_ADDGROUP"), str );
            OpenMessageBox( CMessageBox::TITLE_QUESTION, strTest.c_str(), CMessageBox::TYPE_YESNO, IDENTI_ADDGROUP );
        
            m_strInputData = str;
            m_pEditBox->ClearEdit( ID_TEXTBOX_EDIT );
        }
        
        break;
    case ID_BTN_ADDPARTY:
        if( CHECK_MOUSE_IN( dwMsg ) && CHECK_LB_UP_LIKE( dwMsg ) )
        {
            SFRIEND* pFriend = FindFriend( m_sSelectedData );
            if( pFriend != NULL && pFriend->m_Online )
            {
                std::string strText = sc::string::format( ID2GAMEINTEXT("PARTY_QUESTION"), pFriend->m_ChaName );
                m_pInterface->DoPartyModal( strText, UI::MODAL_PARTY_REQ_DBNUM, EMPARTY_MODE_PARTY, pFriend->m_ChaDbNum );
            }
            else
                OpenMessageBox( CMessageBox::TITLE_WARNING
                , ID2GAMEINTEXT("RNFRIEND_FRIEND_WRONGTARGET")
                , CMessageBox::TYPE_OK );
            return;
        }

        break;
    case ID_BTN_WHISPER:
        if( CHECK_MOUSE_IN( dwMsg ) && CHECK_LB_UP_LIKE( dwMsg ) )
        {
            SFRIEND* pFriend = FindFriend( m_sSelectedData );
            
            if( pFriend != NULL && pFriend->m_Online  )
            {
                m_pInterface->GetBasicChatWindow()->BeginPrivateChat( pFriend->m_ChaName );
                m_pInterface->ADD_FRIEND_NAME_TO_EDITBOX( pFriend->m_ChaName.c_str() );
            }
            else
                OpenMessageBox( CMessageBox::TITLE_WARNING
                , ID2GAMEINTEXT("RNFRIEND_FRIEND_WRONGTARGET")
                , CMessageBox::TYPE_OK );

            return;
        }
        break;
    case ID_BTN_BLOCK:
        if( CHECK_MOUSE_IN( dwMsg ) && CHECK_LB_UP_LIKE( dwMsg ) )
        {

            SFRIEND* pFriend = FindFriend( m_sSelectedData );
            if( pFriend != NULL )
            {
                std::string strText = sc::string::format( ID2GAMEINTEXT("FRIEND_BLOCK_ADD"), pFriend->m_ChaName );
                OpenMessageBox( CMessageBox::TITLE_QUESTION, strText, CMessageBox::TYPE_YESNO, IDENTI_ADDBLOCK );
            }
            else
                OpenMessageBox( CMessageBox::TITLE_WARNING
                , ID2GAMEINTEXT("RNFRIEND_FRIEND_WRONGTARGET")
                , CMessageBox::TYPE_OK );
        }
        break;
    case ID_BTN_DELETE:
        if( CHECK_MOUSE_IN( dwMsg ) && CHECK_LB_UP_LIKE( dwMsg ) )
        {
            SFRIEND* pFriend = FindFriend( m_sSelectedData );
            std::string strMsgBoxString;

            if( m_sSelectedData.nDataIndex == 0 && m_sSelectedData.wGroupId != 0)
                strMsgBoxString = sc::string::format( ID2GAMEINTEXT("RNFRIEND_FRIEND_DELGROUP"), m_pGaeaClient->FriendGetGroupName( m_sSelectedData.wGroupId ) );
            else if( pFriend != NULL )
                strMsgBoxString = sc::string::format( ID2GAMEINTEXT("FRIEND_DEL"), pFriend->m_ChaName );
            
            if( strMsgBoxString.empty() != true )
            {
                OpenMessageBox( CMessageBox::TITLE_QUESTION, strMsgBoxString, CMessageBox::TYPE_YESNO, IDENTI_DELFRIEND );
            }
            else
                OpenMessageBox( CMessageBox::TITLE_WARNING
                , ID2GAMEINTEXT("RNFRIEND_FRIEND_WRONGTARGET")
                , CMessageBox::TYPE_OK );

            return;
        }
        break;
    case ID_COMBO_TEXT:
    case ID_COMBO_BUTTON:
        if( CHECK_MOUSE_IN( dwMsg ) && CHECK_LB_UP_LIKE( dwMsg ) )
        {
            m_pComboBoxRollOver->SetVisibleSingle( !m_pComboBoxRollOver->IsVisible() );
            return;
        }
        break;
    case ID_COMBO_ROLLOVER:
        if( CHECK_MOUSE_IN( dwMsg ) )
        {
            if( CHECK_LB_UP_LIKE( dwMsg ) )
            {
                int nIndex = m_pComboBoxRollOver->GetSelectIndex();
                if ( nIndex < 0 )
                    return;

                
                m_nCurrentFilter = nIndex;
                CString strTemp = m_pComboBoxRollOver->GetSelectText( nIndex );
                m_pComboBoxText->SetText( strTemp, NS_UITEXTCOLOR::WHITE );
                m_pComboBoxRollOver->SetVisibleSingle( FALSE );

                LoadFriend();
                return;
            }
        }
        break;
    }

    //////////////////////////////////////////////////////////////////////////
    // 내부 일괄 처리
    if( ID_LIST_CONTROL_START <= ControlID && ControlID <= ID_LIST_CONTROL_END )
    {
        if( CHECK_MOUSE_IN( dwMsg ) )
        {
            unsigned int nIndex = ControlID - ID_LIST_CONTROL;

            if( m_pSlot[nIndex] == NULL || LIST_Y <= nIndex )
                return;

            if( m_pComboBoxRollOver->IsVisible() == TRUE )
                return;

            RnFriendSlot::eSlotState slotstate =  m_pSlot[nIndex]->GetSlotState();

            switch( slotstate )
            {
            case RnFriendSlot::SLOT_GROUP:
                if( m_pSlot[nIndex]->GetGroupNum() == 0 )
                    m_pInterface->SHOW_COMMON_LINEINFO(
                    strTooltip0,
                    D3DCOLOR_XRGB( 0x50, 0x82, 0xC0 ),
                    TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );

                if ( dwMsg & UIMSG_REDUCTION_CLICK || dwMsg & UIMSG_EXPANSION_CLICK )
                {
                    const WORD wGroupNum = m_pSlot[nIndex]->GetGroupNum();

                    const GROUP_FRIEND_MAP_ITER itFind = m_mapGroupFirend.find( wGroupNum );

                    if( itFind != m_mapGroupFirend.end() )
                    {
                        SGROUP& sGroup = itFind->second;
                        sGroup.m_bOpen = !sGroup.m_bOpen;
                    }
                    break;
                }

                if( CHECK_LB_UP_LIKE( dwMsg ) )
                {
                    m_sSelectedData = m_pSlot[nIndex]->GetDataPos();
                    return;
                }

                break;
            case RnFriendSlot::SLOT_FRIEND:
                ShowPartyInfo( m_pSlot[nIndex] );
                if( CHECK_LB_UP_LIKE( dwMsg ) )
                {
                    m_sSelectedData = m_pSlot[nIndex]->GetDataPos();
                }

				if( CHECK_MOUSEIN_LBDUPLIKE( dwMsg ) )
				{
					SFRIEND* pFriend = FindFriend( m_sSelectedData );
					if( pFriend )
					{
						m_pInterface->GetBasicChatWindow()->BeginPrivateChat( pFriend->m_ChaName );
					}
				}
                break;
            }

            if( CHECK_RB_UP_LIKE( dwMsg ) )
            {
                m_sSelectedData = m_pSlot[nIndex]->GetDataPos();

//                 if( m_sSelectedData.wGroupId == 0 && m_sSelectedData.nDataIndex == 0 )
//                     return;

                if( m_sSelectedData.nDataIndex == 0 )
                {
                    OpenGroupPopup();
                }
                else
                {
                    OpenFriendPopup();

                }
                return;
            }
        }


    }


}

void RnFriendTab::TranslateMessageBox( unsigned int nIdentity, DWORD dwMsg )
{
    switch( nIdentity )
    {
    case IDENTI_ADDFRIEND:
        if( dwMsg & UIMSG_MESSAGEBOX_POSITIVE )
        {
            if( m_strInputData.empty() == FALSE )
                m_pInterface->ADD_FRIEND( m_strInputData.c_str() );
                //m_pGaeaClient->GetFriendClient()->ReqFriendAdd( m_strInputData.c_str() );

            m_strInputData.clear();
        }
        break;
    case IDENTI_ADDGROUP:
        if( dwMsg & UIMSG_MESSAGEBOX_POSITIVE )
        {
            std::vector<FRIEND_GROUP> vecGroup;
            m_pGaeaClient->FriendGroup( vecGroup );

            if( (unsigned int)GLCONST_CHAR::nMAX_FRIEND_GROUP <= vecGroup.size() + 1 )
                return;

            if( m_strInputData.empty() == FALSE )
                m_pGaeaClient->FriendGroupNewCA( m_strInputData );
            m_strInputData.clear();
        }
        break;
    case IDENTI_ADDBLOCK:
        if( dwMsg & UIMSG_MESSAGEBOX_POSITIVE )
        {
            SFRIEND* pFriend = FindFriend( m_sSelectedData );

            if( pFriend != NULL )
            {
                //m_pGaeaClient->GetFriendClient()->ReqFriendDel ( pFriend->m_ChaName.c_str() );
                //m_pGaeaClient->GetFriendClient()->ReqFriendBlock( pFriend->m_ChaName.c_str(), true );
                m_pInterface->ADD_FRIEND_BLOCK( pFriend->m_ChaName.c_str(), true );
            }
        }
        break;
    case IDENTI_DELGROUP:
    case IDENTI_DELFRIEND:
        if( dwMsg & UIMSG_MESSAGEBOX_POSITIVE )
        {
            SFRIEND* pFriend = FindFriend( m_sSelectedData );

            if( m_sSelectedData.nDataIndex == 0 )
            {
                m_pGaeaClient->FriendGroupDelReq( m_sSelectedData.wGroupId );
                m_mapGroupFirend.erase( m_sSelectedData.wGroupId );
            }
            else if( pFriend != NULL )
                m_pGaeaClient->GetFriendClient()->ReqFriendDel( pFriend->m_ChaName.c_str() );            
        }
        break;
    case IDENTI_EDITGROUP:
        if( dwMsg & UIMSG_MESSAGEBOX_POSITIVE )
        {
            std::string strInput = GetInputString();
            if( strInput.empty() == FALSE )
                m_pGaeaClient->FriendRenameGroup( m_sSelectedData.wGroupId, strInput );
        }
        break;
    case IDENTI_MOVETOFRIEND:
        if( dwMsg & UIMSG_MESSAGEBOX_POSITIVE )
        {
            SFRIEND* pFriend = FindFriend( m_sSelectedData );
            if( pFriend != NULL )
            {
                m_pInterface->SetFriendName ( UI::ToString( pFriend->m_ChaName ) );
                m_pInterface->WAITSERVER_DIALOGUE_OPEN( ID2GAMEINTEXT("WAITSERVER_MESSAGE")
                    , WAITSERVER_TOFRIEND, 10.0f );
            }
            
        }
        break;
    }
}

void RnFriendTab::Refresh()
{
    LoadGroup();
    LoadFriend();

}

void RnFriendTab::SetWhisperName( const std::string& strCharName )
{
    m_pEditBox->SetEditString( ID_TEXTBOX_EDIT, strCharName.c_str() );
}

void RnFriendTab::LoadGroup()
{

    //m_mapGroupFirend.clear();

    m_GroupInfo.clear();
    m_pGaeaClient->FriendGroup( m_GroupInfo );

    m_mapGroupFirend.insert( std::make_pair( 0, SGROUP() ) );
    
    BOOST_FOREACH( const FRIEND_GROUP& it, m_GroupInfo )
    {
        m_mapGroupFirend.insert( std::make_pair( it.m_GroupNum, SGROUP() ) );
    }

    CString strGroupText;
    strGroupText.Format( "%s %d/%d", strGroupCount, m_GroupInfo.size() + 1,  GLCONST_CHAR::nMAX_FRIEND_GROUP );

    m_pText_GroupCount->SetText( strGroupText );
}

void RnFriendTab::LoadFriend()
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
            vecTempOnlineFriend.push_back( spFriend );
        else
            vecTempOfflineFriend.push_back( spFriend );

    }

    BOOST_FOREACH( GROUP_FRIEND_MAP_VALUE& it, m_mapGroupFirend )
    {
        SGROUP& sGroup = it.second;
        sGroup.m_vecFriendPtr.clear();
    }

    // 온라인
    BOOST_FOREACH( const std::tr1::shared_ptr<SFRIEND>& it, vecTempOnlineFriend )
    {
        WORD wGroupNum = it->GetGroup();
        GROUP_FRIEND_MAP_ITER itFind = m_mapGroupFirend.find( wGroupNum );


        if( itFind != m_mapGroupFirend.end() )
        {
            SGROUP& sGroup = itFind->second;
            sGroup.m_vecFriendPtr.push_back( it );
        }
    }

    // 오프라인
    BOOST_FOREACH( const std::tr1::shared_ptr<SFRIEND> it, vecTempOfflineFriend )
    {
        WORD wGroupNum = it->GetGroup();
        it->m_Online = false;

        GROUP_FRIEND_MAP_ITER itFind = m_mapGroupFirend.find( wGroupNum );


        if( itFind != m_mapGroupFirend.end() )
        {
            SGROUP& sGroup = itFind->second;
            sGroup.m_vecFriendPtr.push_back( it );
        }

        if( itFind == m_mapGroupFirend.end() )
            m_pGaeaClient->FriendGroupSetFriend( it->GetChaDbNum(), 0 );

    }

    m_nFriendListSize = vecTempOnlineFriend.size() + vecTempOfflineFriend.size();
    CString strListCountText;
    strListCountText.Format( "%s %d/%d",strListCount , m_nFriendListSize, GLCONST_CHAR::nMAX_FRIEND_LIST );

    m_pText_ListCount->SetText( strListCountText );

}

SFRIEND* RnFriendTab::FindFriend( const RnFriendSlot::SDATA_POS& sDataPos )
{
    GROUP_FRIEND_MAP_ITER itFind = m_mapGroupFirend.find( sDataPos.wGroupId );

    // 그룹이라면
    if( sDataPos.nDataIndex <= 0 )
        return NULL;    

    if( itFind != m_mapGroupFirend.end() )
    {   
        SGROUP& sGroup = itFind->second;

        unsigned int nIndex = sDataPos.nDataIndex-1;

        if( sDataPos.nDataIndex <= sGroup.m_vecFriendPtr.size() )
            return sGroup.m_vecFriendPtr[nIndex].get();
    }



    return NULL;
}

std::string RnFriendTab::GetEditBoxString()
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

void RnFriendTab::NoConnectMessage()
{
    OpenMessageBox( CMessageBox::TITLE_WARNING, ID2GAMEINTEXT("RNFRIEND_FRIEND_ISOFFLINE"), CMessageBox::TYPE_OK );
}


void RnFriendTab::ShowPartyInfo( RnFriendSlot* pCurrentSlot )
{
    if( pCurrentSlot == NULL )
        return;

    const GLPartyID& PartyID = pCurrentSlot->GetPartyID();
    std::tr1::shared_ptr<PARTY_CACHE_CLIENT> spParty_Client = m_pGaeaClient->GetOtherPartyInfo( PartyID );

    if( PartyID.isValidParty() == true )
    {
        if( spParty_Client )
        {
            int nMemberCnt = spParty_Client->m_Member.size();
            std::string strText;
            if( 0 < nMemberCnt )
                strText = sc::string::format( ID2GAMEINTEXT("RNFRIEND_FRIEND_TOOLTIP_3"), nMemberCnt, MAXPARTY );

            m_pInterface->SHOW_COMMON_LINEINFO(
                strText.c_str(),
                D3DCOLOR_XRGB( 0xC0, 0xFF, 0x0 ),
                TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );
        }
    }
}

void RnFriendTab::SetVisibleSingle( bool bFlag )
{
    m_pEditBox->EndEdit();
    CUIGroupHelperMessageBox::SetVisibleSingle( bFlag );
}

void RnFriendTab::OpenFriendPopup()
{

    SFRIEND* pFriend = FindFriend( m_sSelectedData );
    bool bOnline = false;
    
    if( pFriend )
        bOnline = pFriend->m_Online;

    popupmenu::SP_POPUP_MENU PopupMenu = popupmenu::CreateRoot();
    {
        PopupMenu->addComponent( 
            CREATEPOPUPITEM2( strWhisper, RnFriendTab, ActionWHISPER, bOnline ) );
        if(GLUseFeatures::GetInstance().IsUsingReferChar())
            PopupMenu->addComponent( CREATEPOPUPITEM2( strCharInfo, RnFriendTab, ActionVIEW_INFO, bOnline ) );
        PopupMenu->addComponent( 
            CREATEPOPUPITEM2( strPartyAdd, RnFriendTab, ActionPARTY_INVITE, bOnline ) );
		if(GLUseFeatures::GetInstance().IsUsingGroupChat())
            PopupMenu->addComponent( CREATEPOPUPITEM2( strGroupChatAdd, RnFriendTab, ActionGROUPCHAT_INVITE, bOnline ) );

        popupmenu::SP_POPUP_MENU PopupGroupEdit = popupmenu::CreatePopupMenuComposite( strGroupEdit, true );     
        PopupMenu->addComponent( PopupGroupEdit );

        PopupMenu->addComponent( 
            CREATEPOPUPITEM2( strMoveToFriend, RnFriendTab, ActionMOVE_TO_FRIEND, bOnline ) );
        PopupMenu->addComponent( 
            CREATEPOPUPITEM( strBlock, RnFriendTab, ActionFRIEND_BLOCK ) );
        PopupMenu->addComponent( 
            CREATEPOPUPITEM( strDelete, RnFriendTab, ActionDELETE_FRIEND ) );

        // PopupGroupEdit
        {
            //텍스트 삽입
            PopupGroupEdit->addComponent( CREATEPOPUPITEM( strGroupNameNORMAL, RnFriendTab, ActionFRIEND_GROUP_MOVE ) );
            BOOST_FOREACH( const FRIEND_GROUP& it, m_GroupInfo )
                PopupGroupEdit->addComponent( CREATEPOPUPITEM( it.m_GroupName, RnFriendTab, ActionFRIEND_GROUP_MOVE ) );
        }
    }

    m_pInterface->OpenPopupMenu( PopupMenu );
}

void RnFriendTab::OpenGroupPopup()
{
    if( m_sSelectedData.nDataIndex == 0 )
    {
        popupmenu::SP_POPUP_MENU PopupMenu = popupmenu::CreateRoot();
        {
            PopupMenu->addComponent( 
                CREATEPOPUPITEM2( strGRename, RnFriendTab, ActionEDIT_GORUP, m_sSelectedData.wGroupId != 0 ) );
            PopupMenu->addComponent( 
                CREATEPOPUPITEM2( strGDelete, RnFriendTab, ActionDELETE_GROUP, m_sSelectedData.wGroupId != 0 ) );

        }
        m_pInterface->OpenPopupMenu( PopupMenu );
    }
}

void RnFriendTab::ActionWHISPER( int nPopupIndex )
{
    SFRIEND* pFriend = FindFriend( m_sSelectedData );

    if( pFriend != NULL && pFriend->m_Online  )
    {
        m_pInterface->GetBasicChatWindow()->BeginPrivateChat( pFriend->m_ChaName );
        m_pInterface->ADD_FRIEND_NAME_TO_EDITBOX( pFriend->m_ChaName.c_str() );
    }

    return;
}

void RnFriendTab::ActionVIEW_INFO( int nPopupIndex )
{
    SFRIEND* pFriend = FindFriend( m_sSelectedData );

    if( pFriend != NULL && pFriend->m_Online  )
        m_pInterface->ReferCharWindowOpen( pFriend->m_ChaName );

}

void RnFriendTab::ActionPARTY_INVITE( int nPopupIndex )
{
    SFRIEND* pFriend = FindFriend( m_sSelectedData );

    if( pFriend != NULL && pFriend->m_Online  )
    {

        std::string strText = sc::string::format( ID2GAMEINTEXT("PARTY_QUESTION"), pFriend->m_ChaName );
        m_pInterface->DoPartyModal( strText, UI::MODAL_PARTY_REQ_DBNUM, EMPARTY_MODE_PARTY, pFriend->m_ChaDbNum );
    }
}

void RnFriendTab::ActionGROUPCHAT_INVITE( int nPopupIndex )
{
    SFRIEND* pFriend = FindFriend( m_sSelectedData );

    if( pFriend != NULL )
    {
        const std::string& strText = sc::string::format( ID2GAMEWORD("INVITE_GROUP_CHAT_WINDOW_INVITE_MSG") , pFriend->GetName() );
        m_pInterface->DoChatGroupModal( strText, UI::MODAL_CHAT_GROUP_REQ, EMPARTY_MODE_PARTY, pFriend->GetChaDbNum() );
    }

}

void RnFriendTab::ActionFRIEND_GROUP_MOVE( int nPopupIndex )
{
    SFRIEND* pFriend = FindFriend( m_sSelectedData );

    if( pFriend != NULL )
    {
        m_pGaeaClient->FriendGroupSetFriend( pFriend->m_ChaDbNum, nPopupIndex );
    }


}

void RnFriendTab::ActionMOVE_TO_FRIEND( int nPopupIndex )
{
	GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
	if( NULL == pCharacter )
		return;

    SFRIEND* pFriend = FindFriend( m_sSelectedData );
    if( pFriend != NULL && pFriend->m_Online  )
    {
		// 친구연결카드가 있는지 먼저 검사한다.
		SINVENITEM* pItem = pCharacter->m_cInventory.FindItem( ITEM_2FRIEND );
		if( NULL == pItem )
		{
			m_pGaeaClient->PrintMsgTextDlg( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT( "EM2FRIEND_FB_NO_ITEM" ) );
			return;
		}

        m_pInterface->SetFriendName ( UI::ToString( pFriend->m_ChaName ) );
        m_pInterface->WAITSERVER_DIALOGUE_OPEN( ID2GAMEINTEXT("WAITSERVER_MESSAGE")
            , WAITSERVER_TOFRIEND, 10.0f );
    }
}

void RnFriendTab::ActionFRIEND_BLOCK( int nPopupIndex )
{
    SFRIEND* pFriend = FindFriend( m_sSelectedData );

    if( pFriend != NULL )
        OpenMessageBox( CMessageBox::TITLE_QUESTION, sc::string::format( ID2GAMEINTEXT("FRIEND_BLOCK_ADD"), pFriend->m_ChaName ), CMessageBox::TYPE_YESNO, IDENTI_ADDBLOCK );
}

void RnFriendTab::ActionDELETE_FRIEND( int nPopupIndex )
{
    SFRIEND* pFriend = FindFriend( m_sSelectedData );

    if( pFriend != NULL )
    {
        std::string strMsgBoxString;

        if( m_sSelectedData.nDataIndex == 0 && m_sSelectedData.wGroupId != 0)
            strMsgBoxString = sc::string::format( ID2GAMEINTEXT("RNFRIEND_FRIEND_DELGROUP"), m_pGaeaClient->FriendGetGroupName( m_sSelectedData.wGroupId ) );
        else if( pFriend != NULL )
            strMsgBoxString = sc::string::format( ID2GAMEINTEXT("FRIEND_DEL"), pFriend->m_ChaName );

        if( strMsgBoxString.empty() != true )
        {
            OpenMessageBox( 
                CMessageBox::TITLE_QUESTION, 
                strMsgBoxString, 
                CMessageBox::TYPE_YESNO, IDENTI_DELFRIEND );
        }
    }
}

void RnFriendTab::ActionEDIT_GORUP( int nPopupIndex )
{
    OpenMessageBox( CMessageBox::TITLE_QUESTION
        , ID2GAMEWORD("RNFRIEND_FRIENDTAB_POPUP", 3)
        , CMessageBox::TYPE_INPUT, RnFriendTab::IDENTI_EDITGROUP );
}

void RnFriendTab::ActionDELETE_GROUP( int nPopupIndex )
{
    if( m_sSelectedData.nDataIndex == 0 )
    {
        m_pGaeaClient->FriendGroupDelReq( m_sSelectedData.wGroupId );
        m_mapGroupFirend.erase( m_sSelectedData.wGroupId );
    }
}
