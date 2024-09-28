#ifndef _CORE_COMMON_LOG_WINDOWS_EVENT_
#define _CORE_COMMON_LOG_WINDOWS_EVENT_

#pragma once

#include "./LogDefine.h"
#include "./LogBase.h"

namespace sc
{
//! 나중에 만들자...
class LogWindowsEvent : public LogBase
{
public:
	LogWindowsEvent(void);
	~LogWindowsEvent(void);

public:
	virtual void write(const char* szLog, EM_LOG_LEVEL emLevel, bool bType=true, bool bTime=true) override;
	virtual void write(const std::string& strLog, EM_LOG_LEVEL emLevel, bool bType=true, bool bTime=true) override;
};

}; // End of namespace CORE_COMMON
#endif // _CORE_COMMON_LOG_WINDOWS_EVENT_