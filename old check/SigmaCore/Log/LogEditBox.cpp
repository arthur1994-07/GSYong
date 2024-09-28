// Copyright (c) Min Communications. All rights reserved.
#include "StdAfx.h"
#pragma warning(disable:4996)
#pragma warning(disable:4995)
#include <strsafe.h>

#include "../Util/Lock.h"

#include "./LogDefine.h"
#include "./LogEditBox.h"

#include "../DebugInclude.h"

HWND	g_hwndMainDlg;

namespace sc
{

LogEditBox::LogEditBox(EM_LOG_TYPE emType, HWND hEditBox)
	: LogBase(emType)
	, m_hEditBox(hEditBox)
	, m_dwLogIndex(0)
{
	if (m_hEditBox)
	{		
		// only accept n k of text
		// n * 1024 = n k
		::SendMessage(m_hEditBox, EM_LIMITTEXT, (WPARAM) (32 * 1024), (LPARAM) 0);
	}
}

LogEditBox::~LogEditBox()
{
}

void LogEditBox::write(const std::string& strLog, EM_LOG_LEVEL emLevel, bool bType, bool bTime)
{
	write(strLog.c_str(), emLevel, bType, bTime);
}

void LogEditBox::write(const char* szLog, EM_LOG_LEVEL emLevel, bool bType, bool bTime)
{
	if (!szLog || !m_hEditBox || m_hEditBox == INVALID_HANDLE_VALUE)
		return;
	
#ifdef _WIN64
	unsigned __int64 dwRetCode = 0;
#else
	DWORD dwRetCode = 0;
#endif

	// Update Control and Scroll to end of the line
	//TCHAR szBuffer[LOG_BUFFER_SIZE] = {0};




	// [2014.08.26][shhan] TCHAR m_arrayszBuffer[LOG_POOL_SIZE][LOG_BUFFER_SIZE]; 로 변경하면서 Thread 상 문제가 있을 수 있어서 추가함.
	//
	TCHAR* szBuffer(NULL);
	static sc::DefaultLock sDefaultLock;
	sDefaultLock.lockOn();
	{
		++m_dwLogIndex;
		if ( m_dwLogIndex >= LOG_POOL_SIZE )
			m_dwLogIndex = 0;

		// [2014.08.26][shhan] char *szBuffer = new char[LOG_BUFFER_SIZE]; 로 인한 Memory Leak 문제로 인해 대안을 찾지 못해 순환 형식으로 바꿈.
		//						더 좋은 아이디어가 있으면 변경 해주세요.
		//						
		szBuffer = m_arrayszBuffer[m_dwLogIndex];
		ZeroMemory(szBuffer, LOG_BUFFER_SIZE);
	}
	sDefaultLock.lockOff();




	// [2014.08.26][shhan] new 사용으로 인한 단편화 문제와 서버들의 Dialog 가 사라지면서 WM_UPDATE_LOG_CONSOLE 를 받지 못해 Delete 를 하지 못하는 문제가 발생함.
	//
	//mjeon.log.main procedure단에서 해당 로그를 출력한 후에 반드시 삭제하도록 하자.
	////char *szBuffer = new char[LOG_BUFFER_SIZE];
	////ZeroMemory(szBuffer, LOG_BUFFER_SIZE);

	if (bTime)
	{
		time_t ltime;
		struct tm *today;
		time(&ltime);
		today = localtime( &ltime );
#ifdef UNICODE
		wchar_t szTime[LOG_MAX_TMP_BUF] = {0};
		wcsftime(szTime, LOG_MAX_TMP_BUF, L"[%H:%M:%S] ", today);
		StringCchCatW(szBuffer, LOG_BUFFER_SIZE, szTime);
#else
        char szTime[LOG_MAX_TMP_BUF] = {0};
        strftime(szTime, LOG_MAX_TMP_BUF, "[%H:%M:%S] ", today);
        StringCchCatA(szBuffer, LOG_BUFFER_SIZE, szTime);
#endif
	}

	if (bType)
	{		
#ifdef UNICODE
		StringCchCatW(szBuffer, LOG_BUFFER_SIZE, getLogStrCode(emLevel));
		StringCchCatW(szBuffer, LOG_BUFFER_SIZE, L" ");
#else
        StringCchCatA(szBuffer, LOG_BUFFER_SIZE, getLogStrCode(emLevel));
        StringCchCatA(szBuffer, LOG_BUFFER_SIZE, " ");
#endif
	}

#ifdef UNICODE
	StringCchCatW(szBuffer, LOG_BUFFER_SIZE, szLog);
	StringCchCatW(szBuffer, LOG_BUFFER_SIZE, L"\r\n");
#else
    StringCchCatA(szBuffer, LOG_BUFFER_SIZE, szLog);
    StringCchCatA(szBuffer, LOG_BUFFER_SIZE, "\r\n");
#endif

#ifdef UNICODE
	SendMessageTimeout(m_hEditBox, EM_SETSEL,      (WPARAM) UINT_MAX-1,  (LPARAM) UINT_MAX, SMTO_BLOCK, 15, &dwRetCode);
	SendMessageTimeout(m_hEditBox, EM_SCROLL,      (WPARAM) SB_PAGEDOWN, (LPARAM) 0,	    SMTO_BLOCK, 15, &dwRetCode);
	SendMessageTimeout(m_hEditBox, EM_SCROLLCARET, (WPARAM) 0,			 (LPARAM) 0,		SMTO_BLOCK, 15, &dwRetCode);
	SendMessageTimeout(m_hEditBox, EM_REPLACESEL,  (WPARAM) FALSE,		 (LPARAM) szBuffer, SMTO_BLOCK, 15, &dwRetCode);
	SendMessageTimeout(m_hEditBox, EM_LINESCROLL,  (WPARAM) 0,			 (LPARAM) 1,		SMTO_BLOCK, 15, &dwRetCode);
	SendMessageTimeout(m_hEditBox, EM_SETSEL,	   (WPARAM) -1,		     (LPARAM) UINT_MAX, SMTO_BLOCK, 15, &dwRetCode);
#else
	
	//mjeon.log.main procedure에게 출력을 맡기자.	
	PostMessage(g_hwndMainDlg, WM_UPDATE_LOG_CONSOLE, (WPARAM)m_hEditBox, (LPARAM)szBuffer);

#endif
}

} // namespace sc