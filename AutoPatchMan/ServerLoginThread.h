#pragma once

#include "LPatchThread.h"

class	CNetClient;
class CHttpPatch;

class CServerLoginThread : public CLPatchThread
{
	DECLARE_DYNCREATE(CServerLoginThread)

protected:
	CNetClient* m_pNetClient;
	CString m_strLoginAddress;
	DWORD m_nDlgThreadID;
    int m_LoginPort;

	CHttpPatch* m_pHttpPatch;

public:
	CServerLoginThread(LPVOID pParam, const CString& strLoginAddress, DWORD nThreadID, int LoginPort);

protected:
	virtual void ThreadMain();
	BOOL GETFILE_USEHTTP( CHttpPatch* pHttpPatch, std::string strRemoteSubPath, std::string strFileName, CString strTempDir );

public:
	CServerLoginThread(){}
	virtual ~CServerLoginThread(){ SAFE_DELETE( m_pHttpPatch ); }

protected:
	DECLARE_MESSAGE_MAP()
};
