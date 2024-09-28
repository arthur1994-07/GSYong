#include "StdAfx.h"
#include "../../../RanLogicClient/GLGaeaClient.h"
#include "SkillPage.h"
#include "SkillSlot.h"
#include "../../../EngineLib/GUInterface/BasicScrollBarEx.h"
#include "../../../EngineLib/GUInterface/BasicScrollThumbFrame.h"
#include "../../../RanLogicClient/Tutorial/GLBaseTutorial.h"
#include "../Tutorial/ExplainInterface.h"
#include "../../InnerInterface.h"
#include "../../../EngineLib/GUInterface/TipBox.h"
#include "SkillWindow.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

const int CSkillPage::nSTARTLINE = 0;
const int CSkillPage::nOUTOFRANGE = -1;

CSkillPage::CSkillPage(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
	, m_pInterface(pInterface)
    , m_pScrollBar(NULL)
    , m_nCurPos(-1)
    , m_nMaxSkillCnt(0)
{
	memset ( m_pSkillSlotArray, 0, sizeof ( CSkillSlot* ) * GLSkillMan::MAX_CLASSSKILL );	
	memset ( m_pSkillSlotArrayDummy, 0, sizeof ( CUIControl* ) * nMAX_ONE_VIEW_SLOT );	

	m_vecViewSkill.clear();
	m_vecNotViewSkill.clear();
}

CSkillPage::~CSkillPage ()
{
}

CUIControl*	CSkillPage::CreateControl ( const char* szControl )
{
	CUIControl* pControl = new CUIControl(m_pEngineDevice);
	pControl->CreateSub ( this, szControl );
	RegisterControl ( pControl );
	return pControl;
}

void CSkillPage::CreateSubControl ( SNATIVEID sNativeIDArray[GLSkillMan::MAX_CLASSSKILL], int nSkillCount )
{	
	if ( nSkillCount < 1 ) return ;

	CString strSlotKeyword[nMAX_ONE_VIEW_SLOT] =
	{
		"SKILL_SLOT0",
		"SKILL_SLOT1",
		"SKILL_SLOT2",
		"SKILL_SLOT3",
		"SKILL_SLOT4",
		"SKILL_SLOT5",
		"SKILL_SLOT6",
		"SKILL_SLOT7"
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

		m_vecViewSkill.clear();
		m_vecNotViewSkill.clear();
		m_nMaxSkillCnt = nSkillCount;
	}

	{	//	스크롤바
		CBasicScrollBarEx* pScrollBar = new CBasicScrollBarEx(m_pEngineDevice);
		pScrollBar->CreateSub ( this, "BASIC_SCROLLBAR", UI_FLAG_RIGHT | UI_FLAG_YSIZE );
		pScrollBar->CreateBaseScrollBar ( "SKILL_SCROLLBAR" );
		pScrollBar->GetThumbFrame()->SetState ( nSkillCount, nMAX_ONE_VIEW_SLOT );
		RegisterControl ( pScrollBar );
		m_pScrollBar = pScrollBar;
	}

	SetSkillSlotRender ( nSTARTLINE, nMAX_ONE_VIEW_SLOT );
}

CSkillSlot*	CSkillPage::CreateSkillSlot ( CString strKeyword, UIGUID ControlID, SNATIVEID sNativeID )
{
	CSkillSlot* pSkillSlot = new CSkillSlot ( m_pGaeaClient, m_pInterface, m_pEngineDevice );
	pSkillSlot->CreateSub ( this, strKeyword.GetString (), UI_FLAG_DEFAULT, ControlID );
	pSkillSlot->CreateSubControl ( sNativeID );
	RegisterControl ( pSkillSlot );
	return pSkillSlot;
}

void CSkillPage::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	if ( !IsVisible () ) return ;

	m_nSlotIndex = nOUTOFRANGE;
	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	if ( m_pScrollBar )
	{
		CBasicScrollThumbFrame* pThumbFrame = m_pScrollBar->GetThumbFrame ();

		const int nViewPerPage = pThumbFrame->GetViewPerPage ();
		int nBeforeSkillCnt = static_cast<int>(m_vecViewSkill.size());

		m_vecViewSkill.clear();
		m_vecNotViewSkill.clear();
		for ( int i = 0; i < m_nMaxSkillCnt; i++ )
		{
			SNATIVEID sSkillID = m_pSkillSlotArray[i]->GetNativeSkillID();
			PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( sSkillID );
			if( pSkill && pSkill->m_sBASIC.bLearnView && !m_pGaeaClient->GetCharacter()->GETLEARNED_SKILL( sSkillID ) )
			{
				m_vecNotViewSkill.push_back( i );
			}else{
				m_vecViewSkill.push_back( i );
			}
		}
		if( nBeforeSkillCnt != m_vecViewSkill.size() )
		{
			m_pScrollBar->GetThumbFrame()->SetState ( static_cast<int>(m_vecViewSkill.size()), nMAX_ONE_VIEW_SLOT );
			ResetAllSkillSlotRender( m_nMaxSkillCnt );
			SetSkillSlotRender ( 0, 0 + nViewPerPage );			
		}

		int nTotal = pThumbFrame->GetTotal ();
		if ( nTotal <= nMAX_ONE_VIEW_SLOT ) return ;

		if ( nViewPerPage < nTotal )
		{
			int nCurPos = nSTARTLINE;
			const int nMovableLine = nTotal - nViewPerPage;
			float fPercent = pThumbFrame->GetPercent ();

			nCurPos = (int)floor(fPercent * nMovableLine);
			if ( nCurPos < nSTARTLINE ) nCurPos = nSTARTLINE;

			if ( m_nCurPos == nCurPos ) return;

			m_nCurPos = nCurPos;

            ResetAllSkillSlotRender ( m_nMaxSkillCnt );
			SetSkillSlotRender ( nCurPos, nCurPos + nViewPerPage );
		}
	}
}

void CSkillPage::ResetAllSkillSlotRender ( int nTotal )
{
	if ( nTotal < 0 ) return ;

	for ( int i = 0; i < nTotal; i++ )
	{
		CSkillSlot* pSkillSlot = m_pSkillSlotArray[i];
		if ( pSkillSlot )
		{
			pSkillSlot->SetVisibleSingle ( FALSE );
		}
	}

	if ( m_pGaeaClient->GetTutorial()->IsQuickNActionSlotStep() &&
		m_pGaeaClient->GetTutorial()->GetCurrentSubStep() >= m_pGaeaClient->GetTutorial()->SIX_
		&& m_pGaeaClient->GetTutorial()->GetCurrentSubStep() <= m_pGaeaClient->GetTutorial()->SEVEN_ )
	{
		m_pInterface->GetExplainInterface()->SetSlotBoxVisibleSingle( FALSE );
		m_pInterface->ExplainSetSlotBoxVisible(0, FALSE);

		m_pInterface->GetExplainInterface()->SetSlotBoxVisibleSingle( FALSE );
		m_pInterface->ExplainSetSlotBoxVisible(1, FALSE);
	}
}

void CSkillPage::SetSkillSlotRender ( int nStartIndex, int nTotal )
{
	if (nTotal < 0)
        return;

	const UIRECT& rcParentPos = GetGlobalPos ();

	int iViewIndex = 0; // 보여질 위치
	int iSlotNum   = 0;	// 보여지는 SlotNum
	for ( int i = nStartIndex; i < static_cast<int>(m_vecViewSkill.size()); i++ )
	{
		if (i >= m_nMaxSkillCnt)
            break;
		if (i >= nTotal)
            break;
		if (iViewIndex >= nMAX_ONE_VIEW_SLOT)
            break;

		iSlotNum = m_vecViewSkill[i];

		CSkillSlot* pSkillSlot = m_pSkillSlotArray[iSlotNum];
		if ( pSkillSlot )
		{

			int nAbsoluteIndex = iViewIndex;
			CUIControl* pDummyControl = m_pSkillSlotArrayDummy[nAbsoluteIndex];

			const UIRECT& rcSlotPos = pDummyControl->GetGlobalPos ();
			const UIRECT& rcSlotLocalPos = pDummyControl->GetLocalPos ();

			pSkillSlot->SetLocalPos ( rcSlotLocalPos );			
			pSkillSlot->SetGlobalPos ( rcSlotPos );

			pSkillSlot->SetVisibleSingle ( TRUE );
			pSkillSlot->SetViewSkillSlot( TRUE );

			iViewIndex++;

			if ( m_pGaeaClient->GetTutorial()->IsQuickNActionSlotStep() &&
				m_pGaeaClient->GetTutorial()->GetCurrentSubStep() >= m_pGaeaClient->GetTutorial()->SIX_
				&& m_pGaeaClient->GetTutorial()->GetCurrentSubStep() <= m_pGaeaClient->GetTutorial()->SEVEN_ )
			{
				if ( m_pGaeaClient->GetTutorial()->m_sSkillID[ 0 ] == pSkillSlot->GetNativeSkillID() )
				{	// 튜토리얼 중 장착해야 할 스킬 아이콘이 보이면, UI 그린다.
					D3DXVECTOR2 vPos;
					vPos.x = rcSlotPos.left;
					vPos.y = rcSlotPos.top;

					//m_pInterface->GetExplainInterface()->GetSlotBox()->SetGlobalPos( vPos );
					//m_pInterface->GetExplainInterface()->GetSlotBox()->SetVisibleSingle( TRUE );
					m_pInterface->GetExplainInterface()->SetSlotBoxOriginPos( vPos, 0 );
					m_pInterface->GetExplainInterface()->SetSlotBoxOriginPos( m_pInterface->GetSkillWindow()->GetUIWindowGlobalPos(), 0 );
					m_pInterface->ExplainSetSlotBoxVisible(0, TRUE);
				}

				if ( m_pGaeaClient->GetTutorial()->m_sSkillID[ 1 ] == pSkillSlot->GetNativeSkillID() )
				{	// 튜토리얼 중 장착해야 할 스킬 아이콘이 보이면, UI 그린다.
					D3DXVECTOR2 vPos;
					vPos.x = rcSlotPos.left;
					vPos.y = rcSlotPos.top;

					//m_pInterface->GetExplainInterface()->GetSlotBox()->SetGlobalPos( vPos );
					//m_pInterface->GetExplainInterface()->GetSlotBox()->SetVisibleSingle( TRUE );
					m_pInterface->GetExplainInterface()->SetSlotBoxOriginPos( vPos, 1 );
					m_pInterface->GetExplainInterface()->SetSlotBoxOriginPos( m_pInterface->GetSkillWindow()->GetUIWindowGlobalPos(), 1 );
					m_pInterface->ExplainSetSlotBoxVisible(1, TRUE);
				}
			}
		}
	}

	if (m_vecViewSkill.size() < nMAX_ONE_VIEW_SLOT && !m_vecNotViewSkill.empty())
	{
		for ( int i = nStartIndex; i < static_cast<int>(m_vecNotViewSkill.size()); i++ )
		{
			if (iViewIndex >= nMAX_ONE_VIEW_SLOT)
                break;

			iSlotNum = m_vecNotViewSkill[i];

			CSkillSlot* pSkillSlot = m_pSkillSlotArray[iSlotNum];
			if (pSkillSlot)
			{

				int nAbsoluteIndex = iViewIndex;
				CUIControl* pDummyControl = m_pSkillSlotArrayDummy[nAbsoluteIndex];

				const UIRECT& rcSlotPos = pDummyControl->GetGlobalPos ();
				const UIRECT& rcSlotLocalPos = pDummyControl->GetLocalPos ();

				pSkillSlot->SetLocalPos ( rcSlotLocalPos );			
				pSkillSlot->SetGlobalPos ( rcSlotPos );

				pSkillSlot->SetVisibleSingle ( TRUE );
				pSkillSlot->SetViewSkillSlot(FALSE);

				iViewIndex++;

				if ( m_pGaeaClient->GetTutorial()->IsQuickNActionSlotStep() &&
					m_pGaeaClient->GetTutorial()->GetCurrentSubStep() >= m_pGaeaClient->GetTutorial()->SIX_
					&& m_pGaeaClient->GetTutorial()->GetCurrentSubStep() <= m_pGaeaClient->GetTutorial()->SEVEN_ )
				{
					if ( m_pGaeaClient->GetTutorial()->m_sSkillID[ 0 ] == pSkillSlot->GetNativeSkillID() )
					{	// 튜토리얼 중 장착해야 할 스킬 아이콘이 보이면, UI 그린다.
						D3DXVECTOR2 vPos;
						vPos.x = rcSlotPos.left;
						vPos.y = rcSlotPos.top;

						//m_pInterface->GetExplainInterface()->GetSlotBox()->SetGlobalPos( vPos );
						//m_pInterface->GetExplainInterface()->GetSlotBox()->SetVisibleSingle( TRUE );
						m_pInterface->GetExplainInterface()->SetSlotBoxOriginPos(vPos, 0);
						m_pInterface->GetExplainInterface()->SetSlotBoxOriginPos(m_pInterface->GetSkillWindow()->GetUIWindowGlobalPos(), 0);
						m_pInterface->ExplainSetSlotBoxVisible(0, TRUE);
					}

					if ( m_pGaeaClient->GetTutorial()->m_sSkillID[ 1 ] == pSkillSlot->GetNativeSkillID() )
					{	// 튜토리얼 중 장착해야 할 스킬 아이콘이 보이면, UI 그린다.
						D3DXVECTOR2 vPos;
						vPos.x = rcSlotPos.left;
						vPos.y = rcSlotPos.top;

						//m_pInterface->GetExplainInterface()->GetSlotBox()->SetGlobalPos( vPos );
						//m_pInterface->GetExplainInterface()->GetSlotBox()->SetVisibleSingle( TRUE );
						m_pInterface->GetExplainInterface()->SetSlotBoxOriginPos(vPos, 1);
						m_pInterface->GetExplainInterface()->SetSlotBoxOriginPos(m_pInterface->GetSkillWindow()->GetUIWindowGlobalPos(), 1);
						m_pInterface->ExplainSetSlotBoxVisible(1, TRUE);
					}
				}
			}
		}
	}
}

SNATIVEID  CSkillPage::GetSkillID ( int nIndex )
{	
	return m_pSkillSlotArray[nIndex]->GetNativeSkillID ();
}

BOOL  CSkillPage::GetViewSkillSlot ( int nIndex )
{	
	return m_pSkillSlotArray[nIndex]->GetViewSkillSlot();
}

void CSkillPage::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
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
