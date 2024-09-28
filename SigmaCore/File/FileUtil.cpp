#include "stdafx.h"
#include <sys/stat.h>
#include <sys/types.h> 
#include <shlobj.h>
#include <string>
#include <Shlwapi.h>

//#include "./SubPath.h"
#include "../String/StringUtil.h"
#include "./FileUtil.h"


#include "../DebugInclude.h"


namespace sc {
    namespace file {

//! code from boost::filesystem.
//! \boost\libs\filesystem\src\operations_posix_windows.cpp
//! modifyed by Jgkim
bool isExist(const std::wstring& strFullPath )
{
	return isExist( strFullPath.c_str() );
}

bool isExist( const wchar_t* szFullPath )
{
	if(::GetFileAttributesW( szFullPath ) == 0xFFFFFFFF)
	{
		UINT err = ::GetLastError();
		if ((err == ERROR_FILE_NOT_FOUND) ||
			(err == ERROR_INVALID_PARAMETER) ||
			(err == ERROR_NOT_READY) ||
			(err == ERROR_PATH_NOT_FOUND) ||
			(err == ERROR_INVALID_NAME) ||
			(err == ERROR_BAD_NETPATH ))
		{
			// GetFileAttributes failed because the path does not exist
			// for any other error we assume the file does exist and fall through,
			// this may not be the best policy though...  (JM 20040330)
			return false;
		}
		else
		{
			return true;
		}
	}
	return true;
}

bool isExist( const std::string& strFullPath )
{
	return isExist( strFullPath.c_str() );
}

bool isExist( const char* szFullPath )
{
	if(::GetFileAttributesA( szFullPath ) == 0xFFFFFFFF)
	{
		UINT err = ::GetLastError();
		if ((err == ERROR_FILE_NOT_FOUND) ||
			(err == ERROR_INVALID_PARAMETER) ||
			(err == ERROR_NOT_READY) ||
			(err == ERROR_PATH_NOT_FOUND) ||
			(err == ERROR_INVALID_NAME) ||
			(err == ERROR_BAD_NETPATH ))
		{
			// GetFileAttributes failed because the path does not exist
			// for any other error we assume the file does exist and fall through,
			// this may not be the best policy though...  (JM 20040330)
			return false;
		}
		else
		{
			return true;
		}
	}
	return true;
}

//! code from boost::filesystem.
//! \boost\libs\filesystem\src\operations_posix_windows.cpp
//! modifyed by Jgkim
bool rename(const std::wstring& strOldPath, const std::wstring& strNewPath)
{
	if (0 == ::MoveFileW(strOldPath.c_str(), strNewPath.c_str()))
		return false;
	else
		return true;
}

//! 파일의 확장자를 strNewExt 로 변경한 파일명을 돌려준다.
//! changeFileExt( test.txt, avi ) -> test.avi
std::wstring changeFileExtW( const std::wstring& strFileName, const std::wstring& strNewExt )
{
	std::basic_string <wchar_t>::size_type indexCh;
	static const std::basic_string <wchar_t>::size_type npos = -1;

	std::wstring NewFileName;
	indexCh = strFileName.rfind( L"." );
	if ( npos == indexCh )
	{
		// "." 이 없기때문에 확장자가 없다.
		NewFileName = strFileName + L"." + strNewExt;
	}
	else
	{		
		NewFileName.assign( strFileName, 0, indexCh );
	}
	NewFileName += L".";
	NewFileName += strNewExt;
	return NewFileName;
}

std::string changeFileExt(const std::string& strFileName, const std::string& strNewExt)
{
    std::basic_string <char>::size_type indexCh;
    static const std::basic_string <char>::size_type npos = -1;

    std::string NewFileName;
    indexCh = strFileName.rfind(".");
    if (npos == indexCh)
    {
        // "." 이 없기때문에 확장자가 없다.
        NewFileName = strFileName + "." + strNewExt;
    }
    else
    {		
        NewFileName.assign(strFileName, 0, indexCh);
    }
    NewFileName += ".";
    NewFileName += strNewExt;
    return NewFileName;
}

//! 파일 확장자를 돌려준다.
std::wstring getFileExtW(const std::wstring& strFullPath)
{
	std::basic_string <wchar_t>::size_type indexCh;
	static const std::basic_string <wchar_t>::size_type npos = -1;

	indexCh = strFullPath.rfind( L"." );
	if ( npos == indexCh )
	{
		return std::wstring(L"");
	}
	else
	{
		std::wstring strExt;
		strExt.assign( strFullPath, indexCh+1, strFullPath.size()-indexCh-1 );
		return strExt;
	}
}

std::string getFileExt(const std::string& strFullPath)
{
    std::basic_string <char>::size_type indexCh;
    static const std::basic_string <char>::size_type npos = -1;

    indexCh = strFullPath.rfind(".");
    if ( npos == indexCh )
    {
        return std::string("");
    }
    else
    {
        std::string strExt;
        strExt.assign( strFullPath, indexCh+1, strFullPath.size()-indexCh-1 );
        return strExt;
    }
}

bool autoRename( const std::string& strFullPath )
{
    std::wstring strPath = sc::string::ansiToUnicode( strFullPath.c_str(), (int) strFullPath.length() );
	return autoRename( strPath );
}

bool autoRename( const std::wstring& strFullPath )
{
	CString FullPath( strFullPath.c_str() );
	DWORD dwFind = FullPath.ReverseFind(L'.');
	CStringW strTempPath;
	CStringW strTempExt;
	if ( dwFind != -1 )
	{	// 확장자가 있다.
		// path+filename
		strTempPath = FullPath.Left( dwFind );
		// . + 확장자명
		strTempExt  = FullPath.Mid( dwFind, FullPath.GetLength()-dwFind );
	}
	else
	{ // 확장자가 없다.
		strTempPath = FullPath;
	}
	
	bool bResult = isExist( strFullPath.c_str() );
	if ( true == bResult ) 
	{ // 파일이 존재.
		bool bRename;
		std::wstring strNewPath;
		// 파일 이름 바꾸기를 시도한다.
		for (UINT i=1; i<UINT_MAX; ++i)
		{
			// UINT_MAX 공간 10개, 끝문자열 1개 = 총 11개
			wchar_t szLoop[11] = {0};
			strNewPath = L"";
			strNewPath += strTempPath; // path + filename
			_itow_s(i, szLoop, 10, 10);
			strNewPath += szLoop;
			strNewPath += strTempExt; // . + 확장자
			bRename = rename(
				std::wstring(strFullPath),
				strNewPath);
			if (true == bRename)
				break;
		}
		// 루프를 다 돌았는데 파일이름 바꾸기를 실패했다.
		return bRename;
	}
	else
	{
		// 파일이 존재하지 않는다.
		return false;
	}
}

//! code from boost::filesystem.
//! \boost\libs\filesystem\src\operations_posix_windows.cpp
//! modifyed by Jgkim
bool isDirectory( const std::wstring& Path )
{
	return isDirectory( Path.c_str() );
}

bool isDirectory( const std::string& Path )
{
	return isDirectory( Path.c_str() );
}

bool isDirectory( const wchar_t* szPath )
{
	DWORD attributes = ::GetFileAttributesW( szPath );
	if ( attributes == 0xFFFFFFFF ) {
		return false;
	} else {
		return (attributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
	}
}

bool isDirectory( const char* szPath )
{
	DWORD attributes = ::GetFileAttributesA( szPath );
	if ( attributes == 0xFFFFFFFF ) {
		return false;
	} else {
		return (attributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
	}
}

//! code from boost::filesystem.
//! \boost\libs\filesystem\src\operations_posix_windows.cpp
//! modifyed by Jgkim
bool createDirectory( const std::wstring& Path )
{
	return createDirectory( Path.c_str() );
}

bool createDirectory( const std::string& Path )
{
	return createDirectory( Path.c_str() );
}

bool createDirectory( const wchar_t* szPath )
{
	if (szPath == NULL)
        return false;

	if ( ::CreateDirectoryW( szPath, 0 ) )
		return true;
      
	if ( ::GetLastError() != ERROR_ALREADY_EXISTS ) {
        return false;
	}
    
	if ( !isDirectory( szPath ) ) {
        return false;
	}
	return true;
}

bool createDirectory( const char* szPath )
{
	if (szPath == NULL) return false;

	if ( ::CreateDirectoryA( szPath, 0 ) )
		return true;

	if ( ::GetLastError() != ERROR_ALREADY_EXISTS ) {
		return false;
	}

	if ( !isDirectory( szPath ) ) {
		return false;
	}
	return true;
}

//! code from boost::filesystem.
//! \boost\libs\filesystem\src\operations_posix_windows.cpp
//! modifyed by Jgkim
bool remove( const std::wstring& Path )
{
	return remove( Path.c_str() );
}

bool remove( const std::string& Path )
{
	return remove( Path.c_str() );
}

bool remove( const wchar_t* szFullPath )
{
	if (NULL == szFullPath)
		return false;

	if ( isExist(szFullPath) )
	{
		if ( isDirectory(szFullPath) )
        {
			if ( !::RemoveDirectoryW(szFullPath) )
			{
				return false;
			}
        }
        else
        {
			// 대상 파일이 read-only 등의 flag 가 있으면 안된다.
			SetFileAttributesW( szFullPath, FILE_ATTRIBUTE_ARCHIVE );
			if ( !::DeleteFileW(szFullPath) )
			{
				return false;
			}
        }
        return true;
	}
    return false;
}

bool remove( const char* szFullPath )
{
	if (NULL == szFullPath)
		return false;

	if ( isExist(szFullPath) )
	{
		if ( isDirectory(szFullPath) )
		{
			if ( !::RemoveDirectoryA(szFullPath) )
			{
				return false;
			}
		}
		else
		{
			// 대상 파일이 read-only 등의 flag 가 있으면 안된다.
			SetFileAttributesA( szFullPath, FILE_ATTRIBUTE_ARCHIVE );
			if ( !::DeleteFileA(szFullPath) )
			{
				return false;
			}
		}
		return true;
	}
	return false;
}

void deleteFiles(const std::string& Path)
{
    CString strDirectoryFile(Path.c_str());
    strDirectoryFile.Append("\\*.*");

    strDirectoryFile.Replace("\\\\", "\\");

    CFileFind finder;
    BOOL bWorking = finder.FindFile(strDirectoryFile.GetString());

    CString strTemp;
    while (bWorking)
    {
        bWorking = finder.FindNextFile();

        // skip . and .. files; otherwise, we'd
        // recur infinitely!
        if (finder.IsDots())
            continue;

        strTemp = finder.GetFilePath();
        remove(strTemp.GetString());
    }
    finder.Close();
}

//! code from boost::filesystem.
//! \boost\libs\filesystem\src\operations_posix_windows.cpp
//! modifyed by Jgkim
bool copyFile(
	const std::wstring& fromFullPath,
	const std::wstring& toFullPath,
	BOOL bOverwrite )
{
	// 대상 파일이 read-only 등의 flag 가 있으면 복사가 안된다.
	SetFileAttributesW( toFullPath.c_str(), FILE_ATTRIBUTE_ARCHIVE );

	BOOL bForce = TRUE;
	if ( bOverwrite == TRUE )
	{
		bForce = FALSE; // 덮어쓰기 가능
		remove( toFullPath.c_str() );
	}	

	if ( 0 == ::CopyFileW(
				fromFullPath.c_str(),
				toFullPath.c_str(),
				bForce ) )
	{
		return false;
	}
	else
	{
		return true;
	}

}

bool copyFile(const CStringW& fromFullPath, const CStringW& toFullPath, BOOL bOverwrite)
{
	return copyFile(std::wstring(fromFullPath), std::wstring(toFullPath), bOverwrite);
}

bool copyFile( const std::string& fromFullPath, const std::string& toFullPath, BOOL bOverwrite )
{
	std::wstring strFrom = sc::string::ansiToUnicode( fromFullPath.c_str(), (int) fromFullPath.length() );
	std::wstring strTo = sc::string::ansiToUnicode( toFullPath.c_str(), (int) toFullPath.length() );
	return copyFile( strFrom, strTo, bOverwrite );
}

/*
bool symbolic_link_exists( const path & ph )
{
	// Windows has no O/S concept of symbolic links
	// (.lnk files are an application feature, not
	// a Windows operating system feature)
	return false;
}
*/
/*
CString& getUserFolder()
{
	return CORE_COMMON::strASUserPath;
}
*/
/*
const CString& getLogFolder()
{
	return SUBPATH::getInstance()->getDebugInfoRoot();
}
*/
/*
const CString& getTempFolder()
{
	return SUBPATH::getInstance()->getTempRoot();
}
*/
/*
const CString& getScreenShotFolder()
{
	return SUBPATH::getInstance()->getCapture();
}
*/
/*
void makeASFolder()
{
	TCHAR szPath[MAX_PATH];
	CString strTemp;
	if ( SUCCEEDED(::SHGetFolderPath(
						NULL, 
                        CSIDL_PERSONAL|CSIDL_FLAG_CREATE, 
                        NULL, 
                        0, 
                        szPath)) )
	{
		strTemp = szPath;
	}
	else
	{
		strTemp = L"c:\\temp";
	}
	
	SUBPATH::getInstance()->setSaveRoot(strTemp + L"\\Game");
	strASUserPath = SUBPATH::getInstance()->getSaveRoot();

	SUBPATH::getInstance()->setPlayInfoRoot(strASUserPath + L"\\Play\\");
	SUBPATH::getInstance()->setDebugInfoRoot(strASUserPath + L"\\Log\\");
	SUBPATH::getInstance()->setTempRoot(strASUserPath + L"\\Temp\\");
	SUBPATH::getInstance()->setCapture(strASUserPath + L"\\ScreenShot\\");
	SUBPATH::getInstance()->setInputUser(strASUserPath + L"\\KeySet\\");

	createDirectory( SUBPATH::getInstance()->getSaveRoot() );
	createDirectory( SUBPATH::getInstance()->getPlayInfoRoot() );
	createDirectory( SUBPATH::getInstance()->getDebugInfoRoot() );
	createDirectory( SUBPATH::getInstance()->getTempRoot() );
	createDirectory( SUBPATH::getInstance()->getCapture() );
	createDirectory( SUBPATH::getInstance()->getInputUser() );
}
*/
__int64 getFileSize64( const TCHAR * szFileName )
{ 
	struct __stat64 fileStat; 
	int err = _tstat64( szFileName, &fileStat ); 
	if (0 != err) return 0; 
	return fileStat.st_size; 
}

int getFileSizeCPP(const char* szFileName) 
{ 
	struct stat fileStat; 

	//char szAnsiName[MAX_PATH];
	//WideCharToMultiByte( CP_ACP, 0, szFileName, -1, szAnsiName, MAX_PATH, NULL, NULL );
	//szAnsiName[MAX_PATH-1] = 0;

	int err = stat( szFileName, &fileStat ); 
	if (0 != err)
        return 0; 
    else
	    return fileStat.st_size; 
}
	
// MFC version
// slow...
ULONGLONG getFileSizeMFC( const TCHAR* szFileName )
{
	CFile* pFile = NULL;
	ULONGLONG llLength=0;

	TRY
	{
		pFile = new CFile(
			szFileName,
			CFile::modeRead | CFile::shareDenyNone);
		llLength = pFile->GetLength();
	}
	CATCH(CFileException, pEx)
	{
		// Simply show an error message to the user.
		pEx->ReportError();			
	}
	AND_CATCH(CMemoryException, pEx)
	{
		// We can't recover from this memory exception, so we'll
		// just terminate the app without any cleanup. Normally, an
		// an application should do everything it possibly can to
		// clean up properly and _not_ call AfxAbort().
		AfxAbort();			
	}
	END_CATCH
	// If an exception occurs in the CFile constructor,
	// the language will free the memory allocated by new
	// and will not complete the assignment to pFile.
	// Thus, our clean-up code needs to test for NULL.
	if (pFile != NULL)   
	{
		pFile->Close();
		delete pFile;
		return llLength;
	}
	else
	{
		return 0;
	}
}

//int CORE_COMMON::nPatchFolderIndex = 0;

//! RootPath 아래에 있는 파일들의 리스트를 작성한다.
void makeFileList(const char* szRootPath, std::vector<PatchFileA>& vecFile)
{
	std::string strWildCard(szRootPath);
	strWildCard += "\\*.*";
    sc::string::lowerString(strWildCard);

	CFileFind FileFind;
	BOOL bWork = FileFind.FindFile( strWildCard.c_str() );
	
	// 파일조사
	while (bWork)
	{
		bWork = FileFind.FindNextFile();

		// . 과 .. 는 skip 한다.
		if ( FileFind.IsDots() ) {
			continue;
		}
		if ( FALSE == FileFind.IsDirectory() )
		{
			CString strFile = FileFind.GetFileName();
			strFile.MakeLower();

			CString strPath = FileFind.GetFilePath();
			strPath.MakeLower();

			PatchFileA FileData;
			//FileData.nFolderIndex = nFolderIndex; // nForderIndex;
			//FileData.strFolder    = strPath.GetString();
			//FileData.strFile      = strFile.GetString();

            sc::string::getFilePathAndName(
				strPath.GetString(),
				FileData.strFolder,
				FileData.strFile,
				FileData.strExt);

			vecFile.push_back( FileData );
		}
	}

	FileFind.Close();

	bWork = FileFind.FindFile( strWildCard.c_str() );

	// 디렉토리 조사
	while (bWork)
	{
		bWork = FileFind.FindNextFile ();

		// . 과 .. 는 skip 한다.
		if ( FileFind.IsDots() ) {
			continue;
		}

		// 디렉토리라면 디렉토리 안을 검사한다.
		if ( TRUE == FileFind.IsDirectory() )
		{
			//int nNewFolderIndex = nForderIndex+1;			
			//nFolderIndex++;
			CString strNewPath = FileFind.GetFilePath();
			strNewPath.MakeLower();
			//PatchFolder FolderData;
			//FolderData.nIndex = nFolderIndex;
			//FolderData.strFolder = strNewPath.GetString();
			//vecFolder.push_back( FolderData );
			
			makeFileList( strNewPath, vecFile );
		}
	}

	FileFind.Close();
}

bool getFileLastWriteTime( const std::wstring& FileName, __time64_t& WriteTime )
{
    FILETIME ftCreate, ftAccess, ftWrite;
    SYSTEMTIME stUTC, stLocal;    

	HANDLE hFile = CreateFileW(
		FileName.c_str(),
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
        OPEN_EXISTING,
		0,
		NULL );

	if ( INVALID_HANDLE_VALUE == hFile )
		return false;

    // Retrieve the file times for the file.
    if ( !GetFileTime( hFile, &ftCreate, &ftAccess, &ftWrite ) )
        return false;

    // Convert the last-write time to local time.
    FileTimeToSystemTime( &ftWrite, &stUTC );
    SystemTimeToTzSpecificLocalTime( NULL, &stUTC, &stLocal );

    // Build a string showing the date and time.    
	CTime LocalTime( stLocal );
	WriteTime = LocalTime.GetTime();
	CloseHandle( hFile );
	return true;
}

//! 파일의 마지막 Access 타임을 구한다.
bool getFileLastAccessTime( const std::wstring& FileName, __time64_t& AccessTime )
{
	FILETIME ftCreate, ftAccess, ftWrite;
    SYSTEMTIME stUTC, stLocal;    

	HANDLE hFile = CreateFileW(
		FileName.c_str(),
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
        OPEN_EXISTING,
		0,
		NULL );

	if ( INVALID_HANDLE_VALUE == hFile )
		return false;

    // Retrieve the file times for the file.
    if ( !GetFileTime( hFile, &ftCreate, &ftAccess, &ftWrite ) )
        return false;

    // Convert the last-write time to local time.
    FileTimeToSystemTime( &ftAccess, &stUTC );
    SystemTimeToTzSpecificLocalTime( NULL, &stUTC, &stLocal );

    // Build a string showing the date and time.    
	CTime LocalTime( stLocal );
	AccessTime = LocalTime.GetTime();
	CloseHandle( hFile );
	return true;
}

//! 파일의 Create 타임을 구한다.
bool getFileCreateTime( const std::wstring& FileName, __time64_t& CreateTime )
{
	FILETIME ftCreate, ftAccess, ftWrite;
    SYSTEMTIME stUTC, stLocal;    

	HANDLE hFile = CreateFileW(
		FileName.c_str(),
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
        OPEN_EXISTING,
		0,
		NULL );

	if ( INVALID_HANDLE_VALUE == hFile )
		return false;

    // Retrieve the file times for the file.
    if ( !GetFileTime( hFile, &ftCreate, &ftAccess, &ftWrite ) )
        return false;

    // Convert the last-write time to local time.
    FileTimeToSystemTime( &ftCreate, &stUTC );
    SystemTimeToTzSpecificLocalTime( NULL, &stUTC, &stLocal );

    // Build a string showing the date and time.    
	CTime LocalTime( stLocal );
	CreateTime = LocalTime.GetTime();
	CloseHandle( hFile );
	return true;
}

bool getFileTime(
	const std::wstring& FileName, __time64_t& CreateTime, __time64_t& AccessTime, __time64_t& WriteTime )
{
	FILETIME ftCreate, ftAccess, ftWrite;
    SYSTEMTIME stUTC, stLocal;    

	HANDLE hFile = CreateFileW(
		FileName.c_str(),
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
        OPEN_EXISTING,
		0,
		NULL );

	if ( INVALID_HANDLE_VALUE == hFile )
		return false;

    // Retrieve the file times for the file.
    if ( !GetFileTime( hFile, &ftCreate, &ftAccess, &ftWrite ) )
        return false;

    // Convert the time to local time.
    FileTimeToSystemTime( &ftCreate, &stUTC );
    SystemTimeToTzSpecificLocalTime( NULL, &stUTC, &stLocal );    
	CTime Time1( stLocal );
	CreateTime = Time1.GetTime();

	FileTimeToSystemTime( &ftAccess, &stUTC );
    SystemTimeToTzSpecificLocalTime( NULL, &stUTC, &stLocal );    
	CTime Time2( stLocal );
	AccessTime = Time2.GetTime();

	FileTimeToSystemTime( &ftWrite, &stUTC );
    SystemTimeToTzSpecificLocalTime( NULL, &stUTC, &stLocal );    
	CTime Time3( stLocal );
	WriteTime = Time3.GetTime();

	CloseHandle( hFile );
	return true;
}

CString GetFileOnly(LPCTSTR Path)
{
    // Strip off the path and return just the filename part
    CString temp = (LPCTSTR) Path; // Force CString to make a copy
    ::PathStripPath(temp.GetBuffer(0));
    temp.ReleaseBuffer(-1);
    return temp;
}

CString GetFileExt(LPCTSTR Path)
{
    CString cs;
    cs = ::PathFindExtension(Path);
    return cs;
}

CString GetFileName(LPCTSTR Path)
{
    CString csFile = GetFileOnly(Path);
    if (!GetFileExt(csFile).IsEmpty()) 
    { 
        // Is there an extension
        ::PathRemoveExtension(csFile.GetBuffer(_MAX_PATH));
        csFile.ReleaseBuffer();
    }
    return csFile;
}

    } // namespace file
} // namespace sc
