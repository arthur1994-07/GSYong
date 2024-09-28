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

	// �������
	ServerConfig::ADO_DB_INFO m_RanMobileAdo;	

    //! Session Server
    ServerConfig::SERVER_IP_PORT m_SessionServer;

	// �����׷� ������ ������� ����Ʈ���� ������ ����Ʈ�� ����. �� ���� true �� �����ϸ� ����Ʈ�� ���� ���̺� �� �������ν����� ���ӵ�񿡼� ã�´�.
	// �����׷캰�� ����Ʈ���� ���� �����Ѵ�.
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

	// �������
	std::string RanMobileDbSource()       const { return m_RanMobileAdo.m_Source; }
	std::string RanMobileDbUser()         const { return m_RanMobileAdo.m_User; }
	std::string RanMobileDbPassword()     const { return m_RanMobileAdo.m_Password; }
	std::string RanMobileDbDatabaseName() const { return m_RanMobileAdo.m_Database; }
	int         RanMobileDbTimeOut()      const { return m_RanMobileAdo.m_ResponseTime; }

	bool IsStandAlonePointShop() const { return m_bUseStandAlonePointShop; }

};

#endif // _CACHE_SERVER_CONFIG_XML_H_