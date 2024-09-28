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
	int		m_nServerVersion;						// 서버 버전, 클라이언트 버전과 동일
	int		m_nPatchVersion;						// 패치 프로그램 버전
	
	TCHAR	m_szServerName[sc::db::DB_SVR_NAME_LENGTH+1];		// 서버명	
	TCHAR	m_szServerIP[MAX_IP_LENGTH+1];				// 서버 IP, 프로그램에서 자동으로 결정될수 있다
	TCHAR	m_szAgentIP [MAX_IP_LENGTH+1];				// Agent 서버 IP
	
	int		m_nPortService;							// 서비스용 포트
	int		m_nPortControl;							// 컨트롤용 포트
	
	int		m_nServerType;							// 서버 유형
	int		m_nServerGroup;							// 서버 그룹
	int		m_nServerNumber;						// 서버 번호
	int		m_nServerField;							// 서버 필드 번호
	int     m_nServerChannel;                       // 서버 채널 번호
	int     m_nServerChannelNumber;                 // Agent 서버가 가진 채널 갯수

	int     m_nMaxFieldNumber; // 한 채널이 가지는 필드서버 갯수.

	int		m_nServiceProvider;						// 서비스 제공 회사유형
	
	int		m_nServerMaxClient;						// 최대 접속 가능 클라이언트 수
	int     m_nMaxChannelUser;                      // 채널당 최대 접속 가능 클라이언트 수
	bool	m_bUseEventThread;						// 이벤트 쓰레드 사용 여부
	bool    m_bHeartBeat;							// HeartBeat 를 사용해서 클라이언트 접속끊김을 조사
	int		m_nMaxThread;							// 최대 Work Thread 갯수.
	bool    m_bTestServer;							// 테스트 서버 유/무
	bool    m_bAdultCheck;							// 성인체크
	bool	m_bUseIntelTBB;							// TBB 사용 여부
	bool	m_bPK;									// true:PK 서버, false:Non PK 서버
	bool    m_bPkLess;                              /// 선도클럽, PK, 학원간 PK 등 모든 적대행위 Disable
	bool    m_bGameGuardAuth;                       // nProtect GameGuard 인증 사용여부
	int		m_nLangSet;								// Language Set
	BYTE    m_ExcessExpProcessType;					// 초과 경험치 처리 타입 0 : 사용안함 1 : 극강부만 2 : 모두
	bool	m_bPKServer;							//	PK 이벤트 서버 ( 모든 유저 적대 ) 
    bool    m_bNewServer;

	TCHAR	m_szWhiteRockServerName[sc::db::DB_SVR_NAME_LENGTH+1];		// 일본 WhiteRock 서버명
	int		m_nWhiteRockServerPort;								// 일본 WhiteRock 서버사용 Port

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
	TCHAR	m_szUserDBUser    [USR_ID_LENGTH+1];	  // 접속가능한 사용자 id
	TCHAR	m_szUserDBPass    [USR_PASS_LENGTH+1];	  // 비밀번호
	TCHAR	m_szUserDBDatabase[sc::db::DB_NAME_LENGTH+1];	  // database 명
	int		m_nUserDBPoolSize;						  // 연결에 사용할 pool 의 size
	int		m_nUserDBResponseTime;					  // 쿼리 대기시간 (sec)

	// Game database setting value 
	TCHAR	m_szGameDBServer  [sc::db::DB_SVR_NAME_LENGTH+1];	// database server name
	TCHAR	m_szGameDBUser    [USR_ID_LENGTH+1];		// 접속가능한 사용자 id
	TCHAR	m_szGameDBPass    [USR_PASS_LENGTH+1];	    // 비밀번호
	TCHAR	m_szGameDBDatabase[sc::db::DB_NAME_LENGTH+1];		// database 명
	int		m_nGameDBPoolSize;						    // 연결에 사용할 pool 의 size
	int		m_nGameDBResponseTime;					    // 쿼리 대기시간 (sec)

	// Log database setting value 
	TCHAR	m_szLogDBServer  [sc::db::DB_SVR_NAME_LENGTH+1]; // database server name
	TCHAR	m_szLogDBUser    [USR_ID_LENGTH+1];		 // 접속가능한 사용자 id
	TCHAR	m_szLogDBPass    [USR_PASS_LENGTH+1];	 // 비밀번호
	TCHAR	m_szLogDBDatabase[sc::db::DB_NAME_LENGTH+1];	 // database 명
	int		m_nLogDBPoolSize;						 // 연결에 사용할 pool 의 size
	int		m_nLogDBResponseTime;					 // 쿼리 대기시간 (sec)	
	
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
	bool				IsPKServer(void);          // 해당 서버가 PK 서버인지 조사한다. Field
	bool                IsPkChannel(int nChannel); // 해당채널이 PK 채널인지 조사한다. Agent
	//! 선도클럽, PK, 학원간 PK 등 모든 적대행위 Disable
	bool				IsPKLessServer(void);
	bool				IsFreePKServer(void);		//	프리PK서버 ( 이벤트 서버 )

	//-- 일본 WhiteRock System 설정 변수-------------------------------------//
	TCHAR*				GetWhiteRockName(void);
	int					GetWhiteRockPort(void);
	int					GetLangSet(void);

	//-- 서버점검시 로그인서버에 접속할 수 있는 IP목록
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

	// 말레이시아 PC방 이벤트
	TCHAR*				GetMyOdbcName(void);
	TCHAR*				GetMyOdbcUser(void);
	TCHAR*				GetMyOdbcPass(void);
	TCHAR*				GetMyOdbcDatabase(void);
	int					GetMyOdbcResponseTime(void);
	int					GetMyOdbcPoolSize(void);
};

#endif // S_CCFG_H_
