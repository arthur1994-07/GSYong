#include "StdAfx.h"

#include "../../../../EngineLib/GUInterface/BasicScrollBarEx.h"
#include "../../../../EngineLib/GUInterface/BasicScrollThumbFrame.h"
#include "../../../../RanLogicClient/DamageRecord/DamageRecord.h"
#include "../../../../RanLogicClient/GLGaeaClient.h"
#include "./DetailSlot/DetailSlot.h"
#include "DetailPage.h"

DetailPage::DetailPage(EngineDeviceMan* pEngineDevice, GLGaeaClient* pGaeaClient)
: CUIGroupHelper(pEngineDevice)
, m_pGaeaClient(pGaeaClient)
, m_nSelectActorIndex(DETAIL_NSIZE)
, m_nCurrentBeginIndex(0)
{
}

void DetailPage::SetActorIndex(const DWORD nActorIndex)
{
	m_nSelectActorIndex = nActorIndex;
}

void DetailPage::SetVisibleSingle(BOOL bVisible)
{
	if ( bVisible == TRUE )
	{
		if ( m_nSelectActorIndex > (DETAIL_NSIZE - 1) )
			return;	
	}
	else
		m_nSelectActorIndex = DETAIL_NSIZE;

	CUIGroupHelper::SetVisibleSingle(bVisible);
}

void DetailPage::CreateSubControl(void)
{
//	CreateLineBox("DPS_WINDOW_DETAIL_PAGE_BOX", "PARTY_INFO_LINEBOX_TEXTUREINFO");

	{ // 상세 정보 슬롯;
		for ( DWORD _i = DETAIL_NSIZE + 1; _i; --_i )
		{
			DetailSlot* pDetailSlot = new DetailSlot(m_pEngineDevice, m_pGaeaClient, _i - 1);
			CString strDetailSlot;
			strDetailSlot.Format("DPS_WINDOW_SLOT%d", _i - 1);
			pDetailSlot->CreateSub(this, strDetailSlot.GetString (), UI_FLAG_DEFAULT);
			pDetailSlot->CreateSubControl();
			RegisterControl(pDetailSlot);
			m_pDetailSlot[_i - 1] = pDetailSlot;
		}
	}

	{ // 스크롤 바;
		m_pScrollBar = new CBasicScrollBarEx(m_pEngineDevice);
		m_pScrollBar->CreateSub(this, "BASIC_SCROLLBAR", UI_FLAG_RIGHT | UI_FLAG_YSIZE);
		m_pScrollBar->CreateBaseScrollBar("DPS_WINDOW_DETAIL_PAGE_SCROLLBAR");
		m_pScrollBar->GetThumbFrame()->SetScrollParent(this);
		m_pScrollBar->GetThumbFrame()->SetScrollMoveLine(1);
		RegisterControl(m_pScrollBar);
		m_pScrollBar->SetVisibleSingle(FALSE);
	}	

	SetVisibleSingle(FALSE);
}

void DetailPage::Update(int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl)
{
	CUIGroupHelper::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );	

	const DamageRecord::Manager* const pDamageRecord = m_pGaeaClient->GetDamageRecord();
	const DamageRecord::ViewElement* const pElement = pDamageRecord->GetViewElement(m_nSelectActorIndex);

	if ( pElement == NULL )
	{
		m_nCurrentBeginIndex = 0;
		m_pScrollBar->SetVisibleSingle(FALSE);
		SetVisibleSingle(FALSE);
		return;
	}

	{ // Update Scroll Bar;
		CBasicScrollThumbFrame* const pThumbFrame = m_pScrollBar->GetThumbFrame();
		const int nElement = int(pElement->getNDetailElement());
		pThumbFrame->SetState(nElement, DETAIL_NSIZE);

		const int nMovableLine = nElement - DETAIL_NSIZE;
		const float fPercent = pThumbFrame->GetPercent();
		const DWORD dwPrevBegineIndex = m_nCurrentBeginIndex;
		if ( nMovableLine > 0 )
		{
			m_nCurrentBeginIndex =  (DWORD)floor(fPercent * float(nMovableLine));
			m_pScrollBar->SetVisibleSingle(TRUE);
		}
		else
		{
			m_nCurrentBeginIndex = 0;
			m_pScrollBar->SetVisibleSingle(FALSE);
		}		
	}
	
	{ // Update Detail Slots;
		DamageRecord::DetailElement Element[DETAIL_NSIZE];
		pElement->getDetailElement(Element, DETAIL_NSIZE, m_nCurrentBeginIndex);
		for ( DWORD _i = DETAIL_NSIZE; _i; --_i )
			m_pDetailSlot[_i]->SetSlotInformation(&Element[_i - 1], pElement->m_nAccumDamage);
	}
}
