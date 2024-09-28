#ifndef _NET_SERVER_HELPER_H_
#define _NET_SERVER_HELPER_H_

#pragma once

#include <boost/tuple/tuple.hpp> 
#include <WinSock2.h>

struct PER_IO_OPERATION_DATA;

namespace net
{
    typedef boost::tuple< int, SOCKET > CONNECT_RESULT;

    //! 주소와 Port 로 접속한다.
	//! WSARecv 는 이제 여기에서 하지 않는다. reference counting 때문에
    CONNECT_RESULT ConnectAndReceive( 
		const char* szServerIP, 
		int nPort, 
		HANDLE hIOServer,
		DWORD SlotNum );

    CONNECT_RESULT ConnectAndReceive( 
		unsigned long ulServerIP, 
		int nPort, 
		HANDLE hIOServer, 
		DWORD SlotNum );
}

#endif // _NET_SERVER_HELPER_H_