#ifndef _CACHE_SERVER_CONFIG_XML_H_
#define _CACHE_SERVER_CONFIG_XML_H_

#pragma once

#include "./ServerConfigXml.h"

//! class SessionServerConfigXml
//! 2011-04-11 Jgkim
//! Copyright(c) MIN COMMUNICATIONS, INC.
class CacheServerConfigXml : public ServerConfigXml
{
public:
    CacheServerConfigXml();
    virtual ~CacheServerConfigXml();

private:
    //! Log database
    ServerConfig::ADO_DB_INFO m_LogDb;
    
    //! Game database
    ServerConfig::ADO_DB_INFO m_GameDb;

    //! Shop database
    ServerConfig::ADO_DB_INFO m_ShopDb;

	// 란모바일
	ServerConfig::ADO_DB_INFO m_RanMobileAdo;	

    //! Session Server
    ServerConfig::SERVER_IP_PORT m_SessionServer;

	// 서버그룹 개수와 상관없이 포인트샵의 아이템 리스트는 같다. 이 값을 true 로 설정하면 포인트샵 관련 테이블 및 저장프로시저를 게임디비에서 찾는다.
	// 서버그룹별로 포인트샵을 각자 관리한다.
	bool m_bUseStandAlonePointShop;

protected:
    virtual bool Parse() override;

	bool ParseStandAlonePointShop();

public:
    const std::string GetSessionServerIP() const { return m_SessionServer.Address; }
    int GetSessionServerPort() const { return m_SessionServer.Port; }

    //! Log Database
    std::string LogDbSource()       const { return m_LogDb.m_Source; }
    std::string LogDbUser()         const { return m_LogDb.m_User; }
    std::string LogDbPassword()     const { return m_LogDb.m_Password; }
    std::string LogDbDatabaseName() const { return m_LogDb.m_Database; }
    int         LogDbTimeOut()      const { return m_LogDb.m_ResponseTime; }

    //! Game Database
    std::string GameDbSource()       const { return m_GameDb.m_Source; }
    std::string GameDbUser()         const { return m_GameDb.m_User; }
    std::string GameDbPassword()     const { return m_GameDb.m_Password; }
    std::string GameDbDatabaseName() const { return m_GameDb.m_Database; }
    int         GameDbTimeOut()      const { return m_GameDb.m_ResponseTime; }

    //! Game Database
    std::string ShopDbSource()       const { return m_ShopDb.m_Source; }
    std::string ShopDbUser()         const { return m_ShopDb.m_User; }
    std::string ShopDbPassword()     const { return m_ShopDb.m_Password; }
    std::string ShopDbDatabaseName() const { return m_ShopDb.m_Database; }
    int         ShopDbTimeOut()      const { return m_ShopDb.m_ResponseTime; }

	// 란모바일
	std::string RanMobileDbSource()       const { return m_RanMobileAdo.m_Source; }
	std::string RanMobileDbUser()         const { return m_RanMobileAdo.m_User; }
	std::string RanMobileDbPassword()     const { return m_RanMobileAdo.m_Password; }
	std::string RanMobileDbDatabaseName() const { return m_RanMobileAdo.m_Database; }
	int         RanMobileDbTimeOut()      const { return m_RanMobileAdo.m_ResponseTime; }

	bool IsStandAlonePointShop() const { return m_bUseStandAlonePointShop; }

};

#endif // _CACHE_SERVER_CONFIG_XML_H_