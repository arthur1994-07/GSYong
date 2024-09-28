#ifndef S_CSMSGLIST_H_
#define S_CSMSGLIST_H_

#pragma once

#include "../Util/Lock.h"
#include "./NetDefine.h"

//
//mjeon.tbb
//
#ifdef _USE_TBB

#include "../=TBB/include/tbb/tbbmalloc_proxy.h"
#include "../=TBB/include/tbb/tbb_allocator.h"
#include "../=TBB/include/tbb/concurrent_queue.h"

//#include <boost/pool/object_pool.hpp>

#endif


namespace sc {
    namespace net {

///////////////////////////////////////////////////////////////////////////////
// s_CSMsgList.h
//
// class CSMsgList
//
// * History
// 2002.05.30 Jgkim Create
// 2003.02.04 Jgkim ����ó�� �߰�
//
// Copyright(c) Min Communications. All rights reserved.
// 
// * Note 
//
// �Ϲ� ����Ʈ�ʹ� �޸� ������ �Բ� �������� ����Ʈ�� �̸� �����Ѵ�.
// (new, delete �ð��� ���ҽ�Ű�� ���ؼ��̴�)
//
// ���Խÿ��� �����ִ� �� ��尡 ���ٸ� ���ο� ��带 �����ϰ�,
// ���� ������ ����ִ� ��尡 �ִٸ� �����Ѵ�.
//
// �����ÿ��� ��带 �������� �ʴ´�.
// ����� ������ ��ü�� �Ҹ��Ҷ��� �̷������.
//
// �޽����� head ���� ���� ����Ǹ�, ������ �޽����� ����Ǿ���� ��ġ�� m_pCurrent �̴�.
// �޽����� �������� head �������� m_pCurrent ���� ������ �ȴ�.
// �޽����� �ϳ��� ������ ���� m_pRead �� �����ϸ鼭 ���� �о���� �޽����� ����Ų��.
// m_pRead �� m_pCurrent �� �����ϸ� null �� ��ȯ�ϰ� ���̻� �о���� �޽����� ����.
//
// Reset() �� ȣ���ϸ� ����Ʈ�� �ʱ�ȭ�ǰ� �ٽ� �޽����� ���� �غ� �Ѵ�.
//
// ���������� Front �޽�������Ʈ�� Back �޽��� ����Ʈ�� �ΰ�
// ����� ������ Front �޽��� ����Ʈ�� �޽����� �����Ѵ�.
// ������ ��ü���� ������Ʈ ���ν���������
// Back �޽��� ����Ʈ���� �ϳ��� �޽����� ������ ó���ϰ�
// ��� ó���� ������ Flip �� ȣ���Ͽ� Front �� Back �޽�������Ʈ�� ��ȯ�Ѵ�.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USE_TBB

class MsgList : public DefaultLock
{
public:	
	MsgList(INT nStartSize = 0);
	~MsgList();
private:
	INT			m_nSize;	// Size of the list
	MSG_LIST*	m_pHead;	// Message head position
	MSG_LIST*	m_pTail;	// Message tail position
	MSG_LIST*	m_pCurrent; // Message insert position
	MSG_LIST*	m_pRead;	// Message read position

public:	
	MSG_LIST*	GetHead(void); // Returns the head element of the list (cannot be empty). 	
	MSG_LIST*	GetTail(void); // Returns the tail element of the list (cannot be empty). 
	MSG_LIST*	GetCurrent(void);
	MSG_LIST*	GetRead(void);
	MSG_LIST*	GetNext(MSG_LIST* pElement); // Gets the next element for iterating. 
	
	// Adds an element (or all the elements in another list) to the tail of the list (makes a new tail).  
	MSG_LIST*	AddTail(); 		

	// Removes the element from the head of the list. 
	void		RemoveHead(void); 
	// Removes the element from the tail of the list.  
	void		RemoveTail(void);	
	// Removes all elements from the list
	void		RemoveAll();
	// Returns the number of elements in this list. 
	INT			GetCount() const;
	// Tests for the empty list condition (no elements). 
	bool		IsEmpty() const;
	void		AddMsg(DWORD dwClient, void* pBuffer, int nSize);
	MSG_LIST*	GetMsg(void);
	// Clear all message buffer of the list
	void		Reset();
};

#endif


///////////////////////////////////////////////////////////////////////////////
// class CSMsgManager
// ���� �޽��� ������
///////////////////////////////////////////////////////////////////////////////
class MsgManager : public sc::DefaultLock
{
public:
	MsgManager(int nAmount=1000);
	~MsgManager();

	//
	//mjeon.tbb
	//
	//object_pool requires LOCK!! just use TBB-memory-allocator for concurrency.
	//boost::object_pool<MSG_LIST>    m_poolMSG;	//mjeon.tbb.concurrent_queue	
	
#ifdef _USE_TBB
	void			MsgQueueFlip()			{}

	typedef tbb::concurrent_queue<MSG_LIST*>		QUEUE_MSG;
	
	QUEUE_MSG		m_queueMsg;
	QUEUE_MSG		m_queueReservedMsg;

	int				m_nAllocCount;
	inline int		GetCount()			{ return m_nAllocCount; }

#else

	MsgList*	m_pMsgFront;
	MsgList*	m_pMsgBack;

	void		MsgQueueFlip();					//do flip
	inline int		GetCount()			{ return m_pMsgBack->GetCount(); }	// Bug ����

#endif

	void			MsgQueueInsert(int nClient, void* pMsg, int nSize);
#ifdef _USE_TBB
	MSG_LIST*		GetMsg( MSG_LIST* pUsedMsg );
#else
	MSG_LIST*		GetMsg( void* DUMMY = NULL );
#endif
};

} // namespace net
} // namespace sc
#endif // S_CSMSGLIST_H_
