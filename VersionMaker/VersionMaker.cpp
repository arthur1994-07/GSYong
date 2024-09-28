#include <afx.h>
#include <afxstr.h>
#include <Windows.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string>

#include "VersionNo.h"

//////////////////////////////////////////////////////////////////////////
// 버전 표기 방법 변경
// 기존 17.1.16152.1043
// 기존 메이저(1.7인데 17로 표기).마이너(릴리즈).시간변수(년<16>그해의몇번째날<152>).월일
// 변경 1.7.1.1 -> 1.7R1 1번째 빌드된 버전
// 변경 메이저.마이너.관리번호.빌드번호
//////////////////////////////////////////////////////////////////////////

// 1 7 1 => 1.7R1
#define VERSION_MAJOR	1		// ex)	1
#define VERSION_MINOR	0		// ex)	7	
#define VERSION_MAINTENANCE 0	// ex)	1

#pragma comment( linker, "/defaultlib:version.lib" )

std::string GetFileVersion( const std::string& strPath )
{
	std::string strReturn = "1.0.0.0";
	CString strVersion;
	HMODULE hModule = LoadLibraryA( strPath.c_str() );
	HRSRC hRsrc = FindResource( hModule, MAKEINTRESOURCE( VS_VERSION_INFO ), RT_VERSION );
	if ( hRsrc != NULL )
	{
		HGLOBAL hGlobalMemory = LoadResource( hModule, hRsrc );
		if ( hGlobalMemory != NULL )
		{
			CString rVersion;
			LPVOID pVersionResouece = LockResource( hGlobalMemory );
			LPVOID pVersion = NULL;
			DWORD uLength,langD;
			BOOL retVal;
			LPCTSTR lpSubBlock = _T( "\\VarFileInfo\\Translation" );

			retVal = VerQueryValue( pVersionResouece, lpSubBlock, ( LPVOID* )&pVersion, ( UINT* )&uLength );
			if ( retVal && uLength == 4 )
			{
				memcpy( &langD, pVersion, 4 ); 
				// ProductVersion <-> FileVersion
				rVersion.Format( _T( "\\StringFileInfo\\%02X%02X%02X%02X\\FileVersion" ), 
					( langD & 0xff00 ) >> 8, langD & 0xff,( langD & 0xff000000 ) >> 24, 
					( langD & 0xff0000 ) >> 16 ); 
			}
			else
			{
				// ProductVersion <-> FileVersion
				rVersion.Format( _T( "\\StringFileInfo\\%04X04B0\\FileVersion" ), GetUserDefaultLangID() );
			}
			if ( VerQueryValue( pVersionResouece, ( LPCTSTR )rVersion, ( LPVOID* )&pVersion, ( UINT* )&uLength ) != 0 )
			{
				strVersion.Format( _T( "%s" ), ( LPCTSTR )pVersion );
			}
		}
		FreeResource( hGlobalMemory );

		strVersion.Replace( _T(" "), NULL );
		strVersion.Replace( _T(","), _T(".") );
		CT2CA pszstrVersion( strVersion );
		strReturn = pszstrVersion;
	}
	return strReturn;
}

int main( int argc, char* argv[], char** )
{
	// jenkins 에서 접근하여 배포 버전을 svn 에 커밋
	if ( 3 == argc )
	{
		std::string strSVNRoot( argv[1] );
		std::string strBuildSource( argv[2] );
		std::string strVersion( "1.0.0.0" );
		std::string strAgentServerPath = strBuildSource + "AgentServer.exe";
		strVersion = GetFileVersion( strAgentServerPath );

		// svn update
		{
			char szCommand[512] = { 0 };
			sprintf_s( szCommand, "svn update %s", strSVNRoot.c_str() );
			if ( 0 != system( szCommand ) )
			{
				return -1;
			}
		}

		// svn 버전 이름의 디렉토리 생성
		std::string strMakeDirectoryName = strSVNRoot + strVersion;
		CreateDirectoryA( strMakeDirectoryName.c_str(), NULL );

		// 실행파일을 생성된 버전 이름의 디렉토리로 복사
		{
			char szCommand[512] = { 0 };
			sprintf_s( szCommand, "copy %s*.* %s\\", strBuildSource.c_str(), strMakeDirectoryName.c_str() );
			if ( 0 != system( szCommand ) )
			{
				return -1;
			}
		}

		// 복사된 실행파일들 svn add
		{
			char szCommand[512] = { 0 };
			sprintf_s( szCommand, "svn add %s", strMakeDirectoryName.c_str() );
			if ( 0 != system( szCommand ) )
			{
				return -1;
			}
		}

		// svn add 된 실행파일들 svn commit
		{
			char szCommand[512] = { 0 };
			sprintf_s( szCommand, "svn commit %s -m \"%s\"", strMakeDirectoryName.c_str(), strVersion.c_str() );
			if ( 0 != system( szCommand ) )
			{
				return -1;
			}
		}
		return 0;
	}

	time_t tCurrentTime = time( NULL );

	// GMT +9 (한국)
	tCurrentTime += 32400;

	tm t;
	gmtime_s( &t, &tCurrentTime );
	

	int nYear = ( t.tm_year ) % 100;
	int nDayofYear = t.tm_yday;
	int nHour = t.tm_hour;
	int nMin = t.tm_min;
	
	int nMonth = t.tm_mon + 1;
	int nDay = t.tm_mday;

	int Incremental = VersionNumbering::INCREMENTAL;

	std::string strCommandNomal( "tf.exe checkout ../VersionMaker/VersionNo.h" );
	if ( 0 != system( strCommandNomal.c_str() ) )
	{
		return -1;
	}

	// 파일 오픈해서 버전을 기록하자
	FILE *fp = fopen( "../VersionMaker/VersionNo.h", "w" );
	fprintf_s( fp, "#define FILEVER\t\t\t" );
	fprintf_s( fp, "%d,%d,%d,%d\n", VERSION_MAJOR, VERSION_MINOR, VERSION_MAINTENANCE, Incremental );
	fprintf_s( fp, "#define PRODUCTVER\t\t" );
	fprintf_s( fp, "%d,%d,%d,%d\n", VERSION_MAJOR, VERSION_MINOR, VERSION_MAINTENANCE, Incremental );
	fprintf_s( fp, "#define STRFILEVER\t\t" );
	fprintf_s( fp, "\"%d.%d.%d.%d\\0\"\n", VERSION_MAJOR, VERSION_MINOR, VERSION_MAINTENANCE, Incremental );
	fprintf_s( fp, "#define STRPRODUCTVER\t" );
	fprintf_s( fp, "\"Version %d.%d.%d\\0\"\n\n", VERSION_MAJOR, VERSION_MINOR, VERSION_MAINTENANCE );

	fprintf_s( fp, "namespace VersionNumbering\n" );
	fprintf_s( fp, "{\n");
	fprintf_s( fp, "static const int MAJOR = %d;\n", VERSION_MAJOR );
	fprintf_s( fp, "static const int MINOR = %d;\n", VERSION_MINOR );
	fprintf_s( fp, "static const int YEAR = %d;\n", nYear );
	fprintf_s( fp, "static const int DAYOFYERAR = %d;\n", nDayofYear );
	fprintf_s( fp, "static const int HOUR = %d;\n", nHour );
	fprintf_s( fp, "static const int MIN = %d;\n", nMin );
	fprintf_s( fp, "static const WORD INCREMENTAL = %d;\n", Incremental + 1 );
	fprintf_s( fp, "}\n");

	return 0;
}