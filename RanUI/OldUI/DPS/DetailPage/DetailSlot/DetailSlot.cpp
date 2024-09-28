#include "StdAfx.h"

#include "../../../../../RanLogic/Skill/GLSkill.h"
#include "../../../../../RanLogicClient/DamageRecord/DamageRecord.h"
#include "../../../../../RanLogicClient/GLGaeaClient.h"
#include "../../../Util/RnButton.h"
#include "DetailSlot.h"

DetailSlot::DetailSlot(EngineDeviceMan* pEngineDevice, GLGaeaClient* pGaeaClient, const DWORD nSlotIndex)
: CUIGroupHelper(pEngineDevice)
, m_pGaeaClient(pGaeaClient)
, m_nSlotIndex(nSlotIndex)
{
}

DetailSlot::~DetailSlot(void)
{
}

void DetailSlot::CreateSubControl(void)
{
	CreateLineBox("DPS_SLOT_BOX", "PARTY_INFO_LINEBOX_TEXTUREINFO");

	m_pName = CreateText("name", "DPS_SLOT_NAME", TEXT_ALIGN_CENTER_X|TEXT_ALIGN_CENTER_Y);
	m_pDamage = CreateText("damage", "DPS_SLOT_DAMAGE", TEXT_ALIGN_CENTER_X|TEXT_ALIGN_CENTER_Y);
	m_pUseCount = CreateText("count", "DPS_SLOT_TIME", TEXT_ALIGN_CENTER_X|TEXT_ALIGN_CENTER_Y);
	m_pDPS = CreateText("DPS", "DPS_SLOT_DPS", TEXT_ALIGN_CENTER_X|TEXT_ALIGN_CENTER_Y);
	m_pRate = CreateText("rate", "DPS_SLOT_SKILL", TEXT_ALIGN_CENTER_X|TEXT_ALIGN_CENTER_Y);	
}

void DetailSlot::SetSlotInformation(const DamageRecord::DetailElement* const pElement, const DWORD nDamage)
{
	if ( (pElement == NULL) || (pElement->m_nUseCounter == 0) )
	{
		SetVisibleSingle(FALSE);
		return;
	}

	if ( pElement->m_SkillID == SNATIVEID(false) )
		m_pName->SetText("Normal");
	else
		m_pName->SetText(GLSkillMan::GetInstance().GetSkillName(pElement->m_SkillID).c_str());	

	CString strDamage;
	strDamage.Format("%d", pElement->m_nDamage);
	m_pDamage->SetText(strDamage);

	CString strCount;
	strCount.Format("%d", pElement->m_nUseCounter);
	m_pUseCount->SetText(strCount);

	CString strDPS;
	strDPS.Format("%d", pElement->m_nDamage / pElement->m_nUseCounter);
	m_pDPS->SetText(strDPS);

	CString strRate;
	strRate.Format("%3d%%", (pElement->m_nDamage * 100) / (nDamage ? nDamage : 1));
	m_pRate->SetText(strRate);

	SetVisibleSingle(TRUE);
}