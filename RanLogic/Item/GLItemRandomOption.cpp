#include "../pch.h"
#include "../../SigmaCore/File/BaseStream.h"
#include "../../SigmaCore/File/SerialFile.h"
#include "./GLItemRandomOption.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace ITEM
{
    BOOL SRANDOM_OPT::NO_VERSION_LOAD ( sc::BaseStream &SFile )
    {
		SFile.ReadBuffer ( szNAME, RandomOption::LENGTH_NAME );
		bUnableRemodelCard = false;
        return TRUE;
    }

	BOOL SRANDOM_OPT::LOAD( sc::BaseStream& SFile )
	{
		DWORD dwVer(0), dwSize(0);
		SFile >> dwVer;
		SFile >> dwSize;

		switch( dwVer )
		{
		case SRANDOM_OPT::VERSION:
			LOAD_CURRENT( SFile );
			break;

		default:
			// ERROR
			 MessageBox ( NULL, "SRANDOM_OPT::LOAD(), unknown data version.", "ERROR", MB_OK );
			break;
		}
		return TRUE;
	}

	BOOL SRANDOM_OPT::LOAD_CURRENT( sc::BaseStream& SFile )
	{
		SFile >> bUnableRemodelCard;
		SFile.ReadBuffer ( szNAME, RandomOption::LENGTH_NAME );
		return TRUE;
	}

    BOOL SRANDOM_OPT::SAVE ( sc::SerialFile &SFile )
    {
		SFile << (DWORD)SRANDOM_OPT::VERSION;		
		SFile.BeginBlock();
		{
			SFile << bUnableRemodelCard;
			SFile.WriteBuffer ( szNAME, RandomOption::LENGTH_NAME );
		}
		SFile.EndBlock();

        return TRUE;
    }

    VOID SRANDOM_OPT::SaveCsvHead ( std::fstream &SFile )
    {
        SFile << "szNAME" << ",";
		SFile << "UnableRandomCard" << ",";
    }

    VOID SRANDOM_OPT::SaveCsv ( std::fstream &SFile )
    {
        if( strlen( szNAME ) < 1 )
            SFile << " " << ",";
        else
            SFile << szNAME << ",";

		SFile << bUnableRemodelCard << ",";
    }

    VOID SRANDOM_OPT::LoadCsv ( CStringArray &StrArray, int& iCsvCur )
    {
        DWORD dwSize = sizeof(char) * RandomOption::LENGTH_NAME;
        memset( szNAME, 0, dwSize );

        if( strlen( StrArray[ iCsvCur ] ) > 1 && StrArray[ iCsvCur ].GetAt( 0 ) != ' ' )
            StringCchCopy( szNAME, dwSize, StrArray[ iCsvCur ] );

		iCsvCur++;

		bUnableRemodelCard = (BOOL)atoi( StrArray[ iCsvCur ] );
    }

} // namespace ITEM