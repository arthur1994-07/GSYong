#pragma once

#include "./LPatchThread.h"

class CHttpPatch;
struct	S_PATCH_THREAD_PARAM;

class CHttpPatchThread : public CLPatchThread
{
	DECLARE_DYNCREATE(CHttpPatchThread)

public:
    CHttpPatchThread( LPVOID pParam, const TCHAR * szAppPath, DWORD nThreadID, SUBPATH* pPath );
    CHttpPatchThread(){}
    virtual ~CHttpPatchThread(){}	

protected:
	virtual void LauncherPatch();
	BOOL GETFILE_USEHTTP( CHttpPatch* pHttpPatch, std::string strRemoteSubPath, std::string strFileName, CString strTempDir );

protected:
	DECLARE_MESSAGE_MAP()
};
