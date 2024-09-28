// LPatchThread.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "LPatchThread.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CLPatchThread, CWinThread)

CLPatchThread::CLPatchThread() :
m_bFail(FALSE),
m_bComplete(FALSE),
m_bExtractError(FALSE)
{
	m_bAutoDelete = FALSE;

	m_hEventKill = CreateEvent( NULL, TRUE, FALSE, NULL );
	m_hEventDead = CreateEvent( NULL, TRUE, FALSE, NULL );
	m_hEventResume = CreateEvent( NULL, TRUE, FALSE, NULL );
	SetEvent( m_hEventResume );

	m_nDownloadFileCount = 0;
	m_nDownloadedFileCount = 0;
}

CLPatchThread::~CLPatchThread()
{
	CloseHandle( m_hEventKill );
	CloseHandle( m_hEventDead );
	CloseHandle( m_hEventResume );
}

BOOL CLPatchThread::InitInstance()
{
	ThreadMain();

	return FALSE; // 메세지 루프를 종료하기 위해서 TRUE가 아닌 FALSE를 준다.
}

void CLPatchThread::Pause()
{
	VERIFY( ResetEvent( m_hEventResume ) );
}

void CLPatchThread::Resume()
{
	VERIFY( SetEvent( m_hEventResume ) );
}

void CLPatchThread::Delete()
{
	CWinThread::Delete();

	VERIFY( SetEvent( m_hEventDead ) );
}

void CLPatchThread::KillThread()
{
	VERIFY( SetEvent( m_hEventKill ));
	VERIFY( SetEvent( m_hEventResume ));

	SetThreadPriority( THREAD_PRIORITY_ABOVE_NORMAL );
	WaitForSingleObject( m_hEventDead, INFINITE );
	WaitForSingleObject( m_hThread, INFINITE );

	delete this;
}

void CLPatchThread::ForceKillThread()
{
	DWORD dwExitCode;
	::GetExitCodeThread(m_hThread, &dwExitCode);
	
	if(dwExitCode == STILL_ACTIVE) // 스레드가 실행 중인지 검사
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

BOOL CLPatchThread::IsPause ()
{
	return WaitForSingleObject( m_hEventResume, 0 ) == WAIT_TIMEOUT;
}

void CLPatchThread::WatingForResume ()
{
	WaitForSingleObject( m_hEventResume, INFINITE );
}

BEGIN_MESSAGE_MAP(CLPatchThread, CWinThread)
END_MESSAGE_MAP()


// CLPatchThread 메시지 처리기입니다.
