#include "pch.h"
#include "./DxLandDef.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void SLAND_FILEMARK::Assign ( SLAND_FILEMARK_000 &sOldMark )
{
	dwNAVI_MARK = sOldMark.dwNAVI_MARK;
	dwGATE_MARK = sOldMark.dwGATE_MARK;
	dwCOLL_MARK = sOldMark.dwCOLL_MARK;
}

void SLAND_FILEMARK::Assign ( SLAND_FILEMARK_100 &sOldMark )
{
	dwNAVI_MARK = sOldMark.dwNAVI_MARK;
	dwGATE_MARK = sOldMark.dwGATE_MARK;
	dwCOLL_MARK = sOldMark.dwCOLL_MARK;
	dwWEATHER_MARK = sOldMark.dwWEATHER_MARK;
}

BOOL SLAND_FILEMARK::LoadSet ( sc::SerialFile &SFile )
{
	DWORD dwVer, dwSize;

	SFile >> dwVer;
	SFile >> dwSize;

	switch ( dwVer ) 
	{
	case 0x0100:
		{
			GASSERT ( dwSize==sizeof(SLAND_FILEMARK_100) );
			SLAND_FILEMARK_100 sOldData;
			SFile.ReadBuffer ( &sOldData, sizeof(SLAND_FILEMARK_100) );
			Assign( sOldData );
		}
		break;
	case VERSION:
		{
			GASSERT ( dwVer==VERSION && dwSize==sizeof(SLAND_FILEMARK) );
			SFile.ReadBuffer ( this, sizeof(SLAND_FILEMARK) );
		}
		break;
	default:
		{
			SFile.SetOffSet ( SFile.GetfTell() + dwSize );
			return FALSE;			
		}
		break;
	};	

	return TRUE;
}

BOOL SLAND_FILEMARK::SaveSet ( sc::SerialFile &SFile )
{
	SFile << static_cast<DWORD> ( VERSION );
	SFile << static_cast<DWORD> ( sizeof(SLAND_FILEMARK) );

	SFile.WriteBuffer ( this, sizeof(SLAND_FILEMARK) );

	return TRUE;
}
