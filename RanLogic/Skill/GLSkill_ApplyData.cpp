#include "../pch.h"
#include "./GLSkillDef.h"
#include "./GLSkill_ApplyData.h"

namespace SKILL
{
	SSTATE_BLOW::SSTATE_BLOW () 
		: fRATE(0.0f)
		, fVAR1(0.0f)
		, fVAR2(0.0f)
	{
	}

	SIMPACT_ADDON_100::SIMPACT_ADDON_100()
		: emADDON ( EMIMPACTA_NONE )
	{
		for ( int i=0; i<MAX_LEVEL_BEFORE; ++i )
		{
			fADDON_VAR[i] = 0.0f;
		}
	}

	SIMPACT_ADDON_101::SIMPACT_ADDON_101()
		: emADDON ( EMIMPACTA_NONE )
	{
		for ( int i=0; i<MAX_LEVEL_BEFORE; ++i )
		{
			fADDON_VAR[i] = 0.0f;
			fRate[i] = 1.0f;
		}
	}

	CDATA_LVL::CDATA_LVL (void) 
		: fDELAYTIME(0.0f)
		, fLIFE(0.0f)
		, wAPPLYRANGE(0)
		, wAPPLYNUM(1)
		, wAPPLYANGLE(0)
		, wPIERCENUM(1)
		, wTARNUM(1)
		, fBASIC_VAR(0.0f)
		, wUSE_ARROWNUM(0)
		, wUSE_CHARMNUM(0)
		, wUSE_BULLETNUM(0)
		, wUSE_EXP(0)
		, wUSE_HP(0)
		, wUSE_MP(0)
		, wUSE_SP(0)
		, wUSE_CP(0)
		, nUSE_BATTERY(0)
		, wUSE_HP_PTY(0)
		, wUSE_MP_PTY(0)
		, wUSE_SP_PTY(0)
	{
	}

	void CDATA_LVL::Assign ( CDATA_LVL_103 &sOldData )
	{
		fDELAYTIME		= sOldData.fDELAYTIME;

		fLIFE			= sOldData.fLIFE;
		wAPPLYRANGE		= sOldData.wAPPLYRANGE;
		wAPPLYNUM		= sOldData.wAPPLYNUM;
		wAPPLYANGLE		= sOldData.wAPPLYANGLE;
		wPIERCENUM		= sOldData.wPIERCENUM;
		wTARNUM			= sOldData.wTARNUM;

		fBASIC_VAR		= sOldData.fBASIC_VAR;

		wUSE_ARROWNUM	= sOldData.wUSE_ARROWNUM;
		wUSE_CHARMNUM	= sOldData.wUSE_CHARMNUM;
		wUSE_BULLETNUM	= sOldData.wUSE_BULLETNUM;

		wUSE_EXP		= wUSE_EXP;
		wUSE_HP			= sOldData.wUSE_HP;
		wUSE_MP			= sOldData.wUSE_MP;
		wUSE_SP			= sOldData.wUSE_SP;
		wUSE_CP			= sOldData.wUSE_CP;

		wUSE_HP_PTY		= sOldData.wUSE_HP_PTY;
		wUSE_MP_PTY		= sOldData.wUSE_MP_PTY;
		wUSE_SP_PTY		= sOldData.wUSE_SP_PTY;
	}

	void CDATA_LVL::Assign ( CDATA_LVL_102 &sOldData )
	{
		fDELAYTIME		= sOldData.fDELAYTIME;

		fLIFE			= sOldData.fLIFE;
		wAPPLYRANGE		= sOldData.wAPPLYRANGE;
		wAPPLYNUM		= sOldData.wAPPLYNUM;
		wAPPLYANGLE		= sOldData.wAPPLYANGLE;
		wPIERCENUM		= sOldData.wPIERCENUM;
		wTARNUM			= sOldData.wTARNUM;

		fBASIC_VAR		= sOldData.fBASIC_VAR;

		wUSE_ARROWNUM	= sOldData.wUSE_ARROWNUM;
		wUSE_CHARMNUM	= sOldData.wUSE_CHARMNUM;

		wUSE_EXP		= wUSE_EXP;
		wUSE_HP			= sOldData.wUSE_HP;
		wUSE_MP			= sOldData.wUSE_MP;
		wUSE_SP			= sOldData.wUSE_SP;
		wUSE_CP			= sOldData.wUSE_CP;

		wUSE_HP_PTY		= sOldData.wUSE_HP_PTY;
		wUSE_MP_PTY		= sOldData.wUSE_MP_PTY;
		wUSE_SP_PTY		= sOldData.wUSE_SP_PTY;
	}

	void CDATA_LVL::Assign ( CDATA_LVL_101 &sOldData )
	{
		fDELAYTIME		= sOldData.fDELAYTIME;

		fLIFE			= sOldData.fLIFE;
		wAPPLYRANGE		= sOldData.wAPPLYRANGE;
		wAPPLYNUM		= sOldData.wAPPLYNUM;
		wAPPLYANGLE		= sOldData.wAPPLYANGLE;
		wPIERCENUM		= sOldData.wPIERCENUM;
		wTARNUM			= sOldData.wTARNUM;

		fBASIC_VAR		= sOldData.fBASIC_VAR;

		wUSE_ARROWNUM	= sOldData.wUSE_ARROWNUM;
		wUSE_CHARMNUM	= sOldData.wUSE_CHARMNUM;

		wUSE_EXP		= wUSE_EXP;
		wUSE_HP			= sOldData.wUSE_HP;
		wUSE_MP			= sOldData.wUSE_MP;
		wUSE_SP			= sOldData.wUSE_SP;
		wUSE_CP			= 0;

		wUSE_HP_PTY		= sOldData.wUSE_HP_PTY;
		wUSE_MP_PTY		= sOldData.wUSE_MP_PTY;
		wUSE_SP_PTY		= sOldData.wUSE_SP_PTY;
	}

	void CDATA_LVL::Assign ( CDATA_LVL_100 &sOldData )
	{
		fDELAYTIME		= sOldData.fDELAYTIME;

		fLIFE			= sOldData.fLIFE;
		wAPPLYRANGE		= sOldData.wAPPLYRANGE;
		wAPPLYNUM		= sOldData.wAPPLYNUM;
		wAPPLYANGLE		= sOldData.wAPPLYANGLE;
		wPIERCENUM		= sOldData.wPIERCENUM;
		wTARNUM			= 1;

		fBASIC_VAR		= sOldData.fBASIC_VAR;

		wUSE_ARROWNUM	= sOldData.wUSE_ARROWNUM;
		wUSE_CHARMNUM	= 0;

		wUSE_EXP		= 0;
		wUSE_HP			= sOldData.wUSE_HP;
		wUSE_MP			= sOldData.wUSE_MP;
		wUSE_SP			= sOldData.wUSE_SP;
		wUSE_CP			= 0;

		wUSE_HP_PTY		= sOldData.wUSE_HP_PTY;
		wUSE_MP_PTY		= sOldData.wUSE_MP_PTY;
		wUSE_SP_PTY		= sOldData.wUSE_SP_PTY;
	}

	SIMPACT_ADDON::SIMPACT_ADDON(const EMIMPACT_ADDON eAddon) : emADDON(eAddon) {}

	SIMPACT_ADDON::SIMPACT_ADDON()
		: emADDON ( EMIMPACTA_NONE )
	{
		for ( int i=0; i<MAX_LEVEL; ++i )
		{
			fADDON_VAR[i] = 0.0f;
			fRate[ i ] = 1.0f; // 기본 비율은 1.0f이다.
		}
	}

	void SIMPACT_ADDON::Assign( SIMPACT_ADDON_100& sOldData )
	{
		emADDON = sOldData.emADDON;
		for ( int i=0; i<MAX_LEVEL_BEFORE; ++i )
		{
			fADDON_VAR[i] = sOldData.fADDON_VAR[ i ];
			fRate[ i ] = 1.0f; // 기본 비율은 1.0f이다.
		}

		for ( int i = MAX_LEVEL_BEFORE; i < MAX_LEVEL; ++i )
		{
			fADDON_VAR[i] = 0.0f;
			fRate[ i ] = 1.0f; // 기본 비율은 1.0f이다;
		}
	}

	void SIMPACT_ADDON::Assign( SIMPACT_ADDON_101& sOldData )
	{
		emADDON = sOldData.emADDON;
		for ( int i=0; i<MAX_LEVEL_BEFORE; ++i )
		{
			fADDON_VAR[i] = sOldData.fADDON_VAR[ i ];
			fRate[ i ] = sOldData.fRate[ i ]; // 기본 비율은 1.0f이다;
		}

		for ( int i = MAX_LEVEL_BEFORE; i < MAX_LEVEL; ++i )
		{
			fADDON_VAR[i] = 0.0f;
			fRate[ i ] = 1.0f; // 기본 비율은 1.0f이다;
		}
	}

	bool SIMPACT_ADDON::operator()(SIMPACT_ADDON& key)		const
	{
		if ( emADDON == key.emADDON )	return true;
		return false;
	}

	SSPEC_ADDON::SSPEC_ADDON()
		: emSPEC ( EMSPECA_NULL )
	{
	}

	SSPEC_ADDON::SSPEC_ADDON(const EMSPEC_ADDON eAddon) : emSPEC(eAddon) {}

	void SSPEC_ADDON::Assign( SSPEC_ADDON_100& sOldData )
	{
		emSPEC = sOldData.emSPEC;
		for ( int i=0; i<MAX_LEVEL_BEFORE; ++i )
		{
			sSPEC[ i ].Assign( sOldData.sSPEC[ i ] );
		}
	}

	void SSPEC_ADDON::Assign( SSPEC_ADDON_101& sOldData )
	{
		emSPEC = sOldData.emSPEC;
		for ( int i=0; i<MAX_LEVEL_BEFORE; ++i )
		{
			sSPEC[ i ].Assign( sOldData.sSPEC[ i ] );
		}
	}

	void SSPEC_ADDON::Assign( SSPEC_ADDON_102& sOldData )
	{
		emSPEC = sOldData.emSPEC;
		for ( int i=0; i<MAX_LEVEL_BEFORE; ++i )
		{
			sSPEC[ i ] = sOldData.sSPEC[ i ];
		}
	}

	bool SSPEC_ADDON::operator()(SSPEC_ADDON& key)		const
	{
		if ( emSPEC == key.emSPEC )	return true;
		return false;
	}

	void SSPEC_CA_ADDON::Assign ( SSPEC_CA_ADDON_100 &sOldData )
	{
		resetemSpec();
		AllReset();
		for ( int i=0; i<MAX_LEVEL_BEFORE; ++i )
			sSPEC_CA[ i ] =  sOldData.sSPEC_CA[ i ];
	}

	void SSPEC_CA_ADDON::resetemSpec() {
		for(int i=0; i<MAX_LEVEL; ++i)
		{
			sSPEC_CA[i].resetemSpec();
		}
	}

	void SSPEC_CA_ADDON::AllReset()
	{
		for(int i=0; i<MAX_LEVEL; ++i)
		{
			sSPEC_CA[i].AllReset();
		}
	}

	SSPEC_CA::SSPEC_CA() : bDamageApply( false ), bAllApply( false ) {
		resetemSpec();
	}

	void SSPEC_CA::resetemSpec() {
		for( int i = 0; i < MAX_ADDON; ++i )
		{
			emSPEC[i] = EMSPECA_NULL;
		}
	}

	void SSPEC_CA::AllReset()	{

		bAllApply    = false;
		bDamageApply = false;

		for( int i = 0; i < MAX_ADDON; ++i )
		{
			emSPEC[i] = EMSPECA_NULL;
		}
	}
};