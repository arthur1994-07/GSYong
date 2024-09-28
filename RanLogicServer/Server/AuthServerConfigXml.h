#ifndef _AUTH_SERVER_CONFIG_XML_H_
#define _AUTH_SERVER_CONFIG_XML_H_

#pragma once

#include "ServerConfigXml.h"

class AuthServerConfigXml : public ServerConfigXml
{
public:
    AuthServerConfigXml();
    virtual ~AuthServerConfigXml();

private:
    //! ODBC
    //ServerConfig::ODBC_DB_INFO m_UserDb;  //! User database setting value	
    //ServerConfig::ODBC_DB_INFO m_LogDb;   //! Log database setting value 
    //ServerConfig::ODBC_DB_INFO m_Terra;   //! Terra database setting value
    //ServerConfig::ODBC_DB_INFO m_Gsp;     //! GSP database setting value

private:
	ServerConfig::ADO_DB_INFO m_AuthAdo;

public:
	////! User Database
 //   std::string UserDbSource()       const { return m_UserAdo.m_Source; }
 //   std::string UserDbUser()         const { return m_UserAdo.m_User; }
 //   std::string UserDbPassword()     const { return m_UserAdo.m_Password; }
 //   std::string UserDbDatabaseName() const { return m_UserAdo.m_Database; }
 //   int         UserDbTimeOut()      const { return m_UserAdo.m_ResponseTime; }

	//! Log Database
    std::string AuthDbSource()       const { return m_AuthAdo.m_Source; }
    std::string AuthDbUser()         const { return m_AuthAdo.m_User; }
    std::string AuthDbPassword()     const { return m_AuthAdo.m_Password; }
    std::string AuthDbDatabaseName() const { return m_AuthAdo.m_Database; }
    int         AuthDbTimeOut()      const { return m_AuthAdo.m_ResponseTime; }
    
protected:
    virtual bool Parse() override;

public:
    ////! ODBC	
    ////! User
    //const char*	GetUserOdbcName()         const { return m_UserDb.OdbcName.c_str(); }
    //const char*	GetUserOdbcUser()         const { return m_UserDb.DbUserName.c_str(); }
    //const char*	GetUserOdbcPass()         const { return m_UserDb.DbUserPassword.c_str(); }
    //const char*	GetUserOdbcDatabase()     const { return m_UserDb.DbDatabase.c_str(); }
    //const int	GetUserOdbcResponseTime() const { return m_UserDb.ResponseTime; }
    //const int	GetUserOdbcPoolSize()     const { return m_UserDb.PoolSize; }

    ////! Log
    //const char*	GetLogOdbcName()         const { return m_LogDb.OdbcName.c_str(); }
    //const char*	GetLogOdbcUser()         const { return m_LogDb.DbUserName.c_str(); }
    //const char*	GetLogOdbcPass()         const { return m_LogDb.DbUserPassword.c_str(); }
    //const char*	GetLogOdbcDatabase()     const { return m_LogDb.DbDatabase.c_str(); }
    //const int	GetLogOdbcResponseTime() const { return m_LogDb.ResponseTime; }
    //const int	GetLogOdbcPoolSize()     const { return m_LogDb.PoolSize; }

    ////! Terra
    //const char*	GetTerraOdbcName()         const { return m_Terra.OdbcName.c_str(); }
    //const char*	GetTerraOdbcUser()         const { return m_Terra.DbUserName.c_str(); }
    //const char*	GetTerraOdbcPass()         const { return m_Terra.DbUserPassword.c_str(); }
    //const char*	GetTerraOdbcDatabase()     const { return m_Terra.DbDatabase.c_str(); }
    //const int	GetTerraOdbcResponseTime() const { return m_Terra.ResponseTime; }
    //const int	GetTerraOdbcPoolSize()     const { return m_Terra.PoolSize; }

    ////! GSP
    //const char*	GetGspOdbcName()         const { return m_Gsp.OdbcName.c_str(); }
    //const char*	GetGspOdbcUser()         const { return m_Gsp.DbUserName.c_str(); }
    //const char*	GetGspOdbcPass()         const { return m_Gsp.DbUserPassword.c_str(); }
    //const char*	GetGspOdbcDatabase()     const { return m_Gsp.DbDatabase.c_str(); }
    //const int	GetGspOdbcResponseTime() const { return m_Gsp.ResponseTime; }
    //const int	GetGspOdbcPoolSize()     const { return m_Gsp.PoolSize; }
};

#endif