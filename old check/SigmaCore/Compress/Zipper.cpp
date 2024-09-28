// Zipper.cpp: implementation of the CZipper class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "../../=zlib/zip.h"
#include "../../=zlib/unzip.h"
#include "../../=ZLib/iowin32.h"
#include "Zipper.h"


#include "../DebugInclude.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

const UINT ZIP_BUFFER_SIZE = 2048;

CZipper::CZipper(LPCTSTR szFilePath, LPCTSTR szRootFolder, bool bAppend) : m_uzFile(0)
{
	CloseZip();
	ZeroMemory(&m_info, sizeof(m_info));

	if (szFilePath)
		OpenZip(szFilePath, szRootFolder, bAppend);
}

CZipper::~CZipper()
{
	CloseZip();
}

bool CZipper::CloseZip()
{
	int nRet = m_uzFile ? zipClose(m_uzFile, NULL) : ZIP_OK;

	m_uzFile = NULL;
	m_szRootFolder[0] = 0;
	// note: we don't clear m_info until the object is re-used or deleted

	return (nRet == ZIP_OK);
}

void CZipper::GetFileInfo(Z_FileInfo& info)
{
	info = m_info;
}

// simple interface
bool CZipper::ZipFile(LPCTSTR szFilePath)
{
	// make zip path
	char szDrive[_MAX_DRIVE], szFolder[MAX_PATH], szName[_MAX_FNAME];
	_splitpath_s( szFilePath, szDrive, _MAX_DRIVE, szFolder, MAX_PATH, szName, _MAX_FNAME, NULL, 0 );

	char szZipPath[MAX_PATH];
	_makepath_s(szZipPath, MAX_PATH, szDrive, szFolder, szName, "zip");

	CZipper zip;

	if (zip.OpenZip(szZipPath, false))
		return zip.AddFileToZip(szFilePath, false, NULL);

	return false;
}
	
bool CZipper::ZipFolder(LPCTSTR szFilePath, bool bIgnoreFilePath)
{
	// make zip path
	char szDrive[_MAX_DRIVE], szFolder[MAX_PATH], szName[_MAX_FNAME], szExt[_MAX_EXT];
	_splitpath_s( szFilePath, szDrive, _MAX_DRIVE, szFolder, MAX_PATH, szName, _MAX_FNAME, szExt, _MAX_EXT );

	lstrcat(szName, szExt); // save extension if any

	// set root path to include the folder name
	char szRootPath[MAX_PATH];
	_makepath_s(szRootPath, MAX_PATH, szDrive, szFolder, szName, NULL);

	char szZipPath[MAX_PATH];
	_makepath_s(szZipPath, MAX_PATH, szDrive, szFolder, szName, "zip");

	CZipper zip;

	if (zip.OpenZip(szZipPath, szRootPath, false))
		return zip.AddFolderToZip(szFilePath, bIgnoreFilePath);

	return false;
}

// works with prior opened zip
bool CZipper::AddFileToZip(LPCTSTR szFilePath, bool bIgnoreFilePath, const char* szPass)
{
	if (!m_uzFile)
		return false;

	// we don't allow paths beginning with '..\' because this would be outside
	// the root folder
	if (!bIgnoreFilePath && strstr(szFilePath, "..\\") == szFilePath)
		return false;

	bool bFullPath = (strchr(szFilePath, ':') != NULL);	

	// if the file is relative then we need to append the root before opening
	char szFullFilePath[MAX_PATH];
	
	lstrcpy(szFullFilePath, szFilePath);
	PrepareSourcePath(szFullFilePath);

	// if the file is a fullpath then remove the root path bit
	char szFileName[MAX_PATH] = "";

	if (bIgnoreFilePath)
	{
		char szName[_MAX_FNAME], szExt[_MAX_EXT];
		_splitpath_s( szFilePath, NULL, 0, NULL, 0, szName, _MAX_FNAME, szExt, _MAX_EXT );

		_makepath_s(szFileName, MAX_PATH, NULL, NULL, szName, szExt);
	}
	else if (bFullPath)
	{
		// check the root can be found
		if (0 != _strnicmp(szFilePath, m_szRootFolder, lstrlen(m_szRootFolder)))
			return false;

		// else
		lstrcpy(szFileName, szFilePath + lstrlen(m_szRootFolder));
	}
	else // relative path
	{
		// if the path begins with '.\' then remove it
		if (strstr(szFilePath, ".\\") == szFilePath)
			lstrcpy(szFileName, szFilePath + 2);
		else
			lstrcpy(szFileName, szFilePath);
	}

	PrepareZipPath(szFileName);

	// save file attributes
	zip_fileinfo zfi;

	zfi.internal_fa = 0;
	zfi.external_fa = (::GetFileAttributes(szFilePath) & ~FILE_ATTRIBUTE_READONLY);
	
	// save file time
	SYSTEMTIME st;

	GetLastModified(szFullFilePath, st);

	zfi.dosDate = 0;
	zfi.tmz_date.tm_year = st.wYear;
	zfi.tmz_date.tm_mon = st.wMonth - 1;
	zfi.tmz_date.tm_mday = st.wDay;
	zfi.tmz_date.tm_hour = st.wHour;
	zfi.tmz_date.tm_min = st.wMinute;
	zfi.tmz_date.tm_sec = st.wSecond;
	
	// load input file
	HANDLE hInputFile = ::CreateFile(szFullFilePath, 
									GENERIC_READ,
									0,
									NULL,
									OPEN_EXISTING,
									FILE_ATTRIBUTE_READONLY,
									NULL);

	if (hInputFile == INVALID_HANDLE_VALUE)
		return false;

	int nRet = zipOpenNewFileInZip3(m_uzFile, 
									szFileName,
									&zfi, 
									NULL, 
									0,
									NULL,
									0, 
									NULL,
									Z_DEFLATED,
									Z_DEFAULT_COMPRESSION,
									0,
									-MAX_WBITS, 
									DEF_MEM_LEVEL, 
									Z_DEFAULT_STRATEGY,
									szPass, 
									0 );

	if (nRet == ZIP_OK)
	{
		m_info.nFileCount++;

		// read the file and output to zip
		char pBuffer[ZIP_BUFFER_SIZE];
		DWORD dwBytesRead = 0, dwFileSize = 0;

		while (nRet == ZIP_OK && ::ReadFile(hInputFile, pBuffer, ZIP_BUFFER_SIZE, &dwBytesRead, NULL))
		{
			dwFileSize += dwBytesRead;

			if (dwBytesRead)
				nRet = zipWriteInFileInZip(m_uzFile, pBuffer, dwBytesRead);
			else
				break;
		}

		m_info.dwUncompressedSize += dwFileSize;
	}

	zipCloseFileInZip(m_uzFile);
	::CloseHandle(hInputFile);

	return (nRet == ZIP_OK);
}

bool CZipper::AddFileToZip(LPCTSTR szFilePath, LPCTSTR szRelFolderPath)
{
	if (!m_uzFile)
		return false;

	// szRelFolderPath cannot contain drive info
	if (szRelFolderPath && strchr(szRelFolderPath, ':'))
		return false;

	// if the file is relative then we need to append the root before opening
	char szFullFilePath[MAX_PATH];
	
	lstrcpy(szFullFilePath, szFilePath);
	PrepareSourcePath(szFullFilePath);

	// save file attributes and time
	zip_fileinfo zfi;

	zfi.internal_fa = 0;
	zfi.external_fa = ::GetFileAttributes(szFilePath);
	
	// save file time
	SYSTEMTIME st;

	GetLastModified(szFullFilePath, st);

	zfi.dosDate = 0;
	zfi.tmz_date.tm_year = st.wYear;
	zfi.tmz_date.tm_mon = st.wMonth - 1;
	zfi.tmz_date.tm_mday = st.wDay;
	zfi.tmz_date.tm_hour = st.wHour;
	zfi.tmz_date.tm_min = st.wMinute;
	zfi.tmz_date.tm_sec = st.wSecond;

	// load input file
	HANDLE hInputFile = ::CreateFile(szFullFilePath, 
									GENERIC_READ,
									0,
									NULL,
									OPEN_EXISTING,
									FILE_ATTRIBUTE_READONLY,
									NULL);

	if (hInputFile == INVALID_HANDLE_VALUE)
		return false;

	// strip drive info off filepath
	char szName[_MAX_FNAME], szExt[_MAX_EXT];
	_splitpath_s( szFilePath, NULL, 0, NULL, 0, szName, _MAX_FNAME, szExt, _MAX_EXT );

	// prepend new folder path 
	char szFileName[MAX_PATH];
	_makepath_s(szFileName, MAX_PATH, NULL, szRelFolderPath, szName, szExt);

	PrepareZipPath(szFileName);

	// open the file in the zip making sure we remove any leading '\'
	int nRet = zipOpenNewFileInZip(m_uzFile, 
									szFileName,
									&zfi, 
									NULL, 
									0,
									NULL,
									0, 
									NULL,
									Z_DEFLATED,
									Z_DEFAULT_COMPRESSION);

	if (nRet == ZIP_OK)
	{
		m_info.nFileCount++;

		// read the file and output to zip
		char pBuffer[ZIP_BUFFER_SIZE];
		DWORD dwBytesRead = 0, dwFileSize = 0;

		while (nRet == ZIP_OK && ::ReadFile(hInputFile, pBuffer, ZIP_BUFFER_SIZE, &dwBytesRead, NULL))
		{
			dwFileSize += dwBytesRead;

			if (dwBytesRead)
				nRet = zipWriteInFileInZip(m_uzFile, pBuffer, dwBytesRead);
			else
				break;
		}

		m_info.dwUncompressedSize += dwFileSize;
	}

	zipCloseFileInZip(m_uzFile);
	::CloseHandle(hInputFile);

	return (nRet == ZIP_OK);
}

bool CZipper::AddFileToZip(const BYTE* pFileContents, int nSize, LPCTSTR szRelFilePath)
{
	if (!m_uzFile)
		return false;

	// szRelFilePath cannot contain drive info
	if (szRelFilePath && strchr(szRelFilePath, ':'))
		return false;

	// save file attributes and time
	zip_fileinfo zfi;

	zfi.internal_fa = 0;
	zfi.external_fa = FILE_ATTRIBUTE_NORMAL;
	
	// use time now
	SYSTEMTIME st;

	GetSystemTime(&st);

	zfi.dosDate = 0;
	zfi.tmz_date.tm_year = st.wYear;
	zfi.tmz_date.tm_mon = st.wMonth - 1;
	zfi.tmz_date.tm_mday = st.wDay;
	zfi.tmz_date.tm_hour = st.wHour;
	zfi.tmz_date.tm_min = st.wMinute;
	zfi.tmz_date.tm_sec = st.wSecond;

	// open the file in the zip making sure we remove any leading '\'
	char szFilePath[MAX_PATH];

	lstrcpy(szFilePath, szRelFilePath);
	PrepareZipPath(szFilePath);

	int nRet = zipOpenNewFileInZip(m_uzFile, 
									szFilePath,
									&zfi, 
									NULL, 
									0,
									NULL,
									0, 
									NULL,
									Z_DEFLATED,
									Z_DEFAULT_COMPRESSION);

	if (nRet == ZIP_OK)
	{
		m_info.nFileCount++;

		// read the file and output to zip
		const BYTE* pBuf = pFileContents;

		do
		{
			DWORD dwBytesRead = min(ZIP_BUFFER_SIZE, pFileContents + nSize - pBuf);
			nRet = zipWriteInFileInZip(m_uzFile, pBuf, dwBytesRead);

			pBuf += dwBytesRead;
		}
		while (nRet == ZIP_OK && pBuf < pFileContents + nSize);

		m_info.dwUncompressedSize += nSize;
	}

	zipCloseFileInZip(m_uzFile);

	return (nRet == ZIP_OK);
}

bool CZipper::AddFolderToZip(LPCTSTR szFolderPath, bool bIgnoreFilePath )
{
	if (!m_uzFile)
		return false;

	m_info.nFolderCount++;

	// if the path is relative then we need to append the root before opening
	char szFullPath[MAX_PATH];
	
	lstrcpy(szFullPath, szFolderPath);
	PrepareSourcePath(szFullPath);

	// always add folder first
	// save file attributes
	zip_fileinfo zfi;
	
	zfi.internal_fa = 0;
	zfi.external_fa = ::GetFileAttributes(szFullPath);
	
	SYSTEMTIME st;
	
	GetLastModified(szFullPath, st);
	
	zfi.dosDate = 0;
	zfi.tmz_date.tm_year = st.wYear;
	zfi.tmz_date.tm_mon = st.wMonth - 1;
	zfi.tmz_date.tm_mday = st.wDay;
	zfi.tmz_date.tm_hour = st.wHour;
	zfi.tmz_date.tm_min = st.wMinute;
	zfi.tmz_date.tm_sec = st.wSecond;
	
	// if the folder is a fullpath then remove the root path bit
	char szFolderName[MAX_PATH] = "";
	
	if (bIgnoreFilePath)
	{
		char szExt[_MAX_EXT];
		_splitpath_s( szFullPath, NULL, 0, NULL, 0, szFolderName, MAX_PATH, szExt, _MAX_EXT );

		lstrcat(szFolderName, szExt);
	}
	else
	{
		// check the root can be found
		if (0 != _strnicmp(szFullPath, m_szRootFolder, lstrlen(m_szRootFolder)))
			return false;
		
		// else
		lstrcpy(szFolderName, szFullPath + lstrlen(m_szRootFolder));
	}
	
	// open the file in the zip making sure we remove any leading '\'
	// provided that the folder name is not empty.
	// note: its ok for it to be empty if this folder coincides with the root folder
	PrepareZipPath(szFolderName);

	if (lstrlen(szFolderName))
	{
		int nRet = zipOpenNewFileInZip(m_uzFile, 
										szFolderName,
										&zfi, 
										NULL, 
										0,
										NULL,
										0, 
										NULL,
										Z_DEFLATED,
										Z_DEFAULT_COMPRESSION);
		
		zipCloseFileInZip(m_uzFile);
	}

	// build searchspec
	char szDrive[_MAX_DRIVE], szFolder[MAX_PATH], szName[_MAX_FNAME], szExt[_MAX_EXT];
	_splitpath_s( szFullPath, szDrive, _MAX_DRIVE, szFolder, MAX_PATH, szName, _MAX_FNAME, szExt, _MAX_EXT );

	lstrcat(szFolder, szName);
	lstrcat(szFolder, szExt);

	char szSearchSpec[MAX_PATH];
	_makepath_s(szSearchSpec, MAX_PATH, szDrive, szFolder, "*", "*");

	WIN32_FIND_DATA finfo;
	HANDLE hSearch = FindFirstFile(szSearchSpec, &finfo);

	if (hSearch != INVALID_HANDLE_VALUE) 
	{
		do 
		{
			if (finfo.cFileName[0] != '.') 
			{
				char szItem[MAX_PATH];
				_makepath_s(szItem, MAX_PATH, szDrive, szFolder, finfo.cFileName, NULL);
				
				if (finfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					AddFolderToZip(szItem, bIgnoreFilePath);
				}
				else 
					AddFileToZip(szItem, bIgnoreFilePath, NULL);
			}
		} 
		while (FindNextFile(hSearch, &finfo));
		
		FindClose(hSearch);
	}

	return TRUE;
}

// extended interface
bool CZipper::OpenZip(LPCTSTR szFilePath, LPCTSTR szRootFolder, bool bAppend)
{
	CloseZip();
	ZeroMemory(&m_info, sizeof(m_info));

	if (!szFilePath || !lstrlen(szFilePath))
		return false;

	// convert szFilePath to fully qualified path 
	char szFullPath[MAX_PATH];

	if (!GetFullPathName(szFilePath, MAX_PATH, szFullPath, NULL))
		return false;

	// zipOpen will fail if bAppend is TRUE and zip does not exist
	if (bAppend && ::GetFileAttributes(szFullPath) == 0xffffffff)
		bAppend = false;

	m_uzFile = zipOpen(szFullPath, bAppend ? APPEND_STATUS_ADDINZIP : APPEND_STATUS_CREATE);

	if (m_uzFile)
	{
		if (!szRootFolder)
		{
			char szDrive[_MAX_DRIVE], szFolder[MAX_PATH];
			_splitpath_s( szFullPath, szDrive, _MAX_DRIVE, szFolder, MAX_PATH, NULL, 0, NULL, 0 );

			_makepath_s(m_szRootFolder, MAX_PATH, szDrive, szFolder, NULL, NULL);
		}
		else if (lstrlen(szRootFolder))
		{
			_makepath_s(m_szRootFolder, MAX_PATH, NULL, szRootFolder, NULL, NULL);
		}

		// remove any trailing whitespace and '\'
		UnterminatePath(m_szRootFolder);
	}

	return (m_uzFile != NULL);
}

void CZipper::UnterminatePath(LPTSTR szPath)
{
	int nEnd = lstrlen(szPath) - 1;

	while (isspace(szPath[nEnd]))
		nEnd--;

	while (szPath[nEnd] == '\\')
		nEnd--;

	szPath[nEnd + 1] = 0;
}

void CZipper::PrepareZipPath(LPTSTR szPath)
{
	UnterminatePath(szPath);

	// remove leading whitespace and '\'
	char szTemp[MAX_PATH];
	lstrcpy(szTemp, szPath);

	int nStart = 0;

	while (isspace(szTemp[nStart]))
		nStart++;

	while (szTemp[nStart] == '\\')
		nStart++;

	if (nStart)
		lstrcpy(szPath, szTemp + nStart);
}

void CZipper::PrepareSourcePath(LPTSTR szPath)
{
	bool bFullPath = (strchr(szPath, ':') != NULL);

	// if the file is relative then we need to append the root before opening
	if (!bFullPath)
	{
		char szTemp[MAX_PATH];
		lstrcpy(szTemp, szPath);

		_makepath_s(szPath, MAX_PATH, NULL, m_szRootFolder, szTemp, NULL);
	}
}

bool CZipper::GetLastModified(const char* szPath, SYSTEMTIME& sysTime)
{
	ZeroMemory(&sysTime, sizeof(SYSTEMTIME));

	DWORD dwAttr = ::GetFileAttributes(szPath);

	// files only
	if (dwAttr == 0xFFFFFFFF)
		return false;

	WIN32_FIND_DATA findFileData;
	HANDLE hFind = FindFirstFile((LPTSTR)szPath, &findFileData);

	if (hFind == INVALID_HANDLE_VALUE)
		return false;

	FindClose(hFind);

	FILETIME ft = findFileData.ftLastWriteTime;

	FileTimeToLocalFileTime(&findFileData.ftLastWriteTime, &ft);
	FileTimeToSystemTime(&ft, &sysTime);

	return true;
}


/////////////////////////////////// // Ivan A. Krestinin // // Crown_s Soft // // http://www.crown-s-soft.com // /////////////////////////////////// 
//	C++로 수정후에 테스트 안함 사용하기전에 반드시 테스트

int CZipper::DeleteFileFromZIP(LPCTSTR zip_name, LPCTSTR del_file) 
{ 
	bool some_was_del = false; 
	// change name for temp file 
	
//	char* tmp_name = (char*)malloc((strlen(zip_name) + 5)*sizeof(char)); 
	char* tmp_name = new char[ strlen(zip_name) + 5 ];

	StringCchCopy( tmp_name, strlen( zip_name ) + 1, zip_name );
	StringCchCat( tmp_name, strlen(".tmp"), ".tmp" );
//	strcpy(tmp_name, zip_name); 	
//	strncat(tmp_name, ".tmp", 5); 
	
	// open source and destination file 
//	zlib_filefunc_def ffunc; 
//	fill_win32_filefunc( &ffunc );
	
	zipFile szip = unzOpen2(zip_name, NULL); 
	if (szip==NULL) 
	{ 
		//free(tmp_name); 
		SAFE_DELETE_ARRAY( tmp_name );
		return -1; 
	} 

	zipFile dzip = zipOpen2(tmp_name, APPEND_STATUS_CREATE, NULL, NULL); 
	if (dzip==NULL) 
	{ 
		unzClose(szip); 
//		free(tmp_name); 
		SAFE_DELETE_ARRAY( tmp_name );
		return -1; 
	} 
	
	// get global commentary 
	unz_global_info glob_info; 	
	if (unzGetGlobalInfo(szip, &glob_info) != UNZ_OK) 
	{ 
		zipClose(dzip, NULL); 
		unzClose(szip); 
//		free(tmp_name); 
		SAFE_DELETE_ARRAY( tmp_name );
		return -1; 
	} 
	
	char* glob_comment = NULL; 
	
	if (glob_info.size_comment > 0) 
	{ 
//		glob_comment = (char*)malloc(glob_info.size_comment+1); 
		glob_comment = new char [ glob_info.size_comment+1];
		
		if ((glob_comment==NULL)&&(glob_info.size_comment!=0)) 
		{ 
			zipClose(dzip, NULL); 
			unzClose(szip); 
//			free(tmp_name); 
			SAFE_DELETE_ARRAY( tmp_name );
			return -1; 
		} 
		
		if ((unsigned int)unzGetGlobalComment(szip, glob_comment, glob_info.size_comment+1) != glob_info.size_comment) 
		{ 
			zipClose(dzip, NULL); 
			unzClose(szip); 
//			free(glob_comment); 
//			free(tmp_name); 
			SAFE_DELETE_ARRAY( glob_comment );
			SAFE_DELETE_ARRAY( tmp_name );
			return -1; 
		} 
	} 
	
	// copying files 
	
	int n_files = 0; 
	int rv = unzGoToFirstFile(szip); 
	while (rv == UNZ_OK) 
	{ 
		// get zipped file info 
		unz_file_info unzfi; 
		char dos_fn[MAX_PATH]; 
		if (unzGetCurrentFileInfo(szip, &unzfi, dos_fn, MAX_PATH, NULL, 0, NULL, 0) != UNZ_OK) 
			break; 
		
		char fn[MAX_PATH]; 
		OemToChar(dos_fn, fn); 
		// if not need delete this file 
		
		if (_stricmp(fn, del_file)==0) // lowercase comparison 
			some_was_del = true; 
		else 
		{ 
//			char* extrafield = (char*)malloc(unzfi.size_file_extra); 
			char* extrafield = new char[ unzfi.size_file_extra ]; 
			
			if ((extrafield==NULL)&&(unzfi.size_file_extra!=0)) 
				break; 
			
//			char* commentary = (char*)malloc(unzfi.size_file_comment); 
			char* commentary = new char[ unzfi.size_file_comment ]; 
			
			if ((commentary==NULL)&&(unzfi.size_file_comment!=0)) 
			{
//				free(extrafield); 
				SAFE_DELETE_ARRAY( extrafield );
				break;
			} 
			
			if (unzGetCurrentFileInfo(szip, &unzfi, dos_fn, MAX_PATH, extrafield, unzfi.size_file_extra, commentary, unzfi.size_file_comment) != UNZ_OK) 
			{
//				free(extrafield); 
//				free(commentary); 
				SAFE_DELETE_ARRAY( extrafield );
				SAFE_DELETE_ARRAY( commentary );
				break;
			} 
			
			// open file for RAW reading 
			int method; 
			int level; 
			
			if (unzOpenCurrentFile2(szip, &method, &level, 1)!=UNZ_OK) 
			{
//				free(extrafield); 
//				free(commentary); 
				SAFE_DELETE_ARRAY( extrafield );
				SAFE_DELETE_ARRAY( commentary );
				break;
			} 
			
			int size_local_extra = unzGetLocalExtrafield(szip, NULL, 0); 
			
			if (size_local_extra<0) 
			{
//				free(extrafield); 
//				free(commentary); 
				SAFE_DELETE_ARRAY( extrafield );
				SAFE_DELETE_ARRAY( commentary );
				break;
			} 
			
//			void* local_extra = malloc(size_local_extra); 
			char* local_extra = new char [ size_local_extra ]; 
			
			if ((local_extra==NULL)&&(size_local_extra!=0)) 
			{
//				free(extrafield); 
//				free(commentary); 
				SAFE_DELETE_ARRAY( extrafield );
				SAFE_DELETE_ARRAY( commentary );
				break;
			} 
			
			if (unzGetLocalExtrafield(szip, local_extra, size_local_extra)<0) 
			{
//				free(extrafield); 
//				free(commentary); 
//				free(local_extra); 
				SAFE_DELETE_ARRAY( extrafield );
				SAFE_DELETE_ARRAY( commentary );
				SAFE_DELETE_ARRAY( local_extra );
				break;
			} 
			
			// this malloc may fail if file very large 
			int nSize = min( unzfi.uncompressed_size, unzfi.compressed_size );
			
//			void* buf = malloc(unzfi.compressed_size); 
//			void* buf = malloc(nSize);
			char* buf = new char[nSize];
			
//			if ((buf==NULL)&&(unzfi.compressed_size!=0)) 
			if ((buf==NULL)&&(nSize!=0)) 
			{
//				free(extrafield); 
//				free(commentary); 
//				free(local_extra); 
				SAFE_DELETE_ARRAY( extrafield );
				SAFE_DELETE_ARRAY( commentary );
				SAFE_DELETE_ARRAY( local_extra );
				break;
			} 
			
			// read file 
			
//			int sz = unzReadCurrentFile(szip, buf, unzfi.compressed_size); 
			int sz = unzReadCurrentFile(szip, buf, nSize); 
			
//			if ((unsigned int)sz != unzfi.compressed_size) 
			if ((unsigned int)sz != nSize) 
			{
//				free(extrafield); 
//				free(commentary); 
//				free(local_extra); 
//				free(buf); 

				SAFE_DELETE_ARRAY( extrafield );
				SAFE_DELETE_ARRAY( commentary );
				SAFE_DELETE_ARRAY( local_extra );
				SAFE_DELETE_ARRAY( buf );

				break;			
			} 
			
			// open destination file 
			zip_fileinfo zfi; 
			
			memcpy (&zfi.tmz_date, &unzfi.tmu_date, sizeof(tm_unz)); 
			zfi.dosDate = unzfi.dosDate; 
			zfi.internal_fa = unzfi.internal_fa; 
			zfi.external_fa = unzfi.external_fa; 
			
			if (zipOpenNewFileInZip2(dzip, dos_fn, &zfi, local_extra, size_local_extra, extrafield, unzfi.size_file_extra, commentary, method, level, 1)!=UNZ_OK) 
			{
//				free(extrafield); 
//				free(commentary); 
//				free(local_extra); 
//				free(buf); 

				SAFE_DELETE_ARRAY( extrafield );
				SAFE_DELETE_ARRAY( commentary );
				SAFE_DELETE_ARRAY( local_extra );
				SAFE_DELETE_ARRAY( buf );
				break;
			} 
			// write file 
			
			if (zipWriteInFileInZip(dzip, buf, unzfi.compressed_size)!=UNZ_OK) 
			{
//				free(extrafield); 
//				free(commentary); 
//				free(local_extra); 
//				free(buf); 

				SAFE_DELETE_ARRAY( extrafield );
				SAFE_DELETE_ARRAY( commentary );
				SAFE_DELETE_ARRAY( local_extra );
				SAFE_DELETE_ARRAY( buf );
				break;
			} 
			
			if (zipCloseFileInZipRaw(dzip, unzfi.uncompressed_size, unzfi.crc)!=UNZ_OK) 
			{
//				free(extrafield); 
//				free(commentary); 
//				free(local_extra); 
//				free(buf); 
				SAFE_DELETE_ARRAY( extrafield );
				SAFE_DELETE_ARRAY( commentary );
				SAFE_DELETE_ARRAY( local_extra );
				SAFE_DELETE_ARRAY( buf );
				break;
			} 
			
			if (unzCloseCurrentFile(szip)==UNZ_CRCERROR) 
			{
//				free(extrafield); 
//				free(commentary); 
//				free(local_extra); 
//				free(buf); 
				SAFE_DELETE_ARRAY( extrafield );
				SAFE_DELETE_ARRAY( commentary );
				SAFE_DELETE_ARRAY( local_extra );
				SAFE_DELETE_ARRAY( buf );
				break;
			} 
			
//			free(commentary); 
//			free(buf); 
//			free(extrafield); 
//			free(local_extra); 
			SAFE_DELETE_ARRAY( extrafield );
			SAFE_DELETE_ARRAY( commentary );
			SAFE_DELETE_ARRAY( local_extra );
			SAFE_DELETE_ARRAY( buf );
			n_files ++; 
		} 
		
		rv = unzGoToNextFile(szip); 
	} 
	
	zipClose(dzip, glob_comment); 
	unzClose(szip); 

//	free(glob_comment); 
	SAFE_DELETE_ARRAY( glob_comment );

	// if fail
	
	if ( (!some_was_del) || (rv!=UNZ_END_OF_LIST_OF_FILE) ) 
	{ 
		remove(tmp_name); 
//		free(tmp_name); 
		SAFE_DELETE_ARRAY( tmp_name );
		return -1; 
	} 
	
	remove(zip_name); 
	
	if (rename(tmp_name, zip_name) != 0) 
	{ 
//		free(tmp_name); 
		SAFE_DELETE_ARRAY( tmp_name );
		return -1; 
	} 
	// if all files were deleted 
	
	if (n_files==0) 
		remove(zip_name); 
	
//	free(tmp_name); 
	SAFE_DELETE_ARRAY( tmp_name );
	
	return 0; 
} 