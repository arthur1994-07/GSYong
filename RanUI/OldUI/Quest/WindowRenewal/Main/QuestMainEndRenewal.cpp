#include "StdAfx.h"

#include "../QuestWindowRenewal.h"
#include "./QuestMainEndRenewal.h"
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
#include "../../../Item/ItemSlot.h"
#include "../../../../../EngineLib/GUInterface/BasicTextButton.h"

#include "../../../../../RanLogic/Quest/GLQuestPlay.h"
#include "../../../../../RanLogic/Quest/GLQuest.h"
#include "../../../../../RanLogic/Quest/GLQuestMan.h"
#include "../../../../../RanLogic/GLogicDataMan.h"
#include "../../../../../RanLogic/GLogicDataMan.h"

#include "../../../../../RanLogicClient/GLGaeaClient.h"
#include "../../../../../RanLogicClient/Char/GLCharacter.h"

#include "../../../../InnerInterface.h"
#include "../../QuestGiftView.h"
#include "../../../Item/ItemImage.h"

// ----------------------------------------------------------------------------
#include "../../../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

const	float	CQuestMainEndRenewal::fANSWER_LINE_INTERVAL = 5.0f;
const	DWORD	CQuestMainEndRenewal::dwMINITE_BY_HOUR = 60;

CQuestMainEndRenewal::CQuestMainEndRenewal(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
    , m_pInterface(pInterface)
    , m_pGaeaClient(pGaeaClient)
	, m_pLineBox(NULL)
	, m_pDoneIcon(NULL)
	, m_pFailIcon(NULL)
	, m_pTitleBack(NULL)
	, m_pTitle(NULL)
	, m_dwQuestID( NATIVEID_NULL().dwID )
	, m_pIndemnifyBack(NULL)
	, m_pIndemnifyTitleBack(NULL)
	, m_pIndemnifyTitle(NULL)
	, m_pIndemnifyText(NULL)
	, m_pIndemnifyTextScrollBar(NULL)
	, m_pIndemnifyItemScrollBar(NULL)
	, m_iStartIndex(0)
	, m_nTotalLine(0)

	, m_pEndResultBack(NULL)
	, m_pEndResultTitle(NULL)
	, m_pEndResultTitleBack(NULL)
	, m_pEndResultText(NULL)
	, m_pEndExplainBack(NULL)
	, m_pEndExplain(NULL)
	, m_pEndExplainTitleBack(NULL)
	, m_pEndExplainText(NULL)
	, m_pEndExplainScrollBar(NULL)
	, m_pEndMoreBack(NULL)
	, m_pEndMore(NULL)
	, m_pEndMoreTitleBack(NULL)
	, m_pEndMoreText(NULL)
	, m_pEndMoreScrollBar(NULL)
	, m_pEndStepBack(NULL)
	, m_pEndStepText(NULL)
	, m_pEndStepScrollBar(NULL)
{
	for ( int i = 0; i < nINDEMNIFY_SLOT_TOTAL; i++ )
	{
		m_pIndemnifyItem[ i ] = NULL;
		m_sID[ i ] = NATIVEID_NULL();
	}
}

CQuestMainEndRenewal::~CQuestMainEndRenewal ()
{
}

void	CQuestMainEndRenewal::CreateSubControl ()
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
		pControl->CreateSub ( this, "QUEST_WINDOW_STATUS_DONE_ICON_LUXES", UI_FLAG_DEFAULT, QUEST_MAIN_DONE_ICON );
		RegisterControl ( pControl );
		m_pDoneIcon = pControl;
		m_pDoneIcon->SetVisibleSingle( FALSE );
	}

	{
		CUIControl* pControl = new CUIControl(m_pEngineDevice);
		pControl->CreateSub ( this, "QUEST_WINDOW_STATUS_FAIL_ICON_LUXES", UI_FLAG_DEFAULT, QUEST_MAIN_FAIL_ICON );
		RegisterControl ( pControl );
		m_pFailIcon = pControl;
		m_pFailIcon->SetVisibleSingle( FALSE );
	}

	// 수행 결과 - 종료된 퀘스트.
	{
		CBasicLineBox* pBasicLineBox = new CBasicLineBox(m_pEngineDevice);
		pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_QUEST_LIST", UI_FLAG_XSIZE | UI_FLAG_YSIZE, QUEST_MAIN_END_RESULT_BACK );
		pBasicLineBox->CreateBaseBoxQuestList ( "QUEST_WINDOW_RESULT_BACK_LUXES" );
		RegisterControl ( pBasicLineBox );
		m_pEndResultBack = pBasicLineBox;
	}

	{
		CUIControl* pControl = new CUIControl(m_pEngineDevice);
		pControl->CreateSub ( this, "QUEST_WINDOW_RESULT_TITLE_BACK_LUXES", UI_FLAG_DEFAULT, QUEST_MAIN_END_RESULT_TITLE_BACK );
		RegisterControl ( pControl );
		m_pEndResultTitleBack = pControl;
		m_pEndResultTitleBack->SetUseRender( TRUE );
		m_pEndResultTitleBack->SetDiffuse(D3DCOLOR_ARGB(128, 5, 5, 5));
		m_pEndResultTitleBack->SetVisibleSingle( TRUE );
	}

	{
		CD3DFontPar* pFont9 = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

		CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
		pTextBox->CreateSub ( this, "QUEST_WINDOW_RESULT_TITLE_LUXES", UI_FLAG_DEFAULT, QUEST_MAIN_END_RESULT_TITLE );
		pTextBox->SetFont ( pFont9 );
		pTextBox->SetTextAlign ( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
		RegisterControl ( pTextBox );
		m_pEndResultTitle = pTextBox;
		m_pEndResultTitle->SetText( ID2GAMEWORD( "QUEST_LIST_RESULT", 0 ), NS_UITEXTCOLOR::ORANGE );
	}

	{
		CD3DFontPar* pFont9 = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

		CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
		pTextBox->CreateSub ( this, "QUEST_WINDOW_RESULT_TEXT_LUXES", UI_FLAG_DEFAULT, QUEST_MAIN_END_RESULT_TEXT );
		pTextBox->SetFont ( pFont9 );
		pTextBox->SetLineInterval ( fANSWER_LINE_INTERVAL );
		//pTextBox->SetTextAlign ( TEXT_ALIGN_BOTH_X );
		RegisterControl ( pTextBox );
		m_pEndResultText = pTextBox;
	}

	// 내용 - 종료된 퀘스트.
	{
		CBasicLineBox* pBasicLineBox = new CBasicLineBox(m_pEngineDevice);
		pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_QUEST_LIST", UI_FLAG_XSIZE | UI_FLAG_YSIZE, QUEST_MAIN_END_EXPLAIN_BACK );
		pBasicLineBox->CreateBaseBoxQuestList ( "QUEST_WINDOW_EXPLAIN_BACK_END_LUXES" );
		RegisterControl ( pBasicLineBox );
		m_pEndExplainBack = pBasicLineBox;
	}

	{
		CUIControl* pControl = new CUIControl(m_pEngineDevice);
		pControl->CreateSub ( this, "QUEST_WINDOW_EXPLAIN_TITLE_BACK_END_LUXES", UI_FLAG_DEFAULT, QUEST_MAIN_END_EXPLAIN_TITLE_BACK );
		RegisterControl ( pControl );
		m_pEndExplainTitleBack = pControl;
		m_pEndExplainTitleBack->SetUseRender( TRUE );
		m_pEndExplainTitleBack->SetDiffuse(D3DCOLOR_ARGB(128, 5, 5, 5));
		m_pEndExplainTitleBack->SetVisibleSingle( TRUE );
	}

	{
		CD3DFontPar* pFont9 = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

		CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
		pTextBox->CreateSub ( this, "QUEST_WINDOW_EXPLAIN_TITLE_END_LUXES", UI_FLAG_DEFAULT, QUEST_MAIN_END_EXPLAIN_TITLE );
		pTextBox->SetFont ( pFont9 );
		pTextBox->SetTextAlign ( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
		RegisterControl ( pTextBox );
		m_pEndExplain = pTextBox;
		m_pEndExplain->SetText( ID2GAMEWORD( "QUEST_LIST_EXPLAIN", 0 ), NS_UITEXTCOLOR::ORANGE );
	}

	{
		CD3DFontPar* pFont9 = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

		CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
		pTextBox->CreateSub ( this, "QUEST_WINDOW_EXPLAIN_TEXT_END_LUXES", UI_FLAG_DEFAULT, QUEST_MAIN_END_EXPLAIN_TEXT );
		pTextBox->SetFont ( pFont9 );
		pTextBox->SetLineInterval ( fANSWER_LINE_INTERVAL );
		//pTextBox->SetTextAlign ( TEXT_ALIGN_BOTH_X );
		RegisterControl ( pTextBox );
		m_pEndExplainText = pTextBox;

		int nTotalLine = m_pEndExplainText->GetVisibleLine ();

		//	스크롤바
		CBasicScrollBarEx* pScrollBar = new CBasicScrollBarEx(m_pEngineDevice);
		pScrollBar->CreateSub ( this, "BASIC_SCROLLBAR", UI_FLAG_RIGHT | UI_FLAG_YSIZE, QUEST_MAIN_END_EXPLAIN_SCROLLBAR );
		pScrollBar->CreateBaseScrollBar ( "QUEST_WINDOW_EXPLAIN_SCROLL_END_LUXES" );
		pScrollBar->GetThumbFrame()->SetState ( 1, nTotalLine );
        pScrollBar->GetThumbFrame()->SetScrollParent( m_pEndExplainBack );
		RegisterControl ( pScrollBar );
		m_pEndExplainScrollBar = pScrollBar;
	}

	// 자세히 - 종료된 퀘스트.
	{
		CBasicLineBox* pBasicLineBox = new CBasicLineBox(m_pEngineDevice);
		pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_QUEST_LIST", UI_FLAG_XSIZE | UI_FLAG_YSIZE, QUEST_MAIN_END_MORE_BACK );
		pBasicLineBox->CreateBaseBoxQuestList ( "QUEST_WINDOW_MORE_TEXT_BACK_END_LUXES" );
		RegisterControl ( pBasicLineBox );
		m_pEndMoreBack = pBasicLineBox;
	}

	{
		CUIControl* pControl = new CUIControl(m_pEngineDevice);
		pControl->CreateSub ( this, "QUEST_WINDOW_MORE_TITLE_BACK_END_LUXES", UI_FLAG_DEFAULT, QUEST_MAIN_END_MORE_TITLE_BACK );
		RegisterControl ( pControl );
		m_pEndMoreTitleBack = pControl;
		m_pEndMoreTitleBack->SetUseRender( TRUE );
		m_pEndMoreTitleBack->SetDiffuse(D3DCOLOR_ARGB(128, 5, 5, 5));
		m_pEndMoreTitleBack->SetVisibleSingle( TRUE );
	}

	{
		CD3DFontPar* pFont9 = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

		CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
		pTextBox->CreateSub ( this, "QUEST_WINDOW_MORE_TITLE_END_LUXES", UI_FLAG_DEFAULT, QUEST_MAIN_END_MORE_TITLE );
		pTextBox->SetFont ( pFont9 );
		pTextBox->SetTextAlign ( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
		RegisterControl ( pTextBox );
		m_pEndMore = pTextBox;
		m_pEndMore->SetText( ID2GAMEWORD( "QUEST_LIST_MORE", 0 ), NS_UITEXTCOLOR::ORANGE );
	}

	{
		CD3DFontPar* pFont9 = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

		CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
		pTextBox->CreateSub ( this, "QUEST_WINDOW_MORE_TEXT_END_LUXES", UI_FLAG_DEFAULT, QUEST_MAIN_END_MORE_TEXT );
		pTextBox->SetFont ( pFont9 );
		pTextBox->SetLineInterval ( fANSWER_LINE_INTERVAL );
		//pTextBox->SetTextAlign ( TEXT_ALIGN_BOTH_X );
		RegisterControl ( pTextBox );
		m_pEndMoreText = pTextBox;

		int nTotalLine = m_pEndMoreText->GetVisibleLine ();

		//	스크롤바
		CBasicScrollBarEx* pScrollBar = new CBasicScrollBarEx(m_pEngineDevice);
		pScrollBar->CreateSub ( this, "BASIC_SCROLLBAR", UI_FLAG_RIGHT | UI_FLAG_YSIZE, QUEST_MAIN_END_MORE_SCROLLBAR );
		pScrollBar->CreateBaseScrollBar ( "QUEST_WINDOW_MORE_SCROLL_END_LUXES" );
		pScrollBar->GetThumbFrame()->SetState ( 1, nTotalLine );
        pScrollBar->GetThumbFrame()->SetScrollParent( m_pEndMoreBack );
		RegisterControl ( pScrollBar );
		m_pEndMoreScrollBar = pScrollBar;
	}

	// 진행 상황 - 종료된 퀘스트.
	{
		CBasicLineBox* pBasicLineBox = new CBasicLineBox(m_pEngineDevice);
		pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_QUEST_LIST", UI_FLAG_XSIZE | UI_FLAG_YSIZE, QUEST_MAIN_END_STEP_BACK );
		pBasicLineBox->CreateBaseBoxQuestList ( "QUEST_WINDOW_STEP_BACK_END_LUXES" );
		RegisterControl ( pBasicLineBox );
		m_pEndStepBack = pBasicLineBox;
	}

	{
		CD3DFontPar* pFont9 = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

		//	텍스트 박스
		CBasicTextBoxEx* pTextBox = new CBasicTextBoxEx(m_pEngineDevice);
		pTextBox->CreateSub ( this, "QUEST_WINDOW_STEP_TEXT_END_LUXES", UI_FLAG_DEFAULT, QUEST_MAIN_END_STEP_TEXT );
		pTextBox->SetFont ( pFont9 );
		pTextBox->SetLineInterval ( fANSWER_LINE_INTERVAL );
		RegisterControl ( pTextBox );
		m_pEndStepText = pTextBox;

		int nTotalLine = pTextBox->GetVisibleLine ();

		//	스크롤바
		CBasicScrollBarEx* pScrollBar = new CBasicScrollBarEx(m_pEngineDevice);
		pScrollBar->CreateSub ( this, "BASIC_SCROLLBAR", UI_FLAG_RIGHT | UI_FLAG_YSIZE, QUEST_MAIN_END_STEP_SCROLLBAR );
		pScrollBar->CreateBaseScrollBar ( "QUEST_WINDOW_STEP_SCROLL_END_LUXES" );
		pScrollBar->GetThumbFrame()->SetState ( 1, nTotalLine );
        pScrollBar->GetThumbFrame()->SetScrollParent( m_pEndStepBack );
		RegisterControl ( pScrollBar );
		m_pEndStepScrollBar = pScrollBar;
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
}

CItemSlot* CQuestMainEndRenewal::CreateItemSlot ( CString strKeyword, UIGUID ControlID )
{
	CItemSlot* pItemSlot = new CItemSlot( m_pGaeaClient, m_pInterface, m_pEngineDevice );
	pItemSlot->CreateSub ( this, strKeyword.GetString (), UI_FLAG_DEFAULT, ControlID );
	pItemSlot->CreateSubControl ( EQuestX, TRUE );
	RegisterControl ( pItemSlot );
	return pItemSlot;
}

CItemImage* CQuestMainEndRenewal::CreateIndemnifyItemSlot ( CString strKeyword, UIGUID ControlID )
{
	CItemImage* pItemSlot = new CItemImage( m_pGaeaClient, m_pEngineDevice );
	pItemSlot->CreateSub ( this, strKeyword.GetString (), UI_FLAG_DEFAULT, ControlID );
	pItemSlot->CreateSubControl();
	RegisterControl ( pItemSlot );
	return pItemSlot;
}

CBasicTextButton* CQuestMainEndRenewal::CreateTextButton ( char* szButton, UIGUID ControlID , char* szText )
{
	const int nBUTTONSIZE = CBasicTextButton::SIZE14;
	CBasicTextButton* pButton = new CBasicTextButton(m_pEngineDevice);
	pButton->CreateSub ( this, "BASIC_TEXT_BUTTON14", UI_FLAG_XSIZE, ControlID );
	pButton->CreateBaseButton ( szButton, nBUTTONSIZE, CBasicButton::CLICK_FLIP, szText );
	RegisterControl ( pButton );
	return pButton;
}

CUIControl*	CQuestMainEndRenewal::CreateControl ( char* szControl )
{
	CUIControl* pControl = new CUIControl(m_pEngineDevice);
	pControl->CreateSub ( this, szControl );
	RegisterControl ( pControl );
	return pControl;
}


CBasicTextBox* CQuestMainEndRenewal::CreateStaticControl (const char* szControlKeyword, CD3DFontPar* pFont, D3DCOLOR dwColor, int nAlign )
{
	CBasicTextBox* pStaticText = new CBasicTextBox(m_pEngineDevice);
	pStaticText->CreateSub ( this, szControlKeyword );
	pStaticText->SetFont ( pFont );
	pStaticText->SetTextAlign ( nAlign );
	RegisterControl ( pStaticText );
	return pStaticText;
}

bool CQuestMainEndRenewal::LoadQuestMain( DWORD dwQuestID )
{
	if ( dwQuestID == NATIVEID_NULL().dwID ) return false;

	m_pTitle->ClearText();
	m_pEndResultText->ClearText();
	m_pEndExplainText->ClearText();
	m_pEndMoreText->ClearText();
	m_pEndStepText->ClearText();
	m_pIndemnifyText->ClearText ();

	m_dwQuestID = dwQuestID;

	GLQuestPlay& cQuestPlay = m_pGaeaClient->GetCharacter ()->m_cQuestPlay;
	//GLQUESTPROG* pQuestProg = cQuestPlay.FindProc ( dwQuestID );

	GLQuestPlay::MAPQUEST QuestEndMap = cQuestPlay.GetQuestEnd ();
	GLQuestPlay::MAPQUEST_ITER iter = QuestEndMap.begin ();
	GLQuestPlay::MAPQUEST_ITER iter_end = QuestEndMap.end ();

	iter = QuestEndMap.find( dwQuestID );

	GLQUESTPROG* pQuestProg = (*iter).second;

	if ( !pQuestProg ) return false;

	GLQUEST* pQuest = GLQuestMan::GetInstance().Find ( dwQuestID );
	if ( !pQuest ) return false;	

	const DWORD dwSTEP = pQuestProg->m_dwSTEP;
	GLQUEST_STEP* pQuestStep = pQuest->GetSTEP ( dwSTEP );

	GLQUESTPROG_STEP& sSTEP_PROG = pQuestProg->m_sSTEP_PROG;
	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		CString strKey;
		strKey.Format("QN_%03d_%03d", pQuest->m_sNID.wMainID, pQuest->m_sNID.wSubID);
		m_pTitle->SetText(CGameTextMan::GetInstance().GetQuestText(strKey.GetString()).GetString(),NS_UITEXTCOLOR::WHITE);
	}
	else
	{
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
	}

	bool bComplete = pQuestProg->m_bCOMPLETE;
	if ( bComplete )
	{
		m_pDoneIcon->SetVisibleSingle( TRUE );
		m_pFailIcon->SetVisibleSingle( FALSE );
	}
	else
	{
		m_pDoneIcon->SetVisibleSingle( FALSE );
		m_pFailIcon->SetVisibleSingle( TRUE );
	}

	// 수행결과.
	{
		DWORD dwCount = pQuestProg->m_dwCOUNT;
		if ( bComplete )
		{
			CString strCombine;
			strCombine.Format( "%s / %s : %d", ID2GAMEWORD( "QUEST_LIST_RESULT", 1 )
				, ID2GAMEWORD( "QUEST_LIST_RESULT", 3 ), dwCount );

			m_pEndResultText->AddText( strCombine, NS_UITEXTCOLOR::PALEGREEN );
		}
		else
		{
			CString strCombine;
			strCombine.Format( "%s / %s : %d", ID2GAMEWORD( "QUEST_LIST_RESULT", 2 )
				, ID2GAMEWORD( "QUEST_LIST_RESULT", 3 ), dwCount );

			m_pEndResultText->AddText( strCombine, NS_UITEXTCOLOR::RED );
		}
	}

	//	내용
	{	
		if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
		{
			CString strKey;
			strKey.Format("QD_%03d_%03d", pQuest->m_sNID.wMainID, pQuest->m_sNID.wSubID);
			m_pEndExplainText->SetText( CGameTextMan::GetInstance().GetQuestText(strKey.GetString()).GetString(), NS_UITEXTCOLOR::WHITE );
		}
		else
		{
			m_pEndExplainText->SetText( pQuest->m_strCOMMENT.c_str(), NS_UITEXTCOLOR::WHITE );
		}
		m_pEndExplainScrollBar->GetThumbFrame()->SetPercent ( 0.0f );
	}

	// 자세히
	{
		if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
		{
			CString strKey;
			strKey.Format("QD_%03d_%03d_D_%d", pQuest->m_sNID.wMainID, pQuest->m_sNID.wSubID, dwSTEP);
			m_pEndMoreText->SetText (CGameTextMan::GetInstance().GetQuestText(strKey.GetString()).GetString(),NS_UITEXTCOLOR::WHITE);
		}
		else
		{
			m_pEndMoreText->SetText ( pQuestStep->GetCOMMENT (), NS_UITEXTCOLOR::WHITE );
		}
		m_pEndMoreScrollBar->GetThumbFrame()->SetPercent ( 0.0f );
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

            D3DCOLOR dwColor = NS_UITEXTCOLOR::DARKGRAY;
            int nImageType = QUEST_CHECK;

			int nIndex;
			if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
			{
				CString strKey;
				strKey.Format("QN_%03d_%03d_P_%d", pQuest->m_sNID.wMainID, pQuest->m_sNID.wSubID, i);
				nIndex = m_pEndStepText->AddText ( CGameTextMan::GetInstance().GetQuestText(strKey.GetString()).GetString(), dwColor );
			}
			else
			{
				nIndex = m_pEndStepText->AddText ( sQuestStep.GetTITLE (), dwColor );
			}
			m_pEndStepText->SetTextImage ( nIndex, strCheckKeyword[nImageType] );
		}
	}

	{
		CBasicScrollThumbFrame* const pThumbFrame = m_pEndStepScrollBar->GetThumbFrame ();

		const int nTotalLine = m_pEndStepText->GetTotalLine ();
		const int nLinePerOneView = m_pEndStepText->GetVisibleLine ();		
		pThumbFrame->SetState ( nTotalLine, nLinePerOneView );


		m_pEndStepText->SetCurLine ( 0 );
		pThumbFrame->SetPercent ( 0.0f );
		const int nCount = m_pEndStepText->GetCount () - 1;
		if ( nLinePerOneView < nTotalLine )
		{			
			float fPercent = (float)dwSTEP / (float)nCount;
			pThumbFrame->SetPercent ( fPercent );
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
			//return true;
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

	return true;
}

void CQuestMainEndRenewal::UnLoadMain()
{
	m_pDoneIcon->SetVisibleSingle( FALSE );
	m_pFailIcon->SetVisibleSingle( FALSE );

	m_pTitle->ClearText();
	m_pEndExplainText->ClearText();
	m_pEndMoreText->ClearText();
	m_pEndStepText->ClearText();

	for ( int i = 0; i < nINDEMNIFY_SLOT_TOTAL; i++ )
	{
		m_pIndemnifyItem[ i ]->ResetItem();
		m_sID[ i ] = NATIVEID_NULL();
	}
	m_pIndemnifyText->ClearText ();
}

void CQuestMainEndRenewal::SetVisibleSingle ( BOOL bVisible )
{
	CUIGroup::SetVisibleSingle ( bVisible );
}

void CQuestMainEndRenewal::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	{
		CBasicScrollThumbFrame* const pThumbFrame = m_pEndExplainScrollBar->GetThumbFrame ();

		const int nTotalLine = m_pEndExplainText->GetTotalLine ();
		const int nLinePerOneView = m_pEndExplainText->GetVisibleLine ();
		CDebugSet::ToView ( 1, 10, "보이는 라인 %d", nLinePerOneView );

		pThumbFrame->SetState ( nTotalLine, nLinePerOneView );
		if ( nLinePerOneView < nTotalLine )
		{
			const int nMovableLine = nTotalLine - nLinePerOneView;
			float fPercent = pThumbFrame->GetPercent ();
			int nPos = (int)floor(fPercent * nMovableLine);
			m_pEndExplainText->SetCurLine ( nPos );			
		}
	}

	{
		CBasicScrollThumbFrame* const pThumbFrame = m_pEndMoreScrollBar->GetThumbFrame ();

		const int nTotalLine = m_pEndMoreText->GetTotalLine ();
		const int nLinePerOneView = m_pEndMoreText->GetVisibleLine ();
		CDebugSet::ToView ( 1, 10, "보이는 라인 %d", nLinePerOneView );

		pThumbFrame->SetState ( nTotalLine, nLinePerOneView );
		if ( nLinePerOneView < nTotalLine )
		{
			const int nMovableLine = nTotalLine - nLinePerOneView;
			float fPercent = pThumbFrame->GetPercent ();
			int nPos = (int)floor(fPercent * nMovableLine);
			m_pEndMoreText->SetCurLine ( nPos );			
		}
	}

	{
		CBasicScrollThumbFrame* const pThumbFrame = m_pEndStepScrollBar->GetThumbFrame ();

		const int nTotalLine = m_pEndStepText->GetTotalLine ();
		const int nLinePerOneView = m_pEndStepText->GetVisibleLine ();
		CDebugSet::ToView ( 1, 10, "보이는 라인 %d", nLinePerOneView );

		pThumbFrame->SetState ( nTotalLine, nLinePerOneView );
		if ( nLinePerOneView < nTotalLine )
		{
			const int nMovableLine = nTotalLine - nLinePerOneView;
			float fPercent = pThumbFrame->GetPercent ();
			int nPos = (int)floor(fPercent * nMovableLine);
			m_pEndStepText->SetCurLine ( nPos );			
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
		if ( nINDEMNIFY_SLOT_TOTAL < m_nTotalLine )
		{
			const int nMovableLine = m_nTotalLine - nINDEMNIFY_SLOT_TOTAL;
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

void CQuestMainEndRenewal::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	CUIGroup::TranslateUIMessage ( ControlID, dwMsg );

// 	switch ( ControlID )
// 	{
// 	}

	for ( int i = QUEST_MAIN_INDEMNIFY_ITEM0; i < QUEST_MAIN_INDEMNIFY_ITEM_END; i++ )
	{
		if ( ControlID == i )
		{
			if ( CHECK_MOUSE_IN( dwMsg ) )
			{
				SNATIVEID sNativeID = m_sID[ i - QUEST_MAIN_INDEMNIFY_ITEM0 ];

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
				}
			}
		}
	}
}
