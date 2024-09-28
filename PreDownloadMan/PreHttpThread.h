#pragma once

#include "./LPatchThread.h"
#include "./PatchSet.h"
#include "./PreDownloadDefine.h"

class SUBPATH;

class CPreHttpThread : public CLPatchThread
{
	DECLARE_DYNCREATE(CPreHttpThread)

protected:
	BOOL Initialize()		{ return TRUE; }
	BOOL LoadList();
	BOOL LoadServerPreFileList();
	BOOL LoadClientPreFileList();

	BOOL GETFILE_USEHTTP( std::string strRemoteSubPath, std::string strFileName, CString strTempDir = PreDownloadDefine::g_strPreDownloadDirectory );

public:
	CPreHttpThread();
	virtual ~CPreHttpThread();

protected:
	CHttpPatch*	m_pHttpPatch;
	DWORD m_nDlgThreadID;

	FILEVECTOR	m_vectorServerFile;
	CPatchSet	m_ClientFileTree;

protected:
	DECLARE_MESSAGE_MAP()
};
