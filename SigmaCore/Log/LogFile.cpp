// Copyright (c) Min Communications. All rights reserved.
#include "stdafx.h"

#include <io.h>       //access

#include "../BugTrap/BugTrap.h"

#include "../Util/SystemInfo.h"
#include "../File/FileUtil.h"
#include "./LogFile.h"

#include "../DebugInclude.h"

namespace sc
{

	LogFile::LogFile(const char* szFileHead, const std::string& strAppPath)
        : LogBase(LOG_FILE)
        , m_hFile(INVALID_HANDLE_VALUE)
		, m_strAppPath(strAppPath)
        , m_dwFileTime(0)
        , m_wFlushCount(0)
        , m_wLineInterval(0)
		, m_BugTrapLog(0)
    {
        if (!szFileHead)
            m_strFileHead = "Log";
        else
            m_strFileHead = szFileHead;
    }

    LogFile::~LogFile()
    {
        if (INVALID_HANDLE_VALUE != m_hFile)
            SAFE_CLOSE_HANDLE(m_hFile);
    }

    void LogFile::startLog(void)
    {	
        char szFileName[LOG_FILENAME_LENGTH] = {0};

        // Application 의 path 에 /log/ 를 더한다.
        std::string strAppPath = m_strAppPath;
        strAppPath += "\\log";

		int nResult = access( strAppPath.c_str(), 0 );
		if( nResult == -1 )
		{
			// Log Directory 를 생성한다.
			if (sc::file::createDirectory(strAppPath) == false)
			{
				DWORD dwError = GetLastError();
				// ERROR_ALREADY_EXISTS 
				//     The specified directory already exists. 
				// ERROR_PATH_NOT_FOUND
				//     One or more intermediate directories do not exist; 
				//     this function will only create the final directory in the path.
				//     To create all intermediate directories on the path, use the SHCreateDirectoryEx function. 
				// return;
			}
		}

        // 정확한 로그파일 경로 및 이름을 만들기 위해서 문자열 \\ 추가
        strAppPath += "\\";

        struct tm today;
        // Use time structure to build a customized time string.
        time(&m_ltime);
        localtime_s( &today, &m_ltime);
        // Use strftime to build a customized time string. 
        _tcsftime(
            szFileName,
            LOG_FILENAME_LENGTH,
            "_%y%m%d_%H%M.csv",
            &today);

        m_strLogFileName = strAppPath;
        m_strLogFileName += m_strFileHead;
        m_strLogFileName += szFileName;

        if (INVALID_HANDLE_VALUE != m_hFile)
        {
            SAFE_CLOSE_HANDLE( m_hFile );
        }

        //int nRetCode = 0;
        m_hFile = ::CreateFile(
            m_strLogFileName.c_str(),
            GENERIC_WRITE,
            FILE_SHARE_READ,
            NULL,
            OPEN_ALWAYS,
            FILE_ATTRIBUTE_NORMAL,
            NULL );
        if ( m_hFile == INVALID_HANDLE_VALUE )
        {
            SAFE_CLOSE_HANDLE(m_hFile);
            return;
        }
#ifdef UNICODE
        // unicode mark
        unsigned char szUnicodeMark[2];
        DWORD dwBytesWritten=0;
        szUnicodeMark[0] = 0xFF;
        szUnicodeMark[1] = 0xFE;
        ::WriteFile( m_hFile, szUnicodeMark, 2, &dwBytesWritten, NULL);
#endif
        m_dwFileTime = ::GetTickCount();

		// Log생성
		/// Log file handle.
		if ( m_BugTrapLog )
		{
			// 로그 clear
			BT_ClearLog( m_BugTrapLog );
		}
    }

	void LogFile::write(const char* szLog, EM_LOG_LEVEL emLevel, bool bType, bool bTime)
	{
		if (!szLog)
			return;
		
		strTemp = "";
		
		if (bTime)
		{
			getTime();		
			strTemp += m_szTime;
		}

		if (bType)
		{
			strTemp += getLogStrCode(emLevel);
		}
		
		strTemp += "\t";
		strTemp += szLog;
		strTemp += "\r\n";
		writeNow(strTemp);
	}
	
	void LogFile::write(const std::string& strLog, EM_LOG_LEVEL emLevel, bool bType, bool bTime)
	{
		write(strLog.c_str(), emLevel, bType, bTime);
	}

    void LogFile::writeNow(const std::string& strLog)
    {
        DWORD dwTimeDiff = ::GetTickCount() - m_dwFileTime;
        if ( m_dwFileTime==0 || 
            dwTimeDiff >= LOG_INTERVAL ||
            m_wLineInterval >= LOG_LINE_INTERVAL )
        {
            startLog();
            m_wLineInterval = 0;
        }

        if (INVALID_HANDLE_VALUE != m_hFile)
        {
            DWORD dwWritten;

            ::WriteFile(
                m_hFile,
                strLog.c_str(),
                (DWORD) strLog.size() /* * sizeof(wchar_t) */,
                &dwWritten,
                NULL);

			// BugTrap 로그 추가.
			if ( m_BugTrapLog )
			{
				BT_InsLogEntryF( m_BugTrapLog, BTLL_INFO, strLog.c_str() );
			}

            m_wLineInterval++;
            m_wFlushCount++;		
            if (m_wFlushCount >= LOG_FLUSH_COUNT)
            {
                ::FlushFileBuffers( m_hFile );
                m_wFlushCount=0;			
            }
        }
    }

    void LogFile::getTime()
    {
        time_t ltime;
        //struct tm *today = NULL;
        struct tm today;

        // Use time structure to build a customized time string. 
        time( &ltime );
        localtime_s( &today, &ltime );

        // Use strftime to build a customized time string. 
        strftime(m_szTime, LOG_MAX_TMP_BUF, "[%Y-%m-%d %H:%M:%S]", &today);
    }

	void LogFile::attachBugTrapLog()
	{
		if ( !m_bBugTrapLog )
		{
			m_bBugTrapLog = true;

			// Log생성
			/// Log file handle.
			m_BugTrapLog = BT_OpenLogFile( "customlog.txt", BTLF_TEXT );
			if ( m_BugTrapLog )
			{
				BT_SetLogSizeInEntries( m_BugTrapLog, LOG_LINE_INTERVAL );
				BT_SetLogFlags( m_BugTrapLog, BTLF_SHOWLOGLEVEL | BTLF_SHOWTIMESTAMP );
				BT_SetLogLevel( m_BugTrapLog, BTLL_INFO );
				BT_AddLogFile( BT_GetLogFileName( m_BugTrapLog ) );
			}
		}
	}

} // namespace sc
