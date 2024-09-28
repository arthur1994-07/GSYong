#include "pch.h"
#include "../../SigmaCore/Util/Lock.h"
#include "../s_NetGlobal.h"
#include "SendMsgBuffer.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CSendMsgBuffer::CSendMsgBuffer(/* CMinLzo* pMinLzo */)
	: m_dwPos(0)
	, m_pBuffer(NULL)
	, m_pSendBuffer(NULL)
	, m_usCount(0)
	// m_pMinLzo(pMinLzo)
{
	::InitializeCriticalSection(&m_CriticalSection);

	m_pBuffer     = new BYTE[NET_DATA_MSG_BUFSIZE];
	m_pSendBuffer = new BYTE[NET_DATA_MSG_BUFSIZE];
}

CSendMsgBuffer::~CSendMsgBuffer()
{
	if (m_pBuffer)
	{
		delete [] m_pBuffer;
		m_pBuffer = NULL;
	}

	if (m_pSendBuffer)
	{
		delete [] m_pSendBuffer;
		m_pSendBuffer = NULL;
	}

	::DeleteCriticalSection(&m_CriticalSection);
}

int CSendMsgBuffer::addMsg(NET_MSG_GENERIC* pMsg, DWORD dwSize)
{
	if (!pMsg || dwSize > NET_DATA_BUFSIZE) 
	{
		// �߸��� ������ �̰ų� ����� �ʰ��ϴ� �޽����̴�.
		return CSendMsgBuffer::BUFFER_ERROR;
	}

    sc::CriticalSectionOwnerLock OwnerLock(m_CriticalSection);
	
	// �� �޽����� �������� ũ�Ⱑ �󸶰� �Ǵ��� �����Ѵ�.
	DWORD dwTotal = m_dwPos + dwSize;	
	
	// �Ѹ޽��� ũ�Ⱑ �����ؾ� �� ũ�� �����̸�...
	if (dwTotal < COMPRESS_PACKET_SIZE)
	{
		// ���� ���ۿ� �޽����� �߰��Ѵ�.
		// ���� ��ġ ���� / ��Ŷ ī���� ����
		// ���� ������ �ʿ䰡 ������ �˸���.
		memcpy(m_pBuffer+m_dwPos, pMsg, dwSize);
		m_dwPos += dwSize;
		m_usCount++;
		return CSendMsgBuffer::BUFFER_ADDED;
	}
	else // �Ѹ޽��� ũ�Ⱑ �����ؾ� �� ũ�� �̻��̸�...
	{
		if (m_dwPos == 0)
		{
			// ���� ������ �� �޽����� ���µ�...
			// �ϳ��� �޽��� ũ�Ⱑ �����ؾ� �� ũ�⺸�� ũ�ٸ�...
			// ������ �� �޽����� �����ϰ�...
			// �����ؾ� �Ѵ�.
			memcpy(m_pBuffer, pMsg, dwSize);
			m_dwPos = dwSize;
			m_usCount++;
			return CSendMsgBuffer::BUFFER_SEND;
		}
		else // m_dwPos != 0
		{
			// ���� �������� �޽����� �ְ�
			// ���� ������ �� �޽����� ��ġ�� �����ؾ� �� ũ�⺸�� ũ��
			// �޽����� �����ϰ� �ٽ� �����϶�� �˷��� �Ѵ�.
			return CSendMsgBuffer::BUFFER_SEND_ADD;
		}
	}
}

int CSendMsgBuffer::getSendSize()
{
	// �����ؾ� �� �����Ͱ� ����.
	if (m_dwPos == 0)
		return 0;
	
	sc::CriticalSectionOwnerLock OwnerLock(m_CriticalSection);

	int nOutLength = NET_DATA_MSG_BUFSIZE - sizeof(NET_COMPRESS);
	int nCompressResult = 0;
	NET_COMPRESS* pNmc = NULL;
	DWORD dwSendSize = 0;
	
	// ����
    nCompressResult = sc::CMinLzo::GetInstance().lzoCompress(
        m_pBuffer, 
        m_dwPos, 
        m_pSendBuffer+sizeof(NET_COMPRESS),
        nOutLength);

    if (nCompressResult == sc::CMinLzo::MINLZO_SUCCESS)
	{
		// ���༺��, ������ ���ļ� �籸��
		pNmc = (NET_COMPRESS*) m_pSendBuffer;
		pNmc->nType = NET_MSG_COMPRESS;
		// ������ �� ũ�� ���
		dwSendSize = sizeof(NET_COMPRESS) + nOutLength;
		pNmc->SetSize( dwSendSize );
		pNmc->bCompress = true;
	}
	else
	{
	
		// ������� ������ ������ ����.
		memcpy(m_pSendBuffer+sizeof(NET_COMPRESS), m_pBuffer, m_dwPos);
		// �����͸� ��ģ��.
		pNmc = (NET_COMPRESS*) m_pSendBuffer;
		pNmc->nType = NET_MSG_COMPRESS;
		// ������ �� ũ�� ���
		dwSendSize = sizeof(NET_COMPRESS) + m_dwPos;
		pNmc->SetSize( dwSendSize );
		pNmc->bCompress = false;
	
	}
	
	// ���� ��ġ ���� / ��Ŷ ī���� �ʱ�ȭ.
	m_dwPos = 0;
	m_usCount = 0;
	// ���� �����ؾ� �� ����Ʈ ���� �����Ѵ�.
	return (int) dwSendSize;
}