
#include "../pch.h"
#include "../../SigmaCore/String/StringUtils.h"
#include "./GLSkillDef.h"
#include "./GLSkill_SpecialSkillData.h"
#include "./GLSkill_SpecialSkill.h"

namespace SKILL
{
	BOOL SSPECIAL_SKILL_102::LOAD( sc::BaseStream &SFile )
	{
		DWORD dwDATA;
		SFile >> dwDATA; 
		emSSTYPE = (EMSPECIALSKILLTYPE)dwDATA;

		SFile >> dwRemainSecond;

		for( int i = 0; i < EMTRANSFORM_NSIZE; i++ )
		{
			SFile >> strTransform_Man[i];
			SFile >> strTransform_Woman[i];
		}

		SFile >> strEffectName;

		SFile >> sCreatureID.dwID;
		SFile >> dwRecallDelaySec;
		SFile >> dwCreatureActTimeSec;

		return TRUE;
	}

	BOOL SSPECIAL_SKILL_104::LOAD( sc::BaseStream &SFile )
	{
		DWORD dwDATA;
		SFile >> dwDATA; 
		emSSTYPE = (EMSPECIALSKILLTYPE)dwDATA;

		SFile >> strEffectName;
		SFile >> dwRemainSecond;

		for( int i = 0; i < EMTRANSFORM_NSIZE; i++ )
		{
			SFile >> strTransform_Man[i];
			SFile >> strTransform_Woman[i];
		}

		for ( int i=0; i<MAX_LEVEL_BEFORE; ++i )
		{
			SFile >> sCREATURE_LVL[i].sID.wMainID;
			SFile >> sCREATURE_LVL[i].sID.wSubID;
			SFile >> sCREATURE_LVL[i].fSEC_RECALLDELAY;
			SFile >> sCREATURE_LVL[i].fSEC_ACTIVETIME;
			SFile >> sCREATURE_LVL[i].dwMAX;
			SFile >> sCREATURE_LVL[i].wUSESKILLLEVEL;
			SFile >> sCREATURE_LVL[i].dwType;
			SFile >> sCREATURE_LVL[i].bEventSkill;
		}

		return TRUE;
	}

	SSPECIAL_SKILL::SSPECIAL_SKILL() 
		: emSSTYPE(EMSSTYPE_NONE)
		, dwRemainSecond(0)
	{
		for( BYTE i = 0; i < SKILL::EMTRANSFORM_NSIZE; i++ )
		{
			strTransform_Man[i].clear();
			strTransform_Woman[i].clear();
		}
	}

	void SSPECIAL_SKILL::ResetAll()
	{
		strEffectName.clear();
		dwRemainSecond		 = 0;
		for( BYTE i = 0; i < SKILL::EMTRANSFORM_NSIZE; i++ )
		{
			strTransform_Man[i].clear();
			strTransform_Woman[i].clear();
		}

		for ( BYTE i = 0; i < SKILL::MAX_LEVEL; i++ )
		{
			sCREATURE_LVL[i].sID               = NATIVEID_NULL();
			sCREATURE_LVL[i].fSEC_RECALLDELAY  = 0.0f;
			sCREATURE_LVL[i].fSEC_ACTIVETIME   = 0.0f;
			sCREATURE_LVL[i].dwMAX             = 0;
			sCREATURE_LVL[i].wUSESKILLLEVEL    = 0;
			sCREATURE_LVL[i].dwType				= 0;
			sCREATURE_LVL[i].bEventSkill		= false;
		}
	}

	BOOL SSPECIAL_SKILL::LOAD_100( sc::BaseStream &SFile )
	{
		DWORD dwDATA;
		SFile >> dwDATA; emSSTYPE = (EMSPECIALSKILLTYPE)dwDATA;
		SFile >> strEffectName;
		SFile >> dwRemainSecond;
		for( int i = 0; i < EMTRANSFORM_NSIZE; i++ )
		{
			SFile >> strTransform_Man[i];
			SFile >> strTransform_Woman[i];
		}

		return TRUE;
	}

	BOOL SSPECIAL_SKILL::LOAD_101( sc::BaseStream &SFile )
	{
		DWORD dwDATA;
		SFile >> dwDATA; 
		emSSTYPE = (EMSPECIALSKILLTYPE)dwDATA;

		SFile >> dwRemainSecond;

		for( int i = 0; i < EMTRANSFORM_NSIZE; i++ )
		{
			SFile >> strTransform_Man[i];
			SFile >> strTransform_Woman[i];
		}

		SFile >> strEffectName;

		return TRUE;

	}

	BOOL SSPECIAL_SKILL::LOAD_102( sc::BaseStream &SFile )
	{
		SSPECIAL_SKILL_102 sSpecialSkill102;

		if ( !sSpecialSkill102.LOAD(SFile) )
		{
			return FALSE;
		}

		emSSTYPE       = sSpecialSkill102.emSSTYPE; 
		dwRemainSecond = sSpecialSkill102.dwRemainSecond;

		for( int i = 0; i < EMTRANSFORM_NSIZE; i++ )
		{
			strTransform_Man[i]   = sSpecialSkill102.strTransform_Man[i];
			strTransform_Woman[i] = sSpecialSkill102.strTransform_Woman[i];
		}

		strEffectName = sSpecialSkill102.strEffectName;

		for ( int i = 0; i < MAX_LEVEL_BEFORE; ++i )
		{
			sCREATURE_LVL[i].sID               = sSpecialSkill102.sCreatureID;
			sCREATURE_LVL[i].fSEC_RECALLDELAY  = (FLOAT)sSpecialSkill102.dwRecallDelaySec;
			sCREATURE_LVL[i].fSEC_ACTIVETIME   = (FLOAT)sSpecialSkill102.dwCreatureActTimeSec;
			sCREATURE_LVL[i].dwMAX             = 1;
			sCREATURE_LVL[i].wUSESKILLLEVEL    = 0;
		}

		return TRUE;
	}

	BOOL SSPECIAL_SKILL::LOAD_103( sc::BaseStream &SFile )
	{
		DWORD dwDATA;
		SFile >> dwDATA; 
		emSSTYPE = (EMSPECIALSKILLTYPE)dwDATA;

		SFile >> strEffectName;
		SFile >> dwRemainSecond;

		for( int i = 0; i < EMTRANSFORM_NSIZE; i++ )
		{
			SFile >> strTransform_Man[i];
			SFile >> strTransform_Woman[i];
		}

		for ( int i=0; i<MAX_LEVEL_BEFORE; ++i )
		{
			SFile >> sCREATURE_LVL[i].sID.wMainID;
			SFile >> sCREATURE_LVL[i].sID.wSubID;
			SFile >> sCREATURE_LVL[i].fSEC_RECALLDELAY;
			SFile >> sCREATURE_LVL[i].fSEC_ACTIVETIME;
			SFile >> sCREATURE_LVL[i].dwMAX;
			SFile >> sCREATURE_LVL[i].wUSESKILLLEVEL;
		}

		return TRUE;
	}

	BOOL SSPECIAL_SKILL::LOAD_104 ( sc::BaseStream &SFile )
	{
		DWORD dwDATA;
		SFile >> dwDATA; 
		emSSTYPE = (EMSPECIALSKILLTYPE)dwDATA;

		SFile >> strEffectName;
		SFile >> dwRemainSecond;

		for( int i = 0; i < EMTRANSFORM_NSIZE; i++ )
		{
			SFile >> strTransform_Man[i];
			SFile >> strTransform_Woman[i];
		}

		for ( int i=0; i<MAX_LEVEL_BEFORE; ++i )
		{
			SFile >> sCREATURE_LVL[i].sID.wMainID;
			SFile >> sCREATURE_LVL[i].sID.wSubID;
			SFile >> sCREATURE_LVL[i].fSEC_RECALLDELAY;
			SFile >> sCREATURE_LVL[i].fSEC_ACTIVETIME;
			SFile >> sCREATURE_LVL[i].dwMAX;
			SFile >> sCREATURE_LVL[i].wUSESKILLLEVEL;
			SFile >> sCREATURE_LVL[i].dwType;
			SFile >> sCREATURE_LVL[i].bEventSkill;
		}

		return TRUE;
	}

	BOOL SSPECIAL_SKILL::SAVE( sc::SerialFile &SFile )
	{
		SFile << (DWORD)emSSTYPE;
		SFile << strEffectName;
		SFile << dwRemainSecond;

		for( int i = 0; i < EMTRANSFORM_NSIZE; i++ )
		{
			SFile << strTransform_Man[i];
			SFile << strTransform_Woman[i];
		}

		for ( int i=0; i<MAX_LEVEL; ++i )
		{
			SFile << sCREATURE_LVL[i].sID.wMainID;
			SFile << sCREATURE_LVL[i].sID.wSubID;
			SFile << sCREATURE_LVL[i].fSEC_RECALLDELAY;
			SFile << sCREATURE_LVL[i].fSEC_ACTIVETIME;
			SFile << sCREATURE_LVL[i].dwMAX;
			SFile << sCREATURE_LVL[i].wUSESKILLLEVEL;
			SFile << sCREATURE_LVL[i].dwType;
			SFile << sCREATURE_LVL[i].bEventSkill;
		}

		return TRUE;
	}

	BOOL SSPECIAL_SKILL::LOAD( sc::BaseStream &SFile )
	{
		DWORD dwDATA;
		SFile >> dwDATA; 
		emSSTYPE = (EMSPECIALSKILLTYPE)dwDATA;

		SFile >> strEffectName;
		SFile >> dwRemainSecond;

		for( int i = 0; i < EMTRANSFORM_NSIZE; i++ )
		{
			SFile >> strTransform_Man[i];
			SFile >> strTransform_Woman[i];
		}

		for ( int i=0; i<MAX_LEVEL; ++i )
		{
			SFile >> sCREATURE_LVL[i].sID.wMainID;
			SFile >> sCREATURE_LVL[i].sID.wSubID;
			SFile >> sCREATURE_LVL[i].fSEC_RECALLDELAY;
			SFile >> sCREATURE_LVL[i].fSEC_ACTIVETIME;
			SFile >> sCREATURE_LVL[i].dwMAX;
			SFile >> sCREATURE_LVL[i].wUSESKILLLEVEL;
			SFile >> sCREATURE_LVL[i].dwType;
			SFile >> sCREATURE_LVL[i].bEventSkill;
		}

		return TRUE;
	}

	SSPECIAL_SKILL& SSPECIAL_SKILL::operator= ( const SSPECIAL_SKILL &value )
	{
		emSSTYPE	   = value.emSSTYPE;
		strEffectName  = value.strEffectName;
		dwRemainSecond = value.dwRemainSecond;
		for( int i = 0; i < EMTRANSFORM_NSIZE; i++ )
		{
			strTransform_Man[i]		  = value.strTransform_Man[i];
			strTransform_Woman[i]	  = value.strTransform_Woman[i];
		}

		for ( int i=0; i<MAX_LEVEL; ++i )
		{
			sCREATURE_LVL[i].sID               = value.sCREATURE_LVL[i].sID;
			sCREATURE_LVL[i].fSEC_RECALLDELAY  = value.sCREATURE_LVL[i].fSEC_RECALLDELAY;
			sCREATURE_LVL[i].fSEC_ACTIVETIME   = value.sCREATURE_LVL[i].fSEC_ACTIVETIME;
			sCREATURE_LVL[i].dwMAX             = value.sCREATURE_LVL[i].dwMAX;
			sCREATURE_LVL[i].wUSESKILLLEVEL    = value.sCREATURE_LVL[i].wUSESKILLLEVEL;
			sCREATURE_LVL[i].dwType				= value.sCREATURE_LVL[i].dwType;
			sCREATURE_LVL[i].bEventSkill		= value.sCREATURE_LVL[i].bEventSkill;
		}

		return *this;
	}

	VOID SSPECIAL_SKILL::SaveCsvHead ( std::fstream &SFile )
	{
		SFile << "Special Skill Type" << ",";
		SFile << "Special Skill Effect" << ",";
		SFile << "Effect Remain Second" << ",";

		SFile << "MAN_HEAD_File" << ",";
		SFile << "MAN_FACE_File" << ",";
		SFile << "MAN_UPPER_File" << ",";
		SFile << "MAN_LOWER_File" << ",";
		SFile << "MAN_LHAND_File" << ",";
		SFile << "MAN_RHAND_File" << ",";
		SFile << "MAN_FOOT_File" << ",";
		SFile << "MAN_HAND_File" << ",";
		SFile << "WOMAN_HEAD_File" << ",";
		SFile << "WOMAN_FACE_File" << ",";
		SFile << "WOMAN_UPPER_File" << ",";
		SFile << "WOMAN_LOWER_File" << ",";
		SFile << "WOMAN_LHAND_File" << ",";
		SFile << "WOMAN_RHAND_File" << ",";
		SFile << "WOMAN_FOOT_File" << ",";
		SFile << "WOMAN_HAND_File" << ",";

		for( int i=0; i<MAX_LEVEL; ++i )
		{
			SFile << "sCREATURE_LVL " << i+1 << " wMainID"           << ",";
			SFile << "sCREATURE_LVL " << i+1 << " wSubID"            << ",";
			SFile << "sCREATURE_LVL " << i+1 << " fSEC_RECALLDELAY"  << ",";
			SFile << "sCREATURE_LVL " << i+1 << " fSEC_ACTIVETIME"   << ",";
			SFile << "sCREATURE_LVL " << i+1 << " dwMAX"             << ",";
			SFile << "sCREATURE_LVL " << i+1 << " wUSESKILLLEVEL"    << ",";
			SFile << "sCREATURE_LVL " << i+1 << " dwType"			<< ",";
			SFile << "sCREATURE_LVL " << i+1 << " bEventSkill"		<< ",";
		}
	}

	VOID SSPECIAL_SKILL::SaveCsv ( std::fstream &SFile )
	{
		SFile << (DWORD)emSSTYPE << ",";
		STRUTIL::OutputStrCsv( SFile, strEffectName );
		SFile << dwRemainSecond << ",";
		for( int i = 0; i < EMTRANSFORM_NSIZE; i++ )
		{
			STRUTIL::OutputStrCsv( SFile, strTransform_Man[i] );
		}
		for( int i = 0; i < EMTRANSFORM_NSIZE; i++ )
		{
			STRUTIL::OutputStrCsv( SFile, strTransform_Woman[i] );
		}

		for ( int i=0; i<MAX_LEVEL; ++i )
		{
			SFile << sCREATURE_LVL[i].sID.wMainID       << ",";
			SFile << sCREATURE_LVL[i].sID.wSubID        << ",";
			SFile << sCREATURE_LVL[i].fSEC_RECALLDELAY  << ",";
			SFile << sCREATURE_LVL[i].fSEC_ACTIVETIME   << ",";
			SFile << sCREATURE_LVL[i].dwMAX             << ",";
			SFile << sCREATURE_LVL[i].wUSESKILLLEVEL    << ",";
			SFile << sCREATURE_LVL[i].dwType		    << ",";
			SFile << sCREATURE_LVL[i].bEventSkill	    << ",";
		}
	}

	VOID SSPECIAL_SKILL::LoadCsv ( CStringArray &StrArray )
	{
		emSSTYPE	   = (EMSPECIALSKILLTYPE)atoi( StrArray[ iCsvCur++ ] );
		STRUTIL::InputStrCsv( StrArray[ iCsvCur++ ], strEffectName );
		dwRemainSecond = (DWORD)atoi( StrArray[ iCsvCur++ ] );
		for( int i = 0; i < EMTRANSFORM_NSIZE; i++ )
		{
			STRUTIL::InputStrCsv( StrArray[ iCsvCur++ ], strTransform_Man[i] );
		}
		for( int i = 0; i < EMTRANSFORM_NSIZE; i++ )
		{
			STRUTIL::InputStrCsv( StrArray[ iCsvCur++ ], strTransform_Woman[i] );
		}

		for ( int i=0; i<MAX_LEVEL; ++i )
		{
			sCREATURE_LVL[i].sID.wMainID       = (WORD)atoi( StrArray[ iCsvCur++ ] );
			sCREATURE_LVL[i].sID.wSubID        = (WORD)atoi( StrArray[ iCsvCur++ ] );
			sCREATURE_LVL[i].fSEC_RECALLDELAY  = (FLOAT)atof( StrArray[ iCsvCur++ ] );
			sCREATURE_LVL[i].fSEC_ACTIVETIME   = (FLOAT)atof( StrArray[ iCsvCur++ ] );
			sCREATURE_LVL[i].dwMAX             = (DWORD)atoi( StrArray[ iCsvCur++ ] );
			sCREATURE_LVL[i].wUSESKILLLEVEL    = (WORD)atoi( StrArray[ iCsvCur++ ] );
			sCREATURE_LVL[i].dwType				= (WORD)atoi( StrArray[ iCsvCur++ ] );
			sCREATURE_LVL[i].bEventSkill		= (WORD)atoi( StrArray[ iCsvCur++ ] );
		}
	}
};