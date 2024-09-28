#include "StdAfx.h"

#include "../../../../../EngineLib/GUInterface/BasicTextBox.h"
#include "ExpeditionInviteSlot.h"

ExpeditionInviteSlot::ExpeditionInviteSlot(EngineDeviceMan* pEngineDevice, GLGaeaClient* pGaeaClient)
: CUIGroupHelper(pEngineDevice)
, m_dwDbNum(DBNUM_NULL)
{
}
ExpeditionInviteSlot::~ExpeditionInviteSlot(void)
{
}
void ExpeditionInviteSlot::CreateSubControl(void)
{
	m_pSelectBox = CreateLineBox("EXPEDITION_INVITE_LIST_SLOT", "PARTY_INFO_SELECTBOX_TEXTUREINFO");
	m_pSelectBox->SetVisibleSingle(FALSE);
	m_pCharacterName = CreateText("NAME", "EXPEDITION_INVITE_LIST_SLOT", TEXT_ALIGN_CENTER_X|TEXT_ALIGN_CENTER_Y);	
}

void ExpeditionInviteSlot::SetSlot(const char* const szName, const DWORD dwDbNum, const bool bSelected)
{
	m_pCharacterName->SetText(szName);
	m_pSelectBox->SetVisibleSingle(bSelected ? TRUE : FALSE);
	m_dwDbNum = dwDbNum;
}

void ExpeditionInviteSlot::Refresh(const bool bSelected)
{
	m_pSelectBox->SetVisibleSingle(bSelected ? TRUE : FALSE);
}