#include "../pch.h"
#include <boost/lexical_cast.hpp>
#include "../../SigmaCore/Xml/2.5.3/ticpp.h"
#include "../../SigmaCore/Util/SystemInfo.h"
#include "../../SigmaCore/Log/LogMan.h"
#include "../../SigmaCore/Net/NetDefine.h"
#include "../../SigmaCore/String/StringFormat.h"
#include "../../SigmaCore/Encrypt/DbEncrypt.h"
#include "./CacheServerConfig.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CacheServerConfig::CacheServerConfig()
    : m_SessionServerPort(4848)
{
    m_pXml = new ticpp::Document;
}

CacheServerConfig::~CacheServerConfig()
{
    SAFE_DELETE(m_pXml);
}

void CacheServerConfig::WriteErrorLog(ticpp::Exception& ex)
{   
    sc::writeLogError(ex.m_details);
}

bool CacheServerConfig::Read()
{   
    try
    {
        std::string FilePath(sc::getAppPath());
        FilePath.append("\\cfg\\");
        FilePath.append("CacheServer.xml");

        m_pXml->LoadFile(FilePath, TIXML_ENCODING_UTF8);
        return Parse();
    }
    catch (ticpp::Exception& ex)
    {
        WriteErrorLog(ex);
        return false;
    }
}

bool CacheServerConfig::Parse()
{
    if (!ParseServicePort() ||
        !ParseWorkThreadNum() ||
        !ParseMaxClientNum() ||
        !ParseSessionServer() ||
        !ParseLogDatabase())
    {
        return false;
    }
    else
    {
        return true;
    }
}

bool CacheServerConfig::ParseServicePort()
{
    try
    {
        ticpp::Element* pElem = (ticpp::Element*) m_pXml->
            FirstChild("RAN_SERVER")->
            FirstChild("SERVER")->
            FirstChild("PORT")->
            FirstChild("SERVICE_PORT");
        pElem->GetTextOrDefault<int, int> (&m_nServicePort, sc::net::CACHE_SERVER_PORT);
        
        sc::writeLogInfo(sc::string::format("Service Port:%1%", m_nServicePort));
        return true;
    }
    catch (ticpp::Exception& ex)
    {
        // Report the error
        WriteErrorLog(ex);
        m_nServicePort = sc::net::CACHE_SERVER_PORT;        
        sc::writeLogWarn(
            sc::string::format(
                "RAN_SERVER/SERVER/PORT/SERVICE_PORT value not define using default value:%1%",
                m_nServicePort));
        return true;
    }
}

bool CacheServerConfig::ParseWorkThreadNum()
{
    const int MaxThreadNum = 4;
    const int MinThreadNum = 2;
    try
    {
        ticpp::Element* pElem = (ticpp::Element*) m_pXml->
            FirstChild("RAN_SERVER")->
            FirstChild("SERVER")->
            FirstChild("THREAD");
        pElem->GetTextOrDefault<int, int> (&m_nMaxThread, MinThreadNum);
        if (m_nMaxThread > MaxThreadNum)
            m_nMaxThread = MaxThreadNum;
        sc::writeLogInfo(sc::string::format("Work thread num:%1%", m_nMaxThread));
        return true;
    }
    catch (ticpp::Exception& ex)
    {
        // Report the error
        WriteErrorLog(ex);
        m_nMaxThread = MinThreadNum;
        sc::writeLogWarn(
            sc::string::format(
            "RAN_SERVER/SERVER/THREAD value not define using default value:%1%",
            m_nMaxThread));
        return true;
    }
}

bool CacheServerConfig::ParseMaxClientNum()
{
    const int DefaultMaxClientNum = 200;
    try
    {
        ticpp::Element* pElem = (ticpp::Element*) m_pXml->
            FirstChild("RAN_SERVER")->
            FirstChild("SERVER")->
            FirstChild("MAX_CLIENT");
        pElem->GetTextOrDefault<int, int> (&m_nServerMaxClient, DefaultMaxClientNum);
        if (m_nServerMaxClient < DefaultMaxClientNum)
            m_nServerMaxClient = DefaultMaxClientNum;
        sc::writeLogInfo(sc::string::format("Max client num:%1%", m_nServerMaxClient));
        return true;
    }
    catch (ticpp::Exception& ex)
    {
        // Report the error
        WriteErrorLog(ex);
        m_nServerMaxClient = DefaultMaxClientNum;
        sc::writeLogWarn(
            sc::string::format(
            "RAN_SERVER/SERVER/MAX_CLIENT value not define using default value:%1%",
            200));
        return true;
    }
}

bool CacheServerConfig::ParseLogDatabase()
{
    return ParseDatabase(        
        "LOG_DB",
        m_LogDbSource,
        m_LogDbUser,
        m_LogDbPass,
        m_LogDbDatabase,
        m_nLogDBResponseTime);
}

bool CacheServerConfig::ParseDatabase(
    const std::string& DbName,
    std::string& DbSource,
    std::string& UserName,
    std::string& UserPass,
    std::string& CatalogName,
    int& TimeOut)
{
    try
    {
        sc::DbEncrypt Encrypt;
        ticpp::Element* pElem = NULL;
        pElem = (ticpp::Element*) m_pXml->FirstChild("RAN_SERVER")->FirstChild(DbName)->FirstChild("DATA_SOURCE");
        DbSource = pElem->GetText(true);

        pElem = (ticpp::Element*) m_pXml->FirstChild("RAN_SERVER")->FirstChild(DbName)->FirstChild("CATALOG");
        CatalogName = pElem->GetText(true);
        
        pElem = (ticpp::Element*) m_pXml->FirstChild("RAN_SERVER")->FirstChild(DbName)->FirstChild("ID");
        std::string Id = pElem->GetText(true);
        UserName = Encrypt.getDecrypt(Id);
        
        pElem = (ticpp::Element*) m_pXml->FirstChild("RAN_SERVER")->FirstChild(DbName)->FirstChild("PASSWORD");
        std::string Password = pElem->GetText(true);
        UserPass = Encrypt.getDecrypt(Password);

        pElem = (ticpp::Element*) m_pXml->FirstChild("RAN_SERVER")->FirstChild(DbName)->FirstChild("TIMEOUT");
        std::string strTimeOut = pElem->GetTextOrDefault("15");
        TimeOut = boost::lexical_cast<int> (strTimeOut);

#ifdef _DEBUG
        sc::writeLogInfo(DbSource);
        sc::writeLogInfo(CatalogName);
        sc::writeLogInfo(UserName);
        sc::writeLogInfo(UserPass);
        sc::writeLogInfo(strTimeOut);
#endif
        return true;
    }
    catch (ticpp::Exception& ex)
    {
        // Report the error
        WriteErrorLog(ex);
        return false;
    }
}

bool CacheServerConfig::ParseSessionServer()
{
    try
    {
        ticpp::Element* pElem = NULL;		
        pElem = (ticpp::Element*) m_pXml->FirstChild("RAN_SERVER")->FirstChild("SESSION_SERVER");
        pElem->GetAttribute("IP", &m_SessionServerIp);
        pElem->GetAttribute("PORT", &m_SessionServerPort);
        sc::writeLogInfo(
            sc::string::format(
                "loaded config: Session Server %1% %2%", m_SessionServerIp, m_SessionServerPort));
        return true;
    }
    catch (ticpp::Exception& ex)
    {
        // Report the error
        sc::writeLogError(std::string("RAN_SERVER/SESSION_SERVER not exist"));
        WriteErrorLog(ex);
        return false;
    }
    return true;
}