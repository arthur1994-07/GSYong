#ifndef _GL_ITEM_PET_H_
#define _GL_ITEM_PET_H_

#pragma once

#include "../../EngineLib/G-Logic/TypeDefine.h"
#include "../Pet/GLPetDefine.h"
#include "./GLItemDef.h"

namespace sc
{
    class BaseStream;
    class SerialFile;
}

namespace ITEM
{
    struct SPETSKINPACKITEMDATA
    {
        SNATIVEID sMobID; // �ٲ� �� ID
        float	  fScale; // �ٲ� ���� ũ��
        float	  fRate;  // �ٲ� Ȯ��

        SPETSKINPACKITEMDATA() :
        sMobID(NATIVEID_NULL()),
            fScale(1.0f),
            fRate(0.0f)			
        {
        }
    };

    struct SPETSKINPACKITEM	// PetData
    {
        enum { VERSION = 0x0101, SKINPACK_MAX = 10, };

        std::vector<SPETSKINPACKITEMDATA> vecPetSkinData;
        DWORD	dwPetSkinTime; // ����Ǵ� �ð�.

        SPETSKINPACKITEM() :
        dwPetSkinTime(0)
        {
        }

        BOOL LOAD ( sc::BaseStream &SFile );
        BOOL SAVE ( sc::SerialFile &SFile );

        static VOID SaveCsvHead ( std::fstream &SFile );
        VOID SaveCsv ( std::fstream &SFile );
        VOID LoadCsv ( CStringArray &StrArray );

        void CLEAR ();
        bool VALID () const;

        bool INSERT ( const SNATIVEID &sMobID, float fRate, float fScale );
        bool DEL ( int nIndex );
    };

    struct SPET	// PetData
    {
        enum { VERSION = 0x0103 };

        int			emPetType; // TYPE
        DWORD			dwPetID;   // ����ID
        ACCESSORYTYPE	emType;    // �Ǽ�����TYPE
        SNATIVEID		sPetID;    // CROWID


        SPET() 
            : dwPetID(0)
            , emType(ACCETYPESIZE)
            , emPetType(PETTYPE_NONE)
            , sPetID(NATIVEID_NULL())
        {
        }

        BOOL LOAD ( sc::BaseStream &SFile );
        BOOL SAVE ( sc::SerialFile &SFile );

        static VOID SaveCsvHead ( std::fstream &SFile );
        VOID SaveCsv ( std::fstream &SFile );
        VOID LoadCsv ( CStringArray &StrArray, int& iCsvCur );
    };

} // namespace ITEM
#endif // _GL_ITEM_PET_H_