#include "stdafx.h"
#include "CCfg.h"


CCfg* CCfg::SelfInstance = NULL;

CCfg::CCfg()
{
	SetDefault();
}

CCfg* CCfg::GetInstance()
{
	if (SelfInstance == NULL)
	{
		SelfInstance = new CCfg();
		SelfInstance->Load(CONFIG_FILENAME);
	}

	return SelfInstance;
}

void CCfg::ReleaseInstance()
{
	if (SelfInstance != NULL)
	{
		delete SelfInstance;
		SelfInstance = NULL;
	}
}

void CCfg::SetDefault(void)
{
	m_bODBC = FALSE;	// Default is ADO

	ZeroMemory(m_szUserDBServer, sizeof(m_szUserDBServer));
	ZeroMemory(m_szUserDBUser, sizeof(m_szUserDBUser));
	ZeroMemory(m_szUserDBPass, sizeof(m_szUserDBPass));
	ZeroMemory(m_szUserDBDatabase, sizeof(m_szUserDBDatabase));
	m_nUserDBPoolSize = 5;
	m_nUserDBResponseTime = 10;


	ZeroMemory(m_szGameDBServer, sizeof(m_szGameDBServer));
	ZeroMemory(m_szGameDBUser, sizeof(m_szGameDBUser));
	ZeroMemory(m_szGameDBPass, sizeof(m_szGameDBPass));
	ZeroMemory(m_szGameDBDatabase, sizeof(m_szGameDBDatabase));
	m_nGameDBPoolSize = 5;
	m_nGameDBResponseTime = 10;


	ZeroMemory(m_szLogDBServer, sizeof(m_szLogDBServer));
	ZeroMemory(m_szLogDBUser, sizeof(m_szLogDBUser));
	ZeroMemory(m_szLogDBPass, sizeof(m_szLogDBPass));
	ZeroMemory(m_szLogDBDatabase, sizeof(m_szLogDBDatabase));
	m_nLogDBPoolSize = 5;
	m_nLogDBResponseTime = 10;


	ZeroMemory(m_szUserOdbcName, sizeof(m_szUserOdbcName));
	ZeroMemory(m_szUserOdbcUser, sizeof (m_szUserOdbcUser));
	ZeroMemory(m_szUserOdbcPass, sizeof(m_szUserOdbcPass));
	ZeroMemory(m_szUserOdbcDatabase, sizeof(m_szUserOdbcDatabase));
	m_nUserOdbcPoolSize = 5;
	m_nUserOdbcResponseTime = 10;


	ZeroMemory(m_szGameOdbcName, sizeof(m_szGameOdbcName));
	ZeroMemory(m_szGameOdbcUser, sizeof(m_szGameOdbcPass));
	ZeroMemory(m_szGameOdbcPass, sizeof(m_szGameOdbcPass));
	ZeroMemory(m_szGameOdbcDatabase, sizeof(m_szGameOdbcDatabase));
	m_nGameOdbcPoolSize = 5;
	m_nGameOdbcResponseTime = 10;


	ZeroMemory(m_szWebOdbcName, sizeof(m_szWebOdbcName));
	ZeroMemory(m_szWebOdbcUser, sizeof(m_szWebOdbcPass));
	ZeroMemory(m_szWebOdbcPass, sizeof(m_szWebOdbcPass));
	ZeroMemory(m_szWebOdbcDatabase, sizeof(m_szWebOdbcDatabase));
	m_nWebOdbcPoolSize = 5;
	m_nWebOdbcResponseTime = 10;


	ZeroMemory(m_szLogOdbcName, sizeof(m_szLogOdbcName));
	ZeroMemory(m_szLogOdbcUser, sizeof(m_szLogOdbcUser));
	ZeroMemory(m_szLogOdbcPass, sizeof(m_szLogOdbcPass));
	ZeroMemory(m_szLogOdbcDatabase, sizeof(m_szLogOdbcDatabase));
	m_nLogOdbcPoolSize = 5;
	m_nLogOdbcResponseTime = 10;


	ZeroMemory(m_szShopOdbcName, sizeof(m_szShopOdbcName));
	ZeroMemory(m_szShopOdbcUser, sizeof(m_szShopOdbcUser));
	ZeroMemory(m_szShopOdbcPass, sizeof(m_szShopOdbcPass));
	ZeroMemory(m_szShopOdbcDatabase, sizeof(m_szShopOdbcDatabase));
	m_nShopOdbcPoolSize = 5;
	m_nShopOdbcResponseTime = 10;
}

CCfg::~CCfg()
{
}

int CCfg::Load(const TCHAR* filename)
{
	// Data type problem
	// program directory + cfg + filename	
	//SERVER_UTIL::CSystemInfo SysInfo;
	CString strAppPath;	

	strAppPath = GetAppPath();
	strAppPath += _T("\\");
	strAppPath += filename;
	
	FILE *oFile = NULL;
	TCHAR line[300];

	// Open for read 
	if ((oFile = ::_tfopen(strAppPath.GetString(), _T("r"))) == NULL)
	{
		return -1;
	}	

	// Read a line and process
	while (::_fgetts(line, 200, oFile))
	{
		Process(line);
	}
	
	// Close file
	if (::fclose( oFile ))
	{
		return -1;
	}	
	return 0;
}

int CCfg::Process(TCHAR* szLine)
{	
	TCHAR seps[]   = _T(" ,\t\n");
	TCHAR* token;	

	token = _tcstok(szLine, seps);
	while (token != NULL)
	{
		if (_tcscmp(token, _T("//")) == 0) 
		{
			return 0;
		}		

		//
		// ADO or ODBC
		//
		if (_tcscmp(token, "odbc") == 0) 
		{
			// 연결에 사용할 pool 의 size
			token = ::_tcstok(NULL, seps );
			if (token)
				m_bODBC = (::_tstoi(token) != 0 ? TRUE : FALSE);
			else
				return 0;
		}

		else if (_tcscmp(token, "user_db_server") == 0) 
		{
			// database server name
			token = ::_tcstok(NULL, seps );
			if (token)
				::StringCchCopy(m_szUserDBServer, DB_SVR_NAME_LENGTH+1, token);
			else
				return 0;
		}
		else if (_tcscmp(token, "user_db_user") == 0) 
		{
			// 접속가능한 사용자 id
			token = ::_tcstok(NULL, seps );
			if (token)
				::StringCchCopy(m_szUserDBUser, USR_ID_LENGTH+1, token);
			else
				return 0;
		}
		else if (_tcscmp(token, "user_db_pass") == 0) 
		{
			// 비밀번호
			token = ::_tcstok(NULL, seps);
			if (token)
				::StringCchCopy(m_szUserDBPass, USR_PASS_LENGTH+1, token);
			else
				return 0;
		}
		else if (_tcscmp(token, "user_db_database") == 0) 
		{
			// database 명
			token = ::_tcstok(NULL, seps );
			if (token)
				::StringCchCopy(m_szUserDBDatabase, DB_NAME_LENGTH+1, token);
			else
				return 0;
		}
		else if (_tcscmp(token, "user_db_pool_size") == 0) 
		{
			// 연결에 사용할 pool 의 size
			token = ::_tcstok(NULL, seps );
			if (token)
				m_nUserDBPoolSize = ::_tstoi(token);
			else
				return 0;
		}
		else if (_tcscmp(token, "user_db_response_time") == 0) 
		{
			// 쿼리 대기시간 (sec)
			token = ::_tcstok(NULL, seps );
			if (token)
				m_nUserDBResponseTime = _tstoi(token);
			else
				return 0;
		}

		//////////////////////////////////////////////////////////////////////
		// DB-LIB C Game database setting value 
		else if (_tcscmp(token, "game_db_server") == 0) 
		{
			// database server name
			token = ::_tcstok(NULL, seps );
			if (token)
				::StringCchCopy(m_szGameDBServer, DB_SVR_NAME_LENGTH+1, token);
			else
				return 0;
		}
		else if (_tcscmp(token, "game_db_user") == 0) 
		{
			// 접속가능한 사용자 id
			token = ::_tcstok(NULL, seps );
			if (token)
				::StringCchCopy(m_szGameDBUser, USR_ID_LENGTH+1, token);
			else
				return 0;
		}
		else if (_tcscmp(token, "game_db_pass") == 0) 
		{
			// 비밀번호
			token = ::_tcstok(NULL, seps);
			if (token)
				::StringCchCopy(m_szGameDBPass, USR_PASS_LENGTH+1, token);
			else
				return 0;
		}
		else if (_tcscmp(token, "game_db_database") == 0) 
		{
			// database 명
			token = ::_tcstok(NULL, seps );
			if (token)
				::StringCchCopy(m_szGameDBDatabase, DB_NAME_LENGTH+1, token);
			else
				return 0;
		}
		else if (_tcscmp(token, "game_db_pool_size") == 0) 
		{
			// 연결에 사용할 pool 의 size
			token = ::_tcstok(NULL, seps );
			if (token)
				m_nGameDBPoolSize = _tstoi(token);
			else
				return 0;
		}
		else if (_tcscmp(token, "game_db_response_time") == 0) 
		{
			// 쿼리 대기시간 (sec)
			token = ::_tcstok(NULL, seps );
			if (token)
				m_nGameDBResponseTime = _tstoi(token);
			else
				return 0;
		}

		//////////////////////////////////////////////////////////////////////
		// ODBC User database setting value 
		else if (_tcscmp(token, "user_odbc_name") == 0) 
		{
			// database server name
			token = ::_tcstok(NULL, seps );
			if (token)	StringCchCopy(m_szUserOdbcName, DB_SVR_NAME_LENGTH+1, token);
			else		return 0;
		}
		else if (_tcscmp(token, "user_odbc_user") == 0) 
		{
			// 접속가능한 사용자 id
			token = ::_tcstok(NULL, seps );
			if (token)		StringCchCopy(m_szUserOdbcUser, USR_ID_LENGTH+1, token);
			else			return 0;
		}
		else if (_tcscmp(token, "user_odbc_pass") == 0) 
		{
			// 비밀번호
			token = ::_tcstok(NULL, seps);
			if (token)	StringCchCopy(m_szUserOdbcPass, USR_PASS_LENGTH+1, token);
			else		return 0;
		}
		else if (_tcscmp(token, "user_odbc_database") == 0) 
		{
			// database 명
			token = ::_tcstok(NULL, seps );
			if (token)		StringCchCopy(m_szUserOdbcDatabase, DB_NAME_LENGTH+1, token);
			else			return 0;
		}
		else if (_tcscmp(token, "user_odbc_pool_size") == 0) 
		{
			// 연결에 사용할 pool 의 size
			token = ::_tcstok(NULL, seps );
			if (token)		m_nUserOdbcPoolSize = ::_tstoi(token);
			else			return 0;
		}
		else if (_tcscmp(token, "user_odbc_response_time") == 0) 
		{
			// 쿼리 대기시간 (sec)
			token = ::_tcstok(NULL, seps );
			if (token)		m_nUserOdbcResponseTime = ::_tstoi(token);
			else			return 0;
		}

		//////////////////////////////////////////////////////////////////////
		// ODBC Game database setting value 
		else if (_tcscmp(token, "game_odbc_name") == 0) 
		{
			// database server name
			token = ::_tcstok(NULL, seps );
			if (token)	StringCchCopy(m_szGameOdbcName, DB_SVR_NAME_LENGTH+1, token);
			else		return 0;
		}
		else if (_tcscmp(token, "game_odbc_user") == 0) 
		{
			// 접속가능한 사용자 id
			token = ::_tcstok(NULL, seps );
			if (token)		StringCchCopy(m_szGameOdbcUser, USR_ID_LENGTH+1, token);
			else			return 0;
		}
		else if (_tcscmp(token, "game_odbc_pass") == 0) 
		{
			// 비밀번호
			token = ::_tcstok(NULL, seps);
			if (token)	StringCchCopy(m_szGameOdbcPass, USR_PASS_LENGTH+1, token);
			else		return 0;
		}
		else if (_tcscmp(token, "game_odbc_database") == 0) 
		{
			// database 명
			token = ::_tcstok(NULL, seps );
			if (token)		StringCchCopy(m_szGameOdbcDatabase, DB_NAME_LENGTH+1, token);
			else			return 0;
		}
		else if (_tcscmp(token, "game_odbc_pool_size") == 0) 
		{
			// 연결에 사용할 pool 의 size
			token = ::_tcstok(NULL, seps );
			if (token)		m_nGameOdbcPoolSize = ::_tstoi(token);
			else			return 0;
		}
		else if (_tcscmp(token, "game_odbc_response_time") == 0) 
		{
			// 쿼리 대기시간 (sec)
			token = ::_tcstok(NULL, seps );
			if (token)		m_nGameOdbcResponseTime = ::_tstoi(token);
			else			return 0;
		}

		//////////////////////////////////////////////////////////////////////
		// ODBC Web database setting value 
		else if (_tcscmp(token, "web_odbc_name") == 0) 
		{
			// database server name
			token = ::_tcstok(NULL, seps );
			if (token)	StringCchCopy(m_szWebOdbcName, DB_SVR_NAME_LENGTH+1, token);
			else		return 0;
		}
		else if (_tcscmp(token, "web_odbc_user") == 0) 
		{
			// 접속가능한 사용자 id
			token = ::_tcstok(NULL, seps );
			if (token)		StringCchCopy(m_szWebOdbcUser, USR_ID_LENGTH+1, token);
			else			return 0;
		}
		else if (_tcscmp(token, "web_odbc_pass") == 0) 
		{
			// 비밀번호
			token = ::_tcstok(NULL, seps);
			if (token)	StringCchCopy(m_szWebOdbcPass, USR_PASS_LENGTH+1, token);
			else		return 0;
		}
		else if (_tcscmp(token, "web_odbc_database") == 0) 
		{
			// database 명
			token = ::_tcstok(NULL, seps );
			if (token)		StringCchCopy(m_szWebOdbcDatabase, DB_NAME_LENGTH+1, token);
			else			return 0;
		}
		else if (_tcscmp(token, "web_odbc_pool_size") == 0) 
		{
			// 연결에 사용할 pool 의 size
			token = ::_tcstok(NULL, seps );
			if (token)		m_nWebOdbcPoolSize = ::_tstoi(token);
			else			return 0;
		}
		else if (_tcscmp(token, "web_odbc_response_time") == 0) 
		{
			// 쿼리 대기시간 (sec)
			token = ::_tcstok(NULL, seps );
			if (token)		m_nWebOdbcResponseTime = ::_tstoi(token);
			else			return 0;
		}

		//////////////////////////////////////////////////////////////////////
		// ODBC Log database setting value 
		else if (_tcscmp(token, "log_odbc_name") == 0) 
		{
			// database server name
			token = ::_tcstok(NULL, seps );
			if (token)	StringCchCopy(m_szLogOdbcName, DB_SVR_NAME_LENGTH+1, token);
			else		return 0;
		}
		else if (_tcscmp(token, "log_odbc_user") == 0) 
		{
			// 접속가능한 사용자 id
			token = ::_tcstok(NULL, seps );
			if (token)		StringCchCopy(m_szLogOdbcUser, USR_ID_LENGTH+1, token);
			else			return 0;
		}
		else if (_tcscmp(token, "log_odbc_pass") == 0) 
		{
			// 비밀번호
			token = ::_tcstok(NULL, seps);
			if (token)	StringCchCopy(m_szLogOdbcPass, USR_PASS_LENGTH+1, token);
			else		return 0;
		}
		else if (_tcscmp(token, "log_odbc_database") == 0) 
		{
			// database 명
			token = ::_tcstok(NULL, seps );
			if (token)		StringCchCopy(m_szLogOdbcDatabase, DB_SVR_NAME_LENGTH+1, token);
			else			return 0;
		}
		else if (_tcscmp(token, "log_odbc_pool_size") == 0) 
		{
			// 연결에 사용할 pool 의 size
			token = ::_tcstok(NULL, seps );
			if (token)		m_nLogOdbcPoolSize = ::_tstoi(token);
			else			return 0;
		}
		else if (_tcscmp(token, "log_odbc_response_time") == 0) 
		{
			// 쿼리 대기시간 (sec)
			token = ::_tcstok(NULL, seps );
			if (token)		m_nLogOdbcResponseTime = ::_tstoi(token);
			else			return 0;
		}

		//////////////////////////////////////////////////////////////////////
		// ODBC Shop database setting value 
		else if (_tcscmp(token, "shop_odbc_name") == 0) 
		{
			// database server name
			token = ::_tcstok(NULL, seps );
			if (token)	StringCchCopy(m_szShopOdbcName, DB_SVR_NAME_LENGTH+1, token);
			else		return 0;
		}
		else if (_tcscmp(token, "shop_odbc_user") == 0) 
		{
			// 접속가능한 사용자 id
			token = ::_tcstok(NULL, seps );
			if (token)		StringCchCopy(m_szShopOdbcUser, USR_ID_LENGTH+1, token);
			else			return 0;
		}
		else if (_tcscmp(token, "shop_odbc_pass") == 0) 
		{
			// 비밀번호
			token = ::_tcstok(NULL, seps);
			if (token)	StringCchCopy(m_szShopOdbcPass, USR_PASS_LENGTH+1, token);
			else		return 0;
		}
		else if (_tcscmp(token, "shop_odbc_database") == 0) 
		{
			// database 명
			token = ::_tcstok(NULL, seps );
			if (token)		StringCchCopy(m_szShopOdbcDatabase, DB_NAME_LENGTH+1, token);
			else			return 0;
		}
		else if (_tcscmp(token, "shop_odbc_pool_size") == 0) 
		{
			// 연결에 사용할 pool 의 size
			token = ::_tcstok(NULL, seps );
			if (token)		m_nShopOdbcPoolSize = ::_tstoi(token);
			else			return 0;
		}
		else if (_tcscmp(token, "shop_odbc_response_time") == 0) 
		{
			// 쿼리 대기시간 (sec)
			token = ::_tcstok(NULL, seps );
			if (token)		m_nShopOdbcResponseTime = ::_tstoi(token);
			else			return 0;
		}

/*

		//////////////////////////////////////////////////////////////////////
		// ODBC Board database setting value 
		else if (_tcscmp(token, "board_odbc_name") == 0) 
		{
			// database server name
			token = ::_tcstok(NULL, seps );
			if (token)	StringCchCopy(m_szBoardOdbcName, DB_SVR_NAME_LENGTH+1, token);
			else		return 0;
		}
		else if (_tcscmp(token, "board_odbc_user") == 0) 
		{
			// 접속가능한 사용자 id
			token = ::_tcstok(NULL, seps );
			if (token)		StringCchCopy(m_szBoardOdbcUser, USR_ID_LENGTH+1, token);
			else			return 0;
		}
		else if (_tcscmp(token, "board_odbc_pass") == 0) 
		{
			// 비밀번호
			token = ::_tcstok(NULL, seps);
			if (token)	StringCchCopy(m_szBoardOdbcPass, USR_PASS_LENGTH+1, token);
			else		return 0;
		}
		else if (_tcscmp(token, "board_odbc_database") == 0) 
		{
			// database 명
			token = ::_tcstok(NULL, seps );
			if (token)		StringCchCopy(m_szBoardOdbcDatabase, DB_NAME_LENGTH+1, token);
			else			return 0;
		}
		else if (_tcscmp(token, "board_odbc_pool_size") == 0) 
		{
			// 연결에 사용할 pool 의 size
			token = ::_tcstok(NULL, seps );
			if (token)		m_nBoardOdbcPoolSize = ::_tstoi(token);
			else			return 0;
		}
		else if (_tcscmp(token, "board_odbc_response_time") == 0) 
		{
			// 쿼리 대기시간 (sec)
			token = ::_tcstok(NULL, seps );
			if (token)		m_nBoardOdbcResponseTime = ::_tstoi(token);
			else			return 0;
		}

		//////////////////////////////////////////////////////////////////////
		// ODBC Terra database setting value 
		else if (_tcscmp(token, "terra_odbc_name") == 0) 
		{
			// database server name
			token = ::_tcstok(NULL, seps );
			if (token)	StringCchCopy(m_szTerraOdbcName, DB_SVR_NAME_LENGTH+1, token);
			else		return 0;
		}
		else if (_tcscmp(token, "terra_odbc_user") == 0) 
		{
			// 접속가능한 사용자 id
			token = ::_tcstok(NULL, seps );
			if (token)		StringCchCopy(m_szTerraOdbcUser, USR_ID_LENGTH+1, token);
			else			return 0;
		}
		else if (_tcscmp(token, "terra_odbc_pass") == 0) 
		{
			// 비밀번호
			token = ::_tcstok(NULL, seps);
			if (token)	StringCchCopy(m_szTerraOdbcPass, USR_PASS_LENGTH+1, token);
			else		return 0;
		}
		else if (_tcscmp(token, "terra_odbc_database") == 0) 
		{
			// database 명
			token = ::_tcstok(NULL, seps );
			if (token)		StringCchCopy(m_szTerraOdbcDatabase, DB_NAME_LENGTH+1, token);
			else			return 0;
		}
		else if (_tcscmp(token, "terra_odbc_pool_size") == 0) 
		{
			// 연결에 사용할 pool 의 size
			token = ::_tcstok(NULL, seps );
			if (token)		m_nTerraOdbcPoolSize = ::_tstoi(token);
			else			return 0;
		}
		else if (_tcscmp(token, "terra_odbc_response_time") == 0) 
		{
			// 쿼리 대기시간 (sec)
			token = ::_tcstok(NULL, seps );
			if (token)		m_nTerraOdbcResponseTime = ::_tstoi(token);
			else			return 0;
		}

		//////////////////////////////////////////////////////////////////////
		// ODBC GSP database setting value 
		else if (_tcscmp(token, "gsp_odbc_name") == 0) 
		{
			// database server name
			token = ::_tcstok(NULL, seps );
			if (token)	StringCchCopy(m_szGspOdbcName, DB_SVR_NAME_LENGTH+1, token);
			else		return 0;
		}
		else if (_tcscmp(token, "gsp_odbc_user") == 0) 
		{
			// 접속가능한 사용자 id
			token = ::_tcstok(NULL, seps );
			if (token)		StringCchCopy(m_szGspOdbcUser, USR_ID_LENGTH+1, token);
			else			return 0;
		}
		else if (_tcscmp(token, "gsp_odbc_pass") == 0) 
		{
			// 비밀번호
			token = ::_tcstok(NULL, seps);
			if (token)	StringCchCopy(m_szGspOdbcPass, USR_PASS_LENGTH+1, token);
			else		return 0;
		}
		else if (_tcscmp(token, "gsp_odbc_database") == 0) 
		{
			// database 명
			token = ::_tcstok(NULL, seps );
			if (token)		StringCchCopy(m_szGspOdbcDatabase, DB_NAME_LENGTH+1, token);
			else			return 0;
		}
		else if (_tcscmp(token, "gsp_odbc_pool_size") == 0) 
		{
			// 연결에 사용할 pool 의 size
			token = ::_tcstok(NULL, seps );
			if (token)		m_nGspOdbcPoolSize = ::_tstoi(token);
			else			return 0;
		}
		else if (_tcscmp(token, "gsp_odbc_response_time") == 0) 
		{
			// 쿼리 대기시간 (sec)
			token = ::_tcstok(NULL, seps );
			if (token)		m_nGspOdbcResponseTime = ::_tstoi(token);
			else			return 0;
		}

		//////////////////////////////////////////////////////////////////////
		// ODBC Korea database setting value 
		else if (_tcscmp(token, "kor_odbc_name") == 0) 
		{
			// database server name
			token = ::_tcstok(NULL, seps );
			if (token)	StringCchCopy(m_szKorOdbcName, DB_SVR_NAME_LENGTH+1, token);
			else		return 0;
		}
		else if (_tcscmp(token, "kor_odbc_user") == 0) 
		{
			// 접속가능한 사용자 id
			token = ::_tcstok(NULL, seps );
			if (token)		StringCchCopy(m_szKorOdbcUser, USR_ID_LENGTH+1, token);
			else			return 0;
		}
		else if (_tcscmp(token, "kor_odbc_pass") == 0) 
		{
			// 비밀번호
			token = ::_tcstok(NULL, seps);
			if (token)	StringCchCopy(m_szKorOdbcPass, USR_PASS_LENGTH+1, token);
			else		return 0;
		}
		else if (_tcscmp(token, "kor_odbc_database") == 0) 
		{
			// database 명
			token = ::_tcstok(NULL, seps );
			if (token)		StringCchCopy(m_szKorOdbcDatabase, DB_NAME_LENGTH+1, token);
			else			return 0;
		}
		else if (_tcscmp(token, "kor_odbc_pool_size") == 0) 
		{
			// 연결에 사용할 pool 의 size
			token = ::_tcstok(NULL, seps );
			if (token)		m_nKorOdbcPoolSize = ::_tstoi(token);
			else			return 0;
		}
		else if (_tcscmp(token, "kor_odbc_response_time") == 0) 
		{
			// 쿼리 대기시간 (sec)
			token = ::_tcstok(NULL, seps );
			if (token)		m_nKorOdbcResponseTime = ::_tstoi(token);
			else			return 0;
		}

		//////////////////////////////////////////////////////////////////////
		// ODBC Malaysia database setting value 
		else if (_tcscmp(token, "my_odbc_name") == 0) 
		{
			// database server name
			token = ::_tcstok(NULL, seps );
			if (token)	StringCchCopy(m_szMyOdbcName, DB_SVR_NAME_LENGTH+1, token);
			else		return 0;
		}
		else if (_tcscmp(token, "my_odbc_user") == 0) 
		{
			// 접속가능한 사용자 id
			token = ::_tcstok(NULL, seps );
			if (token)		StringCchCopy(m_szMyOdbcUser, USR_ID_LENGTH+1, token);
			else			return 0;
		}
		else if (_tcscmp(token, "my_odbc_pass") == 0) 
		{
			// 비밀번호
			token = ::_tcstok(NULL, seps);
			if (token)	StringCchCopy(m_szMyOdbcPass, USR_PASS_LENGTH+1, token);
			else		return 0;
		}
		else if (_tcscmp(token, "my_odbc_database") == 0) 
		{
			// database 명
			token = ::_tcstok(NULL, seps );
			if (token)		StringCchCopy(m_szMyOdbcDatabase, DB_NAME_LENGTH+1, token);
			else			return 0;
		}
		else if (_tcscmp(token, "my_odbc_pool_size") == 0) 
		{
			// 연결에 사용할 pool 의 size
			token = ::_tcstok(NULL, seps );
			if (token)		m_nMyOdbcPoolSize = ::_tstoi(token);
			else			return 0;
		}
		else if (_tcscmp(token, "my_odbc_response_time") == 0) 
		{
			// 쿼리 대기시간 (sec)
			token = ::_tcstok(NULL, seps );
			if (token)		m_nMyOdbcResponseTime = ::_tstoi(token);
			else			return 0;
		}
*/


		// Get next token
		token = ::_tcstok( NULL, seps );
	}

	return 0;
}	


///////////////////////////////////////////////////////////////////////////
// DB-Lib C
// User
TCHAR*	CCfg::GetUserDBServer(void)			{ return m_szUserDBServer; }
TCHAR*	CCfg::GetUserDBUser(void)			{ return m_szUserDBUser; }
TCHAR*	CCfg::GetUserDBPass(void)			{ return m_szUserDBPass; }
TCHAR*	CCfg::GetUserDBDatabase(void)		{ return m_szUserDBDatabase; }
int		CCfg::GetUserDBResponseTime(void)	{ return m_nUserDBResponseTime; }
int		CCfg::GetUserDBPoolSize()			{ return m_nUserDBPoolSize; }

// Game
TCHAR*	CCfg::GetGameDBServer(void)			{ return m_szGameDBServer; }
TCHAR*	CCfg::GetGameDBUser(void)			{ return m_szGameDBUser; }
TCHAR*	CCfg::GetGameDBPass(void)			{ return m_szGameDBPass; }
TCHAR*	CCfg::GetGameDBDatabase(void)		{ return m_szGameDBDatabase; }
int		CCfg::GetGameDBResponseTime(void)	{ return m_nGameDBResponseTime; }
int		CCfg::GetGameDBPoolSize()			{ return m_nGameDBPoolSize; }

///////////////////////////////////////////////////////////////////////////
// ODBC
// User
TCHAR*	CCfg::GetUserOdbcName(void)			{ return m_szUserOdbcName; }
TCHAR*	CCfg::GetUserOdbcUser(void)			{ return m_szUserOdbcUser; }
TCHAR*	CCfg::GetUserOdbcPass(void)			{ return m_szUserOdbcPass; }
TCHAR*	CCfg::GetUserOdbcDatabase(void)		{ return m_szUserOdbcDatabase; }
int		CCfg::GetUserOdbcResponseTime(void)	{ return m_nUserOdbcResponseTime; }
int		CCfg::GetUserOdbcPoolSize()			{ return m_nUserOdbcPoolSize; }

// Game
TCHAR*	CCfg::GetGameOdbcName(void)			{ return m_szGameOdbcName; }
TCHAR*	CCfg::GetGameOdbcUser(void)			{ return m_szGameOdbcUser; }
TCHAR*	CCfg::GetGameOdbcPass(void)			{ return m_szGameOdbcPass; }
TCHAR*	CCfg::GetGameOdbcDatabase(void)		{ return m_szGameOdbcDatabase; }
int		CCfg::GetGameOdbcResponseTime(void)	{ return m_nGameOdbcResponseTime; }
int		CCfg::GetGameOdbcPoolSize()			{ return m_nGameOdbcPoolSize; }

// Web
TCHAR*	CCfg::GetWebOdbcName(void)			{ return m_szWebOdbcName; }
TCHAR*	CCfg::GetWebOdbcUser(void)			{ return m_szWebOdbcUser; }
TCHAR*	CCfg::GetWebOdbcPass(void)			{ return m_szWebOdbcPass; }
TCHAR*	CCfg::GetWebOdbcDatabase(void)		{ return m_szWebOdbcDatabase; }
int		CCfg::GetWebOdbcResponseTime(void)	{ return m_nWebOdbcResponseTime; }
int		CCfg::GetWebOdbcPoolSize()			{ return m_nWebOdbcPoolSize; }

// Log
TCHAR*	CCfg::GetLogOdbcName(void)		    { return m_szLogOdbcName; }
TCHAR*	CCfg::GetLogOdbcUser(void)		    { return m_szLogOdbcUser; }
TCHAR*	CCfg::GetLogOdbcPass(void)		    { return m_szLogOdbcPass; }
TCHAR*	CCfg::GetLogOdbcDatabase(void)	    { return m_szLogOdbcDatabase; }
int		CCfg::GetLogOdbcResponseTime(void)  { return m_nLogOdbcResponseTime; }
int		CCfg::GetLogOdbcPoolSize()		    { return m_nLogOdbcPoolSize; }

// Shop
TCHAR*	CCfg::GetShopOdbcName(void)		    { return m_szShopOdbcName; }
TCHAR*	CCfg::GetShopOdbcUser(void)		    { return m_szShopOdbcUser; }
TCHAR*	CCfg::GetShopOdbcPass(void)		    { return m_szShopOdbcPass; }
TCHAR*	CCfg::GetShopOdbcDatabase(void)	    { return m_szShopOdbcDatabase; }
int		CCfg::GetShopOdbcResponseTime(void) { return m_nShopOdbcResponseTime; }
int		CCfg::GetShopOdbcPoolSize()		    { return m_nShopOdbcPoolSize; }

/*

// Board
TCHAR*	CCfg::GetBoardOdbcName(void)		{ return m_szBoardOdbcName; }
TCHAR*	CCfg::GetBoardOdbcUser(void)		{ return m_szBoardOdbcUser; }
TCHAR*	CCfg::GetBoardOdbcPass(void)		{ return m_szBoardOdbcPass; }
TCHAR*	CCfg::GetBoardOdbcDatabase(void)	{ return m_szBoardOdbcDatabase; }
int		CCfg::GetBoardOdbcResponseTime(void){ return m_nBoardOdbcResponseTime; }
int		CCfg::GetBoardOdbcPoolSize()		{ return m_nBoardOdbcPoolSize; }

// Terra
TCHAR*	CCfg::GetTerraOdbcName(void)		{ return m_szTerraOdbcName; }
TCHAR*	CCfg::GetTerraOdbcUser(void)		{ return m_szTerraOdbcUser; }
TCHAR*	CCfg::GetTerraOdbcPass(void)		{ return m_szTerraOdbcPass; }
TCHAR*	CCfg::GetTerraOdbcDatabase(void)	{ return m_szTerraOdbcDatabase; }
int		CCfg::GetTerraOdbcResponseTime(void){ return m_nTerraOdbcResponseTime; }
int		CCfg::GetTerraOdbcPoolSize()		{ return m_nTerraOdbcPoolSize; }

// GSP
TCHAR*	CCfg::GetGspOdbcName(void)		{ return m_szGspOdbcName; }
TCHAR*	CCfg::GetGspOdbcUser(void)		{ return m_szGspOdbcUser; }
TCHAR*	CCfg::GetGspOdbcPass(void)		{ return m_szGspOdbcPass; }
TCHAR*	CCfg::GetGspOdbcDatabase(void)	{ return m_szGspOdbcDatabase; }
int		CCfg::GetGspOdbcResponseTime(void){ return m_nGspOdbcResponseTime; }
int		CCfg::GetGspOdbcPoolSize()		{ return m_nGspOdbcPoolSize; }

// Korea
TCHAR*	CCfg::GetKorOdbcName(void)		{ return m_szKorOdbcName; }
TCHAR*	CCfg::GetKorOdbcUser(void)		{ return m_szKorOdbcUser; }
TCHAR*	CCfg::GetKorOdbcPass(void)		{ return m_szKorOdbcPass; }
TCHAR*	CCfg::GetKorOdbcDatabase(void)	{ return m_szKorOdbcDatabase; }
int		CCfg::GetKorOdbcResponseTime(void){ return m_nKorOdbcResponseTime; }
int		CCfg::GetKorOdbcPoolSize()		{ return m_nKorOdbcPoolSize; }

// Malaysia
TCHAR*	CCfg::GetMyOdbcName(void)		{ return m_szMyOdbcName; }
TCHAR*	CCfg::GetMyOdbcUser(void)		{ return m_szMyOdbcUser; }
TCHAR*	CCfg::GetMyOdbcPass(void)		{ return m_szMyOdbcPass; }
TCHAR*	CCfg::GetMyOdbcDatabase(void)	{ return m_szMyOdbcDatabase; }
int		CCfg::GetMyOdbcResponseTime(void){ return m_nMyOdbcResponseTime; }
int		CCfg::GetMyOdbcPoolSize()		{ return m_nMyOdbcPoolSize; }
*/



CString	CCfg::GetAppPath ()
{
	CString strFullPath;
	CString strCommandLine;

	TCHAR szPath[MAX_PATH] = {0};
	GetModuleFileName(::AfxGetInstanceHandle(), szPath, MAX_PATH);
	strCommandLine = szPath;

	if ( !strCommandLine.IsEmpty() )
	{
		DWORD dwFind = strCommandLine.ReverseFind ( '\\' );
		if ( dwFind != -1 )
		{
			strFullPath = strCommandLine.Left ( dwFind );

			if ( !strFullPath.IsEmpty() )
				if ( strFullPath.GetAt(0) == '"' )
					strFullPath = strFullPath.Right ( strFullPath.GetLength() - 1 );
		}
	}

	if (strFullPath.GetLength() < 4)
	{
		TCHAR szTest[MAX_PATH] = {0};
		::GetCurrentDirectory(MAX_PATH, szTest);		
		CString strTest(szTest);
		return strTest;
	}
	else
	{
		return strFullPath;
	}
}