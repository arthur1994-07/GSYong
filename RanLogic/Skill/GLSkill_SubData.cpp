#include "../pch.h"
#include "../../SigmaCore/String/StringUtils.h"
#include "./GLSkillDef.h"
#include "./GLSkill_SubData.h"

namespace SKILL
{
	SSPEC_100::SSPEC_100 () 
		: fVAR1(0.0f)
		, fVAR2(0.0f)
		, dwFLAG(NULL)
	{
	}

	SSPEC_101::SSPEC_101 () 
		: fVAR1(0.0f)
		, fVAR2(0.0f)
		, dwFLAG(NULL)
		, dwNativeID(NATIVEID_NULL())
	{
	}

	void SSPEC_101::Assign( SSPEC_100 &sOldData )
	{
		fVAR1 = sOldData.fVAR1;
		fVAR2 = sOldData.fVAR2;
		dwFLAG = sOldData.dwFLAG;
	}

	SSPEC_102::SSPEC_102 () 
		: fVAR1(0.0f)
		, fVAR2(0.0f)
		, fRate(1.0f)
		, fRate2(1.0f)
		, dwFLAG(NULL)
		, dwNativeID(NATIVEID_NULL())
	{
	}

	SSPEC::SSPEC () 
		: fVAR1(0.0f)
		, fVAR2(0.0f)
		, fRate(1.0f)
		, fRate2(1.0f)
		, dwFLAG(NULL)
		, dwNativeID(NATIVEID_NULL())
		, dwLinkID(NATIVEID_NULL())
	{
	}

	void SSPEC::Assign( SSPEC_100 &sOldData )
	{
		fVAR1 = sOldData.fVAR1;
		fVAR2 = sOldData.fVAR2;
		fRate = 1.0f; // 기본 비율은 1.0f이다.
		fRate2 = 1.0f; // 기본 비율은 1.0f이다.
		dwFLAG = sOldData.dwFLAG;
	}

	void SSPEC::Assign( SSPEC_101 &sOldData )
	{
		fVAR1 = sOldData.fVAR1;
		fVAR2 = sOldData.fVAR2;
		fRate = 1.0f; // 기본 비율은 1.0f이다.
		fRate2 = 1.0f; // 기본 비율은 1.0f이다.
		dwFLAG = sOldData.dwFLAG;
	}

	void SSPEC::Assign( SSPEC_102 &sOldData )
	{
		fVAR1 = sOldData.fVAR1;
		fVAR2 = sOldData.fVAR2;
		fRate = 1.0f; // 기본 비율은 1.0f이다.
		fRate2 = 1.0f; // 기본 비율은 1.0f이다.
		dwFLAG = sOldData.dwFLAG;
	}


	SIMPACT_ADDON_LEVEL::SIMPACT_ADDON_LEVEL()
		: emADDON ( EMIMPACTA_NONE )
		, fADDON_VAR( 0.0f )
		, fRate(1.0f) // 기본 비율은 1.0f이다.
	{
	}

	SSPEC_ADDON_LEVEL::SSPEC_ADDON_LEVEL()
		: emSPEC ( EMSPECA_NULL )
	{
	}

	SSPEC_ADDON_LEVEL::SSPEC_ADDON_LEVEL(const EMSPEC_ADDON eAddon ) : emSPEC(eAddon)
	{
	}

	bool SSPEC_ADDON_LEVEL::operator()(SSPEC_ADDON_LEVEL& key) const
	{
		if ( emSPEC == key.emSPEC )	return true;
		return false;
	}

	SKILLDELAY::SKILLDELAY()
		: fDelayTime(0)
		, fMAXDelayTime(0)
		, fDefultMaxDelayTime(0)
	{
	}
};