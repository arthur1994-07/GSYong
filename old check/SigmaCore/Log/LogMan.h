#ifndef SC_LOG_MAN_H_
#define SC_LOG_MAN_H_

#pragma once

#include <string>
#include <vector>
#include "../Util/Lock.h"
#include "./LogDefine.h"

#ifdef _USE_TBB
#include "../../=TBB/include/tbb/tbbmalloc_proxy.h"
#include "../../=TBB/include/tbb/task_scheduler_init.h"
#include "../../=TBB/include/tbb/concurrent_queue.h"

#include "../Thread/EventThread.h"
#endif


#ifdef _RELEASE
	#define WRITE_LOG_DEBUG(message)
#else
	#define WRITE_LOG_DEBUG(message) { sc::LogMan::getInstance()->write(message, sc::LOG_DEBUG); }
#endif

#define WRITE_LOG_INFO(message)  { sc::LogMan::getInstance()->write(message, sc::LOG_INFO); }
#define WRITE_LOG_WARN(message)  { sc::LogMan::getInstance()->write(message, sc::LOG_WARN); }
#define WRITE_LOG_ERROR(message) { sc::LogMan::getInstance()->write(message, sc::LOG_ERROR); }
#define WRITE_LOG_FATAL(message) { sc::LogMan::getInstance()->write(message, sc::LOG_FATAL); }

#define WRITE_LOG_EDIT_INFO(message) { sc::LogMan::getInstance()->writeInfo(message); }

namespace sc
{

class LogBase;

/**
 * \ingroup sc
 *
 * \par requirements
 * win98 or later\n
 * win2k or later\n
 * MFC\n
 *
 * \version 1.0
 * first version
 *
 * \author Jgkim
 *
 * \history
 * 2002-05-30 Jgkim Create
 * 2006-08-09 Jgkim Unicode, CSV
 * 2007-03-14 Jgkim LogEdit, LogFile, LogXML, LogDB, LogWindowsEvent
 * 2008-12-04 Jgkim LogConsole, 구조 변경
 *
 * \par license
 * Copyright (c) Min Communications. All rights reserved.
 * 
 * \todo 
 *
 * \bug 
 *
 */
#ifdef _USE_TBB
class LogMan: public EventThread
#else
class LogMan: public DefaultLock
#endif
{
public:
	typedef std::vector<LogBase*> LOG_VECTOR;
	typedef LOG_VECTOR::iterator  LOG_VECTOR_ITER;

private:
	LogMan();
	virtual ~LogMan();
	static LogMan* SelfInstance;

protected:
	EM_LOG_LEVEL m_emLogLevel;
	LOG_VECTOR m_vecLog;
	LogBase* m_pEditInfo;

#ifdef _USE_TBB
	struct WHAT_TO_LOG
	{
	public:
		WHAT_TO_LOG()
			:emLevel(LOG_INFO)
			,bType(true)
			,bTime(true)
			,bLog4EditInfo(false)
		{
		}

		WHAT_TO_LOG(const std::string &_strLog, EM_LOG_LEVEL _emLevel, bool _bType, bool _bTime, bool _bLog4EditInfo)
			:strLog(_strLog)
			,emLevel(_emLevel)
			,bType(_bType)
			,bTime(_bTime)
			,bLog4EditInfo(_bLog4EditInfo)
		{
		}

		std::string		strLog;
		EM_LOG_LEVEL	emLevel;
		bool			bType;
		bool			bTime;
		bool			bLog4EditInfo;	//Info창에 출력될 Log인가? (Update FPS, Packet count 등)		
	};
	
	virtual void process()	override;
	virtual void start()	override;
	virtual void end()		override;
	virtual void endHook()	override;
	
	void finishRemainJobs();
    
	bool IsJobEmpty() const
	{
		return m_cqLog.empty();
	}

	void writeQueue(WHAT_TO_LOG &wtl);


	/*
	concurrent_bounded_queue
	 :Adds the ability to specify a capacity. The default capacity makes the queue practically unbounded.
	*/
	tbb::concurrent_bounded_queue<WHAT_TO_LOG>	m_cqLog;	//출력할 로그들을 담을 bounded_queue 
															// (blocking pop을 사용하기 위해 bounded_queue를 사용)


#endif

protected:
	bool detachLog(EM_LOG_TYPE emType);
	void writeLoop(
		const std::string& strLog,
		EM_LOG_LEVEL emLevel=LOG_INFO,
		bool bType=true,
		bool bTime=true);

public:
	static LogMan* getInstance();
	static void	releaseInstance();	
	
	bool attachXml();
	bool detachXml();

	bool attachWindowsEvent();
	bool detachWindowsEvent();

	void attachEditControl(HWND hControl, HWND hControlInfo);
	void detachEditControl();

	void attachBugTrapLog();

	bool attachConsole();
	bool detachConsole();

	void setLogLevel(EM_LOG_LEVEL emLevel);
	
	void writeInfo(const char* szLog);
	void writeInfo(const std::string& strLog);
	void writeInfoNow(const std::string& strLog);
	
	void write(
		const char* szLog,
		EM_LOG_LEVEL emLevel=LOG_INFO,
		bool bType=true,
		bool bTime=true);

	void write(
		const std::string& strLog,
		EM_LOG_LEVEL emLevel=LOG_INFO,
		bool bType=true,
		bool bTime=true );

}; // class LogMan

//! -----------------------------------------------------------------------
//! Log Function
void writeLogDebug(const std::string& strMsg);
void writeLogWarn(const std::string& strMsg);
void writeLogInfo(const std::string& strMsg);
void writeLogError(const std::string& strMsg);
void writeLogFatal(const std::string& strMsg);
void writeInfo(const std::string& strMsg);

} // namespace sc

#endif // _CORE_COMMON_LOG_MAN_H_
