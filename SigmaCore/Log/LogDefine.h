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
	LOG_POOL_SIZE		= 100,		// new ������� ���� ����ȭ ������ �������� Dialog �� ������鼭 WM_UPDATE_LOG_CONSOLE �� ���� ���� Delete �� ���� ���ϴ� ���� ���ſ� �迭.
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
//! Level �� �߰��Ҷ� ���� g_LogLevelStrTable �� 
//! ���ڿ��� �߰��ؾ� �Ѵ�.
//! initLogStrCode(); �� ���ڿ� �߰�
//! ConsoleMessage �����ڿ� ȣ��κ��� ����.
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