#ifndef _EVENT_THREAD_H_
#define _EVENT_THREAD_H_

#pragma once

#include "../Util/Lock.h"


namespace sc
{
//! \version 1.0
//! \date 2007-02-12
//! \author Jgkim
//! \history
//! \desc
//!	이벤트가 발생하거나 지정된 시간이 지나면 자동으로 실행되는 Thread
//!	runThread() 를 호출하면 Event 가 발생한다.
//! \par license
//! Copyright (c) Min Communications. All rights reserved.
//!	
//! \todo
//!
//! \bug
class EventThread : public DefaultLock
{
public:
	EventThread(DWORD WaitTime=1, bool PriorityLow=false);
	virtual ~EventThread(void);

protected:
	DWORD	m_dwThreadID;	//! Thread ID
	HANDLE	m_hThread;		//! Thread Handle
	HANDLE	m_hEventQuit;	//! Thread 종료 Event
	HANDLE	m_hEventWork;	//! Thread work Event
	bool    m_bThreadRun;   //! Thread가 실행중인가?
	DWORD   m_WaitTime;     //! 대기 시간

public:
	//! -----------------------------------------------------------------------
	//! Virtual Function	
	virtual void process()	= 0;	//스레드의 실제 처리부	
	virtual void start()	= 0;	//! Start 에서 startThread 를 반드시 호출하시오	
	virtual void end()		= 0;	//! End 에서 endThread 를 반드시 호출하시오	
	virtual void endHook()	= 0;	//mjeon.endThread 중(m_hEventQuit 시그널 후)에 처리할 사항
	
protected:
    void endThread(); //! Thread 를 완전히 멈춘다

public:
	void threadFunc();
	void resume(); //! 다시 가동시킨다
	void stop();   //! 잠시 멈춘다

protected:
	bool startThread(bool PriorityLow=false);
	void runThread();	

}; // class EventThread

} // namespace sc
#endif // _EVENT_THREAD_H_