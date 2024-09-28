#include "StdAfx.h"

#include "../../../../../RanLogicClient/DamageRecord/DamageRecord.h"
#include "../../../../../RanLogicClient/GLGaeaClient.h"
#include "../../../Util/RnButton.h"
#include "DPSSlot.h"

DPSSlot::DPSSlot(EngineDeviceMan* pEngineDevice, GLGaeaClient* pGaeaClient, const DWORD nSlotIndex)
: CUIGroupHelper(pEngineDevice)
, m_pGaeaClient(pGaeaClient)
, m_nSlotIndex(nSlotIndex)
{
}

DPSSlot::~DPSSlot(void)
{
}

void DPSSlot::CreateSubControl(const bool bTitle /* = false */)
{
	CreateLineBox("DPS_SLOT_BOX", "PARTY_INFO_LINEBOX_TEXTUREINFO");

	m_pName = CreateText("name", "DPS_SLOT_NAME", TEXT_ALIGN_CENTER_X|TEXT_ALIGN_CENTER_Y);
	m_pDamage = CreateText("damage", "DPS_SLOT_DAMAGE", TEXT_ALIGN_CENTER_X|TEXT_ALIGN_CENTER_Y);
	m_pTime = CreateText("time", "DPS_SLOT_TIME", TEXT_ALIGN_CENTER_X|TEXT_ALIGN_CENTER_Y);
	m_pDPS = CreateText("DPS", "DPS_SLOT_DPS", TEXT_ALIGN_CENTER_X|TEXT_ALIGN_CENTER_Y);
	m_pSkillID = CreateText("SkillID", "DPS_SLOT_SKILL", TEXT_ALIGN_CENTER_X|TEXT_ALIGN_CENTER_Y);

	if ( bTitle == true )
		m_pResetButton = NULL;
	else
	{
		RnButton::CreateArg Arg;
		Arg.defaultTextureKey = "PARTY_INFO_BUTTON_NORMAL";
		Arg.mouseOverTextureKey = "PARTY_INFO_BUTTON_OVER";
		Arg.mouseClickTextureKey = "PARTY_INFO_BUTTON_CLICK";

		Arg.text = "X";
		m_pResetButton = new RnButton(m_pEngineDevice);
		m_pResetButton->CreateSub(this,"DPS_SLOT_RESET_BUTTON", UI_FLAG_DEFAULT, DPS_SLOT_RESET_BUTTON);
		m_pResetButton->CreateSubControl(Arg);
		RegisterControl(m_pResetButton);
	}

	m_pSelectBox = CreateLineBox("DPS_SLOT_BOX", "PARTY_INFO_SELECTBOX_TEXTUREINFO");	
	m_pSelectBox->SetVisibleSingle(FALSE);
}

void DPSSlot::TranslateUIMessage(UIGUID ControlID, DWORD dwMsg)
{
	switch ( ControlID )
	{
	case DPS_SLOT_RESET_BUTTON:
		if ( CHECK_MOUSEIN_LBUPLIKE(dwMsg) )
		{
			if ( m_nSlotIndex == 1 )
				m_pGaeaClient->resetActorDamageRecord(m_nSlotIndex - 1);
			else
				m_pGaeaClient->dropActorDamageRecord(m_nSlotIndex - 1);
			m_pSelectBox->SetVisibleSingle(FALSE);
			SetVisibleSingle(FALSE);
		}
		break;
	}	
}

void DPSSlot::SetSlotInformation(const DWORD nIndex)
{
	const DamageRecord::Manager* const pDamageRecord = m_pGaeaClient->GetDamageRecord();
	const DamageRecord::ViewElement* const pElement = pDamageRecord->GetViewElement(nIndex);

	m_pSelectBox->SetVisibleSingle(FALSE);
	if ( pElement == NULL )
	{
		SetVisibleSingle(FALSE);
		return;
	}	

	m_pName->SetText(pElement->m_szName);

	CString strDamage;
	strDamage.Format("%d", pElement->m_nAccumDamage);
	m_pDamage->SetText(strDamage);

	const float fTime = pElement->m_Calculator.m_fElpasedTime;
	const DWORD nMinute = DWORD(fTime * (1.0f / 60.0f));
	const DWORD nSecond = DWORD(fTime - float(nMinute * 60));
	CString strTime;
	strTime.Format("%d:%02d", nMinute, nSecond);	
	m_pTime->SetText(strTime);

	CString strDPS;
	strDPS.Format("%.02f", pElement->m_Calculator.m_fDPS);
	m_pDPS->SetText(strDPS);	

	CString strSkill;
	if ( pElement->m_SkillID == SNATIVEID(false) )
		strSkill.Format("--/--(-)");
	else
		strSkill.Format("%d/%d(%d)", pElement->m_SkillID.Mid(), pElement->m_SkillID.Sid(), pElement->m_wSkillLevel + 1);
	m_pSkillID->SetText(strSkill);

	SetVisibleSingle(TRUE);
}

const bool DPSSlot::SetSelect(void)
{
	if ( IsVisible() == FALSE )
	{
		m_pSelectBox->SetVisibleSingle(FALSE);
		return false;
	}
	m_pSelectBox->SetVisibleSingle(TRUE);
	return true;
}

