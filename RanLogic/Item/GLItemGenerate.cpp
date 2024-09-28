#include "../pch.h"
#include "./GLItemGenerate.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace ITEM
{
    void SGENERATE::Assign(SGENERATE_100& sOld)
    {
        dwSpecID = sOld.dwSpecID;
        dwLimitTimeGen = sOld.dwLimitTimeGen;
        dwLimitTime = sOld.dwLimitTime;
    }

    VOID SGENERATE::SaveCsvHead(std::fstream& SFile)
    {
        SFile << "dwSpecID" << ",";
        SFile << "dwLimitTime" << ",";
        SFile << "dwLimitTimeGen" << ",";
    }

    VOID SGENERATE::SaveCsv(std::fstream& SFile)
    {
        SFile << dwSpecID << ",";
        SFile << dwLimitTime << ",";
        SFile << dwLimitTimeGen << ",";
    }

    VOID SGENERATE::LoadCsv(CStringArray &StrArray, int& iCsvCur)
    {
        dwSpecID = (DWORD)atol( StrArray[ iCsvCur++ ] );
        dwLimitTime = (DWORD)atol( StrArray[ iCsvCur++ ] );
        dwLimitTimeGen = (DWORD)atol( StrArray[ iCsvCur++ ] );
    }


} // namespace ITEM