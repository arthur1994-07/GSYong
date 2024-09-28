#ifndef _SERVER_CFG_H_
#define _SERVER_CFG_H_

#pragma once

#include <boost/tr1/unordered_map.hpp>
#include "../../SigmaCore/Database/DbDefine.h"
#include "../../RanLogic/s_NetGlobal.h"


//! class ServerCfg
//!
//! 2002-05-30 jgkim Create
//! 2011-04-04 jgkim rename class ServerCfg->ServerCfg
//!
//! Copyright (c) Mincoms. All rights reserved.
class ServerCfg
{
protected:
    typedef std::tr1::unordered_map<std::string, std::string> MAP_ALLOW_IP;
    typedef MAP_ALLOW_IP::iterator                         MAP_ALLOW_IP_ITER;
    typedef MAP_ALLOW_IP::value_type                       MAP_ALLOW_IP_VALUE;
    typedef MAP_ALLOW_IP::const_iterator                   MAP_ALLOW_IP_CITER;

    struct DB_INFO
    {
        std::string DbServerName; //! database server name
        std::string DbUserName; //! ���Ӱ����� ����� id
        std::string DbUserPassword; //! ��й�ȣ
        std::string DbDatabase; //! database ��
        int PoolSize; //! ���ῡ ����� pool �� size
        int	ResponseTime; //! ���� ���ð� (sec)

        DB_INFO()
            : PoolSize(3)
            , ResponseTime(15)
        {
        }
    };

public:
	ServerCfg();
	ServerCfg(const char* filename);
	virtual ~ServerCfg();
//	static ServerCfg* SelfInstance;

public:
//	static ServerCfg* GetInstance();
//	static void	ReleaseInstance();

protected:
	///////////////////////////////////////////////////////////////////////////
	// Server Setting Values
	int		m_nServerVersion;						// ���� ����, Ŭ���̾�Ʈ ������ ����
	int		m_nPatchVersion;						// ��ġ ���α׷� ����
	
	//char	m_szServerName[sc::db::DB_SVR_NAME_LENGTH+1];		// ������	
	char	m_szServerIP[MAX_IP_LENGTH+1];				// ���� IP, ���α׷����� �ڵ����� �����ɼ� �ִ�
	char	m_szAgentIP [MAX_IP_LENGTH+1];				// Agent ���� IP
	
	int		m_nPortService;							// ���񽺿� ��Ʈ
	int		m_nPortControl;							// ��Ʈ�ѿ� ��Ʈ
	
	int		m_nServerType;							// ���� ����
	int		m_nServerGroup;							// ���� �׷�
	int		m_nServerNumber;						// ���� ��ȣ
	int		m_nServerField;							// ���� �ʵ� ��ȣ
	int     m_nServerChannel;                       // ���� ä�� ��ȣ
	int     m_nServerChannelNumber;                 // Agent ������ ���� ä�� ����

	int     m_nMaxFieldNumber; // �� ä���� ������ �ʵ弭�� ����.

	int		m_nServiceProvider;						// ���� ���� ȸ������
	
	int		m_nServerMaxClient;						// �ִ� ���� ���� Ŭ���̾�Ʈ ��
	int     m_nMaxChannelUser;                      // ä�δ� �ִ� ���� ���� Ŭ���̾�Ʈ ��
	bool	m_bUseEventThread;						// �̺�Ʈ ������ ��� ����
	bool    m_bHeartBeat;							// HeartBeat �� ����ؼ� Ŭ���̾�Ʈ ���Ӳ����� ����
	int		m_nMaxThread;							// �ִ� Work Thread ����.
	bool    m_bTestServer;							// �׽�Ʈ ���� ��/��
	bool    m_bAdultCheck;							// ����üũ	
	bool	m_bPK;									// true:PK ����, false:Non PK ����
	bool    m_bPkLess;                              /// ����Ŭ��, PK, �п��� PK �� ��� �������� Disable
	bool    m_bGameGuardAuth;                       // nProtect GameGuard ���� ��뿩��
	int		m_nLangSet;								// Language Set
	BYTE    m_ExcessExpProcessType;					// �ʰ� ����ġ ó�� Ÿ�� 0 : ������ 1 : �ذ��θ� 2 : ���
	bool	m_bPKServer;							//	PK �̺�Ʈ ���� ( ��� ���� ���� ) 
    bool    m_bNewServer;

	char	m_szWhiteRockServerName[sc::db::DB_SVR_NAME_LENGTH+1];		// �Ϻ� WhiteRock ������
	int		m_nWhiteRockServerPort;								// �Ϻ� WhiteRock ������� Port
  
	MAP_ALLOW_IP m_AllowIPType1;
	//MAP_ALLOW_IP m_AllowIPType2;
	//MAP_ALLOW_IP m_AllowIPType3;
	//bool m_bAllowFileLoad;

	//! ODBC
    DB_INFO m_UserDb;  //! User database setting value	
    DB_INFO m_GameDb;  //! Game database setting value
    DB_INFO m_LogDb;   //! Log database setting value 
    DB_INFO m_BoardDb; //! Board database setting value
    DB_INFO m_ShopDb;  //! Shop database setting value	
    DB_INFO m_Terra;   //! Terra database setting value
    DB_INFO m_Gsp;     //! GSP database setting value
    DB_INFO m_Kor;     //! Korea database setting value	
    DB_INFO m_Mal;     //! Malaysia database setting value
	
	///////////////////////////////////////////////////////////////////////////
	int		m_nGAMEServerNumber;
	int		m_nLOGINServerNumber;
	int		m_nFTPServerNumber;
	int		m_nSESSIONServerNumber;

	// G_SERVER_INFO m_sGAMEServer[100];
	//G_SERVER_INFO m_sLOGINServer[20];
	// G_SERVER_INFO m_sFTPServer[20];
	G_SERVER_INFO m_sSESSIONServer[20];
	F_SERVER_INFO m_sFIELDServer[MAX_CHANNEL_NUMBER][FIELDSERVER_MAX];	// Field Server Information
	CHANNEL_INFO  m_sChannelInfo[MAX_CHANNEL_NUMBER];

public:
	// long				ConvertStrToAddr(const char* szAddr);	
	int	Load(const char* filename);

private:
    void SetDefault();
    int Process(char* szLine);

public:
	F_SERVER_INFO* GetFieldServer(int nServerID, int nServerChannel=0);
	F_SERVER_INFO* GetFieldServerArray();
    bool IsPkChannel(int nChannel) const;

    bool IsAllowIp(const std::string& strAllowIP) const;
   
    char* GetSessionServerIP();	
    int	GetSessionServerPort() const;

    void SetServerVersion(int nVer) { m_nServerVersion = nVer; }
    void SetPatchVersion(int nVer) { m_nPatchVersion = nVer; }
    
    int GetServerVersion() const { return m_nServerVersion; }
    int GetPatchVersion() const { return m_nPatchVersion; }

    int GetServicePort() const { return m_nPortService; }
    int GetControlPort() const { return m_nPortControl; }

    int GetServerGroup() const { return m_nServerGroup; }
    int GetServerNumber() const { return m_nServerNumber; }
    int GetServerField() const { return m_nServerField; }
    int GetServerType() const { return m_nServerType; }
    int GetServerMaxClient() const { return m_nServerMaxClient; }

    int GetServerChannel() const { return m_nServerChannel; }
    int GetServerChannelNumber() const { return m_nServerChannelNumber; }
    int GetServerChannelMaxClient() const { return m_nMaxChannelUser; }

    BYTE GetExcessExpProcessType() const { return m_ExcessExpProcessType; }

    int	GetServiceProvider() const { return m_nServiceProvider; }
    int	GetMaxThread() const { return m_nMaxThread; }

    //const char* GetServerName() { return m_szServerName; }
    //const char* GetServerIP() const { return m_szServerIP; }
    const char* GetAgentIP() const { return m_szAgentIP; }

    //! �űԼ����ΰ� �űԼ����϶� client ���� new �� ǥ�õȴ�.
    bool GetNewServer() const { return m_bNewServer; }
	
    int GetMaxFieldNumber() const  { return m_nMaxFieldNumber; };

	int	GetMaxClient() const { return GetServerMaxClient(); }

    bool HeartBeatCheck() const { return m_bHeartBeat; }
    bool GemeGuardAuth() const { return m_bGameGuardAuth; }
    bool IsTestServer() const { return m_bTestServer; }
    bool AdultCheck() const { return m_bAdultCheck; }
    bool IsPKServer() const { return m_bPK; }	//	�̰� PK ���ɿ���
    bool IsFreePKServer() const { return m_bPKServer; }	//	�̰� ��� ���� ���� ��� 
    
    BOOL IsUserEventThread() const { return m_bUseEventThread; }
    int	 GetLangSet() const { return m_nLangSet; }
	
    //bool IsAllowFileLoad() const { return m_bAllowFileLoad; }
    //void SetAllowFileLoad(bool bAllowFileLoad) { m_bAllowFileLoad = bAllowFileLoad; }

    //-- �Ϻ� WhiteRock System ���� ����-------------------------------------//
    const char* GetWhiteRockName() const { return m_szWhiteRockServerName; }
    int	GetWhiteRockPort() const { return m_nWhiteRockServerPort; }

    //! ����Ŭ��, PK, �п��� PK �� ��� �������� Disable
    bool IsPKLessServer() const { return m_bPkLess; }
    
    ///////////////////////////////////////////////////////////////////////////
    // ODBC
    // User
    const char*	GetUserOdbcName()         const { return m_UserDb.DbServerName.c_str(); }
    const char*	GetUserOdbcUser()         const { return m_UserDb.DbUserName.c_str(); }
    const char*	GetUserOdbcPass()         const { return m_UserDb.DbUserPassword.c_str(); }
    const char*	GetUserOdbcDatabase()     const { return m_UserDb.DbDatabase.c_str(); }
    const int	GetUserOdbcResponseTime() const { return m_UserDb.ResponseTime; }
    const int	GetUserOdbcPoolSize()     const { return m_UserDb.PoolSize; }

    // Game
    const char*	GetGameOdbcName()         const { return m_GameDb.DbServerName.c_str(); }
    const char*	GetGameOdbcUser()         const { return m_GameDb.DbUserName.c_str(); }
    const char*	GetGameOdbcPass()         const { return m_GameDb.DbUserPassword.c_str(); }
    const char*	GetGameOdbcDatabase()     const { return m_GameDb.DbDatabase.c_str(); }
    const int	GetGameOdbcResponseTime() const { return m_GameDb.ResponseTime; }
    const int	GetGameOdbcPoolSize()     const { return m_GameDb.PoolSize; }

    // Board
    const char*	GetBoardOdbcName()         const { return m_BoardDb.DbServerName.c_str(); }
    const char*	GetBoardOdbcUser()         const { return m_BoardDb.DbUserName.c_str(); }
    const char*	GetBoardOdbcPass()         const { return m_BoardDb.DbUserPassword.c_str(); }
    const char*	GetBoardOdbcDatabase()     const { return m_BoardDb.DbDatabase.c_str(); }
    const int	GetBoardOdbcResponseTime() const { return m_BoardDb.ResponseTime; }
    const int	GetBoardOdbcPoolSize()     const { return m_BoardDb.PoolSize; }

    // Log
    const char*	GetLogOdbcName()         const	{ return m_LogDb.DbServerName.c_str(); }
    const char*	GetLogOdbcUser()         const	{ return m_LogDb.DbUserName.c_str(); }
    const char*	GetLogOdbcPass()         const	{ return m_LogDb.DbUserPassword.c_str(); }
    const char*	GetLogOdbcDatabase()     const	{ return m_LogDb.DbDatabase.c_str(); }
    const int	GetLogOdbcResponseTime() const { return m_LogDb.ResponseTime; }
    const int	GetLogOdbcPoolSize()     const	{ return m_LogDb.PoolSize; }

    // Shop
    const char*	GetShopOdbcName()         const { return m_ShopDb.DbServerName.c_str(); }
    const char*	GetShopOdbcUser()         const { return m_ShopDb.DbUserName.c_str(); }
    const char*	GetShopOdbcPass()         const { return m_ShopDb.DbUserPassword.c_str(); }
    const char*	GetShopOdbcDatabase()     const { return m_ShopDb.DbDatabase.c_str(); }
    const int	GetShopOdbcResponseTime() const { return m_ShopDb.ResponseTime; }
    const int	GetShopOdbcPoolSize()     const { return m_ShopDb.PoolSize; }

    // Terra
    const char*	GetTerraOdbcName()         const { return m_Terra.DbServerName.c_str(); }
    const char*	GetTerraOdbcUser()         const { return m_Terra.DbUserName.c_str(); }
    const char*	GetTerraOdbcPass()         const { return m_Terra.DbUserPassword.c_str(); }
    const char*	GetTerraOdbcDatabase()     const { return m_Terra.DbDatabase.c_str(); }
    const int	GetTerraOdbcResponseTime() const { return m_Terra.ResponseTime; }
    const int	GetTerraOdbcPoolSize()     const { return m_Terra.PoolSize; }

    // GSP
    const char*	GetGspOdbcName()         const	{ return m_Gsp.DbServerName.c_str(); }
    const char*	GetGspOdbcUser()         const	{ return m_Gsp.DbUserName.c_str(); }
    const char*	GetGspOdbcPass()         const	{ return m_Gsp.DbUserPassword.c_str(); }
    const char*	GetGspOdbcDatabase()     const	{ return m_Gsp.DbDatabase.c_str(); }
    const int	GetGspOdbcResponseTime() const { return m_Gsp.ResponseTime; }
    const int	GetGspOdbcPoolSize()     const	{ return m_Gsp.PoolSize; }

    // Korea
    const char*	GetKorOdbcName()         const	{ return m_Kor.DbServerName.c_str(); }
    const char*	GetKorOdbcUser()         const	{ return m_Kor.DbUserName.c_str(); }
    const char*	GetKorOdbcPass()         const	{ return m_Kor.DbUserPassword.c_str(); }
    const char*	GetKorOdbcDatabase()     const	{ return m_Kor.DbDatabase.c_str(); }
    const int	GetKorOdbcResponseTime() const { return m_Kor.ResponseTime; }
    const int	GetKorOdbcPoolSize()     const { return m_Kor.PoolSize; }

    // Malaysia
    const char*	GetMyOdbcName()         const { return m_Mal.DbServerName.c_str(); }
    const char*	GetMyOdbcUser()         const { return m_Mal.DbUserName.c_str(); }
    const char*	GetMyOdbcPass()         const { return m_Mal.DbUserPassword.c_str(); }
    const char*	GetMyOdbcDatabase()	    const { return m_Mal.DbDatabase.c_str(); }
    const int	GetMyOdbcResponseTime() const { return m_Mal.ResponseTime; }
    const int	GetMyOdbcPoolSize()     const { return m_Mal.PoolSize; }
};

#endif // _SERVER_CFG_H_
