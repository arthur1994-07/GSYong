#include "pch.h"
#include "../../SigmaCore/Compress/MinLzo.h"
#include "../../SigmaCore/Log/LogMan.h"

#include "../s_NetGlobal.h"
#include "SendMsgBuffer.h"
#include "RcvMsgBuffer.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

//const char* GARBAGE_DATA[] = { "K9IHANA", "L8IDUL", "M7HSET", "N6GNET", "O5FDASEOT" };
							   //"P4EYEOSEOT", "Q3DILGOP", "R2CYEODELOP", "S1BAHOP", "T0AYEOL" */};

//! CRcvMsgBuffer 의 생성자
//! \param pMinLzo Lzo 압축라이브러리의 포인터
CRcvMsgBuffer::CRcvMsgBuffer()
	: m_pRcvBuffer(NULL)
	, m_pOneMsg(NULL)
	, m_pDecompressBuffer(NULL)
	, m_nRcvSize(0)
{
	m_pRcvBuffer        = new BYTE[NET_DATA_CLIENT_MSG_BUFSIZE];
	m_pOneMsg           = new BYTE[NET_DATA_BUFSIZE];
	m_pDecompressBuffer = new BYTE[NET_DATA_BUFSIZE];

	::InitializeCriticalSection(&m_CriticalSection);
}
 
CRcvMsgBuffer::~CRcvMsgBuffer(void)
{
	SAFE_DELETE_ARRAY(m_pRcvBuffer);
	SAFE_DELETE_ARRAY(m_pOneMsg);
	SAFE_DELETE_ARRAY(m_pDecompressBuffer);

	::DeleteCriticalSection(&m_CriticalSection);
}

//! 메시지를 수신버퍼에 복사해 넣는다.
//! \param pMsg 메시지 포인터
//! \param nSize 메시지 크기
//! \return CMsgBuffer::BUFFER_ERROR 잘못된 포인터 이거나 메시지 크기를 초과하는 메시지
//!         다른 모든 경우 총 수신 크기를 리턴해 준다.
int CRcvMsgBuffer::addRcvMsg(void* pMsg, int nSize)
{	
	if (!pMsg || nSize > NET_DATA_BUFSIZE)
	{
		// 잘못된 포인터 이거나 사이즈를 초과하는 메시지이다.
		return CSendMsgBuffer::BUFFER_ERROR;
	}
	else
	{
        sc::CriticalSectionOwnerLock OwnerLock(m_CriticalSection);
		memcpy(m_pRcvBuffer+m_nRcvSize, pMsg, nSize);
		m_nRcvSize += nSize;
		return m_nRcvSize;
	}
}

/*
    압축된 메시지의 구조
	NET_COMPRESS (Head)
	-------------------------------------
	|  dwSize    |	nType   | bCompress |
	-------------------------------------
	      ^            ^          ^
	총메시지크기  메시지타입   압축여부

	----------------------------------------------------------
	| NET_COMPRESS |   Data...                               |	
	----------------------------------------------------------
	^              ^                                         ^
	pNmc           pNmc+sizeof(NET_COMPRESS)                 pNmc+pNmc->dwSize
		           |                                         |
		           |<--- pNmc->dwSize-sizeof(NET_COMPRES) -->|
*/

//! 뭉쳐서 날아온 메시지를 압축해제 또는 재 구성하여 하나씩 돌려준다.
//!
//! -------------------------------------------------------------------------------
//! | Type1 | Size1 | Data1 | Type2 | Size2 | Data2 | Type3 | Size3 | Data3 |......
//! -------------------------------------------------------------------------------
//! ^                       ^                       ^
//! m_nPos                  m_nPos+(Type1->Size)    m_nPos+(Type1->Size+Type2->Size)
//!
//! \return NULL 메시지가 없는 경우
//!         다른 모든경우 해당 메시지의 포인터를 넘겨준다.
void* CRcvMsgBuffer::getMsg()
{
	sc::CriticalSectionOwnerLock OwnerLock(m_CriticalSection);

	NET_MSG_GENERIC* pNmg = (NET_MSG_GENERIC*) m_pRcvBuffer;


	if ((m_nRcvSize < sizeof(NET_MSG_GENERIC)) || 
		(m_nRcvSize < (int) pNmg->Size()))
	{
		// 데이터가 없다.
		return NULL;
	}
	

	if (pNmg->Size() == 0 || pNmg->Size() > NET_DATA_BUFSIZE)
	{
		sc::writeLogError(
            sc::string::format(
                "RcMsgBuffer Error 02 : %1% / %2%", pNmg->Size(), pNmg->Type() ));
		resetPosition();
		return NULL;
	}

	//	압축된 메세지는 클라이언트에서 올수 없다.
	if (NET_MSG_COMPRESS == pNmg->nType)
	{		
		NET_COMPRESS* pPacket = (NET_COMPRESS*) pNmg;

		// 압축여부 검사
		if (pPacket->bCompress)
		{
			// 압축된 메시지이다...
			// 전체 메시지 크기는...
			int nCompressSize = (int) pPacket->Size();
			
			// 뭉쳐서 온 메시지이고 압축되어 있다면...
			int nDeCompressSize = NET_DATA_BUFSIZE;

			if (pNmg->Size() < sizeof(NET_COMPRESS))
			{
                sc::writeLogError(
                    sc::string::format(
                        "RcMsgBuffer Error_03 : %1%", pNmg->Size()));
				resetPosition();
				return NULL;
			}	

            int nResult = sc::CMinLzo::GetInstance().lzoDeCompress(
                (byte*) pPacket+sizeof(NET_COMPRESS),
                pPacket->Size()-sizeof(NET_COMPRESS), 
                m_pDecompressBuffer,
                nDeCompressSize);

            if (nResult == sc::CMinLzo::MINLZO_SUCCESS)
			{
				// 압축해제 성공
				int nMoveSize = m_nRcvSize - nCompressSize;
				::MoveMemory(m_pRcvBuffer+nDeCompressSize, m_pRcvBuffer+nCompressSize, nMoveSize);
				::CopyMemory(m_pRcvBuffer,                 m_pDecompressBuffer,        nDeCompressSize);
				m_nRcvSize = nDeCompressSize + nMoveSize;				
			}
			else
			{
				// 압축해제 실패
				// 뭉쳐온 메시지를 날려버리고 뒤쪽 메시지를 앞으로 이동시킨다.
				{
                    sc::writeLogError(
                        sc::string::format(
                            "RcMsgBuffer Error 04(DeCompress Error) : %1%", nCompressSize));
				}
				
				int nMoveSize = m_nRcvSize-nCompressSize;

				if ( nMoveSize <= 0 ) 
				{
                    sc::writeLogError(
                        sc::string::format(
                            "RcMsgBuffer Error 05 : %1%", nMoveSize));
					
					resetPosition();
					return NULL;
				}

				::MoveMemory(m_pRcvBuffer, m_pRcvBuffer+nCompressSize, nMoveSize);
				m_nRcvSize = nMoveSize;
				return NULL;
			}
		}
		else
		{
			// 뭉쳐서 온 메시지 이고 압축되지 않았다면...			
			int nMoveSize = m_nRcvSize - (int) sizeof(NET_COMPRESS);
			
			if ( nMoveSize <= 0 ) 
			{
                sc::writeLogError(
                    sc::string::format(
                        "RcMsgBuffer Error 06 : %1%", nMoveSize));
				resetPosition();
				return NULL;
			}

			::MoveMemory(m_pRcvBuffer, m_pRcvBuffer+sizeof(NET_COMPRESS), nMoveSize);
			m_nRcvSize = nMoveSize;
		}
	}

	return getOneMsg();
}

void* CRcvMsgBuffer::getOneMsg()
{
	NET_MSG_GENERIC* pNmg = (NET_MSG_GENERIC*) m_pRcvBuffer;

	int nOneMsgSize = pNmg->Size();

	// 하나의 메시지를 돌려주기 위해서 버퍼로 복사하고...
	::CopyMemory(m_pOneMsg, m_pRcvBuffer, nOneMsgSize);

	// 복사한 만큼 나머지 메시지를 앞으로 땡긴다.
	// 옮겨야할 메시지 크기는...
	int nMoveSize = m_nRcvSize-nOneMsgSize;
	if (nMoveSize > 0)
	{
		::MoveMemory(m_pRcvBuffer, m_pRcvBuffer+nOneMsgSize, nMoveSize);
	}
	else
	{
		nMoveSize = 0;
	}
	// 이제 전체 수신메시지 크기는 감소한다.
	m_nRcvSize = nMoveSize;	
    return m_pOneMsg;
}

void CRcvMsgBuffer::resetPosition()
{
    m_nRcvSize = 0;	
}

int CRcvMsgBuffer::getRcvSize()
{
	return m_nRcvSize;
}

void CRcvMsgBuffer::reset()
{
	m_nRcvSize = 0;
	::SecureZeroMemory(m_pRcvBuffer,        NET_DATA_CLIENT_MSG_BUFSIZE);
	::SecureZeroMemory(m_pOneMsg,           NET_DATA_BUFSIZE);
	::SecureZeroMemory(m_pDecompressBuffer, NET_DATA_BUFSIZE);
}