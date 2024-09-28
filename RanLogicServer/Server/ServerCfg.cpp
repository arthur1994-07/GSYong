#include "../pch.h"
//#include "../../RanLogic/Util/s_CSystemInfo.h"
#include "../../SigmaCore/Util/SystemInfo.h"
#include "ServerCfg.h"
#pragma warning(disable:4996)

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

//ServerCfg* ServerCfg::SelfInstance = NULL;

ServerCfg::ServerCfg()
{
	SetDefault();
}

ServerCfg::ServerCfg(const char* filename)
{
	SetDefault();
	Load(filename);
}

void ServerCfg::SetDefault(void)
{
	///////////////////////////////////////////////////////////////////////////
	// Server Setting Values
	m_nServerVersion		= 0;	// 서버 버전, 클라이언트 버전과 동일
	m_nPatchVersion			= 0;	// 패치프로그램 버전
	
	m_nPortService			= 5001;	// 서비스용 포트
	m_nPortControl			= 6001;	// 컨트롤용 포트
	
	m_nServerType			= 0;	// 서버 유형
	m_nServerGroup			= 0;	// 서버 그룹
	m_nServerNumber			= 0;	// 서버 번호
	m_nServerField			= 0;	// 서버 필드 번호.
	m_nServerChannel        = 0;    // 서버 채널 번호
	m_nServerChannelNumber  = 1;    // Agent 가 가진 채널 갯수
	
	m_nServerMaxClient		= 1000;	// 최대 접속 가능 클라이언트 수
	m_nMaxChannelUser       = 1000; // 채널당 최대 접속 가능 클라이언트 수


	m_ExcessExpProcessType  = 0; // 초과 경험치에 대한 디폴트 값

	m_nMaxFieldNumber = 0;

	m_nGAMEServerNumber		= 0;
	m_nLOGINServerNumber	= 0;
	m_nFTPServerNumber		= 0;
	m_nSESSIONServerNumber	= 0;

	m_bHeartBeat			= true;  // Heart beat check
	m_nServiceProvider		= 0;     // Service provider
	m_nMaxThread			= 8;	 // 최대 work thread 갯수
	m_bTestServer			= false; // Test 서버 유무
	m_bAdultCheck			= false; // 성인서버 테스트 유무	
	m_bUseEventThread		= false; // 이벤트 쓰레드 사용 여부
	m_bPK                   = true;  // true:PK 서버, false:Non PK 서버 ( PK 가능여부 ) 
	m_bPkLess               = false;
	m_bGameGuardAuth        = false; // nProtect GameGuard 인증 사용여부

//	m_bAllowFileLoad		= false; // allow_ip.cfg파일 사용여부
	m_bPKServer				= false; // freePK 서버 ( PK이벤트용 ) 

    m_bNewServer            = false;

	for (int i=0; i<MAX_CHANNEL_NUMBER; i++)
	{
		m_sChannelInfo[i].bPK = true;
	}
}

ServerCfg::~ServerCfg()
{
}

int ServerCfg::Load(const char* filename)
{
	// Data type problem
	// program directory + cfg + filename	
	sc::CSystemInfo SysInfo;
	CString strAppPath;	

	strAppPath = SysInfo.GetAppPath();
	strAppPath += _T("\\cfg\\");
	strAppPath += filename;
	
	FILE *oFile = NULL;
	char line[300];

	// Open for read 
	if ((oFile = ::_tfopen(strAppPath.GetString(), _T("r"))) == NULL)
	{
		/*
		MessageBox(
			NULL,
			szAppPath,
			_T("ERROR"),
			MB_OK);
		*/
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

int ServerCfg::Process(char* szLine)
{
	// 토큰으로 분리할 분리자...
	// space, comma, tab, new line
	char seps[]   = _T(" ,\t\n");
	char* token;	

	// Establish string and get the first token
	token = _tcstok(szLine, seps);
	while (token != NULL)
	{	
		// 주석일때는 무시...
		if (_tcscmp(token, _T("//")) == 0) 
		{
			return 0;
		} 		
		//////////////////////////////////////////////////////////////////////
		// server setting value
		
		
		else if (_tcscmp(token, "pkless") == 0)
		{
			// 선도클럽, PK, 학원간 PK 등 모든 적대행위 Disable
			token = _tcstok(NULL, seps);
			if (token)
			{
				if (_tstoi(token)) m_bPkLess = true;
				else             m_bPkLess = false;
			}
			else
			{
				return 0;
			}
		}
		else if (_tcscmp(token, "pk") == 0)
		{
			// PK 만 Disable
			token = ::_tcstok(NULL, seps);
			if (token)
			{
				if (::_tstoi(token))		m_bPK = true;
				else					m_bPK = false;
			}
			else
			{
				return 0;
			}
		}
		else if (_tcscmp(token, "pkserver") == 0)
		{
			// PK 만 Disable
			token = ::_tcstok(NULL, seps);
			if (token)
			{
				if (::_tstoi(token))		m_bPKServer = true;
				else					m_bPKServer = false;
			}
			else
			{
				return 0;
			}
		}
		else if (_tcscmp(token, "adult") == 0)
		{
			// Adust check
			token = ::_tcstok(NULL, seps);
			if (token)
			{
				if (::_tstoi(token))		m_bAdultCheck = true;
				else					m_bAdultCheck = false;
			}
			else
			{
				return 0;
			}
		}		
		else if (_tcscmp(token, "use_event_thread") == 0)
		{
			// use event Thread
			token = ::_tcstok(NULL, seps);
			if (token)
			{
				if (::_tstoi(token))		m_bUseEventThread = true;
				else					m_bUseEventThread = false;
			}
			else
			{
				return 0;
			}
		}
		else if (_tcscmp(token, "testserver") == 0)
		{
			// testserver
			token = ::_tcstok(NULL, seps);
			if (token)
			{
				if (::_tstoi(token))		m_bTestServer = true;
				else					m_bTestServer = false;
			}
			else
			{
				return 0;
			}
		}		
		else if (_tcscmp(token, "heartbeat") == 0)
		{
			// HeartBeat
			token = ::_tcstok(NULL, seps);
			if (token)
			{
				if (::_tstoi(token))		m_bHeartBeat = true;
				else					m_bHeartBeat = false;
			}
			else
			{
				return 0;
			}
		}
		else if (_tcscmp(token, "gameguard") == 0)
		{
			// HeartBeat
			token = ::_tcstok(NULL, seps);
			if (token)
			{
				if (::_tstoi(token))		m_bGameGuardAuth = true;
				else					m_bGameGuardAuth = false;
			}
			else
			{
				return 0;
			}
		}
		else if (_tcscmp(token, "server_version") == 0) 
		{
			// 서버 버전, 클라이언트 버전과 동일
			token = ::_tcstok(NULL, seps);
			if (token)
				m_nServerVersion = _tstoi(token);
			else
				return 0;
		}
		else if (_tcscmp(token, "patch_version") == 0)
		{
			// 패치프로그램 버전
			token = ::_tcstok(NULL, seps);
			if (token)
				m_nPatchVersion = _tstoi(token);
			else
				return 0;
		}
// 		else if (_tcscmp(token, "server_name") == 0) 
// 		{
// 			// 서버명
// 			token = ::_tcstok(NULL, seps );
// 			if (token)
// 				::StringCchCopy(m_szServerName, sc::db::DB_SVR_NAME_LENGTH+1, token);
// 			else
// 				return 0;
// 		}
		else if (_tcscmp(token, "server_max_client") == 0) 
		{
			// 최대 접속 가능 클라이언트 수
			token = ::_tcstok(NULL, seps );
			if (token)
				m_nServerMaxClient = _tstoi(token);
			else
				return 0;
		}
		else if (_tcscmp(token, "server_max_channel_client") == 0) 
		{
			// 최대 접속 가능 클라이언트 수
			token = ::_tcstok(NULL, seps );
			if (token)
				m_nMaxChannelUser = _tstoi(token);
			else
				return 0;
		}
        else if (_tcscmp(token, "new_server") == 0)
        {
            // use inteltbb
            token = ::_tcstok(NULL, seps);
            if (token)
            {
                if (::_tstoi(token))		m_bNewServer = true;
                else					    m_bNewServer = false;
            }
            else
            {
                return 0;
            }
        }			
		else if (_tcscmp(token, "server_ip") == 0) 
		{
			// 서버 IP, 프로그램에서 자동으로 결정될수 있다
			token = ::_tcstok(NULL, seps );
			if (token)
				::StringCchCopy(m_szServerIP, MAX_IP_LENGTH+1, token);
			else
				return 0;
		}
		else if (_tcscmp(token, "server_service_port") == 0) 
		{
			// 서비스용 포트
			token = ::_tcstok(NULL, seps );
			if (token)
				m_nPortService = _tstoi(token);
			else
				return 0;
		}
		else if (_tcscmp(token, "server_control_port") == 0) 
		{
			// 컨트롤용 포트
			token = ::_tcstok(NULL, seps);
			if (token)
				m_nPortControl = _tstoi(token);
			else
				return 0;
		}
		else if (_tcscmp(token, "server_type") == 0)
		{
			// server type [type]
			// [type]
			// 1 : login server
			// 2 : session server
			// 3 : game server
			token = ::_tcstok(NULL, seps);
			if (token)
				m_nServerType = _tstoi(token);
			else
				return 0;
		}
		else if (_tcscmp(token, "server_group") == 0)
		{
			// server group
			token = ::_tcstok(NULL, seps);
			if (token)
				m_nServerGroup = _tstoi(token);
			else
				return 0;
		}
		else if (_tcscmp(token, "server_number") == 0)
		{
			// Server number
			token = ::_tcstok(NULL, seps);
			if (token)
				m_nServerNumber = _tstoi(token);
			else
				return 0;
		}
		else if (_tcscmp(token, "server_field") == 0)
		{
			// Server field
			token = ::_tcstok(NULL, seps);
			if (token)
				m_nServerField = _tstoi(token);
			else
				return 0;
		}		 
		else if (_tcscmp(token, "server_channel") == 0)
		{
			// Server field
			token = ::_tcstok(NULL, seps);
			if (token)
				m_nServerChannel = _tstoi(token);
			else
				return 0;
		}
		else if (_tcscmp(token, "server_channel_number") == 0) 
		{
			// Agent 서버가 가진 채널의 갯수
			token = ::_tcstok(NULL, seps );
			if (token)
				m_nServerChannelNumber = _tstoi(token);
			else
				return 0;
		}
		else if (_tcscmp(token, "service_provider") == 0)
		{
			token = ::_tcstok(NULL, seps);
			if (token)
				m_nServiceProvider = ::_tstoi(token);
			else
				return 0;
		}
		else if (_tcscmp(token, "whiterock_name") == 0) 
		{
			// 일본 WhiteRock 서버명
			token = ::_tcstok(NULL, seps );
			if (token)
				::StringCchCopy(m_szWhiteRockServerName, sc::db::DB_SVR_NAME_LENGTH+1, token);
			else
				return 0;
		}
		else if (_tcscmp(token, "whiterock_port") == 0)
		{
			token = ::_tcstok(NULL, seps);
			if (token)
				m_nWhiteRockServerPort = ::_tstoi(token);
			else
				return 0;
		}
		else if (_tcscmp(token, "max_thread") == 0)
		{
			token = ::_tcstok(NULL, seps);
			if (token)
				m_nMaxThread = ::_tstoi(token);
			else
				return 0;
		}
		else if (_tcscmp(token, "language_set") == 0)
		{
			token = ::_tcstok(NULL, seps);
			if (token)
				m_nLangSet = ::_tstoi(token);
			else
				return 0;
		}
		else if (_tcscmp(token, "excessexp_process_type") == 0) 
		{
			// 최대 접속 가능 클라이언트 수
			token = ::_tcstok(NULL, seps );
			if (token)
				m_ExcessExpProcessType = _tstoi(token);
			else
				return 0;
		}
		else if( _tcscmp( token, "ALLOW_IP_INFO_TYPE1" ) == 0 )
		{
			token = ::_tcstok(NULL, seps);
			if ( token )
			{
				CString strTemp;
				strTemp.Format("%s", token);
				//::StringCchCopy( strTemp.GetBuffer( 0 ), sc::db::DB_SVR_NAME_LENGTH + 1, token );
                std::string Ip(strTemp.GetString());
				m_AllowIPType1.insert(MAP_ALLOW_IP_VALUE(Ip, Ip));
			}
			else
				return 0;
		}

		else if( _tcscmp( token, "ALLOW_IP_INFO_TYPE2" ) == 0 )
		{
			token = ::_tcstok(NULL, seps);
			if ( token )
			{
				CString strTemp;
				strTemp.Format("%s", token);
                std::string Ip(strTemp.GetString());
				//m_AllowIPType2.insert(MAP_ALLOW_IP_VALUE(Ip, Ip));
			}
			else
				return 0;
		}

		else if( _tcscmp( token, "ALLOW_IP_INFO_TYPE3" ) == 0 )
		{
			token = ::_tcstok(NULL, seps);
			if ( token )
			{
				CString strTemp;
				strTemp.Format("%s", token);
                std::string Ip(strTemp.GetString());
				//m_AllowIPType3.insert(MAP_ALLOW_IP_VALUE(Ip, Ip));
			}
			else
				return 0;
		}

		//////////////////////////////////////////////////////////////////////
		// ODBC User database setting value 
		else if (_tcscmp(token, "user_odbc_name") == 0) 
		{
			// database server name
			token = ::_tcstok(NULL, seps );
			if (token)
                m_UserDb.DbServerName = token;
			else
                return 0;
		}
		else if (_tcscmp(token, "user_odbc_user") == 0) 
		{
			// 접속가능한 사용자 id
			token = ::_tcstok(NULL, seps );
			if (token)
                m_UserDb.DbUserName = token;
			else
                return 0;
		}
		else if (_tcscmp(token, "user_odbc_pass") == 0) 
		{
			// 비밀번호
			token = ::_tcstok(NULL, seps);
			if (token)
                m_UserDb.DbUserPassword = token;            
			else
                return 0;
		}
		else if (_tcscmp(token, "user_odbc_database") == 0) 
		{
			// database 명
			token = ::_tcstok(NULL, seps );
			if (token)
                m_UserDb.DbDatabase = token;
			else
                return 0;
		}
		else if (_tcscmp(token, "user_odbc_pool_size") == 0) 
		{
			// 연결에 사용할 pool 의 size
			token = ::_tcstok(NULL, seps );
			if (token)
                m_UserDb.PoolSize = ::_tstoi(token);
			else
                return 0;
		}
		else if (_tcscmp(token, "user_odbc_response_time") == 0) 
		{
			// 쿼리 대기시간 (sec)
			token = ::_tcstok(NULL, seps );
			if (token)
                m_UserDb.ResponseTime = ::_tstoi(token);
			else
                return 0;
		}

		//////////////////////////////////////////////////////////////////////
		// ODBC Game database setting value 
		else if (_tcscmp(token, "game_odbc_name") == 0) 
		{
			// database server name
			token = ::_tcstok(NULL, seps );
			if (token)
                m_GameDb.DbServerName = token;
			else
                return 0;
		}
		else if (_tcscmp(token, "game_odbc_user") == 0) 
		{
			// 접속가능한 사용자 id
			token = ::_tcstok(NULL, seps );
			if (token)
                m_GameDb.DbUserName = token;
			else
                return 0;
		}
		else if (_tcscmp(token, "game_odbc_pass") == 0) 
		{
			// 비밀번호
			token = ::_tcstok(NULL, seps);
			if (token)
                m_GameDb.DbUserPassword = token;
			else
                return 0;
		}
		else if (_tcscmp(token, "game_odbc_database") == 0) 
		{
			// database 명
			token = ::_tcstok(NULL, seps );
			if (token)
                m_GameDb.DbDatabase = token;
			else
                return 0;
		}
		else if (_tcscmp(token, "game_odbc_pool_size") == 0) 
		{
			// 연결에 사용할 pool 의 size
			token = ::_tcstok(NULL, seps );
			if (token)
                m_GameDb.PoolSize = ::_tstoi(token);
			else
                return 0;
		}
		else if (_tcscmp(token, "game_odbc_response_time") == 0) 
		{
			// 쿼리 대기시간 (sec)
			token = ::_tcstok(NULL, seps );
			if (token)
                m_GameDb.ResponseTime = ::_tstoi(token);
			else
                return 0;
		}

		//////////////////////////////////////////////////////////////////////
		// ODBC Board database setting value 
		else if (_tcscmp(token, "board_odbc_name") == 0) 
		{
			// database server name
			token = ::_tcstok(NULL, seps );
			if (token)
                m_BoardDb.DbServerName = token;
			else
                return 0;
		}
		else if (_tcscmp(token, "board_odbc_user") == 0) 
		{
			// 접속가능한 사용자 id
			token = ::_tcstok(NULL, seps );
			if (token)
                m_BoardDb.DbUserName = token;
			else
                return 0;
		}
		else if (_tcscmp(token, "board_odbc_pass") == 0) 
		{
			// 비밀번호
			token = ::_tcstok(NULL, seps);
			if (token)
                m_BoardDb.DbUserPassword = token;
			else		return 0;
		}
		else if (_tcscmp(token, "board_odbc_database") == 0) 
		{
			// database 명
			token = ::_tcstok(NULL, seps );
			if (token)
                m_BoardDb.DbDatabase = token;
			else
                return 0;
		}
		else if (_tcscmp(token, "board_odbc_pool_size") == 0) 
		{
			// 연결에 사용할 pool 의 size
			token = ::_tcstok(NULL, seps );
			if (token)
                m_BoardDb.PoolSize = ::_tstoi(token);
			else
                return 0;
		}
		else if (_tcscmp(token, "board_odbc_response_time") == 0) 
		{
			// 쿼리 대기시간 (sec)
			token = ::_tcstok(NULL, seps );
			if (token)
                m_BoardDb.ResponseTime = ::_tstoi(token);
			else
                return 0;
		}

		//////////////////////////////////////////////////////////////////////
		// ODBC Log database setting value 
		else if (_tcscmp(token, "log_odbc_name") == 0) 
		{
			// database server name
			token = ::_tcstok(NULL, seps );
			if (token)
                m_LogDb.DbServerName = token;
			else
                return 0;
		}
		else if (_tcscmp(token, "log_odbc_user") == 0) 
		{
			// 접속가능한 사용자 id
			token = ::_tcstok(NULL, seps );
			if (token)
                m_LogDb.DbUserName = token;
			else
                return 0;
		}
		else if (_tcscmp(token, "log_odbc_pass") == 0) 
		{
			// 비밀번호
			token = ::_tcstok(NULL, seps);
			if (token)
                m_LogDb.DbUserPassword = token;
			else
                return 0;
		}
		else if (_tcscmp(token, "log_odbc_database") == 0) 
		{
			// database 명
			token = ::_tcstok(NULL, seps );
			if (token)
                m_LogDb.DbDatabase = token;
			else
                return 0;
		}
		else if (_tcscmp(token, "log_odbc_pool_size") == 0) 
		{
			// 연결에 사용할 pool 의 size
			token = ::_tcstok(NULL, seps );
			if (token)
                m_LogDb.PoolSize = ::_tstoi(token);
			else
                return 0;
		}
		else if (_tcscmp(token, "log_odbc_response_time") == 0) 
		{
			// 쿼리 대기시간 (sec)
			token = ::_tcstok(NULL, seps );
			if (token)
                m_LogDb.ResponseTime = ::_tstoi(token);
			else
                return 0;
		}

		//////////////////////////////////////////////////////////////////////
		// ODBC Shop database setting value 
		else if (_tcscmp(token, "shop_odbc_name") == 0) 
		{
			// database server name
			token = ::_tcstok(NULL, seps );
			if (token)
                m_ShopDb.DbServerName = token;
			else
                return 0;
		}
		else if (_tcscmp(token, "shop_odbc_user") == 0) 
		{
			// 접속가능한 사용자 id
			token = ::_tcstok(NULL, seps );
			if (token)
                m_ShopDb.DbUserName = token;
			else
                return 0;
		}
		else if (_tcscmp(token, "shop_odbc_pass") == 0) 
		{
			// 비밀번호
			token = ::_tcstok(NULL, seps);
			if (token)
                m_ShopDb.DbUserPassword = token;
			else
                return 0;
		}
		else if (_tcscmp(token, "shop_odbc_database") == 0) 
		{
			// database 명
			token = ::_tcstok(NULL, seps );
			if (token)
                m_ShopDb.DbDatabase = token;
			else
                return 0;
		}
		else if (_tcscmp(token, "shop_odbc_pool_size") == 0) 
		{
			// 연결에 사용할 pool 의 size
			token = ::_tcstok(NULL, seps );
			if (token)
                m_ShopDb.PoolSize = ::_tstoi(token);
			else
                return 0;
		}
		else if (_tcscmp(token, "shop_odbc_response_time") == 0) 
		{
			// 쿼리 대기시간 (sec)
			token = ::_tcstok(NULL, seps );
			if (token)
                m_ShopDb.ResponseTime = ::_tstoi(token);
			else
                return 0;
		}

		//////////////////////////////////////////////////////////////////////
		// ODBC Terra database setting value 
		else if (_tcscmp(token, "terra_odbc_name") == 0) 
		{
			// database server name
			token = ::_tcstok(NULL, seps );
			if (token)
                m_Terra.DbServerName = token;
			else
                return 0;
		}
		else if (_tcscmp(token, "terra_odbc_user") == 0) 
		{
			// 접속가능한 사용자 id
			token = ::_tcstok(NULL, seps );
			if (token)
                m_Terra.DbUserName = token;
			else
                return 0;
		}
		else if (_tcscmp(token, "terra_odbc_pass") == 0) 
		{
			// 비밀번호
			token = ::_tcstok(NULL, seps);
			if (token)
                m_Terra.DbUserPassword = token;
			else
                return 0;
		}
		else if (_tcscmp(token, "terra_odbc_database") == 0) 
		{
			// database 명
			token = ::_tcstok(NULL, seps );
			if (token)
                m_Terra.DbDatabase = token;
			else
                return 0;
		}
		else if (_tcscmp(token, "terra_odbc_pool_size") == 0) 
		{
			// 연결에 사용할 pool 의 size
			token = ::_tcstok(NULL, seps );
			if (token)
                m_Terra.PoolSize = ::_tstoi(token);
			else
                return 0;
		}
		else if (_tcscmp(token, "terra_odbc_response_time") == 0) 
		{
			// 쿼리 대기시간 (sec)
			token = ::_tcstok(NULL, seps );
			if (token)
                m_Terra.ResponseTime = ::_tstoi(token);
			else
                return 0;
		}

		//////////////////////////////////////////////////////////////////////
		// ODBC GSP database setting value 
		else if (_tcscmp(token, "gsp_odbc_name") == 0) 
		{
			// database server name
			token = ::_tcstok(NULL, seps );
			if (token)
                m_Gsp.DbServerName = token;
			else
                return 0;
		}
		else if (_tcscmp(token, "gsp_odbc_user") == 0) 
		{
			// 접속가능한 사용자 id
			token = ::_tcstok(NULL, seps );
			if (token)
                m_Gsp.DbUserName = token;
			else
                return 0;
		}
		else if (_tcscmp(token, "gsp_odbc_pass") == 0) 
		{
			// 비밀번호
			token = ::_tcstok(NULL, seps);
			if (token)
                m_Gsp.DbUserPassword = token;
			else
                return 0;
		}
		else if (_tcscmp(token, "gsp_odbc_database") == 0) 
		{
			// database 명
			token = ::_tcstok(NULL, seps );
			if (token)
                m_Gsp.DbDatabase = token;
			else
                return 0;
		}
		else if (_tcscmp(token, "gsp_odbc_pool_size") == 0) 
		{
			// 연결에 사용할 pool 의 size
			token = ::_tcstok(NULL, seps );
			if (token)
                m_Gsp.PoolSize = ::_tstoi(token);
			else
                return 0;
		}
		else if (_tcscmp(token, "gsp_odbc_response_time") == 0) 
		{
			// 쿼리 대기시간 (sec)
			token = ::_tcstok(NULL, seps );
			if (token)
                m_Gsp.ResponseTime = ::_tstoi(token);
			else
                return 0;
		}

		//////////////////////////////////////////////////////////////////////
		// ODBC Korea database setting value 
		else if (_tcscmp(token, "kor_odbc_name") == 0) 
		{
			// database server name
			token = ::_tcstok(NULL, seps );
			if (token)
                m_Kor.DbServerName = token;
			else
                return 0;
		}
		else if (_tcscmp(token, "kor_odbc_user") == 0) 
		{
			// 접속가능한 사용자 id
			token = ::_tcstok(NULL, seps );
			if (token)
                m_Kor.DbUserName = token;
			else
                return 0;
		}
		else if (_tcscmp(token, "kor_odbc_pass") == 0) 
		{
			// 비밀번호
			token = ::_tcstok(NULL, seps);
			if (token)
                m_Kor.DbUserPassword = token;
			else
                return 0;
		}
		else if (_tcscmp(token, "kor_odbc_database") == 0) 
		{
			// database 명
			token = ::_tcstok(NULL, seps );
			if (token)
                m_Kor.DbDatabase = token;
			else
                return 0;
		}
		else if (_tcscmp(token, "kor_odbc_pool_size") == 0) 
		{
			// 연결에 사용할 pool 의 size
			token = ::_tcstok(NULL, seps );
			if (token)
                m_Kor.PoolSize = ::_tstoi(token);
			else
                return 0;
		}
		else if (_tcscmp(token, "kor_odbc_response_time") == 0) 
		{
			// 쿼리 대기시간 (sec)
			token = ::_tcstok(NULL, seps );
			if (token)
                m_Kor.ResponseTime = ::_tstoi(token);
			else
                return 0;
		}

		//////////////////////////////////////////////////////////////////////
		// ODBC Malaysia database setting value 
		else if (_tcscmp(token, "my_odbc_name") == 0) 
		{
			// database server name
			token = ::_tcstok(NULL, seps );
			if (token)
                m_Mal.DbServerName = token;
			else
                return 0;
		}
		else if (_tcscmp(token, "my_odbc_user") == 0) 
		{
			// 접속가능한 사용자 id
			token = ::_tcstok(NULL, seps );
			if (token)
                m_Mal.DbUserName = token;
			else
                return 0;
		}
		else if (_tcscmp(token, "my_odbc_pass") == 0) 
		{
			// 비밀번호
			token = ::_tcstok(NULL, seps);
			if (token)
                m_Mal.DbUserPassword = token;
			else
                return 0;
		}
		else if (_tcscmp(token, "my_odbc_database") == 0) 
		{
			// database 명
			token = ::_tcstok(NULL, seps );
			if (token)
                m_Mal.DbDatabase = token;
			else
                return 0;
		}
		else if (_tcscmp(token, "my_odbc_pool_size") == 0) 
		{
			// 연결에 사용할 pool 의 size
			token = ::_tcstok(NULL, seps );
			if (token)
                m_Mal.PoolSize = ::_tstoi(token);
			else
                return 0;
		}
		else if (_tcscmp(token, "my_odbc_response_time") == 0) 
		{
			// 쿼리 대기시간 (sec)
			token = ::_tcstok(NULL, seps );
			if (token)
                m_Mal.ResponseTime = ::_tstoi(token);
			else
                return 0;
		}

		/////////////////////////////////////////////////////////////////////////////
		// Other server information

		// Login Server
// 		else if (_tcscmp(token, "login_server") == 0) 
// 		{
// 			// login_server login1.ran-online.co.kr 211.203.233.100 5001 6001 ran ran
// 			// login_server [server_name] [ip] [service port] [control port] [userid] [userpass]
// 			// [server_name]
// 			token = ::_tcstok(NULL, seps);
// 			if (token) StringCchCopy( m_sLOGINServer[m_nLOGINServerNumber].szServerName, SERVER_NAME_LENGTH+1, token );
// 			else       return 0;
// 			// [ip]
// 			token = ::_tcstok(NULL, seps);
// 			if (token)
// 			{
// 				StringCchCopy( m_sLOGINServer[m_nLOGINServerNumber].szServerIP, MAX_IP_LENGTH+1, token );
// 				// [string ip -> binary Internet address]
// 				unsigned long ulAddr = ::inet_addr( m_sLOGINServer[m_nLOGINServerNumber].szServerIP );
// 				if ( ulAddr==INADDR_NONE )
// 				{
// 					return 0;
// 				}
// 				else
// 				{
// 					m_sLOGINServer[m_nLOGINServerNumber].ulServerIP = ulAddr;
// 				}
// 			}
// 			else
// 			{
// 				return 0;
// 			}
// 			// [service port]
// 			token = ::_tcstok(NULL, seps);
// 			if (token) m_sLOGINServer[m_nLOGINServerNumber].nServicePort = _tstoi(token);
// 			else       return 0;
// 			// [control port]
// 			token = ::_tcstok(NULL, seps);
// 			if (token) m_sLOGINServer[m_nLOGINServerNumber].nControlPort = _tstoi(token);
// 			else       return 0;
// 			// [userid]
// 			token = ::_tcstok(NULL, seps);
// 			if (token) StringCchCopy(m_sLOGINServer[m_nLOGINServerNumber].szUserID, USR_ID_LENGTH+1, token);
// 			else       return 0;
// 			// [userpass]
// 			token = ::_tcstok(NULL, seps);
// 			if (token) StringCchCopy(m_sLOGINServer[m_nLOGINServerNumber].szUserPass, USR_PASS_LENGTH+1, token);
// 			else       return 0;
// 			m_nLOGINServerNumber++;
// 			return 0;
// 		}
		///////////////////////////////////////////////////////////////////////
		// Session Server
		else if ( _tcscmp(token, "session_server") == 0 )
		{
			// session_server [server_name] [ip] [port] [userid] [password]
			// server_name
			token = ::_tcstok(NULL, seps);
			if (token) StringCchCopy(m_sSESSIONServer[m_nSESSIONServerNumber].szServerName, SERVER_NAME_LENGTH+1, token);
			else       return 0;
			// ip
			token = ::_tcstok(NULL, seps);
			if (token)
			{
				StringCchCopy( m_sSESSIONServer[m_nSESSIONServerNumber].szServerIP, MAX_IP_LENGTH+1, token );
				unsigned long ulAddr = ::inet_addr( m_sSESSIONServer[m_nSESSIONServerNumber].szServerIP );
				if ( ulAddr==INADDR_NONE )
				{
					return 0;
				}
				else
				{
					m_sSESSIONServer[m_nSESSIONServerNumber].ulServerIP = ulAddr;
				}
			}
			else
			{
				return 0;
			}
			// port
			token = ::_tcstok(NULL, seps);
			if (token) m_sSESSIONServer[m_nSESSIONServerNumber].nServicePort = _tstoi(token);
			else       return 0;
			// userid
			token = ::_tcstok(NULL, seps);
			if (token) StringCchCopy(m_sSESSIONServer[m_nSESSIONServerNumber].szUserID, USR_ID_LENGTH+1, token);
			else       return 0;
			// password
			token = ::_tcstok(NULL, seps);
			if (token) StringCchCopy(m_sSESSIONServer[m_nSESSIONServerNumber].szUserPass, USR_PASS_LENGTH+1, token);
			else       return 0;
			m_nSESSIONServerNumber++;
			return 0;
		}
		///////////////////////////////////////////////////////////////////////
		// Channel
		else if (_tcscmp(token, "channel") == 0)
		{
			// channel [nubmer] [pk on/off]
			int nChannelPK = 1;
			int nPK;

			token = ::_tcstok(NULL, seps);
			
			if (token) nChannelPK = ::_tstoi(token);
			else return 0;

			if (nChannelPK < 0 || nChannelPK >= MAX_CHANNEL_NUMBER)
                return 0;

			token = ::_tcstok(NULL, seps);

			if (token)
                nPK = ::_tstoi(token);
			else
                return 0;

			if (nPK == 1)
                m_sChannelInfo[nChannelPK].bPK = true;
			else
                m_sChannelInfo[nChannelPK].bPK = false;
		}
		///////////////////////////////////////////////////////////////////////
		// Agent Server
		else if (_tcscmp(token, "agent_server") == 0) 
		{			
			// agent_server [server_name] [ip]
			// server_name			 
			token = ::_tcstok(NULL, seps);
			if ( !token )
			{
				return 0;
			}
			// ip
			token = ::_tcstok(NULL, seps);
			if (token) StringCchCopy( m_szAgentIP, MAX_IP_LENGTH+1, token );
			else return 0;
		}
		///////////////////////////////////////////////////////////////////////
		// Field Server		
		else if (_tcscmp(token, "field_server") == 0) 
		{				
			// field_server [Channel] [serverID] [server_name] [ip] [port]			
			int nServerID = 0;
			int nChannel  = 0;

			// [Channel]
			token = ::_tcstok(NULL, seps);
			if (token)	nChannel = _tstoi(token);
			else 		return 0;

			if (nChannel >= MAX_CHANNEL_NUMBER)
			{
				return 0;
			}
			
			// [Server id]
			token = ::_tcstok(NULL, seps);
			if (token)	nServerID = _tstoi(token);
			else 		return 0;

			if (nServerID >= FIELDSERVER_MAX)
			{
				return 0;
			}
			
			// 0 번 채널의 필드서버 갯수를 세팅해 놓는다.
            if ( 0 == nChannel )
			{
				if ( nServerID > m_nMaxFieldNumber )
				{
					m_nMaxFieldNumber = nServerID;
				}
			}

			F_SERVER_INFO sFieldSVR;

			// [server name]
			token = ::_tcstok(NULL, seps);
			if (token) 	StringCchCopy(sFieldSVR.szServerName, SERVER_NAME_LENGTH+1, token);
			else 		return 0;

			// [ip]
			token = ::_tcstok(NULL, seps);
			if (token) StringCchCopy(sFieldSVR.szPublicIp, MAX_IP_LENGTH+1, token);
			else return 0;

			// [string ip -> binary Internet address]
			unsigned long ulAddr = ::inet_addr( sFieldSVR.szPublicIp );
			if ( ulAddr==INADDR_NONE )
			{
				return 0;
			}
			else
			{
				sFieldSVR.ulPublicIp = ulAddr;
			}

			// [port]
			token = ::_tcstok(NULL, seps);
			if (token) {
				sFieldSVR.nServicePort = _tstoi(token);
			} else {
				return 0;
			}			

			for ( int i=0; i<FIELDSERVER_MAX; i++ )
			{
				if ( (m_sFIELDServer[nChannel][i].ulPublicIp == sFieldSVR.ulPublicIp) &&
					 (m_sFIELDServer[nChannel][i].nServicePort == sFieldSVR.nServicePort) )
				{
					MessageBox(
						NULL,
						"field_server [Channel] [serverID] [server_name] [ip] [port]\r\n"
						"Same IP/PORT Field Server Detected",
						"ERROR",
						MB_OK );
					return 0;
				}
			}

			m_sFIELDServer[nChannel][nServerID] = sFieldSVR;

			return 0;
		}

		// Get next token
		token = ::_tcstok( NULL, seps );
	}
	return 0;
}	

F_SERVER_INFO* ServerCfg::GetFieldServer(int nServerID, int nServerChannel)
{
	if ( nServerID < 0 || 
		 nServerID >= FIELDSERVER_MAX ||
		 nServerChannel < 0 ||
		 nServerChannel >= MAX_CHANNEL_NUMBER)
	{
		return NULL;
	}
	else
	{
		return &m_sFIELDServer[nServerChannel][nServerID];
	}
}

F_SERVER_INFO* ServerCfg::GetFieldServerArray()
{ 
    return &m_sFIELDServer[0][0];
}

char* ServerCfg::GetSessionServerIP()
{ 
    return &m_sSESSIONServer[0].szServerIP[0];
}

int	ServerCfg::GetSessionServerPort() const
{ 
    return m_sSESSIONServer[0].nServicePort;
}

//-- 서버점검시 로그인서버에 접속할 수 있는 IP목록
// int ServerCfg::GetAllowIPType1Size() const
// { 
//     return int (m_AllowIPType1.size());
// }

// int ServerCfg::GetAllowIPType2Size() const
// { 
//     return int (m_AllowIPType2.size());
// }

// int ServerCfg::GetAllowIPType3Size() const
// { 
//     return int (m_AllowIPType3.size());
// }

bool ServerCfg::IsAllowIp(const std::string& strAllowIP) const
{ 
    MAP_ALLOW_IP_CITER iter = m_AllowIPType1.find(strAllowIP);
    if (iter != m_AllowIPType1.end())
        return true;
    else
        return false;
}

// bool ServerCfg::IsAllowIPType2(const std::string& strAllowIP) const
// { 
//     MAP_ALLOW_IP_CITER iter = m_AllowIPType2.find(strAllowIP);
//     if (iter != m_AllowIPType2.end())
//         return true;
//     else
//         return false;
// }
// 
// bool ServerCfg::IsAllowIPType3(const std::string& strAllowIP) const
// { 
//     MAP_ALLOW_IP_CITER iter = m_AllowIPType3.find(strAllowIP);
//     if (iter != m_AllowIPType3.end())
//         return true;
//     else
//         return false;
// }

bool ServerCfg::IsPkChannel(int nChannel) const
{
	if (nChannel < 0 || nChannel >= MAX_CHANNEL_NUMBER)
		return true;
    else
	    return m_sChannelInfo[nChannel].bPK;
}
