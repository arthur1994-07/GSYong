#ifndef _GL_ITEM_QUESTION_H_
#define _GL_ITEM_QUESTION_H_

#pragma once

#include "./GLItemDef.h"

namespace sc
{
    class BaseStream;
    class SerialFile;
}

namespace ITEM
{

struct SQUESTIONITEM // by °æ´ë
{
    enum { VERSION = 0x0101 };

    EMITEM_QUESTION	emType;
    WORD	wParam1;
    WORD	wParam2;
    float	fTime;
    float	fExp; // °æÇèÄ¡ È¹µæ 0.04~0.1%

    SQUESTIONITEM() 
        : emType(QUESTION_NONE)
        , fTime(0.0f)
        , fExp(0.0f)
        , wParam1(0)
        , wParam2(0)
    {
    }

    BOOL LOAD ( sc::BaseStream &SFile );
    BOOL SAVE ( sc::SerialFile &SFile );

    static VOID SaveCsvHead ( std::fstream &SFile );
    VOID SaveCsv ( std::fstream &SFile );
    VOID LoadCsv ( CStringArray &StrArray, int& iCsvCur );
};

} // namespace ITEM
#endif // _GL_ITEM_QUESTION_H_