#include "../pch.h"
#include "./GLItemGrinding.h"

#include "../../SigmaCore/File/BaseStream.h"
#include "../../SigmaCore/File/SerialFile.h"
#include "../../SigmaCore/String/StringUtils.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace ITEM
{
    VOID SGRINDING::SaveCsvHead ( std::fstream &SFile )
    {
        SFile << "emCLASS" << ",";
        SFile << "emGRINDER_TYPE" << ",";
		SFile << "emTYPE" << ",";
		SFile << "strGRINDER_OPT_FILE" << ",";
    }

    VOID SGRINDING::SaveCsv ( std::fstream &SFile )
    {
        SFile << emCLASS << ",";
        SFile << emGRINDER_TYPE << ",";
		SFile << emTYPE << ",";
		STRUTIL::OutputStrCsv( SFile, strGRINDER_OPT_FILE );
    }

    VOID SGRINDING::LoadCsv ( CStringArray &StrArray, int& iCsvCur )
    {
        emCLASS = (EMGRINDING_CLASS)atoi( StrArray[ iCsvCur++ ] );
        emGRINDER_TYPE = (EMGRINDER_TYPE)atoi( StrArray[ iCsvCur++ ] );
		emTYPE = (EMGRINDING_TYPE)atoi( StrArray[ iCsvCur++ ] );
		STRUTIL::InputStrCsv( StrArray[ iCsvCur++ ], strGRINDER_OPT_FILE );
    }

	BOOL SGRINDING::LOAD_100(sc::BaseStream& SFile)
	{
		ITEM::SGRINDING_100 sGrindingOp_Old;
		SFile.ReadBuffer ( &sGrindingOp_Old, sizeof(sGrindingOp_Old) );
		Assign ( sGrindingOp_Old );

		return TRUE;
	}

	BOOL SGRINDING::LOAD_101(sc::BaseStream& SFile)
	{
		ITEM::SGRINDING_101 sGrindingOp_Old;
		SFile.ReadBuffer ( &sGrindingOp_Old, sizeof(sGrindingOp_Old) );
		Assign ( sGrindingOp_Old );

		return TRUE;
	}
	
	BOOL SGRINDING::LOAD_102(sc::BaseStream& SFile)
	{
		ITEM::SGRINDING_102 sGrindingOp_Old;
		SFile.ReadBuffer ( &sGrindingOp_Old, sizeof(sGrindingOp_Old) );
		Assign ( sGrindingOp_Old );

		return TRUE;
	}

	BOOL SGRINDING::LOAD_103(sc::BaseStream& SFile)
	{
		ITEM::SGRINDING_103 sGrindingOp_Old;
		SFile.ReadBuffer ( &sGrindingOp_Old, sizeof(sGrindingOp_Old) );
		Assign ( sGrindingOp_Old );

		return TRUE;
	}

	BOOL SGRINDING::LOAD_104(sc::BaseStream& SFile)
	{
		ITEM::SGRINDING_104 sGrindingOp_Old;
		SFile.ReadBuffer ( &sGrindingOp_Old, sizeof(sGrindingOp_Old) );
		Assign ( sGrindingOp_Old );

		return TRUE;
	}

	BOOL SGRINDING::LOAD(sc::BaseStream& SFile)
	{
		DWORD dwVer, dwSize;
		SFile >> dwVer;
		SFile >> dwSize;

		switch(dwVer)
		{
		case 0x0100:
			{
				GASSERT(sizeof(ITEM::SGRINDING_100)==dwSize);
				LOAD_100(SFile);
			}
			break;
		case 0x0101:
			{
				GASSERT(sizeof(ITEM::SGRINDING_101)==dwSize);
				LOAD_101(SFile);
			}
			break;
		case 0x0102:
			{
				GASSERT(sizeof(ITEM::SGRINDING_102)==dwSize);
				LOAD_102(SFile);
			}
			break;
		case 0x0103:
			{
				GASSERT(sizeof(ITEM::SGRINDING_103)==dwSize);
				LOAD_103(SFile);
			}
			break;
		case 0x0104:
			{
				GASSERT(sizeof(ITEM::SGRINDING_104)==dwSize);
				LOAD_104(SFile);
			}
			break;

		case ITEM::SGRINDING::VERSION:
			{
				UINT nValue;

				SFile >> nValue;
				emGRINDER_TYPE = static_cast<EMGRINDER_TYPE>(nValue);

				SFile >> nValue ;
				emTYPE = static_cast<EMGRINDING_TYPE>(nValue);

				SFile >> nValue;
				emCLASS = static_cast<EMGRINDING_CLASS>(nValue);

				SFile >> strGRINDER_OPT_FILE;
			}
			break;
		};

		return TRUE;
	}

	BOOL SGRINDING::SAVE(sc::SerialFile& SFile)
	{
		SFile << DWORD(ITEM::SGRINDING::VERSION);
		SFile << (DWORD)sizeof(ITEM::SGRINDING);

		SFile << emGRINDER_TYPE;
		SFile << emTYPE;
		SFile << emCLASS;
		SFile << strGRINDER_OPT_FILE;

		return TRUE;
	}

} // namespace ITEM