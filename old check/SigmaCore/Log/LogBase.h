#ifndef SC_LOG_BASE_H_
#define SC_LOG_BASE_H_

#include <string>
#include "./LogDefine.h"

#pragma once

namespace sc
{

class LogBase
{
public:
	LogBase(EM_LOG_TYPE emType);
	virtual ~LogBase();

protected:
	EM_LOG_TYPE m_Type;
	bool		m_bBugTrapLog;

public:
	virtual void attachBugTrapLog()	{}
	EM_LOG_TYPE getType() const { return m_Type; }
	virtual void write(const char* szLog, EM_LOG_LEVEL emLevel, bool bType=true, bool bTime=true) = 0;
	virtual void write(const std::string& strLog, EM_LOG_LEVEL emLevel, bool bType=true, bool bTime=true) = 0;
};

}; // namespace sc
#endif // SC_LOG_BASE_H_