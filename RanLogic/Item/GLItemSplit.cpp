
#include "../pch.h"
#include "../../SigmaCore/File/BaseStream.h"
#include "../../SigmaCore/File/SerialFile.h"
#include "./GLItemSplit.h"

namespace ITEM
{
	SSPLIT::SSPLIT(void)
	{
		memset(szName, 0, sizeof(char) * ITEM_SZNAME);
	}

	void SSPLIT::SaveCsvHead ( std::fstream &SFile )
	{
		SFile << "szSplitFileName" << ",";
	}

	void SSPLIT::SaveCsv ( std::fstream &SFile )
	{
		if ( strlen( szName ) < 1 )
			SFile << " " << ",";
		else
			SFile << szName << ",";
	}

	void SSPLIT::LoadCsv ( CStringArray &StrArray, int& iCsvCur )
	{
		DWORD dwSize = sizeof(char) * ITEM_SZNAME;
		memset( szName, 0, dwSize );

		if ( strlen( StrArray[ iCsvCur ] ) > 1 && StrArray[ iCsvCur ].GetAt( 0 ) != ' ' )
			StringCchCopy( szName, dwSize, StrArray[ iCsvCur ] );

		iCsvCur++;
	}

	BOOL SSPLIT::LOAD(sc::BaseStream& SFile)
	{
		DWORD dwVer(0), dwSize(0);
		SFile >> dwVer;
		SFile >> dwSize;

		switch ( dwVer )
		{
		case SSPLIT::VERSION:
			Load_100(SFile);
			break;
		}
		return TRUE;
	}

	BOOL SSPLIT::SAVE(sc::BaseStream& SFile)
	{
		SFile << (DWORD)SSPLIT::VERSION;
		SFile.BeginBlock();
		{
			SFile.WriteBuffer ( szName, ITEM_SZNAME );
		}
		SFile.EndBlock();
		return TRUE;
	}

	BOOL SSPLIT::Load_100(sc::BaseStream& SFile)
	{
		SFile.ReadBuffer( szName, ITEM_SZNAME );
		return TRUE;
	}
}