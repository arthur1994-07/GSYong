#include "stdafx.h"
#include "../String/StringFormat.h"
#include "../Log/LogMan.h"
#include "MsgList.h"


#include "../DebugInclude.h"



namespace sc {
    namespace net {

//
//mjeon.tbb
//
#ifndef _USE_TBB

MsgList::MsgList(int nStartSize) 
	: m_pHead(NULL)
	, m_pTail(NULL)
	, m_pCurrent(NULL)
	, m_pRead(NULL)
	, m_nSize(0)
{
	int i=0;
	for (i=0; i<nStartSize; ++i)
	{
		AddTail();
	}
}

MsgList::~MsgList()
{	
	int i=0;
	int nSize = m_nSize;

	for (i=0; i<nSize; ++i)
	{		
		RemoveHead();
	}
}
	
// Returns the head element of the list (cannot be empty). 
MSG_LIST* MsgList::GetHead()
{
	return m_pHead;
}
// Returns the tail element of the list (cannot be empty). 
MSG_LIST* MsgList::GetTail()
{
	return m_pTail;
}

MSG_LIST* MsgList::GetCurrent()
{
	return m_pCurrent;
}

MSG_LIST* MsgList::GetRead()
{
	return m_pRead;
}

// Gets the next element for iterating. 
MSG_LIST* MsgList::GetNext(MSG_LIST* pElement)
{
	if (pElement != NULL)
		return pElement->next;
	else
		return NULL;
}

// Adds an element (or all the elements in another list) to the tail of the list (makes a new tail).  
MSG_LIST* MsgList::AddTail()
{
	MSG_LIST* pNewElement = NULL;	
	
	// 메모리할당
	pNewElement = (MSG_LIST*) HeapAlloc(GetProcessHeap(), 
										HEAP_ZERO_MEMORY, 
										sizeof(MSG_LIST));

	if (pNewElement == NULL)
		return NULL;

	// 비어있다면...
	if (m_nSize == 0)
	{
		pNewElement->next = NULL;
		m_pHead           = pNewElement;		
		m_pTail           = pNewElement;
		m_pCurrent        = m_pHead;
		m_pRead           = m_pHead;
	}
	else
	{	
		pNewElement->next = NULL;
		m_pTail->next     = pNewElement;
		m_pTail           = pNewElement;		
	}
	m_nSize++;	
	return m_pTail;
}

// Removes the element from the head of the list. 
void MsgList::RemoveHead(void)
{
	MSG_LIST* pTemp = NULL;
	
	// Check empty list
	if ( m_pHead == NULL )
		return;

	lockOn();	
	pTemp = m_pHead->next;
	
	// Todo : Check memory error...	
	if ( m_pCurrent == m_pHead )
		m_pCurrent = pTemp;
	else
		m_pCurrent = NULL;

	HeapFree(GetProcessHeap(), 0, m_pHead);

	if ( pTemp == NULL ) 
	{		
		m_pHead = NULL;
		m_pTail = NULL;		
	}
	else 
	{	
		m_pHead = pTemp;		
	}
	m_nSize--;
	lockOff();
}

void MsgList::AddMsg(DWORD dwClient, void* pBuffer, int nSize)
{
	if (!pBuffer)
        return;

	if (!m_pCurrent)
		m_pCurrent = AddTail();

	if (m_pCurrent && nSize > 0)
	{
	    m_pCurrent->dwClient = dwClient;		
	    ::CopyMemory(m_pCurrent->Buffer, pBuffer, nSize);
	    m_pCurrent = m_pCurrent->next;
    }
#ifndef _RELEASE
    else
    {
        sc::writeLogError(sc::string::format("MsgList::AddMsg nSize<0 dwClient %1%, nSize %2%", dwClient, nSize));
        return;
    }
#endif // _RELEASE
}


MSG_LIST* MsgList::GetMsg()
{
	if (m_pRead == m_pCurrent || !m_pRead)
	{
		return NULL;
	}
	else
	{
		MSG_LIST* pTemp = m_pRead;
		m_pRead = m_pRead->next;
		return pTemp;
	}
}

// Returns the number of elements in this list. 
INT MsgList::GetCount() const
{
	return m_nSize;
}

// Tests for the empty list condition (no elements). 
bool MsgList::IsEmpty() const
{	
	return m_nSize == 0 ? 1 : 0;
}

// Clear all message buffer of the list
void MsgList::Reset()
{
	m_pCurrent	= m_pHead;
	m_pRead		= m_pHead;
}

#endif	//TBB



///////////////////////////////////////////////////////////////////////////////
// class CSMsgManager
// 서버 메시지 관리기
///////////////////////////////////////////////////////////////////////////////

static MSG_LIST*		AllocateMsgBuffer()
{
	MSG_LIST*	pBuff = (MSG_LIST*)HeapAlloc( GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof( MSG_LIST ) );
	if( !pBuff )
		sc::writeLogError( "Failed allocation message buffer");

	return pBuff;
} //AddBuffer

static void			DeallocateMsgBuffer( MSG_LIST* pBuff )
{
	HeapFree( GetProcessHeap(), 0, pBuff );
} //DeallocateMsgBuffer

MsgManager::MsgManager(int nAmount)
#ifdef _USE_TBB
{
	m_nAllocCount = 0;
	MSG_LIST*		pBuff;
	for( int i = 0; i < nAmount; i++ )
	{
		pBuff = AllocateMsgBuffer();
		if( !pBuff )
			break;

		m_queueReservedMsg.push( pBuff );
		m_nAllocCount++;
	} //for
	
#else
	:m_pMsgFront(NULL)
	,m_pMsgBack(NULL)
{
	// m_pMsgFront = NULL;
	// m_pMsgBack  = NULL;
	m_pMsgFront = new MsgList(nAmount);
	m_pMsgBack  = new MsgList(nAmount);
#endif
}



MsgManager::~MsgManager()
{
#ifdef _USE_TBB
	MSG_LIST*	pMsg = NULL;
	int			nDealloc = 0;
	while( m_queueReservedMsg.try_pop( pMsg ) )
	{		
		DeallocateMsgBuffer( pMsg );
		nDealloc++;
	} //while	

	while( m_queueMsg.try_pop( pMsg ) )
	{		
		DeallocateMsgBuffer( pMsg );
		nDealloc++;
	} //while

	if( m_nAllocCount != nDealloc )
		sc::writeLogError( std::string( "Allocated and Deallocated are not size mismatch" ) );

#else	
	SAFE_DELETE(m_pMsgFront);
	SAFE_DELETE(m_pMsgBack);
#endif
}


#ifndef _USE_TBB
// Front Buffer 와 Back Buffer 를 Flip 한다
void MsgManager::MsgQueueFlip()
{
	lockOn();
	MsgList* pTemp = NULL;
	pTemp		= m_pMsgFront;
	m_pMsgFront = m_pMsgBack;
	m_pMsgBack	= pTemp;
	m_pMsgFront->Reset();
	lockOff();
}
#endif


// Front Buffer 에 메시지를 삽입한다
void MsgManager::MsgQueueInsert( int nClient, void* pMsg, int nSize )
{
	if( !pMsg || nSize <= 0 )
	{
#ifndef _RELEASE
		sc::writeLogError(std::string("MsgManager::MsgQueueInsert pMsg NULL"));
#endif
		return;
	} //if

#ifdef _USE_TBB
	MSG_LIST*		pBuff;
	if( !m_queueReservedMsg.try_pop( pBuff ) )
	{
		pBuff = AllocateMsgBuffer();
		lockOn();
		m_nAllocCount++;
		lockOff();
	} //if

	if( !pBuff )
	{
#ifndef _RELEASE
		sc::writeLogError( sc::string::format( "MsgManager::MsgQueueInsert() failure - dwClient %1%, nSize %2%", nClient, nSize ) );
		return;
#endif
	} //if

	pBuff->dwClient = nClient;
	::CopyMemory( pBuff->Buffer, pMsg, nSize);
	m_queueMsg.push( pBuff );

#else	//_USE_TBB
	lockOn();
	m_pMsgFront->AddMsg(nClient, pMsg, nSize);
	lockOff();

#endif	//TBB
}


// Back Buffer 에서 메시지를 가져온다
#ifdef _USE_TBB
MSG_LIST* MsgManager::GetMsg( MSG_LIST* pUsedMsg )
{
	MSG_LIST*	pBuff;

	if( pUsedMsg )
		m_queueReservedMsg.push( pUsedMsg );

	if( !m_queueMsg.try_pop( pBuff ) )
		return NULL;

	return pBuff;
} //MsgManager::GetMsg
#else
MSG_LIST* MsgManager::GetMsg( void* DUMMY )
{
	return m_pMsgBack->GetMsg();
}
#endif //_USE_TBB

} // namespace net
}// namespace sc