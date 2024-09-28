#include "../pch.h"
#include "../../SigmaCore/String/StringUtils.h"
#include "./GLSkillDef.h"
#include "./GLSkill_Basic.h"

namespace SKILL
{
//////////////////////////////////////////////////////////////////////////
	// SBasic version 0x111
	//////////////////////////////////////////////////////////////////////////
	void SBASIC::Assign ( SBASIC_100 &sOldData )
	{
		sNATIVEID				= sOldData.sNATIVEID;

		StringCchCopy ( szNAME, MAX_SZNAME, sOldData.szNAME );
		dwGRADE					= sOldData.dwGRADE;

		emROLE					= sOldData.emROLE;
		emAPPLY					= sOldData.emAPPLY;

		emIMPACT_TAR			= sOldData.emIMPACT_TAR;
		emIMPACT_REALM			= sOldData.emIMPACT_REALM;
		emIMPACT_SIDE			= sOldData.emIMPACT_SIDE;
		
		wTARRANGE				= sOldData.wTARRANGE;

		emUSE_LITEM				= assign_skill_att ( sOldData.emUSE_LITEM );
		emUSE_RITEM				= assign_skill_att ( sOldData.emUSE_RITEM );

	}

	void SBASIC::Assign ( SBASIC_101 &sOldData )
	{
		sNATIVEID				= sOldData.sNATIVEID;

		StringCchCopy ( szNAME, MAX_SZNAME, sOldData.szNAME );
		dwGRADE					= sOldData.dwGRADE;

		emROLE					= sOldData.emROLE;
		emAPPLY					= sOldData.emAPPLY;

		emIMPACT_TAR			= sOldData.emIMPACT_TAR;
		emIMPACT_REALM			= sOldData.emIMPACT_REALM;
		emIMPACT_SIDE			= sOldData.emIMPACT_SIDE;
		
		wTARRANGE				= sOldData.wTARRANGE;

		emUSE_LITEM				= assign_skill_att ( sOldData.emUSE_LITEM );
		emUSE_RITEM				= assign_skill_att ( sOldData.emUSE_RITEM );

	}

	void SBASIC::Assign ( SBASIC_102 &sOldData )
	{
		sNATIVEID				= sOldData.sNATIVEID;

		StringCchCopy ( szNAME, MAX_SZNAME, sOldData.szNAME );
		dwGRADE					= sOldData.dwGRADE;

		emROLE					= sOldData.emROLE;
		emAPPLY					= sOldData.emAPPLY;

		emIMPACT_TAR			= sOldData.emIMPACT_TAR;
		emIMPACT_REALM			= sOldData.emIMPACT_REALM;
		emIMPACT_SIDE			= sOldData.emIMPACT_SIDE;
		
		wTARRANGE				= sOldData.wTARRANGE;

		emUSE_LITEM				= assign_skill_att ( sOldData.emUSE_LITEM );
		emUSE_RITEM				= assign_skill_att ( sOldData.emUSE_RITEM );
	}

	void SBASIC::Assign ( SBASIC_103 &sOldData )
	{
		sNATIVEID				= sOldData.sNATIVEID;

		StringCchCopy ( szNAME, MAX_SZNAME, sOldData.szNAME );
		dwGRADE					= sOldData.dwGRADE;

		emROLE					= sOldData.emROLE;
		emAPPLY					= sOldData.emAPPLY;

		emIMPACT_TAR			= sOldData.emIMPACT_TAR;
		emIMPACT_REALM			= sOldData.emIMPACT_REALM;
		emIMPACT_SIDE			= sOldData.emIMPACT_SIDE;
		
		wTARRANGE				= sOldData.wTARRANGE;

		emUSE_LITEM				= assign_skill_att ( sOldData.emUSE_LITEM );
		emUSE_RITEM				= assign_skill_att ( sOldData.emUSE_RITEM );

	}
	void SBASIC::Assign ( SBASIC_105 &sOldData )
	{
		sNATIVEID				= sOldData.sNATIVEID;

		StringCchCopy ( szNAME, MAX_SZNAME, sOldData.szNAME );
		dwGRADE					= sOldData.dwGRADE;
		dwMAXLEVEL				= sOldData.dwMAXLEVEL;

		emROLE					= sOldData.emROLE;
		emAPPLY					= sOldData.emAPPLY;

		emIMPACT_TAR			= sOldData.emIMPACT_TAR;
		emIMPACT_REALM			= sOldData.emIMPACT_REALM;
		emIMPACT_SIDE			= sOldData.emIMPACT_SIDE;

		wTARRANGE				= sOldData.wTARRANGE;

		emUSE_LITEM				= assign_skill_att ( sOldData.emUSE_LITEM );
		emUSE_RITEM				= assign_skill_att ( sOldData.emUSE_RITEM );

	}

	void SBASIC::Assign ( SBASIC_106 &sOldData )
	{
		sNATIVEID				= sOldData.sNATIVEID;

		StringCchCopy ( szNAME, MAX_SZNAME, sOldData.szNAME );
		dwGRADE					= sOldData.dwGRADE;
		dwMAXLEVEL				= sOldData.dwMAXLEVEL;

		emROLE					= sOldData.emROLE;
		emAPPLY					= sOldData.emAPPLY;

		emIMPACT_TAR			= sOldData.emIMPACT_TAR;
		emIMPACT_REALM			= sOldData.emIMPACT_REALM;
		emIMPACT_SIDE			= sOldData.emIMPACT_SIDE;

		wTARRANGE				= sOldData.wTARRANGE;

		emUSE_LITEM				= assign_skill_att ( sOldData.emUSE_LITEM );
		emUSE_RITEM				= assign_skill_att ( sOldData.emUSE_RITEM );

		bLearnView				= sOldData.bLearnView;
	}

	void SBASIC::Assign ( SBASIC_107 &sOldData )
	{
		sNATIVEID				= sOldData.sNATIVEID;
		bLearnView				= sOldData.bLearnView;

		StringCchCopy ( szNAME, MAX_SZNAME, sOldData.szNAME );
		
		dwMAXLEVEL				= sOldData.dwMAXLEVEL;
		dwGRADE					= sOldData.dwGRADE;

		emAPPLY					= sOldData.emAPPLY;
		emROLE					= sOldData.emROLE;

		wTARRANGE				= sOldData.wTARRANGE;

		emIMPACT_SIDE			= sOldData.emIMPACT_SIDE;
		emIMPACT_TAR			= sOldData.emIMPACT_TAR;
		emIMPACT_REALM			= sOldData.emIMPACT_REALM;

		emUSE_LITEM				= assign_skill_att ( sOldData.emUSE_LITEM );
		emUSE_RITEM				= assign_skill_att ( sOldData.emUSE_RITEM );
	}

	void SBASIC::Assign ( SBASIC_108 &sOldData )
	{
		sNATIVEID				= sOldData.sNATIVEID;
		bLearnView				= sOldData.bLearnView;

		StringCchCopy ( szNAME, MAX_SZNAME, sOldData.szNAME );
		
		dwMAXLEVEL				= sOldData.dwMAXLEVEL;
		dwGRADE					= sOldData.dwGRADE;

		emAPPLY					= sOldData.emAPPLY;
		emROLE					= sOldData.emROLE;
		emACTION_TYPE			= sOldData.emACTION_TYPE;

		wTARRANGE				= sOldData.wTARRANGE;

		emIMPACT_SIDE			= sOldData.emIMPACT_SIDE;
		emIMPACT_TAR			= sOldData.emIMPACT_TAR;
		emIMPACT_REALM			= sOldData.emIMPACT_REALM;

		emUSE_LITEM				= assign_skill_att( sOldData.emUSE_LITEM );
		emUSE_RITEM				= assign_skill_att( sOldData.emUSE_RITEM );
	}

	void SBASIC::Assign ( SBASIC_109 &sOldData )
	{
		sNATIVEID				= sOldData.sNATIVEID;
		bLearnView				= sOldData.bLearnView;

		StringCchCopy ( szNAME, MAX_SZNAME, sOldData.szNAME );

		dwMAXLEVEL				= sOldData.dwMAXLEVEL;
		dwGRADE					= sOldData.dwGRADE;

		emAPPLY					= sOldData.emAPPLY;
		emROLE					= sOldData.emROLE;
		emACTION_TYPE			= sOldData.emACTION_TYPE;

		wTARRANGE				= sOldData.wTARRANGE;

		emIMPACT_SIDE			= sOldData.emIMPACT_SIDE;
		emIMPACT_TAR			= sOldData.emIMPACT_TAR;
		emIMPACT_REALM			= sOldData.emIMPACT_REALM;

		emUSE_LITEM				= assign_skill_att( sOldData.emUSE_LITEM );
		emUSE_RITEM				= assign_skill_att( sOldData.emUSE_RITEM );
	}

    void SBASIC::Assign ( SBASIC_110& sOldData )
    {
        sNATIVEID				= sOldData.sNATIVEID;
        bLearnView				= sOldData.bLearnView;

        StringCchCopy ( szNAME, MAX_SZNAME, sOldData.szNAME );

        dwMAXLEVEL				= sOldData.dwMAXLEVEL;
        dwGRADE					= sOldData.dwGRADE;

        emAPPLY					= sOldData.emAPPLY;
        emROLE					= sOldData.emROLE;
        emACTION_TYPE			= sOldData.emACTION_TYPE;

        wTARRANGE				= sOldData.wTARRANGE;

        emIMPACT_SIDE			= sOldData.emIMPACT_SIDE;
        emIMPACT_TAR			= sOldData.emIMPACT_TAR;
        emIMPACT_REALM			= sOldData.emIMPACT_REALM;

        emUSE_LITEM				= assign_skill_att( sOldData.emUSE_LITEM );
        emUSE_RITEM				= assign_skill_att( sOldData.emUSE_RITEM );
    }

	void SBASIC::Assign ( SBASIC_111& sOldData )
	{
		sNATIVEID				= sOldData.sNATIVEID;
		bLearnView				= sOldData.bLearnView;

		StringCchCopy ( szNAME, MAX_SZNAME, sOldData.szNAME );

		dwMAXLEVEL			= sOldData.dwMAXLEVEL;
		dwGRADE				= sOldData.dwGRADE;

		emAPPLY				= sOldData.emAPPLY;
		emROLE					= sOldData.emROLE;
		emACTION_TYPE		= sOldData.emACTION_TYPE;

		wTARRANGE			= sOldData.wTARRANGE;

		emIMPACT_SIDE		= sOldData.emIMPACT_SIDE;
		emIMPACT_TAR		= sOldData.emIMPACT_TAR;
		emIMPACT_REALM	= sOldData.emIMPACT_REALM;

		emUSE_LITEM			= sOldData.emUSE_LITEM;
		emUSE_RITEM			= sOldData.emUSE_RITEM;

		dwFlags					= 0;
	}

	void SBASIC::Assign ( SBASIC_112& sOldData )
	{
		sNATIVEID				= sOldData.sNATIVEID;
		bLearnView				= sOldData.bLearnView;

		StringCchCopy ( szNAME, MAX_SZNAME, sOldData.szNAME );

		dwMAXLEVEL			= sOldData.dwMAXLEVEL;
		dwGRADE				= sOldData.dwGRADE;

		emAPPLY				= sOldData.emAPPLY;
		emROLE					= sOldData.emROLE;
		emACTION_TYPE		= sOldData.emACTION_TYPE;

		wTARRANGE			= sOldData.wTARRANGE;

		emIMPACT_SIDE		= sOldData.emIMPACT_SIDE;
		emIMPACT_TAR		= sOldData.emIMPACT_TAR;
		emIMPACT_REALM	= sOldData.emIMPACT_REALM;

		emUSE_LITEM			= assign_skill_att( sOldData.emUSE_LITEM );
		emUSE_RITEM			= assign_skill_att( sOldData.emUSE_RITEM );

		dwFlags				= sOldData.dwFlags;
	}

	void SBASIC::Assign ( SBASIC_113& sOldData )
	{
		sNATIVEID				= sOldData.sNATIVEID;
		bLearnView				= sOldData.bLearnView;

		StringCchCopy ( szNAME, MAX_SZNAME, sOldData.szNAME );

		dwMAXLEVEL			= sOldData.dwMAXLEVEL;
		dwGRADE				= sOldData.dwGRADE;

		emAPPLY				= sOldData.emAPPLY;
		emROLE					= sOldData.emROLE;
		emACTION_TYPE		= sOldData.emACTION_TYPE;

		wTARRANGE			= sOldData.wTARRANGE;

		emIMPACT_SIDE		= sOldData.emIMPACT_SIDE;
		emIMPACT_TAR		= sOldData.emIMPACT_TAR;
		emIMPACT_REALM	= sOldData.emIMPACT_REALM;

		emUSE_LITEM			= assign_skill_att( sOldData.emUSE_LITEM );
		emUSE_RITEM			= assign_skill_att( sOldData.emUSE_RITEM );

		dwFlags				= sOldData.dwFlags;
	}

	void SBASIC::Assign ( SBASIC_114& sOldData )
	{
		sNATIVEID				= sOldData.sNATIVEID;
		bLearnView				= sOldData.bLearnView;

		StringCchCopy ( szNAME, MAX_SZNAME, sOldData.szNAME );

		dwMAXLEVEL			= sOldData.dwMAXLEVEL;
		dwGRADE				= sOldData.dwGRADE;

		emAPPLY				= sOldData.emAPPLY;
		emROLE					= sOldData.emROLE;
		emACTION_TYPE		= sOldData.emACTION_TYPE;

		wTARRANGE			= sOldData.wTARRANGE;

		emIMPACT_SIDE		= sOldData.emIMPACT_SIDE;
		emIMPACT_TAR		= sOldData.emIMPACT_TAR;
		emIMPACT_REALM	= sOldData.emIMPACT_REALM;

		emUSE_LITEM			= assign_skill_att( sOldData.emUSE_LITEM );
		emUSE_RITEM			= assign_skill_att( sOldData.emUSE_RITEM );

		dwFlags				= sOldData.dwFlags;
	}

	VOID SBASIC::SaveCsvHead ( std::fstream &SFile )
	{
		SFile << "sNATIVEID wMainID" << ",";
		SFile << "sNATIVEID wSubID" << ",";

		SFile << "szNAME" << ",";

//		±âÈ¹ÆÀ ¿äÃ»À¸·Î Á¦°ÅÇÔ
//		SFile << "szNAME" << ",";

		SFile << "dwGRADE" << ",";
		SFile << "dwMAXLEVEL" << ",";

		SFile << "emROLE" << ",";
		SFile << "emAPPLY" << ",";

		SFile << "emACTION_TYPE" << ",";


		SFile << "emIMPACT_TAR" << ",";
		SFile << "emIMPACT_REALM" << ",";
		SFile << "emIMPACT_SIDE" << ",";

		SFile << "wTARRANGE" << ",";

		SFile << "emUSE_LITEM" << ",";
		SFile << "emUSE_RITEM" << ",";

		SFile << "bLearnView" << ",";
		SFile << "bMobEffectRate" << ",";

		SFile << "dwFlags" << ",";
	}

	VOID SBASIC::SaveCsv ( std::fstream &SFile )
	{
		SFile << sNATIVEID.wMainID << ",";
		SFile << sNATIVEID.wSubID << ",";

		std::string str = szNAME;
		STRUTIL::OutputStrCsv( SFile, str );

//		±âÈ¹ÆÀ ¿äÃ»À¸·Î Á¦°ÅÇÔ
/*
		if( szNAME )
		{
			const char* szpLongName = GLStringTable::GetInstance().GetString( szNAME, GLStringTable::SKILL );
			if( szpLongName )
				str = szpLongName;
		}
		STRUTIL::OutputStrCsv( SFile, str );
*/

		SFile << dwGRADE << ",";
		SFile << dwMAXLEVEL << ",";

		SFile << emROLE << ",";
		SFile << emAPPLY << ",";
		SFile << emACTION_TYPE << ",";

		SFile << emIMPACT_TAR << ",";
		SFile << emIMPACT_REALM << ",";
		SFile << emIMPACT_SIDE << ",";

		SFile << wTARRANGE << ",";

		SFile << emUSE_LITEM << ",";
		SFile << emUSE_RITEM << ",";

		SFile << bLearnView << ",";
		SFile << bMobEffectRate << ",";

		SFile << dwFlags << ",";
	}

	VOID SBASIC::LoadCsv ( CStringArray &StrArray )
	{
		iCsvCur = 0;

		sNATIVEID.wMainID = (WORD)atoi( StrArray[ iCsvCur++ ] );
		sNATIVEID.wSubID = (WORD)atoi( StrArray[ iCsvCur++ ] );

		std::string str;
		STRUTIL::InputStrCsv( StrArray[ iCsvCur++ ], str );
		memset( szNAME, 0, sizeof(char) * MAX_SZNAME );
		StringCchCopy( szNAME, MAX_SZNAME, str.c_str() );

//		±âÈ¹ÆÀ ¿äÃ»À¸·Î Á¦°ÅÇÔ
//		++iCsvCur;	// Item Name

		dwGRADE = (DWORD)atol( StrArray[ iCsvCur++ ] );
		dwMAXLEVEL = (DWORD)atol( StrArray[ iCsvCur++ ] );

		emROLE = (EMROLE)atoi( StrArray[ iCsvCur++ ] );
		emAPPLY = (EMAPPLY)atoi( StrArray[ iCsvCur++ ] );
		emACTION_TYPE = (EMACTION_TYPE)atoi( StrArray[ iCsvCur++ ] );

		emIMPACT_TAR = (EMIMPACT_TARGET)atoi( StrArray[ iCsvCur++ ] );
		emIMPACT_REALM = (EMIMPACT_REALM)atoi( StrArray[ iCsvCur++ ] );
		emIMPACT_SIDE = (EMIMPACT_SIDE)atoi( StrArray[ iCsvCur++ ] );

		wTARRANGE = (WORD)atoi( StrArray[ iCsvCur++ ] );

		emUSE_LITEM = (GLSKILL_ATT)atoi( StrArray[ iCsvCur++ ] );
		emUSE_RITEM = (GLSKILL_ATT)atoi( StrArray[ iCsvCur++ ] );

		bLearnView = (0!=atoi( StrArray[ iCsvCur++ ] ));
		bMobEffectRate = (0!=atoi( StrArray[ iCsvCur++ ] ));

		dwFlags = DWORD(atoi( StrArray[ iCsvCur++ ] ));
	}
};