#ifndef S_CLOCK_H_
#define S_CLOCK_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CLock
{
public:
	CLock();
	virtual ~CLock();

protected:

	CRITICAL_SECTION	m_CriticalSection; // criticalsection object

public:

	void LockOn();	

	void LockOff();	
};

#endif // S_CLOCK_H_