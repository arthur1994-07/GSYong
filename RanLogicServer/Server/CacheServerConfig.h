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

    int m_nServicePort; //! 서비스용 포트    
    int m_nServiceProvider; //! 서비스 제공 회사유형
    int m_nServerMaxClient; //! 최대 접속 가능 클라이언트 수
    int m_nMaxThread; //! 최대 Work Thread 갯수.
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
    //! 설정파일을 읽느다.
    //! /cfg/CacheServer.xml
    bool Read();

    //! 최대 접속 가능 클라이언트 수
    int MaxClient() const { return m_nServerMaxClient; }
    int MaxWorkThread() const { return m_nMaxThread; }
    //! 서비스용 포트
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