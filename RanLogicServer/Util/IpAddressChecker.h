#ifndef _LOGIC_SERVER_IP_ADDRESS_CHECKER_H_
#define _LOGIC_SERVER_IP_ADDRESS_CHECKER_H_

#pragma once

#include <map>
#include <string>
#include <set>
#include <boost/regex.hpp>

//! 2012-01-02 jgkim
//!
//! http://en.wikipedia.org/wiki/Private_network
//! private ip range
//! 10.0.0.0 - 10.255.255.255 (10/8 prefix) 
//! 172.16.0.0 - 172.31.255.255 (172.16/12 prefix) 
//! 192.168.0.0 - 192.168.255.255 (192.168/16 prefix)


class IpAddressChecker
{
public:
    IpAddressChecker(UINT AllowConnectCount=BLOCK_CONNECTION_COUNT);
    ~IpAddressChecker();

public:
    enum 
    { 
        BLOCK_CONNECTION_COUNT = 50,
    };
    typedef std::map<std::string, UINT>  IP_DATA;
    typedef IP_DATA::iterator            IP_DATA_ITER;
    typedef IP_DATA::const_iterator      IP_DATA_CITER;
    typedef IP_DATA::value_type          IP_DATA_VALUE;

    typedef std::set<std::string>    IP_ALLOW;
    typedef IP_ALLOW::iterator       IP_ALLOW_ITER;
    typedef IP_ALLOW::const_iterator IP_ALLOW_CITER;
    
    typedef std::set<std::string>    IP_BLOCK;
    typedef IP_ALLOW::iterator       IP_BLOCK_ITER;
    typedef IP_ALLOW::const_iterator IP_BLOCK_CITER;

protected:
    CRITICAL_SECTION m_CriticalSection;
    UINT m_AllowCount;

    boost::regex* m_pRegEx1;
    boost::regex* m_pRegEx2;
    boost::regex* m_pRegEx3;
    boost::regex* m_pRegEx4;

    IP_ALLOW m_IpAllow;
    IP_BLOCK m_IpBlock;
    IP_DATA m_IpData;

private:
    bool InitRegEx();
    bool IsPrivateIp(const std::string& IpAddress);
    void AddBlockIp(const std::string& IpAddress);

public:
    inline void SetConnectionCountAllow(UINT AllowCount) { m_AllowCount = AllowCount; }
    void AddAllowIp(const std::string& IpAddress);
    bool IsAllowIp(const std::string& IpAddress);
    void AddConnection(const std::string& IpAddress);
    void DelConnection(const std::string& IpAddress);
    void ClearIpData();
};

#endif // _LOGIC_SERVER_IP_ADDRESS_CHECKER_H_