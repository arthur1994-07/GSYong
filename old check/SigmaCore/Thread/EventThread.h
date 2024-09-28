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
//!	�̺�Ʈ�� �߻��ϰų� ������ �ð��� ������ �ڵ����� ����Ǵ� Thread
//!	runThread() �� ȣ���ϸ� Event �� �߻��Ѵ�.
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
	HANDLE	m_hEventQuit;	//! Thread ���� Event
	HANDLE	m_hEventWork;	//! Thread work Event
	bool    m_bThreadRun;   //! Thread�� �������ΰ�?
	DWORD   m_WaitTime;     //! ��� �ð�

public:
	//! -----------------------------------------------------------------------
	//! Virtual Function	
	virtual void process()	= 0;	//�������� ���� ó����	
	virtual void start()	= 0;	//! Start ���� startThread �� �ݵ�� ȣ���Ͻÿ�	
	virtual void end()		= 0;	//! End ���� endThread �� �ݵ�� ȣ���Ͻÿ�	
	virtual void endHook()	= 0;	//mjeon.endThread ��(m_hEventQuit �ñ׳� ��)�� ó���� ����
	
protected:
    void endThread(); //! Thread �� ������ �����

public:
	void threadFunc();
	void resume(); //! �ٽ� ������Ų��
	void stop();   //! ��� �����

protected:
	bool startThread(bool PriorityLow=false);
	void runThread();	

}; // class EventThread

} // namespace sc
#endif // _EVENT_THREAD_H_