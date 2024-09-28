#ifndef _SC_NET_UTIL_H_
#define _SC_NET_UTIL_H_

#pragma once

#include <string>
#include <WinSock2.h>

namespace sc
{
    namespace net
    {
        //! 네트워크 초기화
        int initializeNetwork();

        //! 네크워크 종료
        void closeNetwork();

        //! Winsock2 에러 코드->문자열 함수
        //std::string getLastError();
        std::string getLastError(int nCode);

        //! ip 또는 domain name 으로 접속 
        //! ex) 127.0.0.1 or login.ranonline.co.kr, 
        unsigned long inetAddr(const wchar_t* szAddr);
        unsigned long inetAddr(const char* szAddr);

        hostent* FAR getHostByName(const wchar_t* szName);
        hostent* FAR getHostByName(const char* szName);

        std::wstring inet_ntoaW(in_addr in);
        std::string inet_ntoaA(in_addr in);
    }
}

#endif // _SC_NET_UTIL_H_