#ifndef _NET_SERVER_HELPER_H_
#define _NET_SERVER_HELPER_H_

#pragma once

#include <boost/tuple/tuple.hpp> 
#include <WinSock2.h>

struct PER_IO_OPERATION_DATA;

namespace net
{
    typedef boost::tuple< int, SOCKET > CONNECT_RESULT;

    //! �ּҿ� Port �� �����Ѵ�.
	//! WSARecv �� ���� ���⿡�� ���� �ʴ´�. reference counting ������
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