#ifndef _SESSION_SERVER_CONFIG_XML_H_
#define _SESSION_SERVER_CONFIG_XML_H_

#pragma once

#include "./ServerConfigXml.h"

//! class SessionServerConfigXml
//! 2011-04-11 Jgkim
//! Copyright(c) MIN COMMUNICATIONS, INC.
class SessionServerConfigXml : public ServerConfigXml
{
public:
    SessionServerConfigXml();
    virtual ~SessionServerConfigXml();

private:
    //! ODBC
    ServerConfig::ODBC_DB_INFO m_UserDb;  //! User database setting value	
    ServerConfig::ODBC_DB_INFO m_LogDb;   //! Log database setting value 
    ServerConfig::ODBC_DB_INFO m_Terra;   //! Terra database setting value
    ServerConfig::ODBC_DB_INFO m_Gsp;     //! GSP database setting value

private:
	ServerConfig::ADO_DB_INFO m_UserAdo;
    ServerConfig::ADO_DB_INFO m_LogAdo;

public:
	ServerConfig::SERVER_IP_PORT m_AuthServer; // for global auth server

public:
	//! User Database
    std::string UserDbSource()       const { return m_UserAdo.m_Source; }
    std::string UserDbUser()         const { return m_UserAdo.m_User; }
    std::string UserDbPassword()     const { return m_UserAdo.m_Password; }
    std::string UserDbDatabaseName() const { return m_UserAdo.m_Database; }
    int         UserDbTimeOut()      const { return m_UserAdo.m_ResponseTime; }

	//! Log Database
    std::string LogDbSource()       const { return m_LogAdo.m_Source; }
    std::string LogDbUser()         const { return m_LogAdo.m_User; }
    std::string LogDbPassword()     const { return m_LogAdo.m_Password; }
    std::string LogDbDatabaseName() const { return m_LogAdo.m_Database; }
    int         LogDbTimeOut()      const { return m_LogAdo.m_ResponseTime; }
    
protected:
    virtual bool Parse() override;

public:
    //! ODBC	
    //! User
    const char*	GetUserOdbcName()         const { return m_UserDb.OdbcName.c_str(); }
    const char*	GetUserOdbcUser()         const { return m_UserDb.DbUserName.c_str(); }
    const char*	GetUserOdbcPass()         const { return m_UserDb.DbUserPassword.c_str(); }
    const char*	GetUserOdbcDatabase()     const { return m_UserDb.DbDatabase.c_str(); }
    const int	GetUserOdbcResponseTime() const { return m_UserDb.ResponseTime; }
    const int	GetUserOdbcPoolSize()     const { return m_UserDb.PoolSize; }

    //! Log
    const char*	GetLogOdbcName()         const { return m_LogDb.OdbcName.c_str(); }
    const char*	GetLogOdbcUser()         const { return m_LogDb.DbUserName.c_str(); }
    const char*	GetLogOdbcPass()         const { return m_LogDb.DbUserPassword.c_str(); }
    const char*	GetLogOdbcDatabase()     const { return m_LogDb.DbDatabase.c_str(); }
    const int	GetLogOdbcResponseTime() const { return m_LogDb.ResponseTime; }
    const int	GetLogOdbcPoolSize()     const { return m_LogDb.PoolSize; }

    //! Terra
    const char*	GetTerraOdbcName()         const { return m_Terra.OdbcName.c_str(); }
    const char*	GetTerraOdbcUser()         const { return m_Terra.DbUserName.c_str(); }
    const char*	GetTerraOdbcPass()         const { return m_Terra.DbUserPassword.c_str(); }
    const char*	GetTerraOdbcDatabase()     const { return m_Terra.DbDatabase.c_str(); }
    const int	GetTerraOdbcResponseTime() const { return m_Terra.ResponseTime; }
    const int	GetTerraOdbcPoolSize()     const { return m_Terra.PoolSize; }

    //! GSP
    const char*	GetGspOdbcName()         const { return m_Gsp.OdbcName.c_str(); }
    const char*	GetGspOdbcUser()         const { return m_Gsp.DbUserName.c_str(); }
    const char*	GetGspOdbcPass()         const { return m_Gsp.DbUserPassword.c_str(); }
    const char*	GetGspOdbcDatabase()     const { return m_Gsp.DbDatabase.c_str(); }
    const int	GetGspOdbcResponseTime() const { return m_Gsp.ResponseTime; }
    const int	GetGspOdbcPoolSize()     const { return m_Gsp.PoolSize; }

public:
	const char* GetAuthServerIP() const { return m_AuthServer.Address.c_str(); }
	int GetAuthServerPort() const { return m_AuthServer.Port; }
};

#endif // _SESSION_SERVER_CONFIG_XML_H_