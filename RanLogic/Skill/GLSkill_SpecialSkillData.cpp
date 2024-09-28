
#include "../pch.h"
#include "./GLSkillDef.h"
#include "./GLSkill_SpecialSkillData.h"

namespace SKILL
{
	SCREATURE_LVL::SCREATURE_LVL (void) 
		: sID(NATIVEID_NULL())
		, fSEC_RECALLDELAY(0.0f)
		, fSEC_ACTIVETIME(0.0f)
		, dwMAX(0)
		, wUSESKILLLEVEL(0)
		, dwType(0)
		, bEventSkill(false)
	{
	}
};