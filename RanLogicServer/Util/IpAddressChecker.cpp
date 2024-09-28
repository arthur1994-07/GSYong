#include "pch.h"
#include <iostream>
#include <WinSock2.h>
//#include <boost/regex.hpp>
#include "../../SigmaCore/String/StringFormat.h"
#include "../../SigmaCore/Util/Lock.h"
#include "../../SigmaCore/Log/LogMan.h"
#include "./IpAddressChecker.h"


#include "../../SigmaCore/DebugInclude.h"


IpAddressChecker::IpAddressChecker(UINT AllowConnectCount)
    : m_AllowCount(AllowConnectCount)
{
    InitializeCriticalSectionAndSpinCount(&m_CriticalSection, sc::CRITICAL_SECTION_SPIN_COUNT);
    
    m_pRegEx1 = new boost::regex;
    m_pRegEx2 = new boost::regex;;
    m_pRegEx3 = new boost::regex;;
    m_pRegEx4 = new boost::regex;;

    // private ip init
    InitRegEx();
}

IpAddressChecker::~IpAddressChecker()
{
    delete m_pRegEx4;
    delete m_pRegEx3;
    delete m_pRegEx2;
    delete m_pRegEx1;

    DeleteCriticalSection(&m_CriticalSection);   
}

bool IpAddressChecker::InitRegEx()
{
    // private ip range
    // 127.0.0.1
    // 10.0.0.0 - 10.255.255.255 (10/8 prefix) 
    // 172.16.0.0 - 172.31.255.255 (172.16/12 prefix) 
    // 192.168.0.0 - 192.168.255.255 (192.168/16 prefix)
    std::string Expression1("(10\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3})");    
    std::string Expression2("(^172\\.1[6-9]\\.\\d{1,3}\\.\\d{1,3})|(^172\\.2[0-9]\\.\\d{1,3}\\.\\d{1,3})|(^172\\.3[0-1]\\.\\d{1,3}\\.\\d{1,3})");
    std::string Expression3("(192\\.168\\.\\d{1,3}\\.\\d{1,3})");
    std::string Expression4("(^127\\.0\\.0\\.1)");

    try
    {
        // ��ҹ��� ����
        m_pRegEx1->assign(Expression1, boost::regex_constants::icase);
        m_pRegEx2->assign(Expression2, boost::regex_constants::icase);
        m_pRegEx3->assign(Expression3, boost::regex_constants::icase);
        m_pRegEx4->assign(Expression4, boost::regex_constants::icase);
        return true;
    }
    catch (boost::regex_error& e)
    {
        std::string ErrMsg = sc::string::format(
            "Not a valid regular expression: %1%",
            e.what());
        sc::writeLogError(ErrMsg);
        //std::cout << ErrMsg << std::endl;
        return false;
    }
}

bool IpAddressChecker::IsPrivateIp(const std::string& IpAddress)
{
    try
    {
        if (boost::regex_match(IpAddress, *m_pRegEx1) ||
            boost::regex_match(IpAddress, *m_pRegEx2) ||
            boost::regex_match(IpAddress, *m_pRegEx3) ||
            boost::regex_match(IpAddress, *m_pRegEx4)) 
        {
            //std::cout << "private ip " << IpAddress << std::endl;
            return true;
        }
        else
        {
            //std::cout << "not private ip " << IpAddress << std::endl;
            return false;
        }
    }
    catch (boost::regex_error& e)
    {
        std::string ErrMsg = sc::string::format("The regexp is invalid! %1%", e.what());
        sc::writeLogError(ErrMsg);
        return false;
    }
	catch (...)
	{
		std::string ErrMsg("IpAddressChecker::IsPrivateIp unknown error");
		sc::writeLogError(ErrMsg);		
		return false;
	}
}

void IpAddressChecker::AddAllowIp(const std::string& IpAddress)
{
    sc::CriticalSectionOwnerLock OwnerLock(m_CriticalSection);
    m_IpAllow.insert(IpAddress);
}

bool IpAddressChecker::IsAllowIp(const std::string& IpAddress)
{
    sc::CriticalSectionOwnerLock OwnerLock(m_CriticalSection);
    
    // private ip
    if (IsPrivateIp(IpAddress))
        return true;

    // ���Ǵ� ip ��� ������ ����Ѵ�.
    IP_ALLOW_CITER AllowIter = m_IpAllow.find(IpAddress);
    if (AllowIter != m_IpAllow.end())
        return true;

    // Block ip ��� ������ block �Ѵ�.
    IP_BLOCK_CITER BlockIter = m_IpBlock.find(IpAddress);
    if (BlockIter != m_IpBlock.end())
        return false;

    // ������ �ʴ� ip ��� ������ üũ�ؼ� ��뿩�θ� �����Ѵ�.
    IP_DATA_ITER iter = m_IpData.find(IpAddress);
    if (iter != m_IpData.end())
    {
        UINT ConnectionCount = iter->second;
        if (ConnectionCount >= m_AllowCount)
        {
            // Block ip �� ����Ѵ�.
            AddBlockIp(IpAddress);
            m_IpData.erase(iter);
            return false;
        }
        else
        {
            return true;
        }
    }
    else
    {
        return true;
    }
}

void IpAddressChecker::AddConnection(const std::string& IpAddress)
{
    sc::CriticalSectionOwnerLock OwnerLock(m_CriticalSection);

    // private ip
    if (IsPrivateIp(IpAddress))
        return;
    
    // ���Ǵ� ip ��� ������ �����Ͻ� �ʴ´�.
    IP_ALLOW_CITER AllowIter = m_IpAllow.find(IpAddress);
    if (AllowIter != m_IpAllow.end())
        return;

    // ip �� ���ؼ� ���Ӽ��ڸ� 1 ������Ų��.
    IP_DATA_ITER iter = m_IpData.find(IpAddress);
    if (iter != m_IpData.end())
    {
        iter->second++;
    }
    else
    {
        m_IpData.insert(IP_DATA_VALUE(IpAddress, 1));
    }
}

void IpAddressChecker::DelConnection(const std::string& IpAddress)
{
    sc::CriticalSectionOwnerLock OwnerLock(m_CriticalSection);

    // private ip
    if (IsPrivateIp(IpAddress))
        return;

    // ���Ǵ� ip ��� ������ �����Ͻ� �ʴ´�.
    IP_ALLOW_CITER AllowIter = m_IpAllow.find(IpAddress);
    if (AllowIter != m_IpAllow.end())
        return;

    // ip �� ���ؼ� ���Ӽ��ڸ� 1���ҽ�Ų��.
    IP_DATA_ITER iter = m_IpData.find(IpAddress);
    if (iter != m_IpData.end())
    {
        UINT& ConnectionCount = iter->second;
        ConnectionCount--;
        if (ConnectionCount == 0)
            m_IpData.erase(iter);
    }
}

void IpAddressChecker::AddBlockIp(const std::string& IpAddress)
{
    m_IpBlock.insert(IpAddress);
}

void IpAddressChecker::ClearIpData()
{
    sc::CriticalSectionOwnerLock OwnerLock(m_CriticalSection);
    m_IpBlock.clear();
    m_IpData.clear();
}