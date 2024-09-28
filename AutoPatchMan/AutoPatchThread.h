#pragma once

#include "../SigmaCore/File/SFileSystem.h"
#include "./HttpThread.h"

class CHttpPatch;

class CAutoPatchThread : public CHttpThread
{
	DECLARE_DYNCREATE(CAutoPatchThread)

protected:
	FILEVECTOR	m_vectorNewFile;
	FILEVECTOR	m_vectorDeleteFile;

	FILEVECTOR	m_vectorDownFile;
	FILEMAP		m_mapDownedFile;
	FILEVECTOR	m_vectorCopyFile;
	FILEMAP		m_mapCopiedFile;
	FILEVECTOR	m_vectorPackFile;

	std::vector<std::string> m_vecZipFile;

//	FILEVECTOR	m_vecCompFile;
	SFileSystem m_FileSystem;

public:
	CAutoPatchThread(LPVOID pParam, DWORD nThreadID);

protected:
	virtual void ThreadMain();

protected:
	BOOL DownloadFilesByHttp( CHttpPatch* pHttpPatch );
//	BOOL CompressFile();
	BOOL Installing();
	BOOL DeleteNotFoundFile();
	BOOL DeleteDownFiles();
	BOOL UnPackageFile();
	BOOL PackageFile();
	BOOL DeleteUnPackFile();
//	BOOL DeleteCompressFile();
//	BOOL OptimizeCompressFile();
	BOOL Destroy();
	BOOL VersionUp( int sGameVer );
	BOOL CheckHashFile();
	BOOL MakeNewList( const int cPatchVer, const int sPatchVer, const int cGameVer, const int sGameVer );
	BOOL ApplyPreDownFile();
	BOOL CreateClientFileList();
	void SaveVersionInRegistry( int sGameVer );

protected:
	BOOL SaveDownList( int sGameVer );
	BOOL LoadDownList();

	BOOL SaveCopyList( int sGameVer );
	BOOL LoadCopyList();

public:
	CAutoPatchThread(){}
	virtual ~CAutoPatchThread(){}

protected:
	DECLARE_MESSAGE_MAP()
};
