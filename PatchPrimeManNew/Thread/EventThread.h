#pragma once

class EventThread
{
public:
	EventThread( DWORD nDlgThreadID, DWORD WaitTime, bool PriorityLow );
	virtual ~EventThread();

protected:
	DWORD	m_dwThreadID;	//! Thread ID
	HANDLE	m_hThread;		//! Thread Handle
	HANDLE	m_hEventQuit;	//! Thread ���� Event
	HANDLE	m_hEventWork;	//! Thread work Event
	bool    m_bThreadRun;   //! Thread�� �������ΰ�?
	DWORD   m_WaitTime;     //! ��� �ð�
	bool	m_bError;		//! ���� �߻�
	DWORD	m_nDlgThreadID;

public:
	//! Virtual Function	
	virtual int process()	= 0;	//! �������� ���� ó����	
	virtual void start()	= 0;	//! Start ���� startThread �� �ݵ�� ȣ���Ͻÿ�	
	virtual void end()		= 0;	//! End ���� endThread �� �ݵ�� ȣ���Ͻÿ�	
	virtual void endHook()	= 0;	//! mjeon.endThread ��(m_hEventQuit �ñ׳� ��)�� ó���� ����

protected:
	void endThread(); //! Thread �� ������ �����

public:
	void threadFunc();
	void resume(); //! �ٽ� ������Ų��
	void stop();   //! ��� �����

protected:
	bool startThread( bool PriorityLow = false );
	void runThread();	

}; // class EventThread