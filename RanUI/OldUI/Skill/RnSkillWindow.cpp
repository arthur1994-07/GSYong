#include "StdAfx.h"
#include "RnSkillWindow.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicLineBoxSmart.h"
#include "../../../EngineLib/GUInterface/BasicTextBoxEx.h"
#include "../../../EngineLib/GUInterface/BasicScrollBarEx.h"
#include "../../../EngineLib/GUInterface/BasicScrollThumbFrame.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/DxTools/d3dfont.h"

#include "../../../RanLogic/GLogicDataMan.h"
#include "../../../RanLogic/Skill/GLRnSkillLearnSetting.h"

#include "../../../RanLogicClient/Char/GLCharacter.h"
#include "../../../RanLogicClient/GLGaeaClient.h"

#include "../Util/RnButton.h"
#include "../Util/MessageBox.h"

#include "RnSkillNomalToolTip.h"
#include "RnSkillHelpToolTip.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace
{
    const char* ID2GAMEWORD_TITLE;
    const char* ID2GAMEWORD_RESET;
    const char* ID2GAMEWORD_CONFIRM;
}


RnSkillWindow::RnSkillWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
: CUIWindowEx( pInterface, pEngineDevice )
, m_pGaeaClient( pGaeaClient )
, m_pInterface( pInterface )
, m_pScrollBar( NULL )
, m_pYLine( NULL )
, m_pGLine( NULL )
, m_pBLine( NULL )
, m_pCurrentLine( NULL )
, m_pRollOverSlot( NULL )
, m_pResetToolTip( NULL )
, m_pConfirmToolTip( NULL )
, m_pCheckToolTip( NULL )
, m_HelpTooltip( NULL )
, m_nCurrentPage( 0 )
, m_pSkillPoint( NULL )
, m_dwSkillPointBACK( 0 )
, m_nViewPos( 0 )
, m_bInit( FALSE )
{
    
    for( int i =0 ; i < TAB_COUNT ; i++ )
        m_pSkillPageText[i] = NULL;

    for( int i =0 ; i <  SKILLSLOT_COUNT ; i++ )
        m_SkillSlot[i] = NULL;

    BOOST_FOREACH(SkillInfoVec& vecSkill, m_vecSkillTab )
        vecSkill.clear();

    ID2GAMEWORD_TITLE = ID2GAMEWORD("RNSKILLWINDOW_TITLE", 0 );
    ID2GAMEWORD_RESET = ID2GAMEWORD("RNSKILLWINDOW_TITLE", 1 );
    ID2GAMEWORD_CONFIRM = ID2GAMEWORD("RNSKILLWINDOW_TITLE", 2 );
}

void RnSkillWindow::CreateSubControl()
{
    CD3DFontPar* pFont9 = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_SHADOW_FLAG );

    CreateBaseWindowLightGray( "RNSKILL_WINDOW",  ID2GAMEWORD_TITLE );

    CreateLineBox( "RNSKILL_TAB_REGION", "RNSKILL_WINDOW_REGION_TEXTUREINFO" );
    //CreateLineBox( "RNSKILL_SKILL_LIST_REGION", "PRODUCT_WINDOW_BASE_SMARTLINEBOX_GRAY" );

    //스킬 TAB 영역
    CBasicLineBoxSmart* pLinBoxSmart = new CBasicLineBoxSmart( m_pEngineDevice );
    pLinBoxSmart->CreateSub( this, "RNSKILL_SKILL_LIST_REGION", UI_FLAG_XSIZE | UI_FLAG_YSIZE, ID_RNSKILL_TAB );
    pLinBoxSmart->CreateSubControl( "PRODUCT_WINDOW_BASE_SMARTLINEBOX_GRAY" );
    RegisterControl( pLinBoxSmart );


    CString strTemp;
    // 18칸 영역 만들기
    for( int i=0; i<SKILLSLOT_COUNT; ++i )
    {
        strTemp.Format( "%s%d", "RNSKILL_SLOT_", i );

        CRnSkillSlot* pSkillSlot = new CRnSkillSlot( m_pGaeaClient, m_pInterface, m_pEngineDevice );
        pSkillSlot->CreateSub ( this, strTemp, UI_FLAG_DEFAULT, ID_SKILL_SLOT_RECT_CONTROL_START + i );
        pSkillSlot->CreateSubControl();
        pSkillSlot->SetVisibleSingle( TRUE );
        RegisterControl( pSkillSlot );

        m_SkillSlot[i] = pSkillSlot;
    }


    
    {   
        // 스킬텝1
        STAPBUTTON* pSkillTab0 = new STAPBUTTON(m_pEngineDevice);
        pSkillTab0->CreateSub( this, "RNSKILL_TAB_1", UI_FLAG_DEFAULT, ID_RNSKILL_TAB_0 );
        pSkillTab0->CreateSubControl( " ",
            "CHARACTER_WINDOW_TAP_BASE","CHARACTER_WINDOW_TAP_ACTIVE",
            "CHARACTER_WINDOW_TAP_BASE","CHARACTER_WINDOW_TAP_INACTIVE",
            "CHARACTER_WINDOW_TAP_BASE");
        RegisterControl ( pSkillTab0 );
        RegisterTapButton( pSkillTab0 );

        //텍스트
        CBasicTextBox* pStaticText = new CBasicTextBox(m_pEngineDevice);
        pStaticText->CreateSub( this, "RNSKILL_TAB_1" );
        pStaticText->SetFont( pFont9 );
        pStaticText->SetTextAlign( TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );
        RegisterControl( pStaticText );
        m_pSkillPageText[0] = pStaticText;
    }

    {      
        // 스킬텝2
        STAPBUTTON* pSkillTab1 = new STAPBUTTON(m_pEngineDevice);
        pSkillTab1->CreateSub ( this, "RNSKILL_TAB_2", UI_FLAG_DEFAULT, ID_RNSKILL_TAB_1 );
        pSkillTab1->CreateSubControl( " " ,
            "CHARACTER_WINDOW_TAP_BASE","CHARACTER_WINDOW_TAP_ACTIVE",
            "CHARACTER_WINDOW_TAP_BASE","CHARACTER_WINDOW_TAP_INACTIVE",
            "CHARACTER_WINDOW_TAP_BASE");
        RegisterControl ( pSkillTab1 );
        RegisterTapButton( pSkillTab1 );

        //텍스트
        CBasicTextBox* pStaticText = new CBasicTextBox(m_pEngineDevice);
        pStaticText->CreateSub( this, "RNSKILL_TAB_2" );
        pStaticText->SetFont( pFont9 );
        pStaticText->SetTextAlign( TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );
        RegisterControl( pStaticText );
        m_pSkillPageText[1] = pStaticText;
    }

    {           
        // 스킬텝3
        STAPBUTTON* pSkillTab2 = new STAPBUTTON(m_pEngineDevice);
        pSkillTab2->CreateSub ( this, "RNSKILL_TAB_3", UI_FLAG_DEFAULT, ID_RNSKILL_TAB_2 );
        pSkillTab2->CreateSubControl( " " ,
            "CHARACTER_WINDOW_TAP_BASE","CHARACTER_WINDOW_TAP_ACTIVE",
            "CHARACTER_WINDOW_TAP_BASE","CHARACTER_WINDOW_TAP_INACTIVE",
            "CHARACTER_WINDOW_TAP_BASE");
        RegisterControl ( pSkillTab2 );
        RegisterTapButton( pSkillTab2 );

        //텍스트
        CBasicTextBox* pStaticText = new CBasicTextBox(m_pEngineDevice);
        pStaticText->CreateSub( this, "RNSKILL_TAB_3" );
        pStaticText->SetFont( pFont9 );
        pStaticText->SetTextAlign( TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );
        RegisterControl( pStaticText );
        m_pSkillPageText[2] = pStaticText;
    }

    {   
        // 스킬텝4
        STAPBUTTON* pSkillTab3 = new STAPBUTTON(m_pEngineDevice);
        pSkillTab3->CreateSub ( this, "RNSKILL_TAB_4", UI_FLAG_DEFAULT, ID_RNSKILL_TAB_3 );
        pSkillTab3->CreateSubControl( " " ,
            "CHARACTER_WINDOW_TAP_BASE","CHARACTER_WINDOW_TAP_ACTIVE",
            "CHARACTER_WINDOW_TAP_BASE","CHARACTER_WINDOW_TAP_INACTIVE",
            "CHARACTER_WINDOW_TAP_BASE");
        RegisterControl ( pSkillTab3 );
        RegisterTapButton( pSkillTab3 );

        //텍스트
        CBasicTextBox* pStaticText = new CBasicTextBox(m_pEngineDevice);
        pStaticText->CreateSub( this, "RNSKILL_TAB_4" );
        pStaticText->SetFont( pFont9 );
        pStaticText->SetTextAlign( TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );
        RegisterControl( pStaticText );
        m_pSkillPageText[3] = pStaticText;
    }

    {   
        // 도움말 아이콘
        RnButton::CreateArg arg;
        //////////////////////////////////////////////////////////////////////////
        arg.defaultTextureKey = "POINT_SHOP_HELP_BUTTON";
        arg.mouseOverTextureKey = "POINT_SHOP_HELP_BUTTON_F";
        arg.mouseClickTextureKey = "POINT_SHOP_HELP_BUTTON";
        //////////////////////////////////////////////////////////////////////////
        arg.dwColor = NS_UITEXTCOLOR::WHITE;

        RnButton* pHelpButton = new RnButton(m_pEngineDevice);
        pHelpButton->CreateSub(this, "RNSKILL_HELP_POS", UI_FLAG_DEFAULT, ID_RNSKILL_HELP_BUTTON );
        pHelpButton->CreateSubControl(arg);
        RegisterControl(pHelpButton);

    }

    // 스크롤바
    {
        CBasicScrollBarEx* pScrollBar = new CBasicScrollBarEx(m_pEngineDevice);
        pScrollBar->CreateSub( this, "BASIC_SCROLLBAR", UI_FLAG_RIGHT | UI_FLAG_YSIZE, ID_RNSKILL_SCROOLBAR );
        pScrollBar->CreateBaseScrollBar( "RNSKILL_SKILL_LIST_SCROOL" );
        //pScrollBar->GetThumbFrame()->SetScrollParent( m_pTextBox );
        RegisterControl ( pScrollBar );
        m_pScrollBar = pScrollBar;
    }

    {
        // 스킬 포인트 영역
        CBasicLineBoxSmart* pLinBoxSmart = new CBasicLineBoxSmart( m_pEngineDevice );
        pLinBoxSmart->CreateSub( this, "RNSKILL_SKILL_POINT_REGION", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pLinBoxSmart->CreateSubControl( "PRODUCT_WINDOW_BASE_SMARTLINEBOX_GRAY" );
        RegisterControl( pLinBoxSmart );

        //텍스트
        CBasicTextBox* pStaticText = new CBasicTextBox(m_pEngineDevice);
        pStaticText->CreateSub( this, "RNSKILL_SKILL_POINT_REGION" );
        pStaticText->SetFont( pFont9 );
        pStaticText->SetTextAlign( TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );
        RegisterControl( pStaticText );
        m_pSkillPoint = pStaticText;
    }


    //테두리[노란선]
    m_pYLine = new CBasicLineBoxSmart( m_pEngineDevice );
    m_pYLine->CreateSub( this, "RNSKILL_SLOT_SIZE", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
    m_pYLine->CreateSubControl( "LINEBOX_Y" );
    m_pYLine->SetVisibleSingle( FALSE );
    RegisterControl( m_pYLine );

    //테두리[녹색선]
    m_pGLine = new CBasicLineBoxSmart( m_pEngineDevice );
    m_pGLine->CreateSub( this, "RNSKILL_SLOT_SIZE", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
    m_pGLine->CreateSubControl( "LINEBOX_G" );
    m_pGLine->SetVisibleSingle( FALSE );
    RegisterControl( m_pGLine );

    //테두리[파란선]
    m_pBLine = new CBasicLineBoxSmart( m_pEngineDevice );
    m_pBLine->CreateSub( this, "RNSKILL_SLOT_SIZE", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
    m_pBLine->CreateSubControl( "LINEBOX_B" );
    m_pBLine->SetVisibleSingle( FALSE );
    RegisterControl( m_pBLine );

    CD3DFontPar* pFont10Shadow = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 10, _DEFAULT_FONT_SHADOW_FLAG );

    RnButton::CreateArg arg;
    //////////////////////////////////////////////////////////////////////////
    arg.defaultTextureKey = "RNBUTTON_DEFAULT_TEXTURE";
    arg.mouseOverTextureKey = "RNBUTTON_OVER_TEXTURE";
    arg.mouseClickTextureKey = "RNBUTTON_CLICK_TEXTURE";
    //////////////////////////////////////////////////////////////////////////
    arg.pFont = pFont10Shadow;
    arg.dwColor = NS_UITEXTCOLOR::WHITE;

    // 초기화 버튼
    CString strText;
    strText.Format( "%s%s", "   ", ID2GAMEWORD_RESET );
    arg.text = strText.GetString();
    RnButton* pReset = new RnButton(m_pEngineDevice);
    pReset->CreateSub(this, "RNSKILL_SKILL_RESET_BUTTON", UI_FLAG_DEFAULT, ID_RNSKILL_RESET_BUTTON );
    pReset->CreateSubControl(arg);
    RegisterControl(pReset);
    m_pButtonReset = pReset;
    
    // 적용 버튼
    strText.Format( "%s%s", "   ", ID2GAMEWORD_CONFIRM );
    arg.text = strText.GetString();
    RnButton* pConfirm = new RnButton(m_pEngineDevice);
    pConfirm->CreateSub(this, "RNSKILL_SKILL_CONFIRM_BUTTON", UI_FLAG_DEFAULT, ID_RNSKILL_CONFIRM_BUTTON );
    pConfirm->CreateSubControl(arg);
    RegisterControl(pConfirm);
    m_pButtonConfirm = pConfirm;

    // 초기화 툴팁
    m_pResetToolTip = new RnSkillNomalToolTip( m_pInterface, m_pEngineDevice );
    m_pResetToolTip->CreateSub( this, "RNSKILL_SKILL_RESET_TOOLTIP", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	m_pResetToolTip->CreateSubControl( "RNSKILL_SKILL_RESET_TOOLTIP_SIZE", "RNSKILL_SKILL_RESET_TOOLTIP_TEXTBOX_SIZE" );
    m_pResetToolTip->SetVisibleSingle( FALSE );
    RegisterControl( m_pResetToolTip );

    // 적용 툴팁
    m_pConfirmToolTip = new RnSkillNomalToolTip( m_pInterface, m_pEngineDevice );
    m_pConfirmToolTip->CreateSub( this, "RNSKILL_SKILL_CONFIRM_TOOLTIP", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	m_pConfirmToolTip->CreateSubControl( "RNSKILL_SKILL_CONFIRM_TOOLTIP_SIZE", "RNSKILL_SKILL_CONFIRM_TOOLTIP_TEXTBOX_SIZE" );
    m_pConfirmToolTip->SetVisibleSingle( FALSE );
    RegisterControl( m_pConfirmToolTip );

    // 확인 툴팁
    m_pCheckToolTip = new RnSkillNomalToolTip( m_pInterface, m_pEngineDevice );
    m_pCheckToolTip->CreateSub( this, "RNSKILL_SKILL_CHECK_TOOLTIP", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	m_pCheckToolTip->CreateSubControl( "RNSKILL_SKILL_CHECK_TOOLTIP", "RNSKILL_SKILL_CHECK_TOOLTIP_TEXTBOX_SIZE" );
    m_pCheckToolTip->SetVisibleSingle( FALSE );
    RegisterControl( m_pCheckToolTip );

    // 도움말 툴팁
    m_HelpTooltip = new RnSkillHelpToolTip( m_pInterface, m_pEngineDevice );
    m_HelpTooltip->CreateSub( this, "RNSKILL_HELP_TOOLTIP", UI_FLAG_XSIZE | UI_FLAG_YSIZE  );
    m_HelpTooltip->CreateSubControl();
    m_HelpTooltip->SetVisibleSingle( FALSE );
    RegisterControl( m_HelpTooltip );

    CMessageBox* pMessageBox = new CMessageBox( m_pEngineDevice );
    pMessageBox->CreateSub( this, "UTILE_MESSAGEBOX_SIZE", UI_FLAG_XSIZE | UI_FLAG_YSIZE, ID_RNSKILL_MESSAGEBOX );
    pMessageBox->CreateSubControl();
    RegisterControl( pMessageBox );
    m_pMessageBox = pMessageBox;
}


void RnSkillWindow::Refresh()
{
    InitSkillPage();
    ResetTabButtonText();
}

void RnSkillWindow::SetVisibleSingle( BOOL bVisible )
{
    CUIWindowEx::SetVisibleSingle( bVisible );
    if( bVisible == FALSE )
        m_pMessageBox->CloseMessageBox();    
}

std::vector<SNATIVEID> RnSkillWindow::GetAllSkillID()
{
    std::vector<SNATIVEID> vecSkillID;

    BOOST_FOREACH( const SkillInfoVec& vecInfo, m_vecSkillTab )
    {
        BOOST_FOREACH( const SKILLINFO& sSkillInfo, vecInfo )
        {
            if( sSkillInfo.pSkill == NULL )
                continue;

            DWORD dwTempSkillLevel = sSkillInfo.dwTempSkillLevel;
            if( 0 < dwTempSkillLevel )
            {
                for( unsigned int i=0 ; i<dwTempSkillLevel ; i++)
                    vecSkillID.push_back( sSkillInfo.pSkill->GetId() );
            }
            
            
        }
    }
 
    return vecSkillID;
}


void RnSkillWindow::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
    if( m_bInit == FALSE )
        return;

    if( m_pMessageBox->IsVisible() == TRUE )
        return;


    TranslateMessageBox( m_pMessageBox->GetIdentifier(), dwMsg);

    EMTAPSELECTORRESULT emResult = EMTAPSELECTORRESULT_NONE;
    TapSelectorUIMessage( ControlID, dwMsg, emResult );
    CUIWindowEx::TranslateUIMessage ( ControlID, dwMsg );

    if( ID_SKILL_SLOT_RECT_CONTROL_START <= ControlID && ControlID <= ID_SKILL_SLOT_RECT_CONTROL_END )
    {

        if( CHECK_MOUSE_IN ( dwMsg ) )
        {
            int nIndex = ControlID - ID_SKILL_SLOT_RECT_CONTROL_START;

            if( nIndex < SKILLSLOT_COUNT )
            {
                m_pCurrentLine = GetLineBox( nIndex );
				if ( m_pCurrentLine )
				{
					m_pCurrentLine->SetVisibleSingle(TRUE);
					m_pCurrentLine->SetGlobalPos( m_SkillSlot[nIndex]->GetGlobalPos() );
					m_pCurrentLine->SetLocalPos( m_SkillSlot[nIndex]->GetLocalPos() );
				}
            }

            m_pRollOverSlot = m_SkillSlot[nIndex];

            // 실제 데이터 처리
            int nCurrentIndex =  nIndex+m_nViewPos;

            SkillInfoVec& sSKInfo = GetCurrentSkillInfoVec();
            if( static_cast<unsigned int>( nCurrentIndex ) < sSKInfo.size() )
            {
                SKILLINFO& pSkillInfo = sSKInfo[ nCurrentIndex ];

                //메시지 처리
                // + 버튼을 클릭 메세지
                if ( dwMsg & UIMSG_RNSKILL_UP_CLICK )
                {
                    if( IsLevelUP( nCurrentIndex ) == TRUE )
                        pSkillInfo.dwTempSkillLevel++;

                }
                // - 버튼을 클릭 메세지
                if ( dwMsg & UIMSG_RNSKILL_DOWN_CLICK )
                {
                    if( IsLevelDown( nCurrentIndex ) == TRUE )
                        pSkillInfo.dwTempSkillLevel--;
                }
            }// if

        }// if( CHECK_MOUSE_IN ( dwMsg ) )
    }

    // 마우스 커서가 밖으로 나간다면 영역을 해제해야 한다.
    if( ControlID == ID_RNSKILL_TAB )
    {
        if( CHECK_MOUSE_IN ( dwMsg ) == FALSE )
        {
            if(m_pCurrentLine != FALSE )
                m_pCurrentLine->SetVisibleSingle( FALSE );

            m_pRollOverSlot = NULL;
        }

    }

    switch( ControlID )
    {
    case ID_RNSKILL_RESET_BUTTON:
        if( CHECK_MOUSE_IN ( dwMsg ) )
        {
            SetResetToolTip();
            if( CHECK_LB_UP_LIKE ( dwMsg ))
            {
                SetCheckToolTip();
                Reset();
            }
        }
        break;
    case ID_RNSKILL_CONFIRM_BUTTON:
        if( CHECK_MOUSE_IN ( dwMsg ) )
        {
            SetConfirmToolTip();
            GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
            if( CHECK_LB_UP_LIKE ( dwMsg ) && pCharacter != NULL )
            {
                DWORD dwUseMoney = GetUsingSkillMoneyAll();
                if( GetUsingSkillMoneyAll() <= pCharacter->GetInvenMoney() )
                {
                    m_pMessageBox->OpenMessageBox( ID2GAMEWORD("MODAL_TITLE", 1)
                        , sc::string::format( ID2GAMEINTEXT("RNSKILLWINDOW_CONFIRM"), 
											NS_UITEXTCONTROL::MAKE_MONEY_FORMAT( dwUseMoney, 3, "," ) )
                        , CMessageBox::TYPE_YESNO, ID_RNSKILL_CONFIRM_BUTTON );
                }  	
                else
                {
                    m_pMessageBox->OpenMessageBox( CMessageBox::TITLE_ANNOUNCEMENT
                        , sc::string::format( ID2GAMEINTEXT("RNSKILLWINDOW_CONFIRM_ENOUGH"), 
											NS_UITEXTCONTROL::MAKE_MONEY_FORMAT( dwUseMoney, 3, "," ) )
                        , CMessageBox::TYPE_OK );
                }
            }
                
        }
        break;

    case ID_RNSKILL_HELP_BUTTON:
        if( m_HelpTooltip->IsVisible() == TRUE)
            m_HelpTooltip->SetVisibleSingle( FALSE );

        if( CHECK_MOUSE_IN ( dwMsg ) )
        {
            m_HelpTooltip->SetVisibleSingle( TRUE );
        }
        break;

    }


}

void RnSkillWindow::TranslateMessageBox( unsigned int nIdentity, DWORD dwMsg )
{

    switch( nIdentity )
    {
    case ID_RNSKILL_CONFIRM_BUTTON:
        if( dwMsg & UIMSG_MESSAGEBOX_POSITIVE )
        {
            m_pGaeaClient->GetCharacter()->ReqRnSkillUp( GetAllSkillID() );
        }
        break;
    }

}

void RnSkillWindow::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    CUIWindowEx::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

    if( m_bInit == FALSE )
    {
        m_bInit = TRUE;
        InitSkillPage();
        EventSelectedTap( ID_RNSKILL_TAB_0 );
        ResetTabButtonText();
        return;
    }

    UINT nTotal = static_cast< UINT >( GetCurrentSkillInfoVec().size() );
    int nVisibleLine = SKILLSLOT_COUNT;

    CBasicScrollThumbFrame* const pThumbFrame = m_pScrollBar->GetThumbFrame();

    const int nMovableLine = nTotal - nVisibleLine;
    float fPercent = pThumbFrame->GetPercent();

    m_nViewPos = (int)floor( fPercent * nMovableLine );
    ( m_nViewPos%2 != 0 ? m_nViewPos++ : m_nViewPos );

    if( m_nViewPos < 0 )
        m_nViewPos = 0;

    for( int i=0; i<nVisibleLine; ++i )
    {
        UINT nViewDataPos = m_nViewPos + i;
        if( nViewDataPos < nTotal )
        {

            SKILLINFO* pSkillInfo = GetSkillInfo( nViewDataPos );
            if( pSkillInfo == NULL )
                continue;

            BOOL bRollOver = FALSE;
            if( m_pRollOverSlot == m_SkillSlot[ i ] )
                bRollOver = TRUE;

            m_SkillSlot[ i ]->SetSkillInfo(	pSkillInfo, bRollOver );
            m_SkillSlot[ i ]->SetVisibleSingle( TRUE );

        } 
        else
        {

            m_SkillSlot[ i ]->SetVisibleSingle( FALSE );
        }
    }


    if ( m_pSkillPoint )
    {
        const DWORD wSkillPoint = GetUsingSkillPointAll();
        const DWORD dwSkillPoint = static_cast<DWORD>( m_pGaeaClient->GetCharacter ()->m_dwSkillPoint ) - wSkillPoint;

        CString strTemp;
        strTemp.Format( "%s: %3d", (char*)ID2GAMEWORD( "QUEST_GIFT", 5), dwSkillPoint  );		
        m_pSkillPoint->SetText( strTemp );
        m_dwSkillPointBACK = dwSkillPoint;		
    }


    //버튼 이름 설정
    CString strText;
    strText.Format( "%2d %s", GetUsingSkillPointPage( m_nCurrentPage ), ID2GAMEWORD_RESET );
    m_pButtonReset->SetText( strText, NS_UITEXTCOLOR::WHITE );

    strText.Format( "%3d %s", GetUsingSkillPointAll(), ID2GAMEWORD_CONFIRM );
    m_pButtonConfirm->SetText( strText, NS_UITEXTCOLOR::WHITE );
}


CBasicLineBoxSmart* RnSkillWindow::GetLineBox( unsigned int nIndex )
{
    m_pYLine->SetVisibleSingle(FALSE);
    m_pBLine->SetVisibleSingle(FALSE);
    m_pGLine->SetVisibleSingle(FALSE);


    const SkillInfoVec& sSKInfo = GetCurrentSkillInfoVec();
	if ( nIndex < 0 || nIndex >= sSKInfo.size() )
		return NULL;

    switch( sSKInfo[ nIndex ].nState )
    {
    case CRnSkillSlot::STATE_IMPOSSIBLE:
        return m_pYLine;

    case CRnSkillSlot::STATE_ACTIVATE_TEMP:
    case CRnSkillSlot::STATE_ACTIVATE:
        return m_pBLine;

    case CRnSkillSlot::STATE_DEACTIVATE_TEMP:
    case CRnSkillSlot::STATE_DEACTIVATE:
        return m_pGLine;

    default:
        GASSERT( 0 && "상태값이 이상합니다.");
        return 0;
    }

}


void RnSkillWindow::EventSelectedTap( UIGUID controlID )
{
    if( !m_bInit )
        return;

    if( ID_RNSKILL_TAB_0 <= controlID &&  controlID <= ID_RNSKILL_TAB_3 )
    {
        UINT nPage = controlID - ID_RNSKILL_TAB_0;
        m_pScrollBar->GetThumbFrame()->SetPercent( 0 );

        // 현재 페이지를 선택
        m_nCurrentPage = nPage;

        // 스크롤 최대 크기 설정
        int nMAX = static_cast< UINT >( GetCurrentSkillInfoVec().size() );
        m_pScrollBar->GetThumbFrame()->SetState( nMAX, SKILLSLOT_COUNT );

    }

    // 탭버튼 텍스트 리셋
    ResetTabButtonText();
    
    // 툴팁 안보이게 설정
    if( m_pResetToolTip != NULL )
        m_pResetToolTip->SetVisibleSingle( FALSE );
    if( m_pConfirmToolTip != NULL )
        m_pConfirmToolTip->SetVisibleSingle( FALSE );
    if( m_pCheckToolTip != NULL )
        m_pCheckToolTip->SetVisibleSingle( FALSE );

    
}


void RnSkillWindow::SetResetToolTip()
{
    if( m_pResetToolTip == NULL )
        return;

    BOOST_FOREACH( SKILLINFO& sSkillInfo, GetCurrentSkillInfoVec() )
    {
        if( 0 < sSkillInfo.dwTempSkillLevel )
        {
            m_pResetToolTip->ClearText();

            CString strTapName;
            strTapName.Format( "'%s' %s", GetClassText( m_nCurrentPage ), ID2GAMEINTEXT("RNSKILLWINDOW_RESET_TOOLTIP") );
            m_pResetToolTip->AddText( strTapName, NS_UITEXTCOLOR::YELLOW );
            m_pResetToolTip->SetVisibleSingle( TRUE );
            return;
        }
    }

}


void RnSkillWindow::SetConfirmToolTip()
{
    if( m_pConfirmToolTip == NULL )
        return;

    CString strTemp;
    strTemp.Format( "'%s'", GetClassText( 0 ) );
    for( int i=1; i < TAB_COUNT; i++ )
    {
        strTemp.AppendFormat( ", '%s'", GetClassText( i ) );
    }
   
    m_pConfirmToolTip->ClearText();

    strTemp.Append( ID2GAMEINTEXT("RNSKILLWINDOW_CONFIRM_TOOLTIP" ) );
    m_pConfirmToolTip->AddText( strTemp, NS_UITEXTCOLOR::YELLOW );
    m_pConfirmToolTip->SetVisibleSingle( TRUE );
}


void RnSkillWindow::SetCheckToolTip()
{
    if( m_pCheckToolTip == NULL )
        return;

    BOOST_FOREACH( SKILLINFO& sSkillInfo, GetCurrentSkillInfoVec() )
    {
        if( 0 < sSkillInfo.dwTempSkillLevel )
            return;
    }

	//  [11/21/2012 gbgim] 
	// - 툴팁의 기준이되는 버튼의 위치를 읽어와야 하는데 다른 컴포넌트의 위치를 일고 있어;
	//   아래의 소스를 다음과 같이 수정;
    //UIRECT rcUIPOS = m_pSkillPoint->GetGlobalPos(); 원본
	UIRECT rcUIPOS = m_pButtonReset->GetGlobalPos();
    float nMyYSize =  m_pCheckToolTip->GetGlobalPos().sizeY ;

    D3DXVECTOR2 vPos( rcUIPOS.left, rcUIPOS.top - nMyYSize - 10 );
    m_pCheckToolTip->SetGlobalPos( vPos );

    // 슬롯 상태를 읽어서
//     for( int nIndex = 0 ; nIndex < SKILLSLOT_COUNT ; nIndex++ )
//     {
//         if( m_SkillSlot[nIndex]->GetSkillInfo().nState == CRnSkillSlot::STATE_DEACTIVATE 
//             || m_SkillSlot[nIndex]->GetSkillInfo().nState == CRnSkillSlot::STATE_ACTIVATE )
//         {
//             if( m_SkillSlot[nIndex]->GetSkillInfo().bMaster == TRUE )
//                 break;
// 
//             vPos.x = m_SkillSlot[nIndex]->GetGlobalPos().left;
//             vPos.y = m_SkillSlot[nIndex]->GetGlobalPos().top - nMyYSize - 10;
//             //m_pCheckToolTip->SetGlobalPos( vPos );
//             break;
//         }
//     }
    
    m_pCheckToolTip->ClearText();
    m_pCheckToolTip->AddText( ID2GAMEINTEXT("RNSKILLWINDOW_CHECK_TOOLTIP"), NS_UITEXTCOLOR::YELLOW );
    m_pCheckToolTip->SetVisibleSingle( TRUE );
}


void RnSkillWindow::ResetTabButtonText()
{
    
    for( UINT i=0; i<TAB_COUNT; ++i )
    {
        int nSkillPoint = GetSkillPoint( m_vecSkillTab[i] );
        //nSkillPointMax = max( nSkillPointMax, nSkillPoint[ i ] );
        DWORD nColor = NS_UITEXTCOLOR::WHITE;

        if( i == m_nCurrentPage )
            nColor = D3DCOLOR_XRGB( 0xE9, 0xCE, 0x00 );

        CString strTapName;
        
        strTapName.Format("%s(%d)", GetClassText( i ), nSkillPoint );

        m_pSkillPageText[i]->ClearText();
        m_pSkillPageText[i]->SetText( strTapName, nColor );
    }


       
}

void RnSkillWindow::Reset()
{
    BOOST_FOREACH( SKILLINFO& sSkillInfo, GetCurrentSkillInfoVec() )
    {
        sSkillInfo.dwTempSkillLevel = 0;
    }
    ResetTabButtonText();
}


SkillInfoVec& RnSkillWindow::GetCurrentSkillInfoVec()
{
    if( m_nCurrentPage < 0 || TAB_COUNT <= m_nCurrentPage )
        GASSERT( 0 && "잘못된 현재 페이지");

    // 현재 페이지를 선택
    return m_vecSkillTab[m_nCurrentPage];
}


SKILLINFO* RnSkillWindow::GetLearnSkill( SNATIVEID skill_id )
{
    //	Note : 익힌 스킬인지 검사. ( 임시로 찍은 스킬도 포함된다 )
    BOOST_FOREACH( SKILLINFO& sSkillInfo, GetCurrentSkillInfoVec() )
    {
        if( sSkillInfo.pSkill->GetId() == skill_id )
            return &sSkillInfo;
    }
    return NULL;
}

SkillInfoVec& RnSkillWindow::GetCurrentSkillInfoVec( int iPage )
{
	if( iPage < 0 || TAB_COUNT <= iPage )
		GASSERT( 0 && "잘못된 현재 페이지");

	// 현재 페이지를 선택
	return m_vecSkillTab[iPage];
}

SKILLINFO* RnSkillWindow::GetLearnSkillAll( SNATIVEID skill_id )
{
	for( int i = 0; i < TAB_COUNT; ++i)
	{
		BOOST_FOREACH( SKILLINFO& sSkillInfo, GetCurrentSkillInfoVec(i) )
		{
			if( sSkillInfo.pSkill->GetId() == skill_id )
				return &sSkillInfo;
		}
	}

	return NULL;
}

SKILLINFO* RnSkillWindow::GetSkillInfo( unsigned int nIndex )
{
    if( GetCurrentSkillInfoVec().size() <= nIndex)
        return NULL;

    GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
    SkillInfoVec& sSKInfo = GetCurrentSkillInfoVec();
    if ( pCharacter == NULL || sSKInfo[ nIndex ].pSkill == NULL )
        return NULL;

    BOOL bMaster = FALSE;
    int nState = 0;
    DWORD dwUsingSkillPoint = 0;
    PGLSKILL pSkill = sSKInfo[ nIndex ].pSkill;
    SKILL_NEED sNeed = {0};

	// 테스트 코드 브레이크용
	if( pSkill->GetId() == SNATIVEID(39, 13) )
	{
		int a = 0;

		a = 1;
	}

    // 스킬을 마스터 했는가?
    bMaster = IsMaster( nIndex );

    // 사용된 스킬 포인트
    dwUsingSkillPoint = GetUsingSkillPoint( nIndex );

    // 선행 조건 입력
    sNeed = CheckSkillPrecedent( nIndex );

    // 임시로 스킬포인트를 상승시킨다면
    int nTempActive = 0;    
    if( 0 < sSKInfo[ nIndex ].dwTempSkillLevel )
        nTempActive = CRnSkillSlot::STATE_TEMP;

//     // 스킬 레벨 입력
//     SCHARSKILL* pCharSkill = pCharacter->GETLEARNED_SKILL( pSkill->GetId() );
//     if( pCharSkill != NULL )
//         sSKInfo[ nIndex ].dwLevel = pCharSkill->wLevel + 1;

    // 상태 결정
    if( IsLevelUP( nIndex ) || bMaster == TRUE )
    {
        if( sSKInfo[ nIndex ].dwLevel == 0 )
            nState = CRnSkillSlot::STATE_DEACTIVATE + nTempActive;
        else
            nState = CRnSkillSlot::STATE_ACTIVATE + nTempActive;
    }
    else
        nState = CRnSkillSlot::STATE_IMPOSSIBLE;

    sSKInfo[ nIndex ].bMaster = bMaster;
    sSKInfo[ nIndex ].nState = nState;
    sSKInfo[ nIndex ].sNeed = sNeed;
    sSKInfo[ nIndex ].dwUsingSkillPoint = dwUsingSkillPoint;

    return &sSKInfo[ nIndex ];
}


DWORD RnSkillWindow::GetUsingSkillPoint( unsigned int nIndex )
{
    const SkillInfoVec& vecSKInfo = GetCurrentSkillInfoVec();

    if( vecSKInfo.size() < nIndex )
        return 0;

    return GetUsingSkillPoint( vecSKInfo[nIndex] );
}

DWORD RnSkillWindow::GetUsingSkillPoint( const SKILLINFO& sSkillInfo )
{
    const PGLSKILL& pSkill = sSkillInfo.pSkill;
    SKILL::SLEARN& sLEARN = pSkill->m_sLEARN;

    DWORD dwLevel = sSkillInfo.dwLevel;
    DWORD dwTempSkillLevel = sSkillInfo.dwTempSkillLevel;
    DWORD dwUsingSkillPoint = 0;

    for( unsigned int nLevel=0 ; nLevel < dwLevel + dwTempSkillLevel ; nLevel++ )
    {
        dwUsingSkillPoint += sLEARN.sLVL_STEP[nLevel].dwSKP;
    }

    return dwUsingSkillPoint;
}

DWORD RnSkillWindow::GetUsingSkillPointPage( unsigned int nPage )
{
    DWORD dwPageUsingSkillPoint = 0;

    const SkillInfoVec& vecInfo = GetCurrentSkillInfoVec();
    
    BOOST_FOREACH( const SKILLINFO& sSkillInfo, vecInfo )
    {
        SKILL::SLEARN& sLEARN = sSkillInfo.pSkill->m_sLEARN;

        DWORD dwLevel = sSkillInfo.dwLevel;
        DWORD dwTempSkillLevel = sSkillInfo.dwTempSkillLevel;

        for( unsigned int nLevel=dwLevel ; nLevel < dwLevel + dwTempSkillLevel ; nLevel++ )
        {
            dwPageUsingSkillPoint += sLEARN.sLVL_STEP[nLevel].dwSKP;
        }

    }

    return dwPageUsingSkillPoint;
}



DWORD RnSkillWindow::GetUsingSkillPointAll()
{
    DWORD dwAllUsingSkillPoint = 0;

    BOOST_FOREACH( const SkillInfoVec& vecInfo, m_vecSkillTab )
    {
        BOOST_FOREACH( const SKILLINFO& sSkillInfo, vecInfo )
        {
            SKILL::SLEARN& sLEARN = sSkillInfo.pSkill->m_sLEARN;

            DWORD dwLevel = sSkillInfo.dwLevel;
            DWORD dwTempSkillLevel = sSkillInfo.dwTempSkillLevel;

            for( unsigned int nLevel=dwLevel ; nLevel < dwLevel + dwTempSkillLevel ; nLevel++ )
            {
                dwAllUsingSkillPoint += sLEARN.sLVL_STEP[nLevel].dwSKP;
            }

        }
    }

    return dwAllUsingSkillPoint;
}


DWORD RnSkillWindow::GetUsingSkillMoneyAll()
{
    DWORD dwUsingAllMoney = 0;
    BOOST_FOREACH( const SkillInfoVec& vecInfo, m_vecSkillTab )
    {
        BOOST_FOREACH( const SKILLINFO& sSkillInfo, vecInfo )
        {
            if( sSkillInfo.pSkill == NULL )
                continue;

            // 잘못된 SKILL ID
            if( sSkillInfo.pSkill->GetId() == NATIVEID_NULL() )
                continue;

            if( sSkillInfo.dwLevel == 0 && 0 < sSkillInfo.dwTempSkillLevel )
            {
                // Note : 골드소모량을 가지고 온다.
                DWORD dwUsingMoney = GLogicData::GetInstance().GetRnSkillUseMoney( sSkillInfo.pSkill->GetId() );

                if( 0 < dwUsingMoney )
                    dwUsingAllMoney += dwUsingMoney;

            }
        }
    }

    return dwUsingAllMoney;

}


BOOL RnSkillWindow::IsMaster( unsigned int nIndex )
{
    const SkillInfoVec& sSKInfo = GetCurrentSkillInfoVec();
    const PGLSKILL& pSkill = sSKInfo[ nIndex ].pSkill;
    DWORD dwLevel = sSKInfo[ nIndex ].dwLevel;
    DWORD dwTempSkillLevel = sSKInfo[ nIndex ].dwTempSkillLevel;

    if( static_cast<DWORD>( dwLevel + dwTempSkillLevel ) >= SKILL::MAX_LEVEL )
        return TRUE;
    if( static_cast<DWORD>( dwLevel + dwTempSkillLevel ) >= pSkill->m_sBASIC.dwMAXLEVEL )
        return TRUE;

    return FALSE;
}


BOOL RnSkillWindow::IsLevelUP( unsigned int nIndex )
{
    const SkillInfoVec& sSKInfo = GetCurrentSkillInfoVec();
    const SKILL_NEED &sNeed = sSKInfo[ nIndex ].sNeed;
    const SNATIVEID sSkillID = sSKInfo[nIndex].pSkill->GetId();
  
    if( IsMaster( nIndex ) == TRUE)
        return FALSE;

    if( 0 < sNeed.dwLevel )
        return FALSE;
    else if( sNeed.bPrecedentSkill == TRUE )
        return FALSE;
    else if( 0 < sNeed.dwSkillPoint )
        return FALSE;
    else if( 0 < sNeed.dwPow )
        return FALSE;
    else if( 0 < sNeed.dwDex )
        return FALSE;
    else if( 0 < sNeed.dwSpi )
        return FALSE;
    else if( 0 < sNeed.dwStr )
        return FALSE;
    else if( 0 < sNeed.dwSta )
        return FALSE;
    else if( sNeed.bUseScrool )
        return FALSE;

    return TRUE;
}

BOOL RnSkillWindow::IsLevelDown( unsigned int nIndex )
{
    const SkillInfoVec& sSKInfo = GetCurrentSkillInfoVec();
    const DWORD dwTempSkillLevel = sSKInfo[ nIndex ].dwTempSkillLevel;
    if( dwTempSkillLevel <= 0 )
        return FALSE;

    if( sSKInfo[ nIndex ].pSkill == NULL)
        return FALSE;

    //내스킬, 레벨
    PGLSKILL pCurrentSkill =  sSKInfo[ nIndex ].pSkill;
    DWORD dwCurrentSkillLevel = sSKInfo[ nIndex ].dwLevel + sSKInfo[ nIndex ].dwTempSkillLevel;

    // 선행스킬로 사용되었는지 여부, 현재 스킬이 선행 스킬이었다면 스킬레벨을 낮출 수 없다.
    for( unsigned int nCnt = 0 ; nCnt < GetCurrentSkillInfoVec().size() ; nCnt++ )
    {
        PGLSKILL pSearchSkill = sSKInfo[ nCnt ].pSkill;
        DWORD dwSearchLevel = sSKInfo[ nCnt ].dwLevel + sSKInfo[ nCnt ].dwTempSkillLevel;
        if( pSearchSkill == pCurrentSkill || sSKInfo[nCnt].pSkill == NULL)
            continue;

        if( pCurrentSkill->GetId() == pSearchSkill->m_sLEARN.sSKILL && 0 < dwSearchLevel )
        {
            
            
            DWORD dwNeedLevel = pSearchSkill->m_sLEARN.sLVL_STEP[ dwSearchLevel-1 ].dwSKILL_LVL + 1;
           
            if( dwCurrentSkillLevel <= dwNeedLevel )
                return FALSE;
        }
    }

    return TRUE;
}


SKILL_NEED RnSkillWindow::CheckSkillPrecedent( unsigned int nIndex )
{
    GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
    SKILL_NEED sNeed = {0}; 

    if( !pCharacter )
        return sNeed;

    const SkillInfoVec& sSKInfo = GetCurrentSkillInfoVec();
    const PGLSKILL pSkill = sSKInfo[ nIndex ].pSkill;
    DWORD dwTempSkillLevel = sSKInfo[ nIndex ].dwTempSkillLevel;
    DWORD dwLevel = sSKInfo[ nIndex ].dwLevel;


    // 선행 조건
    SKILL::SLEARN& sLEARN = pSkill->m_sLEARN;
    SKILL::SLEARN_LVL& sLVL = sLEARN.sLVL_STEP[ dwLevel + dwTempSkillLevel ];

	const SKILLINFO* pNeedSkill = NULL;
	// 일단 임시
	if( m_nCurrentPage == 3 )
	{
		pNeedSkill = GetLearnSkillAll( sLEARN.sSKILL );
	}
	else
	{
		pNeedSkill = GetLearnSkill( sLEARN.sSKILL );
	}
    // 선행 스킬을 배웠는가? (임시 포함)
    //const SKILLINFO* pNeedSkill = GetLearnSkill( sLEARN.sSKILL );

	if( pNeedSkill != NULL )
    {
        // 스킬 레벨
        DWORD dwSkillLevel = pNeedSkill->dwLevel + pNeedSkill->dwTempSkillLevel;
        if(  dwSkillLevel <= sLVL.dwSKILL_LVL )
            sNeed.bPrecedentSkill = TRUE;
    }
	else
	if( pNeedSkill == NULL && NATIVEID_NULL() != sLEARN.sSKILL )
	{
		sNeed.bPrecedentSkill = TRUE;
	}
	
    // 요구 캐릭터 레벨
    if( pCharacter->GETLEVEL() < sLVL.dwLEVEL )
        sNeed.dwLevel = sLVL.dwLEVEL;

    const DWORD& dwUsingSkillPoint = GetUsingSkillPointAll();      // 사용된 스킬 포인트
    const DWORD& dwSkillPoint = pCharacter->m_dwSkillPoint;

    // 요구 스킬 포인트
    if( dwSkillPoint - dwUsingSkillPoint < sLVL.dwSKP )
        sNeed.dwSkillPoint = sLVL.dwSKP;

	// 요구 능력치 하나 충족 시 가능 여부;
	//2014.9.18 스킬효과의 스텟증가효과도 아이템 장착수치 계산에 포함시키기 위해 수정(CRM#3014)
	if( sLEARN.bOnlyOneStats )
	{
		if( pCharacter->m_sSUMSTATS_SKILL.CHECK_REQ_ONLY_ONE ( sLVL.sSTATS ) == FALSE )
		{
			// 요구 힘
			if( pCharacter->m_sSUMSTATS_SKILL.wPow < sLVL.sSTATS.wPow )
				sNeed .dwPow = sLVL.sSTATS.wPow;

			// 요구 체력
			if( pCharacter->m_sSUMSTATS_SKILL.wStr < sLVL.sSTATS.wStr )
				sNeed.dwStr = sLVL.sSTATS.wStr;

			// 요구 정신
			if( pCharacter->m_sSUMSTATS_SKILL.wSpi < sLVL.sSTATS.wSpi )
				sNeed.dwSpi = sLVL.sSTATS.wSpi;

			// 요구 민첩
			if( pCharacter->m_sSUMSTATS_SKILL.wDex < sLVL.sSTATS.wDex )
				sNeed.dwDex = sLVL.sSTATS.wDex;

			// 요구 근력
			if( pCharacter->m_sSUMSTATS_SKILL.wSta < sLVL.sSTATS.wSta )
				sNeed.dwSta = sLVL.sSTATS.wSta;
		}
	}
	else
	{
		// 요구 힘
		if( pCharacter->m_sSUMSTATS_SKILL.wPow < sLVL.sSTATS.wPow )
			sNeed .dwPow = sLVL.sSTATS.wPow;

		// 요구 체력
		if( pCharacter->m_sSUMSTATS_SKILL.wStr < sLVL.sSTATS.wStr )
			sNeed.dwStr = sLVL.sSTATS.wStr;

		// 요구 정신
		if( pCharacter->m_sSUMSTATS_SKILL.wSpi < sLVL.sSTATS.wSpi )
			sNeed.dwSpi = sLVL.sSTATS.wSpi;

		// 요구 민첩
		if( pCharacter->m_sSUMSTATS_SKILL.wDex < sLVL.sSTATS.wDex )
			sNeed.dwDex = sLVL.sSTATS.wDex;

		// 요구 근력
		if( pCharacter->m_sSUMSTATS_SKILL.wSta < sLVL.sSTATS.wSta )
			sNeed.dwSta = sLVL.sSTATS.wSta;
	}
    
    // 소지 골드 부족
    //if( pCharacter->GetInvenMoney() < GetUsingSkillMoneyAll() )
    {
        sNeed.dwGold = GLogicData::GetInstance().GetRnSkillUseMoney( pSkill->GetId() );
    }

    // 스크롤 사용 여부
    if(dwLevel < 1 )
    {
        sNeed.bUseScrool = GLogicData::GetInstance().GetRnSkillUseScrool( pSkill->GetId() );
    }

    return sNeed;

}


void RnSkillWindow::InitSkillPage()
{
    GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();

    if( !pCharacter )
        return;



    for( int i=0; i<TAB_COUNT; ++i )
        m_pSkillPageText[i]->ClearText();

    

    for( UINT i=0; i<TAB_COUNT; ++i )
    {
        SkillInfoVec vecSkill;
        GetSkillList( i, vecSkill );
        m_vecSkillTab[i] = vecSkill;

    }
}

void RnSkillWindow::GetSkillList( UINT nPage, SkillInfoVec& vecSkill )
{
    GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();

    if( !pCharacter )
        return;

    int nBegin = 0;
    int nEnd = 0;
    GetSkillIndexRange( pCharacter->m_emClass, nBegin, nEnd );

    int nSelectPage = nBegin + nPage;
    if( nSelectPage > nEnd )
        return;

    WORD nClass = 0;
    WORD nIndex = 0;
    GLSkillMan::GetInstance().GetMaxSkill( nClass, nIndex );

    for( int i=0; i<nIndex; ++i )
    {
        //초기화 및 기본정보
        SKILLINFO sSkillInfo;
        ZeroMemory( &sSkillInfo, sizeof( sSkillInfo ) );
        sSkillInfo.pSkill = GLSkillMan::GetInstance().GetData( nSelectPage, i ); 
        sSkillInfo.nState = CRnSkillSlot::STATE_IMPOSSIBLE;

        SKILL_NEED sNeed;
        ZeroMemory( &sNeed, sizeof( sNeed ) );
        sSkillInfo.sNeed = sNeed;

        if( sSkillInfo.pSkill != NULL )
        {
            SCHARSKILL* pCharSkill = pCharacter->GETLEARNED_SKILL( sSkillInfo.pSkill->GetId() );
            if( sSkillInfo.pSkill->m_sBASIC.bLearnView == TRUE )
                if( pCharSkill == NULL )
                    continue;
        }

        if( sSkillInfo.pSkill )
            vecSkill.push_back( sSkillInfo );
    }

    if( vecSkill.empty() )
        return;

    class CCompareSkill
    {
    public:
        bool operator() ( const SKILLINFO lvalue, const SKILLINFO rvalue )
        {
            if( lvalue.pSkill->m_sBASIC.dwGRADE < rvalue.pSkill->m_sBASIC.dwGRADE )
            {
                return true;
            }
            else if( lvalue.pSkill->m_sBASIC.dwGRADE == rvalue.pSkill->m_sBASIC.dwGRADE )
            {
                if( lvalue.pSkill->m_sBASIC.sNATIVEID.wSubID == rvalue.pSkill->m_sBASIC.sNATIVEID.wSubID )
                    return true;
            }
            return false;
        }
    };

    std::sort( vecSkill.begin(), vecSkill.end(), CCompareSkill() );
}


DWORD RnSkillWindow::GetSkillPoint( SkillInfoVec& vecSkill )
{
    GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();

    if( !pCharacter )
        return 0;

    DWORD dwSkillPoint =0;
    BOOST_FOREACH( SKILLINFO& sSkillInfo, vecSkill)
    {
        if( sSkillInfo.pSkill == NULL )
            continue;

        // 스킬 레벨 입력
        SCHARSKILL* pCharSkill = pCharacter->GETLEARNED_SKILL( sSkillInfo.pSkill->GetId() );
        if( pCharSkill != NULL )
            sSkillInfo.dwLevel = pCharSkill->wLevel + 1;
        
        dwSkillPoint += GetUsingSkillPoint( sSkillInfo );
    }
    return dwSkillPoint;
}


void RnSkillWindow::GetSkillIndexRange( EMCHARCLASS eClass, int& nBegin, int& nEnd )
{	
    switch( eClass )
    {
    case GLCC_FIGHTER_M:
    case GLCC_FIGHTER_W:
        {
            nBegin = EMSKILL_FIGHTER_01;
            nEnd = EMSKILL_FIGHTER_04;
        }
        break;

    case GLCC_ARMS_M:
    case GLCC_ARMS_W:
        {
            nBegin = EMSKILL_ARMS_01;
            nEnd = EMSKILL_ARMS_04;
        }
        break;

    case GLCC_ARCHER_M:
    case GLCC_ARCHER_W:
        {
            nBegin = EMSKILL_ARCHER_01;
            nEnd = EMSKILL_ARCHER_04;
        }
        break;

    case GLCC_SPIRIT_M:
    case GLCC_SPIRIT_W:
        {
            nBegin = EMSKILL_SPIRIT_01;
            nEnd = EMSKILL_SPIRIT_04;
        }
        break;	

    case GLCC_EXTREME_M:
    case GLCC_EXTREME_W:
        {
            nBegin = EMSKILL_EXTREME_01;
            nEnd = EMSKILL_EXTREME_04;
        }
        break;
    case GLCC_SCIENTIST_M:
    case GLCC_SCIENTIST_W:
        {
            nBegin = EMSKILL_SCIENTIST_01;
            nEnd = EMSKILL_SCIENTIST_04;			
        }
        break;
    case GLCC_ASSASSIN_M:
    case GLCC_ASSASSIN_W:
        {
            nBegin = EMSKILL_ASSASSIN_01;
            nEnd = EMSKILL_ASSASSIN_04;			
        }
        break;
	case GLCC_TRICKER_M:
	case GLCC_TRICKER_W:
		{
			nBegin = EMSKILL_TRICKER_01;
			nEnd = EMSKILL_TRICKER_04;			
		}
		break;
    case GLCC_ETC_M:
    case GLCC_ETC_W:
        {
            nBegin = EMSKILL_ETC_01;
            nEnd = EMSKILL_ETC_04;			
        }
        break;
    }
}

const char* RnSkillWindow::GetClassText( int nPage )
{
    GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();

    if( !pCharacter )
        return "Error";

    EMCLASSINDEX eClassIndex = CharClassToClassIndex( pCharacter->m_emClass );

    const CString strClassKeyword[ GLCL_NUM_CLASS ] =
    {
        "SKILL_TAB_TITLE_FIGHTER",
        "SKILL_TAB_TITLE_ARMS",
        "SKILL_TAB_TITLE_ARCHER",
        "SKILL_TAB_TITLE_SPIRIT",
        "SKILL_TAB_TITLE_EXTREME_M",
        "SKILL_TAB_TITLE_SCIENTIST",
        "SKILL_TAB_TITLE_ASSASSIN",
		"SKILL_TAB_TITLE_TRICKER",
        "SKILL_TAB_TITLE_ETC",
    };

    CString strClassText = strClassKeyword[ eClassIndex ];

    return ID2GAMEWORD( strClassText.GetString(), nPage );
}
