#ifndef _FRAME_THREAD_H_
#define _FRAME_THREAD_H_

#pragma once 

#include "../Util/Lock.h"

#ifndef SAFE_CLOSE_HANDLE
#define SAFE_CLOSE_HANDLE(P) { if(P) { CloseHandle(P); P = INVALID_HANDLE_VALUE; } }
#endif

namespace sc
{
//! \version 1.0
//! \date 2007-09-27
//! \author Jgkim
//! \history
//! \desc
//!	설정된 Frame 수 만큼 실행되는 Thread
//! \par license
//! Copyright (c) Min Communications. All rights reserved.
//!	
//! \todo
//!
//! \bug
class FrameThread : public DefaultLock
{
public:
	FrameThread(DWORD dwFrame, DWORD dwIdealCpu=0);
	virtual ~FrameThread(void);

protected:
	HANDLE m_hThread;
	HANDLE m_hQuitEvent;
	bool   m_bRun;
	DWORD  m_dwFrame;
	DWORD  m_dwWaitTime;
	DWORD  m_dwOldTime;

protected:
	bool createThread(DWORD dwFrame, DWORD dwIdealCpu=0);	
	virtual void threadProcess() = 0;

public:
	void destroyThread();
	void frameThread();
	void setFrame(DWORD dwFrame);
	void resume(); //! 다시 가동시킨다
	void stop();   //! 잠시 멈춘다
	inline bool isRun() { return m_bRun; }
	
}; // class FrameThread

}; // namespace sc
#endif // _FRAME_THREAD_H_