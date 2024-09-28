#include "../pch.h"
#pragma warning(disable:4267)
#include <boost/spirit/include/classic.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/lexical_cast.hpp>
#include "../../SigmaCore/Encrypt/DbEncrypt.h"
#include "../../SigmaCore/Log/LogMan.h"
#include "../../SigmaCore/string/StringFormat.h"
#include "../../SigmaCore/Xml/2.5.3/ticpp.h"

#include "./ServerConfigHelper.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace ServerConfigHelper
{
bool ParseAuthServer(ticpp::Document* pXml, ServerConfig::SERVER_IP_PORT& Data)
{
    try
    {
        ticpp::Element* pElem = NULL;		
        pElem = (ticpp::Element*) pXml->FirstChild("RAN_SERVER")->FirstChild("AUTH_SERVER");
        pElem->GetAttribute("IP", &Data.Address);
        pElem->GetAttribute("PORT", &Data.Port);
        sc::writeLogInfo(
            sc::string::format(
                "loaded config:Auth_SERVER %1% %2%", Data.Address, Data.Port));
        return true;
    }
    catch (ticpp::Exception& ex)
    {
        sc::writeLogError(std::string(ex.what()));
        sc::writeLogError(std::string("RAN_SERVER/Auth_SERVER not exist"));        
        return false;
    }
}

bool ParseSessionServer(ticpp::Document* pXml, ServerConfig::SERVER_IP_PORT& Data)
{
    try
    {
        ticpp::Element* pElem = NULL;		
        pElem = (ticpp::Element*) pXml->FirstChild("RAN_SERVER")->FirstChild("SESSION_SERVER");
        pElem->GetAttribute("IP", &Data.Address);
        pElem->GetAttribute("PORT", &Data.Port);
        sc::writeLogInfo(
            sc::string::format(
                "loaded config: SESSION_SERVER %1% %2%", Data.Address, Data.Port));
        return true;
    }
    catch (ticpp::Exception& ex)
    {
        // Report the error
        sc::writeLogError(std::string(ex.what()));
        sc::writeLogError(std::string("RAN_SERVER/SESSION_SERVER not exist"));        
        return false;
    }
}

bool ParseCacheServer(ticpp::Document* pXml, ServerConfig::SERVER_IP_PORT& Data)
{
    try
    {
        ticpp::Element* pElem = NULL;
        pElem = (ticpp::Element*) pXml->FirstChild("RAN_SERVER")->FirstChild("CACHE_SERVER");
        pElem->GetAttribute("IP", &Data.Address);
        pElem->GetAttribute("PORT", &Data.Port);
        sc::writeLogInfo(
            sc::string::format(
                "loaded config: CACHE_SERVER %1% %2%", Data.Address, Data.Port));
        return true;
    }
    catch (ticpp::Exception& ex)
    {
        // Report the error
        sc::writeLogError(std::string(ex.what()));
        sc::writeLogError(std::string("RAN_SERVER/CACHE_SERVER not exist"));
        return false;
    }
}

bool ParseMatchServer(ticpp::Document* pXml, ServerConfig::SERVER_IP_PORT& Data)
{
    try
    {
        ticpp::Element* pElem = NULL;
        pElem = (ticpp::Element*) pXml->FirstChild("RAN_SERVER")->FirstChild("MATCH_SERVER");
        pElem->GetAttribute("IP", &Data.Address);
        pElem->GetAttribute("PORT", &Data.Port);
        sc::writeLogInfo(
            sc::string::format(
                "loaded config: MATCH_SERVER %1% %2%", Data.Address, Data.Port));
        return true;
    }
    catch (ticpp::Exception& ex)
    {
        // Report the error
        sc::writeLogError(std::string(ex.what()));
        sc::writeLogError(std::string("RAN_SERVER/CACHE_SERVER not exist"));
        return false;
    }
}

bool ParseInstanceServerNum(ticpp::Document* pXml, DWORD& nInstanceServerNum)
{
	try
    {        
        ticpp::Element* pElem = (ticpp::Element*) pXml->FirstChild("RAN_SERVER")->FirstChild("INSTANCE_SERVER_ID");
        pElem->GetTextOrDefault<DWORD, DWORD> (&nInstanceServerNum, 0);
        sc::writeLogInfo(
            sc::string::format(
            "loaded config: INSTANCE_SERVER_ID %1%", nInstanceServerNum));
        return true;
    }
    catch (ticpp::Exception& ex)
    {
        // Report the error
        sc::writeLogError(std::string(ex.what()));
        nInstanceServerNum = 0;
        sc::writeLogWarn(
            sc::string::format(
                "RAN_SERVER/INSTANCE_SERVER_ID does not exist. It will be set to default value:%1%", 0));
        return true;
    }
}


bool ParseDatabase(ticpp::Document* pXml, const std::string& ElmtName, ServerConfig::ODBC_DB_INFO& DbInfo)
{
    try
    {
        sc::DbEncrypt Encrypt;
        ticpp::Element* pElem = NULL;
        pElem = (ticpp::Element*) pXml->FirstChild("RAN_SERVER")->FirstChild(ElmtName)->FirstChild("ODBC_NAME");
        DbInfo.OdbcName = pElem->GetText(true);

        pElem = (ticpp::Element*) pXml->FirstChild("RAN_SERVER")->FirstChild(ElmtName)->FirstChild("CATALOG");
        DbInfo.DbDatabase = pElem->GetText(true);

        pElem = (ticpp::Element*) pXml->FirstChild("RAN_SERVER")->FirstChild(ElmtName)->FirstChild("ID");
        std::string Id = pElem->GetText(true);
        DbInfo.DbUserName = Encrypt.getDecrypt(Id);

        pElem = (ticpp::Element*) pXml->FirstChild("RAN_SERVER")->FirstChild(ElmtName)->FirstChild("PASSWORD");
        std::string Password = pElem->GetText(true);
        DbInfo.DbUserPassword = Encrypt.getDecrypt(Password);

        pElem = (ticpp::Element*) pXml->FirstChild("RAN_SERVER")->FirstChild(ElmtName)->FirstChild("TIMEOUT");
        std::string strTimeOut = pElem->GetTextOrDefault("15");
        DbInfo.ResponseTime = boost::lexical_cast<int> (strTimeOut);

#ifdef _DEBUG
        sc::writeLogDebug(DbInfo.OdbcName);
        sc::writeLogDebug(DbInfo.DbDatabase);
        std::string ResponseTime(boost::lexical_cast<std::string> (DbInfo.ResponseTime));
        sc::writeLogDebug(ResponseTime);
        sc::writeLogDebug(DbInfo.DbUserName);
        sc::writeLogDebug(DbInfo.DbUserPassword);
#endif
        return true;
    }
    catch (ticpp::Exception& ex)
    {
        // Report the error
        sc::writeLogError(std::string(ex.what()));
        sc::writeLogError(
            sc::string::format(
                "RAN_SERVER/%1%", ElmtName));
        return false;
    }
}

bool ParseDatabase(ticpp::Document* pXml, const std::string& ElmtName, ServerConfig::ADO_DB_INFO& DbInfo)
{
    try
    {
        sc::DbEncrypt Encrypt;
        ticpp::Element* pElem = NULL;
        pElem = (ticpp::Element*) pXml->FirstChild("RAN_SERVER")->FirstChild(ElmtName)->FirstChild("DATA_SOURCE");
        DbInfo.m_Source = pElem->GetText(true);

        pElem = (ticpp::Element*) pXml->FirstChild("RAN_SERVER")->FirstChild(ElmtName)->FirstChild("CATALOG");
        DbInfo.m_Database = pElem->GetText(true);

        pElem = (ticpp::Element*) pXml->FirstChild("RAN_SERVER")->FirstChild(ElmtName)->FirstChild("ID");
        std::string Id = pElem->GetText(true);
        DbInfo.m_User = Encrypt.getDecrypt(Id);

        pElem = (ticpp::Element*) pXml->FirstChild("RAN_SERVER")->FirstChild(ElmtName)->FirstChild("PASSWORD");
        std::string Password = pElem->GetText(true);
        DbInfo.m_Password = Encrypt.getDecrypt(Password);

        pElem = (ticpp::Element*) pXml->FirstChild("RAN_SERVER")->FirstChild(ElmtName)->FirstChild("TIMEOUT");
        std::string strTimeOut = pElem->GetTextOrDefault("15");
        DbInfo.m_ResponseTime = boost::lexical_cast<int> (strTimeOut);

#ifdef _DEBUG
        sc::writeLogDebug(DbInfo.m_Source);
        sc::writeLogDebug(DbInfo.m_Database);
        sc::writeLogDebug(DbInfo.m_User);
        sc::writeLogDebug(DbInfo.m_Password);
        sc::writeLogDebug(sc::string::format("%1%", DbInfo.m_ResponseTime));
#endif
        return true;
    }
    catch (ticpp::Exception& ex)
    {
        // Report the error
        sc::writeLogError(std::string(ex.what()));
        return false;
    }
}

bool ParseAuthDatabase(ticpp::Document* pXml, const std::string& ElmtName, ServerConfig::ADO_DB_INFO& DbInfo)
{
    try
    {
        sc::DbEncrypt Encrypt;
        ticpp::Element* pElem = NULL;
        pElem = (ticpp::Element*) pXml->FirstChild("RAN_SERVER")->FirstChild(ElmtName)->FirstChild("DATA_SOURCE");
        DbInfo.m_Source = pElem->GetText(true);

        pElem = (ticpp::Element*) pXml->FirstChild("RAN_SERVER")->FirstChild(ElmtName)->FirstChild("CATALOG");
        DbInfo.m_Database = pElem->GetText(true);

        pElem = (ticpp::Element*) pXml->FirstChild("RAN_SERVER")->FirstChild(ElmtName)->FirstChild("ID");
        DbInfo.m_User = pElem->GetText(true);

        pElem = (ticpp::Element*) pXml->FirstChild("RAN_SERVER")->FirstChild(ElmtName)->FirstChild("PASSWORD");
        DbInfo.m_Password = pElem->GetText(true);

        pElem = (ticpp::Element*) pXml->FirstChild("RAN_SERVER")->FirstChild(ElmtName)->FirstChild("TIMEOUT");
        std::string strTimeOut = pElem->GetTextOrDefault("15");
        DbInfo.m_ResponseTime = boost::lexical_cast<int> (strTimeOut);

#ifdef _DEBUG
        sc::writeLogDebug(DbInfo.m_Source);
        sc::writeLogDebug(DbInfo.m_Database);
        sc::writeLogDebug(DbInfo.m_User);
        sc::writeLogDebug(DbInfo.m_Password);
        sc::writeLogDebug(sc::string::format("%1%", DbInfo.m_ResponseTime));
#endif
        return true;
    }
    catch (ticpp::Exception& ex)
    {
        // Report the error
        sc::writeLogError(std::string(ex.what()));
        return false;
    }
}

bool ParseServerChannel(ticpp::Document* pXml, int& ChannelNumber)
{    
    try
    {        
        ticpp::Element* pElem = (ticpp::Element*) pXml->FirstChild("RAN_SERVER")->FirstChild("SERVER_CHANNEL");
        pElem->GetTextOrDefault<int, int> (&ChannelNumber, ServerConfig::DEFAULT_SERVER_CHANNEL);
        sc::writeLogInfo(
            sc::string::format(
            "loaded config: SERVER_CHANNEL %1%", ChannelNumber));
        return true;
    }
    catch (ticpp::Exception& ex)
    {
        // Report the error
        sc::writeLogError(std::string(ex.what()));
        ChannelNumber = ServerConfig::DEFAULT_SERVER_CHANNEL;
        sc::writeLogWarn(
            sc::string::format(
                "RAN_SERVER/SERVER_CHANNEL does not exist. It will be set to default value:%1%", ServerConfig::DEFAULT_SERVER_CHANNEL));
        return true;
    }
}

bool ParseGameGuard(ticpp::Document* pXml, bool& bUse)
{
    try
    {
        int nUse = 0;
        ticpp::Element* pElem = (ticpp::Element*) pXml->FirstChild("RAN_SERVER")->FirstChild("GAME_GUARD");
        pElem->GetTextOrDefault<int, int> (&nUse, 0);
        if (nUse == 1)
        {
            sc::writeLogInfo(std::string("loaded config: GAME_GUARD 1"));
            bUse = true;
        }
        else
        {
            sc::writeLogInfo(std::string("loaded config: GAME_GUARD 0"));
            bUse = false;
        }        
        return true;
    }
    catch (ticpp::Exception& ex)
    {
        // Report the error
        sc::writeLogError(std::string(ex.what()));
        bUse = false;
        sc::writeLogWarn(std::string("RAN_SERVER/GAME_GUARD does not exist. It will be set to default value:false"));
        return true;
    }
}

bool ParseIntegrationCacheServer( ticpp::Document* pXml, ServerConfig::SERVER_IP_PORT& Data )
{
	try
	{
		ticpp::Element* pElem = NULL;
		pElem = ( ticpp::Element* ) pXml->FirstChild( "RAN_SERVER" )->FirstChild( "INTEGRATION_CACHE_SERVER" );
		pElem->GetAttribute( "IP", &Data.Address );
		pElem->GetAttribute( "PORT", &Data.Port );
		sc::writeLogInfo( 
			sc::string::format( 
			"loaded config: INTEGRATION_CACHE_SERVER %1% %2%", Data.Address, Data.Port ) );
		return true;
	}
	catch ( ticpp::Exception& ex )
	{
		// Report the error
		sc::writeLogError( std::string( ex.what() ) );
		sc::writeLogError( std::string( "RAN_SERVER/INTEGRATION_CACHE_SERVER not exist" ) );
		return false;
	}
}

}