#pragma once
#include  "../RanLogic/Character/GLCharData.h"
#include "../enginelib/GUInterface/GameTextControl.h"
#include "GLogicData.h"
#include "SKILL/GLSkill.h"
#include "GLogicEx.h"



namespace GLSkillToolTip
{
    enum SKILL_INFOR_TYPE
    {
        INFOR_NONE_TYPE,

        NORMAL_DURATION_PLUS,
        NORMAL_DURATION_MINUS,

        NORMAL_NONDURATION_PLUS,
        NORMAL_NONDURATION_MINUS,

        PASSIVE_DURATION_PLUS,
        PASSIVE_DURATION_MINUS,

        PASSIVE_NONDURATION_PLUS,
        PASSIVE_NONDURATION_MINUS,
    };

	std::vector<std::string> MakeTooltipSting_In_Skill_Special( const GLSKILL* glSkill, WORD wLevel, bool bIsTool = false );
    std::vector<std::string> MakeTooltipSting_In_Skill_Impact(const GLSKILL* glSkill, WORD wLevel );
	std::vector<std::string> MakeTooltipSting_In_Skill_Link(const GLSKILL* glSkill, WORD wLevel );
    std::string MakeTooltipSting_In_Skill_Type( GLCHARLOGIC* pTarget, const GLSKILL* glSkill, WORD wLevel );

    std::string MakeTooltipSting_In_Skill_Special( 
        float fLife, 
        float fVAR1_SCALE, 
        float fVAR2_SCALE,
        EMSPEC_ADDON emSPEC,
        const SKILL::SSPEC& sSPEC,
        EMIMPACT_TARGET emIMPACT_TAR,
		bool bIsTool,
		const SKILL::SSPEC_CA& sSPECCA  = SKILL::SSPEC_CA());
    std::string MakeTooltipSting_In_Skill_Impact(
        float fLife,
        const float fADDON_VAR, 
        float fADDON_VAR_SCALE, 
        SKILL::EMROLE emRole, 
        EMIMPACT_ADDON emADDON );


	// 등급/속성;
	bool MakeTooltipString_In_Skill_Grade (
		std::string& strText,
		GLSKILL* pSkill );

	// 적용 타입;
	bool MakeTooltipString_In_Skill_ApplyType (
		std::string& strText,
		GLSKILL* pSkill );

	// 사용 무기;
	bool MakeTooltipString_In_Skill_UseWeapon (
		std::string& strText,
		GLSKILL* pSkill );

	// 소모 조건;
	bool MakeTooltipString_In_Skill_Requirement (
		std::string& strText,
		GLSKILL* pSkill,
		WORD wLevel );

	// 재사용 시간;
	bool MakeTooltipString_In_Skill_ReuseTime (
		std::string& strText,
		GLSKILL* pSkill,
		WORD wLevel );

	// 스킬 영역;
	bool MakeTooltipString_In_Skill_SkillScope (
		std::string& strText,
		GLSKILL* pSkill,
		WORD wLevel,
		GLCHARLOGIC* pTarget );

	// 스킬 효과;
	bool MakeTooltipString_In_Skill_SkillEffect (
		std::string& strText,
		GLSKILL* pSkill,
		WORD wLevel,
		GLCHARLOGIC* pTarget );

	// 상태 이상;
	bool MakeTooltipString_In_Skill_Blow (
		std::string& strText,
		GLSKILL* pSkill,
		WORD wLevel );

}
