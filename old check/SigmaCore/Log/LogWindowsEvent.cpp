#include "StdAfx.h"
#include "./LogWindowsEvent.h"

#include "../DebugInclude.h"

namespace sc
{

LogWindowsEvent::LogWindowsEvent(void)
	: LogBase(LOG_EVENT)
{
}

LogWindowsEvent::~LogWindowsEvent(void)
{
}

void LogWindowsEvent::write(const char* szLog, EM_LOG_LEVEL emLevel, bool bType, bool bTime)
{
	//! 로그 메세지
    std::string ErrorMsg;
	ErrorMsg.append(_T("\n"));
    ErrorMsg.append(szLog);

	//! 실행 파일 경로 얻어오기
	TCHAR szPathName[MAX_PATH*2];
	ZeroMemory(szPathName, sizeof(szPathName));
	::GetModuleFileName(NULL, szPathName, sizeof(szPathName));

    char szErrorMsg[MAX_PATH] = {0};
    StringCchCopy(szErrorMsg, MAX_PATH, ErrorMsg.c_str());
	
    LPTSTR  lpszStrings[5];
	lpszStrings[0] = szPathName;
	lpszStrings[1] = _T("");
	lpszStrings[2] = _T("");
	lpszStrings[3] = _T("");
	lpszStrings[4] = szErrorMsg;

	//! 윈도우 로그 타입 매핑
	WORD wType;
	switch (emLevel)
	{
	case EM_LOG_LEVEL(LOG_DEBUG):
		wType = EVENTLOG_SUCCESS;
		break;
	case EM_LOG_LEVEL(LOG_INFO):
		wType = EVENTLOG_INFORMATION_TYPE;
		break;
	case EM_LOG_LEVEL(LOG_WARN):
		wType = EVENTLOG_WARNING_TYPE;
		break;
	case EM_LOG_LEVEL(LOG_ERROR):
		wType = EVENTLOG_ERROR_TYPE;
		break;
	case EM_LOG_LEVEL(LOG_FATAL):
		wType = EVENTLOG_ERROR_TYPE;
		break;
	}
	/*
	Min Log Type		MS Log Type Value					MS Log Type Meaning	
	LOG_DEBUG			(0x0000)EVENTLOG_SUCCESS			Information Event	없음
	LOG_INFO			(0x0004)EVENTLOG_INFORMATION_TYPE	Information event	정보
	LOG_WARN			(0x0002)EVENTLOG_WARNING_TYPE		Warning event	경고
	LOG_ERROR			(0x0001)EVENTLOG_ERROR_TYPE			Error event	오류
	LOG_FATAL			(0x0001)EVENTLOG_ERROR_TYPE			Error event	오류
	*/


	// Windows Event Begin

	// Get a handle to the event log.
	HANDLE hEventSource = RegisterEventSource(NULL,  // use local computer 
            _T("Application Error") );           // event source name			
    if ( hEventSource )
    {
		// Report the event.
		ReportEvent(hEventSource,           // event log handle 
            wType,                // event type
            NULL,            // event category
            1000,            // event identifier 
            NULL,                 // no user security identifier 
            5,             // number of substitution strings 
            0,                    // no data 
            (LPCTSTR*)&lpszStrings,                // pointer to strings 
            NULL);                // no data 
		DeregisterEventSource(hEventSource);
    }
	// Windows Event End


	//TCHAR szDate[MAX_PATH]	= _T("");
	//TCHAR szError[MAX_PATH] = _T("");

	//LPTSTR  lpszStrings[1];
	//lpszStrings[0] = szError;

	//SYSTEMTIME st;
	//::GetLocalTime(&st);
	//wsprintf(szDate, "[%0.2d.%0.2d %0.2d:%0.2d:%0.2d] ", st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

	//va_list pArg;
	//va_start(pArg, pFormat);
	//_vstprintf(szError, pFormat, pArg);
	//strcat(szDate, szError);

	//if ( nLoop >= 2 )
	//{
	//	for ( int iter = 0; iter < nLoop - 1; iter++ )
	//	{
	//		LPCTSTR lpText = va_arg(pArg, LPCTSTR);
	//		strcat(szDate, " / ");
	//		strcat(szDate, lpText);
	//	}
	//}

	//va_end(pArg);
	//HANDLE hEventSource = RegisterEventSource(NULL, m_szEventLogName);
	//if ( hEventSource )
	//{
	//	ReportEvent(hEventSource, EVENTLOG_WARNING_TYPE, 0, 0, NULL, 1, 0, (LPCTSTR*)&lpszStrings[0], NULL);
	//	DeregisterEventSource(hEventSource);
	//}
}

void LogWindowsEvent::write(const std::string& strLog, EM_LOG_LEVEL emLevel, bool bType, bool bTime)
{
	write( strLog.c_str(), emLevel, bType, bTime );
}

} // namespace sc