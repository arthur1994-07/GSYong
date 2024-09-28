
#pragma once

#include "./GLItemDef.h"

namespace sc {
	class BaseStream;
	class SerialFile;
}

namespace ITEM
{
	struct SSPLIT
	{
	public:
		enum { VERSION = 0x100 };
		char szName[ITEM_SZNAME];

	public:
		SSPLIT(void);
		~SSPLIT(void)	{}

		static void SaveCsvHead ( std::fstream &SFile );
		void SaveCsv ( std::fstream &SFile );
		void LoadCsv ( CStringArray &StrArray, int& iCsvCur );

		BOOL LOAD(sc::BaseStream& SFile);
		BOOL SAVE(sc::BaseStream& SFile);

	private:
		BOOL Load_100(sc::BaseStream& SFile);
	};
}