#pragma once

#include "../../SigmaCore/Compress/MinLzo.h"

struct NET_MSG_GENERIC;

/**
 * \ingroup NetServerLib
 * \date 2006-03-07
 * \author Jgkim
 *
 *  MTU 1500
 * IPv6 : MTU - 60 = 1440 Bytes
 * IPv4 : MTU - 40 = 1460 Bytes
 * ----------------------------------------------------------------------
 * | Size(4) | Type(4) | Compress(1) | Count(2) | Data(...)             |
 * ----------------------------------------------------------------------
 */
class CSendMsgBuffer
{
public:
	enum 
    { 
        BUFFER_SEND     = 1,
        BUFFER_ADDED    = 2,
        BUFFER_SEND_ADD = 3,
        BUFFER_ERROR    = -1
    };

public:	
	//! CSendMsgBuffer 의 생성자
	//! 
	CSendMsgBuffer(/* CMinLzo* pMinLzo */);
	~CSendMsgBuffer();

	int addMsg(NET_MSG_GENERIC* pMsg, DWORD dwSize);
	void reset()								{ m_dwPos = m_usCount = 0; }
	void* getSendBuffer()						{ return m_pSendBuffer; }
	DWORD getMsgSize()							{ return m_dwPos; }
	unsigned short getMsgCount()				{ return m_usCount; }
	int getSendSize();

protected:
	DWORD m_dwPos;
	unsigned short m_usCount;
    //sc::CMinLzo* m_pMinLzo;
	BYTE* m_pBuffer;
	BYTE* m_pSendBuffer;

	CRITICAL_SECTION m_CriticalSection; ///< criticalsection object
};

