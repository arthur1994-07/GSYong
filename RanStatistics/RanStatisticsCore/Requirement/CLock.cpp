#include "stdafx.h"
#include "CLock.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CLock::CLock()
{
	InitializeCriticalSection(&m_CriticalSection); 
}

CLock::~CLock()
{
	DeleteCriticalSection(&m_CriticalSection);
}

void CLock::LockOn()
{
	EnterCriticalSection(&m_CriticalSection);
}

void CLock::LockOff() 
{
	LeaveCriticalSection(&m_CriticalSection);
}
