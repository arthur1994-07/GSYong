#include "StdAfx.h"
#include <iostream>
#include <string>

#include "../Util/ColorConsole.h"
#include "./LogConsole.h"

#include "../DebugInclude.h"

namespace sc
{

namespace CC = sc::console;

LogConsole::LogConsole(void)
	: LogBase(LOG_CONSOLE)
    , m_hConsole(INVALID_HANDLE_VALUE)
{
	_CONSOLE_CODE_PAGE( "kor" );
}

LogConsole::~LogConsole(void)
{
}

bool LogConsole::Init(const TCHAR* pzName)
{
    if (m_hConsole == INVALID_HANDLE_VALUE)
    {
        DWORD dwSize;
        DWORD dwErrorCode;
        SMALL_RECT srctWindow;
        COORD coord = { 0, 0 };
        CONSOLE_SCREEN_BUFFER_INFO BufInfo;

        if ( !AllocConsole() )
        {
            FreeConsole();
            return false;
        }

        m_hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

        if (m_hConsole == INVALID_HANDLE_VALUE)
        {
            return false;
        }

        SetWindow(550);

        SetConsoleTextAttribute(m_hConsole, BACKGROUND_BLUE | FOREGROUND_BLUE |
            FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY);
        FillConsoleOutputAttribute(m_hConsole,
            BACKGROUND_BLUE, 25000, coord, &dwSize);
        GetConsoleScreenBufferInfo(m_hConsole, &BufInfo);

        srctWindow = BufInfo.srWindow;
        srctWindow.Bottom += 25;

        COORD newScreenBufferSize = BufInfo.dwSize;
        newScreenBufferSize.Y = 9999;

        if ( !SetConsoleScreenBufferSize(m_hConsole, newScreenBufferSize) )
        {
            dwErrorCode = GetLastError();
        }

        SetConsoleWindowInfo(m_hConsole, true, &srctWindow);
        SetConsoleTitle(pzName);
    }

    return true;
}

void LogConsole::Destroy()
{
    FreeConsole();
    m_hConsole = INVALID_HANDLE_VALUE;
}

void LogConsole::SetWindow(int iWidth, int iHeight)
{
    RECT rect;
    int screen_x, screen_y;
    HWND hWnd = GetConsoleWindow();

    screen_x = (int)GetSystemMetrics(SM_CXSCREEN);
    screen_y = (int)GetSystemMetrics(SM_CYSCREEN);

    //char temp[125];
    //sprintf_s(temp, ("mode con lines=%d cols=%d"), iWidth, iHeight / 12);
    //system(temp);

    GetWindowRect(hWnd, &rect);
    SetWindowPos(hWnd, NULL, screen_x - iWidth, 0, iWidth, iHeight, 0);
}

void LogConsole::write(const std::string& strLog, EM_LOG_LEVEL emLevel, bool bType, bool bTime)
{
    /*
	if (true == bTime)
	{
		time_t ltime;
		struct tm today;
		time( &ltime );
        localtime_s( &today, &ltime );
#ifdef UNICODE
		wchar_t szTime[LOG_MAX_TMP_BUF] = {0};
        wcsftime(szTime, LOG_MAX_TMP_BUF, L"[%H:%M:%S] ", &today);
        std::wcout << CC::fg_white << szTime;
#else
        char szTime[LOG_MAX_TMP_BUF] = {0};		
        strftime(szTime, LOG_MAX_TMP_BUF, "[%H:%M:%S] ", &today);
        std::cout << CC::fg_white << szTime;
#endif
	}

	if (true == bType)
	{		
		std::wcout << CC::fg_white << getLogStrCode( emLevel );
	}

	switch ( emLevel )
	{
#ifdef UNICODE
	case LOG_DEBUG : std::wcout << CC::fg_yellow; break;
	case LOG_WARN  : std::wcout << CC::fg_blue;   break;
	case LOG_INFO  : std::wcout << CC::fg_green;  break;
	case LOG_ERROR : std::wcout << CC::fg_red;    break;
	case LOG_FATAL : std::wcout << CC::fg_red;    break;
	default:         std::wcout << CC::fg_white;  break;
#else
    case LOG_DEBUG : std::cout << CC::fg_yellow; break;
    case LOG_WARN  : std::cout << CC::fg_blue;   break;
    case LOG_INFO  : std::cout << CC::fg_green;  break;
    case LOG_ERROR : std::cout << CC::fg_red;    break;
    case LOG_FATAL : std::cout << CC::fg_red;    break;
    default:         std::cout << CC::fg_white;  break;
#endif
	}

#ifdef UNICODE
    std::wcout << strLog << std::endl;
#else
    std::cout << strLog << std::endl;
#endif
    */

    DWORD dwSize;
    const TCHAR* pData = strLog.c_str();

#ifdef UNICODE
    static char temp[BUFFER_MAX * 2] = {0};
    ::WideCharToMultiByte(CP_ACP, 0, pData, -1, temp, sizeof(temp), 0, 0);
    WriteConsoleA(m_hConsole, temp, (DWORD)(strlen(temp)), &dwSize, 0);
#else
    WriteConsoleA(m_hConsole, pData, (DWORD)(strlen(pData)), &dwSize, 0);
#endif
}

void LogConsole::write(const char* szLog, EM_LOG_LEVEL emLevel, bool bType, bool bTime)
{
	if (szLog)
    {
#ifdef UNICODE
		write(std::wstring(szLog), emLevel, bType, bTime);
#else
        write(std::string(szLog), emLevel, bType, bTime);
#endif
    }
}

} // namespace sc