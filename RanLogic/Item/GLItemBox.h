#ifndef _GL_ITEM_BOX_H_
#define _GL_ITEM_BOX_H_

#pragma once

#include "../../EngineLib/G-Logic/TypeDefine.h"

namespace sc 
{
    class BaseStream;
    class SerialFile;
}

namespace ITEM
{

struct SBOX_ITEM_101
{
    SNATIVEID	nidITEM;
    DWORD		dwAMOUNT;

    SBOX_ITEM_101 ()
        : nidITEM(false)
        , dwAMOUNT(1)
    {
    }
};

struct SBOX_ITEM	//	데이터 포맷 변경 진행
{
    DWORD		dwAMOUNT;
    SNATIVEID	nidITEM;

    SBOX_ITEM ()
        : nidITEM(false)
        , dwAMOUNT(1)
    {
    }
};
typedef std::vector<SBOX_ITEM> VEC_BOX;
typedef std::vector<SBOX_ITEM>::iterator VECBOX_ITR;
typedef std::vector<SBOX_ITEM>::const_iterator VECBOX_CITR;

// 경험치 압축기에서 생성할 캡슐 종류를 여기다 저장함
struct SBOX
{
    enum { VERSION = 0x0104, ITEM_SIZE = 30, ITEM_PREV_SIZE = 10, RANDOM_ITEM_SIZE = 30 };
	VEC_BOX		vecItems;
    bool ShowContents;

    BOOL LOAD ( sc::BaseStream &SFile );
    BOOL SAVE ( sc::SerialFile &SFile );

    static VOID SaveCsvHead ( std::fstream &SFile );
    VOID SaveCsv(std::fstream& SFile);
    VOID LoadCsv(CStringArray& StrArray, int& iCsvCur);

    SBOX ();
    void CLEAR ();
    bool VALID () const;

    bool INSERT ( const SNATIVEID &nidITEM, DWORD dwAMOUNT );
    bool DEL ( int nIndex );
	bool FIND ( const SNATIVEID &nidITEM, IN OUT int& nIndex, IN OUT int& nAmount ) const;
};

} // namespace ITEM
#endif // _GL_ITEM_BOX_H_