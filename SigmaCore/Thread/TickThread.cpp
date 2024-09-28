#include "stdafx.h"
#include <process.h>
#include "../../BugTrap/BugTrap.h"
#include "./TickThread.h"

#include "../DebugInclude.h"

using namespace sc;

TickThread::TickThread(void)
	: m_hThread( NULL )	
	, m_bRun( false )
	, m_dwWaitTime( 0 )
	, m_dwTickCount( 0 )
{
	m_hQuitEvent = ::CreateEvent( NULL, TRUE, FALSE, NULL );
}

TickThread::~TickThread(void)
{
	if (m_hQuitEvent)
		::CloseHandle(m_hQuitEvent);
	if (m_hThread)
		::CloseHandle(m_hThread);
}

unsigned int WINAPI callTickThread(void* pThread)
{
	if (pThread)
	{
		TickThread* pTickThread = (TickThread*) pThread;
		pTickThread->tickThread();
		return 1;
	}
	else
	{
		return 0;
	}
}

bool TickThread::createThread(DWORD dwWaitTime)
{
	unsigned int ThreadID=0;
	m_hThread = (HANDLE) _beginthreadex(
							NULL,
							0,
							&callTickThread,
							this,
							CREATE_SUSPENDED,
							&ThreadID );
	if (!m_hThread)
	{
		return false;
	}
	m_dwWaitTime = dwWaitTime;
	return true;
}

void TickThread::destroyThread()
{
	run();
	::SetEvent(m_hQuitEvent);
	::WaitForSingleObject(m_hThread, 1000);
}

void TickThread::run()
{
	if (!m_bRun)
	{
		m_bRun = true;
		::ResumeThread(m_hThread);
	}
}

void TickThread::stop()
{
	if (m_bRun)
	{
		m_bRun = false;
		::SuspendThread(m_hThread);
	}
}

void TickThread::tickThread()
{
	BT_SetTerminate();

	while (true)
	{
		DWORD dwReturn = ::WaitForSingleObject(m_hQuitEvent, m_dwWaitTime);
		if (WAIT_OBJECT_0 == dwReturn)
		{
			break;
		}
		else if (WAIT_TIMEOUT == dwReturn)
		{
			m_dwTickCount++;
			threadProcess();
		}
	}
}