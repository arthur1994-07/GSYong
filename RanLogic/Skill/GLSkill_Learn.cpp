#include "../pch.h"
#include "../../SigmaCore/String/StringUtils.h"
#include "./GLSkillDef.h"
#include "./GLSkill_LearnData.h"
#include "./GLSkill_Learn.h"

namespace SKILL
{
	SLEARN::SLEARN (void) 
		: emBRIGHT(BRIGHT_BOTH)
		, dwCLASS(GLCC_NONE)
		, sSKILL(NATIVEID_NULL())
		, bOnlyOneStats(FALSE)
		, sHiddenWeapon(NATIVEID_NULL())
		, bHiddenWeapon(FALSE)
	{
	}

	void SLEARN::Assign ( SLEARN_100 &OldData )
	{
		for ( int i=0; i<MAX_LEVEL_BEFORE; ++i )
		{
			sLVL_STEP[i].dwSKP = 1;
		}
	}

	void SLEARN::Assign ( SLEARN_101 &OldData )
	{
		dwCLASS = OldData.dwCLASS;
		emBRIGHT = OldData.emBRIGHT;
		sSKILL = OldData.sSKILL;

		for ( int i=0; i<MAX_LEVEL_BEFORE; ++i )
		{
			sLVL_STEP[i].Assign( OldData.sLVL_STEP[i] );
		}
	}

	void SLEARN::Assign ( SLEARN_102 &OldData )
	{
		dwCLASS = OldData.dwCLASS;
		emBRIGHT = OldData.emBRIGHT;
		sSKILL = OldData.sSKILL;

		for ( int i=0; i<MAX_LEVEL_BEFORE; ++i )
		{
			sLVL_STEP[i].Assign( OldData.sLVL_STEP[i] );
		}
	}

	void SLEARN::Assign ( SLEARN_103 &OldData )
	{
		dwCLASS = OldData.dwCLASS;
		emBRIGHT = OldData.emBRIGHT;
		sSKILL = OldData.sSKILL;
		bOnlyOneStats = OldData.bOnlyOneStats;
		sHiddenWeapon = OldData.sHiddenWeapon;

		for ( int i=0; i<MAX_LEVEL_BEFORE; ++i )
		{
			sLVL_STEP[i].Assign(OldData.sLVL_STEP[i]);
		}
	}

	void SLEARN::Assign ( SLEARN_104 &OldData )
	{
		dwCLASS = OldData.dwCLASS;
		emBRIGHT = OldData.emBRIGHT;
		sSKILL = OldData.sSKILL;
		bHiddenWeapon = OldData.bHiddenWeapon;
		sHiddenWeapon = OldData.sHiddenWeapon;
		bOnlyOneStats = OldData.bOnlyOneStats;

		for ( int i=0; i<MAX_LEVEL_BEFORE; ++i )
		{
			sLVL_STEP[i].Assign(OldData.sLVL_STEP[i]);
		}
	}

	VOID SLEARN::SaveCsvHead ( std::fstream &SFile )
	{
		SFile << "emBRIGHT" << ",";
		SFile << "dwCLASS" << ",";
		SFile << "sSKILL wMainID" << ",";
		SFile << "sSKILL wSubID" << ",";

		SFile << "bHiddenWeapon" << ",";
		SFile << "sHiddenWeapon wMainID" << ",";
		SFile << "sHiddenWeapon wSubID" << ",";

		SFile << "bOnlyOneStat" << ",";
		for( int i=1; i<MAX_LEVEL+1; ++i )
		{
			SFile << "sLVL_STEP " << i << " dwSKP" << ",";
			SFile << "sLVL_STEP " << i << " dwLEVEL" << ",";

			SFile << "sLVL_STEP " << i << " sSTATS wPow" << ",";
			SFile << "sLVL_STEP " << i << " sSTATS wStr" << ",";
			SFile << "sLVL_STEP " << i << " sSTATS wSpi" << ",";
			SFile << "sLVL_STEP " << i << " sSTATS wDex" << ",";
			SFile << "sLVL_STEP " << i << " sSTATS wInt" << ",";
			SFile << "sLVL_STEP " << i << " sSTATS wSta" << ",";

			SFile << "sLVL_STEP " << i << " dwSKILL_LVL" << ",";
			SFile << "sLVL_STEP " << i << " Req Money" << ",";
		}
	}

	VOID SLEARN::SaveCsv ( std::fstream &SFile )
	{
		SFile << emBRIGHT << ",";
		SFile << dwCLASS << ",";
		SFile << sSKILL.wMainID << ",";
		SFile << sSKILL.wSubID << ",";

		SFile << bHiddenWeapon << ",";
		SFile << sHiddenWeapon.wMainID << ",";
		SFile << sHiddenWeapon.wSubID << ",";

		SFile << bOnlyOneStats << ",";

		for( int i=0; i<MAX_LEVEL; ++i )
		{
			SFile << sLVL_STEP[i].dwSKP << ",";
			SFile << sLVL_STEP[i].dwLEVEL << ",";


			SFile << sLVL_STEP[i].sSTATS.wPow << ",";
			SFile << sLVL_STEP[i].sSTATS.wStr << ",";
			SFile << sLVL_STEP[i].sSTATS.wSpi << ",";
			SFile << sLVL_STEP[i].sSTATS.wDex << ",";
			SFile << sLVL_STEP[i].sSTATS.wInt << ",";
			SFile << sLVL_STEP[i].sSTATS.wSta << ",";

			SFile << sLVL_STEP[i].dwSKILL_LVL << ",";
			SFile << sLVL_STEP[i].dwReqMoney << ",";
		}
	}

	VOID SLEARN::LoadCsv ( CStringArray &StrArray )
	{
		emBRIGHT = (EMBRIGHT)atoi( StrArray[ iCsvCur++ ] );
		dwCLASS = (DWORD)atol( StrArray[ iCsvCur++ ] );
		sSKILL.wMainID = (WORD)atoi( StrArray[ iCsvCur++ ] );
		sSKILL.wSubID = (WORD)atoi( StrArray[ iCsvCur++ ] );

		bHiddenWeapon = (BOOL)atoi( StrArray[ iCsvCur++ ] );
		sHiddenWeapon.wMainID = (WORD)atoi( StrArray[ iCsvCur++ ] );
		sHiddenWeapon.wSubID = (WORD)atoi( StrArray[ iCsvCur++ ] );

		bOnlyOneStats = (BOOL)atoi( StrArray[ iCsvCur++ ] );
		for( int i=0; i<MAX_LEVEL; ++i )
		{
			sLVL_STEP[i].dwSKP = (DWORD)atol( StrArray[ iCsvCur++ ] );
			sLVL_STEP[i].dwLEVEL = (DWORD)atol( StrArray[ iCsvCur++ ] );

			sLVL_STEP[i].sSTATS.wPow = (WORD)atoi( StrArray[ iCsvCur++ ] );
			sLVL_STEP[i].sSTATS.wStr = (WORD)atoi( StrArray[ iCsvCur++ ] );
			sLVL_STEP[i].sSTATS.wSpi = (WORD)atoi( StrArray[ iCsvCur++ ] );
			sLVL_STEP[i].sSTATS.wDex = (WORD)atoi( StrArray[ iCsvCur++ ] );
			sLVL_STEP[i].sSTATS.wInt = (WORD)atoi( StrArray[ iCsvCur++ ] );
			sLVL_STEP[i].sSTATS.wSta = (WORD)atoi( StrArray[ iCsvCur++ ] );

			sLVL_STEP[i].dwSKILL_LVL = (DWORD)atol( StrArray[ iCsvCur++ ] );
			sLVL_STEP[i].dwReqMoney = (DWORD)atol( StrArray[ iCsvCur++ ] );
		}
	}
};