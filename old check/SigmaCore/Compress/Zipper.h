// Zipper.h: interface for the CZipper class.
//
//////////////////////////////////////////////////////////////////////

#ifndef SC_ZIPPER_H_
#define SC_ZIPPER_H_

#pragma once

struct Z_FileInfo
{
	int nFileCount;
	int nFolderCount;
	DWORD dwUncompressedSize;
};

class CZipper  
{
public:
	CZipper(LPCTSTR szFilePath = NULL, LPCTSTR szRootFolder = NULL, bool bAppend = FALSE);
	virtual ~CZipper();

	// simple interface
	static bool ZipFile(LPCTSTR szFilePath); // saves as same name with .zip
	static bool ZipFolder(LPCTSTR szFilePath, bool bIgnoreFilePath); // saves as same name with .zip
	
	// works with prior opened zip
	bool AddFileToZip(LPCTSTR szFilePath, bool bIgnoreFilePath, const char* szPass);
	bool AddFileToZip(LPCTSTR szFilePath, LPCTSTR szRelFolderPath); // replaces path info from szFilePath with szFolder
	bool AddFileToZip(const BYTE* pFileContents, int nSize, LPCTSTR szRelFilePath);
	bool AddFolderToZip(LPCTSTR szFolderPath, bool bIgnoreFilePath = FALSE );

	int DeleteFileFromZIP(LPCTSTR zip_name, LPCTSTR del_file);

	// extended interface
	bool OpenZip(LPCTSTR szFilePath, LPCTSTR szRootFolder = NULL, bool bAppend = FALSE);
	bool CloseZip(); // for multiple reuse
	void GetFileInfo(Z_FileInfo& info);
	
protected:
	void* m_uzFile;
	char m_szRootFolder[MAX_PATH + 1];
	Z_FileInfo m_info;

protected:
	void PrepareSourcePath(LPTSTR szPath);
	bool GetLastModified(const char* szPath, SYSTEMTIME& sysTime);

	void UnterminatePath(LPTSTR szPath);
	void PrepareZipPath(LPTSTR szPath);

};

#endif // SC_ZIPPER_H_
