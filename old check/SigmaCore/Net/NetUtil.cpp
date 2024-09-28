#include "stdafx.h"
#include "../String/StringFormat.h"
#include "./NetDefine.h"
#include "./NetUtil.h"

// ----------------------------------------------------------------------------
#include "../DebugInclude.h"
// ----------------------------------------------------------------------------
namespace sc
{
namespace net
{

//! 네트워크 초기화
int initializeNetwork()
{
    // Load winsock2 dll
    WSADATA wsd;
    int nRetCode = ::WSAStartup( MAKEWORD(2,2), &wsd );
    if ( nRetCode != 0 )
    {
        return NET_ERROR;
    } 
    else 
    {
        return NET_OK;
    }
}

//! 네크워크 종료
void closeNetwork()
{
    // Unload winsock2 dll
    ::WSACleanup();
}

//std::string getLastError()
//{
	// 여기서 WSAGetLastError 검출하면 정확한 에러값이 나오지 않는다.
	// 더이상 사용하지 못하도록 주석처리 한다.
//    return getLastError(::WSAGetLastError());
//}

//! Return socket error string.
std::string getLastError(int nCode)
{
    std::string strTemp;
    switch (nCode)
    {
    case WSA_INVALID_HANDLE : // 6  The handle is invalid.
        strTemp = sc::string::format("WSA_INVALID_HANDLE %d", nCode);
        break;
    case WSA_NOT_ENOUGH_MEMORY : // 8 Not enough storage is available to process this command.
        strTemp = sc::string::format("WSA_NOT_ENOUGH_MEMORY %d", nCode);
        break;
    case WSA_INVALID_PARAMETER : // 87  The parameter is incorrect.
        strTemp = sc::string::format("WSA_INVALID_PARAMETER %d", nCode);
        break;
    case WSAEINTR : // 10004 Interrupted function call.
        strTemp = sc::string::format("WSAEINTR %d", nCode);
        break;
    case WSAEACCES : // 10013 Permission denied.
        strTemp = sc::string::format("WSAEACCES %d", nCode);
        break;
    case WSAEFAULT : // 10014 Bad address.
        strTemp = sc::string::format("WSAEFAULT %d", nCode);
        break;
    case WSAEINVAL : // 10022 Invalid argument. 
        strTemp = sc::string::format("WSAEINVAL %d", nCode);
        break;
    case WSAEMFILE : // 10024 Too many open files. 
        strTemp = sc::string::format("WSAEMFILE %d", nCode);
        break;
    case WSAEWOULDBLOCK : // 10035 Resource temporarily unavailable. 
        strTemp = sc::string::format("WSAEWOULDBLOCK %d", nCode);
        break;
    case WSAEINPROGRESS : // 10036 Operation now in progress.
        strTemp = sc::string::format("WSAEINPROGRESS %d", nCode);
        break;
    case WSAEALREADY : // 10037 Operation already in progress. 
        strTemp = sc::string::format("WSAEALREADY %d", nCode);
        break;
    case WSAENOTSOCK : // 10038 Socket operation on nonsocket.
        strTemp = sc::string::format("WSAENOTSOCK %d", nCode);
        break;
    case WSAEDESTADDRREQ : // 10039 Destination address required.
        strTemp = sc::string::format("WSAEDESTADDRREQ %d", nCode);
        break;
    case WSAEMSGSIZE : // 10040 Message too long.
        strTemp = sc::string::format("WSAEMSGSIZE %d", nCode);
        break;
    case WSAEPROTOTYPE : // 10041 Protocol wrong type for socket.
        strTemp = sc::string::format("WSAEPROTOTYPE %d", nCode);
        break;
    case WSAENOPROTOOPT : // 10042 Bad protocol option.
        strTemp = sc::string::format("WSAENOPROTOOPT %d", nCode);
        break;
    case WSAEPROTONOSUPPORT : // 10043 Protocol not supported. 
        strTemp = sc::string::format("WSAEPROTONOSUPPORT %d", nCode);
        break;
    case WSAESOCKTNOSUPPORT : // 10044 The support for the specified socket type does not exist in this address family.
        strTemp = sc::string::format("WSAESOCKTNOSUPPORT %d", nCode);
        break;
    case WSAEOPNOTSUPP : // 10045 The attempted operation is not supported for the type of object referenced.
        strTemp = sc::string::format("WSAEOPNOTSUPP %d", nCode);
        break;
    case WSAEPFNOSUPPORT : // 10046 The protocol family has not been configured into the system or no implementation for it exists.
        strTemp = sc::string::format("WSAEPFNOSUPPORT %d", nCode);
        break;
    case WSAEAFNOSUPPORT : // 10047 An address incompatible with the requested protocol was used.
        strTemp = sc::string::format("WSAEAFNOSUPPORT %d", nCode);
        break;
    case WSAEADDRINUSE : // 10048 Only one usage of each socket address (protocol/network address/port) is normally permitted.
        strTemp = sc::string::format("WSAEADDRINUSE %d", nCode);
        break;
    case WSAEADDRNOTAVAIL : // 10049 The requested address is not valid in its context.
        strTemp = sc::string::format("WSAEADDRNOTAVAIL %d", nCode);
        break;
    case WSAENETDOWN : // 10050 A socket operation encountered a dead network.
        strTemp = sc::string::format("WSAENETDOWN %d", nCode);
        break;
    case WSAENETUNREACH : // 10051 A socket operation was attempted to an unreachable network.
        strTemp = sc::string::format("WSAENETUNREACH %d", nCode);
        break;
    case WSAENETRESET : // 10052 The connection has been broken due to keep-alive activity detecting a failure while the operation was in progress.
        strTemp = sc::string::format("WSAENETRESET %d", nCode);
        break;
    case WSAECONNABORTED : // 10053 An established connection was aborted by the software in your host machine.
        strTemp = sc::string::format("WSAECONNABORTED %d", nCode);
        break;
    case WSAECONNRESET : // 10054 An existing connection was forcibly closed by the remote host.
        strTemp = sc::string::format("WSAECONNRESET %d", nCode);
        break;
    case WSAENOBUFS : // 10055 An operation on a socket could not be performed because the system lacked sufficient buffer space or because a queue was full.
        strTemp = sc::string::format("WSAENOBUFS %d", nCode);
        break;
    case WSAEISCONN : // 10056 A connect request was made on an already connected socket.
        strTemp = sc::string::format("WSAEISCONN %d", nCode);
        break;
    case WSAENOTCONN : // 10057 A request to send or receive data was disallowed because the socket is not connected and (when sending on a datagram socket using a sendto call) no address was supplied.
        strTemp = sc::string::format("WSAENOTCONN %d", nCode);
        break;
    case WSAESHUTDOWN : // 10058 A request to send or receive data was disallowed because the socket had already been shut down in that direction with a previous shutdown call.
        strTemp = sc::string::format("WSAESHUTDOWN %d", nCode);
        break;
    case WSAETOOMANYREFS : // 10059 Too many references to some kernel object.
        strTemp = sc::string::format("WSAETOOMANYREFS %d", nCode);
        break;
    case WSAETIMEDOUT : // 10060 A connection attempt failed because the connected party did not properly respond after a period of time, or established connection failed because connected host has failed to respond.
        strTemp = sc::string::format("WSAETIMEDOUT %d", nCode);
        break;
    case WSAECONNREFUSED : // 10061 No connection could be made because the target machine actively refused it.
        strTemp = sc::string::format("WSAECONNREFUSED %d", nCode);
        break;
    case WSAELOOP : // 10062 Cannot translate name.
        strTemp = sc::string::format("WSAELOOP %d", nCode);
        break;
    case WSAENAMETOOLONG : // 10063 Name component or name was too long. 
        strTemp = sc::string::format("WSAENAMETOOLONG %d", nCode);
        break;
    case WSAEHOSTDOWN : // 10064 A socket operation failed because the destination host was down.
        strTemp = sc::string::format("WSAEHOSTDOWN %d", nCode);
        break;
    case WSAEHOSTUNREACH : // 10065 A socket operation was attempted to an unreachable host.
        strTemp = sc::string::format("WSAEHOSTUNREACH %d", nCode);
        break;
    case WSAENOTEMPTY : // 10066 Cannot remove a directory that is not empty.
        strTemp = sc::string::format("WSAENOTEMPTY %d", nCode);
        break;
    case WSAEPROCLIM : // 10067 A Windows Sockets implementation may have a limit on the number of applications that may use it simultaneously.
        strTemp = sc::string::format("WSAEPROCLIM %d", nCode);
        break;
    case WSAEUSERS : // 10068 Ran out of quota.
        strTemp = sc::string::format("WSAEUSERS %d", nCode);
        break;
    case WSAEDQUOT : // 10069 Ran out of disk quota.
        strTemp = sc::string::format("WSAEDQUOT %d", nCode);
        break;
    case WSAESTALE : // 10070 File handle reference is no longer available.
        strTemp = sc::string::format("WSAESTALE %d", nCode);
        break;
    case WSAEREMOTE :// 10071 Item is not available locally.
        strTemp = sc::string::format("WSAEREMOTE %d", nCode);
        break;
    case WSASYSNOTREADY : // 10091 WSAStartup cannot function at this time because the underlying system it uses to provide network services is currently unavailable.
        strTemp = sc::string::format("WSASYSNOTREADY %d", nCode);
        break;
    case WSAVERNOTSUPPORTED : // 10092 The Windows Sockets version requested is not supported.
        strTemp = sc::string::format("WSAVERNOTSUPPORTED %d", nCode);
        break;
    case WSANOTINITIALISED : // 10093 Either the application has not called WSAStartup, or WSAStartup failed.
        strTemp = sc::string::format("WSANOTINITIALISED %d", nCode);
        break;
    case WSAEDISCON : // 10101 Returned by WSARecv or WSARecvFrom to indicate the remote party has initiated a graceful shutdown sequence.
        strTemp = sc::string::format("WSAEDISCON %d", nCode);
        break;
    case WSAENOMORE : // 10102 No more results can be returned by WSALookupServiceNext.
        strTemp = sc::string::format("WSAENOMORE %d", nCode);
        break;
    case WSAECANCELLED : // 10103 A call to WSALookupServiceEnd was made while this call was still processing. The call has been canceled.
        strTemp = sc::string::format("WSAECANCELLED %d", nCode);
        break;
    case WSAEINVALIDPROCTABLE : // 10104 The procedure call table is invalid.
        strTemp = sc::string::format("WSAEINVALIDPROCTABLE %d", nCode);
        break;
    case WSAEINVALIDPROVIDER : // 10105 The requested service provider is invalid.
        strTemp = sc::string::format("WSAEINVALIDPROVIDER %d", nCode);
        break;
    case WSAEPROVIDERFAILEDINIT : // 10106 The requested service provider could not be loaded or initialized.
        strTemp = sc::string::format("WSAEPROVIDERFAILEDINIT %d", nCode);
        break;
    case WSASYSCALLFAILURE : // 10107 A system call that should never fail has failed.
        strTemp = sc::string::format("WSASYSCALLFAILURE %d", nCode);
        break;
    case WSASERVICE_NOT_FOUND : // 10108 No such service is known. The service cannot be found in the specified name space.
        strTemp = sc::string::format("WSASERVICE_NOT_FOUND %d", nCode);
        break;
    case WSATYPE_NOT_FOUND : // 10109 The specified class was not found.
        strTemp = sc::string::format("WSATYPE_NOT_FOUND %d", nCode);
        break;
    case WSA_E_NO_MORE : // 10110 No more results can be returned by WSALookupServiceNext.
        strTemp = sc::string::format("WSA_E_NO_MORE %d", nCode);
        break;
    case WSA_E_CANCELLED : // 10111 A call to WSALookupServiceEnd was made while this call was still processing. The call has been canceled.
        strTemp = sc::string::format("WSA_E_CANCELLED %d", nCode);
        break;
    case WSAEREFUSED : // 10112 A database query failed because it was actively refused.
        strTemp = sc::string::format("WSAEREFUSED %d", nCode);
        break;
    case WSAHOST_NOT_FOUND : // 11001 No such host is known.
        strTemp = sc::string::format("WSAHOST_NOT_FOUND %d", nCode);
        break;
    default :
        strTemp = sc::string::format("Unknown Socket ERROR code %d", nCode);
    }
    return strTemp;
}

unsigned long inetAddr(const wchar_t* szAddr)
{
    char szAddrANSI[MAX_PATH];
    ::WideCharToMultiByte(CP_ACP, 0, szAddr, -1, szAddrANSI, MAX_PATH, NULL, NULL);
    szAddrANSI[MAX_PATH-1] = 0;
    return inetAddr(szAddrANSI);
}

unsigned long inetAddr(const char* szAddr)
{
    unsigned long ulReturn = ::inet_addr(szAddr);
    if (INADDR_NONE != ulReturn)
        return ulReturn;

    hostent* pHost = getHostByName(szAddr);
    if (!pHost)
        return INADDR_NONE;

    ulReturn = *(DWORD*) (*pHost->h_addr_list);
    return ulReturn;
}

hostent* FAR getHostByName(const wchar_t* szName)
{
    char szNameANSI[MAX_PATH]={0};
    ::WideCharToMultiByte(CP_ACP, 0, szName, -1, szNameANSI, MAX_PATH, NULL, NULL);
    szNameANSI[MAX_PATH-1] = 0;
    return getHostByName(szNameANSI);
}

hostent* FAR getHostByName(const char* szName)
{
    return ::gethostbyname(szName);
}

std::wstring inet_ntoaW(in_addr in)
{
    char* pName = ::inet_ntoa(in);
    WCHAR szNameW[MAX_PATH]={0};
    ::MultiByteToWideChar(CP_ACP, 0, pName, -1, szNameW, MAX_PATH);
    return std::wstring(szNameW);
}

std::string inet_ntoaA(in_addr in)
{
    char* pName = ::inet_ntoa(in);	
    return std::string(pName);
}

} // namespace net
} // namespace sc