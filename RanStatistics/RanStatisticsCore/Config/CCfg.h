#pragma once

#define	CONFIG_FILENAME	"db.cfg"


class CCfg
{
private:
	CCfg();
	virtual ~CCfg();
	static CCfg* SelfInstance;

public:
	static CCfg* GetInstance();
	static void	ReleaseInstance();

protected:	
	// User database setting value 
	TCHAR	m_szUserDBServer  [DB_SVR_NAME_LENGTH+1]; // database server name
	TCHAR	m_szUserDBUser    [USR_ID_LENGTH+1];	  // 접속가능한 사용자 id
	TCHAR	m_szUserDBPass    [USR_PASS_LENGTH+1];	  // 비밀번호
	TCHAR	m_szUserDBDatabase[DB_NAME_LENGTH+1];	  // database 명
	int		m_nUserDBPoolSize;						  // 연결에 사용할 pool 의 size
	int		m_nUserDBResponseTime;					  // 쿼리 대기시간 (sec)

	// Game database setting value 
	TCHAR	m_szGameDBServer  [DB_SVR_NAME_LENGTH+1];	// database server name
	TCHAR	m_szGameDBUser    [USR_ID_LENGTH+1];		// 접속가능한 사용자 id
	TCHAR	m_szGameDBPass    [USR_PASS_LENGTH+1];	    // 비밀번호
	TCHAR	m_szGameDBDatabase[DB_NAME_LENGTH+1];		// database 명
	int		m_nGameDBPoolSize;						    // 연결에 사용할 pool 의 size
	int		m_nGameDBResponseTime;					    // 쿼리 대기시간 (sec)

	// Log database setting value 
	TCHAR	m_szLogDBServer  [DB_SVR_NAME_LENGTH+1]; // database server name
	TCHAR	m_szLogDBUser    [USR_ID_LENGTH+1];		 // 접속가능한 사용자 id
	TCHAR	m_szLogDBPass    [USR_PASS_LENGTH+1];	 // 비밀번호
	TCHAR	m_szLogDBDatabase[DB_NAME_LENGTH+1];	 // database 명
	int		m_nLogDBPoolSize;						 // 연결에 사용할 pool 의 size
	int		m_nLogDBResponseTime;					 // 쿼리 대기시간 (sec)	

	///////////////////////////////////////////////////////////////////////////
	// ODBC
	// User database setting value
	TCHAR	m_szUserOdbcName    [DB_SVR_NAME_LENGTH+1]; // Database server name
	TCHAR	m_szUserOdbcUser    [USR_ID_LENGTH+1];	    // User account
	TCHAR	m_szUserOdbcPass    [USR_PASS_LENGTH+1];    // Password
	TCHAR	m_szUserOdbcDatabase[DB_NAME_LENGTH+1];	    // Database name
	int		m_nUserOdbcPoolSize;					    // Database pool size
	int		m_nUserOdbcResponseTime;				    // Query response time (sec)

	// Game database setting value 
	TCHAR	m_szGameOdbcName    [DB_SVR_NAME_LENGTH+1]; // Database server name
	TCHAR	m_szGameOdbcUser    [USR_ID_LENGTH+1];	    // User account
	TCHAR	m_szGameOdbcPass    [USR_PASS_LENGTH+1];	// Password
	TCHAR	m_szGameOdbcDatabase[DB_NAME_LENGTH+1];	    // Database name
	int		m_nGameOdbcPoolSize;					    // Database pool size
	int		m_nGameOdbcResponseTime;				    // Query response time (sec)

	// Web database setting value 
	TCHAR	m_szWebOdbcName    [DB_SVR_NAME_LENGTH+1]; // Database server name
	TCHAR	m_szWebOdbcUser    [USR_ID_LENGTH+1];	    // User account
	TCHAR	m_szWebOdbcPass    [USR_PASS_LENGTH+1];		// Password
	TCHAR	m_szWebOdbcDatabase[DB_NAME_LENGTH+1];	    // Database name
	int		m_nWebOdbcPoolSize;							// Database pool size
	int		m_nWebOdbcResponseTime;						// Query response time (sec)

	// Log database setting value 
	TCHAR	m_szLogOdbcName    [DB_SVR_NAME_LENGTH+1]; // Database server name
	TCHAR	m_szLogOdbcUser    [USR_ID_LENGTH+1];	   // User account
	TCHAR	m_szLogOdbcPass    [USR_PASS_LENGTH+1];	   // Password
	TCHAR	m_szLogOdbcDatabase[DB_NAME_LENGTH+1];	   // Database name
	int		m_nLogOdbcPoolSize;					       // Database pool size
	int		m_nLogOdbcResponseTime;				       // Query response time (sec)

	// Shop database setting value 
	TCHAR	m_szShopOdbcName    [DB_SVR_NAME_LENGTH+1]; // Database server name
	TCHAR	m_szShopOdbcUser    [USR_ID_LENGTH+1];	    // User account
	TCHAR	m_szShopOdbcPass    [USR_PASS_LENGTH+1];	// Password
	TCHAR	m_szShopOdbcDatabase[DB_NAME_LENGTH+1];	    // Database name
	int		m_nShopOdbcPoolSize;					    // Database pool size
	int		m_nShopOdbcResponseTime;				    // Query response time (sec)

/*

	TCHAR	m_szBoardOdbcName    [DB_SVR_NAME_LENGTH+1]; // Database server name
	TCHAR	m_szBoardOdbcUser    [USR_ID_LENGTH+1];	     // User account
	TCHAR	m_szBoardOdbcPass    [USR_PASS_LENGTH+1];	 // Password
	TCHAR	m_szBoardOdbcDatabase[DB_NAME_LENGTH+1];	 // Database name
	int		m_nBoardOdbcPoolSize;					     // Database pool size
	int		m_nBoardOdbcResponseTime;				     // Query response time (sec)

	// Terra database setting value 
	TCHAR	m_szTerraOdbcName    [DB_SVR_NAME_LENGTH+1]; // Database server name
	TCHAR	m_szTerraOdbcUser    [USR_ID_LENGTH+1];	     // User account
	TCHAR	m_szTerraOdbcPass    [USR_PASS_LENGTH+1];	 // Password
	TCHAR	m_szTerraOdbcDatabase[DB_NAME_LENGTH+1];	 // Database name
	int		m_nTerraOdbcPoolSize;					     // Database pool size
	int		m_nTerraOdbcResponseTime;				     // Query response time (sec)

	// GSP database setting value 
	TCHAR m_szGspOdbcName    [DB_SVR_NAME_LENGTH+1]; // Database server name
	TCHAR m_szGspOdbcUser    [USR_ID_LENGTH+1];	     // User account
	TCHAR m_szGspOdbcPass    [USR_PASS_LENGTH+1];	 // Password
	TCHAR m_szGspOdbcDatabase[DB_NAME_LENGTH+1];	 // Database name
	int	  m_nGspOdbcPoolSize;					     // Database pool size
	int	  m_nGspOdbcResponseTime;				     // Query response time (sec)

	// Korea database setting value 
	TCHAR m_szKorOdbcName    [DB_SVR_NAME_LENGTH+1]; // Database server name
	TCHAR m_szKorOdbcUser    [USR_ID_LENGTH+1];	     // User account
	TCHAR m_szKorOdbcPass    [USR_PASS_LENGTH+1];	 // Password
	TCHAR m_szKorOdbcDatabase[DB_NAME_LENGTH+1];	 // Database name
	int	  m_nKorOdbcPoolSize;					     // Database pool size
	int	  m_nKorOdbcResponseTime;				     // Query response time (sec)

	// Malaysia database setting value
	TCHAR m_szMyOdbcName    [DB_SVR_NAME_LENGTH+1];	 // Database server name
	TCHAR m_szMyOdbcUser    [USR_ID_LENGTH+1];	     // User account
	TCHAR m_szMyOdbcPass    [USR_PASS_LENGTH+1];	 // Password
	TCHAR m_szMyOdbcDatabase[DB_NAME_LENGTH+1];		 // Database name
	int	  m_nMyOdbcPoolSize;					     // Database pool size
	int	  m_nMyOdbcResponseTime;				     // Query response time (sec)

*/


public:
	BOOL				m_bODBC;
	BOOL				useODBC()	{ return m_bODBC; };





public:
	void				SetDefault(void);
	int					Process(TCHAR* szLine);
	int					Load(const TCHAR* filename);
	
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

	TCHAR*				GetWebOdbcName(void);
	TCHAR*				GetWebOdbcUser(void);
	TCHAR*				GetWebOdbcPass(void);
	TCHAR*				GetWebOdbcDatabase(void);
	int					GetWebOdbcResponseTime(void);
	int					GetWebOdbcPoolSize(void);

	TCHAR*				GetLogOdbcName(void);
	TCHAR*				GetLogOdbcUser(void);
	TCHAR*				GetLogOdbcPass(void);
	TCHAR*				GetLogOdbcDatabase(void);
	int					GetLogOdbcResponseTime(void);
	int					GetLogOdbcPoolSize(void);

	TCHAR*				GetShopOdbcName(void);
	TCHAR*				GetShopOdbcUser(void);
	TCHAR*				GetShopOdbcPass(void);
	TCHAR*				GetShopOdbcDatabase(void);
	int					GetShopOdbcResponseTime(void);
	int					GetShopOdbcPoolSize(void);

	CString				GetAppPath ();

/*

	TCHAR*				GetBoardOdbcName(void);
	TCHAR*				GetBoardOdbcUser(void);
	TCHAR*				GetBoardOdbcPass(void);
	TCHAR*				GetBoardOdbcDatabase(void);
	int					GetBoardOdbcResponseTime(void);
	int					GetBoardOdbcPoolSize(void);

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
*/

};
