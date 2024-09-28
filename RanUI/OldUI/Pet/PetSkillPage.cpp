#include "StdAfx.h"

#include "PetSkillPage.h"
#include "PetSkillSlot.h"

#include "../../../EngineLib/GUInterface/BasicScrollBarEx.h"
#include "../../../EngineLib/GUInterface/BasicScrollThumbFrame.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

const int CPetSkillPage::nSTARTLINE = 0;
const int CPetSkillPage::nOUTOFRANGE = -1;

CPetSkillPage::CPetSkillPage(GLGaeaClient* pGaeaClient, EngineDeviceMan* pEngineDevice)
	: CUIGroup(pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
    , m_pScrollBar(NULL)
	, m_nCurPos(-1)
	, m_nSkillCount(-1)
{
	memset ( m_pSkillSlotArray, 0, sizeof ( m_pSkillSlotArray ) );	
	memset ( m_pSkillSlotArrayDummy, 0, sizeof ( m_pSkillSlotArrayDummy ) );		
}

CPetSkillPage::~CPetSkillPage ()
{
}

CUIControl*	CPetSkillPage::CreateControl ( const char* szControl )
{
	CUIControl* pControl = new CUIControl(m_pEngineDevice);
	pControl->CreateSub ( this, szControl );
	RegisterControl ( pControl );

	return pControl;
}

CPetSkillSlot* CPetSkillPage::CreateSkillSlot ( CString strKeyword, UIGUID ControlID, SNATIVEID sNativeID )
{
	CPetSkillSlot* pSkillSlot = new CPetSkillSlot(m_pGaeaClient, m_pEngineDevice);
	pSkillSlot->CreateSub ( this, strKeyword.GetString (), UI_FLAG_DEFAULT, ControlID );
	pSkillSlot->CreateSubControl ( sNativeID );
	RegisterControl ( pSkillSlot );

	return pSkillSlot;
}

void CPetSkillPage::CreateSubControl ( SNATIVEID sNativeIDArray[GLSkillMan::MAX_CLASSSKILL], int nSkillCount )
{	
	if ( nSkillCount < 1 ) return ;

	CString strSlotKeyword[nMAX_ONE_VIEW_SLOT] =
	{
		"PET_SKILL_PAGE_SLOT0",
		"PET_SKILL_PAGE_SLOT1",
		"PET_SKILL_PAGE_SLOT2",
		"PET_SKILL_PAGE_SLOT3",
	};

	{	//	더미 슬롯
		for ( int i = 0; i < nMAX_ONE_VIEW_SLOT; i++ )
		{
			m_pSkillSlotArrayDummy[i] = CreateControl ( strSlotKeyword[i].GetString () );
		}
	}

	{	//	실제 데이타		
		if ( nSkillCount < nMAX_ONE_VIEW_SLOT )
		{
			nSkillCount = nMAX_ONE_VIEW_SLOT;
		}

		for ( int i = 0; i < nSkillCount; i++ )
		{
			m_pSkillSlotArray[i] = CreateSkillSlot ( strSlotKeyword[0], SKILL_SLOT0 + i, sNativeIDArray[i] );
		}
	}

	{	//	스크롤바
		CBasicScrollBarEx* pScrollBar = new CBasicScrollBarEx(m_pEngineDevice);
		pScrollBar->CreateSub ( this, "BASIC_SCROLLBAR", UI_FLAG_RIGHT | UI_FLAG_YSIZE );
		pScrollBar->CreateBaseScrollBar ( "PET_SKILL_SCROLLBAR" );
		pScrollBar->GetThumbFrame()->SetState ( nSkillCount, nMAX_ONE_VIEW_SLOT );
		RegisterControl ( pScrollBar );
		m_pScrollBar = pScrollBar;
	}
	m_nSkillCount = nSkillCount;
	SetSkillSlotRender ( nSTARTLINE, nMAX_ONE_VIEW_SLOT );
}

void CPetSkillPage::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	if ( !IsVisible () ) return ;

	m_nSlotIndex = nOUTOFRANGE;
	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	if ( m_pScrollBar )
	{
		CBasicScrollThumbFrame* pThumbFrame = m_pScrollBar->GetThumbFrame ();
		int nTotal = pThumbFrame->GetTotal ();

		if ( nTotal <= nMAX_ONE_VIEW_SLOT ) return ;

		const int nViewPerPage = pThumbFrame->GetViewPerPage ();
		if ( nViewPerPage < nTotal )
		{
			int nCurPos = nSTARTLINE;
			const int nMovableLine = nTotal - nViewPerPage;
			float fPercent = pThumbFrame->GetPercent ();

			nCurPos = (int)floor(fPercent * nMovableLine);
			if ( nCurPos < nSTARTLINE ) nCurPos = nSTARTLINE;

			if ( m_nCurPos == nCurPos ) return;

			m_nCurPos = nCurPos;

            ResetAllSkillSlotRender ( nTotal );

			SetSkillSlotRender ( nCurPos, nCurPos + nViewPerPage );
		}
	}
}

void CPetSkillPage::ResetAllSkillSlotRender ( int nTotal )
{
	if ( nTotal < 0 ) return ;

	for ( int i = 0; i < nTotal; i++ )
	{
		CPetSkillSlot* pSkillSlot = m_pSkillSlotArray[i];
		if ( pSkillSlot )
		{
			pSkillSlot->SetVisibleSingle ( FALSE );
		}
	}
}

void CPetSkillPage::SetSkillSlotRender ( int nStartIndex, int nTotal )
{
	if ( nTotal < 0 ) return ;

	const UIRECT& rcParentPos = GetGlobalPos ();

	for ( int i = nStartIndex; i < nTotal; i++ )
	{
		CPetSkillSlot* pSkillSlot = m_pSkillSlotArray[i];
		if ( pSkillSlot )
		{
			int nAbsoluteIndex = i - nStartIndex;
			CUIControl* pDummyControl = m_pSkillSlotArrayDummy[nAbsoluteIndex];

			const UIRECT& rcSlotPos = pDummyControl->GetGlobalPos ();
			const UIRECT& rcSlotLocalPos = pDummyControl->GetLocalPos ();

			pSkillSlot->SetLocalPos ( rcSlotLocalPos );			
			pSkillSlot->SetGlobalPos ( rcSlotPos );

			pSkillSlot->SetVisibleSingle ( TRUE );
		}
	}
}

SNATIVEID  CPetSkillPage::GetSkillID ( int nIndex )
{	
	return m_pSkillSlotArray[nIndex]->GetNativeSkillID ();
}

void CPetSkillPage::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	CUIGroup::TranslateUIMessage ( ControlID, dwMsg );

	if ( SKILL_SLOT0 <= ControlID && ControlID <= SKILL_SLOT_END )
	{
		if ( CHECK_MOUSE_IN ( dwMsg ) )
		{
			m_nSlotIndex = ControlID - SKILL_SLOT0;
		}
	}
}

void CPetSkillPage::ReloadSkillInfo()
{
	this->ResetAllSkillSlotRender(m_nSkillCount);
	// RM# 1314 [5/21/2015 gbgim];
	int nSkillCount(0);
	for ( int i = 0; i < GLSkillMan::MAX_CLASSSKILL; i++ )
	{
		const SNATIVEID sNid(EMSKILL_PET, i);
		PGLSKILL pSkill = GLSkillMan::GetInstance().GetData (sNid);
		if ( pSkill && ( pSkill->m_sBASIC.bLearnView == false || (m_pGaeaClient->GetPetClient()->ISLEARNED_SKILL(sNid) == TRUE)) )
		{
			m_pSkillSlotArray[nSkillCount++]->SetNativeSkillID(sNid);
		}
	}
	m_nSkillCount = nSkillCount;
	m_pScrollBar->GetThumbFrame()->SetState(m_nSkillCount, nMAX_ONE_VIEW_SLOT);	
	this->SetSkillSlotRender(nSTARTLINE, nMAX_ONE_VIEW_SLOT);
}