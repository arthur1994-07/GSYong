#ifndef SC_COMMON_LOG_XML_
#define SC_COMMON_LOG_XML_

#pragma once

#include "./LogDefine.h"
#include "./LogBase.h"

#pragma once

namespace sc
{
//! XML 로 로그저장 나중에 만들자.
class LogXML : public LogBase
{
public:
	LogXML(const char* szFileHead, const std::string& strAppPath);
	~LogXML(void);

protected:
	time_t m_ltime;

	std::string m_strFileHead;
	std::string m_strLogFileName;
	std::string strTemp;
	std::string strTempXmlLog;

	std::string m_strXmlHead;
	std::string m_strXmlFooter;

	char m_szTime[LOG_MAX_TMP_BUF];

	HANDLE m_hFile;
	DWORD m_dwFileTime;
	WORD m_wFlushCount;

	std::string m_strAppPath;

public:
	virtual void write(const char* szLog, EM_LOG_LEVEL emLevel, bool bType=true, bool bTime=true) override;
	virtual void write(const std::string& strLog, EM_LOG_LEVEL emLevel, bool bType=true, bool bTime=true) override;
	void startLog();

protected:
	void writeNow(const std::string& strLog);
	void getTime();
};

}; // namespace sc
#endif // 