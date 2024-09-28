#ifndef _GL_ITEM_RANDOM_BOX_H_
#define _GL_ITEM_RANDOM_BOX_H_

#pragma once

#include "../../EngineLib/G-Logic/TypeDefine.h"

namespace sc
{
    class BaseStream;
    class SerialFile;
} // namespace sc

namespace ITEM
{

struct SRANDOMITEM_100
{
    float		fRATE;
    SNATIVEID	nidITEM;

    SRANDOMITEM_100()
        : fRATE(0)
    {
    }
};

struct SRANDOMITEM	//	데이터 포맷 변경 진행
{
    SNATIVEID	nidITEM;
    float		fRATE;

    SRANDOMITEM()
        : fRATE(0)
    {
    }

    void Assign ( SRANDOMITEM_100& sOldData )
    {
        nidITEM = sOldData.nidITEM;
        fRATE = sOldData.fRATE;	
    }
};

typedef std::vector<SRANDOMITEM> VEC_RANDOMBOX;

struct SRANDOMBOX
{
    enum { VERSION = 0x0101 };

    VEC_RANDOMBOX	vecBOX;

    BOOL LOAD ( sc::BaseStream &SFile );
    BOOL SAVE ( sc::SerialFile &SFile );

    static VOID SaveCsvHead ( std::fstream &SFile );
    VOID SaveCsv ( std::fstream &SFile );
    VOID LoadCsv ( CStringArray &StrArray, int& iCsvCur );

    void CLEAR ();
    bool VALID () const;

    bool INSERT ( const SNATIVEID &nidITEM, float fRATE );
    bool DEL ( int nIndex );
};

} // namespace ITEM
#endif // _GL_ITEM_RANDOM_BOX_H_