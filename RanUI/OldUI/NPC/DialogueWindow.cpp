#include "StdAfx.h"
#include "../../../SigmaCore/String/StringFormat.h"
#include "../../../SigmaCore/Log/LogMan.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/GUInterface/BasicLineBox.h"
#include "../../../EngineLib/GUInterface/BasicTextBoxEx.h"
#include "../../../EngineLib/GUInterface/BasicScrollBarEx.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/BasicScrollThumbFrame.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"

#include "../../../RanLogic/Quest/GLQuestMan.h"
#include "../../../RanLogic/Quest/GLQuest.h"
#include "../../../RanLogic/Item/GLItemMan.h"
#include "../../../RanLogic/NpcTalk/NpcTalkMan.h"
#include "../../../RanLogic/NpcTalk/NpcDialogue.h"
#include "../../../RanLogic/NpcTalk/NpcDialogueCase.h"
#include "../../../RanLogic/NpcTalk/NpcTalkControl.h"
#include "../../../RanLogic/NpcTalk/NpcTalk.h"
#include "../../../RanLogic/NpcTalk/NpcTalkCondition.h"
#include "../../../RanLogic/Crow/GLCrowDataMan.h"
#include "../../../RanLogic/GLogicDataMan.h"

#include "../../../RanLogicClient/Char/GLCharacter.h"
#include "../../../RanLogicClient/Party/GLPartyClient.h"
#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Crow/GLCrowClient.h"
#include "../../../RanLogicClient/Land/GLLandManClient.h"
#include "../../../RanLogicClient/Club/GLClubClient.h"

#include "../../InnerInterface.h"
#include "../Item/InventoryWindow.h"
#include "DialogueWindow.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

const	int		CDialogueWindow::nSTARTLINE = 0;
const	int		CDialogueWindow::nSTARTINDEX = 1;
const	float	CDialogueWindow::fANSWER_PART_INTERVAL = 8.0f;

CDialogueWindow::CDialogueWindow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIWindowEx ( pInterface, pEngineDevice )
    , m_pGaeaClient(pGaeaClient)
    , m_dwRandomTime ( 0 )
    , m_fRandomTime(0.0f)
{
    m_spDialogueSet = std::tr1::shared_ptr<CNpcDialogueSet> (new CNpcDialogueSet);
}

CDialogueWindow::~CDialogueWindow ()
{
}

void CDialogueWindow::CreateSubControl ()
{
	CD3DFontPar* pFont9 = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	{
		CBasicLineBox* pBasicLineBox = new CBasicLineBox(m_pEngineDevice);
		pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_DIALOGUE", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pBasicLineBox->CreateBaseBoxDialogue ( "DIALOGUE_BACK_BOX" );
		RegisterControl ( pBasicLineBox );
	}

	{
		CBasicLineBox* pBasicLineBox = new CBasicLineBox(m_pEngineDevice);
		pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_DIALOGUE", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pBasicLineBox->CreateBaseBoxDialogueAnswer ( "DIALOGUE_ANSWER_BACK_BOX" );
		RegisterControl ( pBasicLineBox );
	}

	//	텍스트 박스
	{
		m_pQuestionBox = new CBasicTextBox(m_pEngineDevice);
		m_pQuestionBox->CreateSub ( this, "DIALOGUE_QUESTION_TEXTBOX" );
		m_pQuestionBox->SetFont ( pFont9 );		
		m_pQuestionBox->SetTextAlign ( TEXT_ALIGN_BOTH_X );		
		RegisterControl ( m_pQuestionBox );

		int nTotalLine = m_pQuestionBox->GetVisibleLine ();

		//	스크롤바
		m_pScrollBar = new CBasicScrollBarEx(m_pEngineDevice);
		m_pScrollBar->CreateSub ( this, "BASIC_SCROLLBAR", UI_FLAG_RIGHT | UI_FLAG_YSIZE, BASIC_DIALOGUE_SCROLLBAR );
		m_pScrollBar->CreateBaseScrollBar ( "BASIC_DIALOGUE_SCROLLBAR" );
		m_pScrollBar->GetThumbFrame()->SetState ( 1, nTotalLine );
		RegisterControl ( m_pScrollBar );
	}

	{
		//	텍스트 박스
		m_pAnswerBox = new CBasicTextBoxEx(m_pEngineDevice);
		m_pAnswerBox->CreateSub ( this, "DIALOGUE_ANSWER_TEXTBOX", UI_FLAG_DEFAULT, DIALOGUE_ANSWER_TEXTBOX );
		m_pAnswerBox->SetFont ( pFont9 );
		m_pAnswerBox->SetPartInterval ( fANSWER_PART_INTERVAL );
		m_pAnswerBox->SetSensitive ( true );
		RegisterControl ( m_pAnswerBox );

		int nVisibleLine = m_pAnswerBox->GetVisibleLine ();
		int nTotalLine = m_pAnswerBox->GetTotalLine ();

		//	스크롤바
		m_pAnswerScrollBar = new CBasicScrollBarEx(m_pEngineDevice);
		m_pAnswerScrollBar->CreateSub ( this, "BASIC_SCROLLBAR", UI_FLAG_RIGHT | UI_FLAG_YSIZE, BASIC_DIALOGUE_ANSWER_SCROLLBAR );
		m_pAnswerScrollBar->CreateBaseScrollBar ( "BASIC_DIALOGUE_ANSWER_SCROLLBAR" );
		m_pAnswerScrollBar->GetThumbFrame()->SetState ( nTotalLine, nVisibleLine );
		RegisterControl ( m_pAnswerScrollBar );
	}


		// RandomTime
	{
		const int nBUTTONSIZE = CBasicTextButton::SIZE16;
		m_pRandomTimeButton = new CBasicTextButton(m_pEngineDevice);
		m_pRandomTimeButton->CreateSub ( this, "BASIC_TEXT_BUTTON19", UI_FLAG_XSIZE, RANDOM_TIME_BUTTON );
		m_pRandomTimeButton->CreateBaseButton ( "RANDOM_TIME_BUTTON", nBUTTONSIZE, CBasicButton::RADIO_FLIP, ID2GAMEWORD( "RANDOM_TIME_BUTTON",0));
		m_pRandomTimeButton->SetFlip ( FALSE );
		RegisterControl ( m_pRandomTimeButton );

		m_pRandomTimeBack = new CBasicLineBox(m_pEngineDevice);
		m_pRandomTimeBack->CreateSub ( this, "BASIC_LINE_BOX_EDIT", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		m_pRandomTimeBack->CreateBaseBoxEditBox ( "RANDOM_TIME_BACK" );
		RegisterControl ( m_pRandomTimeBack );    


		m_pRandomTimeText = new CBasicTextBox(m_pEngineDevice);
		m_pRandomTimeText->CreateSub ( this, "RANDOM_TIME_TEXT", UI_FLAG_DEFAULT, RANDOM_TIME_TEXT );
		m_pRandomTimeText->SetFont ( pFont9 );
		m_pRandomTimeText->SetTextAlign ( TEXT_ALIGN_CENTER_X );	
		RegisterControl ( m_pRandomTimeText );
	}
}

void CDialogueWindow::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    if ( !CUIControlNpcActionable::IsUseable ( m_pGaeaClient ) )
    {
        m_pInterface->SetDialogueWindowClose();
        return;
    }

	m_pAnswerBox->SetUseOverColor ( FALSE );
	CUIWindowEx::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );	
	
	m_pAnswerBox->SetGlobalPos ( m_pAnswerBox->GetGlobalPos () );

	if ( m_dwRandomTime > 0 )
	{
		m_fRandomTime += fElapsedTime;

		CString strTime;
		strTime.Format( "%d", (int) (m_dwRandomTime - m_fRandomTime) );

		m_pRandomTimeText->SetText( strTime, NS_UITEXTUTIL::WHITE );

		// 시간 오버 
		if ( m_dwRandomTime < m_fRandomTime )
		{
			m_pGaeaClient->PrintMsgTextDlg( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT( "NPCTALK_RANDOM_TIME_OVER" ) ); 
			m_pInterface->UiHideGroup ( GetWndID () );
		}
	}

	if ( bFirstControl )
	{
		{
			CBasicScrollThumbFrame* const pThumbFrame = m_pScrollBar->GetThumbFrame ();		
			m_pQuestionBox->SetCurLine ( 0 );
			const int nTotalLine = m_pQuestionBox->GetTotalLine ();
			const int nLinePerOneView = m_pQuestionBox->GetVisibleLine ();
			if ( nLinePerOneView < nTotalLine )
			{
				const int nMovableLine = nTotalLine - nLinePerOneView;
				float fPercent = pThumbFrame->GetPercent ();
				int nPos = (int)floor(fPercent * nMovableLine);
				if ( nPos < nSTARTLINE ) nPos = nSTARTLINE;
				m_pQuestionBox->SetCurLine ( nPos );			
			}
		}
		{
			CBasicScrollThumbFrame* const pThumbFrame = m_pAnswerScrollBar->GetThumbFrame ();		
			m_pAnswerBox->SetCurLine ( 0 );
			const int nTotalLine = m_pAnswerBox->GetTotalLine ();
			const int nLinePerOneView = m_pAnswerBox->GetVisibleLine ();
			if ( nLinePerOneView < nTotalLine )
			{
				const int nMovableLine = nTotalLine - nLinePerOneView;
				float fPercent = pThumbFrame->GetPercent ();
				int nPos = (int)floor(fPercent * nMovableLine);
				if ( nPos < nSTARTLINE ) nPos = nSTARTLINE;
				m_pAnswerBox->SetCurLine ( nPos );			
			}
		}
	}
}

void CDialogueWindow::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	CUIWindowEx::TranslateUIMessage ( ControlID, dwMsg );

	switch ( ControlID )
	{
	case DIALOGUE_ANSWER_TEXTBOX:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				int nIndex = m_pAnswerBox->GetSelectPos ();
				if ( nIndex < 0 ) return ;

				m_pAnswerBox->SetUseOverColor ( TRUE );		
				m_pAnswerBox->SetOverColor ( nIndex, NS_UITEXTCOLOR::IVORY );

				const DWORD NID = m_pAnswerBox->GetTextData ( nIndex );				
				if (!m_spDialogueCase)
                    return;
                std::tr1::shared_ptr<CNpcTalkControl> spNpcTalkControl = m_spDialogueCase->GetTalkControl();
				if (!spNpcTalkControl)
                    return;

                std::tr1::shared_ptr<SNpcTalk> spNpcTalk = spNpcTalkControl->GetTalk ( NID );
				if (!spNpcTalk)
                    return;

				if ( dwMsg & UIMSG_LB_UP )
				{	
					TranslateTalkMessage ( spNpcTalk );
				}
				else
				{
					//	퀘스트 보상치 표현
					if ( spNpcTalk->m_nACTION == SNpcTalk::EM_QUEST_START )
					{						
						DWORD dwQuestID = spNpcTalk->m_dwQuestStartID[0];
						GLQUEST* pQuest = GLQuestMan::GetInstance().Find ( dwQuestID );
						if ( !pQuest )
						{
							sc::writeLogError(
                                sc::string::format(
                                    "quest id = %1% 의 정보를 가져오지 못하였습니다.", dwQuestID));
							return;
						}

						//	보상치가 없으면 아무것도 표시하지 않음
						if ( !(pQuest->m_lnGiftEXP || pQuest->m_dwGiftMONEY ||
							 pQuest->m_dwGiftELEMENT || pQuest->m_dwGiftLIFEPOINT ||
							 pQuest->m_dwGiftSKILLPOINT || pQuest->m_dwGiftSTATSPOINT ||
							 pQuest->m_vecGiftITEM.size () || pQuest->m_vecGiftSKILL.size ()) )
						{
							return ;
						}

						if ( m_pInterface->BEGIN_COMMON_LINEINFO_MULTI () )
						{
							m_pInterface->ADD_COMMON_LINEINFO_MULTI ( ID2GAMEWORD("QUEST_GIFT",9), NS_UITEXTCOLOR::GREENYELLOW );

							CString strCombine;

							if ( pQuest->m_lnGiftEXP )
							{
								strCombine.Format ( "%s : %I64d", ID2GAMEWORD("QUEST_GIFT",0), pQuest->m_lnGiftEXP );
								m_pInterface->ADD_COMMON_LINEINFO_MULTI ( strCombine, NS_UITEXTCOLOR::WHITE );
							}
							if ( pQuest->m_dwGiftMONEY )
							{
								strCombine.Format ( "%s : %d", ID2GAMEWORD("QUEST_GIFT",1), pQuest->m_dwGiftMONEY );
								m_pInterface->ADD_COMMON_LINEINFO_MULTI ( strCombine, NS_UITEXTCOLOR::WHITE );
							}
							if ( pQuest->m_dwGiftELEMENT )
							{
								strCombine.Format ( "%s : %d", ID2GAMEWORD("QUEST_GIFT",2), pQuest->m_dwGiftELEMENT );
								m_pInterface->ADD_COMMON_LINEINFO_MULTI ( strCombine, NS_UITEXTCOLOR::WHITE );
							}
							if ( pQuest->m_dwGiftLIFEPOINT )
							{
								strCombine.Format ( "%s : %d", ID2GAMEWORD("QUEST_GIFT",3), pQuest->m_dwGiftLIFEPOINT );
								m_pInterface->ADD_COMMON_LINEINFO_MULTI ( strCombine, NS_UITEXTCOLOR::WHITE );
							}
							//if ( pQuest->m_dwGiftDisPK )
							//{
							//	strCombine.Format ( "%s : %d", ID2GAMEWORD("QUEST_GIFT",4), pQuest->m_dwGiftDisPK );
							//	m_pInterface->ADD_COMMON_LINEINFO_MULTI ( strCombine, NS_UITEXTCOLOR::WHITE );
							//}
							if ( pQuest->m_dwGiftSKILLPOINT )
							{
								strCombine.Format ( "%s : %d", ID2GAMEWORD("QUEST_GIFT",5), pQuest->m_dwGiftSKILLPOINT );
								m_pInterface->ADD_COMMON_LINEINFO_MULTI ( strCombine, NS_UITEXTCOLOR::WHITE );
							}
							if ( pQuest->m_dwGiftSTATSPOINT )
							{
								strCombine.Format ( "%s : %d", ID2GAMEWORD("QUEST_GIFT",6), pQuest->m_dwGiftSTATSPOINT );
								m_pInterface->ADD_COMMON_LINEINFO_MULTI ( strCombine, NS_UITEXTCOLOR::WHITE );
							}

							if ( pQuest->m_vecGiftITEM.size () )
							{
								CString strItem = ID2GAMEWORD("QUEST_GIFT",7);
								strItem += " : ";
								INVENQARRAY_ITER iter = pQuest->m_vecGiftITEM.begin ();
								INVENQARRAY_ITER iter_end = pQuest->m_vecGiftITEM.end ();
								for ( ; iter != iter_end; )
								{
									SITEMCUSTOM& sItemCustom = (*iter);
									
									++iter;
									const SITEM* pItem = GLogicData::GetInstance().GetItem ( sItemCustom.GetNativeID() );
									if ( !pItem )
										continue;
									strItem += sItemCustom.GETNAME().c_str();
									if ( iter == iter_end ) break;
									strItem += ", ";
								}
								m_pInterface->ADD_COMMON_LINEINFO_MULTI ( strItem, NS_UITEXTCOLOR::WHITE );
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
								m_pInterface->ADD_COMMON_LINEINFO_MULTI ( strSkill, NS_UITEXTCOLOR::WHITE );
							}

							m_pInterface->END_COMMON_LINEINFO_MULTI ();
						}
					}
				}
			}
		}
		break;
	}
}

void CDialogueWindow::TranslateTalkMessage(std::tr1::shared_ptr<SNpcTalk> spNpcTalk)
{
    if (!spNpcTalk)
        return;

	switch ( spNpcTalk->m_nACTION )
	{
	case SNpcTalk::EM_DO_NOTHING:
		{
		}
		break;
	case SNpcTalk::EM_PAGE_MOVE:
		{
			LoadNode ( spNpcTalk->m_dwACTION_NO );
		}
		break;
	case SNpcTalk::EM_BASIC:
		{
			TranslateBasicMessage ( spNpcTalk );
		}
		break;
	case SNpcTalk::EM_QUEST_START:
		{
			LoadNode ( spNpcTalk->m_dwACTION_NO );

			TranslateQuestStartMessage ( spNpcTalk );
		}
		break;
	case SNpcTalk::EM_QUEST_STEP:
		{
			LoadNode ( spNpcTalk->m_dwACTION_NO );

			TranslateQuestStepMessage ( spNpcTalk );
		}
		break;
	};
}

void CDialogueWindow::TranslateBasicMessage(std::tr1::shared_ptr<SNpcTalk> spNpcTalk)
{
    if (!spNpcTalk)
        return;

	// Need Add New Item Con.
	DWORD dwMsg = spNpcTalk->m_dwACTION_NO;
	DWORD dwTalkID = spNpcTalk->m_dwGLOB_ID;

	GLLandManClient* pLand = m_pGaeaClient->GetActiveMap();
	if (!pLand)
        return;
    std::tr1::shared_ptr<GLCrowClient> pCrow = pLand->GetCrow ( CUIControlNpcActionable::GetNPCGlobalID() );
	if (!pCrow)
        return;

	// ReqNpcTalkBasic() 대화문 유효성 체크 ( 서버 ) 
	switch ( dwMsg )
	{
	case SNpcTalk::EM_STORAGE:
		{
			m_pInterface->UiHideGroup ( GetWndID () );
			m_pGaeaClient->GetCharacter()->ReqNpcTalkBasic( CUIControlNpcActionable::GetNPCGlobalID(), dwMsg, dwTalkID );
		}
		break;
	case SNpcTalk::EM_MARKET:
		{
			m_pInterface->UiHideGroup ( GetWndID () );
			m_pGaeaClient->GetCharacter()->ReqNpcTalkBasic( CUIControlNpcActionable::GetNPCGlobalID(), dwMsg, dwTalkID );
		}
		break;
	case SNpcTalk::EM_CURE:
		{
			m_pInterface->UiHideGroup ( GetWndID () );
			m_pGaeaClient->GetCharacter()->ReqNpcTalkBasic( CUIControlNpcActionable::GetNPCGlobalID(), dwMsg, dwTalkID );
		}
		break;
	case SNpcTalk::EM_STARTPOINT:
		{
			m_pInterface->UiHideGroup ( GetWndID () );
			m_pGaeaClient->GetCharacter()->ReqNpcTalkBasic( CUIControlNpcActionable::GetNPCGlobalID(), dwMsg, dwTalkID );
		}
		break;

	case SNpcTalk::EM_CHAR_RESET:
		{
			m_pInterface->UiHideGroup ( GetWndID () );
			m_pGaeaClient->GetCharacter()->ReqNpcTalkBasic( CUIControlNpcActionable::GetNPCGlobalID(), dwMsg, dwTalkID );
		}
		break;

	// Need Add New Item Con.
	case SNpcTalk::EM_ITEM_TRADE:
		{
			m_pInterface->UiHideGroup ( GetWndID () );
			m_pGaeaClient->GetCharacter()->ReqItemTrade ( CUIControlNpcActionable::GetNPCGlobalID(), dwTalkID, spNpcTalk );
		}
		break;

	case SNpcTalk::EM_BUSSTATION:
		{
			m_pInterface->UiHideGroup ( GetWndID () );
            m_pInterface->SetBusWindowOpen (  CUIControlNpcActionable::GetNPCID(), CUIControlNpcActionable::GetNPCGlobalID(), spNpcTalk );
		}
		break;

	case SNpcTalk::EM_CLUB_NEW:
		{
			m_pInterface->UiHideGroup ( GetWndID () );

            //	NOTE : 클럽 만들기 신청
			m_pGaeaClient->GetCharacter()->ReqNpcTalkBasic( CUIControlNpcActionable::GetNPCGlobalID(), dwMsg, dwTalkID );
		}
		break;

	case SNpcTalk::EM_CLUB_UP:
		{
			m_pInterface->UiHideGroup ( GetWndID () );
			
			m_pGaeaClient->GetCharacter()->ReqNpcTalkBasic( CUIControlNpcActionable::GetNPCGlobalID(), dwMsg, dwTalkID );
		}
		break;

	case SNpcTalk::EM_CD_CERTIFY:
		{
			m_pInterface->UiHideGroup ( GetWndID () );
			m_pGaeaClient->GetCharacter()->ReqNpcTalkBasic( CUIControlNpcActionable::GetNPCGlobalID(), dwMsg, dwTalkID );
		}
		break;

	case SNpcTalk::EM_COMMISSION:
		{
			m_pInterface->UiHideGroup ( GetWndID () );

			//	NOTE : 수수료 지정
			//
			m_pGaeaClient->GetCharacter()->ReqNpcTalkBasic( CUIControlNpcActionable::GetNPCGlobalID(), dwMsg, dwTalkID );
		}
		break;

	case SNpcTalk::EM_CLUB_STORAGE:
		{
			GLCharacter* pCharacter = m_pGaeaClient->GetCharacter ();
			GLClubClient* pMyClub = m_pGaeaClient->GetMyClub();
			bool bVALID_CLUB = pMyClub->IsValid();
			bool bMASTER = pMyClub->MasterCharDbNum()==pCharacter->m_CharDbNum;

			if ( bVALID_CLUB && bMASTER )
			{
				m_pInterface->UiHideGroup ( GetWndID () );
				m_pGaeaClient->GetCharacter()->ReqNpcTalkBasic( CUIControlNpcActionable::GetNPCGlobalID(), dwMsg, dwTalkID );
			}
			else
			{
				m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("ONLY_CLUB_MASTER_USE_CLUBSTORAGE") );
			}
		}
		break;

	case SNpcTalk::EM_ITEM_REBUILD:	// ITEMREBUILD_MARK
		{
			m_pInterface->UiHideGroup( GetWndID() );
			m_pGaeaClient->GetCharacter()->ReqNpcTalkBasic( CUIControlNpcActionable::GetNPCGlobalID(), dwMsg, dwTalkID );
		}
		break;

	case SNpcTalk::EM_ODDEVEN:
		{
			m_pInterface->UiHideGroup( GetWndID() );
			m_pGaeaClient->GetCharacter()->ReqNpcTalkBasic( CUIControlNpcActionable::GetNPCGlobalID(), dwMsg, dwTalkID );
		}
		break;
	case SNpcTalk::EM_RECOVERY_EXP:
		{
			m_pInterface->UiHideGroup( GetWndID() );
			m_pGaeaClient->GetCharacter()->ReqNpcTalkBasic( CUIControlNpcActionable::GetNPCGlobalID(), dwMsg, dwTalkID );
		}
		break;
	case SNpcTalk::EM_RANDOM_PAGE:
		{
			int nCnt = MAX_RANDOM_PAGE;

			for ( int i = 0; i < MAX_RANDOM_PAGE; ++i )
			{
				if ( spNpcTalk->m_dwRandomPageID[i] == UINT_MAX )
				{
					nCnt = i;
					break;
				}
			}

			if ( nCnt <= 0 ) 
			{
                sc::writeLogError(
                    sc::string::format(
                        "Random Page Not Found [%1%]",
                        m_spDialogueSet->GetDlgSetName()));
				break;
			}
			
			int nChoice = int ( rand() % nCnt );

			LoadNode( spNpcTalk->m_dwRandomPageID[nChoice], true, spNpcTalk->m_dwRandomTime );
		}
		break;
	case SNpcTalk::EM_ITEMSEARCH_PAGE:
		{
			m_pInterface->UiHideGroup ( GetWndID () );
			m_pGaeaClient->GetCharacter()->ReqNpcTalkBasic( CUIControlNpcActionable::GetNPCGlobalID(), dwMsg, dwTalkID );
		}
		break;

	case SNpcTalk::EM_ATTENDANCE_BOOK:
		{
			m_pInterface->UiHideGroup ( GetWndID () );
			m_pGaeaClient->GetCharacter()->ReqNpcTalkBasic( CUIControlNpcActionable::GetNPCGlobalID(), dwMsg, dwTalkID );
		}
		break;
	case SNpcTalk::EM_ITEM_MIX:
		{
			m_pInterface->UiHideGroup ( GetWndID () );
			m_pGaeaClient->GetCharacter()->ReqNpcTalkBasic( CUIControlNpcActionable::GetNPCGlobalID(), dwMsg, dwTalkID );
		}
		break;
	case SNpcTalk::EM_HIRE_SUMMON:
		{
			m_pInterface->UiHideGroup( GetWndID() );
			m_pGaeaClient->GetCharacter()->ReqHireSummon ( CUIControlNpcActionable::GetNPCGlobalID(), dwTalkID, spNpcTalk );
		}
		break;
	};
}

void CDialogueWindow::TranslateQuestStartMessage(std::tr1::shared_ptr<SNpcTalk> spNpcTalk)
{
    GASSERT(spNpcTalk && "CDialogueWindow::TranslateQuestStartMessage");

    if (!spNpcTalk)
        return;

	GLLandManClient* pLand = m_pGaeaClient->GetActiveMap();
	if ( !pLand )
        return;
	std::tr1::shared_ptr<GLCrowClient> pCrow = pLand->GetCrow ( CUIControlNpcActionable::GetNPCGlobalID() );
	if ( !pCrow )
        return;

	CTime cCurSvrTime = m_pGaeaClient->GetCurrentTime();
	for ( int i = 0; i < MAX_QUEST_START; ++i )
	{
		if ( spNpcTalk->m_dwQuestStartID[i] != UINT_MAX && spNpcTalk->DoTEST(m_pCHAR, i, cCurSvrTime.GetHour()))
		{
			m_pGaeaClient->GetCharacter()->ReqQuestStart ( 
				CUIControlNpcActionable::GetNPCGlobalID(), spNpcTalk->m_dwGLOB_ID, spNpcTalk->m_dwQuestStartID[i] );
		}
	}
}

void CDialogueWindow::TranslateQuestStepMessage(std::tr1::shared_ptr<SNpcTalk> spNpcTalk)
{
	GASSERT(spNpcTalk&&"CDialogueWindow::TranslateQuestStepMessage");
    if (!spNpcTalk)
        return;

	GLLandManClient* pLand = m_pGaeaClient->GetActiveMap();
	if ( !pLand )
        return;
	std::tr1::shared_ptr<GLCrowClient> pCrow = pLand->GetCrow ( CUIControlNpcActionable::GetNPCGlobalID() );
	if ( !pCrow )
        return;

	m_pGaeaClient->GetCharacter()->ReqQuestStepNpcTalk(
        CUIControlNpcActionable::GetNPCGlobalID(),
		spNpcTalk->m_dwGLOB_ID,
        spNpcTalk->m_dwACTION_PARAM1,
        spNpcTalk->m_dwACTION_PARAM2);
}

void CDialogueWindow::LoadBasicTalk ()
{
	if ( !m_spDialogueCase )
        return;
	const std::string str = m_spDialogueCase->GetBasicTalk ();

	m_pQuestionBox->SetText ( str.c_str(), NS_UITEXTCOLOR::BURLYWOOD );

	int nTotal = m_pQuestionBox->GetTotalLine ();
	int nViewPerPage = m_pQuestionBox->GetVisibleLine ();
	CBasicScrollThumbFrame* const pThumbFrame = m_pScrollBar->GetThumbFrame ();
	pThumbFrame->SetState ( nTotal, nViewPerPage );
	pThumbFrame->SetPercent ( 0.0f );
}

void CDialogueWindow::LoadButtonTalk ( bool bRandom )
{
	m_pAnswerBox->ClearText ();

	static const int nKEYWORDSIZE = 10;
	static const CString strKeyword[nKEYWORDSIZE] = 
	{
		"DIALOGUE_ICON_HEAL",
		"DIALOGUE_ICON_TRADE",
		"DIALOGUE_ICON_STARTPOINT",
		"DIALOGUE_ICON_STORAGE",
		"DIALOGUE_ICON_MARKET",
		"DIALOGUE_ICON_RESETPOINT",
		"DIALOGUE_ICON_BUS",
		"DIALOGUE_ICON_QUEST_START",
		"DIALOGUE_ICON_QUEST_ING",
		"DIALOGUE_ICON_TALK",
	};

	enum
	{
		ICON_HEAL,
		ICON_TRADE,
		ICON_STARTPOINT,
		ICON_STORAGE,
		ICON_MARKET,
		ICON_RESETPOINT,
		ICON_BUS,
		ICON_QUEST_START,
		ICON_QUEST_ING,
		ICON_QUEST_TALK,
	};

	//	버튼 텍스트 설정
	if (!m_spDialogueCase)
        return;
    std::tr1::shared_ptr<CNpcTalkControl> spNpcTalkControl = m_spDialogueCase->GetTalkControl ();
	if (spNpcTalkControl)
	{
		CNpcTalkControl::NPCMAP* pMap = spNpcTalkControl->GetTalkMap ();
		CNpcTalkControl::NPCMAP_IT iter = pMap->begin();
		CNpcTalkControl::NPCMAP_IT iter_end = pMap->end();

		std::vector<std::tr1::shared_ptr<SNpcTalk> > vecTalk;
		vecTalk.reserve( pMap->size() );

		for ( ; iter != iter_end; ++iter )
		{
            std::tr1::shared_ptr<SNpcTalk> spNpcTalk = iter->second;
            if (spNpcTalk)
			    vecTalk.push_back(spNpcTalk);
		}
		
		if ( bRandom )
		{
			random_shuffle( vecTalk.begin(), vecTalk.end() );			
		}

		CTime cCurSvrTime = m_pGaeaClient->GetCurrentTime();

		std::vector<std::tr1::shared_ptr<SNpcTalk> >::iterator	pos = vecTalk.begin();
		std::vector<std::tr1::shared_ptr<SNpcTalk> >::iterator	pos_end = vecTalk.end();

		for ( ; pos != pos_end; ++pos )
		{
			std::tr1::shared_ptr<SNpcTalk> spNpcTalk = (*pos);

			//	Note : 조건에 부합하지 못할 경우 추가하지 않음.
			if (!spNpcTalk->DoTEST(m_pCHAR, m_pGaeaClient->GetMyPartyClient()->GetNMember(), cCurSvrTime.GetHour()))
                continue;
			
			CString strAnswerTalk = spNpcTalk->GetTalk();			

            int nImageType = 0;	
			D3DCOLOR dwColor = NS_UITEXTCOLOR::PERU;
			switch ( spNpcTalk->m_nACTION )
			{
			case SNpcTalk::EM_QUEST_START:
				{
					dwColor = NS_UITEXTCOLOR::LIGHTCYAN;
					nImageType = ICON_QUEST_START;

					//	NOTE : 비용이 있을 경우 추가
					DWORD dwQuestID = spNpcTalk->m_dwQuestStartID[0];
					GLQUEST* pQuest = GLQuestMan::GetInstance().Find ( dwQuestID );
					if ( !pQuest )
					{
						sc::writeLogError(
                            sc::string::format(
                                "quest id = %1% 의 정보를 가져오지 못하였습니다.", dwQuestID));
						continue;
					}

					const DWORD dwBeginMoney = pQuest->m_dwBeginMoney;
					if ( dwBeginMoney )
					{
						CString strQuestCost;
						strQuestCost.Format ( " [%s:%d]", ID2GAMEWORD ( "QUEST_BEGIN_MONEY" ), dwBeginMoney );
						strAnswerTalk += strQuestCost;
					}					
				}
				break;

			case SNpcTalk::EM_QUEST_STEP:
				{
					dwColor = NS_UITEXTCOLOR::LIGHTCYAN;
					nImageType = ICON_QUEST_START;
				}
				break;

			case SNpcTalk::EM_BASIC:
				{
					switch ( spNpcTalk->m_dwACTION_NO )
					{
					//case SNpcTalk::EM_STORAGE:	nImageType = ICON_STORAGE; break;
					//case SNpcTalk::EM_MARKET:	nImageType = ICON_MARKET; break;
					case SNpcTalk::EM_CURE:		nImageType = ICON_HEAL; break;
					case SNpcTalk::EM_STARTPOINT: nImageType = ICON_STARTPOINT; break;
					//case SNpcTalk::EM_CHAR_RESET: nImageType = ICON_RESETPOINT; break;
					//case SNpcTalk::EM_ITEM_TRADE: nImageType = ICON_TRADE; break;
					default:	nImageType = ICON_QUEST_TALK;		break;
					}
				}
				break;

			case SNpcTalk::EM_PAGE_MOVE:
				{
					nImageType = ICON_QUEST_TALK;
				}
				break;
			}

			nImageType = ( nImageType >= nKEYWORDSIZE )?nKEYWORDSIZE-1:nImageType;

			int nIndex = m_pAnswerBox->AddText ( strAnswerTalk, dwColor );
			m_pAnswerBox->SetTextData ( nIndex, spNpcTalk->m_dwNID );				 
			m_pAnswerBox->SetTextImage ( nIndex, strKeyword[nImageType] );
		}

		int nTotal = m_pAnswerBox->GetTotalLine ();
		int nViewPerPage = m_pAnswerBox->GetVisibleLine ();
		CBasicScrollThumbFrame* const pThumbFrame = m_pAnswerScrollBar->GetThumbFrame ();
		pThumbFrame->SetState ( nTotal, nViewPerPage );
		pThumbFrame->SetPercent ( 0.0f );
	}
}

void CDialogueWindow::LoadNode( DWORD dwNID, bool bRandom, DWORD dwRandomTime )
{
	m_spDialogue = m_spDialogueSet->GetDialogue(dwNID);
	GASSERT(m_spDialogue && "m_DialogueSet.GetDialogue()");
	if (!m_spDialogue)
        return;
	
	if ( !m_pGaeaClient )
		return;

	CTime cCurSvrTime = m_pGaeaClient->GetCurrentTime();
	DWORD dwIndex = m_spDialogue->FindConditionSatisfied(m_pCHAR, m_pGaeaClient->GetMyPartyClient()->GetNMember(), cCurSvrTime.GetHour());
	GASSERT(dwIndex!=UINT_MAX&&"m_pDialogue->FindConditionSatisfied()");
	if (dwIndex==UINT_MAX)
        return;

	std::tr1::shared_ptr<CNpcDialogueCase> spCase = m_spDialogue->GetDlgCase(dwIndex);	
	GASSERT(spCase&&"m_pDialogueCase = NULL");
	m_spDialogueCase = spCase;

	m_dwRandomTime = dwRandomTime;
	m_fRandomTime = 0.0f;

    //
	if ( m_dwRandomTime > 0 )
	{
		m_pRandomTimeButton->SetVisibleSingle( TRUE );
		m_pRandomTimeBack->SetVisibleSingle( TRUE );
		m_pRandomTimeText->SetVisibleSingle( TRUE );
		
		CString strTime;
		strTime.Format( "%d", m_dwRandomTime );

		m_pRandomTimeText->SetText( strTime, NS_UITEXTUTIL::WHITE );

	}
	else
	{
		m_pRandomTimeButton->SetVisibleSingle( FALSE );
		m_pRandomTimeBack->SetVisibleSingle( FALSE );
		m_pRandomTimeText->SetVisibleSingle( FALSE );
	}

	LoadBasicTalk ();
	LoadButtonTalk ( bRandom );
}

BOOL CDialogueWindow::SetDialogueData (  const SNATIVEID sNPCID, const DWORD dwNPCGlobalID, GLCHARLOGIC *pCHAR )
{
	GASSERT ( pCHAR );

	m_pCHAR = pCHAR;

	std::tr1::shared_ptr<GLCrowClient> pCrow = m_pGaeaClient->GetActiveMap()->GetCrow(dwNPCGlobalID);
	if ( !pCrow )						
        return FALSE;

    if ( pCrow->GETCROW() != CROW_NPC )
        return FALSE;

    CUIControlNpcActionable::SetNpcActionableNPCID( sNPCID, dwNPCGlobalID );

	const PCROWDATA pCrowData = GLCrowDataMan::GetInstance().GetCrowData(pCrow->m_pCrowData->sNativeID);
    std::tr1::shared_ptr<NpcTalkMan> spTalkMan = GLogicData::GetInstance().GetNpcTalkMan();
    if (spTalkMan)
	{
        std::tr1::shared_ptr<CNpcDialogueSet> spDlgSet = spTalkMan->Find(pCrowData->GetTalkFile());
        if (spDlgSet)
        {
            m_spDialogueSet = spDlgSet;
		    LoadNode(nSTARTINDEX);
		    return TRUE;
        }
	}
	return FALSE;
}