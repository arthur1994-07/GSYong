#include "stdafx.h"
#include <process.h>
#include "../../BugTrap/BugTrap.h"
#include "./FrameThread.h"

#include "../DebugInclude.h"

using namespace sc;

FrameThread::FrameThread(DWORD dwFrame, DWORD dwIdealCpu)
	: m_hThread( NULL )
	, m_bRun( false )
	, m_dwWaitTime( 0 )	
	, m_dwOldTime( 0 )
{
	m_hQuitEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);	
	createThread(dwFrame, dwIdealCpu);
}

FrameThread::~FrameThread(void)
{
	if (isRun())
		destroyThread();

	if (m_hQuitEvent)
		::CloseHandle(m_hQuitEvent);
	if (m_hThread)
		::CloseHandle(m_hThread);
}

unsigned int WINAPI callFrameThread(void* pThread)
{
	if (pThread)
	{
		FrameThread* pFrameThread = (FrameThread*) pThread;
		pFrameThread->frameThread();
		return 1;
	}
	else
	{
		return 0;
	}
}

void FrameThread::setFrame(DWORD dwFrame)
{
	m_dwFrame = dwFrame;
	m_dwWaitTime = 1000/m_dwFrame;
}

bool FrameThread::createThread(DWORD dwFrame, DWORD dwIdealCpu)
{
	unsigned int ThreadID=0;
	m_hThread = (HANDLE) _beginthreadex(
		NULL,
		0,
		&callFrameThread,
		this,
		CREATE_SUSPENDED,
		&ThreadID);
	if (!m_hThread)
	{
		return false;
	}
	setFrame(dwFrame);
	m_dwOldTime = ::GetTickCount();
	if (dwIdealCpu > 0)
	{
		::SetThreadIdealProcessor(m_hThread, dwIdealCpu);
	}

	resume();
	return true;
}

void FrameThread::destroyThread()
{
	resume();
	::SetEvent(m_hQuitEvent);
	m_bRun = false;	
	::WaitForSingleObject(m_hThread, INFINITE);
	SAFE_CLOSE_HANDLE(m_hThread);
}

void FrameThread::resume()
{
	if (!m_bRun)
	{
		m_bRun = true;
		::ResumeThread(m_hThread);
	}
}

void FrameThread::stop()
{
	if (m_bRun)
	{
		m_bRun = false;
		::SuspendThread(m_hThread);
	}
}

void FrameThread::frameThread()
{
	BT_SetTerminate();

	DWORD dwNewTime = 0;
	DWORD dwReturn = 0;
	DWORD dwNewWaitTime = 0;
	DWORD dwFrameTime = 0;
	int nElaspTime = 0;
	while (true)
	{
		// Frame 경과시간
		dwNewTime   = ::GetTickCount();
		dwFrameTime = dwNewTime - m_dwOldTime;
		m_dwOldTime = dwNewTime;
		// 경과시간 체크
		nElaspTime = (int)(m_dwWaitTime - dwFrameTime);

		if ( 0 >= nElaspTime )
			dwNewWaitTime = 0;
		else
			dwNewWaitTime = nElaspTime;

		dwReturn = ::WaitForSingleObject(m_hQuitEvent, dwNewWaitTime);
		if (WAIT_OBJECT_0 == dwReturn)
			break;
		else if (WAIT_TIMEOUT == dwReturn)
			threadProcess();
	}
}