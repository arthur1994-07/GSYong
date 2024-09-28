#include "../pch.h"
#include "../../SigmaCore/String/StringUtils.h"
#include "./GLSkillDef.h"
#include "./GLSkill_ExtData.h"

namespace SKILL
{
	SEXT_DATA_112::SEXT_DATA_112 (void) 
		: emANIMTYPE(AN_SKILL)
		, emANISTYPE(AN_SUB_00)
		, emANIVehicleMTYPE( AN_VEHICLE_SKILL_A )
		, fDELAY4DAMAGE(0.0f)
		, emTARG_POSA(EMPOS_GROUND)
		, emTARG_POSB(EMPOS_GROUND)
		, bTARG_ONE(false)
		, emSELFZONE01_POS(EMPOS_GROUND)
		, emSELFZONE02_POS(EMPOS_GROUND)
		, emSELFZONE03_POS(EMPOS_GROUND)
		, emTARGZONE01_POS(EMPOS_GROUND)
		, emTARGZONE02_POS(EMPOS_GROUND)
		, emTARGZONE03_POS(EMPOS_GROUND)
		, emTARG(EMTIME_IMPACT)
		, emSELFZONE01(EMTIME_FIRST)
		, emSELFZONE02(EMTIME_IMPACT)
		, emSELFZONE03(EMTIME_EFFECT)
		, emTARGZONE01(EMTIME_TARG_OVR)
		, emTARGZONE02(EMTIME_PASSIVE)			
		, emTARGZONE03(EMTIME_EFFECT)
		, emSELFBODY(EMTIME_FIRST)
		, emTARGBODY01(EMTIME_TARG_OVR)
		, emTARGBODY02(EMTIME_PASSIVE)
		, nMultiTatgetEffectFrame(0)			
		, sICONINDEX(0,0)
	{
		idMultiTargetSkill = NATIVEID_NULL();
		idDisguiseSkill	   = NATIVEID_NULL();
	}

	void SEXT_DATA_112::Assign ( const SEXT_DATA_100 &OldData )
	{
		emANIMTYPE = OldData.emANIMTYPE;
		emANISTYPE = OldData.emANISTYPE;

		emTARG = OldData.emTARG;
		strTARG[EMELEMENT_SPIRIT] = OldData.szTARG;

		emSELFZONE01 = OldData.emSELFZONE01;
		strSELFZONE01[EMELEMENT_SPIRIT] = OldData.szSELFZONE01;

		emTARGZONE01 = OldData.emTARGZONE01;
		strTARGZONE01[EMELEMENT_SPIRIT] = OldData.szTARGZONE01;

		emTARGZONE02 = OldData.emTARGZONE02;
		strTARGZONE02[EMELEMENT_SPIRIT] = OldData.szTARGZONE02;

		emSELFBODY = OldData.emSELFBODY;
		strSELFBODY[EMELEMENT_SPIRIT] = OldData.szSELFBODY;

		emTARGBODY01 = OldData.emTARGBODY01;
		strTARGBODY01[EMELEMENT_SPIRIT] = OldData.szTARGBODY01;

		emTARGBODY02 = OldData.emTARGBODY02;
		strTARGBODY02[EMELEMENT_SPIRIT] = OldData.szTARGBODY02;

		sICONINDEX = OldData.sICONINDEX;
		strICONFILE = OldData.szICONFILE;

		strCOMMENTS = OldData.szCOMMENTS;
	}

	void SEXT_DATA_112::Assign ( const SEXT_DATA_101 &OldData )
	{
		emANIMTYPE = OldData.emANIMTYPE;
		emANISTYPE = OldData.emANISTYPE;

		emTARG = OldData.emTARG;
		strTARG[EMELEMENT_SPIRIT] = OldData.szTARG;

		emSELFZONE01 = OldData.emSELFZONE01;
		strSELFZONE01[EMELEMENT_SPIRIT] = OldData.szSELFZONE01;

		emSELFZONE02 = OldData.emSELFZONE02;
		strSELFZONE02[EMELEMENT_SPIRIT] = OldData.szSELFZONE02;

		emTARGZONE01 = OldData.emTARGZONE01;
		strTARGZONE01[EMELEMENT_SPIRIT] = OldData.szTARGZONE01;

		emTARGZONE02 = OldData.emTARGZONE02;
		strTARGZONE02[EMELEMENT_SPIRIT] = OldData.szTARGZONE02;

		emSELFBODY = OldData.emSELFBODY;
		strSELFBODY[EMELEMENT_SPIRIT] = OldData.szSELFBODY;

		emTARGBODY01 = OldData.emTARGBODY01;
		strTARGBODY01[EMELEMENT_SPIRIT] = OldData.szTARGBODY01;

		emTARGBODY02 = OldData.emTARGBODY02;
		strTARGBODY02[EMELEMENT_SPIRIT] = OldData.szTARGBODY02;

		sICONINDEX = OldData.sICONINDEX;
		strICONFILE = OldData.szICONFILE;

		strCOMMENTS = OldData.szCOMMENTS;
	}

	void SEXT_DATA_112::Assign ( const SEXT_DATA_102 &OldData )
	{
		emANIMTYPE = OldData.emANIMTYPE;
		emANISTYPE = OldData.emANISTYPE;

		emTARG = OldData.emTARG;
		emTARG_POSA = OldData.emTARG_POSA;
		emTARG_POSB = OldData.emTARG_POSB;
		strTARG[EMELEMENT_SPIRIT] = OldData.szTARG;

		emSELFZONE01 = OldData.emSELFZONE01;
		emSELFZONE01_POS = OldData.emSELFZONE01_POS;
		strSELFZONE01[EMELEMENT_SPIRIT] = OldData.szSELFZONE01;

		emSELFZONE02 = OldData.emSELFZONE02;
		emSELFZONE02_POS = OldData.emSELFZONE02_POS;
		strSELFZONE02[EMELEMENT_SPIRIT] = OldData.szSELFZONE02;

		emTARGZONE01 = OldData.emTARGZONE01;
		emTARGZONE01_POS = OldData.emTARGZONE01_POS;
		strTARGZONE01[EMELEMENT_SPIRIT] = OldData.szTARGZONE01;

		emTARGZONE02 = OldData.emTARGZONE02;
		emTARGZONE02_POS = OldData.emTARGZONE02_POS;
		strTARGZONE02[EMELEMENT_SPIRIT] = OldData.szTARGZONE02;

		emSELFBODY = OldData.emSELFBODY;
		strSELFBODY[EMELEMENT_SPIRIT] = OldData.szSELFBODY;

		emTARGBODY01 = OldData.emTARGBODY01;
		strTARGBODY01[EMELEMENT_SPIRIT] = OldData.szTARGBODY01;

		emTARGBODY02 = OldData.emTARGBODY02;
		strTARGBODY02[EMELEMENT_SPIRIT] = OldData.szTARGBODY02;

		sICONINDEX = OldData.sICONINDEX;
		strICONFILE = OldData.szICONFILE;

		strCOMMENTS = OldData.szCOMMENTS;
	}

	void SEXT_DATA_112::Assign ( const SEXT_DATA_103 &value )
	{
		int i=0;

		emANIMTYPE = value.emANIMTYPE;
		emANISTYPE = value.emANISTYPE;

		emTARG = value.emTARG;
		emTARG_POSA = value.emTARG_POSA;
		emTARG_POSB = value.emTARG_POSB;
		for ( i=0; i<EMELEMENT_OLDMAX; ++i )		strTARG[i] = value.strTARG[i];

		emSELFZONE01 = value.emSELFZONE01;
		emSELFZONE01_POS = value.emSELFZONE01_POS;
		for ( i=0; i<EMELEMENT_OLDMAX; ++i )		strSELFZONE01[i] = value.strSELFZONE01[i];

		emSELFZONE02 = value.emSELFZONE02 ;
		emSELFZONE02_POS = value.emSELFZONE02_POS;
		for ( i=0; i<EMELEMENT_OLDMAX; ++i )		strSELFZONE02[i] = value.strSELFZONE02[i];

		emTARGZONE01 = value.emTARGZONE01;
		emTARGZONE01_POS = value.emTARGZONE01_POS;
		for ( i=0; i<EMELEMENT_OLDMAX; ++i )		strTARGZONE01[i] = value.strTARGZONE01[i];

		emTARGZONE02 = value.emTARGZONE02;
		emTARGZONE02_POS = value.emTARGZONE02_POS;
		for ( i=0; i<EMELEMENT_OLDMAX; ++i )		strTARGZONE02[i] = value.strTARGZONE02[i];

		emSELFBODY = value.emSELFBODY;
		for ( i=0; i<EMELEMENT_OLDMAX; ++i )		strSELFBODY[i] = value.strSELFBODY[i];

		emTARGBODY01 = value.emTARGBODY01;
		for ( i=0; i<EMELEMENT_OLDMAX; ++i )		strTARGBODY01[i] = value.strTARGBODY01[i];

		emTARGBODY02 = value.emTARGBODY02;
		for ( i=0; i<EMELEMENT_OLDMAX; ++i )		strTARGBODY02[i] = value.strTARGBODY02[i];

		strHOLDOUT = value.strHOLDOUT;

		sICONINDEX = value.sICONINDEX;
		strICONFILE = value.strICONFILE;

		strCOMMENTS = value.strCOMMENTS;
	}

	void SEXT_DATA_112::Assign ( const SEXT_DATA_104 &value )
	{
		int i=0;

		emANIMTYPE = value.emANIMTYPE;
		emANISTYPE = value.emANISTYPE;

		fDELAY4DAMAGE = value.fDELAY4DAMAGE;
		emTARG = value.emTARG;
		emTARG_POSA = value.emTARG_POSA;
		emTARG_POSB = value.emTARG_POSB;
		for ( i=0; i<EMELEMENT_OLDMAX; ++i )		strTARG[i] = value.strTARG[i];

		emSELFZONE01 = value.emSELFZONE01;
		emSELFZONE01_POS = value.emSELFZONE01_POS;
		for ( i=0; i<EMELEMENT_OLDMAX; ++i )		strSELFZONE01[i] = value.strSELFZONE01[i];

		emSELFZONE02 = value.emSELFZONE02 ;
		emSELFZONE02_POS = value.emSELFZONE02_POS;
		for ( i=0; i<EMELEMENT_OLDMAX; ++i )		strSELFZONE02[i] = value.strSELFZONE02[i];

		emTARGZONE01 = value.emTARGZONE01;
		emTARGZONE01_POS = value.emTARGZONE01_POS;
		for ( i=0; i<EMELEMENT_OLDMAX; ++i )		strTARGZONE01[i] = value.strTARGZONE01[i];

		emTARGZONE02 = value.emTARGZONE02;
		emTARGZONE02_POS = value.emTARGZONE02_POS;
		for ( i=0; i<EMELEMENT_OLDMAX; ++i )		strTARGZONE02[i] = value.strTARGZONE02[i];

		emSELFBODY = value.emSELFBODY;
		for ( i=0; i<EMELEMENT_OLDMAX; ++i )		strSELFBODY[i] = value.strSELFBODY[i];

		emTARGBODY01 = value.emTARGBODY01;
		for ( i=0; i<EMELEMENT_OLDMAX; ++i )		strTARGBODY01[i] = value.strTARGBODY01[i];

		emTARGBODY02 = value.emTARGBODY02;
		for ( i=0; i<EMELEMENT_OLDMAX; ++i )		strTARGBODY02[i] = value.strTARGBODY02[i];

		strHOLDOUT = value.strHOLDOUT;

		sICONINDEX = value.sICONINDEX;
		strICONFILE = value.strICONFILE;

		strCOMMENTS = value.strCOMMENTS;
	}

	void SEXT_DATA_112::Assign ( const SEXT_DATA_105 &value )
	{
		int i=0;

		emANIMTYPE = value.emANIMTYPE;
		emANISTYPE = value.emANISTYPE;

		fDELAY4DAMAGE = value.fDELAY4DAMAGE;
		emTARG = value.emTARG;
		emTARG_POSA = value.emTARG_POSA;
		emTARG_POSB = value.emTARG_POSB;
		for ( i=0; i<EMELEMENT_OLDMAX; ++i )		strTARG[i] = value.strTARG[i];

		emSELFZONE01 = value.emSELFZONE01;
		emSELFZONE01_POS = value.emSELFZONE01_POS;
		for ( i=0; i<EMELEMENT_OLDMAX; ++i )		strSELFZONE01[i] = value.strSELFZONE01[i];

		emSELFZONE02 = value.emSELFZONE02 ;
		emSELFZONE02_POS = value.emSELFZONE02_POS;
		for ( i=0; i<EMELEMENT_OLDMAX; ++i )		strSELFZONE02[i] = value.strSELFZONE02[i];

		emTARGZONE01 = value.emTARGZONE01;
		emTARGZONE01_POS = value.emTARGZONE01_POS;
		for ( i=0; i<EMELEMENT_OLDMAX; ++i )		strTARGZONE01[i] = value.strTARGZONE01[i];

		emTARGZONE02 = value.emTARGZONE02;
		emTARGZONE02_POS = value.emTARGZONE02_POS;
		for ( i=0; i<EMELEMENT_OLDMAX; ++i )		strTARGZONE02[i] = value.strTARGZONE02[i];

		emSELFBODY = value.emSELFBODY;
		for ( i=0; i<EMELEMENT_OLDMAX; ++i )		strSELFBODY[i] = value.strSELFBODY[i];

		emTARGBODY01 = value.emTARGBODY01;
		for ( i=0; i<EMELEMENT_OLDMAX; ++i )		strTARGBODY01[i] = value.strTARGBODY01[i];

		emTARGBODY02 = value.emTARGBODY02;
		for ( i=0; i<EMELEMENT_OLDMAX; ++i )		strTARGBODY02[i] = value.strTARGBODY02[i];

		strHOLDOUT = value.strHOLDOUT;

		sICONINDEX = value.sICONINDEX;
		strICONFILE = value.strICONFILE;

		strCOMMENTS = value.strCOMMENTS;
	}

	void SEXT_DATA_112::Assign ( const SEXT_DATA_106 &value )
	{
		int i=0;

		emANIMTYPE = value.emANIMTYPE;
		emANISTYPE = value.emANISTYPE;

		fDELAY4DAMAGE = value.fDELAY4DAMAGE;
		emTARG = value.emTARG;
		emTARG_POSA = value.emTARG_POSA;
		emTARG_POSB = value.emTARG_POSB;
		for ( i=0; i<EMELEMENT_MAXNUM; ++i )		strTARG[i] = value.strTARG[i];

		emSELFZONE01 = value.emSELFZONE01;
		emSELFZONE01_POS = value.emSELFZONE01_POS;
		for ( i=0; i<EMELEMENT_MAXNUM; ++i )		strSELFZONE01[i] = value.strSELFZONE01[i];

		emSELFZONE02 = value.emSELFZONE02 ;
		emSELFZONE02_POS = value.emSELFZONE02_POS;
		for ( i=0; i<EMELEMENT_MAXNUM; ++i )		strSELFZONE02[i] = value.strSELFZONE02[i];

		emTARGZONE01 = value.emTARGZONE01;
		emTARGZONE01_POS = value.emTARGZONE01_POS;
		for ( i=0; i<EMELEMENT_MAXNUM; ++i )		strTARGZONE01[i] = value.strTARGZONE01[i];

		emTARGZONE02 = value.emTARGZONE02;
		emTARGZONE02_POS = value.emTARGZONE02_POS;
		for ( i=0; i<EMELEMENT_MAXNUM; ++i )		strTARGZONE02[i] = value.strTARGZONE02[i];

		emSELFBODY = value.emSELFBODY;
		for ( i=0; i<EMELEMENT_MAXNUM; ++i )		strSELFBODY[i] = value.strSELFBODY[i];

		emTARGBODY01 = value.emTARGBODY01;
		for ( i=0; i<EMELEMENT_MAXNUM; ++i )		strTARGBODY01[i] = value.strTARGBODY01[i];

		emTARGBODY02 = value.emTARGBODY02;
		for ( i=0; i<EMELEMENT_MAXNUM; ++i )		strTARGBODY02[i] = value.strTARGBODY02[i];

		strHOLDOUT = value.strHOLDOUT;

		sICONINDEX = value.sICONINDEX;
		strICONFILE = value.strICONFILE;

		strCOMMENTS = value.strCOMMENTS;
	}

	void SEXT_DATA_112::Assign ( const SEXT_DATA_107 &value )
	{
		int i=0;

		emANIMTYPE = value.emANIMTYPE;
		emANISTYPE = value.emANISTYPE;

		fDELAY4DAMAGE = value.fDELAY4DAMAGE;
		emTARG = value.emTARG;
		emTARG_POSA = value.emTARG_POSA;
		emTARG_POSB = value.emTARG_POSB;
		bTARG_ONE = value.bTARG_ONE;
		for ( i=0; i<EMELEMENT_MAXNUM; ++i )		strTARG[i] = value.strTARG[i];

		emSELFZONE01 = value.emSELFZONE01;
		emSELFZONE01_POS = value.emSELFZONE01_POS;
		for ( i=0; i<EMELEMENT_MAXNUM; ++i )		strSELFZONE01[i] = value.strSELFZONE01[i];

		emSELFZONE02 = value.emSELFZONE02 ;
		emSELFZONE02_POS = value.emSELFZONE02_POS;
		for ( i=0; i<EMELEMENT_MAXNUM; ++i )		strSELFZONE02[i] = value.strSELFZONE02[i];

		emTARGZONE01 = value.emTARGZONE01;
		emTARGZONE01_POS = value.emTARGZONE01_POS;
		for ( i=0; i<EMELEMENT_MAXNUM; ++i )		strTARGZONE01[i] = value.strTARGZONE01[i];

		emTARGZONE02 = value.emTARGZONE02;
		emTARGZONE02_POS = value.emTARGZONE02_POS;
		for ( i=0; i<EMELEMENT_MAXNUM; ++i )		strTARGZONE02[i] = value.strTARGZONE02[i];

		emSELFBODY = value.emSELFBODY;
		for ( i=0; i<EMELEMENT_MAXNUM; ++i )		strSELFBODY[i] = value.strSELFBODY[i];

		emTARGBODY01 = value.emTARGBODY01;
		for ( i=0; i<EMELEMENT_MAXNUM; ++i )		strTARGBODY01[i] = value.strTARGBODY01[i];

		emTARGBODY02 = value.emTARGBODY02;
		for ( i=0; i<EMELEMENT_MAXNUM; ++i )		strTARGBODY02[i] = value.strTARGBODY02[i];

		strHOLDOUT = value.strHOLDOUT;

		sICONINDEX = value.sICONINDEX;
		strICONFILE = value.strICONFILE;

		strCOMMENTS = value.strCOMMENTS;
	}

	void SEXT_DATA_112::Assign ( const SEXT_DATA_108 &value )
	{
		int i=0;

		emANIMTYPE = value.emANIMTYPE;
		emANISTYPE = value.emANISTYPE;		

		fDELAY4DAMAGE = value.fDELAY4DAMAGE;
		emTARG = value.emTARG;
		emTARG_POSA = value.emTARG_POSA;
		emTARG_POSB = value.emTARG_POSB;
		bTARG_ONE = value.bTARG_ONE;
		for ( i=0; i<EMELEMENT_MAXNUM; ++i )		strTARG[i] = value.strTARG[i];

		emSELFZONE01 = value.emSELFZONE01;
		emSELFZONE01_POS = value.emSELFZONE01_POS;
		for ( i=0; i<EMELEMENT_MAXNUM; ++i )		strSELFZONE01[i] = value.strSELFZONE01[i];

		emSELFZONE02 = value.emSELFZONE02 ;
		emSELFZONE02_POS = value.emSELFZONE02_POS;
		for ( i=0; i<EMELEMENT_MAXNUM; ++i )		strSELFZONE02[i] = value.strSELFZONE02[i];

		emTARGZONE01 = value.emTARGZONE01;
		emTARGZONE01_POS = value.emTARGZONE01_POS;
		for ( i=0; i<EMELEMENT_MAXNUM; ++i )		strTARGZONE01[i] = value.strTARGZONE01[i];

		emTARGZONE02 = value.emTARGZONE02;
		emTARGZONE02_POS = value.emTARGZONE02_POS;
		for ( i=0; i<EMELEMENT_MAXNUM; ++i )		strTARGZONE02[i] = value.strTARGZONE02[i];

		emSELFBODY = value.emSELFBODY;
		for ( i=0; i<EMELEMENT_MAXNUM; ++i )		strSELFBODY[i] = value.strSELFBODY[i];

		emTARGBODY01 = value.emTARGBODY01;
		for ( i=0; i<EMELEMENT_MAXNUM; ++i )		strTARGBODY01[i] = value.strTARGBODY01[i];

		emTARGBODY02 = value.emTARGBODY02;
		for ( i=0; i<EMELEMENT_MAXNUM; ++i )		strTARGBODY02[i] = value.strTARGBODY02[i];

		strHOLDOUT = value.strHOLDOUT;

		sICONINDEX = value.sICONINDEX;
		strICONFILE = value.strICONFILE;

		strCOMMENTS = value.strCOMMENTS;
	}

	void SEXT_DATA_112::Assign ( const SEXT_DATA_109 &value )
	{
		int i=0;

		emANIMTYPE = value.emANIMTYPE;
		emANISTYPE = value.emANISTYPE;		

		fDELAY4DAMAGE = value.fDELAY4DAMAGE;
		emTARG = value.emTARG;
		emTARG_POSA = value.emTARG_POSA;
		emTARG_POSB = value.emTARG_POSB;
		bTARG_ONE = value.bTARG_ONE;
		for ( i=0; i<EMELEMENT_MAXNUM; ++i )		strTARG[i] = value.strTARG[i];

		emSELFZONE01 = value.emSELFZONE01;
		emSELFZONE01_POS = value.emSELFZONE01_POS;
		for ( i=0; i<EMELEMENT_MAXNUM; ++i )		strSELFZONE01[i] = value.strSELFZONE01[i];

		emSELFZONE02 = value.emSELFZONE02 ;
		emSELFZONE02_POS = value.emSELFZONE02_POS;
		for ( i=0; i<EMELEMENT_MAXNUM; ++i )		strSELFZONE02[i] = value.strSELFZONE02[i];

		emSELFZONE03 = value.emSELFZONE03 ;
		emSELFZONE03_POS = value.emSELFZONE03_POS;
		for ( i=0; i<EMELEMENT_MAXNUM; ++i )		strSELFZONE03[i] = value.strSELFZONE03[i];

		emTARGZONE01 = value.emTARGZONE01;
		emTARGZONE01_POS = value.emTARGZONE01_POS;
		for ( i=0; i<EMELEMENT_MAXNUM; ++i )		strTARGZONE01[i] = value.strTARGZONE01[i];

		emTARGZONE02 = value.emTARGZONE02;
		emTARGZONE02_POS = value.emTARGZONE02_POS;
		for ( i=0; i<EMELEMENT_MAXNUM; ++i )		strTARGZONE02[i] = value.strTARGZONE02[i];

		emTARGZONE03 = value.emTARGZONE03;
		emTARGZONE03_POS = value.emTARGZONE03_POS;
		for ( i=0; i<EMELEMENT_MAXNUM; ++i )		strTARGZONE03[i] = value.strTARGZONE03[i];

		emSELFBODY = value.emSELFBODY;
		for ( i=0; i<EMELEMENT_MAXNUM; ++i )		strSELFBODY[i] = value.strSELFBODY[i];

		emTARGBODY01 = value.emTARGBODY01;
		for ( i=0; i<EMELEMENT_MAXNUM; ++i )		strTARGBODY01[i] = value.strTARGBODY01[i];

		emTARGBODY02 = value.emTARGBODY02;
		for ( i=0; i<EMELEMENT_MAXNUM; ++i )		strTARGBODY02[i] = value.strTARGBODY02[i];

		strHOLDOUT = value.strHOLDOUT;

		sICONINDEX = value.sICONINDEX;
		strICONFILE = value.strICONFILE;

		strCOMMENTS = value.strCOMMENTS;

		idMultiTargetSkill = value.idMultiTargetSkill;
		nMultiTatgetEffectFrame = value.nMultiTatgetEffectFrame;
	}

	void SEXT_DATA_112::Assign ( const SEXT_DATA_110 &value )
	{
		int i=0;

		emANIMTYPE = value.emANIMTYPE;
		emANISTYPE = value.emANISTYPE;		

		fDELAY4DAMAGE = value.fDELAY4DAMAGE;
		emTARG = value.emTARG;
		emTARG_POSA = value.emTARG_POSA;
		emTARG_POSB = value.emTARG_POSB;
		bTARG_ONE = value.bTARG_ONE;
		for ( i=0; i<EMELEMENT_MAXNUM; ++i )		strTARG[i] = value.strTARG[i];

		emSELFZONE01 = value.emSELFZONE01;
		emSELFZONE01_POS = value.emSELFZONE01_POS;
		for ( i=0; i<EMELEMENT_MAXNUM; ++i )		strSELFZONE01[i] = value.strSELFZONE01[i];

		emSELFZONE02 = value.emSELFZONE02 ;
		emSELFZONE02_POS = value.emSELFZONE02_POS;
		for ( i=0; i<EMELEMENT_MAXNUM; ++i )		strSELFZONE02[i] = value.strSELFZONE02[i];

		emSELFZONE03 = value.emSELFZONE03 ;
		emSELFZONE03_POS = value.emSELFZONE03_POS;
		for ( i=0; i<EMELEMENT_MAXNUM; ++i )		strSELFZONE03[i] = value.strSELFZONE03[i];

		emTARGZONE01 = value.emTARGZONE01;
		emTARGZONE01_POS = value.emTARGZONE01_POS;
		for ( i=0; i<EMELEMENT_MAXNUM; ++i )		strTARGZONE01[i] = value.strTARGZONE01[i];

		emTARGZONE02 = value.emTARGZONE02;
		emTARGZONE02_POS = value.emTARGZONE02_POS;
		for ( i=0; i<EMELEMENT_MAXNUM; ++i )		strTARGZONE02[i] = value.strTARGZONE02[i];

		emTARGZONE03 = value.emTARGZONE03;
		emTARGZONE03_POS = value.emTARGZONE03_POS;
		for ( i=0; i<EMELEMENT_MAXNUM; ++i )		strTARGZONE03[i] = value.strTARGZONE03[i];

		emSELFBODY = value.emSELFBODY;
		for ( i=0; i<EMELEMENT_MAXNUM; ++i )		strSELFBODY[i] = value.strSELFBODY[i];

		emTARGBODY01 = value.emTARGBODY01;
		for ( i=0; i<EMELEMENT_MAXNUM; ++i )		strTARGBODY01[i] = value.strTARGBODY01[i];

		emTARGBODY02 = value.emTARGBODY02;
		for ( i=0; i<EMELEMENT_MAXNUM; ++i )		strTARGBODY02[i] = value.strTARGBODY02[i];

		strHOLDOUT = value.strHOLDOUT;

		sICONINDEX = value.sICONINDEX;
		strICONFILE = value.strICONFILE;

		strCOMMENTS = value.strCOMMENTS;

		idMultiTargetSkill = value.idMultiTargetSkill;
		nMultiTatgetEffectFrame = value.nMultiTatgetEffectFrame;
	}

	void SEXT_DATA_112::Assign ( const SEXT_DATA_111 &value )
	{
		int i=0;

		emANIMTYPE = value.emANIMTYPE;
		emANISTYPE = value.emANISTYPE;		

		fDELAY4DAMAGE = value.fDELAY4DAMAGE;
		emTARG = value.emTARG;
		emTARG_POSA = value.emTARG_POSA;
		emTARG_POSB = value.emTARG_POSB;
		bTARG_ONE = value.bTARG_ONE;
		for ( i=0; i<EMELEMENT_MAXNUM; ++i )		strTARG[i] = value.strTARG[i];

		emSELFZONE01 = value.emSELFZONE01;
		emSELFZONE01_POS = value.emSELFZONE01_POS;
		for ( i=0; i<EMELEMENT_MAXNUM; ++i )		strSELFZONE01[i] = value.strSELFZONE01[i];

		emSELFZONE02 = value.emSELFZONE02 ;
		emSELFZONE02_POS = value.emSELFZONE02_POS;
		for ( i=0; i<EMELEMENT_MAXNUM; ++i )		strSELFZONE02[i] = value.strSELFZONE02[i];

		emSELFZONE03 = value.emSELFZONE03 ;
		emSELFZONE03_POS = value.emSELFZONE03_POS;
		for ( i=0; i<EMELEMENT_MAXNUM; ++i )		strSELFZONE03[i] = value.strSELFZONE03[i];

		emTARGZONE01 = value.emTARGZONE01;
		emTARGZONE01_POS = value.emTARGZONE01_POS;
		for ( i=0; i<EMELEMENT_MAXNUM; ++i )		strTARGZONE01[i] = value.strTARGZONE01[i];

		emTARGZONE02 = value.emTARGZONE02;
		emTARGZONE02_POS = value.emTARGZONE02_POS;
		for ( i=0; i<EMELEMENT_MAXNUM; ++i )		strTARGZONE02[i] = value.strTARGZONE02[i];

		emTARGZONE03 = value.emTARGZONE03;
		emTARGZONE03_POS = value.emTARGZONE03_POS;
		for ( i=0; i<EMELEMENT_MAXNUM; ++i )		strTARGZONE03[i] = value.strTARGZONE03[i];

		emSELFBODY = value.emSELFBODY;
		for ( i=0; i<EMELEMENT_MAXNUM; ++i )		strSELFBODY[i] = value.strSELFBODY[i];

		emTARGBODY01 = value.emTARGBODY01;
		for ( i=0; i<EMELEMENT_MAXNUM; ++i )		strTARGBODY01[i] = value.strTARGBODY01[i];

		emTARGBODY02 = value.emTARGBODY02;
		for ( i=0; i<EMELEMENT_MAXNUM; ++i )		strTARGBODY02[i] = value.strTARGBODY02[i];

		strHOLDOUT = value.strHOLDOUT;

		sICONINDEX = value.sICONINDEX;
		strICONFILE = value.strICONFILE;

		strCOMMENTS = value.strCOMMENTS;

		idMultiTargetSkill = value.idMultiTargetSkill;
		nMultiTatgetEffectFrame = value.nMultiTatgetEffectFrame;

		idDisguiseSkill = value.idDisguiseSkill;
	}

	BOOL SEXT_DATA_103::LOAD ( sc::BaseStream &SFile )
	{
		DWORD dwDATA;
		SFile >> dwDATA;	emANIMTYPE = EMANI_MAINTYPE(dwDATA);
		SFile >> dwDATA;	emANISTYPE = EMANI_SUBTYPE(dwDATA);

		SFile >> dwDATA;	emTARG = EMEFFECTIME(dwDATA);
		SFile >> dwDATA;	emTARG_POSA = EMEFFECTPOS(dwDATA);
		SFile >> dwDATA;	emTARG_POSB = EMEFFECTPOS(dwDATA);

		for ( int i=0; i<EMELEMENT_OLDMAX; ++i )
			SFile >> strTARG[i];

		SFile >> dwDATA;	emSELFZONE01 = EMEFFECTIME(dwDATA);
		SFile >> dwDATA;	emSELFZONE01_POS = EMEFFECTPOS(dwDATA);
		for ( int i=0; i<EMELEMENT_OLDMAX; ++i )
			SFile >> strSELFZONE01[i];

		SFile >> dwDATA;	emSELFZONE02 = EMEFFECTIME(dwDATA);
		SFile >> dwDATA;	emSELFZONE02_POS = EMEFFECTPOS(dwDATA);
		for ( int i=0; i<EMELEMENT_OLDMAX; ++i )
			SFile >> strSELFZONE02[i];

		SFile >> dwDATA;	emTARGZONE01 = EMEFFECTIME(dwDATA);
		SFile >> dwDATA;	emTARGZONE01_POS = EMEFFECTPOS(dwDATA);
		for ( int i=0; i<EMELEMENT_OLDMAX; ++i )
			SFile >> strTARGZONE01[i];

		SFile >> dwDATA;	emTARGZONE02 = EMEFFECTIME(dwDATA);
		SFile >> dwDATA;	emTARGZONE02_POS = EMEFFECTPOS(dwDATA);
		for ( int i=0; i<EMELEMENT_OLDMAX; ++i )
			SFile >> strTARGZONE02[i];

		SFile >> dwDATA;	emSELFBODY = EMEFFECTIME(dwDATA);
		for ( int i=0; i<EMELEMENT_OLDMAX; ++i )
			SFile >> strSELFBODY[i];

		SFile >> dwDATA;	emTARGBODY01 = EMEFFECTIME(dwDATA);
		for ( int i=0; i<EMELEMENT_OLDMAX; ++i )
			SFile >> strTARGBODY01[i];

		SFile >> dwDATA;	emTARGBODY02 = EMEFFECTIME(dwDATA);
		for ( int i=0; i<EMELEMENT_OLDMAX; ++i )
			SFile >> strTARGBODY02[i];

		SFile >> strHOLDOUT;

		SFile >> sICONINDEX.dwID;
		SFile >> strICONFILE;

		SFile >> strCOMMENTS;

		strICONFILE = MakeNewIconFileName( strICONFILE.c_str(), sICONINDEX );

		return TRUE;
	}

	BOOL SEXT_DATA_104::LOAD ( sc::BaseStream &SFile )
	{
		DWORD dwDATA;
		SFile >> dwDATA;	emANIMTYPE = EMANI_MAINTYPE(dwDATA);
		SFile >> dwDATA;	emANISTYPE = EMANI_SUBTYPE(dwDATA);

		SFile >> fDELAY4DAMAGE;
		SFile >> dwDATA;	emTARG = EMEFFECTIME(dwDATA);
		SFile >> dwDATA;	emTARG_POSA = EMEFFECTPOS(dwDATA);
		SFile >> dwDATA;	emTARG_POSB = EMEFFECTPOS(dwDATA);

		for ( int i=0; i<EMELEMENT_OLDMAX; ++i )
			SFile >> strTARG[i];

		SFile >> dwDATA;	emSELFZONE01 = EMEFFECTIME(dwDATA);
		SFile >> dwDATA;	emSELFZONE01_POS = EMEFFECTPOS(dwDATA);
		for ( int i=0; i<EMELEMENT_OLDMAX; ++i )
			SFile >> strSELFZONE01[i];

		SFile >> dwDATA;	emSELFZONE02 = EMEFFECTIME(dwDATA);
		SFile >> dwDATA;	emSELFZONE02_POS = EMEFFECTPOS(dwDATA);
		for ( int i=0; i<EMELEMENT_OLDMAX; ++i )
			SFile >> strSELFZONE02[i];

		SFile >> dwDATA;	emTARGZONE01 = EMEFFECTIME(dwDATA);
		SFile >> dwDATA;	emTARGZONE01_POS = EMEFFECTPOS(dwDATA);
		for ( int i=0; i<EMELEMENT_OLDMAX; ++i )
			SFile >> strTARGZONE01[i];

		SFile >> dwDATA;	emTARGZONE02 = EMEFFECTIME(dwDATA);
		SFile >> dwDATA;	emTARGZONE02_POS = EMEFFECTPOS(dwDATA);
		for ( int i=0; i<EMELEMENT_OLDMAX; ++i )
			SFile >> strTARGZONE02[i];

		SFile >> dwDATA;	emSELFBODY = EMEFFECTIME(dwDATA);
		for ( int i=0; i<EMELEMENT_OLDMAX; ++i )
			SFile >> strSELFBODY[i];

		SFile >> dwDATA;	emTARGBODY01 = EMEFFECTIME(dwDATA);
		for ( int i=0; i<EMELEMENT_OLDMAX; ++i )
			SFile >> strTARGBODY01[i];

		SFile >> dwDATA;	emTARGBODY02 = EMEFFECTIME(dwDATA);
		for ( int i=0; i<EMELEMENT_OLDMAX; ++i )
			SFile >> strTARGBODY02[i];

		SFile >> strHOLDOUT;

		SFile >> sICONINDEX.dwID;
		SFile >> strICONFILE;

		SFile >> strCOMMENTS;

		strICONFILE = MakeNewIconFileName( strICONFILE.c_str(), sICONINDEX );

		return TRUE;
	}

	BOOL SEXT_DATA_105::LOAD ( sc::BaseStream &SFile )
	{
		DWORD dwDATA;
		SFile >> dwDATA;	emANIMTYPE = EMANI_MAINTYPE(dwDATA);
		SFile >> dwDATA;	emANISTYPE = EMANI_SUBTYPE(dwDATA);

		SFile >> fDELAY4DAMAGE;
		SFile >> dwDATA;	emTARG = EMEFFECTIME(dwDATA);
		SFile >> dwDATA;	emTARG_POSA = EMEFFECTPOS(dwDATA);
		SFile >> dwDATA;	emTARG_POSB = EMEFFECTPOS(dwDATA);
		SFile >> bTARG_ONE;

		for ( int i=0; i<EMELEMENT_OLDMAX; ++i )
			SFile >> strTARG[i];

		SFile >> dwDATA;	emSELFZONE01 = EMEFFECTIME(dwDATA);
		SFile >> dwDATA;	emSELFZONE01_POS = EMEFFECTPOS(dwDATA);
		for ( int i=0; i<EMELEMENT_OLDMAX; ++i )
			SFile >> strSELFZONE01[i];

		SFile >> dwDATA;	emSELFZONE02 = EMEFFECTIME(dwDATA);
		SFile >> dwDATA;	emSELFZONE02_POS = EMEFFECTPOS(dwDATA);
		for ( int i=0; i<EMELEMENT_OLDMAX; ++i )
			SFile >> strSELFZONE02[i];

		SFile >> dwDATA;	emTARGZONE01 = EMEFFECTIME(dwDATA);
		SFile >> dwDATA;	emTARGZONE01_POS = EMEFFECTPOS(dwDATA);
		for ( int i=0; i<EMELEMENT_OLDMAX; ++i )
			SFile >> strTARGZONE01[i];

		SFile >> dwDATA;	emTARGZONE02 = EMEFFECTIME(dwDATA);
		SFile >> dwDATA;	emTARGZONE02_POS = EMEFFECTPOS(dwDATA);
		for ( int i=0; i<EMELEMENT_OLDMAX; ++i )
			SFile >> strTARGZONE02[i];

		SFile >> dwDATA;	emSELFBODY = EMEFFECTIME(dwDATA);
		for ( int i=0; i<EMELEMENT_OLDMAX; ++i )
			SFile >> strSELFBODY[i];

		SFile >> dwDATA;	emTARGBODY01 = EMEFFECTIME(dwDATA);
		for ( int i=0; i<EMELEMENT_OLDMAX; ++i )
			SFile >> strTARGBODY01[i];

		SFile >> dwDATA;	emTARGBODY02 = EMEFFECTIME(dwDATA);
		for ( int i=0; i<EMELEMENT_OLDMAX; ++i )
			SFile >> strTARGBODY02[i];

		SFile >> strHOLDOUT;

		SFile >> sICONINDEX.dwID;
		SFile >> strICONFILE;

		SFile >> strCOMMENTS;

		strICONFILE = MakeNewIconFileName( strICONFILE.c_str(), sICONINDEX );

		return TRUE;
	}

	BOOL SEXT_DATA_106::LOAD ( sc::BaseStream &SFile )
	{
		DWORD dwDATA;
		SFile >> dwDATA;	emANIMTYPE = EMANI_MAINTYPE(dwDATA);
		SFile >> dwDATA;	emANISTYPE = EMANI_SUBTYPE(dwDATA);

		SFile >> fDELAY4DAMAGE;
		SFile >> dwDATA;	emTARG = EMEFFECTIME(dwDATA);
		SFile >> dwDATA;	emTARG_POSA = EMEFFECTPOS(dwDATA);
		SFile >> dwDATA;	emTARG_POSB = EMEFFECTPOS(dwDATA);
		SFile >> bTARG_ONE;

		for ( int i=0; i<EMELEMENT_MAXNUM; ++i )
			SFile >> strTARG[i];

		SFile >> dwDATA;	emSELFZONE01 = EMEFFECTIME(dwDATA);
		SFile >> dwDATA;	emSELFZONE01_POS = EMEFFECTPOS(dwDATA);
		for ( int i=0; i<EMELEMENT_MAXNUM; ++i )
			SFile >> strSELFZONE01[i];

		SFile >> dwDATA;	emSELFZONE02 = EMEFFECTIME(dwDATA);
		SFile >> dwDATA;	emSELFZONE02_POS = EMEFFECTPOS(dwDATA);
		for ( int i=0; i<EMELEMENT_MAXNUM; ++i )
			SFile >> strSELFZONE02[i];

		SFile >> dwDATA;	emTARGZONE01 = EMEFFECTIME(dwDATA);
		SFile >> dwDATA;	emTARGZONE01_POS = EMEFFECTPOS(dwDATA);
		for ( int i=0; i<EMELEMENT_MAXNUM; ++i )
			SFile >> strTARGZONE01[i];

		SFile >> dwDATA;	emTARGZONE02 = EMEFFECTIME(dwDATA);
		SFile >> dwDATA;	emTARGZONE02_POS = EMEFFECTPOS(dwDATA);
		for ( int i=0; i<EMELEMENT_MAXNUM; ++i )
			SFile >> strTARGZONE02[i];

		SFile >> dwDATA;	emSELFBODY = EMEFFECTIME(dwDATA);
		for ( int i=0; i<EMELEMENT_MAXNUM; ++i )
			SFile >> strSELFBODY[i];

		SFile >> dwDATA;	emTARGBODY01 = EMEFFECTIME(dwDATA);
		for ( int i=0; i<EMELEMENT_MAXNUM; ++i )
			SFile >> strTARGBODY01[i];

		SFile >> dwDATA;	emTARGBODY02 = EMEFFECTIME(dwDATA);
		for ( int i=0; i<EMELEMENT_MAXNUM; ++i )
			SFile >> strTARGBODY02[i];

		SFile >> strHOLDOUT;

		SFile >> sICONINDEX.dwID;
		SFile >> strICONFILE;

		SFile >> strCOMMENTS;

		strICONFILE = MakeNewIconFileName( strICONFILE.c_str(), sICONINDEX );

		return TRUE;
	}

	BOOL SEXT_DATA_107::LOAD ( sc::BaseStream &SFile )
	{
		DWORD dwDATA;
		SFile >> dwDATA;	emANIMTYPE = EMANI_MAINTYPE(dwDATA);
		SFile >> dwDATA;	emANISTYPE = EMANI_SUBTYPE(dwDATA);

		SFile >> sICONINDEX.dwID;
		SFile >> strICONFILE;

		SFile >> strCOMMENTS;

		SFile >> dwDATA;	emTARG = EMEFFECTIME(dwDATA);
		SFile >> dwDATA;	emTARG_POSA = EMEFFECTPOS(dwDATA);
		SFile >> dwDATA;	emTARG_POSB = EMEFFECTPOS(dwDATA);
		SFile >> fDELAY4DAMAGE;

		SFile >> strHOLDOUT;

		SFile >> dwDATA;	emSELFZONE01 = EMEFFECTIME(dwDATA);
		SFile >> dwDATA;	emSELFZONE01_POS = EMEFFECTPOS(dwDATA);
		for ( int i=0; i<EMELEMENT_MAXNUM; ++i )
			SFile >> strSELFZONE01[i];

		SFile >> dwDATA;	emSELFZONE02 = EMEFFECTIME(dwDATA);
		SFile >> dwDATA;	emSELFZONE02_POS = EMEFFECTPOS(dwDATA);
		for ( int i=0; i<EMELEMENT_MAXNUM; ++i )
			SFile >> strSELFZONE02[i];

		SFile >> bTARG_ONE;
		for ( int i=0; i<EMELEMENT_MAXNUM; ++i )
			SFile >> strTARG[i];

		SFile >> dwDATA;	emTARGZONE01 = EMEFFECTIME(dwDATA);
		SFile >> dwDATA;	emTARGZONE01_POS = EMEFFECTPOS(dwDATA);
		for ( int i=0; i<EMELEMENT_MAXNUM; ++i )
			SFile >> strTARGZONE01[i];

		SFile >> dwDATA;	emTARGZONE02 = EMEFFECTIME(dwDATA);
		SFile >> dwDATA;	emTARGZONE02_POS = EMEFFECTPOS(dwDATA);
		for ( int i=0; i<EMELEMENT_MAXNUM; ++i )
			SFile >> strTARGZONE02[i];

		SFile >> dwDATA;	emTARGBODY01 = EMEFFECTIME(dwDATA);
		for ( int i=0; i<EMELEMENT_MAXNUM; ++i )
			SFile >> strTARGBODY01[i];

		SFile >> dwDATA;	emTARGBODY02 = EMEFFECTIME(dwDATA);
		for ( int i=0; i<EMELEMENT_MAXNUM; ++i )
			SFile >> strTARGBODY02[i];

		SFile >> dwDATA;	emSELFBODY = EMEFFECTIME(dwDATA);
		for ( int i=0; i<EMELEMENT_MAXNUM; ++i )
			SFile >> strSELFBODY[i];

		strICONFILE = MakeNewIconFileName( strICONFILE.c_str(), sICONINDEX );

		return TRUE;
	}

	BOOL SEXT_DATA_108::LOAD ( sc::BaseStream &SFile )
	{
		DWORD dwDATA;
		SFile >> dwDATA;	emANIMTYPE = EMANI_MAINTYPE(dwDATA);
		SFile >> dwDATA;	emANISTYPE = EMANI_SUBTYPE(dwDATA);
		SFile >> dwDATA;	emANIVehicleMTYPE = EMANI_MAINTYPE(dwDATA);

		SFile >> sICONINDEX.dwID;
		SFile >> strICONFILE;

		SFile >> strCOMMENTS;

		SFile >> dwDATA;	emTARG = EMEFFECTIME(dwDATA);
		SFile >> dwDATA;	emTARG_POSA = EMEFFECTPOS(dwDATA);
		SFile >> dwDATA;	emTARG_POSB = EMEFFECTPOS(dwDATA);
		SFile >> fDELAY4DAMAGE;

		SFile >> strHOLDOUT;

		SFile >> dwDATA;	emSELFZONE01 = EMEFFECTIME(dwDATA);
		SFile >> dwDATA;	emSELFZONE01_POS = EMEFFECTPOS(dwDATA);
		for ( int i=0; i<EMELEMENT_MAXNUM; ++i )
			SFile >> strSELFZONE01[i];

		SFile >> dwDATA;	emSELFZONE02 = EMEFFECTIME(dwDATA);
		SFile >> dwDATA;	emSELFZONE02_POS = EMEFFECTPOS(dwDATA);
		for ( int i=0; i<EMELEMENT_MAXNUM; ++i )
			SFile >> strSELFZONE02[i];

		SFile >> bTARG_ONE;
		for ( int i=0; i<EMELEMENT_MAXNUM; ++i )
			SFile >> strTARG[i];

		SFile >> dwDATA;	emTARGZONE01 = EMEFFECTIME(dwDATA);
		SFile >> dwDATA;	emTARGZONE01_POS = EMEFFECTPOS(dwDATA);
		for ( int i=0; i<EMELEMENT_MAXNUM; ++i )
			SFile >> strTARGZONE01[i];

		SFile >> dwDATA;	emTARGZONE02 = EMEFFECTIME(dwDATA);
		SFile >> dwDATA;	emTARGZONE02_POS = EMEFFECTPOS(dwDATA);
		for ( int i=0; i<EMELEMENT_MAXNUM; ++i )
			SFile >> strTARGZONE02[i];

		SFile >> dwDATA;	emTARGBODY01 = EMEFFECTIME(dwDATA);
		for ( int i=0; i<EMELEMENT_MAXNUM; ++i )
			SFile >> strTARGBODY01[i];

		SFile >> dwDATA;	emTARGBODY02 = EMEFFECTIME(dwDATA);
		for ( int i=0; i<EMELEMENT_MAXNUM; ++i )
			SFile >> strTARGBODY02[i];

		SFile >> dwDATA;	emSELFBODY = EMEFFECTIME(dwDATA);
		for ( int i=0; i<EMELEMENT_MAXNUM; ++i )
			SFile >> strSELFBODY[i];

		strICONFILE = MakeNewIconFileName( strICONFILE.c_str(), sICONINDEX );

		return TRUE;
	}

	BOOL SEXT_DATA_109::LOAD ( sc::BaseStream &SFile )
	{
		DWORD dwDATA;
		SFile >> dwDATA;	emANIMTYPE = EMANI_MAINTYPE(dwDATA);
		SFile >> dwDATA;	emANISTYPE = EMANI_SUBTYPE(dwDATA);
		SFile >> dwDATA;	emANIVehicleMTYPE = EMANI_MAINTYPE(dwDATA);

		SFile >> sICONINDEX.dwID;
		SFile >> strICONFILE;

		SFile >> strCOMMENTS;

		SFile >> dwDATA;	emTARG = EMEFFECTIME(dwDATA);
		SFile >> dwDATA;	emTARG_POSA = EMEFFECTPOS(dwDATA);
		SFile >> dwDATA;	emTARG_POSB = EMEFFECTPOS(dwDATA);
		SFile >> fDELAY4DAMAGE;

		SFile >> strHOLDOUT;

		SFile >> dwDATA;	emSELFZONE01 = EMEFFECTIME(dwDATA);
		SFile >> dwDATA;	emSELFZONE01_POS = EMEFFECTPOS(dwDATA);
		for ( int i=0; i<EMELEMENT_MAXNUM; ++i )
			SFile >> strSELFZONE01[i];

		SFile >> dwDATA;	emSELFZONE02 = EMEFFECTIME(dwDATA);
		SFile >> dwDATA;	emSELFZONE02_POS = EMEFFECTPOS(dwDATA);
		for ( int i=0; i<EMELEMENT_MAXNUM; ++i )
			SFile >> strSELFZONE02[i];

		SFile >> bTARG_ONE;
		for ( int i=0; i<EMELEMENT_MAXNUM; ++i )
			SFile >> strTARG[i];

		SFile >> dwDATA;	emTARGZONE01 = EMEFFECTIME(dwDATA);
		SFile >> dwDATA;	emTARGZONE01_POS = EMEFFECTPOS(dwDATA);
		for ( int i=0; i<EMELEMENT_MAXNUM; ++i )
			SFile >> strTARGZONE01[i];

		SFile >> dwDATA;	emTARGZONE02 = EMEFFECTIME(dwDATA);
		SFile >> dwDATA;	emTARGZONE02_POS = EMEFFECTPOS(dwDATA);
		for ( int i=0; i<EMELEMENT_MAXNUM; ++i )
			SFile >> strTARGZONE02[i];


		SFile >> dwDATA;	emTARGBODY01 = EMEFFECTIME(dwDATA);
		for ( int i=0; i<EMELEMENT_MAXNUM; ++i )
			SFile >> strTARGBODY01[i];

		SFile >> dwDATA;	emTARGBODY02 = EMEFFECTIME(dwDATA);
		for ( int i=0; i<EMELEMENT_MAXNUM; ++i )
			SFile >> strTARGBODY02[i];

		SFile >> dwDATA;	emSELFBODY = EMEFFECTIME(dwDATA);
		for ( int i=0; i<EMELEMENT_MAXNUM; ++i )
			SFile >> strSELFBODY[i];

		SFile >> idMultiTargetSkill.dwID;
		SFile >> nMultiTatgetEffectFrame;

		SFile >> dwDATA;	emTARGZONE03 = EMEFFECTIME(dwDATA);
		SFile >> dwDATA;	emTARGZONE03_POS = EMEFFECTPOS(dwDATA);
		for ( int i=0; i<EMELEMENT_MAXNUM; ++i )
			SFile >> strTARGZONE03[i];

		SFile >> dwDATA;	emSELFZONE03 = EMEFFECTIME(dwDATA);
		SFile >> dwDATA;	emSELFZONE03_POS = EMEFFECTPOS(dwDATA);
		for ( int i=0; i<EMELEMENT_MAXNUM; ++i )
			SFile >> strSELFZONE03[i];	

		strICONFILE = MakeNewIconFileName( strICONFILE.c_str(), sICONINDEX );

		return TRUE;
	}

	BOOL SEXT_DATA_110::LOAD ( sc::BaseStream &SFile )
	{
		DWORD dwDATA;
		SFile >> dwDATA;	emANIMTYPE = EMANI_MAINTYPE(dwDATA);
		SFile >> dwDATA;	emANISTYPE = EMANI_SUBTYPE(dwDATA);
		SFile >> dwDATA;	emANIVehicleMTYPE = EMANI_MAINTYPE(dwDATA);

		SFile >> sICONINDEX.dwID;
		SFile >> strICONFILE;

		SFile >> strCOMMENTS;

		SFile >> dwDATA;	emTARG = EMEFFECTIME(dwDATA);
		SFile >> dwDATA;	emTARG_POSA = EMEFFECTPOS(dwDATA);
		SFile >> dwDATA;	emTARG_POSB = EMEFFECTPOS(dwDATA);
		SFile >> fDELAY4DAMAGE;

		SFile >> strHOLDOUT;

		SFile >> dwDATA;	emSELFZONE01 = EMEFFECTIME(dwDATA);
		SFile >> dwDATA;	emSELFZONE01_POS = EMEFFECTPOS(dwDATA);
		for ( int i=0; i<EMELEMENT_MAXNUM; ++i )
			SFile >> strSELFZONE01[i];

		SFile >> dwDATA;	emSELFZONE02 = EMEFFECTIME(dwDATA);
		SFile >> dwDATA;	emSELFZONE02_POS = EMEFFECTPOS(dwDATA);
		for ( int i=0; i<EMELEMENT_MAXNUM; ++i )
			SFile >> strSELFZONE02[i];

		SFile >> dwDATA;	emSELFZONE03 = EMEFFECTIME(dwDATA);
		SFile >> dwDATA;	emSELFZONE03_POS = EMEFFECTPOS(dwDATA);
		for ( int i=0; i<EMELEMENT_MAXNUM; ++i )
			SFile >> strSELFZONE03[i];		

		SFile >> bTARG_ONE;
		for ( int i=0; i<EMELEMENT_MAXNUM; ++i )
			SFile >> strTARG[i];

		SFile >> dwDATA;	emTARGZONE01 = EMEFFECTIME(dwDATA);
		SFile >> dwDATA;	emTARGZONE01_POS = EMEFFECTPOS(dwDATA);
		for ( int i=0; i<EMELEMENT_MAXNUM; ++i )
			SFile >> strTARGZONE01[i];

		SFile >> dwDATA;	emTARGZONE02 = EMEFFECTIME(dwDATA);
		SFile >> dwDATA;	emTARGZONE02_POS = EMEFFECTPOS(dwDATA);
		for ( int i=0; i<EMELEMENT_MAXNUM; ++i )
			SFile >> strTARGZONE02[i];

		SFile >> dwDATA;	emTARGZONE03 = EMEFFECTIME(dwDATA);
		SFile >> dwDATA;	emTARGZONE03_POS = EMEFFECTPOS(dwDATA);
		for ( int i=0; i<EMELEMENT_MAXNUM; ++i )
			SFile >> strTARGZONE03[i];

		SFile >> dwDATA;	emTARGBODY01 = EMEFFECTIME(dwDATA);
		for ( int i=0; i<EMELEMENT_MAXNUM; ++i )
			SFile >> strTARGBODY01[i];

		SFile >> dwDATA;	emTARGBODY02 = EMEFFECTIME(dwDATA);
		for ( int i=0; i<EMELEMENT_MAXNUM; ++i )
			SFile >> strTARGBODY02[i];

		SFile >> dwDATA;	emSELFBODY = EMEFFECTIME(dwDATA);
		for ( int i=0; i<EMELEMENT_MAXNUM; ++i )
			SFile >> strSELFBODY[i];

		SFile >> idMultiTargetSkill.dwID;
		SFile >> nMultiTatgetEffectFrame;

		strICONFILE = MakeNewIconFileName( strICONFILE.c_str(), sICONINDEX );

		return TRUE;
	}

	BOOL SEXT_DATA_111::LOAD ( sc::BaseStream &SFile )
	{
		DWORD dwDATA;
		SFile >> dwDATA;	emANIMTYPE = EMANI_MAINTYPE(dwDATA);
		SFile >> dwDATA;	emANISTYPE = EMANI_SUBTYPE(dwDATA);
		SFile >> dwDATA;	emANIVehicleMTYPE = EMANI_MAINTYPE(dwDATA);

		SFile >> sICONINDEX.dwID;
		SFile >> strICONFILE;

		SFile >> strCOMMENTS;

		SFile >> dwDATA;	emTARG = EMEFFECTIME(dwDATA);
		SFile >> dwDATA;	emTARG_POSA = EMEFFECTPOS(dwDATA);
		SFile >> dwDATA;	emTARG_POSB = EMEFFECTPOS(dwDATA);
		SFile >> fDELAY4DAMAGE;

		SFile >> strHOLDOUT;

		SFile >> dwDATA;	emSELFZONE01 = EMEFFECTIME(dwDATA);
		SFile >> dwDATA;	emSELFZONE01_POS = EMEFFECTPOS(dwDATA);
		for ( int i=0; i<EMELEMENT_MAXNUM; ++i )
			SFile >> strSELFZONE01[i];

		SFile >> dwDATA;	emSELFZONE02 = EMEFFECTIME(dwDATA);
		SFile >> dwDATA;	emSELFZONE02_POS = EMEFFECTPOS(dwDATA);
		for ( int i=0; i<EMELEMENT_MAXNUM; ++i )
			SFile >> strSELFZONE02[i];

		SFile >> dwDATA;	emSELFZONE03 = EMEFFECTIME(dwDATA);
		SFile >> dwDATA;	emSELFZONE03_POS = EMEFFECTPOS(dwDATA);
		for ( int i=0; i<EMELEMENT_MAXNUM; ++i )
			SFile >> strSELFZONE03[i];		

		SFile >> bTARG_ONE;
		for ( int i=0; i<EMELEMENT_MAXNUM; ++i )
			SFile >> strTARG[i];

		SFile >> dwDATA;	emTARGZONE01 = EMEFFECTIME(dwDATA);
		SFile >> dwDATA;	emTARGZONE01_POS = EMEFFECTPOS(dwDATA);
		for ( int i=0; i<EMELEMENT_MAXNUM; ++i )
			SFile >> strTARGZONE01[i];

		SFile >> dwDATA;	emTARGZONE02 = EMEFFECTIME(dwDATA);
		SFile >> dwDATA;	emTARGZONE02_POS = EMEFFECTPOS(dwDATA);
		for ( int i=0; i<EMELEMENT_MAXNUM; ++i )
			SFile >> strTARGZONE02[i];

		SFile >> dwDATA;	emTARGZONE03 = EMEFFECTIME(dwDATA);
		SFile >> dwDATA;	emTARGZONE03_POS = EMEFFECTPOS(dwDATA);
		for ( int i=0; i<EMELEMENT_MAXNUM; ++i )
			SFile >> strTARGZONE03[i];

		SFile >> dwDATA;	emTARGBODY01 = EMEFFECTIME(dwDATA);
		for ( int i=0; i<EMELEMENT_MAXNUM; ++i )
			SFile >> strTARGBODY01[i];

		SFile >> dwDATA;	emTARGBODY02 = EMEFFECTIME(dwDATA);
		for ( int i=0; i<EMELEMENT_MAXNUM; ++i )
			SFile >> strTARGBODY02[i];

		SFile >> dwDATA;	emSELFBODY = EMEFFECTIME(dwDATA);
		for ( int i=0; i<EMELEMENT_MAXNUM; ++i )
			SFile >> strSELFBODY[i];

		SFile >> idMultiTargetSkill.dwID;
		SFile >> nMultiTatgetEffectFrame;

		SFile >> idDisguiseSkill.dwID;

		strICONFILE = MakeNewIconFileName( strICONFILE.c_str(), sICONINDEX );

		return TRUE;
	}

	BOOL SEXT_DATA_112::LOAD ( sc::BaseStream &SFile )
	{
		DWORD dwDATA;
		SFile >> dwDATA;	emANIMTYPE = EMANI_MAINTYPE(dwDATA);
		SFile >> dwDATA;	emANISTYPE = EMANI_SUBTYPE(dwDATA);
		SFile >> dwDATA;	emANIVehicleMTYPE = EMANI_MAINTYPE(dwDATA);

		SFile >> sICONINDEX.dwID;
		SFile >> strICONFILE;

		SFile >> strCOMMENTS;

		SFile >> dwDATA;	emTARG = EMEFFECTIME(dwDATA);
		SFile >> dwDATA;	emTARG_POSA = EMEFFECTPOS(dwDATA);
		SFile >> dwDATA;	emTARG_POSB = EMEFFECTPOS(dwDATA);
		SFile >> fDELAY4DAMAGE;

		SFile >> strHOLDOUT;

		SFile >> dwDATA;	emSELFZONE01 = EMEFFECTIME(dwDATA);
		SFile >> dwDATA;	emSELFZONE01_POS = EMEFFECTPOS(dwDATA);
		for ( int i=0; i<EMELEMENT_MAXNUM; ++i )
			SFile >> strSELFZONE01[i];

		SFile >> dwDATA;	emSELFZONE02 = EMEFFECTIME(dwDATA);
		SFile >> dwDATA;	emSELFZONE02_POS = EMEFFECTPOS(dwDATA);
		for ( int i=0; i<EMELEMENT_MAXNUM; ++i )
			SFile >> strSELFZONE02[i];

		SFile >> dwDATA;	emSELFZONE03 = EMEFFECTIME(dwDATA);
		SFile >> dwDATA;	emSELFZONE03_POS = EMEFFECTPOS(dwDATA);
		for ( int i=0; i<EMELEMENT_MAXNUM; ++i )
			SFile >> strSELFZONE03[i];		

		SFile >> bTARG_ONE;
		for ( int i=0; i<EMELEMENT_MAXNUM; ++i )
			SFile >> strTARG[i];

		SFile >> dwDATA;	emTARGZONE01 = EMEFFECTIME(dwDATA);
		SFile >> dwDATA;	emTARGZONE01_POS = EMEFFECTPOS(dwDATA);
		for ( int i=0; i<EMELEMENT_MAXNUM; ++i )
			SFile >> strTARGZONE01[i];

		SFile >> dwDATA;	emTARGZONE02 = EMEFFECTIME(dwDATA);
		SFile >> dwDATA;	emTARGZONE02_POS = EMEFFECTPOS(dwDATA);
		for ( int i=0; i<EMELEMENT_MAXNUM; ++i )
			SFile >> strTARGZONE02[i];

		SFile >> dwDATA;	emTARGZONE03 = EMEFFECTIME(dwDATA);
		SFile >> dwDATA;	emTARGZONE03_POS = EMEFFECTPOS(dwDATA);
		for ( int i=0; i<EMELEMENT_MAXNUM; ++i )
			SFile >> strTARGZONE03[i];

		SFile >> dwDATA;	emTARGBODY01 = EMEFFECTIME(dwDATA);
		for ( int i=0; i<EMELEMENT_MAXNUM; ++i )
			SFile >> strTARGBODY01[i];

		SFile >> dwDATA;	emTARGBODY02 = EMEFFECTIME(dwDATA);
		for ( int i=0; i<EMELEMENT_MAXNUM; ++i )
			SFile >> strTARGBODY02[i];

		SFile >> dwDATA;	emSELFBODY = EMEFFECTIME(dwDATA);
		for ( int i=0; i<EMELEMENT_MAXNUM; ++i )
			SFile >> strSELFBODY[i];

		SFile >> idMultiTargetSkill.dwID;
		SFile >> nMultiTatgetEffectFrame;

		SFile >> idDisguiseSkill.dwID;

		return TRUE;
	}

	SEXT_DATA_112& SEXT_DATA_112::operator= ( const SEXT_DATA_112 &value )
	{
		int i=0;

		emANIMTYPE = value.emANIMTYPE;
		emANISTYPE = value.emANISTYPE;
		emANIVehicleMTYPE = value.emANIVehicleMTYPE;

		fDELAY4DAMAGE = value.fDELAY4DAMAGE;
		emTARG = value.emTARG;
		emTARG_POSA = value.emTARG_POSA;
		emTARG_POSB = value.emTARG_POSB;
		bTARG_ONE = value.bTARG_ONE;
		for ( i=0; i<EMELEMENT_MAXNUM; ++i )
			strTARG[i] = value.strTARG[i];

		emSELFZONE01 = value.emSELFZONE01;
		emSELFZONE01_POS = value.emSELFZONE01_POS;
		for ( i=0; i<EMELEMENT_MAXNUM; ++i )
			strSELFZONE01[i] = value.strSELFZONE01[i];

		emSELFZONE02 = value.emSELFZONE02 ;
		emSELFZONE02_POS = value.emSELFZONE02_POS;
		for ( i=0; i<EMELEMENT_MAXNUM; ++i )
			strSELFZONE02[i] = value.strSELFZONE02[i];

		emSELFZONE03 = value.emSELFZONE03 ;
		emSELFZONE03_POS = value.emSELFZONE03_POS;
		for ( i=0; i<EMELEMENT_MAXNUM; ++i )
			strSELFZONE03[i] = value.strSELFZONE03[i];

		emTARGZONE01 = value.emTARGZONE01;
		emTARGZONE01_POS = value.emTARGZONE01_POS;
		for ( i=0; i<EMELEMENT_MAXNUM; ++i )
			strTARGZONE01[i] = value.strTARGZONE01[i];

		emTARGZONE02 = value.emTARGZONE02;
		emTARGZONE02_POS = value.emTARGZONE02_POS;
		for ( i=0; i<EMELEMENT_MAXNUM; ++i )
			strTARGZONE02[i] = value.strTARGZONE02[i];

		emTARGZONE03 = value.emTARGZONE03;
		emTARGZONE03_POS = value.emTARGZONE03_POS;
		for ( i=0; i<EMELEMENT_MAXNUM; ++i )
			strTARGZONE03[i] = value.strTARGZONE03[i];

		emSELFBODY = value.emSELFBODY;
		for ( i=0; i<EMELEMENT_MAXNUM; ++i )
			strSELFBODY[i] = value.strSELFBODY[i];

		emTARGBODY01 = value.emTARGBODY01;
		for ( i=0; i<EMELEMENT_MAXNUM; ++i )
			strTARGBODY01[i] = value.strTARGBODY01[i];

		emTARGBODY02 = value.emTARGBODY02;
		for ( i=0; i<EMELEMENT_MAXNUM; ++i )
			strTARGBODY02[i] = value.strTARGBODY02[i];

		strHOLDOUT = value.strHOLDOUT;

		sICONINDEX = value.sICONINDEX;
		strICONFILE = value.strICONFILE;

		strCOMMENTS = value.strCOMMENTS;

		nMultiTatgetEffectFrame = value.nMultiTatgetEffectFrame;
		idMultiTargetSkill = value.idMultiTargetSkill;

		idDisguiseSkill	= value.idDisguiseSkill;

		return *this;
	}

	//////////////////////////////////////////////////////////////////////////
	// Ext Data current version
	//////////////////////////////////////////////////////////////////////////

	BOOL SEXT_DATA_112::SAVE( sc::SerialFile &SFile )
	{
		SFile << (DWORD)emANIMTYPE;
		SFile << (DWORD)emANISTYPE;
		SFile << (DWORD)emANIVehicleMTYPE;

		SFile << sICONINDEX.dwID;
		SFile << strICONFILE;

		SFile << strCOMMENTS;

		SFile << (DWORD)emTARG;
		SFile << (DWORD)emTARG_POSA;
		SFile << (DWORD)emTARG_POSB;
		SFile << fDELAY4DAMAGE;

		SFile << strHOLDOUT;

		SFile << (DWORD)emSELFZONE01;
		SFile << (DWORD)emSELFZONE01_POS;
		for ( int i=0; i<EMELEMENT_MAXNUM; ++i )
			SFile << strSELFZONE01[i];

		SFile << (DWORD)emSELFZONE02;
		SFile << (DWORD)emSELFZONE02_POS;
		for ( int i=0; i<EMELEMENT_MAXNUM; ++i )
			SFile << strSELFZONE02[i];

		SFile << (DWORD)emSELFZONE03;
		SFile << (DWORD)emSELFZONE03_POS;
		for( int i = 0; i < EMELEMENT_MAXNUM; ++i )
			SFile << strSELFZONE03[i];

		SFile << bTARG_ONE;
		for ( int i=0; i<EMELEMENT_MAXNUM; ++i )
			SFile << strTARG[i];

		SFile << (DWORD)emTARGZONE01;
		SFile << (DWORD)emTARGZONE01_POS;
		for ( int i=0; i<EMELEMENT_MAXNUM; ++i )
			SFile << strTARGZONE01[i];

		SFile << (DWORD)emTARGZONE02;
		SFile << (DWORD)emTARGZONE02_POS;
		for ( int i=0; i<EMELEMENT_MAXNUM; ++i )
			SFile << strTARGZONE02[i];

		SFile << (DWORD)emTARGZONE03;
		SFile << (DWORD)emTARGZONE03_POS;
		for( int i = 0; i < EMELEMENT_MAXNUM; ++i )
			SFile << strTARGZONE03[i];

		SFile << (DWORD)emTARGBODY01;
		for ( int i=0; i<EMELEMENT_MAXNUM; ++i )
			SFile << strTARGBODY01[i];

		SFile << (DWORD)emTARGBODY02;
		for ( int i=0; i<EMELEMENT_MAXNUM; ++i )
			SFile << strTARGBODY02[i];		

		SFile << (DWORD)emSELFBODY;
		for ( int i=0; i<EMELEMENT_MAXNUM; ++i )
			SFile << strSELFBODY[i];

		SFile << idMultiTargetSkill.dwID;
		SFile << nMultiTatgetEffectFrame;	

		SFile << idDisguiseSkill.dwID;

		return TRUE;
	} //SEXT_DATA_109::SAVE


	//////////////////////////////////////////////////////////////////////////
	VOID SEXT_DATA_112::SaveCsvHead ( std::fstream &SFile )
	{
		SFile << "emANIMTYPE" << ",";
		SFile << "emANISTYPE" << ",";
		SFile << "emANIVehicleMTYPE" << ",";

		SFile << "fDELAY4DAMAGE" << ",";
		SFile << "emTARG" << ",";
		SFile << "emTARG_POSA" << ",";
		SFile << "emTARG_POSB" << ",";
		SFile << "bTARG_ONE" << ",";

		int i = 0;
		for( i=0; i<EMELEMENT_MAXNUM; ++i )
			SFile << "strTARG " << i+1 << ",";

		SFile << "emSELFZONE01" << ",";
		SFile << "emSELFZONE01_POS" << ",";
		for( i=0; i<EMELEMENT_MAXNUM; ++i )
			SFile << "strSELFZONE01 " << i+1 << ",";

		SFile << "emSELFZONE02" << ",";
		SFile << "emSELFZONE02_POS" << ",";
		for( i=0; i<EMELEMENT_MAXNUM; ++i )
			SFile << "strSELFZONE02 " << i+1 << ",";

		SFile << "emSELFZONE03" << ",";
		SFile << "emSELFZONE03_POS" << ",";
		for( i=0; i<EMELEMENT_MAXNUM; ++i )
			SFile << "strSELFZONE03 " << i+1 << ",";

		SFile << "emTARGZONE01" << ",";
		SFile << "emTARGZONE01_POS" << ",";
		for( i=0; i<EMELEMENT_MAXNUM; ++i )
			SFile << "strTARGZONE01 " << i+1 << ",";

		SFile << "emTARGZONE02" << ",";
		SFile << "emTARGZONE02_POS" << ",";
		for( i=0; i<EMELEMENT_MAXNUM; ++i )
			SFile << "strTARGZONE02 " << i+1 << ",";

		SFile << "emTARGZONE03" << ",";
		SFile << "emTARGZONE03_POS" << ",";
		for( i=0; i<EMELEMENT_MAXNUM; ++i )
			SFile << "strTARGZONE03 " << i+1 << ",";

		SFile << "emSELFBODY" << ",";
		for( i=0; i<EMELEMENT_MAXNUM; ++i )
			SFile << "strSELFBODY " << i+1 << ",";

		SFile << "emTARGBODY01" << ",";
		for( i=0; i<EMELEMENT_MAXNUM; ++i )
			SFile << "strTARGBODY01 " << i+1 << ",";

		SFile << "emTARGBODY02" << ",";
		for( i=0; i<EMELEMENT_MAXNUM; ++i )
			SFile << "strTARGBODY02 " << i+1 << ",";

		SFile << "strHOLDOUT" << ",";

		//SFile << "sICONINDEX wMainID" << ",";
		//SFile << "sICONINDEX wSubID" << ",";
		SFile << "strICONFILE" << ",";

		SFile << "strCOMMENTS" << ",";

		SFile << "idMultiTargetSkill wMainID" << ",";
		SFile << "idMultiTargetSkill wSubID" << ",";

		SFile << "nMultiTatgetEffectFrame" << ",";

		SFile << "idDisguise wMainID" << ",";
		SFile << "idDisguise wSubID" << ",";
	}

	VOID SEXT_DATA_112::SaveCsv ( std::fstream &SFile )
	{
		SFile << emANIMTYPE << ",";
		SFile << emANISTYPE << ",";
		SFile << emANIVehicleMTYPE << ",";

		SFile << fDELAY4DAMAGE << ",";
		SFile << emTARG << ",";
		SFile << emTARG_POSA << ",";
		SFile << emTARG_POSB << ",";
		SFile << bTARG_ONE << ",";

		int i = 0;
		for( i=0; i<EMELEMENT_MAXNUM; ++i )
			STRUTIL::OutputStrCsv( SFile, strTARG[i] );

		SFile << emSELFZONE01 << ",";
		SFile << emSELFZONE01_POS << ",";
		for( i=0; i<EMELEMENT_MAXNUM; ++i )
			STRUTIL::OutputStrCsv( SFile, strSELFZONE01[i] );

		SFile << emSELFZONE02 << ",";
		SFile << emSELFZONE02_POS << ",";
		for( i=0; i<EMELEMENT_MAXNUM; ++i )
			STRUTIL::OutputStrCsv( SFile, strSELFZONE02[i] );

		SFile << emSELFZONE03 << ",";
		SFile << emSELFZONE03_POS << ",";
		for( i=0; i<EMELEMENT_MAXNUM; ++i )
			STRUTIL::OutputStrCsv( SFile, strSELFZONE03[i] );

		SFile << emTARGZONE01 << ",";
		SFile << emTARGZONE01_POS << ",";
		for( i=0; i<EMELEMENT_MAXNUM; ++i )
			STRUTIL::OutputStrCsv( SFile, strTARGZONE01[i] );

		SFile << emTARGZONE02 << ",";
		SFile << emTARGZONE02_POS << ",";
		for( i=0; i<EMELEMENT_MAXNUM; ++i )
			STRUTIL::OutputStrCsv( SFile, strTARGZONE02[i] );

		SFile << emTARGZONE03 << ",";
		SFile << emTARGZONE03_POS << ",";
		for( i=0; i<EMELEMENT_MAXNUM; ++i )
			STRUTIL::OutputStrCsv( SFile, strTARGZONE03[i] );

		SFile << emSELFBODY << ",";
		for( i=0; i<EMELEMENT_MAXNUM; ++i )
			STRUTIL::OutputStrCsv( SFile, strSELFBODY[i] );

		SFile << emTARGBODY01 << ",";
		for( i=0; i<EMELEMENT_MAXNUM; ++i )
			STRUTIL::OutputStrCsv( SFile, strTARGBODY01[i] );

		SFile << emTARGBODY02 << ",";
		for( i=0; i<EMELEMENT_MAXNUM; ++i )
			STRUTIL::OutputStrCsv( SFile, strTARGBODY02[i] );

		STRUTIL::OutputStrCsv( SFile, strHOLDOUT );

		//SFile << sICONINDEX.wMainID << ",";
		//SFile << sICONINDEX.wSubID << ",";
		STRUTIL::OutputStrCsv( SFile, strICONFILE );

		STRUTIL::OutputStrCsv( SFile, strCOMMENTS );

		SFile << idMultiTargetSkill.wMainID<< ",";
		SFile << idMultiTargetSkill.wSubID << ",";

		SFile << nMultiTatgetEffectFrame << ",";

		SFile << idDisguiseSkill.wMainID << ",";
		SFile << idDisguiseSkill.wSubID  << ",";
	}

	VOID SEXT_DATA_112::LoadCsv ( CStringArray &StrArray )
	{
		emANIMTYPE = (EMANI_MAINTYPE)atoi( StrArray[ iCsvCur++ ] );
		emANISTYPE = (EMANI_SUBTYPE)atoi( StrArray[ iCsvCur++ ] );
		emANIVehicleMTYPE = (EMANI_MAINTYPE)atoi( StrArray[ iCsvCur++ ] );

		fDELAY4DAMAGE = (float)atof( StrArray[ iCsvCur++ ] );
		emTARG = (EMEFFECTIME)atoi( StrArray[ iCsvCur++ ] );
		emTARG_POSA = (EMEFFECTPOS)atoi( StrArray[ iCsvCur++ ] );
		emTARG_POSB = (EMEFFECTPOS)atoi( StrArray[ iCsvCur++ ] );
		bTARG_ONE = (bool)( atoi( StrArray[ iCsvCur++ ] ) != 0 );

		int i = 0;
		for( i=0; i<EMELEMENT_MAXNUM; ++i )
			STRUTIL::InputStrCsv( StrArray[ iCsvCur++ ], strTARG[i] );

		emSELFZONE01 = (EMEFFECTIME)atoi( StrArray[ iCsvCur++ ] );
		emSELFZONE01_POS = (EMEFFECTPOS)atoi( StrArray[ iCsvCur++ ] );
		for( i=0; i<EMELEMENT_MAXNUM; ++i )
			STRUTIL::InputStrCsv( StrArray[ iCsvCur++ ], strSELFZONE01[i] );

		emSELFZONE02 = (EMEFFECTIME)atoi( StrArray[ iCsvCur++ ] );
		emSELFZONE02_POS = (EMEFFECTPOS)atoi( StrArray[ iCsvCur++ ] );
		for( i=0; i<EMELEMENT_MAXNUM; ++i )
			STRUTIL::InputStrCsv( StrArray[ iCsvCur++ ], strSELFZONE02[i] );

		emSELFZONE03 = (EMEFFECTIME)atoi( StrArray[ iCsvCur++ ] );
		emSELFZONE03_POS = (EMEFFECTPOS)atoi( StrArray[ iCsvCur++ ] );
		for( i=0; i<EMELEMENT_MAXNUM; ++i )
			STRUTIL::InputStrCsv( StrArray[ iCsvCur++ ], strSELFZONE03[i] );

		emTARGZONE01 = (EMEFFECTIME)atoi( StrArray[ iCsvCur++ ] );
		emTARGZONE01_POS = (EMEFFECTPOS)atoi( StrArray[ iCsvCur++ ] );
		for( i=0; i<EMELEMENT_MAXNUM; ++i )
			STRUTIL::InputStrCsv( StrArray[ iCsvCur++ ], strTARGZONE01[i] );

		emTARGZONE02 = (EMEFFECTIME)atoi( StrArray[ iCsvCur++ ] );
		emTARGZONE02_POS = (EMEFFECTPOS)atoi( StrArray[ iCsvCur++ ] );
		for( i=0; i<EMELEMENT_MAXNUM; ++i )
			STRUTIL::InputStrCsv( StrArray[ iCsvCur++ ], strTARGZONE02[i] );

		emTARGZONE03 = (EMEFFECTIME)atoi( StrArray[ iCsvCur++ ] );
		emTARGZONE03_POS = (EMEFFECTPOS)atoi( StrArray[ iCsvCur++ ] );
		for( i=0; i<EMELEMENT_MAXNUM; ++i )
			STRUTIL::InputStrCsv( StrArray[ iCsvCur++ ], strTARGZONE03[i] );

		emSELFBODY = (EMEFFECTIME)atoi( StrArray[ iCsvCur++ ] );
		for( i=0; i<EMELEMENT_MAXNUM; ++i )
			STRUTIL::InputStrCsv( StrArray[ iCsvCur++ ], strSELFBODY[i] );

		emTARGBODY01 = (EMEFFECTIME)atoi( StrArray[ iCsvCur++ ] );
		for( i=0; i<EMELEMENT_MAXNUM; ++i )
			STRUTIL::InputStrCsv( StrArray[ iCsvCur++ ], strTARGBODY01[i] );

		emTARGBODY02 = (EMEFFECTIME)atoi( StrArray[ iCsvCur++ ] );
		for( i=0; i<EMELEMENT_MAXNUM; ++i )
			STRUTIL::InputStrCsv( StrArray[ iCsvCur++ ], strTARGBODY02[i] );

		STRUTIL::InputStrCsv( StrArray[ iCsvCur++ ], strHOLDOUT );

		//sICONINDEX.wMainID = (WORD)atoi( StrArray[ iCsvCur++ ] );
		//sICONINDEX.wSubID = (WORD)atoi( StrArray[ iCsvCur++ ] );
		STRUTIL::InputStrCsv( StrArray[ iCsvCur++ ], strICONFILE );

		STRUTIL::InputStrCsv( StrArray[ iCsvCur++ ], strCOMMENTS );

		idMultiTargetSkill.wMainID = (WORD)atoi( StrArray[ iCsvCur++ ] );
		idMultiTargetSkill.wSubID = (WORD)atoi( StrArray[ iCsvCur++ ] );

		nMultiTatgetEffectFrame = atoi( StrArray[ iCsvCur++ ] );

		idDisguiseSkill.wMainID = (WORD)atoi( StrArray[ iCsvCur++ ] );
		idDisguiseSkill.wSubID = (WORD)atoi( StrArray[ iCsvCur++ ] );
	}
};