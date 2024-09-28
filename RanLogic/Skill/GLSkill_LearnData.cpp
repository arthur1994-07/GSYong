#include "../pch.h"
#include "../../SigmaCore/String/StringUtils.h"
#include "./GLSkillDef.h"
#include "./GLSkill_LearnData.h"

namespace SKILL
{
	SLEARN_LVL_101::SLEARN_LVL_101 (void) 
		: dwSKP(1)
		, dwLEVEL(1)
		, dwSKILL_LVL(0)
	{
	}

	SLEARN_LVL_102::SLEARN_LVL_102 (void) 
		: dwSKP(1)
		, dwLEVEL(1)
		, dwSKILL_LVL(0)
	{
	}

	SLEARN_LVL_103::SLEARN_LVL_103 (void) 
		: dwSKP(1)
		, dwLEVEL(1)
		, dwSKILL_LVL(0)
	{
	}

	SLEARN_LVL_104::SLEARN_LVL_104 (void) 
		: dwSKP(1)
		, dwLEVEL(1)
		, dwSKILL_LVL(0)
		, dwReqMoney(0)
	{
	}

	SLEARN_LVL::SLEARN_LVL (void)
	{
	}

	void SLEARN_LVL::Assign ( SLEARN_LVL_101& sOldData )
	{
		dwLEVEL = sOldData.dwLEVEL;
		sSTATS = sOldData.sSTATS;
		dwSKP = sOldData.dwSKP;
		dwSKILL_LVL = sOldData.dwSKILL_LVL;
	}

	void SLEARN_LVL::Assign ( SLEARN_LVL_102& sOldData )
	{
		dwLEVEL = sOldData.dwLEVEL;
		sSTATS = sOldData.sSTATS;
		dwSKP = sOldData.dwSKP;
		dwSKILL_LVL = sOldData.dwSKILL_LVL;
	}

	void SLEARN_LVL::Assign ( SLEARN_LVL_103& sOldData )
	{
		dwLEVEL = sOldData.dwLEVEL;
		sSTATS = sOldData.sSTATS;
		dwSKP = sOldData.dwSKP;
		dwSKILL_LVL = sOldData.dwSKILL_LVL;
	}
};