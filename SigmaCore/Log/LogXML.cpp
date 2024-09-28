// Copyright (c) Min Communications. All rights reserved.
#include "stdafx.h"
#include "../String/StringUtil.h"
#include "../Util/SystemInfo.h"
#include "../File/FileUtil.h"
#include "LogXML.h"

#include "../DebugInclude.h"

namespace sc
{

LogXML::LogXML(const char* szFileHead, const std::string& strAppPath)
   : LogBase(LOG_XML)
   , m_hFile(INVALID_HANDLE_VALUE)
   , m_strAppPath(strAppPath)
   , m_dwFileTime(0)
   , m_wFlushCount(0)
{
	if (!szFileHead)
		m_strFileHead = "Log";
	else
		m_strFileHead = szFileHead;

	m_strXmlHead = "<?xml version=\"1.0\" encoding=\"utf-8\" ?>\r\n<Log>\r\n<LogHeader>\r\n</LogHeader>\r\n<LogEvent>\r\n";
	m_strXmlFooter = "</LogEvent>\r\n</Log>\r\n";
}

LogXML::~LogXML(void)
{
	if (INVALID_HANDLE_VALUE != m_hFile)
	{
		SAFE_CLOSE_HANDLE(m_hFile);
	}
}

void LogXML::startLog(void)
{	
	TCHAR szFileName[LOG_FILENAME_LENGTH] = {0};

	// Application 의 path 에 /log/ 를 더한다.
    std::string strAppPath = m_strAppPath;
	strAppPath += "\\log";

	// Log Directory 를 생성한다.
    if (file::createDirectory(strAppPath) == false)
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
	
	// 정확한 로그파일 경로 및 이름을 만들기 위해서 문자열 \\ 추가
	strAppPath += "\\";

	struct tm today;
	// Use time structure to build a customized time string.
	time(&m_ltime);
	localtime_s( &today, &m_ltime );
    // Use strftime to build a customized time string. 
	_tcsftime(
		szFileName,
		LOG_FILENAME_LENGTH,
		"_%Y%m%d%H%M.xml",
		&today );
	
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
					0,
					NULL,
					OPEN_ALWAYS,
					FILE_ATTRIBUTE_NORMAL,
					NULL );
	if ( m_hFile == INVALID_HANDLE_VALUE )
	{
		SAFE_CLOSE_HANDLE(m_hFile);
		return;
	}

	DWORD dwBytesWritten=0;

	::WriteFile( m_hFile, m_strXmlHead.c_str(), (DWORD) m_strXmlHead.size(), &dwBytesWritten, NULL);
	::WriteFile( m_hFile, m_strXmlFooter.c_str(), (DWORD) m_strXmlFooter.size(), &dwBytesWritten, NULL);

	//::FlushFileBuffers( m_hFile );

	m_dwFileTime = ::GetTickCount();
	//::SetFilePointer( m_hFile, 0, NULL, FILE_END );
}

void LogXML::write(const char* szLog, EM_LOG_LEVEL emLevel, bool bType, bool bTime)
{
	if (!szLog)
        return;

	strTempXmlLog = szLog;

//	std::replace(strTempXmlLog.begin(), strTempXmlLog.end(), '&', '＆');	// &amp;
//	std::replace(strTempXmlLog.begin(), strTempXmlLog.end(), '<', '〈');	// &lt;
//	std::replace(strTempXmlLog.begin(), strTempXmlLog.end(), '>', '〉');	// &gt;
//	std::replace(strTempXmlLog.begin(), strTempXmlLog.end(), '\"', '＂');	// &quot;
//	std::replace(strTempXmlLog.begin(), strTempXmlLog.end(), '\'', '＇');	// &apos;
	
	getTime();
	strTemp = "";
	strTemp += "<Msg LT=\"";
	strTemp += m_szTime;
	strTemp += "\" LC=\"";
	strTemp += getLogStrCode(emLevel);
	strTemp += "\" LM=\"";
	strTemp += strTempXmlLog;
	strTemp += "\" />\r\n";

	writeNow(strTemp);
}

void LogXML::write(const std::string& strLog, EM_LOG_LEVEL emLevel, bool bType, bool bTime)
{
	write(strLog.c_str(), emLevel);
}

void LogXML::writeNow(const std::string& strLog)
{
	DWORD dwTimeDiff = ::GetTickCount() - m_dwFileTime;
	if (m_dwFileTime==0 || dwTimeDiff >= LOG_INTERVAL)
	{
		startLog();
	}

	if (INVALID_HANDLE_VALUE != m_hFile)
	{
		DWORD dwWritten;
		std::string szLogUTF8(strLog);
		//szLogUTF8 = StringUtil::unicodeToUtf8( strLog.c_str(), (int) strLog.size() );        
		szLogUTF8 += m_strXmlFooter;

		SetFilePointer(m_hFile, -((LONG)m_strXmlFooter.size()), 0, FILE_END);
		::WriteFile(
			m_hFile,
			szLogUTF8.c_str(),
			(DWORD) szLogUTF8.size(),
			&dwWritten,
			NULL);

		m_wFlushCount++;
		if (m_wFlushCount >= LOG_FLUSH_COUNT)
		{
			::FlushFileBuffers( m_hFile );
			m_wFlushCount=0;			
		}
	}
}

void LogXML::getTime()
{
	time_t ltime;
	struct tm today;

	// Use time structure to build a customized time string. 
	time( &ltime );
	localtime_s( &today, &ltime );

    // Use strftime to build a customized time string. 
	//::wcsftime( m_szTime, LOG_MAX_TMP_BUF, _T("[%Y-%m-%d %H:%M:%S]"), today );	
    strftime(m_szTime, LOG_MAX_TMP_BUF, "[%Y-%m-%d %H:%M:%S]", &today);
}

} // namespace sc