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

//! CRcvMsgBuffer �� ������
//! \param pMinLzo Lzo ������̺귯���� ������
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

//! �޽����� ���Ź��ۿ� ������ �ִ´�.
//! \param pMsg �޽��� ������
//! \param nSize �޽��� ũ��
//! \return CMsgBuffer::BUFFER_ERROR �߸��� ������ �̰ų� �޽��� ũ�⸦ �ʰ��ϴ� �޽���
//!         �ٸ� ��� ��� �� ���� ũ�⸦ ������ �ش�.
int CRcvMsgBuffer::addRcvMsg(void* pMsg, int nSize)
{	
	if (!pMsg || nSize > NET_DATA_BUFSIZE)
	{
		// �߸��� ������ �̰ų� ����� �ʰ��ϴ� �޽����̴�.
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
    ����� �޽����� ����
	NET_COMPRESS (Head)
	-------------------------------------
	|  dwSize    |	nType   | bCompress |
	-------------------------------------
	      ^            ^          ^
	�Ѹ޽���ũ��  �޽���Ÿ��   ���࿩��

	----------------------------------------------------------
	| NET_COMPRESS |   Data...                               |	
	----------------------------------------------------------
	^              ^                                         ^
	pNmc           pNmc+sizeof(NET_COMPRESS)                 pNmc+pNmc->dwSize
		           |                                         |
		           |<--- pNmc->dwSize-sizeof(NET_COMPRES) -->|
*/

//! ���ļ� ���ƿ� �޽����� �������� �Ǵ� �� �����Ͽ� �ϳ��� �����ش�.
//!
//! -------------------------------------------------------------------------------
//! | Type1 | Size1 | Data1 | Type2 | Size2 | Data2 | Type3 | Size3 | Data3 |......
//! -------------------------------------------------------------------------------
//! ^                       ^                       ^
//! m_nPos                  m_nPos+(Type1->Size)    m_nPos+(Type1->Size+Type2->Size)
//!
//! \return NULL �޽����� ���� ���
//!         �ٸ� ����� �ش� �޽����� �����͸� �Ѱ��ش�.
void* CRcvMsgBuffer::getMsg()
{
	sc::CriticalSectionOwnerLock OwnerLock(m_CriticalSection);

	NET_MSG_GENERIC* pNmg = (NET_MSG_GENERIC*) m_pRcvBuffer;


	if ((m_nRcvSize < sizeof(NET_MSG_GENERIC)) || 
		(m_nRcvSize < (int) pNmg->Size()))
	{
		// �����Ͱ� ����.
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

	//	����� �޼����� Ŭ���̾�Ʈ���� �ü� ����.
	if (NET_MSG_COMPRESS == pNmg->nType)
	{		
		NET_COMPRESS* pPacket = (NET_COMPRESS*) pNmg;

		// ���࿩�� �˻�
		if (pPacket->bCompress)
		{
			// ����� �޽����̴�...
			// ��ü �޽��� ũ���...
			int nCompressSize = (int) pPacket->Size();
			
			// ���ļ� �� �޽����̰� ����Ǿ� �ִٸ�...
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
				// �������� ����
				int nMoveSize = m_nRcvSize - nCompressSize;
				::MoveMemory(m_pRcvBuffer+nDeCompressSize, m_pRcvBuffer+nCompressSize, nMoveSize);
				::CopyMemory(m_pRcvBuffer,                 m_pDecompressBuffer,        nDeCompressSize);
				m_nRcvSize = nDeCompressSize + nMoveSize;				
			}
			else
			{
				// �������� ����
				// ���Ŀ� �޽����� ���������� ���� �޽����� ������ �̵���Ų��.
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
			// ���ļ� �� �޽��� �̰� ������� �ʾҴٸ�...			
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

	// �ϳ��� �޽����� �����ֱ� ���ؼ� ���۷� �����ϰ�...
	::CopyMemory(m_pOneMsg, m_pRcvBuffer, nOneMsgSize);

	// ������ ��ŭ ������ �޽����� ������ �����.
	// �Űܾ��� �޽��� ũ���...
	int nMoveSize = m_nRcvSize-nOneMsgSize;
	if (nMoveSize > 0)
	{
		::MoveMemory(m_pRcvBuffer, m_pRcvBuffer+nOneMsgSize, nMoveSize);
	}
	else
	{
		nMoveSize = 0;
	}
	// ���� ��ü ���Ÿ޽��� ũ��� �����Ѵ�.
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