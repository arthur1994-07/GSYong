#include "StdAfx.h"
#include "./ActivityWindowPage.h"
#include "./ActivityGrade.h"
#include "./ActivityMainPage.h"
#include "./ActivitySalesItemWindow.h"
#include "./StudentRecordWindowRequest.h"

#include "../../../SigmaCore/String/StringFormat.h"

#include "../../../EngineLib/GUInterface/BasicLineBox.h"
#include "../../../EngineLib/GUInterface/BasicLineBoxSmart.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/BasicScrollBarEx.h"
#include "../../../EngineLib/GUInterface/BasicScrollThumbFrame.h"

#include "../../../EngineLib/DxTools/DxFontMan.h"

#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicButton.h"
#include "../../../EngineLib/GUInterface/TipBox.h"

#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"

#include "../Util/CheckBox.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

const int CActivityWindowPage::nMaxReportSpecAct = 11;

CActivityWindowPage::CActivityWindowPage(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
    : CUIPage(pEngineDevice)
    , m_pReportListScroll( NULL ) 
    , m_pSpecActMainPage( NULL )
    , m_pSelect ( NULL ) 
    , m_nReportScrollPos ( 0 ) 
    , m_nReportSelect ( 0 ) 
    , m_nMainType( 0 )
    , m_nSubType( 0 )
    , m_pProgressDisplay(NULL)
    , m_pGaeaClient(pGaeaClient)
    , m_pInterface(pInterface)
{
    memset ( m_pSpecActGrade, NULL, sizeof(ActivityGrade*) * UI_ACTIVITY_CLASS_SIZE );
    memset ( m_pSpecActType, NULL, sizeof(CBasicTextBox*) * UI_ACTIVITY_CLASS_SIZE*UI_SPEC_ACT_GATEGORY_SIZE );   
    memset ( m_pActivityCountBox, NULL, sizeof(CBasicTextBox*) * UI_ACTIVITY_CLASS_SIZE );
    memset ( m_pActivityPointBox, NULL, sizeof(CBasicTextBox*) * UI_ACTIVITY_CLASS_SIZE );
    memset ( m_pActivitySalesItem, NULL, sizeof(CBasicButton*) * UI_ACTIVITY_CLASS_SIZE );
}


CActivityWindowPage::~CActivityWindowPage()
{
}


void CActivityWindowPage::CreateSubControl ()
{
    CBasicLineBoxSmart* pBasicLineBoxSM = new CBasicLineBoxSmart(m_pEngineDevice);
	pBasicLineBoxSM->CreateSub ( this, "SPEC_ACT_LINEBOX_RESULT_TITLE", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pBasicLineBoxSM->CreateSubControl ( "SPEC_ACT_LINEBOX_RESULT_TITLE_TEXINFO" );
	RegisterControl ( pBasicLineBoxSM );

    pBasicLineBoxSM = new CBasicLineBoxSmart(m_pEngineDevice);
	pBasicLineBoxSM->CreateSub ( this, "SPEC_ACT_LINEBOX_RESULT_CONTENT", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pBasicLineBoxSM->CreateSubControl ( "SPEC_ACT_LINEBOX_RESULT_CONTENT_TEXINFO" );
	RegisterControl ( pBasicLineBoxSM );

    CBasicLineBox* pBasicLineBox = new CBasicLineBox(m_pEngineDevice);
	pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_QUEST_LIST", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pBasicLineBox->CreateBaseBoxQuestList ( "SPEC_ACT_LINEBOX_REPORT_LIST" );
	RegisterControl ( pBasicLineBox );

    CUIControl* pScrollParent = pBasicLineBox;

    //  MainPage
    {
        m_pSpecActMainPage = new ActivityMainPage(m_pGaeaClient, m_pEngineDevice);
	    m_pSpecActMainPage->CreateSub ( this, "SPEC_ACT_MAIN_PAGE" );
        m_pSpecActMainPage->CreateSubControl();
	    RegisterControl ( m_pSpecActMainPage );
    }

    CBasicButton* pButton = new CBasicButton(m_pEngineDevice);
    pButton->CreateSub ( this, "SPEC_ACT_QUESTION_BUTTON", UI_FLAG_DEFAULT, SPEC_ACT_QUESTION_BUTTON );
    pButton->CreateFlip ( "SPEC_ACT_QUESTION_BUTTON_OVER", CBasicButton::MOUSEIN_FLIP );
    RegisterControl ( pButton );

    D3DCOLOR dwColor = NS_UITEXTCOLOR::DEFAULT;
    CD3DFontPar* pFont9 = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

    {
        CBasicTextBox* pTextBox = CreateStaticControl ( "SPEC_ACT_WINDOW_RESULT_TITLE", pFont9, NS_UITEXTCOLOR::DARKORANGE, TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );
        pTextBox->SetOneLineText ( (char*)ID2GAMEWORD ( "SPEC_ACT_WINDOW_RESULT", 0  ), NS_UITEXTCOLOR::DARKORANGE );

        pTextBox = CreateStaticControl ( "SPEC_ACT_WINDOW_RESULT_HIGH_NUM", pFont9, dwColor, TEXT_ALIGN_LEFT );    
        pTextBox->SetOneLineText ( sc::string::format( (char*)ID2GAMEWORD ( "SPEC_ACT_WINDOW_RESULT", 1  ), 0 ).c_str() );
        m_pActivityCountBox[UI_ACTIVITY_CLASS_HIGH] = pTextBox;

        pTextBox = CreateStaticControl ( "SPEC_ACT_WINDOW_RESULT_HIGH_POINT", pFont9, dwColor, TEXT_ALIGN_LEFT );
        pTextBox->SetOneLineText ( sc::string::format( (char*)ID2GAMEWORD ( "SPEC_ACT_WINDOW_RESULT", 2  ), 0 ).c_str() );
        m_pActivityPointBox[UI_ACTIVITY_CLASS_HIGH] = pTextBox;

        pTextBox = CreateStaticControl ( "SPEC_ACT_WINDOW_RESULT_MEDIUM_NUM", pFont9, dwColor, TEXT_ALIGN_LEFT );
        pTextBox->SetOneLineText ( sc::string::format( (char*)ID2GAMEWORD ( "SPEC_ACT_WINDOW_RESULT", 3  ), 0 ).c_str() );
        m_pActivityCountBox[UI_ACTIVITY_CLASS_MEDIUM] = pTextBox;

        pTextBox = CreateStaticControl ( "SPEC_ACT_WINDOW_RESULT_MEDIUM_POINT", pFont9, dwColor, TEXT_ALIGN_LEFT );
        pTextBox->SetOneLineText ( sc::string::format( (char*)ID2GAMEWORD ( "SPEC_ACT_WINDOW_RESULT", 4  ), 0 ).c_str() );
        m_pActivityPointBox[UI_ACTIVITY_CLASS_MEDIUM] = pTextBox;

        pTextBox = CreateStaticControl ( "SPEC_ACT_WINDOW_RESULT_LOW_NUM", pFont9, dwColor, TEXT_ALIGN_LEFT );
        pTextBox->SetOneLineText ( sc::string::format( (char*)ID2GAMEWORD ( "SPEC_ACT_WINDOW_RESULT", 5  ), 0 ).c_str() );
        m_pActivityCountBox[UI_ACTIVITY_CLASS_LOW] = pTextBox;

        pTextBox = CreateStaticControl ( "SPEC_ACT_WINDOW_RESULT_LOW_POINT", pFont9, dwColor, TEXT_ALIGN_LEFT );
        pTextBox->SetOneLineText ( sc::string::format( (char*)ID2GAMEWORD ( "SPEC_ACT_WINDOW_RESULT", 6  ), 0 ).c_str() );
        m_pActivityPointBox[UI_ACTIVITY_CLASS_LOW] = pTextBox;

        // 판매 아이템 버튼
        {
            CBasicButton* pButton = new CBasicButton(m_pEngineDevice);
            pButton->CreateSub ( this, "SPEC_ACT_SALESITEM_BUTTON_HIGH", UI_FLAG_DEFAULT, SPEC_ACT_SALESITEM_BUTTON_HIGH );
            pButton->CreateFlip ( "SPEC_ACT_SALESITEM_BUTTON_CLICK", CBasicButton::CLICK_FLIP );
            pButton->CreateMouseOver( "SPEC_ACT_SALESITEM_BUTTON_OVER" );
            RegisterControl ( pButton );
            m_pActivitySalesItem[UI_ACTIVITY_CLASS_HIGH] = pButton;

            pButton = new CBasicButton(m_pEngineDevice);
            pButton->CreateSub ( this, "SPEC_ACT_SALESITEM_BUTTON_MEDIUM", UI_FLAG_DEFAULT, SPEC_ACT_SALESITEM_BUTTON_MEDIUM );
            pButton->CreateFlip ( "SPEC_ACT_SALESITEM_BUTTON_CLICK", CBasicButton::CLICK_FLIP );
            pButton->CreateMouseOver( "SPEC_ACT_SALESITEM_BUTTON_OVER" );
            RegisterControl ( pButton );
            m_pActivitySalesItem[UI_ACTIVITY_CLASS_MEDIUM] = pButton;

            pButton = new CBasicButton(m_pEngineDevice);
            pButton->CreateSub ( this, "SPEC_ACT_SALESITEM_BUTTON_LOW", UI_FLAG_DEFAULT, SPEC_ACT_SALESITEM_BUTTON_LOW );
            pButton->CreateFlip ( "SPEC_ACT_SALESITEM_BUTTON_CLICK", CBasicButton::CLICK_FLIP );
            pButton->CreateMouseOver( "SPEC_ACT_SALESITEM_BUTTON_OVER" );
            RegisterControl ( pButton );
            m_pActivitySalesItem[UI_ACTIVITY_CLASS_LOW] = pButton;
        }

        CUIControl* pControl = new CUIControl(m_pEngineDevice);
        pControl->CreateSub( this, "SPEC_ACT_WINDOW_RESULT_LINE0" );
        RegisterControl( pControl );

        pControl = new CUIControl(m_pEngineDevice);
        pControl->CreateSub( this, "SPEC_ACT_WINDOW_RESULT_LINE1" );
        RegisterControl( pControl );
    }

	CBasicScrollBarEx* pScrollBar = new CBasicScrollBarEx(m_pEngineDevice);
	pScrollBar->CreateSub ( this, "BASIC_SCROLLBAR", UI_FLAG_RIGHT | UI_FLAG_YSIZE );
	pScrollBar->CreateBaseScrollBar ( "SPEC_ACT_REPORT_LIST_SCROLL" );
	pScrollBar->GetThumbFrame()->SetState ( UI_ACTIVITY_CLASS_SIZE, nMaxReportSpecAct );

    //  해당 컨트롤 위에서만 마우스 휠이 동작한다.
    pScrollBar->GetThumbFrame()->SetScrollParent( pScrollParent ); 
	RegisterControl ( pScrollBar );
	m_pReportListScroll = pScrollBar;    

    for ( int i = 0; i < UI_ACTIVITY_CLASS_SIZE; ++i ) 
    {
	    m_pSpecActGrade[ i ] = new ActivityGrade( m_pInterface, m_pEngineDevice );
	    m_pSpecActGrade[ i ]->CreateSub ( this, "SPEC_ACT_REPORT_GRADE", UI_FLAG_DEFAULT, ( SPEC_ACT_REPORT_GRADE + i ) );
	    m_pSpecActGrade[ i ]->CreateSubControl();
        m_pSpecActGrade[ i ]->SetText( (char*)ID2GAMEWORD ( "SPEC_ACT_REPORT_GRADE", i  ));
        RegisterControl ( m_pSpecActGrade[ i ] );       


        for ( int j = 0; j < UI_SPEC_ACT_GATEGORY_SIZE; ++j ) 
        {   
            int nIndex = ( i * UI_SPEC_ACT_GATEGORY_SIZE ) + j;
            m_pSpecActType[nIndex] = CreateStaticControl ( "SPEC_ACT_REPORT_TYPE", pFont9, NS_UITEXTCOLOR::WHITE, 
                                                            TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y, SPEC_ACT_REPORT_TYPE+nIndex );

            m_pSpecActType[nIndex]->SetOneLineText  ( (char*)ID2GAMEWORD ( "SPEC_ACT_REPORT_TYPE", j  ) );
            m_pSpecActType[nIndex]->SetUseTextColor ( 0, TRUE );
        }
    }

	m_pSelect = new CTipBox(m_pEngineDevice);;
	m_pSelect->CreateSub( this, "QUEST_WINDOW_CHILD_BOX", UI_FLAG_XSIZE | UI_FLAG_YSIZE, SPEC_ACT_LIST_SELECT );
	m_pSelect->CreateQuestSelect( "SPEC_ACT_REPORT_TYPE" );
	RegisterControl ( m_pSelect );
	m_pSelect->SetVisibleSingle( FALSE );

    // 진행중 텍스트박스
    {
        CBasicTextBox* pTextBox = CreateStaticControl ( "SPEC_ACT_PROGRESSDISPAY_TEXTBOX", pFont9, NS_UITEXTCOLOR::WHITE, TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
        pTextBox->SetOneLineText( ID2GAMEINTEXT("SPEC_ACT_PROGRESSDISPAY_TEXTBOX"), NS_UITEXTCOLOR::WHITE );

        m_pProgressDisplay = new CCheckBox(m_pEngineDevice);
        m_pProgressDisplay->CreateSub ( this, "SPEC_ACT_PROGRESSDISPAY_CHECKBOX", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        m_pProgressDisplay->CreateSubControl( "SPEC_ACT_PROGRESSDISPAY_CHECKBOX_ON", "SPEC_ACT_PROGRESSDISPAY_CHECKBOX_OFF" );
        RegisterControl ( m_pProgressDisplay );
    }

    UpdateReportPosControl();

    SetLoadPage( 0, 0 );
}

CBasicTextBox*	CActivityWindowPage::CreateStaticControl (const char* szControlKeyword, CD3DFontPar* pFont, D3DCOLOR dwColor, int nAlign, UIGUID nWndID )
{
	CBasicTextBox* pStaticText = new CBasicTextBox(m_pEngineDevice);
	pStaticText->CreateSub ( this, szControlKeyword, nAlign, nWndID );
	pStaticText->SetFont ( pFont );
	pStaticText->SetTextAlign ( nAlign );
	RegisterControl ( pStaticText );

	return pStaticText;
}

void CActivityWindowPage::ReCalcReportTotalLine()
{
    int nTotalLine = 0;

    for ( int i = 0; i < UI_ACTIVITY_CLASS_SIZE; ++i ) 
    {
        nTotalLine++;
        
        if ( m_pSpecActGrade[i]->IsOpen() ) 
            nTotalLine += UI_SPEC_ACT_GATEGORY_SIZE;
    }
    
    CBasicScrollThumbFrame* const pThumbFrame = m_pReportListScroll->GetThumbFrame ();

    pThumbFrame->SetState( nTotalLine, nMaxReportSpecAct );

    const int nTotal = pThumbFrame->GetTotal ();
    const int nViewPerPage = pThumbFrame->GetViewPerPage ();
    
    int nCurPos = 0;
    const int nMovableLine = nTotal - nViewPerPage;

    if ( nMovableLine < 0 )
    {
        m_nReportScrollPos = 0;
        return;
    }
    
    if ( m_nReportScrollPos > nMovableLine ) 
    {
        m_nReportScrollPos = nMovableLine;
    }

    float fPercent = (float)m_nReportScrollPos / (float)nMovableLine;
    pThumbFrame->SetPercent (fPercent);



}

void CActivityWindowPage::UpdateReportPosControl()
{
    //  현재 화면에 표시해야되는 갯수를 체크한다.

    CBasicScrollThumbFrame* const pThumbFrame = m_pReportListScroll->GetThumbFrame ();
    const int nTotal = pThumbFrame->GetTotal ();
    const int nViewPerPage = pThumbFrame->GetViewPerPage ();

    // UI 모두 숨긴다.
    {
        for ( int i = 0; i < UI_ACTIVITY_CLASS_SIZE; ++i ) 
        {
            m_pSpecActGrade[i]->SetVisibleSingle( FALSE );

            for ( int j = 0; j < UI_SPEC_ACT_GATEGORY_SIZE; ++j )
            {
                int nIndex = ( i * UI_SPEC_ACT_GATEGORY_SIZE ) + j;
                m_pSpecActType[nIndex]->SetVisibleSingle( FALSE );
            }
        }

        m_pSelect->SetVisibleSingle( FALSE );
    }
    

    //  화면에 보여질 UI를 리스트를 만든다.
    std::vector<CUIControl*> vecControl;

    for ( int i = 0; i < UI_ACTIVITY_CLASS_SIZE; ++i ) 
    {
        vecControl.push_back( (CUIControl*) m_pSpecActGrade[i] );
        
        if ( m_pSpecActGrade[i]->IsOpen() )
        {
            for( int j = 0; j < UI_SPEC_ACT_GATEGORY_SIZE; ++j ) 
            {
                 int nIndex = ( i * UI_SPEC_ACT_GATEGORY_SIZE ) + j;            
                 vecControl.push_back( (CUIControl*) m_pSpecActType[nIndex] );
            }
        }           
    }

    const UIRECT& rcGlobalPos = m_pSpecActGrade[0]->GetGlobalPos();
    const UIRECT& rcLocalPos = m_pSpecActGrade[0]->GetLocalPos();

    // 스크롤바를 계산하여 ResizeControl 한다.
    for ( int i = 0; i < (int)vecControl.size(); ++i ) 
    {
        if ( i < m_nReportScrollPos ) continue;
        if ( i >= m_nReportScrollPos + nViewPerPage ) break;

        int nCount = i - m_nReportScrollPos;

        // resize
        CUIControl* pControl = vecControl[i];

        UIRECT rcGlobalPosNew = rcGlobalPos;
        rcGlobalPosNew.top = rcGlobalPosNew.top + ( (rcGlobalPosNew.sizeY + 2) * nCount );

        UIRECT rcLocalPosNew = rcLocalPos;
        rcLocalPosNew.top = rcLocalPosNew.top + ( (rcLocalPosNew.sizeY + 2) * nCount );

        pControl->SetGlobalPos( rcGlobalPosNew );
        pControl->SetLocalPos( rcLocalPosNew );
        pControl->SetVisibleSingle( TRUE );
    }   


    if ( m_pSpecActType[m_nReportSelect]->IsVisible() ) 
    {
        m_pSelect->SetGlobalPos( m_pSpecActType[m_nReportSelect]->GetGlobalPos() );
        m_pSelect->SetLocalPos( m_pSpecActType[m_nReportSelect]->GetLocalPos() );
        m_pSelect->SetVisibleSingle( TRUE );

        m_pSpecActType[m_nReportSelect]->SetTextColor(0, NS_UITEXTCOLOR::YELLOW );
    }

    return;
}

void CActivityWindowPage::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	if ( !IsVisible () ) return ;

	CUIPage::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

    {
        CBasicScrollThumbFrame* const pThumbFrame = m_pReportListScroll->GetThumbFrame ();
        const int nTotal = pThumbFrame->GetTotal ();
        const int nViewPerPage = pThumbFrame->GetViewPerPage ();

        if ( nViewPerPage < nTotal )
        {
	        int nCurPos = 0;
	        const int nMovableLine = nTotal - nViewPerPage;
	        float fPercent = pThumbFrame->GetPercent ();

	        nCurPos = (int)floor(fPercent * nMovableLine);
	        if ( nCurPos < 0 ) nCurPos = 0;

	        if ( m_nReportScrollPos == nCurPos ) return;

	        m_nReportScrollPos = nCurPos;
            
            UpdateReportPosControl();
        }
    }
}

void CActivityWindowPage::BeginSubControl () 
{
    UpdateActivity();
    UpdateActivityPoint();
}

void CActivityWindowPage::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
    CUIPage::TranslateUIMessage ( ControlID, dwMsg );

    switch ( ControlID )
    {
    case SPEC_ACT_SALESITEM_BUTTON_HIGH:
    case SPEC_ACT_SALESITEM_BUTTON_MEDIUM:
    case SPEC_ACT_SALESITEM_BUTTON_LOW:
        {
            UIGUID cButtonID = ControlID - SPEC_ACT_SALESITEM_BUTTON_BEGIN;

            if ( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
            {
                RequestPush( NS_STUDENTRECORDWINDOW::PAGE_REQ_ACTIVITY_SALESITEMWINDOW_OPEN, CActivitySalesItemWindow::EMSALESTYPE(cButtonID) );
            }
        }
        break;

    case SPEC_ACT_QUESTION_BUTTON:
        {
            if ( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
            {
                RequestPush( NS_STUDENTRECORDWINDOW::PAGE_REQ_ACTIVITY_HELPWINDOW_OPEN );
            }
        }
        break;

    case SPEC_ACT_REPORT_GRADE:
    case SPEC_ACT_REPORT_GRADE+1:
    case SPEC_ACT_REPORT_GRADE+2:
        {
            if ( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
            {
                ReCalcReportTotalLine();
                UpdateReportPosControl();
            }
        }
        break;
    }

    if ( ControlID >= SPEC_ACT_REPORT_TYPE && ControlID < SPEC_ACT_REPORT_TYPE + (UI_ACTIVITY_CLASS_SIZE*UI_SPEC_ACT_GATEGORY_SIZE) ) 
    {
        if ( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
        {
            int nSelect = ControlID - SPEC_ACT_REPORT_TYPE;
            if ( m_nReportSelect != nSelect ) 
            {
                m_pSpecActType[m_nReportSelect]->SetTextColor ( 0, NS_UITEXTCOLOR::WHITE );
                m_nReportSelect = nSelect;                

                m_pSelect->SetGlobalPos( m_pSpecActType[m_nReportSelect]->GetGlobalPos() );
                m_pSelect->SetLocalPos( m_pSpecActType[m_nReportSelect]->GetLocalPos() );
                m_pSelect->SetVisibleSingle( TRUE );

                m_pSpecActType[m_nReportSelect]->SetTextColor(0, NS_UITEXTCOLOR::YELLOW );

                int nMainType = m_nReportSelect / UI_SPEC_ACT_GATEGORY_SIZE;
                int nSubType = m_nReportSelect % UI_SPEC_ACT_GATEGORY_SIZE;

                SetLoadPage( nMainType, nSubType );
            }
        }
    }
}

ENUM_ACTIVITY_CLASS CActivityWindowPage::GetClass( int nMainType )
{
    switch ( nMainType ) 
    {
    case UI_ACTIVITY_CLASS_HIGH:
        return ACTIVITY_CLASS_HIGH;
    case UI_ACTIVITY_CLASS_MEDIUM:
        return ACTIVITY_CLASS_MEDIUM;
    case UI_ACTIVITY_CLASS_LOW: 
        return ACTIVITY_CLASS_LOW;
    }

    return ACTIVITY_CLASS_SIZE;
}

ENUM_ACTIVITY_CATEGORY CActivityWindowPage::GetCateGory ( UI_SPEC_ACT_GATEGORY emType )
{
    switch ( emType )
    {
    case UI_SPEC_ACT_GATEGORY_ALL:
        return ACTIVITY_CATEGORY_SIZE;
    case UI_SPEC_ACT_GATEGORY_LEVEL:
        return ACTIVITY_CATEGORY_LEVEL;
    case UI_SPEC_ACT_GATEGORY_KILL:
        return ACTIVITY_CATEGORY_KILL;
    case UI_SPEC_ACT_GATEGORY_TAKE:
        return ACTIVITY_CATEGORY_TAKE;
    case UI_SPEC_ACT_GATEGORY_MAP:
        return ACTIVITY_CATEGORY_MAP;
    case UI_SPEC_ACT_GATEGORY_USE:
        return ACTIVITY_CATEGORY_USEITEM;
    }

    return ACTIVITY_CATEGORY_SIZE;
}

void CActivityWindowPage::SetLoadPage( int nMainType, int nSubType )
{
    //  Page를 갱신하다.....

    m_nMainType = nMainType;
    m_nSubType = nSubType;

    VEC_ACTIVITIES  vecActivity;

    MMACTIVITIES& mmActivity = ActivityBaseData::GetInstance()->GetData();

    ITER_MMACTIVITIES pos = mmActivity.begin();
    ITER_MMACTIVITIES pos_end = mmActivity.end();

    ENUM_ACTIVITY_CLASS emClass = GetClass(m_nMainType);


    for ( ; pos != pos_end; ++pos ) 
    {
        Activity& cActivity = pos->second;

        if ( !cActivity.GetValidate() ) 
            continue;
        
        if ( emClass != cActivity.m_nClass ) 
            continue;
        
        if ( m_nSubType != UI_SPEC_ACT_GATEGORY_ALL &&
             cActivity.GetType() != GetCateGory( (UI_SPEC_ACT_GATEGORY)m_nSubType ) )
            continue;
    
        vecActivity.push_back( cActivity );
    }

    std::sort( vecActivity.begin(), vecActivity.end() );

    m_pSpecActMainPage->SetPage( vecActivity );

}

void CActivityWindowPage::UpdateActivity()
{
    VEC_ACTIVITIES  vecActivity;

    MMACTIVITIES& mmActivity = ActivityBaseData::GetInstance()->GetData();

    ITER_MMACTIVITIES pos = mmActivity.begin();
    ITER_MMACTIVITIES pos_end = mmActivity.end();

    ENUM_ACTIVITY_CLASS emClass = GetClass(m_nMainType);


    for ( ; pos != pos_end; ++pos ) 
    {
        Activity& cActivity = pos->second;

        if ( !cActivity.GetValidate() ) 
            continue;
        
        if ( emClass != cActivity.m_nClass ) 
            continue;
        
        if ( m_nSubType != UI_SPEC_ACT_GATEGORY_ALL &&
             cActivity.GetType() != GetCateGory( (UI_SPEC_ACT_GATEGORY)m_nSubType ) )
            continue;
    
        vecActivity.push_back( cActivity );
    }

    std::sort( vecActivity.begin(), vecActivity.end() );

    m_pSpecActMainPage->UpdatePage( vecActivity );
}

void CActivityWindowPage::UpdateActivityPoint()
{
    int      nCount  = 0;
    LONGLONG llPoint = 0;

    nCount  = ActivityBaseData::GetInstance()->GetComplateCount( ACTIVITY_CLASS_HIGH );
    llPoint = m_pGaeaClient->GetCharacter()->m_llActivityPoint[ACTIVITY_CLASS_HIGH];

    m_pActivityCountBox[UI_ACTIVITY_CLASS_HIGH]->SetOneLineText (
        sc::string::format( (char*)ID2GAMEWORD ( "SPEC_ACT_WINDOW_RESULT", 1  ), nCount ).c_str() );
    m_pActivityPointBox[UI_ACTIVITY_CLASS_HIGH]->SetOneLineText (
        sc::string::format( (char*)ID2GAMEWORD ( "SPEC_ACT_WINDOW_RESULT", 2  ), llPoint ).c_str() );

    nCount  = ActivityBaseData::GetInstance()->GetComplateCount( ACTIVITY_CLASS_MEDIUM );
    llPoint = m_pGaeaClient->GetCharacter()->m_llActivityPoint[ACTIVITY_CLASS_MEDIUM];

    m_pActivityCountBox[UI_ACTIVITY_CLASS_MEDIUM]->SetOneLineText (
        sc::string::format( (char*)ID2GAMEWORD ( "SPEC_ACT_WINDOW_RESULT", 3  ), nCount ).c_str() );
    m_pActivityPointBox[UI_ACTIVITY_CLASS_MEDIUM]->SetOneLineText (
        sc::string::format( (char*)ID2GAMEWORD ( "SPEC_ACT_WINDOW_RESULT", 4  ), llPoint ).c_str() );

    nCount  = ActivityBaseData::GetInstance()->GetComplateCount( ACTIVITY_CLASS_LOW );
    llPoint = m_pGaeaClient->GetCharacter()->m_llActivityPoint[ACTIVITY_CLASS_LOW];

    m_pActivityCountBox[UI_ACTIVITY_CLASS_LOW]->SetOneLineText (
        sc::string::format( (char*)ID2GAMEWORD ( "SPEC_ACT_WINDOW_RESULT", 5  ), nCount ).c_str() );
    m_pActivityPointBox[UI_ACTIVITY_CLASS_LOW]->SetOneLineText (
        sc::string::format( (char*)ID2GAMEWORD ( "SPEC_ACT_WINDOW_RESULT", 6  ), llPoint ).c_str() );

}

const bool CActivityWindowPage::IsProgressDisplay()
{
    if ( m_pProgressDisplay->IsChecked() )
    {
        return true;
    }

    return false;
}

void CActivityWindowPage::SetProgressDisplay( const bool bEnable )
{
    m_pProgressDisplay->SetCheck( (BOOL)bEnable );
}
