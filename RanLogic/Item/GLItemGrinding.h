#ifndef _GL_ITEM_GRINDING_H_
#define _GL_ITEM_GRINDING_H_

#pragma once

#include "./GLItemDef.h"

namespace sc
{
	class BaseStream;
	class SerialFile;
}

namespace ITEM
{
    struct SGRINDING_100
    {
        SGRIND_OLD			sNO[EMGRINDING_NO];
    };

    struct SGRINDING_101
    {
        EMGRINDING_CLASS	emCLASS;
        SGRIND_OLD			sNO[EMGRINDING_NO];
    };

    struct SGRINDING_102
    {
        EMGRINDING_CLASS	emCLASS;
        bool				bHIGH;
        EMGRINDING_TYPE		emTYPE;
    };

    struct SGRINDING_103	//	데이터 포맷 변경 진행
    {
        EMGRINDING_CLASS	emCLASS;
        EMGRINDER_TYPE		emGRINDER_TYPE;
        EMGRINDING_TYPE		emTYPE;
    };

	struct SGRINDING_104
	{
		EMGRINDER_TYPE		emGRINDER_TYPE;
		EMGRINDING_TYPE		emTYPE;
		EMGRINDING_CLASS	emCLASS;
	};

    //	연마 아이템, 관련 데이터.
    struct SGRINDING // 연마 옵션파일, 데이터 추가
    {
        enum { VERSION = 0x0105 };

        EMGRINDER_TYPE		emGRINDER_TYPE;
        EMGRINDING_TYPE		emTYPE;
        EMGRINDING_CLASS	emCLASS;
		std::string			strGRINDER_OPT_FILE;

        SGRINDING ()
            : emCLASS(EMGRINDING_CLASS_ARM)
            , emGRINDER_TYPE(EMGRINDER_NORMAL)
            , emTYPE(EMGRINDING_NUNE)
        {
        }

        void Assign ( SGRINDING_100 &sOldData )
		{
			strGRINDER_OPT_FILE.clear();
        }

        void Assign ( SGRINDING_101 &sOldData )
        {
            emCLASS = sOldData.emCLASS;

            switch ( sOldData.sNO[0].emTYPE )
            {
            case 3:	emTYPE = EMGRINDING_DAMAGE;		break;
            case 4:	emTYPE = EMGRINDING_DEFENSE;	break;
            };

			strGRINDER_OPT_FILE.clear();
        }

        void Assign (  SGRINDING_102 &sOldData )
        {
            emCLASS = sOldData.emCLASS;
            emTYPE = sOldData.emTYPE;

            if ( !sOldData.bHIGH )	emGRINDER_TYPE = EMGRINDER_NORMAL;
			else emGRINDER_TYPE = EMGRINDER_HIGH;

			strGRINDER_OPT_FILE.clear();
        }

        void Assign ( SGRINDING_103 &sOldData )
        {
            emCLASS = sOldData.emCLASS;
            emTYPE = sOldData.emTYPE;
			emGRINDER_TYPE = sOldData.emGRINDER_TYPE;

			strGRINDER_OPT_FILE.clear();
        }

		void Assign ( SGRINDING_104 &sOldData )
		{
			emCLASS = sOldData.emCLASS;
			emTYPE = sOldData.emTYPE;
			emGRINDER_TYPE = sOldData.emGRINDER_TYPE;

			strGRINDER_OPT_FILE.clear();
		}

		BOOL LOAD_100(sc::BaseStream& SFile);
		BOOL LOAD_101(sc::BaseStream& SFile);
		BOOL LOAD_102(sc::BaseStream& SFile);
		BOOL LOAD_103(sc::BaseStream& SFile);
		BOOL LOAD_104(sc::BaseStream& SFile);

		BOOL LOAD(sc::BaseStream& SFile);
		BOOL SAVE(sc::SerialFile& SFile);

        static VOID SaveCsvHead ( std::fstream &SFile );
        VOID SaveCsv ( std::fstream &SFile );
        VOID LoadCsv ( CStringArray &StrArray, int& iCsvCur );
    };
} // namespace ITEM
#endif // _GL_ITEM_GRINDING_H_