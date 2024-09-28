#include "StdAfx.h"

#include "../../../../RanLogic/Quest/GLQuestMan.h"
#include "../../../../SigmaCore/String/StringFormat.h"
#include "../../../../EngineLib/GUInterface/SwapImage.h"
#include "../../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../../EngineLib/DxTools/d3dfont.h"
#include "../../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../../EngineLib/GUInterface/SwapImage.h"
#include "../../../../EngineLib/GUInterface/BasicLineBoxSmart.h"
#include "../../../../EngineLib/GUInterface/BasicTextBoxEx.h"
#include "../../../../EngineLib/GUInterface/BasicScrollBarEx.h"
#include "../../../InnerInterface.h"
#include "../../../../RanLogicClient/GLGaeaClient.h"
#include "RnattendanceDayInfo.h"
#include "../../../../enginelib/GUInterface/BasicScrollBarNoTHUMB.h"
#include "RnCalenderDefine.h"

const int MAXQUESTSLOT = 2;

const char* RNATTENDANCE_DAY_INFO_CONDITION = "[과제 %1%]";
const char* RNATTENDANCE_DAY_INFO_REWARDITEM = "보상아이템";
const char* RNATTENDANCE_DAY_INFO_ANY_DAY_REWARD = "출석 %1%일차 보상";

RnattendanceDayInfo::RnattendanceDayInfo(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
: CUIGroup(pEngineDevice)
, m_pGaeaClient(pGaeaClient)
, m_pInterface(pInterface)
, m_pDayText(NULL)
, m_pInfoListScrollBar(NULL)
, m_pInfoQuestScrollBar(NULL)
, m_nQuestListScrollPos(0)
{
	RNATTENDANCE_DAY_INFO_CONDITION = ID2GAMEINTEXT("RNATTENDANCE_DAY_INFO_CONDITION");
	RNATTENDANCE_DAY_INFO_REWARDITEM = ID2GAMEINTEXT("RNATTENDANCE_DAY_INFO_REWARDITEM");
	RNATTENDANCE_DAY_INFO_ANY_DAY_REWARD = ID2GAMEINTEXT("RNATTENDANCE_DAY_INFO_ANY_DAY_REWARD");

}

RnattendanceDayInfo::~RnattendanceDayInfo()
{

}

void RnattendanceDayInfo::CreateSubControl()
{
	CD3DFontPar* pFont9 = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9, _DEFAULT_FONT_SHADOW_FLAG );

	//날짜 ex) 5일 - 오늘
	CBasicTextBoxEx* pDayText = new CBasicTextBoxEx(m_pEngineDevice);
	pDayText->CreateSub( this, "RNATTENDANCEPAGE_DAY_INFO_DAY" );
	pDayText->SetFont( pFont9 );
	pDayText->SetTextAlign( TEXT_ALIGN_LEFT|TEXT_ALIGN_TOP );
	pDayText->SetLineInterval ( 5.0f );
	pDayText->SetPartInterval ( 2.0f );
	RegisterControl( pDayText );
	m_pDayText = pDayText;

	int nTotalLine = pDayText->GetVisibleLine ();

	//	날짜 정보 창 스크롤바
	CBasicScrollBarEx* pInfoListScrollBar = new CBasicScrollBarEx(m_pEngineDevice);
	pInfoListScrollBar->CreateSub ( this, "BASIC_SCROLLBAR", UI_FLAG_RIGHT | UI_FLAG_YSIZE );
	pInfoListScrollBar->CreateBaseScrollBar ( "RNATTENDANCEPAGE_DAY_INFO_SCROLLBAR" );
	pInfoListScrollBar->GetThumbFrame()->SetState ( 1, nTotalLine );
	RegisterControl ( pInfoListScrollBar );
	m_pInfoListScrollBar = pInfoListScrollBar;
	m_pInfoListScrollBar->GetThumbFrame ()->SetScrollParent(m_pDayText);
	m_pInfoListScrollBar->SetVisibleSingle(FALSE);

	//퀘스트 슬롯
	for ( int i = 0; i < MAXQUESTSLOT; ++i ) 
	{
		CString strKey;
		strKey.Format( "RNATTENDANCEPAGE_QUEST_SLOT_LIST_%d", i );
		RnAttendanceQuestSlot* pQuestSlot = new RnAttendanceQuestSlot( m_pGaeaClient, m_pInterface, m_pEngineDevice);
		pQuestSlot->CreateSub( this, strKey, UI_FLAG_DEFAULT );
		pQuestSlot->CreateSubControl();
		//pQuestSlot->SetUseRender(true);
		//pQuestSlot->SetText(strKey);
		RegisterControl( pQuestSlot );
		m_pQuestSlot.push_back(pQuestSlot);
	}
	//m_pQuestSlot[0]->SetDiffuse(NS_UITEXTCOLOR::ORANGE);
	//m_pQuestSlot[1]->SetDiffuse(NS_UITEXTCOLOR::AQUAMARINE);

	CUIGroup* pInfoQuestScrollPos = new CUIGroup(m_pEngineDevice);
	pInfoQuestScrollPos->CreateSub ( this, "RNATTENDANCEPAGE_QUEST_SLOT_SCROLL_POS", UI_FLAG_DEFAULT);
	RegisterControl ( pInfoQuestScrollPos );

	//	날짜 정보 창 스크롤바
	CBasicScrollBarNoTHUMB* pInfoQuestScrollBar = new CBasicScrollBarNoTHUMB(m_pEngineDevice);
	pInfoQuestScrollBar->CreateSub ( this, "BASIC_SCROLLBAR", UI_FLAG_RIGHT | UI_FLAG_YSIZE );
	pInfoQuestScrollBar->CreateBaseScrollBar ( "RNATTENDANCEPAGE_DAY_INFO_QUEST_SCROLLBAR" );
	pInfoQuestScrollBar->GetThumbFrame()->SetState ( 0, MAXQUESTSLOT );
	RegisterControl ( pInfoQuestScrollBar );
	m_pInfoQuestScrollBar = pInfoQuestScrollBar;
	m_pInfoQuestScrollBar->GetThumbFrame ()->SetScrollParent(pInfoQuestScrollPos);
	m_nQuestListScrollPos = 0;
	m_pInfoQuestScrollBar->SetVisibleSingle(FALSE);

}

void RnattendanceDayInfo::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroup::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	{
		CBasicScrollThumbFrame* const pThumbFrame = m_pInfoListScrollBar->GetThumbFrame ();

		const int nTotalLine = m_pDayText->GetTotalLine ();
		const int nLinePerOneView = m_pDayText->GetVisibleLine ();
		CDebugSet::ToView ( 1, 10, "보이는 라인 %d", nLinePerOneView );
		pThumbFrame->SetState ( nTotalLine, nLinePerOneView );
		if ( nLinePerOneView < nTotalLine )
		{
			const int nMovableLine = nTotalLine - nLinePerOneView;
			float fPercent = pThumbFrame->GetPercent ();
			int nPos = (int)floor(fPercent * nMovableLine);
			m_pDayText->SetCurLine ( nPos );		
		}
	}

	{

		CBasicScrollThumbFrame* const pThumbFrame = m_pInfoQuestScrollBar->GetThumbFrame ();

		const int nTotal = pThumbFrame->GetTotal ();
		const int nViewPerPage = pThumbFrame->GetViewPerPage ();

		if ( nTotal <= 1 )
			m_pInfoQuestScrollBar->SetVisibleSingleUp(false);

		if ( nViewPerPage < nTotal )
		{
			int nCurPos = 0;
			const int nMovableLine = nTotal - nViewPerPage;
			float fPercent = pThumbFrame->GetPercent ();

			nCurPos = (int)floor(fPercent * nMovableLine);
			if ( nCurPos < 0 )
				nCurPos = 0;

			if ( m_nQuestListScrollPos == nCurPos )
				return;

			m_nQuestListScrollPos = nCurPos;

			if (nCurPos < 1)
				m_pInfoQuestScrollBar->SetVisibleSingleUp(false);
			else
				m_pInfoQuestScrollBar->SetVisibleSingleUp(true);

			if (nTotal - 2 <= nCurPos)
				m_pInfoQuestScrollBar->SetVisibleSingleDowon(false);
			else
				m_pInfoQuestScrollBar->SetVisibleSingleDowon(true);

			UpdateSlot();
		}
	}
}

void RnattendanceDayInfo::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{

}

void RnattendanceDayInfo::SetInfoLScroolBarUpDate(CBasicTextBoxEx* pBasicTextBoxEx)
{
	CBasicScrollThumbFrame* const pThumbFrame = m_pInfoListScrollBar->GetThumbFrame ();
	if ( !pThumbFrame )
	{
		GASSERT ( 0 && "썸이 만들어지지 않았습니다.");
		return ;
	}

	int nTotal, nView;
	pThumbFrame->GetState ( nTotal, nView );	

	const int nTotalLine = m_pDayText->GetTotalLine();

	if ( nTotalLine > 3)
	{
		m_pInfoListScrollBar->SetVisibleSingle(TRUE);
	}
	else
	{
		m_pInfoListScrollBar->SetVisibleSingle(FALSE);
	}
	pThumbFrame->SetState ( nTotalLine, nView );
	pThumbFrame->SetPercent ( CBasicScrollThumbFrame::fHUNDRED_PERCENT );
}


void RnattendanceDayInfo::SetSlotLScroolBarUpDate()
{
	CBasicScrollThumbFrame* const pThumbFrame = m_pInfoQuestScrollBar->GetThumbFrame ();
	if ( !pThumbFrame )
	{
		GASSERT ( 0 && "썸이 만들어지지 않았습니다.");
		return ;
	}

	//int nTotal, nView;
	//pThumbFrame->GetState ( nTotal, nView );	

	const int nTotalLine = m_pGaeaClient->GetAttendanceCount();

	if ( nTotalLine > 2)
	{
		m_pInfoQuestScrollBar->SetVisibleSingle(TRUE);
	}
	else
	{
		m_pInfoQuestScrollBar->SetVisibleSingle(FALSE);
	}
	pThumbFrame->SetState ( nTotalLine, MAXQUESTSLOT );
	//pThumbFrame->SetPercent ( CBasicScrollThumbFrame::fHUNDRED_PERCENT );
	pThumbFrame->SetPercent ( 0 );
	m_nQuestListScrollPos = 0;
}

void RnattendanceDayInfo::SetDayInfoText( const CString strEventText, const D3DCOLOR& TextColor /*= NS_UITEXTUTIL::ENABLE */ )
{
	m_pDayText->AddText( strEventText, TextColor );
	SetInfoLScroolBarUpDate( m_pDayText );
}

void RnattendanceDayInfo::ClearDayInfo()
{
	m_pDayText->ClearText();

	BOOST_FOREACH (RnAttendanceQuestSlot* pQuestSlot, m_pQuestSlot)
	{
		pQuestSlot->ClearSlot();
		pQuestSlot->SetVisibleSingle( FALSE );
		pQuestSlot->SetVisibleSingleReWardButton( FALSE );
		pQuestSlot->SetVisibleSingleSelectButton( FALSE );
		pQuestSlot->SetQuestId(UINT_MAX);
	}
}

void RnattendanceDayInfo::UpdateSlot()
{
	CTime TTdoay = CTime::GetCurrentTime();
	SDAYDATA sSelectDay = m_pGaeaClient->GetSelectDay();
	/*std::vector<SATTENDANCEQUESTDATA> sAttendanceQuestData;
	sAttendanceQuestData = m_pGaeaClient->GetAttendanceQuestData();*/

	
	if ( (TTdoay.GetDay() != sSelectDay.m_nDays) &&
		((TTdoay.GetMonth() == sSelectDay.m_nMonth) &&
		(TTdoay.GetYear() == sSelectDay.m_nYear)) )
	{
		//SDAYDATA sSelectDay = m_pGaeaClient->GetSelectDay();

		if ( sSelectDay.m_nAnyDay > 0)
		{
			std::string strQuestReward = sc::string::format(RNATTENDANCE_DAY_INFO_ANY_DAY_REWARD, sSelectDay.m_nAnyDay);

			m_pQuestSlot[0]->SetInfoText(
				strQuestReward.c_str(), NS_UITEXTCOLOR::DARKORANGE,
				"", NS_UITEXTCOLOR::WHITE,
				0, NS_UITEXTCOLOR::WHITE,
				0, NS_UITEXTCOLOR::WHITE,
				RNATTENDANCE_DAY_INFO_REWARDITEM, NS_UITEXTCOLOR::WHITE,
				GLQuestMan::GetInstance().GetRnattendanceRewardNDay( sSelectDay.m_nAnyDay ));
			m_pQuestSlot[0]->SetVisibleSingle( TRUE );
		}
		return;
	}

	if ( (TTdoay.GetMonth() != sSelectDay.m_nMonth) ||
		(TTdoay.GetYear() != sSelectDay.m_nYear) )
	{
		return;
	}

	std::vector<SATTENDANCEQUESTDATA> sAttendanceQuestData;
	sAttendanceQuestData = m_pGaeaClient->GetAttendanceQuestData();
	//  내용을 출력한다.
	int nSize = (int)sAttendanceQuestData.size();

	ClearSlot();
	//for ( int i = 0; i < MAXQUESTSLOT; ++i ) 
	//{
	//	if (m_pQuestSlot[i])
	//	{
	//		m_pQuestSlot[i]->ClearSlot();
	//		m_pQuestSlot[i]->SetVisibleSingle( FALSE );
	//		m_pQuestSlot[i]->SetVisibleSingleReWardButton( FALSE );
	//		m_pQuestSlot[i]->SetVisibleSingleSelectButton( FALSE );
	//		m_pQuestSlot[i]->SetQuestId(UINT_MAX);
	//	}
	//}

	int nIndex = 0;

	for ( int i = 0; i < nSize; ++i )
	{
		if ( i < m_nQuestListScrollPos )
			continue;

		if ( nIndex >= MAXQUESTSLOT )
			break;

		std::string strQuestCondition = sc::string::format(RNATTENDANCE_DAY_INFO_CONDITION, i + 1);

		m_pQuestSlot[nIndex]->SetInfoText(
			strQuestCondition.c_str(), NS_UITEXTCOLOR::DARKORANGE,
			(sAttendanceQuestData[i].strTitle + sAttendanceQuestData[i].strProgress).c_str(), NS_UITEXTCOLOR::WHITE,
			sAttendanceQuestData[i].dwMoney, NS_UITEXTCOLOR::WHITE,
			sAttendanceQuestData[i].lnExp, NS_UITEXTCOLOR::WHITE,
			RNATTENDANCE_DAY_INFO_REWARDITEM, NS_UITEXTCOLOR::WHITE,
			sAttendanceQuestData[i].dwItemID);
		m_pQuestSlot[nIndex]->SetVisibleSingle( TRUE );

		m_pQuestSlot[nIndex]->SetQuestId( sAttendanceQuestData[i].dwQuestID );

		if ( sAttendanceQuestData[i].bComplete )
			m_pQuestSlot[nIndex]->SetVisibleSingleReWardButton( TRUE );

		GLQuestPlay& cQuestPlay = m_pGaeaClient->GetCharacter()->m_cQuestPlay;
		GLQUESTPROG* pQUEST_PROG = cQuestPlay.FindProc ( sAttendanceQuestData[i].dwQuestID );
		GLQUESTPROG* pQUEST_End = cQuestPlay.FindEnd ( sAttendanceQuestData[i].dwQuestID );

		if ( !pQUEST_PROG && !pQUEST_End )
		{
			m_pQuestSlot[nIndex]->SetVisibleSingleSelectButton( TRUE );
		}

		if ( pQUEST_End )
		{
			CTime TToday = CTime::GetCurrentTime();
			GLQUEST* pQUEST = GLQuestMan::GetInstance().Find ( sAttendanceQuestData[i].dwQuestID );
			if ( !pQUEST )
				continue;
			CTime CQuestStartTime(pQUEST_End->m_tStartTime);

			if ( CQuestStartTime.GetYear() != TToday.GetYear() ||
				CQuestStartTime.GetMonth() != TToday.GetMonth() ||
				CQuestStartTime.GetDay() != TToday.GetDay() )
			{
				m_pQuestSlot[nIndex]->SetVisibleSingleSelectButton( TRUE );
			}
		}

		nIndex++;
	}
}

void RnattendanceDayInfo::ClearSlot()
{
	for ( int i = 0; i < MAXQUESTSLOT; ++i ) 
	{
		if (m_pQuestSlot[i])
		{
			m_pQuestSlot[i]->ClearSlot();
			m_pQuestSlot[i]->SetVisibleSingle( FALSE );
			m_pQuestSlot[i]->SetVisibleSingleReWardButton( FALSE );
			m_pQuestSlot[i]->SetVisibleSingleSelectButton( FALSE );
			m_pQuestSlot[i]->SetQuestId(UINT_MAX);
		}
	}
}
