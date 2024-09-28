#ifndef SC_NET_DEFINE_H_
#define SC_NET_DEFINE_H_

#pragma once

#include <winsock2.h>

#include "../gassert.h"


// Definitions
// #define can changed to const...
// ex) const int NET_NOTIRY = WM_USER + 99;
#define NET_NOTIFY					WM_USER + 99  // Network Notify Message

enum {
    NET_DATA_BUFSIZE			= 2048, //! Data Buffer Size
    NET_DATA_MSG_BUFSIZE		= 4096, //! 
    NET_DATA_CLIENT_MSG_BUFSIZE = 32768,
    COMPRESS_PACKET_SIZE        = 1000, //! 압축의 기준이 되는 크기
};

#define NET_OK			  1
#define NET_ERROR		 -1
#define NET_INVALID_SLOT 99999

// Connection protocols
// This version not support UDP protocol
#define NET_TCP 0
#define NET_UDP 1

enum EMMSG_IO_TYPE
{
    NET_SEND_POSTED = 0, //! IOCP send post
    NET_RECV_POSTED = 1, //! IOCP receive post
};

enum
{
    GARBAGE_MSG_SIZE = 12,
};

//! 메시지 저장을 위한 구조체
//! nClient : 메시지를 보낸 클라이언트 번호
//! Buffer : 실제 메시지 데이타 NET_DATA_BUFSIZE 는 현재 NET_DATA_BUFSIZE 바이트이다.

struct MSG_LIST
{
    DWORD		dwClient;
    CHAR		Buffer[NET_DATA_BUFSIZE];
    float		fDelay;

    MSG_LIST	*prev;
    MSG_LIST	*next;

    MSG_LIST()
        : dwClient(0)
        , fDelay(0.0f)
        , prev(NULL)
        , next(NULL)
    {
        memset(Buffer, 0, NET_DATA_BUFSIZE * sizeof(CHAR));		
    }
};
typedef MSG_LIST* LPMSG_LIST;


struct PER_IO_OPERATION_DATA
{
    OVERLAPPED	Overlapped; //! 항상 최상위에 있어야 한다. 위치이동 금지!!!
    WSABUF		DataBuf;
    CHAR		Buffer[NET_DATA_BUFSIZE];
    EMMSG_IO_TYPE OperationType;
    DWORD		dwTotalBytes;
    DWORD		dwSndBytes;
    DWORD		dwRcvBytes;
    //INT			nRefCount;

    PER_IO_OPERATION_DATA()
        : OperationType(NET_SEND_POSTED)
        , dwTotalBytes(0)
        , dwSndBytes(0)
        , dwRcvBytes(0)
      //, nRefCount(0) // Set ref count to 0
    {
        Overlapped.Internal		= 0;
        Overlapped.InternalHigh	= 0;
        Overlapped.Offset		= 0;
        Overlapped.OffsetHigh	= 0;
        Overlapped.hEvent		= 0;

        DataBuf.buf				= Buffer;
        DataBuf.len				= NET_DATA_BUFSIZE;	        
    };

    void Reset(void)
    {		
        DataBuf.len				= NET_DATA_BUFSIZE;
        OperationType			= NET_SEND_POSTED;
        dwTotalBytes			= 0;
        dwSndBytes				= 0;
        dwRcvBytes				= 0;
      //nRefCount				= 0; // Set ref count to 0
    };
};
typedef PER_IO_OPERATION_DATA* LPPER_IO_OPERATION_DATA;

namespace sc
{
    namespace net
    {
        enum EMSERVER_PORT
        {            
            CACHE_SERVER_PORT = 6912,
        };

        const int DefaultWorkThreadNum = 2;
    }
}

#endif // SC_NET_DEFINE_H_