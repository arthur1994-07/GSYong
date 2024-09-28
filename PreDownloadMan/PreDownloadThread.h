#pragma once

#include "../SigmaCore/File/SFileSystem.h"
#include "./PreHttpThread.h"

class CPreDownloadThread : public CPreHttpThread
{
	DECLARE_DYNCREATE(CPreDownloadThread)

public:
	CPreDownloadThread( CHttpPatch* pParam, DWORD nThreadID );
	CPreDownloadThread(){}
	virtual ~CPreDownloadThread(){}

protected:
	virtual void ThreadMain();

	BOOL DownloadFilesByHttp( CHttpPatch* pHttpPatch );
	BOOL GETFILE_USEHTTP( std::string strRemoteSubPath, std::string strFileName, CString strTempDir = PreDownloadDefine::g_strPreDownloadDirectory  );

	BOOL DeleteIsNotDownFile();
	BOOL DeleteUnPackFile();

	BOOL Destroy();

	BOOL MakeNewList();

	BOOL SaveDownList();
	BOOL LoadDownList();

	void PreDownLoadComplete();

	// �ٿ�ε� �ߴ� �� ��� ������� ���� ���ϸ�� ����;
	void PreDownLoadFail();

protected:
	DECLARE_MESSAGE_MAP()

protected:
	FILEVECTOR	m_vectorNewFile;
	FILEVECTOR	m_vectorDeleteFile;

	FILEVECTOR	m_vectorDownFile;
	FILEMAP		m_mapDownedFile;

	SFileSystem m_FileSystem;

	std::vector<std::string> m_vecZipFile;

	CString m_strPreFileList;
};
