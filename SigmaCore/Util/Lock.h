#ifndef SC_LOCK_H_
#define SC_LOCK_H_

#pragma once

#include <boost/thread/recursive_mutex.hpp>
#include <boost/thread/mutex.hpp>
//#include <windows.h>

namespace sc
{
	enum
	{
		CRITICAL_SECTION_SPIN_COUNT = 2000,
	};
/**
 * \ingroup CORE_COMMON
 *
 * \par requirements
 * win98 or later\n
 * win2k or later\n
 * MFC\n
 *
 * \version 1.0
 *
 * \date 2006.08.08
 *
 * \author Jgkim
 *
 * \history
 *
 * \par license
 * Copyright (c) Min Communications. All rights reserved.
 *	
 * \todo
 *
 * \bug
 *
 */

class NoCriticalSection
{
public:
	void lock() {}
	void unlock() {}
};

// WIN32 �� CRITICAL_SECTION �� �⺻������ ��Ͷ��̴�!
// class CriticalSectionWin32
// {
// public:
// 	CriticalSectionWin32() { ::InitializeCriticalSection( &m_criticalSection ); }
// 	~CriticalSectionWin32() { ::DeleteCriticalSection( &m_criticalSection ); }
// 
// 	void lock() { ::EnterCriticalSection( &m_criticalSection ); }
// 	void unlock() { ::LeaveCriticalSection( &m_criticalSection ); }
// 
// private:
// 	CriticalSectionWin32(const CriticalSectionWin32& );
// 	CriticalSectionWin32& operator=(const CriticalSectionWin32& );
// private:
// 	// ��Ͷ��� �ȴ�
// 	CRITICAL_SECTION m_criticalSection; ///< CriticalSection object
// };
// 
// /// �ϴ� �ٸ� Ÿ������ �����صΰ� ... boost �� ���ԵǸ� ������� ��ü
// class RecursiveCriticalSectionWin32 : private CriticalSectionWin32
// {
// public:
// 	void lock() { CriticalSectionWin32::lock(); }
// 	void unlock() { CriticalSectionWin32::unlock(); }
// };


template<class LOCK_TYPE>
class Lock_
{
public:
	Lock_() {}
	virtual ~Lock_() {}

protected:
//	CRITICAL_SECTION m_CriticalSection; ///< CriticalSection object
	LOCK_TYPE m_mutex;
private:	
	Lock_( const Lock_& rhs );
	Lock_& operator=( const Lock_& rhs );

public:
	/// Critical Section Lock On
	void lockOn() { m_mutex.lock(); }
	/// Critical Section Lock Off
	void lockOff() { m_mutex.unlock(); }
	/// Try to lock
//	bool tryLock();
	
}; // End of class Lock


// ��������� ����Ҷ� �����
typedef Lock_<boost::recursive_mutex> RecursiveLock;
//typedef Lock_<RecursiveCriticalSectionWin32> RecursiveLock;

// ����Ʈ�� ��Ͷ� -_-; �ϴ� �̰ɾ��� ���� ��Ͷ� �ƴѳ����� �Ѵ� boost�� mutex�� ��Ͷ��� �ƴϾ� �ӵ��� ������
//typedef Lock_<CriticalSectionWin32> DefaultLock;
typedef Lock_<boost::mutex> DefaultLock;


typedef Lock_<NoCriticalSection> NoLock;

template<class LOCK>
class OwnerLock_
{
public:
	OwnerLock_( LOCK& lock ) : m_lock( lock ) { 	m_lock.lockOn(); }
	~OwnerLock_() { m_lock.lockOff(); }

private:
	LOCK& m_lock;
	OwnerLock_( const OwnerLock_& rhs );
	OwnerLock_& operator=( const OwnerLock_& rhs );
}; // End of class OwnerLock

typedef OwnerLock_<DefaultLock> OwnerDefaultLock;
typedef OwnerLock_<RecursiveLock> OwnerRecursiveLock;

class CriticalSectionOwnerLock
{
public:
    CriticalSectionOwnerLock(CRITICAL_SECTION& CS)
        : m_CriticalSection(CS)
    {
        EnterCriticalSection(&m_CriticalSection);
    }

    ~CriticalSectionOwnerLock()
    {
        LeaveCriticalSection(&m_CriticalSection);
    }

private:
    CRITICAL_SECTION& m_CriticalSection;

    //! ����/���� ����
    CriticalSectionOwnerLock(const CriticalSectionOwnerLock& rhs);
    CriticalSectionOwnerLock& operator = (const CriticalSectionOwnerLock& rhs);
};

} // namespace sc

#define DECLARE_CLASS_DEFAULT_LOCK sc::DefaultLock m_lock
#define CLASS_AUTO_DEFAULT_LOCK sc::OwnerDefaultLock lock ## __LINE__ ## (m_lock)
#define THIS_AUTO_DEFAULT_LOCK sc::OwnerDefaultLock lock ## __LINE__ ## (*this)

#define DECLARE_CLASS_RECURSIVE_LOCK sc::RecursiveLock m_lock
#define CLASS_AUTO_RECURSIVE_LOCK sc::OwnerRecursiveLock lock ## __LINE__ ## (m_lock)
#define THIS_AUTO_RECURSIVE_LOCK sc::OwnerRecursiveLock lock ## __LINE__ ## (*this)

#endif // SC_LOCK_H_