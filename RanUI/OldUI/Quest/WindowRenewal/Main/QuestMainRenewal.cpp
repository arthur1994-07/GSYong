#include "StdAfx.h"

#include "../../../../../EngineLib/DeviceMan.h"
#include "../../../../../EngineLib/GUInterface/BasicScrollBarEx.h"
#include "../../../../../EngineLib/GUInterface/BasicScrollThumbFrame.h"
#include "../../../../../EngineLib/GUInterface/BasicTextBoxEx.h"
#include "../../../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../../../EngineLib/DxTools/DebugSet.h"
#include "../../../../../EngineLib/GUInterface/BasicLineBox.h"
#include "../../../../../EngineLib/GUInterface/BasicTextBoxEx.h"
#include "../../../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../../../EngineLib/GUInterface/BasicTextButton.h"


#include "../../../../../RanLogicClient/RnAttendance/RnAttendanceClient.h" // 임시 코드 수정 예정 2014.7.19 bjkim
#include "../../../../../RanLogic/Quest/GLQuestMan.h"
#include "../../../../../RanLogic/Quest/GLQuestPlay.h"
#include "../../../../../RanLogic/Quest/GLQuest.h"
#include "../../../../../RanLogic/Quest/GLQuestMan.h"
#include "../../../../../RanLogic/GLogicDataMan.h"

#include "../../../../../RanLogicClient/GLGaeaClient.h"
#include "../../../../../RanLogicClient/Char/GLCharacter.h"

#include "../../../../InnerInterface.h"
#include "../../../Item/ItemImage.h"
#include "../../../Item/ItemSlot.h"
#include "../../QuestGiftView.h"
#include "../QuestWindowRenewal.h"
#include "./QuestMainRenewal.h"

// ----------------------------------------------------------------------------
#include "../../../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

const	float	CQuestMainRenewal::fANSWER_LINE_INTERVAL = 5.0f;
const	DWORD	CQuestMainRenewal::dwMINITE_BY_HOUR = 60;

CQuestMainRenewal::CQuestMainRenewal(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
    , m_pInterface(pInterface)
    , m_pGaeaClient(pGaeaClient)
	, m_pLineBox(NULL)
	, m_pIngIcon(NULL)
	, m_pTitleBack(NULL)
	, m_pTitle(NULL)
	, m_dwQuestID( NATIVEID_NULL().dwID )
	, m_pExplainBack(NULL)
	, m_pExplain(NULL)
	, m_pExplainTitleBack(NULL)
	, m_pExplainText(NULL)
	, m_pExplainScrollBar(NULL)
	, m_pMoreBack(NULL)
	, m_pMore(NULL)
	, m_pMoreTitleBack(NULL)
	, m_pMoreText(NULL)
	, m_pMoreScrollBar(NULL)
	, m_pStepBack(NULL)
	, m_pStepText(NULL)
	, m_pStepScrollBar(NULL)
	, m_pColItemBack(NULL)
	, m_pColItemTitle(NULL)
	, m_pColItemTitleBack(NULL)
	, m_pIndemnifyBack(NULL)
	, m_pIndemnifyTitleBack(NULL)
	, m_pIndemnifyTitle(NULL)
	, m_pIndemnifyText(NULL)
	, m_pIndemnifyTextScrollBar(NULL)
	, m_pIndemnifyItemScrollBar(NULL)
    , m_pIndemnifyItemMouseOver(NULL)
	, m_iStartIndex(0)
	, m_nTotalLine(0)
	, m_pGiveUpButton(NULL)
	, m_pCompleteButton(NULL)
{
	for ( int i = 0; i < nINDEMNIFY_SLOT_TOTAL; i++ )
	{
		m_pIndemnifyItem[ i ] = NULL;
		m_sID[ i ] = NATIVEID_NULL();
	}
}

CQuestMainRenewal::~CQuestMainRenewal ()
{
}

void	CQuestMainRenewal::CreateSubControl ()
{
	{
		CBasicLineBox* pBasicLineBox = new CBasicLineBox(m_pEngineDevice);
		pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_QUEST_MAIN_ITEM", UI_FLAG_XSIZE | UI_FLAG_YSIZE, QUEST_MAIN_BACK );
		pBasicLineBox->CreateBaseBoxQuestStepItem ( "QUEST_WINDOW_MORE_PAGE_BACK_LUXES" );
		RegisterControl ( pBasicLineBox );
		m_pLineBox = pBasicLineBox;
	}

	{
		CBasicLineBox* pTitle = new CBasicLineBox(m_pEngineDevice);
		pTitle->CreateSub ( this, "BASIC_LINE_BOX_QUEST_MAIN_DESC", UI_FLAG_XSIZE | UI_FLAG_YSIZE, QUEST_MAIN_TITLE_BACK );
		pTitle->CreateBaseBoxQuestStepDesc ( "QUEST_WINDOW_TITLE_BACK_LUXES" );
		RegisterControl ( pTitle );
		m_pTitleBack = pTitle;
	}

	{
		CD3DFontPar* pFont9 = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

		CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
		pTextBox->CreateSub ( this, "QUEST_WINDOW_QUEST_TITLE_LUXES", UI_FLAG_DEFAULT, QUEST_MAIN_TITLE_TEXT );
		pTextBox->SetFont ( pFont9 );
		pTextBox->SetTextAlign ( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
		RegisterControl ( pTextBox );
		m_pTitle = pTextBox;
	}

	{
		CUIControl* pControl = new CUIControl(m_pEngineDevice);
		pControl->CreateSub ( this, "QUEST_WINDOW_STATUS_ING_ICON_LUXES", UI_FLAG_DEFAULT, QUEST_MAIN_ICON );
		RegisterControl ( pControl );
		m_pIngIcon = pControl;
		m_pIngIcon->SetVisibleSingle( FALSE );
	}

	// 내용.
	{
		CBasicLineBox* pBasicLineBox = new CBasicLineBox(m_pEngineDevice);
		pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_QUEST_LIST", UI_FLAG_XSIZE | UI_FLAG_YSIZE, QUEST_MAIN_EXPLAIN_BACK );
		pBasicLineBox->CreateBaseBoxQuestList ( "QUEST_WINDOW_EXPLAIN_BACK_LUXES" );
		RegisterControl ( pBasicLineBox );
		m_pExplainBack = pBasicLineBox;
	}

	{
		CUIControl* pControl = new CUIControl(m_pEngineDevice);
		pControl->CreateSub ( this, "QUEST_WINDOW_EXPLAIN_TITLE_BACK_LUXES", UI_FLAG_DEFAULT, QUEST_MAIN_EXPLAIN_TITLE_BACK );
		RegisterControl ( pControl );
		m_pExplainTitleBack = pControl;
		m_pExplainTitleBack->SetUseRender( TRUE );
		m_pExplainTitleBack->SetDiffuse(D3DCOLOR_ARGB(128, 5, 5, 5));
		m_pExplainTitleBack->SetVisibleSingle( TRUE );
	}

	{
		CD3DFontPar* pFont9 = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

		CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
		pTextBox->CreateSub ( this, "QUEST_WINDOW_EXPLAIN_TITLE_LUXES", UI_FLAG_DEFAULT, QUEST_MAIN_EXPLAIN_TITLE );
		pTextBox->SetFont ( pFont9 );
		pTextBox->SetTextAlign ( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
		RegisterControl ( pTextBox );
		m_pExplain = pTextBox;
		m_pExplain->SetText( ID2GAMEWORD( "QUEST_LIST_EXPLAIN", 0 ), NS_UITEXTCOLOR::ORANGE );
	}

	{
		CD3DFontPar* pFont9 = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

		CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
		pTextBox->CreateSub ( this, "QUEST_WINDOW_EXPLAIN_TEXT_LUXES", UI_FLAG_DEFAULT, QUEST_MAIN_EXPLAIN_TEXT );
		pTextBox->SetFont ( pFont9 );
		pTextBox->SetLineInterval ( fANSWER_LINE_INTERVAL );
		//pTextBox->SetTextAlign ( TEXT_ALIGN_BOTH_X );
		RegisterControl ( pTextBox );
		m_pExplainText = pTextBox;

		int nTotalLine = m_pExplainText->GetVisibleLine ();

		//	스크롤바
		CBasicScrollBarEx* pScrollBar = new CBasicScrollBarEx(m_pEngineDevice);
		pScrollBar->CreateSub ( this, "BASIC_SCROLLBAR", UI_FLAG_RIGHT | UI_FLAG_YSIZE, QUEST_MAIN_EXPLAIN_SCROLLBAR );
		pScrollBar->CreateBaseScrollBar ( "QUEST_WINDOW_EXPLAIN_SCROLL_LUXES" );
		pScrollBar->GetThumbFrame()->SetState ( 1, nTotalLine );
        pScrollBar->GetThumbFrame()->SetScrollParent( m_pExplainBack );
		RegisterControl ( pScrollBar );
		m_pExplainScrollBar = pScrollBar;
	}

	// 자세히.
	{
		CBasicLineBox* pBasicLineBox = new CBasicLineBox(m_pEngineDevice);
		pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_QUEST_LIST", UI_FLAG_XSIZE | UI_FLAG_YSIZE, QUEST_MAIN_MORE_BACK );
		pBasicLineBox->CreateBaseBoxQuestList ( "QUEST_WINDOW_MORE_TEXT_BACK_LUXES" );
		RegisterControl ( pBasicLineBox );
		m_pMoreBack = pBasicLineBox;
	}

	{
		CUIControl* pControl = new CUIControl(m_pEngineDevice);
		pControl->CreateSub ( this, "QUEST_WINDOW_MORE_TITLE_BACK_LUXES", UI_FLAG_DEFAULT, QUEST_MAIN_MORE_TITLE_BACK );
		RegisterControl ( pControl );
		m_pMoreTitleBack = pControl;
		m_pMoreTitleBack->SetUseRender( TRUE );
		m_pMoreTitleBack->SetDiffuse(D3DCOLOR_ARGB(128, 5, 5, 5));
		m_pMoreTitleBack->SetVisibleSingle( TRUE );
	}

	{
		CD3DFontPar* pFont9 = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

		CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
		pTextBox->CreateSub ( this, "QUEST_WINDOW_MORE_TITLE_LUXES", UI_FLAG_DEFAULT, QUEST_MAIN_MORE_TITLE );
		pTextBox->SetFont ( pFont9 );
		pTextBox->SetTextAlign ( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
		RegisterControl ( pTextBox );
		m_pMore = pTextBox;
		m_pMore->SetText( ID2GAMEWORD( "QUEST_LIST_MORE", 0 ), NS_UITEXTCOLOR::ORANGE );
	}

	{
		CD3DFontPar* pFont9 = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

		CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
		pTextBox->CreateSub ( this, "QUEST_WINDOW_MORE_TEXT_LUXES", UI_FLAG_DEFAULT, QUEST_MAIN_MORE_TEXT );
		pTextBox->SetFont ( pFont9 );
		pTextBox->SetLineInterval ( fANSWER_LINE_INTERVAL );
		//pTextBox->SetTextAlign ( TEXT_ALIGN_BOTH_X );
		RegisterControl ( pTextBox );
		m_pMoreText = pTextBox;

		int nTotalLine = m_pMoreText->GetVisibleLine ();

		//	스크롤바
		CBasicScrollBarEx* pScrollBar = new CBasicScrollBarEx(m_pEngineDevice);
		pScrollBar->CreateSub ( this, "BASIC_SCROLLBAR", UI_FLAG_RIGHT | UI_FLAG_YSIZE, QUEST_MAIN_MORE_SCROLLBAR );
		pScrollBar->CreateBaseScrollBar ( "QUEST_WINDOW_MORE_SCROLL_LUXES" );
		pScrollBar->GetThumbFrame()->SetState ( 1, nTotalLine );
        pScrollBar->GetThumbFrame()->SetScrollParent( m_pMoreBack );
		RegisterControl ( pScrollBar );
		m_pMoreScrollBar = pScrollBar;
	}

	// 진행 상황.
	{
		CBasicLineBox* pBasicLineBox = new CBasicLineBox(m_pEngineDevice);
		pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_QUEST_LIST", UI_FLAG_XSIZE | UI_FLAG_YSIZE, QUEST_MAIN_STEP_BACK );
		pBasicLineBox->CreateBaseBoxQuestList ( "QUEST_WINDOW_STEP_BACK_LUXES" );
		RegisterControl ( pBasicLineBox );
		m_pStepBack = pBasicLineBox;
	}

	{
		CD3DFontPar* pFont9 = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

		//	텍스트 박스
		CBasicTextBoxEx* pTextBox = new CBasicTextBoxEx(m_pEngineDevice);
		pTextBox->CreateSub ( this, "QUEST_WINDOW_STEP_TEXT_LUXES", UI_FLAG_DEFAULT, QUEST_MAIN_STEP_TEXT );
		pTextBox->SetFont ( pFont9 );
		pTextBox->SetLineInterval ( fANSWER_LINE_INTERVAL );
		RegisterControl ( pTextBox );
		m_pStepText = pTextBox;

		int nTotalLine = pTextBox->GetVisibleLine ();

		//	스크롤바
		CBasicScrollBarEx* pScrollBar = new CBasicScrollBarEx(m_pEngineDevice);
		pScrollBar->CreateSub ( this, "BASIC_SCROLLBAR", UI_FLAG_RIGHT | UI_FLAG_YSIZE, QUEST_MAIN_STEP_SCROLLBAR );
		pScrollBar->CreateBaseScrollBar ( "QUEST_WINDOW_STEP_SCROLL_LUXES" );
		pScrollBar->GetThumbFrame()->SetState ( 1, nTotalLine );
        pScrollBar->GetThumbFrame()->SetScrollParent( m_pStepBack );
		RegisterControl ( pScrollBar );
		m_pStepScrollBar = pScrollBar;
	}

	// 퀘스트 수집 아이템.
	{
		CBasicLineBox* pBasicLineBox = new CBasicLineBox(m_pEngineDevice);
		pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_QUEST_LIST", UI_FLAG_XSIZE | UI_FLAG_YSIZE, QUEST_MAIN_COLL_BACK );
		pBasicLineBox->CreateBaseBoxQuestList ( "QUEST_WINDOW_COLLECT_ITEM_BACK_LUXES" );
		RegisterControl ( pBasicLineBox );
		m_pColItemBack = pBasicLineBox;
	}

	{
		CUIControl* pControl = new CUIControl(m_pEngineDevice);
		pControl->CreateSub ( this, "QUEST_WINDOW_COLLECT_ITEM_TITLE_BACK_LUXES", UI_FLAG_DEFAULT, QUEST_MAIN_COLL_TITLE_BACK );
		RegisterControl ( pControl );
		m_pColItemTitleBack = pControl;
		m_pColItemTitleBack->SetUseRender( TRUE );
		m_pColItemTitleBack->SetDiffuse(D3DCOLOR_ARGB(128, 5, 5, 5));
		m_pColItemTitleBack->SetVisibleSingle( TRUE );
	}

	{
		CD3DFontPar* pFont9 = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

		CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
		pTextBox->CreateSub ( this, "QUEST_WINDOW_COLLECT_ITEM_TITLE_LUXES", UI_FLAG_DEFAULT, QUEST_MAIN_COLL_TITLE );
		pTextBox->SetFont ( pFont9 );
		pTextBox->SetTextAlign ( TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );
		RegisterControl ( pTextBox );
		m_pColItemTitle = pTextBox;
		m_pColItemTitle->SetText( ID2GAMEINTEXT( "QUEST_LIST_COLLECT_ITEM" ), NS_UITEXTCOLOR::ORANGE );
	}

	{
		CBasicLineBox* pBasicLineBox = new CBasicLineBox(m_pEngineDevice);
		pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_QUEST_MAIN_ITEM", UI_FLAG_XSIZE | UI_FLAG_YSIZE, QUEST_MAIN_COLL_SLOT_BACK );
		pBasicLineBox->CreateBaseBoxQuestStepItem ( "QUEST_WINDOW_COLLECT_ITEM_SLOT_BACK_LUXES" );
		RegisterControl ( pBasicLineBox );

		m_pItemSlot = CreateItemSlot ( "QUEST_WINDOW_COLLECT_ITEM_SLOT_LUXES", QUEST_MAIN_COLL_SLOT );
	}
	
	//	퀘스트 보상.
	{
		CBasicLineBox* pBasicLineBox = new CBasicLineBox(m_pEngineDevice);
		pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_QUEST_LIST", UI_FLAG_XSIZE | UI_FLAG_YSIZE, QUEST_MAIN_INDEMNIFY_BACK);
		pBasicLineBox->CreateBaseBoxQuestList ( "QUEST_WINDOW_INDEMNIFY_BACK_LUXES" );
		RegisterControl ( pBasicLineBox );
		m_pIndemnifyBack = pBasicLineBox;
	}

	{
		CUIControl* pControl = new CUIControl(m_pEngineDevice);
		pControl->CreateSub ( this, "QUEST_WINDOW_INDEMNIFY_TITLE_BACK_LUXES", UI_FLAG_DEFAULT, QUEST_MAIN_INDEMNIFY_TITLE_BACK );
		RegisterControl ( pControl );
		m_pIndemnifyTitleBack = pControl;
		m_pIndemnifyTitleBack->SetUseRender( TRUE );
		m_pIndemnifyTitleBack->SetDiffuse(D3DCOLOR_ARGB(128, 5, 5, 5));
		m_pIndemnifyTitleBack->SetVisibleSingle( TRUE );
	}

	{
		CD3DFontPar* pFont9 = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

		CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
		pTextBox->CreateSub ( this, "QUEST_WINDOW_INDEMNIFY_TITLE_LUXES", UI_FLAG_DEFAULT, QUEST_MAIN_INDEMNIFY_TITLE );
		pTextBox->SetFont ( pFont9 );
		pTextBox->SetTextAlign ( TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );
		RegisterControl ( pTextBox );
		m_pIndemnifyTitle = pTextBox;
		m_pIndemnifyTitle->SetText( ID2GAMEINTEXT( "QUEST_LIST_INDEMNIFY_ITEM" ), NS_UITEXTCOLOR::ORANGE );
	}

	{
		CD3DFontPar* pFont9 = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

		//	텍스트 박스
		CBasicTextBoxEx* pTextBox = new CBasicTextBoxEx(m_pEngineDevice);
		pTextBox->CreateSub ( this, "QUEST_WINDOW_INDEMNIFY_TEXT_LUXES", UI_FLAG_DEFAULT, QUEST_MAIN_INDEMNIFY_TEXT );
		pTextBox->SetFont ( pFont9 );
		pTextBox->SetLineInterval ( fANSWER_LINE_INTERVAL );
		RegisterControl ( pTextBox );
		m_pIndemnifyText = pTextBox;

		int nTotalLine = pTextBox->GetVisibleLine ();

		//	스크롤바
		CBasicScrollBarEx* pScrollBar = new CBasicScrollBarEx(m_pEngineDevice);
		pScrollBar->CreateSub ( this, "BASIC_SCROLLBAR", UI_FLAG_RIGHT | UI_FLAG_YSIZE, QUEST_MAIN_INDEMNIFY_TEXT_SCROLLBAR );
		pScrollBar->CreateBaseScrollBar ( "QUEST_WINDOW_INDEMNIFY_TEXT_SCROLLBAR_LUXES" );
		pScrollBar->GetThumbFrame()->SetState ( 1, nTotalLine );
        pScrollBar->GetThumbFrame()->SetScrollParent( m_pIndemnifyText );
		RegisterControl ( pScrollBar );
		m_pIndemnifyTextScrollBar = pScrollBar;
	}


	{
        CUIControl* pContorlItemBack = new CUIControl(m_pEngineDevice);
        pContorlItemBack->CreateSub( this, "QUEST_WINDOW_INDEMNIFY_ITEM_BACK_LUXES" );
        RegisterControl( pContorlItemBack );
        //pContorlItemBack->SetVisibleSingle( FALSE );

		CString strUI[ nINDEMNIFY_SLOT_TOTAL ] = 
		{
			 "QUEST_WINDOW_INDEMNIFY_ITEM0_LUXES"
			,"QUEST_WINDOW_INDEMNIFY_ITEM1_LUXES"
			,"QUEST_WINDOW_INDEMNIFY_ITEM2_LUXES"
			,"QUEST_WINDOW_INDEMNIFY_ITEM3_LUXES"
			,"QUEST_WINDOW_INDEMNIFY_ITEM4_LUXES"
			,"QUEST_WINDOW_INDEMNIFY_ITEM5_LUXES"
		};

		char* chUI[ nINDEMNIFY_SLOT_TOTAL ] =
		{
			"QUEST_WINDOW_INDEMNIFY_ITEM0_LUXES"
			,"QUEST_WINDOW_INDEMNIFY_ITEM1_LUXES"
			,"QUEST_WINDOW_INDEMNIFY_ITEM2_LUXES"
			,"QUEST_WINDOW_INDEMNIFY_ITEM3_LUXES"
			,"QUEST_WINDOW_INDEMNIFY_ITEM4_LUXES"
			,"QUEST_WINDOW_INDEMNIFY_ITEM5_LUXES"
		};

		for ( int i = 0; i < nINDEMNIFY_SLOT_TOTAL; i++ )
		{
			m_pIndemnifyItem[ i ] = CreateIndemnifyItemSlot ( strUI[ i ], ( QUEST_MAIN_INDEMNIFY_ITEM0 + i ) );

			CBasicLineBox* pBasicLineBox = new CBasicLineBox(m_pEngineDevice);
			pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_QUEST_MAIN_ITEM", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
			pBasicLineBox->CreateBaseBoxQuestStepItem ( chUI[ i ]);
			RegisterControl ( pBasicLineBox );
		}

		//	스크롤바
		CBasicScrollBarEx* pScrollBar = new CBasicScrollBarEx(m_pEngineDevice);
		pScrollBar->CreateSub ( this, "BASIC_SCROLLBAR", UI_FLAG_RIGHT | UI_FLAG_YSIZE, QUEST_MAIN_INDEMNIFY_ITEM_SCROLLBAR );
		pScrollBar->CreateBaseScrollBar ( "QUEST_WINDOW_INDEMNIFY_SCROLL_LUXES" );
		pScrollBar->GetThumbFrame()->SetState ( 1, nINDEMNIFY_SLOT_TOTAL );
        pScrollBar->GetThumbFrame()->SetScrollParent( pContorlItemBack );
		RegisterControl ( pScrollBar );
		m_pIndemnifyItemScrollBar = pScrollBar;
	}

    {
        m_pIndemnifyItemMouseOver = new CUIControl(m_pEngineDevice);
        m_pIndemnifyItemMouseOver->CreateSub ( this, "ITEM_MOUSE_OVER" );
        RegisterControl ( m_pIndemnifyItemMouseOver );
    }

	//	버튼.
	{
		m_pGiveUpButton = CreateTextButton ( "QUEST_WINDOW_GIVE_UP_BUTTON_LUXES", QUEST_MAIN_GIVE_UP_BUTTON, (char*)ID2GAMEWORD("QUEST_MAIN_BUTTON",0) );
		m_pGiveUpButton->SetVisibleSingle( FALSE );
		m_pCompleteButton = CreateTextButton ( "QUEST_WINDOW_COMPLETE_BUTTON_LUXES", QUEST_MAIN_COMPLETE_BUTTON, (char*)ID2GAMEWORD("QUEST_MAIN_BUTTON",3) );
		m_pCompleteButton->SetVisibleSingle( FALSE );
	}
}

CItemSlot* CQuestMainRenewal::CreateItemSlot ( CString strKeyword, UIGUID ControlID )
{
	CItemSlot* pItemSlot = new CItemSlot( m_pGaeaClient, m_pInterface, m_pEngineDevice );
	pItemSlot->CreateSub ( this, strKeyword.GetString (), UI_FLAG_DEFAULT, ControlID );
	pItemSlot->CreateSubControl ( EQuestX, TRUE );
	RegisterControl ( pItemSlot );
	return pItemSlot;
}

CItemImage* CQuestMainRenewal::CreateIndemnifyItemSlot ( CString strKeyword, UIGUID ControlID )
{
	CItemImage* pItemSlot = new CItemImage( m_pGaeaClient, m_pEngineDevice );
	pItemSlot->CreateSub ( this, strKeyword.GetString (), UI_FLAG_DEFAULT, ControlID );
	pItemSlot->CreateSubControl();
	RegisterControl ( pItemSlot );
	return pItemSlot;
}

CBasicTextButton* CQuestMainRenewal::CreateTextButton ( char* szButton, UIGUID ControlID , char* szText )
{
	const int nBUTTONSIZE = CBasicTextButton::SIZE14;
	CBasicTextButton* pButton = new CBasicTextButton(m_pEngineDevice);
	pButton->CreateSub ( this, "BASIC_TEXT_BUTTON14", UI_FLAG_XSIZE, ControlID );
	pButton->CreateBaseButton ( szButton, nBUTTONSIZE, CBasicButton::CLICK_FLIP, szText );
	RegisterControl ( pButton );
	return pButton;
}

CUIControl*	CQuestMainRenewal::CreateControl ( char* szControl )
{
	CUIControl* pControl = new CUIControl(m_pEngineDevice);
	pControl->CreateSub ( this, szControl );
	RegisterControl ( pControl );
	return pControl;
}


CBasicTextBox* CQuestMainRenewal::CreateStaticControl (const char* szControlKeyword, CD3DFontPar* pFont, D3DCOLOR dwColor, int nAlign )
{
	CBasicTextBox* pStaticText = new CBasicTextBox(m_pEngineDevice);
	pStaticText->CreateSub ( this, szControlKeyword );
	pStaticText->SetFont ( pFont );
	pStaticText->SetTextAlign ( nAlign );
	RegisterControl ( pStaticText );
	return pStaticText;
}

bool CQuestMainRenewal::LoadQuestMain( DWORD dwQuestID )
{
	if ( dwQuestID == NATIVEID_NULL().dwID ) return false;

	m_pIngIcon->SetVisibleSingle( TRUE );

	m_pTitle->ClearText();
	m_pExplainText->ClearText();
	m_pMoreText->ClearText();
	m_pStepText->ClearText();
	m_pIndemnifyText->ClearText();

    for ( int i = 0; i < EQuestX; ++i )
    {
        UnLoadItem ( i );
    }

    for ( int i = 0; i < nINDEMNIFY_SLOT_TOTAL; i++ )
    {
        m_pIndemnifyItem[ i ]->ResetItem();
        m_sID[ i ] = NATIVEID_NULL();
    }

	m_dwQuestID = dwQuestID;

	GLQuestPlay& cQuestPlay = m_pGaeaClient->GetCharacter ()->m_cQuestPlay;
	GLQUESTPROG* pQuestProg = cQuestPlay.FindProc ( dwQuestID );
	if ( !pQuestProg ) return false;

	GLQUEST* pQuest = GLQuestMan::GetInstance().Find ( dwQuestID );
	if ( !pQuest ) return false;

	const DWORD dwSTEP = pQuestProg->m_dwSTEP;
	GLQUEST_STEP* pQuestStep = pQuest->GetSTEP ( dwSTEP );
	if( !pQuestStep ) return false;

	GLQUESTPROG_STEP& sSTEP_PROG = pQuestProg->m_sSTEP_PROG;

	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		CString strKey;
		strKey.Format("QN_%03d_%03d", pQuest->m_sNID.wMainID, pQuest->m_sNID.wSubID);
		// CGameTextMan::GetInstance().GetQuestText(strKey).GetString();
		m_pTitle->SetText( CGameTextMan::GetInstance().GetQuestText(strKey.GetString()).GetString(), NS_UITEXTCOLOR::WHITE );
	}
	else
	{
		m_pTitle->SetText( pQuest->GetTITLE(), NS_UITEXTCOLOR::WHITE );
	}

	m_pIngIcon->SetVisibleSingle( TRUE );

	//	내용
	{	
		if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
		{
			CString strKey;
			strKey.Format("QD_%03d_%03d", pQuest->m_sNID.wMainID, pQuest->m_sNID.wSubID);
			m_pExplainText->SetText( CGameTextMan::GetInstance().GetQuestText(strKey.GetString()).GetString(), NS_UITEXTCOLOR::WHITE );
		}
		else
		{
			m_pExplainText->SetText( pQuest->m_strCOMMENT.c_str(), NS_UITEXTCOLOR::WHITE );
		}
		m_pExplainScrollBar->GetThumbFrame()->SetPercent ( 0.0f );
	}

	// 자세히
	{
		if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
		{
			CString strKey;
			strKey.Format("QD_%03d_%03d_D_%d", pQuest->m_sNID.wMainID, pQuest->m_sNID.wSubID, dwSTEP);
			m_pMoreText->SetText (CGameTextMan::GetInstance().GetQuestText(strKey.GetString()).GetString(),NS_UITEXTCOLOR::WHITE);
		}
		else
		{
			m_pMoreText->SetText ( pQuestStep->GetCOMMENT (), NS_UITEXTCOLOR::WHITE );
		}
		m_pMoreScrollBar->GetThumbFrame()->SetPercent ( 0.0f );
	}

	//	진행 상황.
	{
		enum
		{
			QUEST_CHECK = 0,
			QUEST_CHECK_F
		};

		static	CString strCheckKeyword[] =
		{
			"QUEST_CHECK",
			"QUEST_CHECK_F"
		};

		GLQUEST::VECQSTEP& vecProgStep = pQuest->m_vecProgStep;
		for ( int i = 0; i < (int)vecProgStep.size(); ++i )
		{
			const GLQUEST_STEP& sQuestStep = vecProgStep[i];

			D3DCOLOR dwColor = NS_UITEXTCOLOR::WHITE;
			int nImageType = QUEST_CHECK;
			if ( i < (int)dwSTEP )
			{
				dwColor = NS_UITEXTCOLOR::DARKGRAY;
				nImageType = QUEST_CHECK_F;
			}
			else if ( i == (int)dwSTEP )
			{
				dwColor = NS_UITEXTCOLOR::GOLD;
			}

			int nIndex;
			if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
			{
				CString strKey;
				strKey.Format("QN_%03d_%03d_P_%d", pQuest->m_sNID.wMainID, pQuest->m_sNID.wSubID, i);
				nIndex = m_pStepText->AddText ( CGameTextMan::GetInstance().GetQuestText(strKey.GetString()).GetString(), dwColor );
			}
			else
			{
				nIndex = m_pStepText->AddText ( sQuestStep.GetTITLE (), dwColor );
			}
			m_pStepText->SetTextImage ( nIndex, strCheckKeyword[nImageType] );

			if ( i == (int)dwSTEP )
			{
				//	조건.
				{			
					CString strCombine;		
					CString strConditionLine1, strConditionLine2;

					if ( pQuest->m_dwLimitTime )
					{
						const DWORD dwLimitTime = pQuest->m_dwLimitTime;
						const DWORD dwLimitHour = dwLimitTime / dwMINITE_BY_HOUR;
						const DWORD dwLimitMinute = dwLimitTime % dwMINITE_BY_HOUR;

						const float fLapsTime = pQuestProg->m_fLAPSTIME;
						const DWORD dwLapsTime = ((DWORD)fLapsTime) / dwMINITE_BY_HOUR;	// 초->분단위
						const DWORD dwLapsHour = dwLapsTime / dwMINITE_BY_HOUR;
						const DWORD dwLapsMinute = dwLapsTime % dwMINITE_BY_HOUR;

						CString strLimitTime;
						if ( dwLimitHour )
						{
							strLimitTime.Format ( "%d%s %d%s",
								dwLimitHour, ID2GAMEWORD("QUEST_CONDITION_TIME_UNIT",0),	// 2, 3
								dwLimitMinute, ID2GAMEWORD("QUEST_CONDITION_TIME_UNIT",1) );
						}
						else
						{
							strLimitTime.Format ( "%d%s",
								dwLimitMinute, ID2GAMEWORD("QUEST_CONDITION_TIME_UNIT",1) );
						}

						CString strLapsTime;
						if ( dwLapsHour )
						{
							strLapsTime.Format ( "%d%s %d%s",
								dwLapsHour, ID2GAMEWORD("QUEST_CONDITION_TIME_UNIT",0),	// 2, 3
								dwLapsMinute, ID2GAMEWORD("QUEST_CONDITION_TIME_UNIT",1) );
						}
						else
						{
							strLapsTime.Format ( "%d%s",
								dwLapsMinute, ID2GAMEWORD("QUEST_CONDITION_TIME_UNIT",1) );
						}

						strConditionLine1.Format ( "%s : [%s/%s]", 
							ID2GAMEWORD("QUEST_MAIN_CONDITION",0), strLapsTime, strLimitTime );			
					}

					// QQQ : Quest;
					if ( pQuest->m_dwLimitPartyMemNum )
					{
						if ( strConditionLine1.GetLength () )
							strCombine.Format ( ", %s : %d", ID2GAMEWORD("QUEST_MAIN_CONDITION",1), pQuest->m_dwLimitPartyMemNum );
						else
							strCombine.Format ( "%s : %d", ID2GAMEWORD("QUEST_MAIN_CONDITION",1), pQuest->m_dwLimitPartyMemNum );
						strConditionLine1 += strCombine;			
					}

					//	첫번째 줄
					if ( strConditionLine1.GetLength () )
					{
						strCombine.Format ( "%s - %s", ID2GAMEWORD("QUEST_CONDITION"), strConditionLine1 );
						m_pStepText->AddText( strCombine, NS_UITEXTCOLOR::ORANGE );
						m_pStepText->SetTextImage ( nIndex, strCheckKeyword[nImageType] );
					}

					//	두번째 줄
					if ( pQuest->m_bNonDie )
					{
						strConditionLine2 = ID2GAMEWORD("QUEST_MAIN_CONDITION",2);
						if ( strConditionLine1.GetLength () )	//	윗 라인이 있을 경우
						{
							std::string strCondition = ID2GAMEWORD("QUEST_CONDITION");
							std::string strBlank ( strCondition.size(), ' ' );

							strCombine.Format ( "%s - %s", strBlank.c_str(), strConditionLine2 );				
						}
						else
						{
							strCombine.Format ( "%s - %s", ID2GAMEWORD("QUEST_CONDITION"), strConditionLine2 );				
						}

						m_pStepText->AddText( strCombine, NS_UITEXTCOLOR::ORANGE );
						m_pStepText->SetTextImage ( nIndex, strCheckKeyword[nImageType] );
					}
				}

				//	진행 상황 자세히.
				{
					enum
					{
						QUEST_CHECK = 0,
						QUEST_CHECK_F,
                        QUEST_CHECK_NO_IMAGE,
					};

					static	CString strCheckKeyword[] =
					{
						"QUEST_CHECK",
						"QUEST_CHECK_F",
                        "QUEST_CHECK_NO_IMAGE"
					};

					if ( pQuestStep->IsNEED_NPCTALK() )
					{
						D3DCOLOR dwColor = NS_UITEXTCOLOR::WHITE; 
						int nImageType = QUEST_CHECK;
						if ( sSTEP_PROG.m_bTALK )
						{
							dwColor = NS_UITEXTCOLOR::DARKGRAY;
							nImageType = QUEST_CHECK_F;
						}

						CString strCombine;
// 						if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
// 						{
// 							SNATIVEID sNPCID = pQuestStep->m_dwNID_NPCTALK;
// 							CString strKey;
// 							strKey.Format("CN_%03d_%03d", sNPCID.Mid(), sNPCID.Sid());
// 							CString strCrowName		= CGameTextMan::GetInstance().GetText(strKey.GetString(), 0, CGameTextMan::EM_CROW_TEXT).GetString();
// 							CString strCommentText	= CGameTextMan::GetInstance().GetText("QUEST_STEP_COMMENT_TEXT", EMQSCT_NPCTALK, CGameTextMan::EM_COMMENT_TEXT).GetString();
// 							CString strText;				
// 							strCombine.Format("%s \'%s\'%s", ID2GAMEWORD( "QUEST_LIST_STEP_PLUS", 0 ), strCrowName, strCommentText);
// 						}
// 						else
						{
							strCombine.Format( "%s %s", ID2GAMEWORD( "QUEST_LIST_STEP_PLUS", 0 ), pQuestStep->GetOBJ_NPCTALK() );
						}

						int nIndex = m_pStepText->AddText ( strCombine, dwColor );
						m_pStepText->SetTextImage ( nIndex, strCheckKeyword[ QUEST_CHECK_NO_IMAGE ] );
					}

					if ( pQuestStep->IsNEED_QITEM() )
					{
						D3DCOLOR dwColor = NS_UITEXTCOLOR::WHITE;
						int nImageType = QUEST_CHECK;
						if ( sSTEP_PROG.m_bQITEM )
						{
							dwColor = NS_UITEXTCOLOR::DARKGRAY;
							nImageType = QUEST_CHECK_F;
						}

						CString strCombine;
// 						if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
// 						{
// 							strCombine.Format( "%s %s"
// 								, ID2GAMEWORD( "QUEST_LIST_STEP_PLUS", 0 )
// 								, CGameTextMan::GetInstance().GetText("QUEST_STEP_COMMENT_TEXT", EMQSCT_MOBGEN_ITEM, CGameTextMan::EM_COMMENT_TEXT).GetString() );
// 						}
// 						else
						{
							strCombine.Format( "%s %s", ID2GAMEWORD( "QUEST_LIST_STEP_PLUS", 0 ), pQuestStep->GetOBJ_MOBGEN_QITEM() );
						}

						int nIndex = m_pStepText->AddText ( strCombine, dwColor );
						m_pStepText->SetTextImage ( nIndex, strCheckKeyword[ QUEST_CHECK_NO_IMAGE ] );
					}

					if ( pQuestStep->IsNEED_MOBKILL()|| pQuestStep->IsNEED_PLAYERKILL() )
					{
						D3DCOLOR dwColor = NS_UITEXTCOLOR::GOLD;
						int nImageType = QUEST_CHECK;
						if ( sSTEP_PROG.m_bMOBKILL_ALL )
						{
							dwColor = NS_UITEXTCOLOR::DARKGRAY;
							nImageType = QUEST_CHECK_F;
						}

                        CString strCombine;
						CString strCommentText;
// 						if (GLUseFeatures::GetInstance().IsUsingDataIntegrate())
// 						{
// 							strCommentText = CGameTextMan::GetInstance().GetText("QUEST_STEP_COMMENT_TEXT", EMQSCT_MOBKILL, CGameTextMan::EM_COMMENT_TEXT).GetString();
// 						}
// 						else
						{
							strCommentText = pQuestStep->GetOBJ_MOBKILL();
						}
                        if ( sSTEP_PROG.m_dwNUM_MOBKILL >= 0 )
                        {
						    strCombine.Format( "%s %s [%d/%d]", ID2GAMEWORD( "QUEST_LIST_STEP_PLUS", 0 ), strCommentText.GetString(),
							    sSTEP_PROG.m_dwNUM_MOBKILL,
							    pQuestStep->m_dwNUM_MOBKILL );
                        }
                        else
                        {
                            strCombine.Format( "%s %s", ID2GAMEWORD( "QUEST_LIST_STEP_PLUS", 0 ), strCommentText.GetString() );
                        }

						int nIndex = m_pStepText->AddText ( strCombine, dwColor );
						m_pStepText->SetTextImage ( nIndex, strCheckKeyword[ QUEST_CHECK_NO_IMAGE ] );
					}

					if ( pQuestStep->IsNEED_ITEMUSE() )
					{
						D3DCOLOR dwColor = NS_UITEXTCOLOR::GOLD;
						int nImageType = QUEST_CHECK;
						if ( sSTEP_PROG.m_bITEMUSE_ALL )
						{
							dwColor = NS_UITEXTCOLOR::DARKGRAY;
							nImageType = QUEST_CHECK_F;
						}

						CString strCombine;
						CString strComment;
// 						if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
// 						{
// 							strComment = CGameTextMan::GetInstance().GetText("QUEST_STEP_COMMENT_TEXT", EMQSCT_ITEM_USE, CGameTextMan::EM_COMMENT_TEXT).GetString();
// 						}
// 						else
						{
							strComment = pQuestStep->GetOBJ_ITEMUSE();
						}
						if ( sSTEP_PROG.m_dwNUM_ITEMUSE >= 0 )
						{
							strCombine.Format( "%s %s [%d/%d]", ID2GAMEWORD( "QUEST_LIST_STEP_PLUS", 0 ), strComment,
								sSTEP_PROG.m_dwNUM_ITEMUSE,
								pQuestStep->m_dwNUM_ITEMUSE );
						}
						else
						{
							strCombine.Format( "%s %s", ID2GAMEWORD( "QUEST_LIST_STEP_PLUS", 0 ), strComment );
						}

						int nIndex = m_pStepText->AddText ( strCombine, dwColor );
						m_pStepText->SetTextImage ( nIndex, strCheckKeyword[ QUEST_CHECK_NO_IMAGE ] );
					}

					if ( pQuestStep->IsNEED_ITEMGRINDING() )
					{
						D3DCOLOR dwColor = NS_UITEXTCOLOR::GOLD;
						int nImageType = QUEST_CHECK;
						if ( sSTEP_PROG.m_bITEMGRINDING_ALL )
						{
							dwColor = NS_UITEXTCOLOR::DARKGRAY;
							nImageType = QUEST_CHECK_F;
						}

						CString strCombine;
						if ( sSTEP_PROG.m_dwNUM_ITEMGRINDING >= 0 )
						{
							strCombine.Format( "%s %s [%d/%d]", ID2GAMEWORD( "QUEST_LIST_STEP_PLUS", 0 ), pQuestStep->m_strOBJ_ITEMGRINDING.c_str (),
								sSTEP_PROG.m_dwNUM_ITEMGRINDING,
								pQuestStep->m_dwNUM_ITEMGRINDING );
						}
						else
						{
							strCombine.Format( "%s %s", ID2GAMEWORD( "QUEST_LIST_STEP_PLUS", 0 ), pQuestStep->m_strOBJ_ITEMGRINDING.c_str () );
						}

						int nIndex = m_pStepText->AddText ( strCombine, dwColor );
						m_pStepText->SetTextImage ( nIndex, strCheckKeyword[ QUEST_CHECK_NO_IMAGE ] );
					}

					if ( pQuestStep->IsNEED_LEVELUP() )
					{
						D3DCOLOR dwColor = NS_UITEXTCOLOR::GOLD;
						int nImageType = QUEST_CHECK;
						if ( sSTEP_PROG.m_bLEVELUP_ALL )
						{
							dwColor = NS_UITEXTCOLOR::DARKGRAY;
							nImageType = QUEST_CHECK_F;
						}

						CString strCombine;
						CString strComment;
// 						if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
// 						{
// 							strComment = CGameTextMan::GetInstance().GetText("QUEST_STEP_COMMENT_TEXT", EMQSCT_LEVEL_UP_COMMENT, CGameTextMan::EM_COMMENT_TEXT).GetString();
// 						}
// 						else
						{
							strComment = pQuestStep->GetOBJ_LEVEL_UP();
						}
						if ( sSTEP_PROG.m_bLEVELUP_ALL )
						{
							strCombine.Format( "%s %s [%d/%d]"
								, ID2GAMEWORD( "QUEST_LIST_STEP_PLUS", 0 )
								, strComment.GetString()
								, sSTEP_PROG.m_dwNUM_LEVELUP
								, pQuestStep->m_dwNUM_LEVELUP );
						}
						else
						{
							strCombine.Format( "%s %s"
								, ID2GAMEWORD( "QUEST_LIST_STEP_PLUS", 0 )
								, strComment.GetString() );
						}

						int nIndex = m_pStepText->AddText ( strCombine, dwColor );
						m_pStepText->SetTextImage ( nIndex, strCheckKeyword[ QUEST_CHECK_NO_IMAGE ] );
					}

					if ( pQuestStep->IsNEED_CONFTING() )
					{
						D3DCOLOR dwColor = NS_UITEXTCOLOR::GOLD;
						int nImageType = QUEST_CHECK;
						if ( sSTEP_PROG.m_bCONFTING_ALL )
						{
							dwColor = NS_UITEXTCOLOR::DARKGRAY;
							nImageType = QUEST_CHECK_F;
						}

						CString strCombine;
						if ( sSTEP_PROG.m_dwNUM_CONFTING >= 0 )
						{
							strCombine.Format( "%s %s [%d/%d]", ID2GAMEWORD( "QUEST_LIST_STEP_PLUS", 0 ), pQuestStep->m_strOBJ_CONFTING.c_str (),
								sSTEP_PROG.m_dwNUM_CONFTING,
								pQuestStep->m_dwNUM_CONFTING );
						}
						else
						{
							strCombine.Format( "%s %s", ID2GAMEWORD( "QUEST_LIST_STEP_PLUS", 0 ), pQuestStep->m_strOBJ_CONFTING.c_str () );
						}

						int nIndex = m_pStepText->AddText ( strCombine, dwColor );
						m_pStepText->SetTextImage ( nIndex, strCheckKeyword[ QUEST_CHECK_NO_IMAGE ] );
					}

					if ( pQuestStep->IsNEED_NPCGUARD() )
					{
						D3DCOLOR dwColor = NS_UITEXTCOLOR::WHITE;
						int nImageType = QUEST_CHECK;
						if ( sSTEP_PROG.m_bNPCGUARD_DEATH )
						{
							dwColor = NS_UITEXTCOLOR::DARKGRAY;
							nImageType = QUEST_CHECK_F;
						}

						CString strCombine;
// 						if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
// 						{
// 							strCombine.Format( "%s %s"
// 								, ID2GAMEWORD( "QUEST_LIST_STEP_PLUS", 0 )
// 								, CGameTextMan::GetInstance().GetText("QUEST_STEP_COMMENT_TEXT", EMQSCT_NPCQUARD, CGameTextMan::EM_COMMENT_TEXT).GetString() );
// 						}
// 						else
						{
							strCombine.Format( "%s %s"
								, ID2GAMEWORD( "QUEST_LIST_STEP_PLUS", 0 )
								, pQuestStep->GetOBJ_NPCGUARD() );
						}

						int nIndex = m_pStepText->AddText ( strCombine, dwColor );
						m_pStepText->SetTextImage ( nIndex, strCheckKeyword[ QUEST_CHECK_NO_IMAGE ] );
					}

					if ( pQuestStep->IsNEED_REACHZONE() )
					{
						D3DCOLOR dwColor = NS_UITEXTCOLOR::WHITE;
						int nImageType = QUEST_CHECK;
						if ( sSTEP_PROG.m_bREACH_ZONE )
						{
							dwColor = NS_UITEXTCOLOR::DARKGRAY;
							nImageType = QUEST_CHECK_F;
						}

						CString strCombine;
// 						if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
// 						{
// 							strCombine.Format( "%s %s", ID2GAMEWORD( "QUEST_LIST_STEP_PLUS", 0 ), 
// 								CGameTextMan::GetInstance().GetText("QUEST_STEP_COMMENT_TEXT", EMQSCT_REACH_ZONE, CGameTextMan::EM_COMMENT_TEXT).GetString());
// 						}
// 						else
						{
							strCombine.Format( "%s %s", ID2GAMEWORD( "QUEST_LIST_STEP_PLUS", 0 ), pQuestStep->GetOBJ_REACH_ZONE() );
						}

						int nIndex = m_pStepText->AddText ( strCombine, dwColor );
						m_pStepText->SetTextImage ( nIndex, strCheckKeyword[ QUEST_CHECK_NO_IMAGE ] );
					}

					if ( pQuestStep->IsNEED_DEFENSEZONE() )
					{
						D3DCOLOR dwColor = NS_UITEXTCOLOR::WHITE;
						int nImageType = QUEST_CHECK;
						if ( sSTEP_PROG.m_bDEFENSE_ZONE_COMPLETE )
						{
							dwColor = NS_UITEXTCOLOR::DARKGRAY;
							nImageType = QUEST_CHECK_F;
						}

						CString strCombine;
// 						if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
// 						{
// 							strCombine.Format( "%s %s"
// 								, ID2GAMEWORD( "QUEST_LIST_STEP_PLUS", 0 )
// 								, CGameTextMan::GetInstance().GetText("QUEST_STEP_COMMENT_TEXT", EMQSCT_GUARD_MAP_COMMENT, CGameTextMan::EM_COMMENT_TEXT).GetString());
// 						}
// 						else
						{
							strCombine.Format( "%s %s", ID2GAMEWORD( "QUEST_LIST_STEP_PLUS", 0 ), pQuestStep->GetOBJ_DEFENSE_ZONE() );
						}

						int nIndex = m_pStepText->AddText ( strCombine, dwColor );
						m_pStepText->SetTextImage ( nIndex, strCheckKeyword[ QUEST_CHECK_NO_IMAGE ] );
					}		
				}
			}
		}
	}

	{
		CBasicScrollThumbFrame* const pThumbFrame = m_pStepScrollBar->GetThumbFrame ();

		const int nTotalLine = m_pStepText->GetTotalLine ();
		const int nLinePerOneView = m_pStepText->GetVisibleLine ();		
		pThumbFrame->SetState ( nTotalLine, nLinePerOneView );


		m_pStepText->SetCurLine ( 0 );
		pThumbFrame->SetPercent ( 0.0f );
		const int nCount = m_pStepText->GetCount () - 1;
		if ( nLinePerOneView < nTotalLine )
		{			
			float fPercent = (float)dwSTEP / (float)nCount;
			pThumbFrame->SetPercent ( fPercent );
		}
	}

	//	아이템 설정.
	{
		const GLInventory::CELL_MAP *ItemList = pQuestProg->m_sINVENTORY.GetItemList ();

		if ( ItemList->size () > EQuestTotalSlot )
		{
			GASSERT ( 0 && "인터페이스 사이즈보다, 데이타크기가 큽니다." );
			return false;
		}

		GLInventory::CELL_MAP_CITER iter = ItemList->begin();
		GLInventory::CELL_MAP_CITER iter_end = ItemList->end();

		for ( ; iter!=iter_end; ++iter )
		{
			SINVENITEM* pInvenItem = (*iter).second;
			SITEMCUSTOM &ref_ItemCustom = pInvenItem->sItemCustom;
			const SITEM* pItemData = GLogicData::GetInstance().GetItem ( ref_ItemCustom.GetNativeID() );
			if ( !pItemData )	continue;

			LoadItem ( *pInvenItem );
		}
	}

	//	퀘스트 보상.
	{
		{
			for ( int i = 0; i < nINDEMNIFY_SLOT_TOTAL; i++ )
			{
				m_pIndemnifyItem[ i ]->ResetItem();
				m_sID[ i ] = NATIVEID_NULL();
			}
		}

		//	보상치가 없으면 아무것도 표시하지 않음
		if ( !(pQuest->m_lnGiftEXP || pQuest->m_dwGiftMONEY ||
			pQuest->m_dwGiftELEMENT || pQuest->m_dwGiftLIFEPOINT ||
			pQuest->m_dwGiftSKILLPOINT || pQuest->m_dwGiftSTATSPOINT ||
			pQuest->m_vecGiftITEM.size () || pQuest->m_vecGiftSKILL.size ()) )
		{
			//return ;
		}

		{
			m_pIndemnifyText->AddText ( ID2GAMEWORD("QUEST_GIFT",9), NS_UITEXTCOLOR::GREENYELLOW );

			CString strCombine;

			if ( pQuest->m_lnGiftEXP )
			{
				strCombine.Format ( "%s : %I64d", ID2GAMEWORD("QUEST_GIFT",0), pQuest->m_lnGiftEXP );
				m_pIndemnifyText->AddText ( strCombine, NS_UITEXTCOLOR::WHITE );
			}
			if ( pQuest->m_dwGiftMONEY )
			{
				strCombine.Format ( "%s : %d", ID2GAMEWORD("QUEST_GIFT",1), pQuest->m_dwGiftMONEY );
				m_pIndemnifyText->AddText ( strCombine, NS_UITEXTCOLOR::WHITE );
			}
			if ( pQuest->m_dwGiftELEMENT )
			{
				strCombine.Format ( "%s : %d", ID2GAMEWORD("QUEST_GIFT",2), pQuest->m_dwGiftELEMENT );
				m_pIndemnifyText->AddText ( strCombine, NS_UITEXTCOLOR::WHITE );
			}
			if ( pQuest->m_dwGiftLIFEPOINT )
			{
				strCombine.Format ( "%s : %d", ID2GAMEWORD("QUEST_GIFT",3), pQuest->m_dwGiftLIFEPOINT );
				m_pIndemnifyText->AddText ( strCombine, NS_UITEXTCOLOR::WHITE );
			}
			//if ( pQuest->m_dwGiftDisPK )
			//{
			//	strCombine.Format ( "%s : %d", ID2GAMEWORD("QUEST_GIFT",4), pQuest->m_dwGiftDisPK );
			//	m_pIndemnifyText->AddText ( strCombine, NS_UITEXTCOLOR::WHITE );
			//}
			if ( pQuest->m_dwGiftSKILLPOINT )
			{
				strCombine.Format ( "%s : %d", ID2GAMEWORD("QUEST_GIFT",5), pQuest->m_dwGiftSKILLPOINT );
				m_pIndemnifyText->AddText ( strCombine, NS_UITEXTCOLOR::WHITE );
			}
			if ( pQuest->m_dwGiftSTATSPOINT )
			{
				strCombine.Format ( "%s : %d", ID2GAMEWORD("QUEST_GIFT",6), pQuest->m_dwGiftSTATSPOINT );
				m_pIndemnifyText->AddText ( strCombine, NS_UITEXTCOLOR::WHITE );
			}

			if ( pQuest->m_vecGiftITEM.size () || pQuest->m_sSTARTOPT.nJOIN == EMQJ_ATTENDANCE )
			{
				if ( pQuest->m_sSTARTOPT.nJOIN == EMQJ_ATTENDANCE )
				{
					int nday = m_pGaeaClient->GetRnAttendanceClient()->m_nDayCount; // 임시 코드 수정 예정 2014.7.19 bjkim
					DWORD dwItemId = GLQuestMan::GetInstance().GetRnattendanceRewardNDay( nday ) ;

					if (dwItemId != UINT_MAX)
					{
						SNATIVEID sItemId( dwItemId );
						SITEMCUSTOM sItemCustom( sItemId );
						pQuest->m_vecGiftITEM.clear();
						pQuest->m_vecGiftITEM.push_back(sItemCustom);
					}
				}

				m_nTotalLine = static_cast<int> (pQuest->m_vecGiftITEM.size());

				CString strItem = ID2GAMEWORD("QUEST_GIFT",7);
				strItem += " : ";
				INVENQARRAY_ITER iter = pQuest->m_vecGiftITEM.begin ();
				INVENQARRAY_ITER iter_end = pQuest->m_vecGiftITEM.end ();

				for ( int i = 0; iter != iter_end && i < m_iStartIndex * nINDEMNIFY_SLOT_SIZE_X; iter++, i++ );

				int iCurIndex = 0;

				for ( ; iter != iter_end && iCurIndex < nINDEMNIFY_SLOT_TOTAL; iter++ )
				{
					SITEMCUSTOM& sItemCustom = (*iter);
					const SITEM* pItem = GLogicData::GetInstance().GetItem ( sItemCustom.GetNativeID() );
					if ( !pItem ) continue;

					m_pIndemnifyItem[ iCurIndex ]->SetItem ( sItemCustom );
					m_sID[ iCurIndex ] = sItemCustom.GetNativeID();
					iCurIndex++;
				}

				/*CString strItem = ID2GAMEWORD("QUEST_GIFT",7);
				strItem += " : ";
				INVENQARRAY_ITER iter = pQuest->m_vecGiftITEM.begin ();
				INVENQARRAY_ITER iter_end = pQuest->m_vecGiftITEM.end ();
				for ( ; iter != iter_end; )
				{
					SITEMCUSTOM& sItemCustom = (*iter);
					const SITEM* pItem = GLogicData::GetInstance().GetItem ( sItemCustom.sNativeID );
					if ( !pItem ) continue;
					strItem += pItem->GetName ();

					++iter;
					if ( iter == iter_end ) break;
					strItem += ", ";
				}
				m_pIndemnifyText->AddText ( strItem, NS_UITEXTCOLOR::WHITE );*/
			}

			if ( pQuest->m_vecGiftSKILL.size () )
			{
				CString strSkill = ID2GAMEWORD("QUEST_GIFT",8);
				strSkill += " : ";
				DWQARRAY_ITER iter = pQuest->m_vecGiftSKILL.begin ();
				DWQARRAY_ITER iter_end = pQuest->m_vecGiftSKILL.end ();
				for ( ; iter != iter_end; )
				{
					DWORD dwID = (*iter);									
					PGLSKILL pSKILL = GLSkillMan::GetInstance().GetData ( SNATIVEID(dwID) );
					if ( !pSKILL ) continue;
					strSkill += pSKILL->GetName();

					++iter;
					if ( iter == iter_end ) break;
					strSkill += ", ";
				}
				m_pIndemnifyText->AddText ( strSkill, NS_UITEXTCOLOR::WHITE );
			}
		}
	}

	//	버튼들.
	{
		//	우선순위가 중요함.
		//	완료가 포기에 우선한다.
		//	만약, 퀘스트가 완료되었다면, 포기는 뿌릴 필요가 없다.
		m_pCompleteButton->SetVisibleSingle ( FALSE );
		m_pGiveUpButton->SetVisibleSingle ( FALSE );

		if ( pQuestProg->CheckCOMPLETE() )	//	완료버튼을 눌러야 하는 퀘스트
		{		
			m_pCompleteButton->SetVisibleSingle ( TRUE );
			return true;
		}	
		else if ( pQuest->IsGIVEUP () )	//	포기 퀘스트
		{
			m_pGiveUpButton->SetVisibleSingle ( TRUE );
			return true;
		}
	}

	return true;
}

void CQuestMainRenewal::UnLoadMain()
{
	m_pIngIcon->SetVisibleSingle( FALSE );

	m_pTitle->ClearText();
	m_pExplainText->ClearText();
	m_pMoreText->ClearText();
	m_pStepText->ClearText();
	for ( int i = 0; i < EQuestX; ++i )
	{
		UnLoadItem ( i );
	}
	for ( int i = 0; i < nINDEMNIFY_SLOT_TOTAL; i++ )
	{
		m_pIndemnifyItem[ i ]->ResetItem();
		m_sID[ i ] = NATIVEID_NULL();
	}
	m_pIndemnifyText->ClearText ();
}

void CQuestMainRenewal::SetVisibleSingle ( BOOL bVisible )
{
	CUIGroup::SetVisibleSingle ( bVisible );
}

void CQuestMainRenewal::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    m_pIndemnifyItemMouseOver->SetVisibleSingle ( FALSE );

	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	{
		CBasicScrollThumbFrame* const pThumbFrame = m_pExplainScrollBar->GetThumbFrame ();

		const int nTotalLine = m_pExplainText->GetTotalLine ();
		const int nLinePerOneView = m_pExplainText->GetVisibleLine ();
		CDebugSet::ToView ( 1, 10, "보이는 라인 %d", nLinePerOneView );

		pThumbFrame->SetState ( nTotalLine, nLinePerOneView );
		if ( nLinePerOneView < nTotalLine )
		{
			const int nMovableLine = nTotalLine - nLinePerOneView;
			float fPercent = pThumbFrame->GetPercent ();
			int nPos = (int)floor(fPercent * nMovableLine);
			m_pExplainText->SetCurLine ( nPos );			
		}
	}

	{
		CBasicScrollThumbFrame* const pThumbFrame = m_pMoreScrollBar->GetThumbFrame ();

		const int nTotalLine = m_pMoreText->GetTotalLine ();
		const int nLinePerOneView = m_pMoreText->GetVisibleLine ();
		CDebugSet::ToView ( 1, 10, "보이는 라인 %d", nLinePerOneView );

		pThumbFrame->SetState ( nTotalLine, nLinePerOneView );
		if ( nLinePerOneView < nTotalLine )
		{
			const int nMovableLine = nTotalLine - nLinePerOneView;
			float fPercent = pThumbFrame->GetPercent ();
			int nPos = (int)floor(fPercent * nMovableLine);
			m_pMoreText->SetCurLine ( nPos );			
		}
	}

	{
		CBasicScrollThumbFrame* const pThumbFrame = m_pStepScrollBar->GetThumbFrame ();

		const int nTotalLine = m_pStepText->GetTotalLine ();
		const int nLinePerOneView = m_pStepText->GetVisibleLine ();
		CDebugSet::ToView ( 1, 10, "보이는 라인 %d", nLinePerOneView );

		pThumbFrame->SetState ( nTotalLine, nLinePerOneView );
		if ( nLinePerOneView < nTotalLine )
		{
			const int nMovableLine = nTotalLine - nLinePerOneView;
			float fPercent = pThumbFrame->GetPercent ();
			int nPos = (int)floor(fPercent * nMovableLine);
			m_pStepText->SetCurLine ( nPos );			
		}
	}

	{
		CBasicScrollThumbFrame* const pThumbFrame = m_pIndemnifyTextScrollBar->GetThumbFrame ();

		const int nTotalLine = m_pIndemnifyText->GetTotalLine ();
		const int nLinePerOneView = m_pIndemnifyText->GetVisibleLine ();
		CDebugSet::ToView ( 1, 10, "보이는 라인 %d", nLinePerOneView );

		pThumbFrame->SetState ( nTotalLine, nLinePerOneView );
		if ( nLinePerOneView < nTotalLine )
		{
			const int nMovableLine = nTotalLine - nLinePerOneView;
			float fPercent = pThumbFrame->GetPercent ();
			int nPos = (int)floor(fPercent * nMovableLine);
			m_pIndemnifyText->SetCurLine ( nPos );			
		}
	}

	{
		CBasicScrollThumbFrame* const pThumbFrame = m_pIndemnifyItemScrollBar->GetThumbFrame ();

		const int nTotalLine = (int)ceil( m_nTotalLine / (float)(nINDEMNIFY_SLOT_SIZE_X) );
		const int nLinePerOneView = nINDEMNIFY_SLOT_TOTAL / nINDEMNIFY_SLOT_SIZE_X;
		//CDebugSet::ToView ( 1, 10, "보이는 라인 %d", nLinePerOneView );
		pThumbFrame->SetState ( nTotalLine, nLinePerOneView );
		if ( nLinePerOneView < m_nTotalLine )
		{
			const int nMovableLine = nTotalLine - nLinePerOneView;
			float fPercent = pThumbFrame->GetPercent ();
			int nPos = (int)floor(fPercent * nMovableLine);
			//m_pListTextProg->SetCurLine ( nPos );
			if ( nPos != m_iStartIndex )
			{
				m_iStartIndex = nPos;

				for ( int i = 0; i < nINDEMNIFY_SLOT_TOTAL; i++ )
				{
					m_pIndemnifyItem[ i ]->ResetItem();
					m_sID[ i ] = NATIVEID_NULL();
				}

				GLQuestPlay& cQuestPlay = m_pGaeaClient->GetCharacter ()->m_cQuestPlay;
				GLQUESTPROG* pQuestProg = cQuestPlay.FindProc ( m_dwQuestID );
				if ( pQuestProg )
				{
					GLQUEST* pQuest = GLQuestMan::GetInstance().Find ( m_dwQuestID );
					if ( pQuest )
					{
						CString strItem = ID2GAMEWORD("QUEST_GIFT",7);
						strItem += " : ";
						INVENQARRAY_ITER iter = pQuest->m_vecGiftITEM.begin ();
						INVENQARRAY_ITER iter_end = pQuest->m_vecGiftITEM.end ();

						for ( int i = 0; iter != iter_end && i < m_iStartIndex * nINDEMNIFY_SLOT_SIZE_X; iter++, i++ );

						int iCurIndex = 0;

						for ( ; iter != iter_end && iCurIndex < nINDEMNIFY_SLOT_TOTAL; iter++ )
						{
							SITEMCUSTOM& sItemCustom = (*iter);
							const SITEM* pItem = GLogicData::GetInstance().GetItem ( sItemCustom.GetNativeID() );
							if ( !pItem ) continue;

							m_pIndemnifyItem[ iCurIndex ]->SetItem ( sItemCustom );
							m_sID[ iCurIndex ] = sItemCustom.GetNativeID();
							iCurIndex++;
						}
					}
				}
			}
			else
				m_iStartIndex = nPos;
		}
	}
}

void CQuestMainRenewal::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	CUIGroup::TranslateUIMessage ( ControlID, dwMsg );

	switch ( ControlID )
	{
	case QUEST_MAIN_COLL_SLOT_BACK:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				int nPosX = m_pItemSlot->GetItemIndex ();
				if (nPosX < 0)
                    return;
				const SINVENITEM* pInvenItem = GetItem ( nPosX );
				{				
					if (pInvenItem && pInvenItem->sItemCustom.GetNativeID() != NATIVEID_NULL () )
					{
						m_pInterface->SHOW_ITEM_INFO_SIMPLE(pInvenItem->sItemCustom);
					}
				}
			}
		}
		break;

	case QUEST_MAIN_GIVE_UP_BUTTON:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				AddMessageEx ( UIMSG_BUTTON_GIVEUP_RENEWAL );
			}
		}
		break;

	case QUEST_MAIN_COMPLETE_BUTTON:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				AddMessageEx ( UIMSG_BUTTON_COMPLETE_RENEWAL );
			}
		}
		break;
	}

	for ( int i = QUEST_MAIN_INDEMNIFY_ITEM0; i < QUEST_MAIN_INDEMNIFY_ITEM_END; i++ )
	{
		if ( ControlID == i )
		{
			if ( CHECK_MOUSE_IN( dwMsg ) )
			{
                int nIndex = i - QUEST_MAIN_INDEMNIFY_ITEM0;

				SNATIVEID sNativeID = m_sID[ nIndex ];

				if ( sNativeID != NATIVEID_NULL() )
				{
					SITEMCUSTOM sItemCustom = SITEMCUSTOM( sNativeID );

					m_pInterface->SHOW_ITEM_INFO(
						sItemCustom,
						FALSE, FALSE, FALSE, TRUE, FALSE,
						USHRT_MAX, USHRT_MAX,
						NATIVEID_NULL() );

					if ( CHECK_LB_UP_LIKE( dwMsg ) && (DxInputDevice::GetInstance().GetKeyState ( DIK_LALT ) & DXKEY_PRESSED) )
					{
						if ( m_pInterface->ItemPreviewWindowIsPreviewItem( sItemCustom ) )
						{
							m_pInterface->ItemPreviewWindowOpen( sItemCustom );
						}                        
					}
                    else if ( CHECK_RB_UP_LIKE( dwMsg ) && (DxInputDevice::GetInstance().GetKeyState (DIK_LALT ) & DXKEY_PRESSED) )
                    {
                        const SITEM* pItem = GLItemMan::GetInstance().GetItem( sNativeID );
                        if (pItem->BasicType() == ITEM_BOX || pItem->BasicType() == ITEM_RANDOMITEM )
                        {
                            m_pInterface->GetInvenItemTooltipGroup()->ShowItemToolTipMulti( sItemCustom, DxInputDevice::GetInstance().GetMouseLocateX(), DxInputDevice::GetInstance().GetMouseLocateY() );
                        }
                    }

                    //	마우스 표시 테두리
                    const UIRECT& rcImagePos = m_pIndemnifyItem[ nIndex ]->GetGlobalPos();
                    const UIRECT& rcImageLocalPos = m_pIndemnifyItem[ nIndex ]->GetLocalPos ();
                    m_pIndemnifyItemMouseOver->SetLocalPos ( rcImageLocalPos );
                    m_pIndemnifyItemMouseOver->SetGlobalPos ( rcImagePos );
                    m_pIndemnifyItemMouseOver->SetVisibleSingle ( TRUE );
				}
			}
		}
	}
}

void CQuestMainRenewal::LoadItem ( SINVENITEM& ref_InvenItem )
{	
	int nPosX = ref_InvenItem.wPosX;
	//	int nPosY = ref_InvenItem.wPosY;
	m_pItemSlot->SetItemImage ( nPosX, ref_InvenItem );
}


const SINVENITEM* CQuestMainRenewal::GetItem(int nPosX) const
{
	return m_pItemSlot->GetItemImage(nPosX);
}

void CQuestMainRenewal::UnLoadItem ( int nPosX )
{
	m_pItemSlot->ResetItemImage ( nPosX );
}
