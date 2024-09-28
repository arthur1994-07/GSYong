#include "../pch.h"
#include <WinSock2.h>
#include "../../SigmaCore/Log/LogMan.h"
#include "../../SigmaCore/Net/NetDefine.h"
#include "../../SigmaCore/Net/NetUtil.h"
#include "../../SigmaCore/String/StringFormat.h"
#include "../../RanLogic/s_NetGlobal.h"
#include "../../RanLogic/Network/NetLogicDefine.h"
#include "./NetServerHelper.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace net
{

CONNECT_RESULT ConnectAndReceive( 
								 const char* szServerIP, 
								 int nPort, 
								 HANDLE hIOServer,
								 DWORD SlotNum )
{
    hostent* hst;
    char szIpAddress[MAX_IP_LENGTH+1] = { 0 };
    unsigned long ulIP;
    struct in_addr inetAddr;

    hst = ::gethostbyname( szServerIP );
    if (hst == NULL)
    {
        sc::writeLogError( std::string( "gethostbyname hst==NULL" ) );
        SOCKET ConnectSocket = INVALID_SOCKET;
        return boost::make_tuple( NET_ERROR, ConnectSocket );
    }
    ulIP = *( DWORD* )( *hst->h_addr_list );
    inetAddr.s_addr = ulIP;
    ::StringCchCopy( szIpAddress, MAX_IP_LENGTH+1, ::inet_ntoa( inetAddr ) );
        
    return ConnectAndReceive( inet_addr( szIpAddress ), nPort, hIOServer, SlotNum );
}

CONNECT_RESULT ConnectAndReceive( 
								 unsigned long ulServerIP, 
								 int nPort, 
								 HANDLE hIOServer, 
								 DWORD SlotNum )
{
    SOCKET ConnectSocket = ::WSASocket( PF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED );
    if ( ConnectSocket == INVALID_SOCKET )
    {
		int errCode = WSAGetLastError();
        sc::writeLogError( sc::string::format( "WSASocket %1%", sc::net::getLastError( errCode ) ) );
        return boost::make_tuple( NET_ERROR, ConnectSocket );
    }

    SOCKADDR_IN Addr;
    ::SecureZeroMemory( &Addr, sizeof( SOCKADDR_IN ) );
    Addr.sin_family      = AF_INET;    
    Addr.sin_port        = ::htons( ( u_short ) nPort );
    Addr.sin_addr.s_addr = ulServerIP;

    // Connect to server
    int nRetCode = ::connect( ConnectSocket, ( sockaddr* ) &Addr, sizeof( SOCKADDR_IN ) );
    if ( nRetCode == SOCKET_ERROR )
    {	
		int errCode = WSAGetLastError();
        sc::writeLogError( sc::string::format( "ConnectAndReceive connect %1%", sc::net::getLastError( errCode ) ) );
        ::closesocket( ConnectSocket );
        return boost::make_tuple( NET_ERROR, ConnectSocket );
    }

    HANDLE hRetCode = CreateIoCompletionPort( ( HANDLE ) ConnectSocket, hIOServer, SlotNum, 0 );
    if (hRetCode == NULL) 
    {
        sc::writeLogError( sc::string::format( "ConnectAndReceive CreateIoCompletionPort Error %1%", GetLastError() ) );
        ::closesocket( ConnectSocket );
        return boost::make_tuple( NET_ERROR, ConnectSocket );
    }

    return boost::make_tuple( NET_OK, ConnectSocket );
}

}