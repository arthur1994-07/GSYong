#include "StdAfx.h"

#include "../../../../../Skill/SkillImage.h"
#include "SkillEffectSlot.h"

SkillEffectSlot::SkillEffectSlot(EngineDeviceMan* pEngineDevice, GLGaeaClient* pGaeaClient)
: CUIGroupHelper(pEngineDevice)
, m_pGaeaClient(pGaeaClient)
, m_SkillID(SNATIVEID(false))
{
}

void SkillEffectSlot::CreateSubControl(const bool bSmall)
{
	m_pSkillImage = new CSkillImage(m_pGaeaClient, m_pEngineDevice);
	m_pSkillImage->CreateSub(this, bSmall ? "SKILLEFFECT_SLOT_SMALL" : "SKILLEFFECT_SLOT", UI_FLAG_DEFAULT, SKILLEFFECT_IMAGE);
	m_pSkillImage->CreateSubControl();
	m_pSkillImage->SetUseRender(TRUE);
	RegisterControl(m_pSkillImage);	
}

const bool SkillEffectSlot::SetSkillImage(const SNATIVEID& SkillID)
{
	if ( SkillID == SNATIVEID(false) )
	{
		SetVisibleSingle(FALSE);
		return false;
	}	

	SetVisibleSingle(TRUE);
	m_SkillID = SkillID;
	m_pSkillImage->SetSkill(SkillID);		

	return true;
}