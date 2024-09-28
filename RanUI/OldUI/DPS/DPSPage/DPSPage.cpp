#include "StdAfx.h"

#include "./DPSSlot/DPSSlot.h"
#include "DPSPage.h"

DPSPage::DPSPage(EngineDeviceMan* pEngineDevice, GLGaeaClient* pGaeaClient)
: CUIGroupHelper(pEngineDevice)
, m_pGaeaClient(pGaeaClient)
, m_nSelectIndex(DamageRecord::VIEW_SLOT_NSIZE)
{
}

void DPSPage::ResetSelectIndex(void)
{
	m_nSelectIndex = DamageRecord::VIEW_SLOT_NSIZE;
}

const bool DPSPage::isSelected(void) const
{
	return m_nSelectIndex != DamageRecord::VIEW_SLOT_NSIZE;
}
const DWORD DPSPage::GetSelectIndex(void) const
{
	return m_nSelectIndex;
}

void DPSPage::CreateSubControl(void)
{
	for ( DWORD _i = DamageRecord::VIEW_SLOT_NSIZE + 1; _i; --_i )
	{
		const DWORD nIndex = _i - 1;
		DPSSlot* pDPSSlot = new DPSSlot(m_pEngineDevice, m_pGaeaClient, nIndex);
		CString strDPSSlot;
		strDPSSlot.Format("DPS_WINDOW_SLOT%d", _i - 1);
		pDPSSlot->CreateSub(this, strDPSSlot.GetString (), UI_FLAG_DEFAULT, DPS_SLOT0 + nIndex);
		pDPSSlot->CreateSubControl(((_i == 1) ? true : false));
		RegisterControl(pDPSSlot);
		m_pDPSSlot[nIndex] = pDPSSlot;
	}
}

void DPSPage::TranslateUIMessage(UIGUID ControlID, DWORD dwMsg)
{
	switch ( ControlID )
	{
	case DPS_SLOT1:
	case DPS_SLOT2:
	case DPS_SLOT3:
	case DPS_SLOT4:
	case DPS_SLOT5:
		if ( CHECK_MOUSEIN_LBUPLIKE(dwMsg) )
		{
			const DWORD nSelectIndex = ControlID - DPS_SLOT1;
			if ( m_nSelectIndex == nSelectIndex )
				m_nSelectIndex = DamageRecord::VIEW_SLOT_NSIZE;
			else
				m_nSelectIndex = ControlID - DPS_SLOT1;
		}
	}
}

void DPSPage::Update(int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl)
{
	CUIGroupHelper::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );	

	for ( DWORD _i = DamageRecord::VIEW_SLOT_NSIZE; _i; --_i )
		m_pDPSSlot[_i]->SetSlotInformation(_i - 1);

	if ( m_nSelectIndex != DamageRecord::VIEW_SLOT_NSIZE )
	{
		if ( m_pDPSSlot[m_nSelectIndex + 1]->SetSelect() == false )
			m_nSelectIndex = DamageRecord::VIEW_SLOT_NSIZE;
	}
}

