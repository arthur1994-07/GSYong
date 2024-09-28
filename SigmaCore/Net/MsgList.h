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
// 2003.02.04 Jgkim 에러처리 추가
//
// Copyright(c) Min Communications. All rights reserved.
// 
// * Note 
//
// 일반 리스트와는 달리 생성과 함께 일정량의 리스트를 미리 생성한다.
// (new, delete 시간을 감소시키기 위해서이다)
//
// 삽입시에는 남아있는 빈 노드가 없다면 새로운 노드를 생성하고,
// 기존 생성된 비어있는 노드가 있다면 재사용한다.
//
// 삭제시에는 노드를 삭제하지 않는다.
// 노드의 삭제는 객체가 소멸할때만 이루어진다.
//
// 메시지는 head 에서 부터 저장되며, 다음에 메시지가 저장되어야할 위치는 m_pCurrent 이다.
// 메시지를 읽을때는 head 에서부터 m_pCurrent 까지 읽으면 된다.
// 메시지를 하나씩 읽을때 마다 m_pRead 가 증가하면서 다음 읽어야할 메시지를 가르킨다.
// m_pRead 가 m_pCurrent 에 도달하면 null 을 반환하고 더이상 읽어야할 메시지가 없다.
//
// Reset() 을 호출하면 리스트가 초기화되고 다시 메시지를 받을 준비를 한다.
//
// 서버에서는 Front 메시지리스트와 Back 메시지 리스트를 두고
// 통신이 들어오면 Front 메시지 리스트에 메시지를 삽입한다.
// 서버의 전체게임 업데이트 프로시저에서는
// Back 메시지 리스트에서 하나씩 메시지를 꺼내서 처리하고
// 모든 처리가 끝나면 Flip 을 호출하여 Front 와 Back 메시지리스트를 전환한다.
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
// 서버 메시지 관리기
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
	inline int		GetCount()			{ return m_pMsgBack->GetCount(); }	// Bug 유발

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
