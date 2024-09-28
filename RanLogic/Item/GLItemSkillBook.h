#ifndef _GL_ITEM_SKILL_BOOK_H_
#define _GL_ITEM_SKILL_BOOK_H_

#pragma once

#include "../../EngineLib/G-Logic/TypeDefine.h"

namespace ITEM
{

    //	Skill ����, ���� ������.
    struct SSKILLBOOK_101
    {
        SNATIVEID	sSkill_ID;		//	���� Skill.

        SSKILLBOOK_101 () : sSkill_ID(NATIVEID_NULL()) {}
    };

    struct SSKILLBOOK	//	������ ���� ���� ��
    {
        enum { VERSION = 0x0102 };

        SNATIVEID	sSkill_ID;		//	���� Skill.
        WORD        wSkill_Level;   //  Skill ����.

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