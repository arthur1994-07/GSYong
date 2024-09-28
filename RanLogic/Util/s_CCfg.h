///////////////////////////////////////////////////////////////////////////////
// s_CCfg.h
//
// class CCfg
//
// * History
// 2002.05.30 jgkim Create
//
// Copyright 2002-2003 (c) Mincoms. All rights reserved.                 
// 
// * Note
// Config file load class head file
//
///////////////////////////////////////////////////////////////////////////////

#ifndef S_CCFG_H_
#define S_CCFG_H_

#pragma once

#include "../s_NetGlobal.h"
#include "../../SigmaCore/Database/DbDefine.h"

class CCfg
{
public:
	CCfg();
	CCfg(const TCHAR* filename);
	virtual ~CCfg();
	//static CCfg* SelfInstance;

public:
	//static CCfg* GetInstance();
	//static void	ReleaseInstance();

protected:
	///////////////////////////////////////////////////////////////////////////
	// Server Setting Values
	int		m_nServerVersion;						// ���� ����, Ŭ���̾�Ʈ ������ ����
	int		m_nPatchVersion;						// ��ġ ���α׷� ����
	
	TCHAR	m_szServerName[sc::db::DB_SVR_NAME_LENGTH+1];		// ������	
	TCHAR	m_szServerIP[MAX_IP_LENGTH+1];				// ���� IP, ���α׷����� �ڵ����� �����ɼ� �ִ�
	TCHAR	m_szAgentIP [MAX_IP_LENGTH+1];				// Agent ���� IP
	
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
	bool	m_bUseIntelTBB;							// TBB ��� ����
	bool	m_bPK;									// true:PK ����, false:Non PK ����
	bool    m_bPkLess;                              /// ����Ŭ��, PK, �п��� PK �� ��� �������� Disable
	bool    m_bGameGuardAuth;                       // nProtect GameGuard ���� ��뿩��
	int		m_nLangSet;								// Language Set
	BYTE    m_ExcessExpProcessType;					// �ʰ� ����ġ ó�� Ÿ�� 0 : ������ 1 : �ذ��θ� 2 : ���
	bool	m_bPKServer;							//	PK �̺�Ʈ ���� ( ��� ���� ���� ) 
    bool    m_bNewServer;

	TCHAR	m_szWhiteRockServerName[sc::db::DB_SVR_NAME_LENGTH+1];		// �Ϻ� WhiteRock ������
	int		m_nWhiteRockServerPort;								// �Ϻ� WhiteRock ������� Port

	std::vector<CString> m_vecAllowIPType1;
	std::vector<CString> m_vecAllowIPType2;
	std::vector<CString> m_vecAllowIPType3;
	bool m_bAllowFileLoad;

	///////////////////////////////////////////////////////////////////////////
	// database setting value 	

	///////////////////////////////////////////////////////////////////////////
	// DB-Lib C
	// User database setting value 
	TCHAR	m_szUserDBServer  [sc::db::DB_SVR_NAME_LENGTH+1]; // database server name
	TCHAR	m_szUserDBUser    [USR_ID_LENGTH+1];	  // ���Ӱ����� ����� id
	TCHAR	m_szUserDBPass    [USR_PASS_LENGTH+1];	  // ��й�ȣ
	TCHAR	m_szUserDBDatabase[sc::db::DB_NAME_LENGTH+1];	  // database ��
	int		m_nUserDBPoolSize;						  // ���ῡ ����� pool �� size
	int		m_nUserDBResponseTime;					  // ���� ���ð� (sec)

	// Game database setting value 
	TCHAR	m_szGameDBServer  [sc::db::DB_SVR_NAME_LENGTH+1];	// database server name
	TCHAR	m_szGameDBUser    [USR_ID_LENGTH+1];		// ���Ӱ����� ����� id
	TCHAR	m_szGameDBPass    [USR_PASS_LENGTH+1];	    // ��й�ȣ
	TCHAR	m_szGameDBDatabase[sc::db::DB_NAME_LENGTH+1];		// database ��
	int		m_nGameDBPoolSize;						    // ���ῡ ����� pool �� size
	int		m_nGameDBResponseTime;					    // ���� ���ð� (sec)

	// Log database setting value 
	TCHAR	m_szLogDBServer  [sc::db::DB_SVR_NAME_LENGTH+1]; // database server name
	TCHAR	m_szLogDBUser    [USR_ID_LENGTH+1];		 // ���Ӱ����� ����� id
	TCHAR	m_szLogDBPass    [USR_PASS_LENGTH+1];	 // ��й�ȣ
	TCHAR	m_szLogDBDatabase[sc::db::DB_NAME_LENGTH+1];	 // database ��
	int		m_nLogDBPoolSize;						 // ���ῡ ����� pool �� size
	int		m_nLogDBResponseTime;					 // ���� ���ð� (sec)	
	
	///////////////////////////////////////////////////////////////////////////
	// ODBC
	// User database setting value
	TCHAR	m_szUserOdbcName    [sc::db::DB_SVR_NAME_LENGTH+1]; // Database server name
	TCHAR	m_szUserOdbcUser    [USR_ID_LENGTH+1];	    // User account
	TCHAR	m_szUserOdbcPass    [USR_PASS_LENGTH+1];    // Password
	TCHAR	m_szUserOdbcDatabase[sc::db::DB_NAME_LENGTH+1];	    // Database name
	int		m_nUserOdbcPoolSize;					    // Database pool size
	int		m_nUserOdbcResponseTime;				    // Query response time (sec)

	// Game database setting value 
	TCHAR	m_szGameOdbcName    [sc::db::DB_SVR_NAME_LENGTH+1]; // Database server name
	TCHAR	m_szGameOdbcUser    [USR_ID_LENGTH+1];	    // User account
	TCHAR	m_szGameOdbcPass    [USR_PASS_LENGTH+1];	// Password
	TCHAR	m_szGameOdbcDatabase[sc::db::DB_NAME_LENGTH+1];	    // Database name
	int		m_nGameOdbcPoolSize;					    // Database pool size
	int		m_nGameOdbcResponseTime;				    // Query response time (sec)

	// Log database setting value 
	TCHAR	m_szLogOdbcName    [sc::db::DB_SVR_NAME_LENGTH+1]; // Database server name
	TCHAR	m_szLogOdbcUser    [USR_ID_LENGTH+1];	   // User account
	TCHAR	m_szLogOdbcPass    [USR_PASS_LENGTH+1];	   // Password
	TCHAR	m_szLogOdbcDatabase[sc::db::DB_NAME_LENGTH+1];	   // Database name
	int		m_nLogOdbcPoolSize;					       // Database pool size
	int		m_nLogOdbcResponseTime;				       // Query response time (sec)

	// Board database setting value 
	TCHAR	m_szBoardOdbcName    [sc::db::DB_SVR_NAME_LENGTH+1]; // Database server name
	TCHAR	m_szBoardOdbcUser    [USR_ID_LENGTH+1];	     // User account
	TCHAR	m_szBoardOdbcPass    [USR_PASS_LENGTH+1];	 // Password
	TCHAR	m_szBoardOdbcDatabase[sc::db::DB_NAME_LENGTH+1];	 // Database name
	int		m_nBoardOdbcPoolSize;					     // Database pool size
	int		m_nBoardOdbcResponseTime;				     // Query response time (sec)

	// Shop database setting value 
	TCHAR	m_szShopOdbcName    [sc::db::DB_SVR_NAME_LENGTH+1]; // Database server name
	TCHAR	m_szShopOdbcUser    [USR_ID_LENGTH+1];	    // User account
	TCHAR	m_szShopOdbcPass    [USR_PASS_LENGTH+1];	// Password
	TCHAR	m_szShopOdbcDatabase[sc::db::DB_NAME_LENGTH+1];	    // Database name
	int		m_nShopOdbcPoolSize;					    // Database pool size
	int		m_nShopOdbcResponseTime;				    // Query response time (sec)

	// Terra database setting value 
	TCHAR	m_szTerraOdbcName    [sc::db::DB_SVR_NAME_LENGTH+1]; // Database server name
	TCHAR	m_szTerraOdbcUser    [USR_ID_LENGTH+1];	     // User account
	TCHAR	m_szTerraOdbcPass    [USR_PASS_LENGTH+1];	 // Password
	TCHAR	m_szTerraOdbcDatabase[sc::db::DB_NAME_LENGTH+1];	 // Database name
	int		m_nTerraOdbcPoolSize;					     // Database pool size
	int		m_nTerraOdbcResponseTime;				     // Query response time (sec)

	// GSP database setting value 
	TCHAR m_szGspOdbcName    [sc::db::DB_SVR_NAME_LENGTH+1]; // Database server name
	TCHAR m_szGspOdbcUser    [USR_ID_LENGTH+1];	     // User account
	TCHAR m_szGspOdbcPass    [USR_PASS_LENGTH+1];	 // Password
	TCHAR m_szGspOdbcDatabase[sc::db::DB_NAME_LENGTH+1];	 // Database name
	int	  m_nGspOdbcPoolSize;					     // Database pool size
	int	  m_nGspOdbcResponseTime;				     // Query response time (sec)

	// Korea database setting value 
	TCHAR m_szKorOdbcName    [sc::db::DB_SVR_NAME_LENGTH+1]; // Database server name
	TCHAR m_szKorOdbcUser    [USR_ID_LENGTH+1];	     // User account
	TCHAR m_szKorOdbcPass    [USR_PASS_LENGTH+1];	 // Password
	TCHAR m_szKorOdbcDatabase[sc::db::DB_NAME_LENGTH+1];	 // Database name
	int	  m_nKorOdbcPoolSize;					     // Database pool size
	int	  m_nKorOdbcResponseTime;				     // Query response time (sec)

	// Malaysia database setting value
	TCHAR m_szMyOdbcName    [sc::db::DB_SVR_NAME_LENGTH+1];	 // Database server name
	TCHAR m_szMyOdbcUser    [USR_ID_LENGTH+1];	     // User account
	TCHAR m_szMyOdbcPass    [USR_PASS_LENGTH+1];	 // Password
	TCHAR m_szMyOdbcDatabase[sc::db::DB_NAME_LENGTH+1];		 // Database name
	int	  m_nMyOdbcPoolSize;					     // Database pool size
	int	  m_nMyOdbcResponseTime;				     // Query response time (sec)

	///////////////////////////////////////////////////////////////////////////
	int		m_nGAMEServerNumber;
	int		m_nLOGINServerNumber;
	int		m_nFTPServerNumber;
	int		m_nSESSIONServerNumber;

	// G_SERVER_INFO m_sGAMEServer[100];
	G_SERVER_INFO m_sLOGINServer[20];
	// G_SERVER_INFO m_sFTPServer[20];
	G_SERVER_INFO m_sSESSIONServer[20];
	F_SERVER_INFO m_sFIELDServer[MAX_CHANNEL_NUMBER][FIELDSERVER_MAX];	// Field Server Information
	CHANNEL_INFO  m_sChannelInfo[MAX_CHANNEL_NUMBER];

public:
	// long				ConvertStrToAddr(const TCHAR* szAddr);
	void				SetDefault(void);
	int					Process(TCHAR* szLine);
	int					Load(const TCHAR* filename);

	F_SERVER_INFO*		GetFieldServer(int nServerID, int nServerChannel=0);
	F_SERVER_INFO*		GetFieldServerArray()		{	return &m_sFIELDServer[0][0]; }

	// G_SERVER_INFO*		GetFTPServer(void);
	
	TCHAR*				GetSessionServerIP(void);
	int					GetSessionServerPort(void);
	
	int					GetServerGroup(void);	
	int					GetServerNumber(void);
	int					GetServerField(void);
	int					GetServerType(void);
    
	int                 GetServerChannel(void);
	int                 GetServerChannelNumber(void);
	int                 GetServerChannelMaxClient(void);

    bool                GetNewServer(void);

	BYTE				GetExcessExpProcessType(void);
	
	int					GetServerMaxClient(void);	
	int					GetServiceProvider(void);
	
	int					GetServerVersion(void);
	int					GetPatchVersion(void);

	void				SetServerVersion(int nVer);
	void				SetPatchVersion(int nVer);

	int GetMaxFieldNumber() { return m_nMaxFieldNumber; };
	
	const TCHAR*		GetServerName(void);
	const TCHAR*		GetServerIP(void);
	int					GetServicePort();
	TCHAR*				GetAgentIP(void);
	int					GetControlPort(void);
	int					GetMaxClient(void)	{ return GetServerMaxClient(); }
	int					GetMaxThread(void);
	bool				HeartBeatCheck(void);
	bool                GemeGuardAuth(void);
	bool				AdultCheck(void);
	BOOL				IsUseIntelTBB(void);
	BOOL				IsUserEventThread(void);
	bool				IsTestServer(void);
	bool				IsPKServer(void);          // �ش� ������ PK �������� �����Ѵ�. Field
	bool                IsPkChannel(int nChannel); // �ش�ä���� PK ä������ �����Ѵ�. Agent
	//! ����Ŭ��, PK, �п��� PK �� ��� �������� Disable
	bool				IsPKLessServer(void);
	bool				IsFreePKServer(void);		//	����PK���� ( �̺�Ʈ ���� )

	//-- �Ϻ� WhiteRock System ���� ����-------------------------------------//
	TCHAR*				GetWhiteRockName(void);
	int					GetWhiteRockPort(void);
	int					GetLangSet(void);

	//-- �������˽� �α��μ����� ������ �� �ִ� IP���
	int					GetAllowIPType1Size( void );
	int					GetAllowIPType2Size( void );
	int					GetAllowIPType3Size( void );
	void				GetAllowIPType1( int nCount, CString& strAllowIP );
	void				GetAllowIPType2( int nCount, CString& strAllowIP );
	void				GetAllowIPType3( int nCount, CString& strAllowIP );
	bool				GetAllowFileLoad( void );
	void				SetAllowFileLoad( bool bAllowFileLoad );

	
	///////////////////////////////////////////////////////////////////////////
	// DB-Lib C	
	int					GetUserDBPoolSize(void);
	TCHAR*				GetUserDBServer(void);
	TCHAR*				GetUserDBUser(void);
	TCHAR*				GetUserDBPass(void);
	TCHAR*				GetUserDBDatabase(void);
	int					GetUserDBResponseTime(void);
	
	int					GetGameDBPoolSize(void);
	TCHAR*				GetGameDBServer(void);
	TCHAR*				GetGameDBUser(void);
	TCHAR*				GetGameDBPass(void);
	TCHAR*				GetGameDBDatabase(void);
	int					GetGameDBResponseTime(void);	

	///////////////////////////////////////////////////////////////////////////
	// ODBC	
	TCHAR*				GetUserOdbcName(void);
	TCHAR*				GetUserOdbcUser(void);
	TCHAR*				GetUserOdbcPass(void);
	TCHAR*				GetUserOdbcDatabase(void);
	int					GetUserOdbcResponseTime(void);
	int					GetUserOdbcPoolSize(void);	
	
	TCHAR*				GetGameOdbcName(void);
	TCHAR*				GetGameOdbcUser(void);
	TCHAR*				GetGameOdbcPass(void);
	TCHAR*				GetGameOdbcDatabase(void);
	int					GetGameOdbcResponseTime(void);
	int					GetGameOdbcPoolSize(void);

	TCHAR*				GetLogOdbcName(void);
	TCHAR*				GetLogOdbcUser(void);
	TCHAR*				GetLogOdbcPass(void);
	TCHAR*				GetLogOdbcDatabase(void);
	int					GetLogOdbcResponseTime(void);
	int					GetLogOdbcPoolSize(void);

	TCHAR*				GetBoardOdbcName(void);
	TCHAR*				GetBoardOdbcUser(void);
	TCHAR*				GetBoardOdbcPass(void);
	TCHAR*				GetBoardOdbcDatabase(void);
	int					GetBoardOdbcResponseTime(void);
	int					GetBoardOdbcPoolSize(void);

	TCHAR*				GetShopOdbcName(void);
	TCHAR*				GetShopOdbcUser(void);
	TCHAR*				GetShopOdbcPass(void);
	TCHAR*				GetShopOdbcDatabase(void);
	int					GetShopOdbcResponseTime(void);
	int					GetShopOdbcPoolSize(void);

	TCHAR*				GetTerraOdbcName(void);
	TCHAR*				GetTerraOdbcUser(void);
	TCHAR*				GetTerraOdbcPass(void);
	TCHAR*				GetTerraOdbcDatabase(void);
	int					GetTerraOdbcResponseTime(void);
	int					GetTerraOdbcPoolSize(void);

	TCHAR*				GetGspOdbcName(void);
	TCHAR*				GetGspOdbcUser(void);
	TCHAR*				GetGspOdbcPass(void);
	TCHAR*				GetGspOdbcDatabase(void);
	int					GetGspOdbcResponseTime(void);
	int					GetGspOdbcPoolSize(void);

	TCHAR*				GetKorOdbcName(void);
	TCHAR*				GetKorOdbcUser(void);
	TCHAR*				GetKorOdbcPass(void);
	TCHAR*				GetKorOdbcDatabase(void);
	int					GetKorOdbcResponseTime(void);
	int					GetKorOdbcPoolSize(void);

	// �����̽þ� PC�� �̺�Ʈ
	TCHAR*				GetMyOdbcName(void);
	TCHAR*				GetMyOdbcUser(void);
	TCHAR*				GetMyOdbcPass(void);
	TCHAR*				GetMyOdbcDatabase(void);
	int					GetMyOdbcResponseTime(void);
	int					GetMyOdbcPoolSize(void);
};

#endif // S_CCFG_H_
