// LPatchThread.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "LPatchThread.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CLPatchThread, CWinThread)

CLPatchThread::CLPatchThread() :
	m_bFail(FALSE),
	m_bExtractError(FALSE)
{
	m_bAutoDelete = FALSE;

	m_hEventKill = CreateEvent( NULL, TRUE, FALSE, NULL );
	m_hEventDead = CreateEvent( NULL, TRUE, FALSE, NULL );
}

CLPatchThread::~CLPatchThread()
{
	CloseHandle( m_hEventKill );
	CloseHandle( m_hEventDead );
}

BOOL CLPatchThread::InitInstance()
{
	ThreadMain();

	return FALSE; // �޼��� ������ �����ϱ� ���ؼ� TRUE�� �ƴ� FALSE�� �ش�.
}

void CLPatchThread::Delete() // delete ������ ������
{
	CWinThread::Delete();

	VERIFY( SetEvent( m_hEventDead ) );
}

void CLPatchThread::KillThread()
{
	VERIFY( SetEvent( m_hEventKill ));

	SetThreadPriority( THREAD_PRIORITY_ABOVE_NORMAL );
	WaitForSingleObject( m_hEventDead, INFINITE );
	WaitForSingleObject( m_hThread, INFINITE );

	delete this;
}

void CLPatchThread::ForceKillThread()
{
	DWORD dwExitCode;
	::GetExitCodeThread(m_hThread, &dwExitCode);
	
	if(dwExitCode == STILL_ACTIVE) // �����尡 ���� ������ �˻�
		::TerminateThread(m_hThread, 0);

	delete this;
}

BOOL CLPatchThread::IsRunning ()
{
	return WaitForSingleObject( m_hEventDead, 0 ) == WAIT_TIMEOUT;
}

BOOL CLPatchThread::IsForceTerminate ()
{
	return !(WaitForSingleObject( m_hEventKill, 0 ) == WAIT_TIMEOUT);
}

BEGIN_MESSAGE_MAP(CLPatchThread, CWinThread)
END_MESSAGE_MAP()


// CLPatchThread �޽��� ó�����Դϴ�.