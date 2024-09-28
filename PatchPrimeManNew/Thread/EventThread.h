#pragma once

class EventThread
{
public:
	EventThread( DWORD nDlgThreadID, DWORD WaitTime, bool PriorityLow );
	virtual ~EventThread();

protected:
	DWORD	m_dwThreadID;	//! Thread ID
	HANDLE	m_hThread;		//! Thread Handle
	HANDLE	m_hEventQuit;	//! Thread 종료 Event
	HANDLE	m_hEventWork;	//! Thread work Event
	bool    m_bThreadRun;   //! Thread가 실행중인가?
	DWORD   m_WaitTime;     //! 대기 시간
	bool	m_bError;		//! 에러 발생
	DWORD	m_nDlgThreadID;

public:
	//! Virtual Function	
	virtual int process()	= 0;	//! 스레드의 실제 처리부	
	virtual void start()	= 0;	//! Start 에서 startThread 를 반드시 호출하시오	
	virtual void end()		= 0;	//! End 에서 endThread 를 반드시 호출하시오	
	virtual void endHook()	= 0;	//! mjeon.endThread 중(m_hEventQuit 시그널 후)에 처리할 사항

protected:
	void endThread(); //! Thread 를 완전히 멈춘다

public:
	void threadFunc();
	void resume(); //! 다시 가동시킨다
	void stop();   //! 잠시 멈춘다

protected:
	bool startThread( bool PriorityLow = false );
	void runThread();	

}; // class EventThread