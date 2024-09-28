#pragma once

#include <windows.h>
#include <Wincrypt.h>
#include <wininet.h>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <strsafe.h>

#include "./ServiceProviderDefine.h"

#ifndef SAFE_DELETE
#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#endif    
#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#endif    
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }
#endif

#define STRING_NUM_128	128

#define BUFSIZE 1024
#define MD5LEN  16

class SUBPATH;

/*
struct PatchFile 
{
	DWORD TotalSize;
	CProgressCtrl* pProgress;
	HINTERNET hInternet;
	CStatic* pDisplay;
	std::string FileName;
};
*/
struct PatchFile 
{
	std::string FileName;
	std::string MD5;
	std::string FilePath;
	int GameVersion;
	bool NeedVersionUpdate;
};

namespace autopatch
{
	enum
	{ 
		MAX_FTP = 24, 
		EMGAMEOPT_VER = 0x0127,
		MAX_HTTP = 24,
		MAX_SERVER = 20,
		MAX_CHINA_REGION = 8,
		X_RESOLUTION = 1024,
		Y_RESOLUTION = 768,
		MAX_QUEST_DISPLAY = 5,
		MAX_SEARCH_WORD = 50,
		MAX_POINT_SHOP_WISH_LIST = 10,
	};

	std::wstring ansiToUnicode( const char* szAscii, int nLen );
	std::wstring ansiToUnicode( const std::string& StrW );

	//! Convert a Unicode string to an ASCII string
	std::string unicodeToAnsi( const std::wstring& strW );

	//! Convert string to wstring
	//! 여기서 유니코드는 MS OS 에서 말하는 유니코드이다. UTF16
	std::string unicodeToAnsi( const wchar_t* szUnicode, int nLen );

	std::string GetCountryFilePath();
	std::string GetPatchListFilePath();

	void SetBasicPath( SUBPATH* pSubPath );
	BOOL LOAD_PARAM();
	BOOL DELETEFILE( const TCHAR* szFullPath );

	DWORD GetMD5( const TCHAR* szPath );

	extern TCHAR HttpAddressTable[MAX_HTTP][STRING_NUM_128];

	extern std::string strRanOnlineSavePath;
	extern std::string strRanOnlinePath;

	extern std::string strPatchListCabFileName;
	extern std::string strVcredistCabFileName;
	extern std::string strVersionFileName;
	extern std::string strLauncherCabFileName;
	extern std::string strServerCabFileList;
	extern std::string strServerFileList;
	extern std::string strClientFileList;
	extern std::string strClientLauncherPatchList;

}

enum emFileList
{	
	FILENAME_SIZE	= 64,
	SUBPATH_SIZE	= 128,
	MD5_SIZE		= 33, // MD5값 32자리 + 종료문자 1 = 33

	VER_1_OFFSET	= 196,
	VER_2_OFFSET	= 232, // 4바이트 정렬로 229가 아닌 3바이트 늘어난 232
	VER_3_OFFSET	= 236
};

struct SFILENODE_OLD
{
	// Version 1
	CHAR FileName[FILENAME_SIZE];
	CHAR SubPath[SUBPATH_SIZE];
	INT Ver;
	// Version 2
	CHAR szMD5[MD5_SIZE];

public:
	SFILENODE_OLD() :
	  Ver( 0 )
	  {
		  SecureZeroMemory( FileName, sizeof( FileName ) );
		  SecureZeroMemory( SubPath, sizeof( SubPath ) );
		  SecureZeroMemory( szMD5, sizeof( szMD5 ) );
	  }
};

// SFILENODE 구조체 런처 프로젝트에도 있다. 이 부분 수정할 때 꼭 확인 및 함께 수정 필요
struct SFILENODE
{
	// Version 1
	CHAR FileName[FILENAME_SIZE];
	CHAR SubPath[SUBPATH_SIZE];
	INT Ver;
	// Version 2
	CHAR szMD5[MD5_SIZE];
	// Version 3
	INT IsPreDown;

public:
	SFILENODE() :
	  Ver( 0 ), IsPreDown( 0 )
	  {
		  SecureZeroMemory( FileName, sizeof( FileName ) );
		  SecureZeroMemory( SubPath, sizeof( SubPath ) );
		  SecureZeroMemory( szMD5, sizeof( szMD5 ) );
	  }

	  void SetData( SFILENODE *pData )
	  {
		  ::StringCchCopy( FileName, FILENAME_SIZE, pData->FileName );
		  ::StringCchCopy( SubPath, SUBPATH_SIZE, pData->SubPath );
		  ::StringCchCopy( szMD5, MD5_SIZE, pData->szMD5 );

		  Ver = pData->Ver;
		  IsPreDown = pData->IsPreDown;
	  }

	  void SetData( SFILENODE &data )
	  {
		  ::StringCchCopy( FileName, FILENAME_SIZE, data.FileName );
		  ::StringCchCopy( SubPath, SUBPATH_SIZE, data.SubPath );
		  ::StringCchCopy( szMD5, MD5_SIZE, data.szMD5 );

		  Ver = data.Ver;
		  IsPreDown = data.IsPreDown;
	  }

	  void SetData( int nVer )
	  {
		  SecureZeroMemory( FileName, sizeof( FileName ) );
		  SecureZeroMemory( SubPath, sizeof( SubPath ) );
		  SecureZeroMemory( szMD5, sizeof( szMD5 ) );

		  Ver = nVer;
		  IsPreDown = 0;
	  }

	  void Clear()
	  {
		  SecureZeroMemory( FileName, sizeof( FileName ) );
		  SecureZeroMemory( SubPath, sizeof( SubPath ) );
		  SecureZeroMemory( szMD5, sizeof( szMD5 ) );

		  Ver = 0;
		  IsPreDown = 0;
	  }

	  void SetFileName( const CHAR* pData )
	  {
		  ::StringCchCopy( FileName, FILENAME_SIZE, pData );
	  }

	  void SetSubPath( const CHAR* pData )
	  {
		  ::StringCchCopy( SubPath, SUBPATH_SIZE, pData );
	  }

	  void SetMD5( const CHAR* pData )
	  {
		  ::StringCchCopy( szMD5, MD5_SIZE, pData );
	  }
};

typedef	std::vector< SFILENODE* > FILEVECTOR;
typedef	FILEVECTOR::iterator FILEVECTOR_ITER;

typedef	std::map< std::string, SFILENODE* > FILEMAP;
typedef	FILEMAP::iterator FILEMAP_ITER;

class CPatchSetNode
{
public:
	CPatchSetNode( void );
	~CPatchSetNode( void );

public:
	BOOL		SetFile ( SFILENODE* pFile );
	SFILENODE*	GetFile ( void )				{ return m_pFile; }

	void		SetUseFlag ( void )				{ m_bUse = TRUE; }
	BOOL		GetUseFlag ( void )				{ return m_bUse; }

protected:
	SFILENODE*	m_pFile;
	BOOL		m_bUse;
};

class CCompare
{
public:
	bool operator() ( CPatchSetNode *p1, CPatchSetNode *p2 ) const
	{
		SFILENODE * pFile1 = p1->GetFile();
		SFILENODE * pFile2 = p2->GetFile();

		int Value1 = stricmp( pFile1->FileName, pFile2->FileName );
		int Value2 = stricmp( pFile1->SubPath, pFile2->SubPath );

		return ( Value1<0 ) || ( !( 0<Value1 ) && ( Value2<0 ) );
	}
};

class CPatchSet
{
public:
	typedef std::set< CPatchSetNode*, CCompare >	SETPATCH;
	typedef SETPATCH::iterator					SETPATCH_ITER;
	SETPATCH m_setPatch;

	void Add( SFILENODE* pFile );
	void Clear();
	size_t Size() { return m_setPatch.size(); }

	CPatchSetNode* Find( SFILENODE* pFile );
	void GetNotUseItem( FILEVECTOR* pNotUseItems );

	CPatchSet( void );
	~CPatchSet( void );
};
