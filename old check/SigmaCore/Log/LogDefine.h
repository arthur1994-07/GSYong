#ifndef SC_LOG_DEFINE_H_
#define SC_LOG_DEFINE_H_

#pragma once

#ifndef SAFE_CLOSE_HANDLE
#define SAFE_CLOSE_HANDLE(P) { if(P) { CloseHandle(P); P = INVALID_HANDLE_VALUE; } }
#endif

//mjeon.log.
#define WM_UPDATE_LOG_CONSOLE	WM_USER + 555

namespace sc
{

enum EM_LOG_VALUE
{	
	LOG_FLUSH_COUNT     = 10, //! Log file flush rate
	LOG_MAX_TMP_BUF     = 100,
	LOG_FILENAME_LENGTH = 255,
	LOG_MAX_PATH        = MAX_PATH,
	LOG_INTERVAL        = 14400000, //! 1000msec X 60sec X 60min X 4 = 4hour(msec)	
	LOG_LINE_INTERVAL   = 5000,
	LOG_POOL_SIZE		= 100,		// new 사용으로 인한 단편화 문제와 서버들의 Dialog 가 사라지면서 WM_UPDATE_LOG_CONSOLE 를 받지 못해 Delete 를 하지 못하는 문제 제거용 배열.
	LOG_BUFFER_SIZE     = 512,
};

enum EM_LOG_POSITION
{
	LOG_POS_NORMAL  = 1,
	LOG_POS_INFO    = 2,
};

//! Log Type
enum EM_LOG_TYPE
{
	LOG_CONSOLE       = 0x0001,
	LOG_FILE          = 0x0002,
	LOG_DATABASE      = 0x0004,
	LOG_XML           = 0x0008,
	LOG_EVENT         = 0x0010,
	LOG_EDIT_CTL      = 0x0020,
	LOG_EDIT_CTL_INFO = 0x0040,
};

//! Log Level
//! Level 을 추가할때 마다 g_LogLevelStrTable 에 
//! 문자열을 추가해야 한다.
//! initLogStrCode(); 에 문자열 추가
//! ConsoleMessage 생성자에 호출부분이 있음.
enum EM_LOG_LEVEL
{
	LOG_DEBUG = 0,	
	LOG_WARN  = 1,
	LOG_INFO  = 2,
	LOG_ERROR = 3,
	LOG_FATAL = 4,
};

#ifdef UNICODE
    static wchar_t g_LogLevelStrTable[5][10];
#else
    static char g_LogLevelStrTable[5][10];
#endif

void initLogStrCode(void);

//! Example:const wchar_t* pStr = getLogStrCode( LOG_DEBUG );
#ifdef UNICODE
    const wchar_t* getLogStrCode(EM_LOG_LEVEL emLevel);
#else
    const char* getLogStrCode(EM_LOG_LEVEL emLevel);
#endif

}; // namespace sc
#endif // SC_LOG_DEFINE_H_