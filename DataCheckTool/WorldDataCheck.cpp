#include "StdAfx.h"


#include "../EngineLib/G-Logic/GLogic.h"
#include "../Enginelib/Common/SUBPATH.h"

#include "../RanLogic/GLStringTable.h"

#include "../MfcExLib/ExLibs/EtcFunction.h"

#include "../EngineLib/DxLand/DxLandMan.h"
#include "../EngineLib/DxMeshs/FrameMesh/DxFrameMesh.h"
#include "../EngineLib/DxMeshs/FrameMesh/DxReplaceContainer.h"

#include ".\worlddatacheck.h"

const char	CWorldDataCheck::FILEMARK[128] = "LAND.MAN";

CWorldDataCheck::CWorldDataCheck(void)
{
}

CWorldDataCheck::~CWorldDataCheck(void)
{
}

BOOL CWorldDataCheck::LoadFile ( const char *szFile )
{
	sc::SerialFile sFileMap;
	char szFileType[FILETYPESIZE] = "";
	char szTempChar[256] = {0};
	DWORD dwVersion;

	if( !sFileMap.OpenFile ( FOT_READ, szFile ) ) return FALSE;

	//	Note : 파일 형식 버전확인.
	//
	sFileMap.GetFileType( szFileType, FILETYPESIZE, dwVersion );

	if ( strcmp(FILEMARK,szFileType) )
	{
		CString Str;
		Str.Format ( "[%s]  맵 파일 형식이 아닙니다.", szFile );
		MessageBox ( NULL, Str.GetString(), "ERROR", MB_OK );

		return FALSE;
	}

	if ( dwVersion>=0x0108 && dwVersion<=DxLandMan::VERSION )	{}
	else
	{
		CString Str;
		Str.Format ( "[%s]  Error Load Map File!!!", szFile );
		MessageBox ( NULL, Str.GetString(), "ERROR", MB_OK );

		return FALSE;
	}

	if ( dwVersion == 0x0108 )
	{
		return FALSE;
	}
	if ( dwVersion == 0x0109 )
	{
		return FALSE;
	}
	if ( dwVersion == 0x0110 )
	{
		return FALSE;
	}
	if( (dwVersion==0x0112) || (dwVersion==0x0111) )
	{
		return FALSE;
	}

	DWORD dwMapID;
	sFileMap >> dwMapID;
	sFileMap.ReadBuffer ( szTempChar, sizeof(char)*MAXLANDNAME );
	

	return TRUE;
}