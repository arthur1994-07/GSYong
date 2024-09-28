#include "StdAfx.h"
#include "./ActivityMainPage.h"
#include "./ActivityListButton.h."
#include "./ActivityListSlot.h."

#include "../../../EngineLib/GUInterface/BasicLineBox.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/BasicScrollBarEx.h"
#include "../../../EngineLib/GUInterface/BasicScrollThumbFrame.h"

#include "../../../EngineLib/DxTools/DxFontMan.h"

#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"


// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

ActivityMainPage::ActivityMainPage(GLGaeaClient* pGaeaClient, EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
    , m_nListScrollPos ( 0 )
    , m_nPage( SPEC_ACT_LIST_PAGE_ALL )
    , m_pGaeaClient( pGaeaClient )
{
}

ActivityMainPage::~ActivityMainPage()
{
}

void ActivityMainPage::CreateSubControl ()
{
    CBasicLineBox* pBasicLineBox = new CBasicLineBox(m_pEngineDevice);
	pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_QUEST_LIST", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pBasicLineBox->CreateBaseBoxQuestList ( "SPEC_ACT_LINEBOX_MAIN_PAGE" );
	RegisterControl ( pBasicLineBox );

    CUIControl* pContorl = new CUIControl(m_pEngineDevice);
    pContorl->CreateSub( this, "SPEC_ACT_PAGE_LEFT_BACK" );
    RegisterControl( pContorl );

    pContorl = new CUIControl(m_pEngineDevice);
    pContorl->CreateSub( this, "SPEC_ACT_PAGE_CENTER_BACK" );
    RegisterControl( pContorl );

    pContorl = new CUIControl(m_pEngineDevice);
    pContorl->CreateSub( this, "SPEC_ACT_PAGE_RIGHT_BACK" );
    RegisterControl( pContorl );

    m_pListButton[0] = new ActivityListButton(m_pEngineDevice);
    m_pListButton[0]->CreateSub( this, "SPEC_ACT_LIST_ALL_BUTTON", UI_FLAG_DEFAULT, SPEC_ACT_LIST_ALL_BUTTON );
    m_pListButton[0]->CreateSubControl();
    m_pListButton[0]->SetText( (char*)ID2GAMEWORD ( "SPEC_ACT_LIST_BUTTON", 0  ));
    RegisterControl ( m_pListButton[0] );

    m_pListButton[1] = new ActivityListButton(m_pEngineDevice);
    m_pListButton[1]->CreateSub( this, "SPEC_ACT_LIST_PROG_BUTTON", UI_FLAG_DEFAULT, SPEC_ACT_LIST_PROG_BUTTON );
    m_pListButton[1]->CreateSubControl();
    m_pListButton[1]->SetText( (char*)ID2GAMEWORD ( "SPEC_ACT_LIST_BUTTON", 1  ));
    RegisterControl ( m_pListButton[1] );

    m_pListButton[2] = new ActivityListButton(m_pEngineDevice);
    m_pListButton[2]->CreateSub( this, "SPEC_ACT_LIST_COMP_BUTTON", UI_FLAG_DEFAULT, SPEC_ACT_LIST_COMP_BUTTON );
    m_pListButton[2]->CreateSubControl();
    m_pListButton[2]->SetText( (char*)ID2GAMEWORD ( "SPEC_ACT_LIST_BUTTON", 2  ));
    RegisterControl ( m_pListButton[2] );

    m_pListButton[3] = new ActivityListButton(m_pEngineDevice);
    m_pListButton[3]->CreateSub( this, "SPEC_ACT_LIST_NOTCOMP_BUTTON", UI_FLAG_DEFAULT, SPEC_ACT_LIST_NOTCOMP_BUTTON );
    m_pListButton[3]->CreateSubControl();
    m_pListButton[3]->SetText( (char*)ID2GAMEWORD ( "SPEC_ACT_LIST_BUTTON", 3  ));
    RegisterControl ( m_pListButton[3] );

    CD3DFontPar* pFont9 = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );
    D3DCOLOR dwColor = NS_UITEXTCOLOR::DEFAULT;
    
    CBasicTextBox* pTextBox = CreateStaticControl ( "SPEC_ACT_PAGE_ARRIVAL_TEXT", pFont9, dwColor, TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );
    pTextBox->SetOneLineText ( (char*)ID2GAMEWORD ( "SPEC_ACT_PAGE_TEXT", 0  ) );

    pTextBox = CreateStaticControl ( "SPEC_ACT_PAGE_PRESENT_TEXT", pFont9, dwColor, TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );
    pTextBox->SetOneLineText ( (char*)ID2GAMEWORD ( "SPEC_ACT_PAGE_TEXT", 1  ) );

    pTextBox = CreateStaticControl ( "SPEC_ACT_PAGE_REWARD_TEXT", pFont9, dwColor, TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );
    pTextBox->SetOneLineText ( (char*)ID2GAMEWORD ( "SPEC_ACT_PAGE_TEXT", 2  ) );


    for ( int i = 0; i < SPEC_ACT_LIST_SLOT_NUM; ++i ) 
    {
        CString strKey;
        strKey.Format( "SPEC_ACT_LIST_SLOT%d", i );
        m_pListSlot[i] = new ActivityListSlot(m_pGaeaClient, m_pEngineDevice);
        m_pListSlot[i]->CreateSub( this, strKey, UI_FLAG_DEFAULT );
        m_pListSlot[i]->CreateSubControl();
        RegisterControl ( m_pListSlot[i] );  
    }

	CBasicScrollBarEx* pScrollBar = new CBasicScrollBarEx(m_pEngineDevice);
	pScrollBar->CreateSub ( this, "BASIC_SCROLLBAR", UI_FLAG_RIGHT | UI_FLAG_YSIZE );
	pScrollBar->CreateBaseScrollBar ( "SPEC_ACT_SLOT_LIST_SCROLL" );
	pScrollBar->GetThumbFrame()->SetState ( 0, SPEC_ACT_LIST_SLOT_NUM );
    
    //  해당 컨트롤 위에서만 마우스 휠이 동작한다.
    pScrollBar->GetThumbFrame()->SetScrollParent( this ); 

	RegisterControl ( pScrollBar );
	m_pSlotListScroll = pScrollBar;
    m_nListScrollPos = 0;
}

void  ActivityMainPage::SetPage( const VEC_ACTIVITIES& vecSpecAct )
{
    m_vecSpecAct = vecSpecAct;
    m_nPage = SPEC_ACT_LIST_PAGE_ALL;

    SetVisiblePage();
}

void  ActivityMainPage::UpdatePage( const VEC_ACTIVITIES& vecSpecAct ) 
{
    m_vecSpecAct = vecSpecAct;
    UpdatePageData();

    //  스크롤 원상복귀
    int nSize = (int)m_vecSpecActPage.size();
        
    m_pSlotListScroll->GetThumbFrame()->SetState ( nSize, SPEC_ACT_LIST_SLOT_NUM );

    UpdateSlot();
}

void  ActivityMainPage:: SetVisiblePage()
{
    if ( m_nPage < 0 || m_nPage >= SPEC_ACT_LIST_PAGE_NUM ) return;

    
    for ( int i = 0; i < SPEC_ACT_LIST_PAGE_NUM; ++i ) 
    {
        m_pListButton[i]->SetFlip( FALSE );
    }

    m_pListButton[m_nPage]->SetFlip( TRUE );

    UpdatePageData();

    //  스크롤 원상복귀
    int nSize = (int)m_vecSpecActPage.size();
        
    m_pSlotListScroll->GetThumbFrame()->SetState ( nSize, SPEC_ACT_LIST_SLOT_NUM );
    m_pSlotListScroll->GetThumbFrame()->SetPercent( 0.0f );
    m_nListScrollPos = 0;

    UpdateSlot();
}

void ActivityMainPage::UpdatePageData()
{
    m_vecSpecActPage.clear();
    
    if( SPEC_ACT_LIST_PAGE_ALL == m_nPage )
    {
        m_vecSpecActPage = m_vecSpecAct;
    }
    else
    {
        ENUM_ACTIVITY_STATUS emStatus = ACTIVITY_STATUS_INPROGRESS;
        switch ( m_nPage ) 
        {
        case SPEC_ACT_LIST_PAGE_PROG:
            emStatus = ACTIVITY_STATUS_INPROGRESS;
            break;
        case SPEC_ACT_LIST_PAGE_COMP:
            emStatus = ACTIVITY_STATUS_COMPLETED;
            break;
        case SPEC_ACT_LIST_PAGE_NOTCOMP:
            emStatus = ACTIVITY_STATUS_CLOSED;
            break;
        };

        ITER_VEC_ACTIVITIES pos = m_vecSpecAct.begin();
        ITER_VEC_ACTIVITIES pos_end = m_vecSpecAct.end();

        for ( ; pos != pos_end; ++pos )
        {
            Activity& cActivity = *pos;

            if ( cActivity.GetStatus() != emStatus ) continue;

            // 진행중 상태일때 실제로 진행중인 값만 넣는다
            if ( emStatus == ACTIVITY_STATUS_INPROGRESS )
            {           
                //  레벨 달성일 경우
                if ( cActivity.m_id.wMainID == ACTIVITY_MID_LEVEL )
                {
                    // Blank
                }
                else if ( !cActivity.GetValue() )
                {
                    continue;
                }
            }

            m_vecSpecActPage.push_back( cActivity );
        }
    }
}

void ActivityMainPage::UpdateSlot()
{    
    //  내용을 출력한다.
    int nSize = (int)m_vecSpecActPage.size();

    for ( int i = 0; i < SPEC_ACT_LIST_SLOT_NUM; ++i ) 
    {
        m_pListSlot[i]->SetVisibleSingle( FALSE );
    }

    int nIndex = 0;

    for ( int i = 0; i < nSize; ++i )
    {
        if ( i < m_nListScrollPos ) continue;
        if ( nIndex >= SPEC_ACT_LIST_SLOT_NUM ) break;
            
        m_pListSlot[nIndex]->SetSpecAct( m_vecSpecActPage[i] );
        m_pListSlot[nIndex]->SetVisibleSingle( TRUE );

        nIndex++;
    }
}

CBasicTextBox*	ActivityMainPage::CreateStaticControl (const char* szControlKeyword, CD3DFontPar* pFont, D3DCOLOR dwColor, int nAlign, UIGUID nWndID )
{
	CBasicTextBox* pStaticText = new CBasicTextBox(m_pEngineDevice);
	pStaticText->CreateSub ( this, szControlKeyword, nAlign, nWndID );
	pStaticText->SetFont ( pFont );
	pStaticText->SetTextAlign ( nAlign );
	RegisterControl ( pStaticText );

	return pStaticText;
}

void ActivityMainPage::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    CUIGroup::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

    {
        CBasicScrollThumbFrame* const pThumbFrame = m_pSlotListScroll->GetThumbFrame ();
        const int nTotal = pThumbFrame->GetTotal ();
        const int nViewPerPage = pThumbFrame->GetViewPerPage ();

        if ( nViewPerPage < nTotal )
        {
	        int nCurPos = 0;
	        const int nMovableLine = nTotal - nViewPerPage;
	        float fPercent = pThumbFrame->GetPercent ();

	        nCurPos = (int)floor(fPercent * nMovableLine);
	        if ( nCurPos < 0 ) nCurPos = 0;

	        if ( m_nListScrollPos == nCurPos ) return;

	        m_nListScrollPos = nCurPos;
            
            UpdateSlot();
        }
    }
}

void ActivityMainPage::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
    CUIGroup::TranslateUIMessage ( ControlID, dwMsg );

    switch ( ControlID )
    {
    case SPEC_ACT_LIST_ALL_BUTTON:
    case SPEC_ACT_LIST_PROG_BUTTON:
    case SPEC_ACT_LIST_COMP_BUTTON:
    case SPEC_ACT_LIST_NOTCOMP_BUTTON:
        {
            if ( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
            {
                int nIndex = ControlID - SPEC_ACT_LIST_ALL_BUTTON;
                m_nPage = nIndex;
                SetVisiblePage ();
            }
        }
    }
}