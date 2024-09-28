#ifndef TICKTHREAD_H_
#define TICKTHREAD_H_

#pragma once

namespace sc
{
//! \version 1.0
//! \date 2007-01-27
//! \author Jgkim
//! \desc
//!		�����ð� ����� ����Ǵ� Thread \n
//!     Thread ���� ������ m_dwWaitTime ���� �����Ѵ�. \n
//! \history
//! \par license
//! Copyright (c) Min Communications. All rights reserved.
//! \todo
//! \bug
class TickThread
{
public:
	TickThread(void);
	virtual ~TickThread(void);

protected:
	HANDLE m_hThread;
	HANDLE m_hQuitEvent;
	bool m_bRun;
	DWORD m_dwWaitTime;
	DWORD m_dwTickCount;

public:
	bool createThread(DWORD dwWaitTime);
	void destroyThread();
	void run();
	void stop();
	void tickThread();
	virtual void threadProcess() = 0;

	inline DWORD getTickCount() const { return m_dwTickCount; }
	inline bool isRun() const { return m_bRun; }
};

} // End of namespace sc

#endif // TICKTHREAD_H_