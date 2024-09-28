#include "stdafx.h"

#include <vector>

#include "./IniLoader.h"
#include "./SUBPATH.h"
#include "./Define.h"

namespace autopatch
{
	std::string strRanOnlineSavePath;
	std::string strRanOnlinePath;

	std::string strPatchListCabFileName = "patchlist.lua.cab";
	std::string strVcredistCabFileName = "vcredist_x86.exe.cab";
	std::string strVersionFileName = "cVer.bin";
	std::string strLauncherCabFileName = "launcher.exe.cab";
	std::string strServerCabFileList = "filelist.bin.cab";
	std::string strServerFileList = "filelist.bin";
	std::string strClientFileList = "cfilelist.bin";
	std::string strClientLauncherPatchList = "lfilelist.bin";

	TCHAR HttpAddressTable[MAX_HTTP][STRING_NUM_128] =
	{
		"patch.ran-online.co.kr", "", "", "", "",
		"", "", "", "", "",
		"", "", "", "", "",
		"", "", "", "", "",
		"", "", "", "",
	};

	std::wstring ansiToUnicode( const char* szAscii, int nLen )
	{
		int bufSize = MultiByteToWideChar( CP_ACP, 0, szAscii, nLen, NULL, 0 );
		int strSize = bufSize;
		if ( nLen == -1 )
			strSize = bufSize-1;
		if ( strSize <= 0 )
			return std::wstring();
		std::vector< wchar_t > wsv( bufSize );
		if ( MultiByteToWideChar( CP_ACP, 0, szAscii, nLen, &wsv[0], bufSize ) == 0 )
			return std::wstring();
		else
			return std::wstring( wsv.begin(), wsv.begin() + strSize );
	}

	std::wstring ansiToUnicode( const std::string& StrW )
	{
		return ansiToUnicode( StrW.c_str(), (int) StrW.length() );
	}

	//! Convert a Unicode string to an ASCII string
	std::string unicodeToAnsi( const std::wstring& strW )
	{
		return unicodeToAnsi( strW.c_str(), ( int )strW.length() );
	}

	//! Convert string to wstring
	//! 여기서 유니코드는 MS OS 에서 말하는 유니코드이다. UTF16
	std::string unicodeToAnsi( const wchar_t* szUnicode, int nLen )
	{
		// 필요버퍼크기를 구한다.
		int bufSize = WideCharToMultiByte( CP_ACP, 0, szUnicode, nLen, NULL, 0, NULL, NULL );
		int strSize = bufSize;
		// len 이 -1 일 경우 마지막 터미널 문자의 개수까지 포함되기때문에
		// 실제 문자열 길이는 1을 뺀다.
		if ( nLen == -1 )
			strSize = bufSize-1;
		// 실제 문자열 길이가 0 보다 작을 경우 그냥 종료
		if ( strSize <= 0 )
			return std::string();
		std::vector< char > msv( bufSize );
		if ( WideCharToMultiByte( CP_ACP, 0, szUnicode, nLen, &msv[0], bufSize, NULL, NULL ) == 0 )
			return std::string();
		else
			return std::string( msv.begin(), msv.begin() + strSize );
	}

	std::string GetCountryFilePath()
	{
		CString strAppPath;
		CString strAppPathTemp;

		TCHAR szPath[MAX_PATH] = {0};
		GetModuleFileName( ::AfxGetInstanceHandle(), szPath, MAX_PATH );
		strAppPathTemp = szPath;

		if ( !strAppPathTemp.IsEmpty() )
		{
			strAppPath = strAppPathTemp.Left( strAppPathTemp.ReverseFind ( '\\' ) );
			if ( !strAppPath.IsEmpty() )
				if ( strAppPath.GetAt(0) == '"' )
					strAppPath = strAppPath.Right( strAppPath.GetLength() - 1 );
			strAppPath += '\\';
		}

		std::string CountryFile( strAppPath );
		CountryFile.append( "country.luc" );
		return CountryFile;
	}

	std::string GetPatchListFilePath()
	{
		std::string PatchListFile( strRanOnlineSavePath );
		PatchListFile.append( strPatchListCabFileName.c_str() );
		return PatchListFile;
	}

	BOOL LOAD_PARAM()
	{
		std::string strPath( strRanOnlinePath );
		strPath += "param.ini";

		CIniLoader cFILE( ",", false );

		if ( !cFILE.open( strPath, true ) )
			return FALSE;

		UINT nNum = cFILE.GetKeyDataSize( "SERVER SET", "HttpAddress" );
		memset( HttpAddressTable, 0, sizeof( HttpAddressTable ) );
		for ( UINT i = 0; i < nNum; ++i )
		{
			CString strHttpAddress;
			cFILE.getflag( "SERVER SET", "HttpAddress", i, nNum, strHttpAddress );
			strHttpAddress.Trim( _T(" ") );
			StringCchCopy( HttpAddressTable[i], STRING_NUM_128, strHttpAddress );
		}
		return TRUE;
	}

	BOOL DELETEFILE( const TCHAR* szFullPath )
	{
		SetFileAttributes ( szFullPath, FILE_ATTRIBUTE_NORMAL );
		return DeleteFile ( szFullPath );
	}

	void SetBasicPath( SUBPATH* pSubPath )
	{
		if ( !pSubPath )
		{
			return;
		}

		CString strAppPath;
		CString strAppPathTemp;

		TCHAR szPath[MAX_PATH] = {0};
		GetModuleFileName( ::AfxGetInstanceHandle(), szPath, MAX_PATH );
		strAppPathTemp = szPath;

		if ( !strAppPathTemp.IsEmpty() )
		{
			strAppPath = strAppPathTemp.Left( strAppPathTemp.ReverseFind ( '\\' ) );
			if ( !strAppPath.IsEmpty() )
				if ( strAppPath.GetAt(0) == '"' )
					strAppPath = strAppPath.Right( strAppPath.GetLength() - 1 );

			strAppPath += '\\';
		}

		TCHAR szPROFILE[MAX_PATH] = { 0, };
		TCHAR szSaveFullPath[MAX_PATH] = { 0, };
		SHGetSpecialFolderPath( NULL, szPROFILE, CSIDL_PERSONAL, FALSE );

		StringCchCopy( szSaveFullPath, MAX_PATH, szPROFILE );
		StringCchCat ( szSaveFullPath, MAX_PATH, pSubPath->SaveRoot().c_str() );
		CreateDirectory( szSaveFullPath, NULL );
		strRanOnlineSavePath = szSaveFullPath;

		strRanOnlinePath = strAppPath;
	}

	DWORD GetMD5( const TCHAR* szPath )
	{
		DWORD dwStatus = 0;
		BOOL bResult = FALSE;
		HCRYPTPROV hProv = 0;
		HCRYPTHASH hHash = 0;
		HANDLE hFile = NULL;
		BYTE rgbFile[BUFSIZE];
		DWORD cbRead = 0;
		BYTE rgbHash[MD5LEN];
		DWORD cbHash = 0;
		CHAR rgbDigits[] = "0123456789abcdef";
		CString strMessage;

		hFile = CreateFileA( szPath,
			GENERIC_READ,
			FILE_SHARE_READ,
			NULL,
			OPEN_EXISTING,
			FILE_FLAG_SEQUENTIAL_SCAN,
			NULL );

		if ( INVALID_HANDLE_VALUE == hFile )
		{
			dwStatus = GetLastError();
			strMessage.Format( "Error opening file(MD5) path: %s status: %d", szPath, dwStatus );
			::AfxMessageBox( _T( strMessage.GetString() ) );
			return dwStatus;
		}

		// Get handle to the crypto provider
		if ( !CryptAcquireContext( &hProv,
			NULL,
			NULL,
			PROV_RSA_FULL,
			CRYPT_VERIFYCONTEXT ) )
		{
			dwStatus = GetLastError();
			strMessage.Format( "CryptAcquireContext failed. status: %d", dwStatus );
			::AfxMessageBox( _T( strMessage.GetString() ) );
			CloseHandle( hFile );
			return dwStatus;
		}

		if ( !CryptCreateHash( hProv, CALG_MD5, 0, 0, &hHash ) )
		{
			dwStatus = GetLastError();
			strMessage.Format( "CryptCreateHash failed. status: %d", dwStatus );
			::AfxMessageBox( _T( strMessage.GetString() ) );
			CloseHandle( hFile );
			CryptReleaseContext( hProv, 0 );
			return dwStatus;
		}

		while ( bResult = ReadFile( hFile, rgbFile, BUFSIZE, 
			&cbRead, NULL ) )
		{
			if ( 0 == cbRead )
			{
				break;
			}

			if ( !CryptHashData( hHash, rgbFile, cbRead, 0 ) )
			{
				dwStatus = GetLastError();
				strMessage.Format( "CryptHashData failed. status: %d", dwStatus );
				::AfxMessageBox( _T( strMessage.GetString() ) );
				CryptReleaseContext( hProv, 0 );
				CryptDestroyHash( hHash );
				CloseHandle( hFile );
				return dwStatus;
			}
		}

		if ( !bResult )
		{
			dwStatus = GetLastError();
			strMessage.Format( "ReadFile failed. status: %d", dwStatus );
			::AfxMessageBox( _T( strMessage.GetString() ) );
			CryptReleaseContext( hProv, 0 );
			CryptDestroyHash( hHash );
			CloseHandle( hFile );
			return dwStatus;
		}

		CString strMD5;
		cbHash = MD5LEN;
		if ( CryptGetHashParam( hHash, HP_HASHVAL, rgbHash, &cbHash, 0 ) )
		{
			for ( DWORD i = 0; i < cbHash; i++ )
			{
				CString strTemp;
				strTemp.Format( "%c%c", rgbDigits[rgbHash[i] >> 4], rgbDigits[rgbHash[i] & 0xf] );
				strMD5 += strTemp;
			}

			::MessageBox ( NULL, strMD5.GetString(), "CryptGetHashParam", MB_ICONEXCLAMATION|MB_OK );
		}
		else
		{
			::AfxMessageBox( _T( "6" ) );
			dwStatus = GetLastError();
			strMessage.Format( "CryptGetHashParam failed. status: %d", dwStatus );
			::AfxMessageBox( _T( strMessage.GetString() ) );
		}

		CryptDestroyHash( hHash );
		CryptReleaseContext( hProv, 0 );
		CloseHandle( hFile );

		return dwStatus; 
	}
}


CPatchSetNode::CPatchSetNode(void) :
m_pFile(NULL),
m_bUse(FALSE)
{
}

CPatchSetNode::~CPatchSetNode(void)
{
	SAFE_DELETE ( m_pFile );
	m_bUse = FALSE;
}

BOOL CPatchSetNode::SetFile ( SFILENODE* pFile )
{
	if ( m_pFile )
		return FALSE;

	m_pFile = new SFILENODE;

	StringCchCopy ( m_pFile->FileName, FILENAME_SIZE, pFile->FileName );
	StringCchCopy ( m_pFile->SubPath, SUBPATH_SIZE, pFile->SubPath );
	m_pFile->Ver = pFile->Ver;
	StringCchCopy ( m_pFile->szMD5, MD5_SIZE, pFile->szMD5 );

	return TRUE;
}

////////////////////////////////////////////////////////////////////////

CPatchSet::CPatchSet(void)
{
}

CPatchSet::~CPatchSet(void)
{
	Clear();
}

void CPatchSet::Clear()
{
	SETPATCH_ITER iter = m_setPatch.begin();
	SETPATCH_ITER iter_end = m_setPatch.end();

	for( ; iter != iter_end; ++iter )
	{
		SAFE_DELETE( *iter );
	}

	m_setPatch.clear();
}

void CPatchSet::Add( SFILENODE* pFile )
{
	CPatchSetNode* pNode = new CPatchSetNode;
	pNode->SetFile( pFile );
	m_setPatch.insert( pNode );
}

CPatchSetNode* CPatchSet::Find( SFILENODE* pFile )
{
	CPatchSetNode searchNode;
	searchNode.SetFile( pFile );

	SETPATCH_ITER iter = m_setPatch.find( &searchNode );
	if( iter == m_setPatch.end() )
		return NULL;
	else
		return *iter;
}

void CPatchSet::GetNotUseItem( FILEVECTOR* pNotUseItems )
{
	if( !pNotUseItems )
		return;

	SETPATCH_ITER iter = m_setPatch.begin();
	SETPATCH_ITER iter_end = m_setPatch.end();

	for( ; iter != iter_end; ++iter )
	{
		if( ! (*iter)->GetUseFlag() )
		{
			pNotUseItems->push_back( (*iter)->GetFile() );
		}
	}
}
