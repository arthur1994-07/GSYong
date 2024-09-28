#include "StdAfx.h"
#include "QuestMain.h"
#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/DxTools/DebugSet.h"
#include "../../../EngineLib/GUInterface/BasicLineBox.h"
#include "../../../EngineLib/GUInterface/BasicTextBoxEx.h"
#include "../../../EngineLib/GUInterface/BasicScrollBarEx.h"
#include "../../../EngineLib/GUInterface/BasicScrollThumbFrame.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../Item/ItemSlot.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"

#include "../../../RanLogic/Quest/GLQuestPlay.h"
#include "../../../RanLogic/Quest/GLQuest.h"
#include "../../../RanLogic/Quest/GLQuestMan.h"
#include "../../../RanLogic/GLogicDataMan.h"
#include "../../../RanLogic/GLUseFeatures.h"

#include "../../../RanLogicClient/GLGaeaClient.h"

#include "../../InnerInterface.h"
#include "QuestGiftView.h"

#include "../../../RanLogicClient/Tutorial/GLBaseTutorial.h" // by luxes.

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

const	float	CQuestMain::fANSWER_LINE_INTERVAL = 5.0f;
const	DWORD	CQuestMain::dwMINITE_BY_HOUR = 60;

CQuestMain::CQuestMain(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
    , m_pInterface(pInterface)
    , m_pGaeaClient(pGaeaClient)
{
}

CQuestMain::~CQuestMain ()
{
}

void	CQuestMain::CreateSubControl ()
{
	CD3DFontPar* pFont9 = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );
	
	CBasicLineBox* pBasicLineBox = new CBasicLineBox(m_pEngineDevice);
	pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_QUEST_MAIN_DESC", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pBasicLineBox->CreateBaseBoxQuestStepDesc ( "QUEST_MAIN_DESC_BACK" );
	RegisterControl ( pBasicLineBox );
	m_pBasicLinebox = pBasicLineBox;

	//	�ؽ�Ʈ �ڽ�
	{
		CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
		pTextBox->CreateSub ( this, "QUEST_MAIN_DESC_TEXTBOX", UI_FLAG_DEFAULT, QUEST_MAIN_DESC_TEXTBOX );
		pTextBox->SetFont ( pFont9 );
		pTextBox->SetTextAlign ( TEXT_ALIGN_BOTH_X );
		RegisterControl ( pTextBox );
		m_pDescText = pTextBox;

		int nTotalLine = pTextBox->GetVisibleLine ();

		//	��ũ�ѹ�
		CBasicScrollBarEx* pScrollBar = new CBasicScrollBarEx(m_pEngineDevice);
		pScrollBar->CreateSub ( this, "BASIC_SCROLLBAR", UI_FLAG_RIGHT | UI_FLAG_YSIZE, QUEST_MAIN_DESC_SCROLLBAR );
		pScrollBar->CreateBaseScrollBar ( "QUEST_MAIN_DESC_SCROLLBAR" );
		pScrollBar->GetThumbFrame()->SetState ( 1, nTotalLine );
		RegisterControl ( pScrollBar );
		m_pDescScrollBar = pScrollBar;
	}

	//	����
	{
		CBasicTextBoxEx* pTextBox = new CBasicTextBoxEx(m_pEngineDevice);
		pTextBox->CreateSub ( this, "QUEST_MAIN_CONDITION", UI_FLAG_DEFAULT, QUEST_MAIN_CONDITION );
		pTextBox->SetFont ( pFont9 );
		RegisterControl ( pTextBox );
		m_pCondition = pTextBox;
	}

	{
		//	�ؽ�Ʈ �ڽ�
		CBasicTextBoxEx* pTextBox = new CBasicTextBoxEx(m_pEngineDevice);
		pTextBox->CreateSub ( this, "QUEST_MAIN_TEXTBOX", UI_FLAG_DEFAULT, QUEST_MAIN_TEXTBOX );
		pTextBox->SetFont ( pFont9 );
		pTextBox->SetLineInterval ( fANSWER_LINE_INTERVAL );
		RegisterControl ( pTextBox );
		m_pMainText = pTextBox;

		int nTotalLine = pTextBox->GetVisibleLine ();

		//	��ũ�ѹ�
		CBasicScrollBarEx* pScrollBar = new CBasicScrollBarEx(m_pEngineDevice);
		pScrollBar->CreateSub ( this, "BASIC_SCROLLBAR", UI_FLAG_RIGHT | UI_FLAG_YSIZE, QUEST_MAIN_SCROLLBAR );
		pScrollBar->CreateBaseScrollBar ( "QUEST_MAIN_SCROLLBAR" );
		pScrollBar->GetThumbFrame()->SetState ( 1, nTotalLine );
		RegisterControl ( pScrollBar );
		m_pMainScrollBar = pScrollBar;
	}

	{	//	������ Ÿ��Ʋ BACK �̹���
		CreateControl ( "QUEST_MAIN_ITEM_TITLE_BACK_L" );
		CreateControl ( "QUEST_MAIN_ITEM_TITLE_BACK_R" );

		CBasicTextBox* pTitle = CreateStaticControl ( "QUEST_MAIN_ITEM_TITLE", pFont9, NS_UITEXTCOLOR::DEFAULT, TEXT_ALIGN_CENTER_X );
		pTitle->AddText ( ID2GAMEWORD("QUEST_MAIN_ITEM_TITLE") );

		CBasicLineBox* pBasicLineBox = new CBasicLineBox(m_pEngineDevice);
		pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_QUEST_MAIN_ITEM", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pBasicLineBox->CreateBaseBoxQuestStepItem ( "QUEST_MAIN_ITEM_BACK" );
		RegisterControl ( pBasicLineBox );

		m_pItemSlot = CreateItemSlot ( "QUEST_MAIN_ITEMSLOT", QUEST_MAIN_ITEMSLOT );
	}

	{
		m_pButtonL = CreateTextButton ( "QUEST_MAIN_BUTTON_L", QUEST_MAIN_BUTTON_L, (char*)ID2GAMEWORD("QUEST_MAIN_BUTTON",0) );        
		m_pButtonM = CreateTextButton ( "QUEST_MAIN_BUTTON_M", QUEST_MAIN_BUTTON_M, (char*)ID2GAMEWORD("QUEST_MAIN_BUTTON",1) );		
		m_pButtonR = CreateTextButton ( "QUEST_MAIN_BUTTON_R", QUEST_MAIN_BUTTON_R, (char*)ID2GAMEWORD("QUEST_MAIN_BUTTON",2) );
        
		m_pButtonCOMPLETE = CreateTextButton ( "QUEST_MAIN_BUTTON_COMPLETE", QUEST_MAIN_BUTTON_COMPLETE, (char*)ID2GAMEWORD("QUEST_MAIN_BUTTON",3) );
		m_pButtonGIFT = CreateTextButton ( "QUEST_MAIN_BUTTON_GIFT", QUEST_MAIN_BUTTON_GIFT, (char*)ID2GAMEWORD("QUEST_MAIN_BUTTON",4) );
	}

	{
		CQuestGiftView*	pGiftView = new CQuestGiftView(m_pEngineDevice);
		pGiftView->CreateSub ( this, "QUEST_GIFT_VIEW", UI_FLAG_DEFAULT, QUEST_MAIN_GIFT_VIEW );
		pGiftView->CreateSubControl ();
		pGiftView->SetVisibleSingle ( FALSE );
		RegisterControl ( pGiftView );
		m_pGiftView = pGiftView;
	}
}

CItemSlot* CQuestMain::CreateItemSlot ( CString strKeyword, UIGUID ControlID )
{
	CItemSlot* pItemSlot = new CItemSlot ( m_pGaeaClient, m_pInterface, m_pEngineDevice );
	pItemSlot->CreateSub ( this, strKeyword.GetString (), UI_FLAG_DEFAULT, ControlID );
	pItemSlot->CreateSubControl ( EQuestX, TRUE );
	RegisterControl ( pItemSlot );
	return pItemSlot;
}

CBasicTextButton* CQuestMain::CreateTextButton ( char* szButton, UIGUID ControlID , char* szText )
{
	const int nBUTTONSIZE = CBasicTextButton::SIZE14;
	CBasicTextButton* pButton = new CBasicTextButton(m_pEngineDevice);
	pButton->CreateSub ( this, "BASIC_TEXT_BUTTON14", UI_FLAG_XSIZE, ControlID );
	pButton->CreateBaseButton ( szButton, nBUTTONSIZE, CBasicButton::CLICK_FLIP, szText );
	RegisterControl ( pButton );
	return pButton;
}

CUIControl*	CQuestMain::CreateControl ( char* szControl )
{
	CUIControl* pControl = new CUIControl(m_pEngineDevice);
	pControl->CreateSub ( this, szControl );
	RegisterControl ( pControl );
	return pControl;
}


CBasicTextBox* CQuestMain::CreateStaticControl (const char* szControlKeyword, CD3DFontPar* pFont, D3DCOLOR dwColor, int nAlign )
{
	CBasicTextBox* pStaticText = new CBasicTextBox(m_pEngineDevice);
	pStaticText->CreateSub ( this, szControlKeyword );
	pStaticText->SetFont ( pFont );
	pStaticText->SetTextAlign ( nAlign );
	RegisterControl ( pStaticText );
	return pStaticText;
}

void CQuestMain::SetVisibleSingle ( BOOL bVisible )
{
	CUIGroup::SetVisibleSingle ( bVisible );

	if ( bVisible ) m_pGiftView->SetVisibleSingle ( FALSE );
}

void CQuestMain::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{	
	//	NOTE
	//		����Ʈ ����ġ�� �����ٶ��� ����Ʈ���ο� �ٸ� ��Ʈ���� ������Ʈ ���� �ʴ´�.
	if ( m_pGiftView->IsVisible () )
	{
		m_pGiftView->Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
		return ;
	}

	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	{
		CBasicScrollThumbFrame* const pThumbFrame = m_pDescScrollBar->GetThumbFrame ();

		const int nTotalLine = m_pDescText->GetTotalLine ();
		const int nLinePerOneView = m_pDescText->GetVisibleLine ();
		CDebugSet::ToView ( 1, 10, "���̴� ���� %d", nLinePerOneView );

		pThumbFrame->SetState ( nTotalLine, nLinePerOneView );
		if ( nLinePerOneView < nTotalLine )
		{
			const int nMovableLine = nTotalLine - nLinePerOneView;
			float fPercent = pThumbFrame->GetPercent ();
			int nPos = (int)floor(fPercent * nMovableLine);
			m_pDescText->SetCurLine ( nPos );			
		}
	}
	{
		CBasicScrollThumbFrame* const pThumbFrame = m_pMainScrollBar->GetThumbFrame ();

		const int nTotalLine = m_pMainText->GetTotalLine ();
		const int nLinePerOneView = m_pMainText->GetVisibleLine ();
		CDebugSet::ToView ( 1, 10, "���̴� ���� %d", nLinePerOneView );

		pThumbFrame->SetState ( nTotalLine, nLinePerOneView );
		if ( nLinePerOneView < nTotalLine )
		{
			const int nMovableLine = nTotalLine - nLinePerOneView;
			float fPercent = pThumbFrame->GetPercent ();
			int nPos = (int)floor(fPercent * nMovableLine);
			m_pMainText->SetCurLine ( nPos );			
		}
	}
}

void CQuestMain::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	CUIGroup::TranslateUIMessage ( ControlID, dwMsg );

	switch ( ControlID )
	{
	case QUEST_MAIN_BUTTON_L:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				AddMessageEx ( UIMSG_BUTTON_GIVEUP );
			}
		}
		break;
	case QUEST_MAIN_BUTTON_M:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				if ( m_pGaeaClient->GetTutorial()->IsTutorial()
					&& m_pGaeaClient->GetTutorial()->IsQuestWindowStep()
					&& !m_pGaeaClient->GetTutorial()->IsSubSixStep() ) // Ʃ�丮�� �߿��� ������.
				{
					m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("TUTORIAL_CANNOT_USE_YET") );
				}
				else
					AddMessageEx ( UIMSG_BUTTON_GO_QUESTSTEP );
			}
		}
		break;
	case QUEST_MAIN_BUTTON_R:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				if ( m_pGaeaClient->GetTutorial()->IsTutorial()
					&& m_pGaeaClient->GetTutorial()->IsQuestWindowStep()
					&& m_pGaeaClient->GetTutorial()->GetCurrentSubStep() >= m_pGaeaClient->GetTutorial()->THREE_
					&& m_pGaeaClient->GetTutorial()->GetCurrentSubStep() <= m_pGaeaClient->GetTutorial()->SIX_ ) // Ʃ�丮�� �߿��� ������.
				{
					m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("TUTORIAL_CANNOT_USE_YET") );
				}
				else
					AddMessageEx ( UIMSG_BUTTON_BACK );
			}
		}
		break;
	
	case QUEST_MAIN_BUTTON_COMPLETE:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				AddMessageEx ( UIMSG_BUTTON_COMPLETE );
			}
		}
		break;

	case QUEST_MAIN_ITEMSLOT:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				int nPosX = m_pItemSlot->GetItemIndex ();
				if (nPosX < 0)
                    return;
				const SINVENITEM* pInvenItem = GetItem ( nPosX );
				{				
					if (pInvenItem && (pInvenItem->sItemCustom.GetNativeID() != NATIVEID_NULL()))
					{
						m_pInterface->SHOW_ITEM_INFO_SIMPLE ( pInvenItem->sItemCustom );
					}
				}
			}
		}
		break;

	case QUEST_MAIN_TEXTBOX:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( UIMSG_LB_DUP & dwMsg )
				{
					if ( m_pGaeaClient->GetTutorial()->IsTutorial()
						&& m_pGaeaClient->GetTutorial()->IsQuestWindowStep()
						&& !m_pGaeaClient->GetTutorial()->IsSubSixStep() ) // Ʃ�丮�� �߿��� ������.
					{
						m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("TUTORIAL_CANNOT_USE_YET") );
					}
					else
						AddMessageEx ( UIMSG_BUTTON_GO_QUESTSTEP );
				}
			}
		}
		break;

	case QUEST_MAIN_BUTTON_GIFT:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				m_pGiftView->SetVisibleSingle ( TRUE );
				m_pGiftView->SetQuestID ( m_dwQuestID );
			}
		}
		break;
	}
}

bool	CQuestMain::LoadQuestMain ( DWORD dwQuestID )
{
	if ( dwQuestID == NATIVEID_NULL().dwID ) return false;

	m_dwQuestID = dwQuestID;
	
	GLQuestPlay& cQuestPlay = m_pGaeaClient->GetCharacter ()->m_cQuestPlay;
	GLQUESTPROG* pQuestProg = cQuestPlay.FindProc ( dwQuestID );
	if ( !pQuestProg ) return false;

	GLQUEST* pQuest = GLQuestMan::GetInstance().Find ( dwQuestID );
	if ( !pQuest ) return false;	

	const DWORD dwSTEP = pQuestProg->m_dwSTEP;
	GLQUEST_STEP* pQuestStep = pQuest->GetSTEP ( dwSTEP );		

	//	����
	{	
		if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
		{
			CString strKey;
			strKey.Format("QD_%03d_%03d", pQuest->m_sNID.wMainID, pQuest->m_sNID.wSubID);
			m_pDescText->SetText( CGameTextMan::GetInstance().GetQuestText(strKey.GetString()).GetString(), NS_UITEXTCOLOR::WHITE );
		}
		else
		{
			m_pDescText->SetText( pQuest->m_strCOMMENT.c_str(), NS_UITEXTCOLOR::WHITE );
		}
		m_pDescScrollBar->GetThumbFrame()->SetPercent ( 0.0f );
	}

	//	����
	{
		m_pCondition->ClearText ();				
		CString strCombine;		
		CString strConditionLine1, strConditionLine2;

		if ( pQuest->m_dwLimitTime )
		{
			const DWORD dwLimitTime = pQuest->m_dwLimitTime;
			const DWORD dwLimitHour = dwLimitTime / dwMINITE_BY_HOUR;
			const DWORD dwLimitMinute = dwLimitTime % dwMINITE_BY_HOUR;

			const float fLapsTime = pQuestProg->m_fLAPSTIME;
			const DWORD dwLapsTime = ((DWORD)fLapsTime) / dwMINITE_BY_HOUR;	// ��->�д���
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
// 		if ( pQuest->m_dwLimitPartyMemNum )
// 		{
// 			if ( strConditionLine1.GetLength () )
// 				strCombine.Format ( ", %s : %d", ID2GAMEWORD("QUEST_MAIN_CONDITION",1), pQuest->m_dwLimitPartyMemNum );
// 			else
// 				strCombine.Format ( "%s : %d", ID2GAMEWORD("QUEST_MAIN_CONDITION",1), pQuest->m_dwLimitPartyMemNum );
// 			strConditionLine1 += strCombine;			
// 		}

		//	ù��° ��
		if ( strConditionLine1.GetLength () )
		{
			strCombine.Format ( "%s - %s", ID2GAMEWORD("QUEST_CONDITION"), strConditionLine1 );
			m_pCondition->AddText ( strCombine, NS_UITEXTCOLOR::ORANGE );
		}

		//	�ι�° ��
		if ( pQuest->m_bNonDie )
		{
			strConditionLine2 = ID2GAMEWORD("QUEST_MAIN_CONDITION",2);
			if ( strConditionLine1.GetLength () )	//	�� ������ ���� ���
			{
				std::string strCondition = ID2GAMEWORD("QUEST_CONDITION");
				std::string strBlank ( strCondition.size(), ' ' );

				strCombine.Format ( "%s - %s", strBlank.c_str(), strConditionLine2 );				
			}
			else
			{
				strCombine.Format ( "%s - %s", ID2GAMEWORD("QUEST_CONDITION"), strConditionLine2 );				
			}

			m_pCondition->AddText ( strCombine, NS_UITEXTCOLOR::ORANGE );
		}
	}

	//	���� ��Ȳ
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

		m_pMainText->ClearText ();
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
				nIndex = m_pMainText->AddText ( CGameTextMan::GetInstance().GetQuestText(strKey.GetString()).GetString(), dwColor );
			}
			else
			{
				nIndex = m_pMainText->AddText ( sQuestStep.GetTITLE (), dwColor );
			}
			m_pMainText->SetTextImage ( nIndex, strCheckKeyword[nImageType] );
		}

		CBasicScrollThumbFrame* const pThumbFrame = m_pMainScrollBar->GetThumbFrame ();

		const int nTotalLine = m_pMainText->GetTotalLine ();
		const int nLinePerOneView = m_pMainText->GetVisibleLine ();		
		pThumbFrame->SetState ( nTotalLine, nLinePerOneView );


		m_pMainText->SetCurLine ( 0 );
		pThumbFrame->SetPercent ( 0.0f );
		const int nCount = m_pMainText->GetCount () - 1;
		if ( nLinePerOneView < nTotalLine )
		{			
			float fPercent = (float)dwSTEP / (float)nCount;
			pThumbFrame->SetPercent ( fPercent );
		}
	}

	//	������ ����
	{
		//	Ŭ����
		for ( int i = 0; i < EQuestX; ++i )
		{
			UnLoadItem ( i );
		}

		const GLInventory::CELL_MAP *ItemList = pQuestProg->m_sINVENTORY.GetItemList ();

		if ( ItemList->size () > EQuestTotalSlot )
		{
			GASSERT ( 0 && "�������̽� �������, ����Ÿũ�Ⱑ Ů�ϴ�." );
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

	//	�켱������ �߿���.
	//	�Ϸᰡ ���⿡ �켱�Ѵ�.
	//	����, ����Ʈ�� �Ϸ�Ǿ��ٸ�, ����� �Ѹ� �ʿ䰡 ����.
	m_pButtonCOMPLETE->SetVisibleSingle ( FALSE );
	m_pButtonL->SetVisibleSingle ( FALSE );
	
	if ( pQuestProg->CheckCOMPLETE() )	//	�Ϸ��ư�� ������ �ϴ� ����Ʈ
	{		
		m_pButtonCOMPLETE->SetVisibleSingle ( TRUE );
		return true;
	}	
	else if ( pQuest->IsGIVEUP () )	//	���� ����Ʈ
	{
		m_pButtonL->SetVisibleSingle ( TRUE );
		return true;
	}

	return true;
}

void CQuestMain::LoadItem ( SINVENITEM& ref_InvenItem )
{	
	int nPosX = ref_InvenItem.wPosX;
//	int nPosY = ref_InvenItem.wPosY;
	m_pItemSlot->SetItemImage ( nPosX, ref_InvenItem );
}

const SINVENITEM* CQuestMain::GetItem(int nPosX) const
{
	return m_pItemSlot->GetItemImage ( nPosX );
}

void CQuestMain::UnLoadItem ( int nPosX )
{
	m_pItemSlot->ResetItemImage ( nPosX );
}
