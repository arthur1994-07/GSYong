#include "../pch.h"
#pragma warning(disable:4267)
#include <boost/lexical_cast.hpp>
#include <boost/spirit/include/classic.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include "../../SigmaCore/Encrypt/DbEncrypt.h"
#include "../../SigmaCore/Log/LogMan.h"
#include "../../SigmaCore/Net/NetUtil.h"
#include "../../SigmaCore/String/StringFormat.h"
#include "../../SigmaCore/String/StringUtil.h"
#include "../../SigmaCore/Xml/2.5.3/ticpp.h"
#include "../../SigmaCore/Util/SystemInfo.h"
#include "../../SigmaCore/File/FileUtil.h"
#include "./ServerConfigHelper.h"
#include "./ServerConfigXml.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

ServerConfigXml::ServerConfigXml()
{
    m_pXml = new ticpp::Document;

    m_bPkLess = false;
    m_bPK = true;    
    m_bAdultCheck = false;    
    m_bUseEventThread = false;
	m_nFPS			  = 10;
    m_bTestServer = false;
    m_bHeartBeat = true;    
    m_nServerMaxClient = ServerConfig::DEFAULT_MAX_CLIENT;
    m_nMaxChannelUser = ServerConfig::DEFAULT_MAX_CLIENT;    
    m_nPortService = ServerConfig::DEFAULT_SERVICE_PORT;
    m_nPortControl = ServerConfig::DEFAULT_CONTORL_PORT;
    m_ServerType = net::SERVER_UNKNOWN;
    m_nServerGroup = ServerConfig::DEFAULT_SERVER_GROUP;
    m_nServerNumber = ServerConfig::DEFAULT_SERVER_NUMBER;    
    m_ServiceProvider = SP_OFFICE_TEST;

    m_nMaxThread = ServerConfig::DEFAULT_MAX_THREAD_NUMBER;
    m_LanguageSet = language::DEFAULT;

    m_ConnectionPerIpAddress = ServerConfig::DEFAULT_CONNECTION_PER_IP_ADDRESS;

    m_UseCustomOdbcPool = false;

	m_nReferenceCountMax = ServerConfig::DEFAULT_REFERENCE_COUNT_MAX;

	m_nAdoMaxPoolSize = ServerConfig::DEFAULT_ADO_MAX_POOL_SIZE;
	m_nDatabaseTcpPacketSize = ServerConfig::DEFAULT_DB_PACKET_SIZE;
}

ServerConfigXml::~ServerConfigXml()
{
    SAFE_DELETE(m_pXml);
}

void ServerConfigXml::WriteErrorLog(ticpp::Exception& ex)
{   
    sc::writeLogError(ex.m_details);
}

void ServerConfigXml::WriteWarningLog(ticpp::Exception& ex)
{
    sc::writeLogWarn(ex.m_details);
}

void ServerConfigXml::WriteDebugLog(ticpp::Exception& ex)
{
    sc::writeLogDebug(ex.m_details);
}

bool ServerConfigXml::Load(const std::string& ConfigXmlFile)
{
    try
    {
        std::string FilePath(sc::getAppPath());
        FilePath.append("\\cfg\\");
        FilePath.append(ConfigXmlFile);
        m_XmlFileName = FilePath;
        m_pXml->LoadFile(FilePath, TIXML_ENCODING_UTF8);
        return Parse();
    }
    catch (ticpp::Exception& ex)
    {
        WriteErrorLog(ex);
        return false;
    }
}

bool ServerConfigXml::Parse()
{
	sc::writeLogInfo(std::string(">Start Loading Server-Configurations."));

    if (!ParseServiceProvider())
        return false;

    if (!ParseServerType())
        return false;

    if (!ParseLanguageSet())
        return false;

    if (!ParseServerIp())
        return false;

    if (!ParseServicePort())
        return false;

    if (!ParseControlPort())
        return false;

    if (!ParseServerVersion())
        return false;

    if (!ParsePatchVersion())
        return false;

    if (!ParseVersionGap())
        return false;

    if (!ParsePkLess())
        return false;
    
    if (!ParsePk())
        return false;

    if (!ParseAdult())
        return false;

	/* TBB는 전처리기에 의해 컨트롤된다.

    if (!ParseIntelTbb())
        return false;
	*/

    if (!ParseEventThread())
        return false;

	if (!ParseFPS())
		return false;

    if (!ParseTestServer())
        return false;

    if (!ParseHeartbeat())
        return false;

    if (!ParseMaxClient())
        return false;

    if (!ParseMaxChannelClient())
        return false;   
    
    if (!ParseServerGroup())
        return false;

    if (!ParseServerNumber())
        return false; 
    
    if (!ParseMaxThreadNumber())
        return false;

    if (!ParseConnectionPerIpAddress())
        return false;

    if (!ParseUseCustomOdbcPool())
        return false;

	if ( !ParseReferenceCountMax() )
		return false;

	if ( !ParseAdoMaxPoolSize() )
		return false;

	if ( !ParseDatabaseTcpPacketSize() )
		return false;
    
    return true;
}

bool ServerConfigXml::ParseServerIp()
{
    try
    {        
        ticpp::Element* pElem = (ticpp::Element*) m_pXml->FirstChild("RAN_SERVER")->FirstChild("SERVER_IP");
        m_ServerIp = pElem->GetText(true);
        sc::writeLogInfo(
            sc::string::format(
				"loaded config: loaded config: SERVER_IP %1%", m_ServerIp));
        return true;
    }
    catch (ticpp::Exception& ex)
    {
        // Report the error
        WriteErrorLog(ex);
        m_ServerIp = "none";
        sc::writeLogError(
            std::string(
                "RAN_SERVER/SERVER_IP does not exist."));
        return false;
    }
}

bool ServerConfigXml::ParsePatchVersion()
{
    try
    {        
        ticpp::Element* pElem = (ticpp::Element*) m_pXml->FirstChild("RAN_SERVER")->FirstChild("PATCH_VERSION");
        pElem->GetTextOrDefault<int, int> (&m_LauncherVersion, 0);
        sc::writeLogInfo(
            sc::string::format(
            "loaded config: PATCH_VERSION %1%", m_LauncherVersion));
        return true;
    }
    catch (ticpp::Exception& ex)
    {
        // Report the error
        WriteErrorLog(ex);
        m_LauncherVersion = 0;
        sc::writeLogWarn(std::string("RAN_SERVER/PATCH_VERSION does not exist. It will be set to default value:0"));
        return true;
    }
}

bool ServerConfigXml::ParseServerVersion()
{
    try
    {        
        ticpp::Element* pElem = (ticpp::Element*) m_pXml->FirstChild("RAN_SERVER")->FirstChild("SERVER_VERSION");
        pElem->GetTextOrDefault<int, int> (&m_PatchVersion, 0);
        sc::writeLogInfo(
            sc::string::format(
            "loaded config: SERVER_VERSION %1%", m_PatchVersion));
        return true;
    }
    catch (ticpp::Exception& ex)
    {
        // Report the error
        WriteErrorLog(ex);
        m_PatchVersion = 0;
        sc::writeLogWarn(std::string("RAN_SERVER/SERVER_VERSION does not exist. It will be set to default value:0"));
        return true;
    }
}


bool ServerConfigXml::ParseVersionGap()
{
    try
    {        
        ticpp::Element* pElem = (ticpp::Element*) m_pXml->FirstChild("RAN_SERVER")->FirstChild("PATCH_CUT_VERSION");
        pElem->GetTextOrDefault<int, int> (&m_PatchCutVersion, 0);
        sc::writeLogInfo(
            sc::string::format(
            "loaded config: PATCH_CUT_VERSION %1%", m_PatchCutVersion));
        return true;
    }
    catch (ticpp::Exception& ex)
    {
        // Report the error
        WriteErrorLog(ex);
        m_PatchCutVersion = 0;
        sc::writeLogWarn(std::string("RAN_SERVER/PATCH_CUT_VERSION does not exist. It will be set to default value:0"));
        return true;
    }
}

bool ServerConfigXml::ParseLanguageSet()
{
    try
    {
        int nLanguage = 0;
        ticpp::Element* pElem = (ticpp::Element*) m_pXml->FirstChild("RAN_SERVER")->FirstChild("LANGUAGE_SET");
        pElem->GetTextOrDefault<int, int> (&nLanguage, language::DEFAULT);
        m_LanguageSet = language::LANGFLAG(nLanguage);
        sc::writeLogInfo(
            sc::string::format(
                "loaded config: LANGUAGE_SET %1%", m_LanguageSet));
        return true;
    }
    catch (ticpp::Exception& ex)
    {
        // Report the error
        WriteErrorLog(ex);
        m_LanguageSet = language::DEFAULT;
        sc::writeLogWarn(
            sc::string::format(
                "RAN_SERVER/LANGUAGE_SET does not exist. %1%", m_XmlFileName));
        return false;
    }
}

bool ServerConfigXml::ParseMaxThreadNumber()
{
    try
    {
        ticpp::Element* pElem = (ticpp::Element*) m_pXml->FirstChild("RAN_SERVER")->FirstChild("THREAD");
        pElem->GetTextOrDefault<int, int> (&m_nMaxThread, ServerConfig::DEFAULT_MAX_THREAD_NUMBER);
        sc::writeLogInfo(
            sc::string::format(
                "loaded config: THREAD %1%", m_nMaxThread));
        return true;
    }
    catch (ticpp::Exception& ex)
    {
        // Report the error
        WriteErrorLog(ex);
        m_nMaxThread = ServerConfig::DEFAULT_MAX_THREAD_NUMBER;
        sc::writeLogWarn(
            sc::string::format(
                "RAN_SERVER/THREAD does not exist. It will be set to default value:%1%", ServerConfig::DEFAULT_MAX_THREAD_NUMBER));
        return true;
    }
}

bool ServerConfigXml::ParseServiceProvider()
{       
    try
    {
        int ServiceProvider = 0;
        ticpp::Element* pElem = (ticpp::Element*) m_pXml->FirstChild("RAN_SERVER")->FirstChild("SERVICE_PROVIDER");
        pElem->GetTextOrDefault<int, int> (&ServiceProvider, ServerConfig::DEFAULT_SERVICE_PROVIDER);

        if (ServiceProvider < 0 || ServiceProvider >= SP_TOTAL_NUM)
        {
            sc::writeLogError(
                sc::string::format(
                    "SERVICE_PROVIDER wrong value %1%", ServiceProvider));
            return false;
        }
        
        m_ServiceProvider = EMSERVICE_PROVIDER(ServiceProvider);

        sc::writeLogInfo(
            sc::string::format(
                "loaded config: SERVICE_PROVIDER %1%", m_ServiceProvider));
        return true;
    }
    catch (ticpp::Exception& ex)
    {
        // Report the error
        WriteErrorLog(ex);
        m_ServiceProvider = SP_OFFICE_TEST;
        sc::writeLogError(
            sc::string::format(
                "RAN_SERVER/SERVICE_PROVIDER does not exist. %1%", m_XmlFileName));
        return false;
    }
}

bool ServerConfigXml::ParseServerNumber()
{
    try
    {        
        ticpp::Element* pElem = (ticpp::Element*) m_pXml->FirstChild("RAN_SERVER")->FirstChild("SERVER_NUMBER");
        pElem->GetTextOrDefault<int, int> (&m_nServerNumber, ServerConfig::DEFAULT_SERVER_NUMBER);
        sc::writeLogInfo(
            sc::string::format(
                "loaded config: SERVER_NUMBER %1%", m_nServerNumber));
        return true;
    }
    catch (ticpp::Exception& ex)
    {
        // Report the error
        WriteErrorLog(ex);
        m_nServerNumber = ServerConfig::DEFAULT_SERVER_NUMBER;
        sc::writeLogWarn(std::string("RAN_SERVER/SERVER_NUMBER does not exist. It will be set to default value:0"));
        return true;
    }
}

bool ServerConfigXml::ParseServerGroup()
{
    try
    {        
        ticpp::Element* pElem = (ticpp::Element*) m_pXml->FirstChild("RAN_SERVER")->FirstChild("SERVER_GROUP");
        pElem->GetTextOrDefault<int, int> (&m_nServerGroup, ServerConfig::DEFAULT_SERVER_GROUP);
        sc::writeLogInfo(
            sc::string::format(
                "loaded config: SERVER_GROUP %1%", m_nServerGroup));
        return true;
    }
    catch (ticpp::Exception& ex)
    {
        // Report the error
        WriteErrorLog(ex);
        m_nServerGroup = ServerConfig::DEFAULT_SERVER_GROUP;
        sc::writeLogWarn(std::string("RAN_SERVER/SERVER_GROUP does not exist. It will be set to default value:0"));
        return true;
    }
}

bool ServerConfigXml::ParseServerType()
{
    try
    {
        int nServerType = ServerConfig::DEFAULT_SERVER_TYPE;
        ticpp::Element* pElem = (ticpp::Element*) m_pXml->FirstChild("RAN_SERVER")->FirstChild("SERVER_TYPE");
        pElem->GetTextOrDefault<int, int> (&nServerType, ServerConfig::DEFAULT_SERVER_TYPE);
        m_ServerType = net::EMSERVERTYPE(nServerType);
        sc::writeLogInfo(
            sc::string::format(
                "loaded config: SERVER_TYPE %1%", m_ServerType));
        return true;
    }
    catch (ticpp::Exception& ex)
    {
        // Report the error
        WriteErrorLog(ex);
        m_ServerType = net::SERVER_UNKNOWN;
        sc::writeLogWarn(
            sc::string::format(
                "RAN_SERVER/SERVER_TYPE does not exist. %1%", m_XmlFileName));
        return false;
    }
}

bool ServerConfigXml::ParseControlPort()
{
    try
    {        
        ticpp::Element* pElem = (ticpp::Element*) m_pXml->FirstChild("RAN_SERVER")->FirstChild("CONTROL_PORT");
        pElem->GetTextOrDefault<int, int> (&m_nPortControl, ServerConfig::DEFAULT_CONTORL_PORT);
        sc::writeLogInfo(
            sc::string::format(
            "loaded config: CONTROL_PORT %1%", m_nPortControl));
        return true;
    }
    catch (ticpp::Exception& ex)
    {
        // Report the error
        WriteErrorLog(ex);
        m_nPortControl = ServerConfig::DEFAULT_CONTORL_PORT;
        sc::writeLogWarn(std::string("RAN_SERVER/CONTROL_PORT does not exist. It will be set to default value:5001"));
        return true;
    }
}

bool ServerConfigXml::ParseServicePort()
{
    try
    {        
        ticpp::Element* pElem = (ticpp::Element*) m_pXml->FirstChild("RAN_SERVER")->FirstChild("SERVICE_PORT");
        pElem->GetTextOrDefault<int, int> (&m_nPortService, ServerConfig::DEFAULT_SERVICE_PORT);
        sc::writeLogInfo(
            sc::string::format(
                "loaded config: SERVICE_PORT %1%", m_nPortService));
        return true;
    }
    catch (ticpp::Exception& ex)
    {
        // Report the error
        WriteErrorLog(ex);
        m_nPortService = ServerConfig::DEFAULT_SERVICE_PORT;
        sc::writeLogWarn(std::string("RAN_SERVER/SERVICE_PORT does not exist. It will be set to default value:5001"));
        return true;
    }
}

bool ServerConfigXml::ParseMaxChannelClient()
{
    try
    {        
        ticpp::Element* pElem = (ticpp::Element*) m_pXml->FirstChild("RAN_SERVER")->FirstChild("MAX_CHANNEL_CLIENT");
        pElem->GetTextOrDefault<int, int> (&m_nMaxChannelUser, ServerConfig::DEFAULT_MAX_CLIENT);
        sc::writeLogInfo(
            sc::string::format(
                "loaded config: MAX_CHANNEL_CLIENT %1%", m_nMaxChannelUser));
        return true;
    }
    catch (ticpp::Exception& ex)
    {
        // Report the error
        WriteErrorLog(ex);
        m_nMaxChannelUser = ServerConfig::DEFAULT_MAX_CLIENT;
        sc::writeLogWarn(std::string("RAN_SERVER/MAX_CHANNEL_CLIENT does not exist. It will be set to default value:1000"));
        return true;
    }
}

bool ServerConfigXml::ParseMaxClient()
{
    try
    {        
        ticpp::Element* pElem = (ticpp::Element*) m_pXml->FirstChild("RAN_SERVER")->FirstChild("MAX_CLIENT");
        pElem->GetTextOrDefault<int, int> (&m_nServerMaxClient, ServerConfig::DEFAULT_MAX_CLIENT);
        sc::writeLogInfo(
            sc::string::format(
                "loaded config: MAX_CLIENT %1%", m_nServerMaxClient));

        return true;
    }
    catch (ticpp::Exception& ex)
    {
        // Report the error
        WriteErrorLog(ex);
        m_nServerMaxClient = ServerConfig::DEFAULT_MAX_CLIENT;
        sc::writeLogWarn(std::string("RAN_SERVER/MAX_CLIENT does not exist. It will be set to default value:1000"));
        return true;
    }
}

bool ServerConfigXml::ParseHeartbeat()
{
    try
    {
        int nUse = 1;
        ticpp::Element* pElem = (ticpp::Element*) m_pXml->FirstChild("RAN_SERVER")->FirstChild("HEARTBEAT");
        pElem->GetTextOrDefault<int, int> (&nUse, 1);
        if (nUse == 1)
        {
            sc::writeLogInfo(std::string("loaded config: HEARTBEAT true"));
            m_bHeartBeat = true;
        }
        else
        {
            sc::writeLogInfo(std::string("loaded config: HEARTBEAT false"));
            m_bHeartBeat = false;
        }        
        return true;
    }
    catch (ticpp::Exception& ex)
    {
        // Report the error
        WriteErrorLog(ex);
        m_bHeartBeat = true;
        sc::writeLogWarn(std::string("RAN_SERVER/HEARTBEAT does not exist. It will be set to default value:true"));
        return true;
    }
}

bool ServerConfigXml::ParseTestServer()
{
    try
    {
        int nUse = 0;
        ticpp::Element* pElem = (ticpp::Element*) m_pXml->FirstChild("RAN_SERVER")->FirstChild("TEST_SERVER");
        pElem->GetTextOrDefault<int, int> (&nUse, 0);
        if (nUse == 1)
        {
            sc::writeLogInfo(std::string("loaded config: TEST_SERVER true"));
            m_bTestServer = true;
        }
        else
        {
            sc::writeLogInfo(std::string("loaded config: TEST_SERVER false"));
            m_bTestServer = false;
        }        
        return true;
    }
    catch (ticpp::Exception& ex)
    {
        // Report the error
        WriteErrorLog(ex);
        m_bTestServer = false;
        sc::writeLogWarn(std::string("RAN_SERVER/TEST_SERVER does not exist. It will be set to default value:false"));
        return true;
    }
}

bool ServerConfigXml::ParseEventThread()
{
    try
    {
        int nUse = 0;
        ticpp::Element* pElem = (ticpp::Element*) m_pXml->FirstChild("RAN_SERVER")->FirstChild("EVENT_THREAD");
        pElem->GetTextOrDefault<int, int> (&nUse, 0);
        if (nUse == 1)
        {
            sc::writeLogInfo(std::string("loaded config: EVENT_THREAD true"));
            m_bUseEventThread = true;
        }
        else
        {
            sc::writeLogInfo(std::string("loaded config: EVENT_THREAD false"));
            m_bUseEventThread = false;
        }        
        return true;
    }
    catch (ticpp::Exception& ex)
    {
        // Report the error
        WriteErrorLog(ex);
        m_bUseEventThread = false;
        sc::writeLogWarn(std::string("RAN_SERVER/EVENT_THREAD does not exist. It will be set to default value:false"));
        return true;
    }
}


bool ServerConfigXml::ParseFPS()
{
    try
    {   
		int nFPS = 10;
        ticpp::Element* pElem = (ticpp::Element*) m_pXml->FirstChild("RAN_SERVER")->FirstChild("FPS");
        pElem->GetTextOrDefault<int, int> (&nFPS, 10);
		sc::writeLogInfo(sc::string::format("loaded config: FPS %1%", nFPS));

		m_nFPS = nFPS;

        return true;
    }
    catch (ticpp::Exception& ex)
    {
        // Report the error
        WriteWarningLog(ex);
        m_nFPS = 10;
        sc::writeLogWarn(std::string("RAN_SERVER/FPS does not exist. It will be set to default value:10"));
        return true;
    }
}

bool ServerConfigXml::ParseAdult()
{
    try
    {
        int nAdult = 0;
        ticpp::Element* pElem = (ticpp::Element*) m_pXml->FirstChild("RAN_SERVER")->FirstChild("ADULT");
        pElem->GetTextOrDefault<int, int> (&nAdult, 0);
        if (nAdult == 1)
        {
            sc::writeLogInfo(std::string("loaded config: ADULT true"));
            m_bAdultCheck = true;
        }
        else
        {
            sc::writeLogInfo(std::string("loaded config: ADULT false"));
            m_bAdultCheck = false;
        }        
        return true;
    }
    catch (ticpp::Exception& ex)
    {
        // Report the error
        WriteErrorLog(ex);
        m_bAdultCheck = false;
        sc::writeLogWarn(std::string("RAN_SERVER/ADULT does not exist. It will be set to default value:false"));
        return true;
    }
}

bool ServerConfigXml::ParsePk()
{
    try
    {
        int nPk = 0;
        ticpp::Element* pElem = (ticpp::Element*) m_pXml->FirstChild("RAN_SERVER")->FirstChild("PK");
        pElem->GetTextOrDefault<int, int> (&nPk, 1);
        if (nPk == 1)
        {
            sc::writeLogInfo(std::string("loaded config: PK true"));
            m_bPK = true;
        }
        else
        {
            sc::writeLogInfo(std::string("loaded config: PK false"));
            m_bPK = false;
        }        
        return true;
    }
    catch (ticpp::Exception& ex)
    {
        // Report the error
        WriteErrorLog(ex);
        m_bPK = true;
        sc::writeLogWarn(std::string("RAN_SERVER/PK does not exist. It will be set to default value:true"));
        return true;
    }
}

bool ServerConfigXml::ParsePkLess()
{
    try
    {
        int nPkLess = 0;
        ticpp::Element* pElem = (ticpp::Element*) m_pXml->FirstChild("RAN_SERVER")->FirstChild("PKLESS");
        pElem->GetTextOrDefault<int, int> (&nPkLess, 0);
        if (nPkLess == 0)
        {
            sc::writeLogInfo(std::string("loaded config: PKLESS false"));
            m_bPkLess = false;
        }
        else
        {
            sc::writeLogInfo(std::string("loaded config: PKLESS true"));
            m_bPkLess = true;
        }        
        return true;
    }
    catch (ticpp::Exception& ex)
    {
        // Report the error
        WriteErrorLog(ex);
        m_bPkLess = false;
        sc::writeLogWarn(std::string("RAN_SERVER/PKLESS does not exist. It will be set to default value:false"));
        return true;
    }
}

bool ServerConfigXml::ParseConnectionPerIpAddress()
{
    try
    {
        int nPkLess = 0;
        ticpp::Element* pElem = (ticpp::Element*) m_pXml->FirstChild("RAN_SERVER")->FirstChild("CONNECTION_PER_IP_ADDRESS");
        pElem->GetTextOrDefault<unsigned int, unsigned int> (&m_ConnectionPerIpAddress, ServerConfig::DEFAULT_CONNECTION_PER_IP_ADDRESS);
        sc::writeLogInfo(
            sc::string::format(
                "loaded config: CONNECTION_PER_IP_ADDRESS %1%", m_ConnectionPerIpAddress));
        return true;
    }
    catch (ticpp::Exception& ex)
    {
        // Report the error
        WriteWarningLog(ex);
        sc::writeLogWarn(
            sc::string::format(
                "RAN_SERVER/CONNECTION_PER_IP_ADDRESS does not exist. It will be set to default value %1%",
                m_ConnectionPerIpAddress));
        return true;
    }
}

bool ServerConfigXml::ParseUseCustomOdbcPool()
{
    try
    {
        int nUse = 0;
        ticpp::Element* pElem = (ticpp::Element*) m_pXml->FirstChild("RAN_SERVER")->FirstChild("USE_CUSTOM_DB_POOL");
        pElem->GetTextOrDefault<int, int> (&nUse, 0);
        if (nUse == 1)
        {
            sc::writeLogInfo(std::string("loaded config: USE_CUSTOM_DB_POOL 1"));
            m_UseCustomOdbcPool = true;
        }
        else
        {
            sc::writeLogInfo(std::string("loaded config: USE_CUSTOM_DB_POOL 0"));
            m_UseCustomOdbcPool = false;
        }        
        return true;
    }
    catch (ticpp::Exception& ex)
    {
        // Report the error
        WriteWarningLog(ex);
        m_UseCustomOdbcPool = false;
        sc::writeLogWarn(std::string("RAN_SERVER/USE_CUSTOM_DB_POOL does not exist. It will be set to default value:false"));
        return true;
    }
}

bool ServerConfigXml::ParseReferenceCountMax()
{
	try
	{        
		ticpp::Element* pElem = ( ticpp::Element* ) m_pXml->FirstChild( "RAN_SERVER" )->FirstChild( "REFERENCE_COUNT_MAX" );
		pElem->GetTextOrDefault< int, int > ( &m_nReferenceCountMax, ServerConfig::DEFAULT_REFERENCE_COUNT_MAX );
		sc::writeLogInfo(
			sc::string::format(
			"loaded config: REFERENCE_COUNT_MAX %1%", m_nReferenceCountMax ) );
		return true;
	}
	catch ( ticpp::Exception& ex )
	{
		// Report the error
		WriteErrorLog( ex );
		m_nReferenceCountMax = ServerConfig::DEFAULT_REFERENCE_COUNT_MAX;
		sc::writeLogWarn(
			sc::string::format(
			"RAN_SERVER/REFERENCE_COUNT_MAX does not exist. It will be set to default value:%1%", m_nReferenceCountMax ) );
		return true;
	}
}

bool ServerConfigXml::ParseAdoMaxPoolSize()
{
	try
	{        
		ticpp::Element* pElem = ( ticpp::Element* ) m_pXml->FirstChild( "RAN_SERVER" )->FirstChild( "MAX_POOL_SIZE" );
		pElem->GetTextOrDefault< int, int > ( &m_nAdoMaxPoolSize, ServerConfig::DEFAULT_ADO_MAX_POOL_SIZE );
		sc::writeLogInfo(
			sc::string::format(
			"loaded config: MAX_POOL_SIZE %1%", m_nAdoMaxPoolSize ) );
		return true;
	}
	catch ( ticpp::Exception& ex )
	{
		// Report the error
		WriteErrorLog( ex );
		m_nAdoMaxPoolSize = ServerConfig::DEFAULT_ADO_MAX_POOL_SIZE;
		sc::writeLogWarn(
			sc::string::format(
			"RAN_SERVER/MAX_POOL_SIZE does not exist. It will be set to default value:%1%", m_nAdoMaxPoolSize ) );
		return true;
	}
}

bool ServerConfigXml::ParseDatabaseTcpPacketSize()
{
	try
	{        
		ticpp::Element* pElem = ( ticpp::Element* ) m_pXml->FirstChild( "RAN_SERVER" )->FirstChild( "DB_PACKET_SIZE" );
		pElem->GetTextOrDefault< int, int > ( &m_nDatabaseTcpPacketSize, ServerConfig::DEFAULT_DB_PACKET_SIZE );
		sc::writeLogInfo(
			sc::string::format(
			"loaded config: DB_PACKET_SIZE %1%", m_nDatabaseTcpPacketSize ) );
		return true;
	}
	catch ( ticpp::Exception& ex )
	{
		// Report the error
		WriteErrorLog( ex );
		m_nDatabaseTcpPacketSize = ServerConfig::DEFAULT_DB_PACKET_SIZE;
		sc::writeLogWarn(
			sc::string::format(
			"RAN_SERVER/DB_PACKET_SIZE does not exist. It will be set to default value:%1%", m_nDatabaseTcpPacketSize ) );
		return true;
	}
}

void ServerConfigXml::SetVersion( int nLauncherVersion, int nPatchVersion, int nPatchCutVersion )
{
	try
	{
		if ( nLauncherVersion < 0 || nPatchVersion < 0 || nPatchCutVersion < 0 )
		{
			sc::writeLogError( "version setting error." );
		}
		else
		{
			m_LauncherVersion = nLauncherVersion;
			m_PatchVersion = nPatchVersion;
			m_PatchCutVersion = nPatchCutVersion;

			ticpp::Element* pElem = NULL;
			pElem = ( ticpp::Element* ) m_pXml->FirstChild( "RAN_SERVER" )->FirstChild( "PATCH_VERSION" );
			pElem->SetText< int >( m_LauncherVersion );
			sc::writeLogInfo( sc::string::format( "Launcher Version set to %1%", m_LauncherVersion ) );

			pElem = NULL;
			pElem = ( ticpp::Element* ) m_pXml->FirstChild( "RAN_SERVER" )->FirstChild( "SERVER_VERSION" );
			pElem->SetText< int >( m_PatchVersion );
			sc::writeLogInfo( sc::string::format( "Patch Version set to %1%", m_PatchVersion ) );

			pElem = NULL;
			pElem = ( ticpp::Element* ) m_pXml->FirstChild( "RAN_SERVER" )->FirstChild( "PATCH_CUT_VERSION" );
			pElem->SetText< int >( m_PatchCutVersion );
			sc::writeLogInfo( sc::string::format( "Patch cut Version set to %1%", m_PatchCutVersion ) );

			Save();	
		}
	}
	catch ( ticpp::Exception& ex )
	{
		WriteErrorLog( ex );
	}
}

void ServerConfigXml::Save()
{
	sc::file::autoRename( m_XmlFileName );
	m_pXml->SaveFile( m_XmlFileName );
}