#include "StdAfx.h"
#include "QuestList.h"
#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/DxTools/DebugSet.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../EngineLib/GUInterface/BasicTextBoxEx.h"
#include "../../../EngineLib/GUInterface/BasicScrollBarEx.h"
#include "../../../EngineLib/GUInterface/BasicScrollThumbFrame.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/BasicLineBox.h"
#include "../../../EngineLib/Common/ByteStream.h"
#include "../../../RanLogic/Quest/GLQuestPlay.h"
#include "../../../RanLogic/Quest/GLQuest.h"
#include "../../../RanLogic/Quest/GLQuestMan.h"
#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"
#include "../../../RanLogic/GLUseFeatures.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

const	int	CQuestList::nOUTOFRANGE = -1;
const	int CQuestList::nLimitQuest = 1000;

CQuestList::CQuestList(GLGaeaClient* pGaeaClient, EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
	, m_dwQuestID(NATIVEID_NULL().dwID)
    , m_nSelectIndex(nOUTOFRANGE)
{
}

CQuestList::~CQuestList ()
{
}

void CQuestList::CreateSubControl ()
{
	m_pButtonMenuL = CreateTextButton19 ( "QUEST_LIST_BUTTON_L", QUEST_LIST_BUTTON_L, (char*)ID2GAMEWORD("QUEST_LIST_BUTTON",0) );
	m_pButtonMenuL->SetFlip ( FALSE );
	m_pButtonMenuR = CreateTextButton19 ( "QUEST_LIST_BUTTON_R", QUEST_LIST_BUTTON_R, (char*)ID2GAMEWORD("QUEST_LIST_BUTTON",1) );	
	m_pButtonMenuR->SetFlip ( FALSE );

	CD3DFontPar* pFont9 = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );
	
	CBasicLineBox* pBasicLineBox = new CBasicLineBox(m_pEngineDevice);
	pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_QUEST_LIST", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pBasicLineBox->CreateBaseBoxQuestList ( "QUEST_LIST_BACK" );
	RegisterControl ( pBasicLineBox );
	m_pListLineBox = pBasicLineBox;

	//	텍스트 박스
	{
		CBasicTextBoxEx* pTextBox = new CBasicTextBoxEx(m_pEngineDevice);
		pTextBox->CreateSub ( this, "QUEST_LIST_TEXTBOX", UI_FLAG_DEFAULT, QUEST_LIST_TEXTBOX_PROG );
		pTextBox->SetFont ( pFont9 );		
		pTextBox->SetLineInterval ( 3.0f );
		pTextBox->SetSensitive ( true );
		pTextBox->SetLimitLine ( nLimitQuest );
		RegisterControl ( pTextBox );
		m_pListTextProg = pTextBox;

		int nTotalLine = pTextBox->GetVisibleLine ();

		//	스크롤바
		CBasicScrollBarEx* pScrollBar = new CBasicScrollBarEx(m_pEngineDevice);
		pScrollBar->CreateSub ( this, "BASIC_SCROLLBAR", UI_FLAG_RIGHT | UI_FLAG_YSIZE, QUEST_LIST_SCROLLBAR_PROG );
		pScrollBar->CreateBaseScrollBar ( "QUEST_LIST_SCROLLBAR" );
		pScrollBar->GetThumbFrame()->SetState ( 1, nTotalLine );
		RegisterControl ( pScrollBar );
		m_pListScrollBarProg = pScrollBar;
	}

	{
		CBasicTextBoxEx* pTextBox = new CBasicTextBoxEx(m_pEngineDevice);
		pTextBox->CreateSub ( this, "QUEST_LIST_TEXTBOX", UI_FLAG_DEFAULT, QUEST_LIST_TEXTBOX_END );
		pTextBox->SetFont ( pFont9 );		
		pTextBox->SetSensitive ( true );
		pTextBox->SetLineInterval ( 3.0f );
		pTextBox->SetLimitLine ( nLimitQuest );
		RegisterControl ( pTextBox );
		m_pListTextEnd = pTextBox;	

		int nTotalLine = pTextBox->GetVisibleLine ();

		//	스크롤바
		CBasicScrollBarEx* pScrollBar = new CBasicScrollBarEx(m_pEngineDevice);
		pScrollBar->CreateSub ( this, "BASIC_SCROLLBAR", UI_FLAG_RIGHT | UI_FLAG_YSIZE, QUEST_LIST_SCROLLBAR_END );
		pScrollBar->CreateBaseScrollBar ( "QUEST_LIST_SCROLLBAR" );
		pScrollBar->GetThumbFrame()->SetState ( 1, nTotalLine );
		RegisterControl ( pScrollBar );
		m_pListScrollBarEnd = pScrollBar;
	}

	PageFlip ( TRUE );
}

void CQuestList::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	bool bProg = (m_pListTextProg->IsVisible ())?true:false;

	if ( bProg )
	{
		m_pListTextProg->SetUseOverColor ( FALSE );
		if ( m_pListTextProg->GetCount () && 0 <= m_nSelectIndex )
			m_pListTextProg->SetUseTextColor ( m_nSelectIndex, FALSE );
	}	
	else
	{
		m_pListTextEnd->SetUseOverColor ( FALSE );
		if ( m_pListTextEnd->GetCount () && 0 <= m_nSelectIndex )
			m_pListTextEnd->SetUseTextColor ( m_nSelectIndex, FALSE );
	}

	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	if ( bProg )
	{
		if ( m_pListTextProg->GetCount () && 0 <= m_nSelectIndex )
		{
			m_pListTextProg->SetUseTextColor ( m_nSelectIndex, TRUE );
			m_pListTextProg->SetTextColor ( m_nSelectIndex, NS_UITEXTCOLOR::ORANGE );
		}
	}
	else
	{
		if ( m_pListTextEnd->GetCount () && 0 <= m_nSelectIndex )
		{
			m_pListTextEnd->SetUseTextColor ( m_nSelectIndex, TRUE );
			m_pListTextEnd->SetTextColor ( m_nSelectIndex, NS_UITEXTCOLOR::ORANGE );
		}
	}


	if ( bProg )
	{
		CBasicScrollThumbFrame* const pThumbFrame = m_pListScrollBarProg->GetThumbFrame ();

		const int nTotalLine = m_pListTextProg->GetTotalLine ();
		const int nLinePerOneView = m_pListTextProg->GetVisibleLine ();
		CDebugSet::ToView ( 1, 10, "보이는 라인 %d", nLinePerOneView );		
		pThumbFrame->SetState ( nTotalLine, nLinePerOneView );
		if ( nLinePerOneView < nTotalLine )
		{
			const int nMovableLine = nTotalLine - nLinePerOneView;
			float fPercent = pThumbFrame->GetPercent ();
			int nPos = (int)floor(fPercent * nMovableLine);
			m_pListTextProg->SetCurLine ( nPos );			
		}
	}
	{
		CBasicScrollThumbFrame* const pThumbFrame = m_pListScrollBarEnd->GetThumbFrame ();

		const int nTotalLine = m_pListTextEnd->GetTotalLine ();
		const int nLinePerOneView = m_pListTextEnd->GetVisibleLine ();
		CDebugSet::ToView ( 1, 10, "보이는 라인 %d", nLinePerOneView );
		pThumbFrame->SetState ( nTotalLine, nLinePerOneView );
		if ( nLinePerOneView < nTotalLine )
		{
			const int nMovableLine = nTotalLine - nLinePerOneView;
			float fPercent = pThumbFrame->GetPercent ();
			int nPos = (int)floor(fPercent * nMovableLine);
			m_pListTextEnd->SetCurLine ( nPos );			
		}
	}
}

BOOL CQuestList::IsPROGLIST ()
{
	return m_bPROG;
}

void CQuestList::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	CUIGroup::TranslateUIMessage ( ControlID, dwMsg );

	switch ( ControlID )
	{
	case QUEST_LIST_TEXTBOX_PROG:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				int nIndex = m_pListTextProg->GetSelectPos ();
				if ( m_pListTextProg->GetCount () <= nIndex ) return ;

				if ( UIMSG_LB_UP & dwMsg )
				{
					m_dwQuestID = m_pListTextProg->GetTextData ( nIndex );
					m_nSelectIndex = nIndex;
				}
				if ( UIMSG_LB_DUP & dwMsg ) AddMessageEx ( UIMSG_LIST_LB_DUP );

				m_pListTextProg->SetUseOverColor ( TRUE );
				m_pListTextProg->SetOverColor ( nIndex, NS_UITEXTCOLOR::ORANGE );
			}
		}
		break;

	case QUEST_LIST_TEXTBOX_END:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				//int nIndex = m_pListTextEnd->GetSelectPos ();
				//if ( m_pListTextEnd->GetCount () <= nIndex ) return ;

				//if ( UIMSG_LB_UP & dwMsg )
				//{
				//	m_dwQuestID = m_pListTextProg->GetTextData ( nIndex );
				//	m_nSelectIndex = nIndex;
				//}
				//if ( UIMSG_LB_DUP & dwMsg )	AddMessageEx ( UIMSG_LIST_LB_DUP );

				//m_pListTextEnd->SetUseOverColor ( TRUE );
				//m_pListTextEnd->SetOverColor ( nIndex, NS_UITEXTCOLOR::ORANGE );
			}
		}
		break;

	case QUEST_LIST_BUTTON_L:
	case QUEST_LIST_BUTTON_R:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				BOOL bProg = (ControlID == QUEST_LIST_BUTTON_L);
				PageFlip ( bProg );
			}
		}
		break;
	}
}

void CQuestList::PageFlip ( BOOL bProg )
{
//	if ( bProg && m_pButtonMenuL->IsFlip () ) return ;
//	if ( !bProg && m_pButtonMenuR->IsFlip () ) return ;
	
	m_pButtonMenuL->SetFlip ( bProg );
	m_pButtonMenuR->SetFlip ( !bProg );

	m_nSelectIndex = nOUTOFRANGE;
	m_dwQuestID = NATIVEID_NULL().dwID;

	m_pListTextProg->SetVisibleSingle ( bProg );
	m_pListTextEnd->SetVisibleSingle ( !bProg );

	m_pListScrollBarProg->SetVisibleSingle ( bProg );
	m_pListScrollBarEnd->SetVisibleSingle ( !bProg );

	m_bPROG = bProg;
}

void CQuestList::SetQuestID ( DWORD dwQuestID )
{
	m_dwQuestID = dwQuestID;
}

DWORD	CQuestList::GetQuestID ()
{
	return m_dwQuestID;
}

CBasicTextButton* CQuestList::CreateTextButton14 ( char* szButton, UIGUID ControlID , char* szText )
{
	const int nBUTTONSIZE = CBasicTextButton::SIZE14;
	CBasicTextButton* pButton = new CBasicTextButton(m_pEngineDevice);
	pButton->CreateSub ( this, "BASIC_TEXT_BUTTON14", UI_FLAG_XSIZE, ControlID );
	pButton->CreateBaseButton ( szButton, nBUTTONSIZE, CBasicButton::CLICK_FLIP, szText );
	RegisterControl ( pButton );
	return pButton;
}

CBasicTextButton* CQuestList::CreateTextButton19 ( char* szButton, UIGUID ControlID , char* szText )
{
	const int nBUTTONSIZE = CBasicTextButton::SIZE19;
	CBasicTextButton* pButton = new CBasicTextButton(m_pEngineDevice);
	pButton->CreateSub ( this, "BASIC_TEXT_BUTTON19", UI_FLAG_XSIZE, ControlID );
	pButton->CreateBaseButton ( szButton, nBUTTONSIZE, CBasicButton::RADIO_FLIP, szText );
	RegisterControl ( pButton );
	return pButton;
}

bool CQuestList::LoadQuestList ()
{
	GLQuestPlay& cQuestPlay = m_pGaeaClient->GetCharacter ()->m_cQuestPlay;

	m_pListTextProg->ClearText ();
	m_pListTextEnd->ClearText ();

	PageFlip ( true );

	{
		GLQuestPlay::MAPQUEST QuestProcMap = cQuestPlay.GetQuestProc ();
		GLQuestPlay::MAPQUEST_ITER iter = QuestProcMap.begin ();
		GLQuestPlay::MAPQUEST_ITER iter_end = QuestProcMap.end ();		
		for ( ; iter != iter_end; ++iter )
		{
			GLQUESTPROG* pQuestProg = (*iter).second;
			DWORD dwNID = pQuestProg->m_sNID.dwID;
			GLQUEST* pQuest = GLQuestMan::GetInstance().Find ( dwNID );
			if ( !pQuest ) continue;

			int nIndex;
			if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
			{
				CString strKey;
				strKey.Format("QN_%03d_%03d", pQuest->m_sNID.wMainID, pQuest->m_sNID.wSubID);
				// CGameTextMan::GetInstance().GetQuestText(strKey).GetString();
				nIndex = m_pListTextProg->AddText(CGameTextMan::GetInstance().GetQuestText(strKey.GetString()).GetString());
			}
			else
			{
				nIndex = m_pListTextProg->AddText(pQuest->GetTITLE());
			}
			m_pListTextProg->SetTextData ( nIndex, dwNID );
		}
//		m_pListTextProg->AddText ( "프로크" );
	}

	{
		GLQuestPlay::MAPQUEST QuestEndMap = cQuestPlay.GetQuestEnd ();
		GLQuestPlay::MAPQUEST_ITER iter = QuestEndMap.begin ();
		GLQuestPlay::MAPQUEST_ITER iter_end = QuestEndMap.end ();
		for ( ; iter != iter_end; ++iter )
		{
			GLQUESTPROG* pQuestProg = (*iter).second;
			DWORD dwNID = pQuestProg->m_sNID.dwID;
			GLQUEST* pQuest = GLQuestMan::GetInstance().Find ( dwNID );

			if ( !pQuest ) continue;

			CString strCombine;
			std::string strTitle;
			if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
			{
				CString strKey;
				strKey.Format("QN_%03d_%03d", pQuestProg->m_sNID.wMainID, pQuestProg->m_sNID.wSubID);
				strTitle = CGameTextMan::GetInstance().GetQuestText(strKey.GetString()).GetString();
			}
			else
			{
				strTitle = pQuest->GetTitleStr();
			}
			const int nCOMPLETE_INDEX = (pQuestProg->m_bCOMPLETE)?1:0;
			const DWORD dwCOUNT = pQuestProg->m_dwCOUNT;

			strCombine.Format ( "%s [%s/%s:%d]", strTitle.c_str (),
				ID2GAMEWORD("QUEST_COMPLETE",nCOMPLETE_INDEX),
				ID2GAMEWORD("QUEST_TRYCOUNT"), dwCOUNT );

			D3DCOLOR dwColor = NS_UITEXTCOLOR::GREENYELLOW;
			if ( !pQuestProg->m_bCOMPLETE ) dwColor = NS_UITEXTCOLOR::RED;

			int nIndex = m_pListTextEnd->AddText ( strCombine, dwColor );
			//	의도적으로 넣지 않음.
			//	혹시 정보가 필요할 경우에 풀면 됨
//			m_pListTextEnd->SetTextData ( nIndex, dwNID );
		}
	}

	return true;
}
//
//
//void CQuestList::SetVisibleSingle ( BOOL bVisible )
//{
//	CUIGroup::SetVisibleSingle ( bVisible );
//}
