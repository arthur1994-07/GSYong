#ifndef _CACHE_SERVER_CONFIG_H_
#define _CACHE_SERVER_CONFIG_H_

#pragma once

#include <boost/noncopyable.hpp>

namespace ticpp
{
    class Document;
    class Exception;
}

class CacheServerConfig sealed : private boost::noncopyable
{
public:
    CacheServerConfig();
    ~CacheServerConfig();

private:
    ticpp::Document* m_pXml;

    int m_nServicePort; //! ���񽺿� ��Ʈ    
    int m_nServiceProvider; //! ���� ���� ȸ������
    int m_nServerMaxClient; //! �ִ� ���� ���� Ŭ���̾�Ʈ ��
    int m_nMaxThread; //! �ִ� Work Thread ����.
    int m_nLangSet; //! Language Set

    std::string m_SessionServerIp;
    int m_SessionServerPort;
        
    //! -----------------------------------------------------------------------
    //! database setting value 	
    //! -----------------------------------------------------------------------

    //! Log database
    std::string	m_LogDbSource;
    std::string	m_LogDbUser;
    std::string	m_LogDbPass;
    std::string	m_LogDbDatabase;
    int	m_nLogDBResponseTime;

private:
    void WriteErrorLog(ticpp::Exception& ex);
    bool ParseDatabase(
        const std::string& DbName,
        std::string& DbSource,
        std::string& UserName,
        std::string& UserPass,
        std::string& CatalogName,
        int& TimeOut);

    bool Parse();
    bool ParseSessionServer();
    bool ParseServicePort();
    bool ParseWorkThreadNum();
    bool ParseMaxClientNum();    
    bool ParseLogDatabase();
    
public:
    //! ���������� �д���.
    //! /cfg/CacheServer.xml
    bool Read();

    //! �ִ� ���� ���� Ŭ���̾�Ʈ ��
    int MaxClient() const { return m_nServerMaxClient; }
    int MaxWorkThread() const { return m_nMaxThread; }
    //! ���񽺿� ��Ʈ
    int ServicePort() const { return m_nServicePort; }

    std::string LogDbSource()       const { return m_LogDbSource; }
    std::string LogDbUser()         const { return m_LogDbUser; }
    std::string LogDbPassword()     const { return m_LogDbPass; }
    std::string LogDbDatabaseName() const { return m_LogDbDatabase; }
    int         LogDbTimeOut()      const { return m_nLogDBResponseTime; }

    std::string SessionServerIp() const { return m_SessionServerIp; }
    int SessionServerPort() const { return m_SessionServerPort; }
};

#endif // _CACHE_SERVER_CONFIG_H_