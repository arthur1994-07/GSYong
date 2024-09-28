#include "../pch.h"
#include "./GLItemSkillBook.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace ITEM
{
    VOID SSKILLBOOK::SaveCsvHead ( std::fstream &SFile )
    {
        SFile << "sSkill_ID wMainID"        << ",";
        SFile << "sSkill_ID wSubID"         << ",";

        SFile << "wSkill_Level wSkillLevel" << ",";
    }

    VOID SSKILLBOOK::SaveCsv ( std::fstream &SFile )
    {
        SFile << sSkill_ID.wMainID << ",";
        SFile << sSkill_ID.wSubID << ",";

        SFile << wSkill_Level << ",";
    }

    VOID SSKILLBOOK::LoadCsv ( CStringArray &StrArray, int& iCsvCur )
    {
        sSkill_ID.wMainID = (WORD)atoi( StrArray[ iCsvCur++ ] );
        sSkill_ID.wSubID = (WORD)atoi( StrArray[ iCsvCur++ ] );

        wSkill_Level = (WORD)atoi( StrArray[ iCsvCur++ ] );
    }
} // namespace ITEM