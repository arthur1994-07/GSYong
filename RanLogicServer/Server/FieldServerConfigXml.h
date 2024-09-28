#ifndef _FIELD_SERVER_CONFIG_XML_H_
#define _FIELD_SERVER_CONFIG_XML_H_

#pragma once

#include "./ServerConfigXml.h"

class FieldServerConfigXml : public ServerConfigXml
{
public:
    FieldServerConfigXml();
    virtual ~FieldServerConfigXml();

private:
    ServerConfig::SERVER_IP_PORT m_SessionServer;
    ServerConfig::SERVER_IP_PORT m_AgentServer;
    ServerConfig::SERVER_IP_PORT m_CacheServer;
	ServerConfig::SERVER_IP_PORT m_IntegrationCacheServer;
    BYTE m_ExcessExpProcessType; //! 초과 경험치 처리 타입 0 : 사용안함 1 : 극강부만 2 : 모두
    int m_nServerField; //! 서버 필드 번호
    bool m_bGameGuardAuth; //! nProtect GameGuard 인증 사용여부
    bool m_bPKServer; //! PK 이벤트 서버(모든 유저 적대)

    int m_nServerChannel; //! 서버 채널 번호	
    
    //! ODBC
    ServerConfig::ODBC_DB_INFO m_UserDb;  //! User database setting value	
    ServerConfig::ODBC_DB_INFO m_GameDb;  //! Game database setting value
    ServerConfig::ODBC_DB_INFO m_LogDb;   //! Log database setting value
    ServerConfig::ODBC_DB_INFO m_ShopDb;  //! Shop database setting value	
    ServerConfig::ODBC_DB_INFO m_Terra;   //! Terra database setting value
    ServerConfig::ODBC_DB_INFO m_Gsp;     //! GSP database setting value
    ServerConfig::ODBC_DB_INFO m_Kor;     //! Korea database setting value	
    ServerConfig::ODBC_DB_INFO m_Mal;     //! Malaysia database setting value
	ServerConfig::ODBC_DB_INFO m_WorldBattleDb;  //! database setting value	
	ServerConfig::ODBC_DB_INFO m_RanMobileDb;  //! 란모바일

private:
	ServerConfig::ADO_DB_INFO m_UserAdo;
	ServerConfig::ADO_DB_INFO m_GameAdo;
    ServerConfig::ADO_DB_INFO m_LogAdo;    
    ServerConfig::ADO_DB_INFO m_ShopAdo;	
	ServerConfig::ADO_DB_INFO m_TexasHoldemIFAdo;
	ServerConfig::ADO_DB_INFO m_WorldBattleAdo;	
	ServerConfig::ADO_DB_INFO m_RanMobileAdo;	

private:
	int bPARTY_2OTHERSCHOOL;
	int bCLUB_2OTHERSCHOOL;
	float fITEM_DROP_SCALE;
	float fMONEY_DROP_SCALE;
	float fITEM_DROP_LOWER_SCALE;
	DWORD dwCRITICAL_MAX;
	float fNONPK_ITEM_DROP;
	float fPK_ITEM_DROP;
	bool bUseRanMobile;

	int ServerRestartDayOfWeek;
	int ServerRestartHour;
	int ServerRestartWorkingsetSize;	// 단위 mb

	int ForceLogItemRandomOption;	// 강제로 랜덤옵션을 남길 것인가? 0 남기지 않는다.(기본값) 1 남긴다.

public:
	//! User Database
    std::string UserDbSource()       const { return m_UserAdo.m_Source; }
    std::string UserDbUser()         const { return m_UserAdo.m_User; }
    std::string UserDbPassword()     const { return m_UserAdo.m_Password; }
    std::string UserDbDatabaseName() const { return m_UserAdo.m_Database; }
    int         UserDbTimeOut()      const { return m_UserAdo.m_ResponseTime; }

    //! Game Database
    std::string GameDbSource()       const { return m_GameAdo.m_Source; }
    std::string GameDbUser()         const { return m_GameAdo.m_User; }
    std::string GameDbPassword()     const { return m_GameAdo.m_Password; }
    std::string GameDbDatabaseName() const { return m_GameAdo.m_Database; }
    int         GameDbTimeOut()      const { return m_GameAdo.m_ResponseTime; }

	//! Log Database
    std::string LogDbSource()       const { return m_LogAdo.m_Source; }
    std::string LogDbUser()         const { return m_LogAdo.m_User; }
    std::string LogDbPassword()     const { return m_LogAdo.m_Password; }
    std::string LogDbDatabaseName() const { return m_LogAdo.m_Database; }
    int         LogDbTimeOut()      const { return m_LogAdo.m_ResponseTime; }

    //! Shop Database
    std::string ShopDbSource()       const { return m_ShopAdo.m_Source; }
    std::string ShopDbUser()         const { return m_ShopAdo.m_User; }
    std::string ShopDbPassword()     const { return m_ShopAdo.m_Password; }
    std::string ShopDbDatabaseName() const { return m_ShopAdo.m_Database; }
    int         ShopDbTimeOut()      const { return m_ShopAdo.m_ResponseTime; }

	//! TexasHoldemIF Database
	std::string TexasHoldemIFDbSource()       const { return m_TexasHoldemIFAdo.m_Source; }
	std::string TexasHoldemIFDbUser()         const { return m_TexasHoldemIFAdo.m_User; }
	std::string TexasHoldemIFDbPassword()     const { return m_TexasHoldemIFAdo.m_Password; }
	std::string TexasHoldemIFDbDatabaseName() const { return m_TexasHoldemIFAdo.m_Database; }
	int         TexasHoldemIFDbTimeOut()      const { return m_TexasHoldemIFAdo.m_ResponseTime; }

	//! WorldBattle Database
	std::string WorldBattleDbSource()       const { return m_WorldBattleAdo.m_Source; }
	std::string WorldBattleDbUser()         const { return m_WorldBattleAdo.m_User; }
	std::string WorldBattleDbPassword()     const { return m_WorldBattleAdo.m_Password; }
	std::string WorldBattleDbDatabaseName() const { return m_WorldBattleAdo.m_Database; }
	int         WorldBattleDbTimeOut()      const { return m_WorldBattleAdo.m_ResponseTime; }

	// 란모바일
	std::string RanMobileDbSource()       const { return m_RanMobileAdo.m_Source; }
	std::string RanMobileDbUser()         const { return m_RanMobileAdo.m_User; }
	std::string RanMobileDbPassword()     const { return m_RanMobileAdo.m_Password; }
	std::string RanMobileDbDatabaseName() const { return m_RanMobileAdo.m_Database; }
	int         RanMobileDbTimeOut()      const { return m_RanMobileAdo.m_ResponseTime; }

protected:
    virtual bool Parse() override;    
    bool ParseAgentServer();
    bool ParseExcessXpProcessType();
    bool ParseServerField();
    bool ParsePkServer();

	bool ParsePARTY_2OTHERSCHOOL();
	bool ParseCLUB_2OTHERSCHOOL();
	bool ParseITEM_DROP_SCALE();
	bool ParseMONEY_DROP_SCALE();
	bool ParseITEM_DROP_LOWER_SCALE();
	bool ParseCRITICAL_MAX();
	bool ParseNONPK_ITEM_DROP();
	bool ParsePK_ITEM_DROP();
	bool PaseUseRanMobile();

	bool ParseServerRestartDayOfWeek();
	bool ParseServerRestartHour();
	bool ParseServerRestartWorkingsetSize();

	bool ParseForceLogItemRandomOption();

public:
    int GetServerChannel() const { return m_nServerChannel; }
    const char* GetAgentIP() const { return m_AgentServer.Address.c_str(); }
    
    const char* GetSessionServerIP() const { return m_SessionServer.Address.c_str(); }
    int GetSessionServerPort() const { return m_SessionServer.Port; }

    const char* GetCacheServerIP() const { return m_CacheServer.Address.c_str(); }
    int GetCacheServerPort() const { return m_CacheServer.Port; }

	const char* GetIntegrationCacheServerIP() const { return m_IntegrationCacheServer.Address.c_str(); }
	int GetIntegrationCacheServerPort() const { return m_IntegrationCacheServer.Port; }

    BYTE GetExcessExpProcessType() const { return m_ExcessExpProcessType; }
    int GetServerField() const { return m_nServerField; }
    bool GemeGuardAuth() const { return m_bGameGuardAuth; }
    bool IsFreePKServer() const { return m_bPKServer; }	//	이건 모든 유저 적대 모드

    ///////////////////////////////////////////////////////////////////////////
    // ODBC	
    // User
    const char*	GetUserOdbcName()         const { return m_UserDb.OdbcName.c_str(); }
    const char*	GetUserOdbcUser()         const { return m_UserDb.DbUserName.c_str(); }
    const char*	GetUserOdbcPass()         const { return m_UserDb.DbUserPassword.c_str(); }
    const char*	GetUserOdbcDatabase()     const { return m_UserDb.DbDatabase.c_str(); }
    const int	GetUserOdbcResponseTime() const { return m_UserDb.ResponseTime; }
    const int	GetUserOdbcPoolSize()     const { return m_UserDb.PoolSize; }

    // Game
    const char*	GetGameOdbcName()         const { return m_GameDb.OdbcName.c_str(); }
    const char*	GetGameOdbcUser()         const { return m_GameDb.DbUserName.c_str(); }
    const char*	GetGameOdbcPass()         const { return m_GameDb.DbUserPassword.c_str(); }
    const char*	GetGameOdbcDatabase()     const { return m_GameDb.DbDatabase.c_str(); }
    const int	GetGameOdbcResponseTime() const { return m_GameDb.ResponseTime; }
    const int	GetGameOdbcPoolSize()     const { return m_GameDb.PoolSize; }

    // Log
    const char*	GetLogOdbcName()         const	{ return m_LogDb.OdbcName.c_str(); }
    const char*	GetLogOdbcUser()         const	{ return m_LogDb.DbUserName.c_str(); }
    const char*	GetLogOdbcPass()         const	{ return m_LogDb.DbUserPassword.c_str(); }
    const char*	GetLogOdbcDatabase()     const	{ return m_LogDb.DbDatabase.c_str(); }
    const int	GetLogOdbcResponseTime() const { return m_LogDb.ResponseTime; }
    const int	GetLogOdbcPoolSize()     const	{ return m_LogDb.PoolSize; }

    // Shop
    const char*	GetShopOdbcName()         const { return m_ShopDb.OdbcName.c_str(); }
    const char*	GetShopOdbcUser()         const { return m_ShopDb.DbUserName.c_str(); }
    const char*	GetShopOdbcPass()         const { return m_ShopDb.DbUserPassword.c_str(); }
    const char*	GetShopOdbcDatabase()     const { return m_ShopDb.DbDatabase.c_str(); }
    const int	GetShopOdbcResponseTime() const { return m_ShopDb.ResponseTime; }
    const int	GetShopOdbcPoolSize()     const { return m_ShopDb.PoolSize; }

    // Terra
    const char*	GetTerraOdbcName()         const { return m_Terra.OdbcName.c_str(); }
    const char*	GetTerraOdbcUser()         const { return m_Terra.DbUserName.c_str(); }
    const char*	GetTerraOdbcPass()         const { return m_Terra.DbUserPassword.c_str(); }
    const char*	GetTerraOdbcDatabase()     const { return m_Terra.DbDatabase.c_str(); }
    const int	GetTerraOdbcResponseTime() const { return m_Terra.ResponseTime; }
    const int	GetTerraOdbcPoolSize()     const { return m_Terra.PoolSize; }

    // GSP
    const char*	GetGspOdbcName()         const	{ return m_Gsp.OdbcName.c_str(); }
    const char*	GetGspOdbcUser()         const	{ return m_Gsp.DbUserName.c_str(); }
    const char*	GetGspOdbcPass()         const	{ return m_Gsp.DbUserPassword.c_str(); }
    const char*	GetGspOdbcDatabase()     const	{ return m_Gsp.DbDatabase.c_str(); }
    const int	GetGspOdbcResponseTime() const { return m_Gsp.ResponseTime; }
    const int	GetGspOdbcPoolSize()     const	{ return m_Gsp.PoolSize; }

    // Korea
    const char*	GetKorOdbcName()         const	{ return m_Kor.OdbcName.c_str(); }
    const char*	GetKorOdbcUser()         const	{ return m_Kor.DbUserName.c_str(); }
    const char*	GetKorOdbcPass()         const	{ return m_Kor.DbUserPassword.c_str(); }
    const char*	GetKorOdbcDatabase()     const	{ return m_Kor.DbDatabase.c_str(); }
    const int	GetKorOdbcResponseTime() const { return m_Kor.ResponseTime; }
    const int	GetKorOdbcPoolSize()     const { return m_Kor.PoolSize; }

    // Malaysia
    const char*	GetMyOdbcName()         const { return m_Mal.OdbcName.c_str(); }
    const char*	GetMyOdbcUser()         const { return m_Mal.DbUserName.c_str(); }
    const char*	GetMyOdbcPass()         const { return m_Mal.DbUserPassword.c_str(); }
    const char*	GetMyOdbcDatabase()	    const { return m_Mal.DbDatabase.c_str(); }
    const int	GetMyOdbcResponseTime() const { return m_Mal.ResponseTime; }
    const int	GetMyOdbcPoolSize()     const { return m_Mal.PoolSize; }

	// WorldBattle
	const char*	GetWorldBattleOdbcName()         const { return m_WorldBattleDb.OdbcName.c_str(); }
	const char*	GetWorldBattleOdbcUser()         const { return m_WorldBattleDb.DbUserName.c_str(); }
	const char*	GetWorldBattleOdbcPass()         const { return m_WorldBattleDb.DbUserPassword.c_str(); }
	const char*	GetWorldBattleOdbcDatabase()     const { return m_WorldBattleDb.DbDatabase.c_str(); }
	const int	GetWorldBattleOdbcResponseTime() const { return m_WorldBattleDb.ResponseTime; }
	const int	GetWorldBattleOdbcPoolSize()     const { return m_WorldBattleDb.PoolSize; }

	// 란모바일
	const char*	GetRanMobileOdbcName()         const { return m_RanMobileDb.OdbcName.c_str(); }
	const char*	GetRanMobileOdbcUser()         const { return m_RanMobileDb.DbUserName.c_str(); }
	const char*	GetRanMobileOdbcPass()         const { return m_RanMobileDb.DbUserPassword.c_str(); }
	const char*	GetRanMobileOdbcDatabase()     const { return m_RanMobileDb.DbDatabase.c_str(); }
	const int	GetRanMobileOdbcResponseTime() const { return m_RanMobileDb.ResponseTime; }
	const int	GetRanMobileOdbcPoolSize()     const { return m_RanMobileDb.PoolSize; }

	int GetPARTY_2OTHERSCHOOL() { return bPARTY_2OTHERSCHOOL; }
	int GetCLUB_2OTHERSCHOOL() { return bCLUB_2OTHERSCHOOL; }
	float GetITEM_DROP_SCALE() { return fITEM_DROP_SCALE; }
	float GetMONEY_DROP_SCALE() { return fMONEY_DROP_SCALE; }
	float GetITEM_DROP_LOWER_SCALE() { return fITEM_DROP_LOWER_SCALE; }
	DWORD GetCRITICAL_MAX() { return dwCRITICAL_MAX; }
	float GetNONPK_ITEM_DROP() { return fNONPK_ITEM_DROP; }
	float GetPK_ITEM_DROP() { return fPK_ITEM_DROP; }
	bool GetUseRanMobile() { return bUseRanMobile; }

	int GetServerRestartDayOfWeek() { return ServerRestartDayOfWeek; }
	int GetServerRestartHour() { return ServerRestartHour; }
	int GetServerRestartWorkingsetSize() { return ServerRestartWorkingsetSize; }

	int GetForceLogItemRandomOption() { return ForceLogItemRandomOption; }
};

#endif