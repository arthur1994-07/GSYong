#include "stdafx.h"
#include "AutoPatchMan.h"
#include "AutoPatchManDlg.h"

#include "PatchThread.h"
#include "WebBrowserThread.h"
#include "ServerLoginThread.h"
#include "AutoPatchThread.h"
#include "HashCheckThread.h"
#include "../SigmaCore/gassert.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void CAutoPatchManDlg::BeginThread(
    INT nThreadType,
    LPCLPATCHTHREAD& pThread,
    LPVOID pParam,
    int LoginPort,
    const CString& strParam)
{
	if( pThread != NULL )
	{
		GASSERT( 0 && "BeginThread()" );
		return;
	}

    CAutoPatchManApp* pApp = (CAutoPatchManApp*) AfxGetApp();
    
	switch( nThreadType )
	{
	case E_THREAD_WEB:
		pThread = new CWebBrowserThread(pParam, strParam);
		break;
	case E_THREAD_LOGIN:
		pThread = new CServerLoginThread(pParam, strParam, AfxGetThread()->m_nThreadID, LoginPort);
		break;
	case E_THREAD_PATCH:
		pThread = new CAutoPatchThread(pParam, AfxGetThread()->m_nThreadID);
		break;
	case E_THREAD_HASH:
		pThread = new CHashCheckThread(pParam, AfxGetThread()->m_nThreadID);
		break;
	default:
		GASSERT(0 && "BeginThread()");
		return;
	}

	if( pThread == NULL )
		return;

	ASSERT_VALID(pThread);
	pThread->m_pThreadParams = NULL;

	if ( !pThread->CreateThread(CREATE_SUSPENDED) )
	{
		delete pThread;
		return;
	}

	VERIFY(pThread->SetThreadPriority(THREAD_PRIORITY_IDLE));
	pThread->ResumeThread();
}

void CAutoPatchManDlg::EndThread( LPCLPATCHTHREAD & pThread )
{
	if( pThread != NULL )
	{
		pThread->KillThread();
		pThread = NULL;
	}
}

void CAutoPatchManDlg::ForceKillThread( LPCLPATCHTHREAD & pThread )
{
	if( pThread != NULL )
	{
		pThread->ForceKillThread();
		pThread = NULL;
	}
}