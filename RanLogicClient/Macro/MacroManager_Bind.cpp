#include "pch.h"
#include "../../enginelib/GUInterface/GameTextControl.h"
#include "../../RanLogic/GLUseFeatures.h"
#include "../GLGaeaClient.h"
#include "../Char/GLCharacter.h"

#include "./MacroDefine.h"
#include "./BaseMacro.h"
#include "./AutomaticSkillMacro.h"
#include "./ReturnByOptionMacro.h"
#include "./MacroManager.h"

void GLMacroManager::MacroOnOff(bool bOn)
{
	GLMacroManager::GetInstance().AddMessage(MACRO::EMMACRO_MSG_REQSERVER_ONOFF, bOn);
}

void GLMacroManager::SetCheckRemainDrugCount(bool bCheck)
{
	GLMacroManager::GetInstance().AddMessage(MACRO::EMMACRO_MSG_RETURNBYOPTION_CHECKDRUG, bCheck);
}

void GLMacroManager::SetCheckRemainMacroTime(bool bCheck)
{
	GLMacroManager::GetInstance().AddMessage(MACRO::EMMACRO_MSG_RETURNBYOPTION_CHECKTIME, bCheck);
}

bool GLMacroManager::IsCheckRemainDrugCount(void)
{
	ReturnByOptionMacro* pMacro = static_cast<ReturnByOptionMacro*>(m_vecMacroModel[MACRO::MACRO_RETURNBYOPTION]);
	return pMacro->IsCheckDrug();
}

bool GLMacroManager::IsCheckRemainMacroTime(void)
{
	ReturnByOptionMacro* pMacro = static_cast<ReturnByOptionMacro*>(m_vecMacroModel[MACRO::MACRO_RETURNBYOPTION]);
	return pMacro->IsCheckTime();
}

void GLMacroManager::SetSkillTabEnable(WORD wSlotNumber, bool bEnable)
{
	GLMacroManager::GetInstance().AddMessage(bEnable?MACRO::EMMACRO_MSG_AUTOMATICSKILLTAB_ENABLE:MACRO::EMMACRO_MSG_AUTOMATICSKILLTAB_DISABLE, wSlotNumber);
}

bool GLMacroManager::IsSkillTabEnable(WORD wSlotNumber)
{
	AutomaticSkillMacro* pMacro = static_cast<AutomaticSkillMacro*>(m_vecMacroModel[MACRO::MACRO_AUTOMATICSKILLUSE]);
	return pMacro->IsSkillTabEnable(wSlotNumber);
}

LuaTable GLMacroManager::GetAllOptionState(void)
{
	LuaTable tbOption(GLWidgetScript::GetInstance().GetLuaState());
	tbOption.set("bSkillTrayF1", IsSkillTabEnable(0));
	tbOption.set("bSkillTrayF2", IsSkillTabEnable(1));
	tbOption.set("bSkillTrayF3", IsSkillTabEnable(2));
	tbOption.set("bSkillTrayF4", IsSkillTabEnable(3));
	tbOption.set("bRemainDrug", IsCheckRemainDrugCount());
	tbOption.set("bRemainMacro", IsCheckRemainMacroTime());
	tbOption.set("fRemainTime", m_pCharacter->GetRemainMacroTime());
	tbOption.set("bActive", IsActive());
	return tbOption;
}