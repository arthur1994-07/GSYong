#include "../pch.h"
#include "./GLItemDrug.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace ITEM
{
    void SDRUG::Assign ( SDRUG_100 &sDRUG )
    {
        bInstance = sDRUG.bInstance;
        emDrug = sDRUG.emDrug;
        wPileNum = sDRUG.wPileNum;

        bRatio = sDRUG.bRatio;

        dwCureVolume = (DWORD)sDRUG.wCureVolume;
        dwArrowNum = (DWORD)sDRUG.wArrowNum;

        dwCureDISORDER = sDRUG.dwCureDISORDER;
    }

    void SDRUG::Assign ( SDRUG_101 &sDRUG )
    {
        tTIME_LMT = sDRUG.tTIME_LMT;
        bInstance = sDRUG.bInstance;
        emDrug = sDRUG.emDrug;
        wPileNum = sDRUG.wPileNum;

        bRatio = sDRUG.bRatio;

        dwCureVolume = (DWORD)sDRUG.wCureVolume;
        dwArrowNum = (DWORD)sDRUG.wArrowNum;

        dwCureDISORDER = sDRUG.dwCureDISORDER;
    }

	void SDRUG::Assign ( SDRUG_102 &sDRUG )
	{
		tTIME_LMT = sDRUG.tTIME_LMT;
		bInstance = sDRUG.bInstance;
		emDrug = sDRUG.emDrug;
		wPileNum = sDRUG.wPileNum;

		bRatio = sDRUG.bRatio;

		dwCureVolume = (DWORD)sDRUG.wCureVolume;
		dwArrowNum = (DWORD)sDRUG.wArrowNum;

		dwCureDISORDER = sDRUG.dwCureDISORDER;
	}

	void SDRUG::Assign ( SDRUG_103 &sDRUG )
	{
		tTIME_LMT = sDRUG.tTIME_LMT;
		bInstance = sDRUG.bInstance;
		emDrug = sDRUG.emDrug;
		wPileNum = sDRUG.wPileNum;

		bRatio = sDRUG.bRatio;

		dwCureVolume = sDRUG.dwCureVolume;
		dwArrowNum = sDRUG.dwArrowNum;

		dwCureDISORDER = sDRUG.dwCureDISORDER;
	}

	void SDRUG::Assign ( SDRUG_104 &sDRUG )
	{
		tTIME_LMT = sDRUG.tTIME_LMT;
		bInstance = sDRUG.bInstance;
		emDrug = sDRUG.emDrug;
		wPileNum = sDRUG.wPileNum;

		bRatio = sDRUG.bRatio;

		dwCureVolume = sDRUG.dwCureVolume;
		dwArrowNum = sDRUG.dwArrowNum;

		dwCureDISORDER = sDRUG.dwCureDISORDER;
		tDuration      = sDRUG.tDuration;
	}

    VOID SDRUG::SaveCsvHead ( std::fstream &SFile )
    {
        SFile << "tTIME_LMT" << ",";
        SFile << "bInstance" << ",";
        SFile << "emDrug" << ",";
        SFile << "wPileNum" << ",";

        SFile << "bRatio" << ",";

        SFile << "dwCureVolume" << ",";

        SFile << "dwCureDISORDER" << ",";
		SFile << "tDuration" << ",";
		SFile << "dwERList" << ",";
    }

    VOID SDRUG::SaveCsv ( std::fstream &SFile )
    {
        SFile << tTIME_LMT << ",";
        SFile << bInstance << ",";
        SFile << emDrug << ",";
        SFile << wPileNum << ",";

        SFile << bRatio << ",";

        SFile << dwCureVolume << ",";

        SFile << dwCureDISORDER << ",";
		SFile << tDuration << ",";
		SFile << dwERList << ",";
    }

    VOID SDRUG::LoadCsv ( CStringArray &StrArray, int& iCsvCur )
    {
        tTIME_LMT = (__time64_t)_atoi64( StrArray[ iCsvCur++ ] );
        bInstance = (BOOL)atol( StrArray[ iCsvCur++ ] );
        emDrug = (EMITEM_DRUG)atoi( StrArray[ iCsvCur++ ] );
        wPileNum = (WORD)atoi( StrArray[ iCsvCur++ ] );

        bRatio = (BOOL)atol( StrArray[ iCsvCur++ ] );

        dwCureVolume = (DWORD)atoi( StrArray[ iCsvCur++ ] );

        dwCureDISORDER = (DWORD)atol( StrArray[ iCsvCur++ ] );

		tDuration = (__time64_t)_atoi64( StrArray[ iCsvCur++ ] );
		dwERList = (DWORD)atoi( StrArray[ iCsvCur++ ] );
    }


    bool SDRUG::IsTimeLimited() const
    {
		return tTIME_LMT != 0;
    }

	bool SDRUG::IsTimeDuration() const
	{
		return tDuration != 0;
	}

} // namespace ITEM