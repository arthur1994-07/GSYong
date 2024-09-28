#include "StdAfx.h"
#include "QuestStep.h"
#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/BasicLineBox.h"
#include "../../../EngineLib/GUInterface/BasicTextBoxEx.h"
#include "../../../EngineLib/GUInterface/BasicScrollBarEx.h"
#include "../../../EngineLib/GUInterface/BasicScrollThumbFrame.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../Item/ItemSlot.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../RanLogic/Quest/GLQuestMan.h"
#include "../../../RanLogic/Quest/GLQuest.h"
#include "../../../RanLogic/Quest/GLQuestPlay.h"
#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogic/GLogicDataMan.h"

#include "../../InnerInterface.h"

#include "../../../RanLogicClient/Tutorial/GLBaseTutorial.h" // by luxes.

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

const	float	CQuestStep::fANSWER_LINE_INTERVAL = 5.0f;

CQuestStep::CQuestStep(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
	: CUIGroup(pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
    , m_pInterface(pInterface)
{
}

CQuestStep::~CQuestStep ()
{
}

void	CQuestStep::CreateSubControl ()
{
	CD3DFontPar* pFont9 = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );
	
	CBasicLineBox* pBasicLineBox = new CBasicLineBox(m_pEngineDevice);
	pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_QUEST_STEP_DESC", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pBasicLineBox->CreateBaseBoxQuestRunDesc ( "QUEST_STEP_DESC_BACK" );
	RegisterControl ( pBasicLineBox );

	//	텍스트 박스
	{
		CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
		pTextBox->CreateSub ( this, "QUEST_STEP_DESC_TEXTBOX", UI_FLAG_DEFAULT, QUEST_STEP_DESC_TEXTBOX );
		pTextBox->SetFont ( pFont9 );		
		pTextBox->SetTextAlign ( TEXT_ALIGN_BOTH_X );
		RegisterControl ( pTextBox );
		m_pDescText = pTextBox;

		int nTotalLine = pTextBox->GetVisibleLine ();

		//	스크롤바
		CBasicScrollBarEx* pScrollBar = new CBasicScrollBarEx(m_pEngineDevice);
		pScrollBar->CreateSub ( this, "BASIC_SCROLLBAR", UI_FLAG_RIGHT | UI_FLAG_YSIZE, QUEST_STEP_DESC_SCROLLBAR );
		pScrollBar->CreateBaseScrollBar ( "QUEST_STEP_DESC_SCROLLBAR" );
		pScrollBar->GetThumbFrame()->SetState ( 1, nTotalLine );
		RegisterControl ( pScrollBar );
		m_pDescScrollBar = pScrollBar;
	}

	{
		//	텍스트 박스
		CBasicTextBoxEx* pTextBox = new CBasicTextBoxEx(m_pEngineDevice);
		pTextBox->CreateSub ( this, "QUEST_STEP_TEXTBOX", UI_FLAG_DEFAULT, QUEST_STEP_TEXTBOX );
		pTextBox->SetFont ( pFont9 );
		pTextBox->SetLineInterval ( fANSWER_LINE_INTERVAL );
		RegisterControl ( pTextBox );
		m_pRunText = pTextBox;

		int nTotalLine = pTextBox->GetVisibleLine ();

		//	스크롤바
		CBasicScrollBarEx* pScrollBar = new CBasicScrollBarEx(m_pEngineDevice);
		pScrollBar->CreateSub ( this, "BASIC_SCROLLBAR", UI_FLAG_RIGHT | UI_FLAG_YSIZE, QUEST_STEP_SCROLLBAR );
		pScrollBar->CreateBaseScrollBar ( "QUEST_STEP_SCROLLBAR" );
		pScrollBar->GetThumbFrame()->SetState ( 1, nTotalLine );
		RegisterControl ( pScrollBar );
		m_pRunScrollBar = pScrollBar;
	}

	{	//	아이템 타이틀 BACK 이미지
		CreateControl ( "QUEST_STEP_ITEM_TITLE_BACK_L" );
		CreateControl ( "QUEST_STEP_ITEM_TITLE_BACK_R" );

		CBasicTextBox* pTitle = CreateStaticControl ( "QUEST_STEP_ITEM_TITLE", pFont9, NS_UITEXTCOLOR::DEFAULT, TEXT_ALIGN_CENTER_X );
		pTitle->AddText ( ID2GAMEWORD("QUEST_STEP_ITEM_TITLE") );

		CBasicLineBox* pBasicLineBox = new CBasicLineBox(m_pEngineDevice);
		pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_QUEST_STEP_ITEM", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pBasicLineBox->CreateBaseBoxQuestRunItem ( "QUEST_STEP_ITEM_BACK" );
		RegisterControl ( pBasicLineBox );

		m_pItemSlot = CreateItemSlot ( "QUEST_STEP_ITEMSLOT", QUEST_STEP_ITEMSLOT );
	}

	{
//		m_pButtonL = CreateTextButton ( "QUEST_STEP_BUTTON_L", QUEST_STEP_BUTTON_L, (char*)ID2GAMEWORD("QUEST_STEP_BUTTON",0) );
		m_pButtonR = CreateTextButton ( "QUEST_STEP_BUTTON_R", QUEST_STEP_BUTTON_R, (char*)ID2GAMEWORD("QUEST_STEP_BUTTON",1) );
		m_pButtonPrint = CreateTextButton ( "QUEST_STEP_BUTTON_PRINT", QUEST_STEP_BUTTON_PRINT, (char*)ID2GAMEWORD("QUEST_STEP_BUTTON",2) );

		m_pButtonCOMPLETE = CreateTextButton ( "QUEST_MAIN_BUTTON_COMPLETE", QUEST_MAIN_BUTTON_COMPLETE, (char*)ID2GAMEWORD("QUEST_MAIN_BUTTON",3) );
	}
}

CItemSlot* CQuestStep::CreateItemSlot ( CString strKeyword, UIGUID ControlID )
{
	CItemSlot* pItemSlot = new CItemSlot ( m_pGaeaClient, m_pInterface, m_pEngineDevice );
	pItemSlot->CreateSub ( this, strKeyword.GetString (), UI_FLAG_DEFAULT, ControlID );
	pItemSlot->CreateSubControl ( 8, TRUE );
	RegisterControl ( pItemSlot );
	return pItemSlot;
}

CBasicTextButton* CQuestStep::CreateTextButton ( char* szButton, UIGUID ControlID , char* szText )
{
	const int nBUTTONSIZE = CBasicTextButton::SIZE14;
	CBasicTextButton* pButton = new CBasicTextButton(m_pEngineDevice);
	pButton->CreateSub ( this, "BASIC_TEXT_BUTTON14", UI_FLAG_XSIZE, ControlID );
	pButton->CreateBaseButton ( szButton, nBUTTONSIZE, CBasicButton::CLICK_FLIP, szText );
	RegisterControl ( pButton );
	return pButton;
}

CUIControl*	CQuestStep::CreateControl ( char* szControl )
{
	CUIControl* pControl = new CUIControl(m_pEngineDevice);
	pControl->CreateSub ( this, szControl );
	RegisterControl ( pControl );
	return pControl;
}


CBasicTextBox* CQuestStep::CreateStaticControl (const char* szControlKeyword, CD3DFontPar* pFont, D3DCOLOR dwColor, int nAlign )
{
	CBasicTextBox* pStaticText = new CBasicTextBox(m_pEngineDevice);
	pStaticText->CreateSub ( this, szControlKeyword );
	pStaticText->SetFont ( pFont );
	pStaticText->SetTextAlign ( nAlign );
	RegisterControl ( pStaticText );
	return pStaticText;
}

bool	CQuestStep::LoadQuestStep ( DWORD dwQuestID )
{
	if ( dwQuestID == NATIVEID_NULL().dwID ) return false;

	GLQuestPlay& cQuestPlay = m_pGaeaClient->GetCharacter ()->m_cQuestPlay;
	GLQUESTPROG* pQuestProg = cQuestPlay.FindProc ( dwQuestID );
	if ( !pQuestProg ) return false;

	GLQUEST* pQuest = GLQuestMan::GetInstance().Find ( dwQuestID );
	if ( !pQuest ) return false;	

	const DWORD dwSTEP = pQuestProg->m_dwSTEP;
	GLQUEST_STEP* pQuestStep = pQuest->GetSTEP ( dwSTEP );

	GLQUESTPROG_STEP& sSTEP_PROG = pQuestProg->m_sSTEP_PROG;	

	//	설명
	{
		if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
		{
			CString strKey;
			strKey.Format("QN_%03d_%03d_P_%d", pQuestProg->m_sNID.wMainID, pQuestProg->m_sNID.wSubID, dwSTEP);
			m_pDescText->SetText ( CGameTextMan::GetInstance().GetQuestText(strKey.GetString()).GetString(), NS_UITEXTCOLOR::WHITE );
		}
		else
		{
			m_pDescText->SetText ( pQuestStep->GetCOMMENT (), NS_UITEXTCOLOR::WHITE );
		}
		m_pDescScrollBar->GetThumbFrame()->SetPercent ( 0.0f );
	}

	//	진행 상황
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

		m_pRunText->ClearText ();
		if ( pQuestStep->IsNEED_NPCTALK() )
		{
			D3DCOLOR dwColor = NS_UITEXTCOLOR::WHITE;
			int nImageType = QUEST_CHECK;
			if ( sSTEP_PROG.m_bTALK )
			{
				dwColor = NS_UITEXTCOLOR::DARKGRAY;
				nImageType = QUEST_CHECK_F;
			}

			int nIndex = 0;
// 			if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
// 			{
// 				SNATIVEID sNPCID = pQuestStep->m_dwNID_NPCTALK;
// 				CString strKey;
// 				strKey.Format("CN_%03d_%03d", sNPCID.Mid(), sNPCID.Sid());
// 				CString strCrowName		= CGameTextMan::GetInstance().GetText(strKey.GetString(), 0, CGameTextMan::EM_CROW_TEXT).GetString();
// 				CString strCommentText	= CGameTextMan::GetInstance().GetText("QUEST_STEP_COMMENT_TEXT", EMQSCT_NPCTALK, CGameTextMan::EM_COMMENT_TEXT).GetString();
// 				CString strText;				
// 				strText.Format("\'%s\'%s", strCrowName, strCommentText);
// 				nIndex = m_pRunText->AddText(strText, dwColor);
// 			}
// 			else
			{
				nIndex = m_pRunText->AddText ( pQuestStep->GetOBJ_NPCTALK(), dwColor );
			}
			m_pRunText->SetTextImage ( nIndex, strCheckKeyword[nImageType] );
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
			int nIndex = 0;
// 			if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
// 			{
// 				CString strCommentText = 
// 					CGameTextMan::GetInstance().GetText("QUEST_STEP_COMMENT_TEXT", EMQSCT_MOBGEN_ITEM, CGameTextMan::EM_COMMENT_TEXT).GetString();
// 				nIndex = m_pRunText->AddText ( strCommentText.GetString(), dwColor );		
// 			}
// 			else
			{
				nIndex = m_pRunText->AddText ( pQuestStep->GetOBJ_MOBGEN_QITEM(), dwColor );		
			}
			m_pRunText->SetTextImage ( nIndex, strCheckKeyword[nImageType] );
		}

		if ( pQuestStep->IsNEED_MOBKILL() || pQuestStep->IsNEED_PLAYERKILL())
		{
			D3DCOLOR dwColor = NS_UITEXTCOLOR::GOLD;
			int nImageType = QUEST_CHECK;
			if ( sSTEP_PROG.m_bMOBKILL_ALL )
			{
				dwColor = NS_UITEXTCOLOR::DARKGRAY;
				nImageType = QUEST_CHECK_F;
			}

			CString strCombine;
// 			if (GLUseFeatures::GetInstance().IsUsingDataIntegrate())
// 			{
// 				strCombine.Format ( "%s [%d/%d]"
// 					, CGameTextMan::GetInstance().GetText("QUEST_STEP_COMMENT_TEXT", EMQSCT_MOBKILL, CGameTextMan::EM_COMMENT_TEXT).GetString()
// 					, sSTEP_PROG.m_dwNUM_MOBKILL
// 					, pQuestStep->m_dwNUM_MOBKILL );
// 			}
// 			else
			{
				strCombine.Format ( "%s [%d/%d]",
					pQuestStep->GetOBJ_MOBKILL(),
					sSTEP_PROG.m_dwNUM_MOBKILL,
					pQuestStep->m_dwNUM_MOBKILL );
			}
			int nIndex = m_pRunText->AddText ( strCombine, dwColor );
			m_pRunText->SetTextImage ( nIndex, strCheckKeyword[nImageType] );
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
// 			if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
// 			{
// 				strCombine.Format ( "%s [%d/%d]"
// 					, CGameTextMan::GetInstance().GetText("QUEST_STEP_COMMENT_TEXT", EMQSCT_ITEM_USE, CGameTextMan::EM_COMMENT_TEXT).GetString()
// 					, sSTEP_PROG.m_dwNUM_ITEMUSE
// 					, pQuestStep->m_dwNUM_ITEMUSE );
// 			}
// 			else
			{
				strCombine.Format ( "%s [%d/%d]",
				pQuestStep->GetOBJ_ITEMUSE(),
				sSTEP_PROG.m_dwNUM_ITEMUSE,
				pQuestStep->m_dwNUM_ITEMUSE );
			}
			int nIndex = m_pRunText->AddText ( strCombine, dwColor );
			m_pRunText->SetTextImage ( nIndex, strCheckKeyword[nImageType] );
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
// 			if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
// 			{
// 				strCombine.Format ( "%s [%d/%d]"
// 					, CGameTextMan::GetInstance().GetText("QUEST_STEP_COMMENT_TEXT", EMQSCT_ITEM_GRINDING, CGameTextMan::EM_COMMENT_TEXT).GetString()
// 					, sSTEP_PROG.m_dwNUM_ITEMGRINDING
// 					, pQuestStep->m_dwNUM_ITEMGRINDING );
// 			}
// 			else
			{
				strCombine.Format ( "%s [%d/%d]",
					pQuestStep->GetOBJ_ITEMGRINDING(),
					sSTEP_PROG.m_dwNUM_ITEMGRINDING,
					pQuestStep->m_dwNUM_ITEMGRINDING );
			}
			int nIndex = m_pRunText->AddText ( strCombine, dwColor );
			m_pRunText->SetTextImage ( nIndex, strCheckKeyword[nImageType] );
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
			strCombine.Format ( "%s [%d/%d]",
				pQuestStep->m_strOBJ_CONFTING.c_str (),
				sSTEP_PROG.m_dwNUM_CONFTING,
				pQuestStep->m_dwNUM_CONFTING );
			int nIndex = m_pRunText->AddText ( strCombine, dwColor );
			m_pRunText->SetTextImage ( nIndex, strCheckKeyword[nImageType] );
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
// 			if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
// 			{
// 				strCombine.Format ( "%s [%d/%d]"
// 					, CGameTextMan::GetInstance().GetText("QUEST_STEP_COMMENT_TEXT", EMQSCT_LEVEL_UP_COMMENT, CGameTextMan::EM_COMMENT_TEXT).GetString()
// 					, sSTEP_PROG.m_dwNUM_LEVELUP
// 					, pQuestStep->m_dwNUM_LEVELUP );
// 			}
// 			else
			{
				strCombine.Format ( "%s [%d/%d]",
					pQuestStep->GetOBJ_LEVEL_UP(),
					sSTEP_PROG.m_dwNUM_LEVELUP,
					pQuestStep->m_dwNUM_LEVELUP );
			}
			int nIndex = m_pRunText->AddText ( strCombine, dwColor );
			m_pRunText->SetTextImage ( nIndex, strCheckKeyword[nImageType] );
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
			int nIndex = 0;
// 			if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
// 			{
// 				nIndex = m_pRunText->AddText ( 
// 					CGameTextMan::GetInstance().GetText("QUEST_STEP_COMMENT_TEXT", EMQSCT_NPCQUARD, CGameTextMan::EM_COMMENT_TEXT).GetString(), dwColor );
// 			}
// 			else
			{
				nIndex = m_pRunText->AddText ( pQuestStep->GetOBJ_NPCGUARD(), dwColor );
			}
			m_pRunText->SetTextImage ( nIndex, strCheckKeyword[nImageType] );
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
			int nIndex = 0;
// 			if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
// 			{
// 				nIndex = m_pRunText->AddText ( 
// 					CGameTextMan::GetInstance().GetText("QUEST_STEP_COMMENT_TEXT", EMQSCT_REACH_ZONE, CGameTextMan::EM_COMMENT_TEXT).GetString(), dwColor );
// 			}
// 			else
			{
				nIndex = m_pRunText->AddText ( pQuestStep->GetOBJ_REACH_ZONE(), dwColor );
			}
			m_pRunText->SetTextImage ( nIndex, strCheckKeyword[nImageType] );
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

			int nIndex = 0;
// 			if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
// 			{
// 				nIndex = m_pRunText->AddText ( CGameTextMan::GetInstance().GetText("QUEST_STEP_COMMENT_TEXT", EMQSCT_GUARD_MAP_COMMENT, CGameTextMan::EM_COMMENT_TEXT).GetString(), dwColor );
// 			}
// 			else
			{
				nIndex = m_pRunText->AddText ( pQuestStep->GetOBJ_DEFENSE_ZONE(), dwColor );
			}
			m_pRunText->SetTextImage ( nIndex, strCheckKeyword[nImageType] );
		}		
	}

	//	아이템 설정
	{
		//	클리어
		for ( int i = 0; i < EQuestX; ++i )
		{
			UnLoadItem ( i );
		}

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
			if ( !pItemData )
				continue;

			LoadItem ( *pInvenItem );
		}
	}



	//	우선순위가 중요함.
	m_pButtonCOMPLETE->SetVisibleSingle ( FALSE );	
	
	if ( pQuestProg->CheckCOMPLETE() )	//	완료버튼을 눌러야 하는 퀘스트
	{		
		m_pButtonCOMPLETE->SetVisibleSingle ( TRUE );
		m_pButtonPrint->SetVisibleSingle( FALSE );
		return true;
	}
	else
	{
		if( pQuestStep->IsNEED_QITEM() || pQuestStep->IsNEED_MOBKILL() )
			m_pButtonPrint->SetVisibleSingle( TRUE );
		else
			m_pButtonPrint->SetVisibleSingle( FALSE );

	}

	return true;
}

void	CQuestStep::TranslateUIMessage ( UIGUID cID, DWORD dwMsg )
{
	CUIGroup::TranslateUIMessage ( cID, dwMsg );

	switch ( cID )
	{
	case QUEST_STEP_BUTTON_R:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				if ( m_pGaeaClient->GetTutorial()->IsTutorial()
					&& m_pGaeaClient->GetTutorial()->IsQuestWindowStep()
					&& m_pGaeaClient->GetTutorial()->GetCurrentSubStep() >= m_pGaeaClient->GetTutorial()->SEVEN_
					&& m_pGaeaClient->GetTutorial()->GetCurrentSubStep() <= m_pGaeaClient->GetTutorial()->NINE_ ) // 튜토리얼 중에는 사용금지.
				{
					m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("TUTORIAL_CANNOT_USE_YET") );
				}
				else
					AddMessageEx ( UIMSG_BUTTON_BACK_LBUPLIKE );
			}
		}
		break;

	case QUEST_MAIN_BUTTON_COMPLETE:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				AddMessageEx ( UIMSG_BUTTON_COMPLETE_STEP );
			}
		}
		break;

	case QUEST_STEP_BUTTON_PRINT:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				AddMessageEx ( UIMSG_BUTTON_PRINT_STEP );
			}
		}
		break;
	case QUEST_STEP_ITEMSLOT:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				int nPosX = m_pItemSlot->GetItemIndex ();
				if ( nPosX < 0 ) return ;
				const SINVENITEM* pInvenItem = GetItem ( nPosX );
				{				
					if (pInvenItem && (pInvenItem->sItemCustom.GetNativeID() != NATIVEID_NULL()))
					{
						m_pInterface->SHOW_ITEM_INFO_SIMPLE(pInvenItem->sItemCustom);
					}
				}
			}
		}
		break;
	}
}


void CQuestStep::LoadItem ( SINVENITEM& ref_InvenItem )
{	
	int nPosX = ref_InvenItem.wPosX;
//	int nPosY = ref_InvenItem.wPosY;
	m_pItemSlot->SetItemImage ( nPosX, ref_InvenItem );
}

const SINVENITEM* CQuestStep::GetItem(int nPosX) const
{
	return m_pItemSlot->GetItemImage(nPosX);
}


void CQuestStep::UnLoadItem ( int nPosX )
{
	m_pItemSlot->ResetItemImage ( nPosX );
}
