#include "stdafx.h"
#include <process.h>

#include "./EventThread.h"


EventThread::EventThread( DWORD nDlgThreadID, DWORD WaitTime, bool PriorityLow )
: m_dwThreadID( UINT_MAX )
, m_hThread( NULL )
, m_bThreadRun( false )
, m_WaitTime( WaitTime )
, m_bError( false )
, m_nDlgThreadID( nDlgThreadID )
{
	m_hEventQuit = ::CreateEvent( 0, FALSE, FALSE, 0 );
	m_hEventWork = ::CreateEvent( 0, FALSE, FALSE, 0 );
}

EventThread::~EventThread()
{
}

void EventThread::endThread()
{
	if ( m_hThread )
	{
		::SetEvent( m_hEventQuit );

		endHook();

		::WaitForSingleObject( m_hThread, 1000 );
		::CloseHandle( m_hThread );
		m_hThread = NULL;
	}
}

unsigned int WINAPI extThreadFunc( void* pThread )
{
	if ( pThread )
	{
		EventThread* pClass = ( EventThread* ) pThread;
		pClass->threadFunc();
		return 1;
	}
	else
	{
		return 0;
	}
}

bool EventThread::startThread( bool PriorityLow )
{	
	// Thread 생성
	m_hThread = ( HANDLE ) ::_beginthreadex(
		NULL,
		0,
		extThreadFunc,
		this,
		0,
		( unsigned int* ) &m_dwThreadID );
	if ( !m_hThread )
	{		
		return FALSE;
	}
	else 
	{
		if ( PriorityLow )
			SetThreadPriority( m_hThread, THREAD_PRIORITY_LOWEST );
		m_bThreadRun = false;
		return TRUE;
	}
}

void EventThread::threadFunc()
{
	::ResetEvent( m_hEventQuit );
	::ResetEvent( m_hEventWork );

	HANDLE hEvents[] = { m_hEventQuit, m_hEventWork };
	DWORD dwReturn = 0;
	m_bThreadRun = true;

	while ( m_bThreadRun )
	{
		dwReturn = ::WaitForMultipleObjects(
			2, 
			hEvents,
			FALSE,
			m_WaitTime );

		switch ( dwReturn )
		{
		case WAIT_OBJECT_0: // m_hEventQuit
			{
				m_bThreadRun = false;
				break;
			}
		case WAIT_OBJECT_0+1: // m_hEventWork
		case WAIT_TIMEOUT:
			{
				if ( -1 == process() )
				{
					m_bError = true;
				}
				break;
			}
		default:
			{
				// Unknown Event, 알수없는 이벤트...
			}
			break;
		}
	}
}

void EventThread::runThread()
{
	::SetEvent( m_hEventWork );
}

void EventThread::resume()
{
	if ( !m_bThreadRun )
	{
		m_bThreadRun = true;
		::ResumeThread( m_hThread );
	}
}

void EventThread::stop()
{
	if ( m_bThreadRun )
	{
		m_bThreadRun = false;
		::SuspendThread( m_hThread );
	}
}
