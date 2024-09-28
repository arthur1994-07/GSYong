#ifndef LOG_FILE_H
#define LOG_FILE_H

#include <string>
#include "./LogDefine.h"
#include "./LogBase.h"

#pragma once

namespace sc
{

//! class LogFile
//!
//! \History
//! 2002.05.30 Jgkim Create
//!
//! Copyright (c) Min Communications. All rights reserved.
//! \Note
//!
class LogFile : public LogBase
{
public:
	LogFile(const char* szFileHead, const std::string& strAppPath);
	virtual ~LogFile();

protected:
	time_t m_ltime;
	
#ifdef UNICODE
	std::wstring m_strFileHead;
	std::wstring m_strLogFileName;
	std::wstring strTemp;
	wchar_t m_szTime[LOG_MAX_TMP_BUF];
#else
    std::string m_strFileHead;
    std::string m_strLogFileName;
    std::string strTemp;
    char m_szTime[LOG_MAX_TMP_BUF];
#endif
	
	HANDLE m_hFile;
	DWORD m_dwFileTime;
	WORD m_wFlushCount; //! Log file flush rate
	WORD m_wLineInterval; //! Log line 수가 많아지면 파일을 분리한다.

	std::string m_strAppPath;

	INT_PTR m_BugTrapLog;

public:

#ifdef UNICODE
	virtual void write(const wchar_t* szLog, EM_LOG_LEVEL emLevel, bool bType=true, bool bTime=true) override;
	virtual void write(const std::wstring& strLog, EM_LOG_LEVEL emLevel, bool bType=true, bool bTime=true) override;
#else
    virtual void write(const char* szLog, EM_LOG_LEVEL emLevel, bool bType=true, bool bTime=true) override;
    virtual void write(const std::string& strLog, EM_LOG_LEVEL emLevel, bool bType=true, bool bTime=true) override;
#endif
	void startLog();

	virtual void attachBugTrapLog()	override;

protected:
	void writeNow(const std::string& strLog);
	void getTime();
};

} // End of name space CORE_UTIL

#endif // LOG_FILE_H