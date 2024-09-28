#ifndef _GL_ITEM_SKILL_BOOK_H_
#define _GL_ITEM_SKILL_BOOK_H_

#pragma once

#include "../../EngineLib/G-Logic/TypeDefine.h"

namespace ITEM
{

    //	Skill 서적, 관련 데이터.
    struct SSKILLBOOK_101
    {
        SNATIVEID	sSkill_ID;		//	습득 Skill.

        SSKILLBOOK_101 () : sSkill_ID(NATIVEID_NULL()) {}
    };

    struct SSKILLBOOK	//	데이터 포맷 변경 진
    {
        enum { VERSION = 0x0102 };

        SNATIVEID	sSkill_ID;		//	습득 Skill.
        WORD        wSkill_Level;   //  Skill 레벨.

        SSKILLBOOK ()
            : sSkill_ID(NATIVEID_NULL()), wSkill_Level(0)
        {
        }

        static VOID SaveCsvHead ( std::fstream &SFile );
        VOID SaveCsv ( std::fstream &SFile );
        VOID LoadCsv ( CStringArray &StrArray, int& iCsvCur );
    };

} // namespace ITEM
#endif // _GL_ITEM_SKILL_BOOK_H_