#ifndef _SERVER_CONFIG_HELPER_H_
#define _SERVER_CONFIG_HELPER_H_

#pragma once

#include "./ServerConfigDefine.h"

namespace ticpp
{
    class Document;
}

//! 2011-04-08 Jgkim
//! Server Config 파일 도우미
namespace ServerConfigHelper
{
	bool ParseAuthServer(ticpp::Document* pXml, ServerConfig::SERVER_IP_PORT& Data); 
    bool ParseSessionServer(ticpp::Document* pXml, ServerConfig::SERVER_IP_PORT& Data);
    bool ParseCacheServer(ticpp::Document* pXml, ServerConfig::SERVER_IP_PORT& Data);
	bool ParseMatchServer(ticpp::Document* pXml, ServerConfig::SERVER_IP_PORT& Data);
	bool ParseInstanceServerNum(ticpp::Document* pXml, DWORD& nInstanceServerNum);
    bool ParseDatabase(ticpp::Document* pXml, const std::string& ElmtName, ServerConfig::ODBC_DB_INFO& DbInfo);
    bool ParseAuthDatabase(ticpp::Document* pXml, const std::string& ElmtName, ServerConfig::ADO_DB_INFO& DbInfo);
	bool ParseDatabase(ticpp::Document* pXml, const std::string& ElmtName, ServerConfig::ADO_DB_INFO& DbInfo);
    bool ParseServerChannel(ticpp::Document* pXml, int& ChannelNumber);
    bool ParseGameGuard(ticpp::Document* pXml, bool& bUse);
	bool ParseIntegrationCacheServer( ticpp::Document* pXml, ServerConfig::SERVER_IP_PORT& Data );
}

#endif // _SERVER_CONFIG_HELPER_H_