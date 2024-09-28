#ifndef _SC_NET_UTIL_H_
#define _SC_NET_UTIL_H_

#pragma once

#include <string>
#include <WinSock2.h>

namespace sc
{
    namespace net
    {
        //! ��Ʈ��ũ �ʱ�ȭ
        int initializeNetwork();

        //! ��ũ��ũ ����
        void closeNetwork();

        //! Winsock2 ���� �ڵ�->���ڿ� �Լ�
        //std::string getLastError();
        std::string getLastError(int nCode);

        //! ip �Ǵ� domain name ���� ���� 
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