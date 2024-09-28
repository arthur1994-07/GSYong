// Copyright (c) Min Communications. All rights reserved.
#include "stdafx.h"
#include "../String/StringUtil.h"
#include "./LogFile.h"
#include "./LogEditBox.h"
#include "./LogWindowsEvent.h"
#include "./LogXML.h"
#include "./LogDatabase.h"
#include "./LogConsole.h"
#include "./LogMan.h"

#include "../DebugInclude.h"


namespace sc
{

    LogMan* LogMan::SelfInstance = NULL;

    LogMan* LogMan::getInstance()
    {
        if (!SelfInstance)
            SelfInstance = new LogMan();
        return SelfInstance;
    }

void LogMan::releaseInstance()
{
	if (SelfInstance)
	{
		delete SelfInstance;
		SelfInstance = NULL;
	}
}

//
//mjeon.tbb
//
LogMan::LogMan()
#ifdef _USE_TBB
	:EventThread(0)	//Event의 상태에 관계없이 무조건 wait상태로 들어가지 말라.
	,m_pEditInfo(NULL)
#else
	:m_pEditInfo(NULL)
#endif
{
#if defined(_RELEASED) || defined(_DEBUG)
	m_emLogLevel = LOG_DEBUG;
#else
        m_emLogLevel = LOG_WARN;
#endif
	sc::initLogStrCode();

	// File Log 는 무조건 남긴다.
	std::stringstream sstrName;
	sstrName << _T("LOG_");
	sstrName << GetCurrentProcessId();
	LogBase* pLogFile = new sc::LogFile( sstrName.str().c_str(), sc::getAppPath() );	
	m_vecLog.push_back( pLogFile );

#ifdef _USE_TBB
	start();
#endif
}

LogMan::~LogMan()
{
#ifdef _USE_TBB

	end();

	finishRemainJobs();	//반드시 end()가 완료되고 난 후에 처리하라. 파괴자가 호출되는 스레드는 process()와 다른 스레드이다.
						//즉, end()하지 않고 호출하면 consumer스레드가 2개인 상태가 될 수 있다.
#endif

	size_t LogSize = m_vecLog.size();
	for ( size_t i=0; i<LogSize; ++i )
	{
		LogBase* pLog = m_vecLog[i];
		SAFE_DELETE( pLog );
	}
	SAFE_DELETE( m_pEditInfo );
}    

bool LogMan::detachLog( EM_LOG_TYPE emType )
{
    LOG_VECTOR_ITER iter = m_vecLog.begin();
    for ( iter; iter!=m_vecLog.end(); ++iter )
    {
        LogBase* pLog = *iter;
        if ( emType == pLog->getType() )
        {
            iter = m_vecLog.erase(iter);
            SAFE_DELETE( pLog );
            return true;
        }
    }
    return false;
}

bool LogMan::attachXml()
{
	detachXml();

	LogBase* pLog = new LogXML( _T("LOG"), sc::getAppPath() );
	m_vecLog.push_back( pLog );
	return true;
}

bool LogMan::detachXml()
{	
    return detachLog( LOG_XML );
}

bool LogMan::attachWindowsEvent()
{
    detachWindowsEvent();

    LogBase* pLog = new LogWindowsEvent();
    m_vecLog.push_back( pLog );
    return true;
}

bool LogMan::detachWindowsEvent()
{
    return detachLog( LOG_EVENT );
}

bool LogMan::attachConsole()
{
    detachConsole();

    LogBase* pLog = new LogConsole();
    m_vecLog.push_back( pLog );
    return true;
}

bool LogMan::detachConsole()
{
    return detachLog( LOG_CONSOLE );
}

void LogMan::detachEditControl()
{
    detachLog( LOG_EDIT_CTL );
    SAFE_DELETE( m_pEditInfo );	
}

void LogMan::attachEditControl(HWND hControl, HWND hEditBoxInfo)
{
	detachLog(LOG_EDIT_CTL);

	if (hControl )
	{
		LogBase* pEdit = new LogEditBox(LOG_EDIT_CTL, hControl);
		m_vecLog.push_back(pEdit);
	}        

    if (hEditBoxInfo)
    {
        SAFE_DELETE(m_pEditInfo);
        m_pEditInfo = new LogEditBox(LOG_EDIT_CTL_INFO, hEditBoxInfo);
    }
}

void LogMan::attachBugTrapLog()
{
	size_t LogSize = m_vecLog.size();
	for ( size_t i=0; i<LogSize; ++i )
	{
		LogBase* pLog = m_vecLog[i];
		pLog->attachBugTrapLog();
	}
}

void LogMan::write(const char* szLog, EM_LOG_LEVEL emLevel, bool bType, bool bTime)
{
	if (szLog)
	{
		write(std::string(szLog), emLevel, bType, bTime);            
	}
}

void LogMan::write(const std::string& strLog, EM_LOG_LEVEL emLevel, bool bType, bool bTime)
{
	if (emLevel >= m_emLogLevel)
	{
#ifdef _USE_TBB
		writeQueue(WHAT_TO_LOG(strLog, emLevel, bType, bTime, false));
#else
		writeLoop(strLog, emLevel, bType, bTime);
#endif
	}
}

void LogMan::writeInfo(const std::string& strLog)
{
#ifdef _USE_TBB
	writeQueue(WHAT_TO_LOG(strLog, LOG_INFO, true, true, true));
#else
	writeInfoNow(strLog);
#endif
}

void LogMan::writeInfo(const char* szLog)
{
	if (szLog)
		writeInfo(std::string(szLog));
}

void LogMan::writeInfoNow(const std::string& strLog)
{
	if (m_pEditInfo)
	{
#ifdef _USE_TBB
		m_pEditInfo->write(strLog, LOG_INFO);
#else
		lockOn();
		m_pEditInfo->write(strLog, LOG_INFO);
		lockOff();
#endif
	}
	else
	{
		writeLoop(strLog);
	}
}

//
//mjeon.tbb
//
#ifdef _USE_TBB

//자식에서 명시적인 구현이 필요하다.
void LogMan::start()
{
	startThread();
}
//자식에서 명시적인 구현이 필요하다.
void LogMan::end()
{
	endThread();
}

void LogMan::endHook()
{
	static const WHAT_TO_LOG WhatToLogLast;
	m_cqLog.push(WhatToLogLast);
}

void LogMan::writeQueue(sc::LogMan::WHAT_TO_LOG &wtl)
{
	m_cqLog.try_push(wtl);
}

//overriden
void LogMan::process()
{
	WHAT_TO_LOG wtl;
	
	m_cqLog.pop(wtl);	/** Block until an item becomes available, and then dequeue it.*/

	if(wtl.strLog.empty())
		return;

	if(wtl.bLog4EditInfo)
	{
		writeInfoNow(wtl.strLog);
	}
	else
	{	
		writeLoop(wtl.strLog, wtl.emLevel, wtl.bType, wtl.bTime);
	}
}

void LogMan::finishRemainJobs()
{
	WHAT_TO_LOG wtl;

	while (m_cqLog.try_pop(wtl))
	{
		if(wtl.bLog4EditInfo)
		{
			writeInfoNow(wtl.strLog);
		}
		else
		{	
			writeLoop(wtl.strLog, wtl.emLevel, wtl.bType, wtl.bTime);
		}
	}
}

void LogMan::writeLoop(const std::string& strLog, EM_LOG_LEVEL emLevel, bool bType, bool bTime)
{
	UINT nSize = (UINT)m_vecLog.size();
	for (UINT i=0; i<nSize; ++i)
		m_vecLog[i]->write(strLog, emLevel, bType, bTime);
}
#else
void LogMan::writeLoop(const std::string& strLog, EM_LOG_LEVEL emLevel, bool bType, bool bTime)
{
	lockOn();
	size_t nSize = m_vecLog.size();
	for (size_t i=0; i<nSize; ++i)
		m_vecLog[i]->write(strLog, emLevel, bType, bTime);
	lockOff();
}
#endif //_USE_TBB

void writeLogDebug(const std::string& strMsg)
{
#if defined(_RELEASED) || defined(_DEBUG)
	sc::LogMan::getInstance()->write(strMsg, sc::LOG_DEBUG);
#endif
}

void writeLogWarn(const std::string& strMsg)
{
	sc::LogMan::getInstance()->write(strMsg, sc::LOG_WARN);
}

void writeLogInfo(const std::string& strMsg)
{
	sc::LogMan::getInstance()->write(strMsg, sc::LOG_INFO);
}

void writeLogError(const std::string& strMsg)
{
	sc::LogMan::getInstance()->write(strMsg, sc::LOG_ERROR);
}

void writeLogFatal(const std::string& strMsg)
{
	sc::LogMan::getInstance()->write(strMsg, sc::LOG_FATAL);
}

void writeInfo(const std::string& strMsg)
{
	sc::LogMan::getInstance()->writeInfo(strMsg);
}

} // namespace sc
