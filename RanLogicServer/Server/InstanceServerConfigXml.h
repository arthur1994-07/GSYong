#ifndef _FIELD_SERVER_CONFIG_XML_H_
#define _FIELD_SERVER_CONFIG_XML_H_

#pragma once

#include "./ServerConfigXml.h"

class InstanceServerConfigXml : public ServerConfigXml
{
public:
    InstanceServerConfigXml();
    virtual ~InstanceServerConfigXml();

private:
    ServerConfig::SERVER_IP_PORT m_SessionServer;    
    ServerConfig::SERVER_IP_PORT m_CacheServer;
	ServerConfig::SERVER_IP_PORT m_MatchServer;

    BYTE m_ExcessExpProcessType; //초과 경험치 처리 타입 0 : 사용안함 1 : 극강부만 2 : 모두
    
    bool m_bGameGuardAuth; //! nProtect GameGuard 인증 사용여부
    bool m_bPKServer; //! PK 이벤트 서버(모든 유저 적대)

	DWORD	m_dwInstanceServerNum;	//InstanceServerID
    
    //! ODBC    
    ServerConfig::ODBC_DB_INFO m_GameDb[MAX_SERVER_GROUP];  //! Game database setting value
    ServerConfig::ODBC_DB_INFO m_LogDb;						//! Log database setting value

private:	
	ServerConfig::ADO_DB_INFO m_GameAdo[MAX_SERVER_GROUP];
    ServerConfig::ADO_DB_INFO m_LogAdo;

public:
    // Game ADO
    std::string GameDbSource(DWORD dwSvrGroupNum)		const { return m_GameAdo[dwSvrGroupNum].m_Source; }
    std::string GameDbUser(DWORD dwSvrGroupNum)			const { return m_GameAdo[dwSvrGroupNum].m_User; }
    std::string GameDbPassword(DWORD dwSvrGroupNum)		const { return m_GameAdo[dwSvrGroupNum].m_Password; }
    std::string GameDbDatabaseName(DWORD dwSvrGroupNum)	const { return m_GameAdo[dwSvrGroupNum].m_Database; }
    int         GameDbTimeOut(DWORD dwSvrGroupNum)		const { return m_GameAdo[dwSvrGroupNum].m_ResponseTime; }

	// Log ADO
    std::string LogDbSource()       const { return m_LogAdo.m_Source; }
    std::string LogDbUser()         const { return m_LogAdo.m_User; }
    std::string LogDbPassword()     const { return m_LogAdo.m_Password; }
    std::string LogDbDatabaseName() const { return m_LogAdo.m_Database; }
    int         LogDbTimeOut()      const { return m_LogAdo.m_ResponseTime; }

protected:
    virtual bool Parse() override;    
    bool ParseMatchServer();
    bool ParseExcessXpProcessType();    
    bool ParsePkServer();
	bool ParseDatabaseGroup();

public:
    const char* GetSessionServerIP() const { return m_SessionServer.Address.c_str(); }
    int GetSessionServerPort() const { return m_SessionServer.Port; }

    const char* GetCacheServerIP() const { return m_CacheServer.Address.c_str(); }
    int GetCacheServerPort() const { return m_CacheServer.Port; }

	const char* GetMatchServerIP() const { return m_MatchServer.Address.c_str(); }
    int GetMatchServerPort() const { return m_MatchServer.Port; }

	DWORD GetInstanceServerNum() const { return m_dwInstanceServerNum; }

    BYTE GetExcessExpProcessType() const { return m_ExcessExpProcessType; }
    
    bool GemeGuardAuth() const { return m_bGameGuardAuth; }
    bool IsFreePKServer() const { return m_bPKServer; }	//	이건 모든 유저 적대 모드

    	
    // ODBC

    // Game
    const char*	GetGameOdbcName(DWORD dwSvrGroupNum)		const { return m_GameDb[dwSvrGroupNum].OdbcName.c_str(); }
    const char*	GetGameOdbcUser(DWORD dwSvrGroupNum)		const { return m_GameDb[dwSvrGroupNum].DbUserName.c_str(); }
    const char*	GetGameOdbcPass(DWORD dwSvrGroupNum)		const { return m_GameDb[dwSvrGroupNum].DbUserPassword.c_str(); }
    const char*	GetGameOdbcDatabase(DWORD dwSvrGroupNum)	const { return m_GameDb[dwSvrGroupNum].DbDatabase.c_str(); }
    const int	GetGameOdbcResponseTime(DWORD dwSvrGroupNum)const { return m_GameDb[dwSvrGroupNum].ResponseTime; }
    const int	GetGameOdbcPoolSize(DWORD dwSvrGroupNum)	const { return m_GameDb[dwSvrGroupNum].PoolSize; }

    // Log
    const char*	GetLogOdbcName()         const	{ return m_LogDb.OdbcName.c_str(); }
    const char*	GetLogOdbcUser()         const	{ return m_LogDb.DbUserName.c_str(); }
    const char*	GetLogOdbcPass()         const	{ return m_LogDb.DbUserPassword.c_str(); }
    const char*	GetLogOdbcDatabase()     const	{ return m_LogDb.DbDatabase.c_str(); }
    const int	GetLogOdbcResponseTime() const { return m_LogDb.ResponseTime; }
    const int	GetLogOdbcPoolSize()     const	{ return m_LogDb.PoolSize; }
};

#endif