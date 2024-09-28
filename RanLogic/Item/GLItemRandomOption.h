#ifndef _GL_ITEM_RANDOM_OPTION_H_
#define _GL_ITEM_RANDOM_OPTION_H_

#pragma once

#include "./GLItemDef.h"

namespace sc
{
    class BaseStream;
    class SerialFile;
}

namespace ITEM
{
    struct SRANDOM_OPT
    {
        enum { VERSION = 0x0100 };

		
        char szNAME[RandomOption::LENGTH_NAME];
		bool bUnableRemodelCard;

        SRANDOM_OPT ()
        {
            memset(szNAME, 0, sizeof(char) * RandomOption::LENGTH_NAME);
			bUnableRemodelCard = false;
        }

        BOOL NO_VERSION_LOAD(sc::BaseStream& SFile);	// 바이너리 상에 이 구조체의 버전이 없다.
		
		BOOL LOAD(sc::BaseStream& SFile);
		BOOL LOAD_CURRENT(sc::BaseStream& SFile);
        BOOL SAVE(sc::SerialFile& SFile);

        static VOID SaveCsvHead(std::fstream& SFile);
        VOID SaveCsv(std::fstream& SFile);
        VOID LoadCsv(CStringArray& StrArray, int& iCsvCur);

	private:
		BOOL LOAD_100(sc::BaseStream& SFile);
    };

} // namespace ITEM
#endif // _GL_ITEM_RANDOM_OPTION_H_