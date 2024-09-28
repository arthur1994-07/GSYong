#include "../pch.h"
#include <process.h>
#include "../../SigmaCore/Net/MsgList.h"
#include "../../SigmaCore/Net/NetUtil.h"
#include "../../SigmaCore/Util/SystemInfo.h"
#include "../Database/Odbc/s_COdbcManager.h"
#include "../Util/IpAddressChecker.h"
#include "./ServerConfigXml.h"
#include "./NetServer.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


NetServer::NetServer(HWND hWnd, HWND hEditBox, HWND hEditBoxInfo, SUBPATH* pPath, EMSERVICE_PROVIDER ServiceProvider)
	: m_pPath(pPath)
    , m_ServiceProvider(ServiceProvider)
    , m_hWnd                 (hWnd)
	, m_hEditBox             (hEditBox)
	, m_hEditBoxInfo         (hEditBoxInfo)
	, m_hIOServer		     (NULL)

	, m_hAcceptThread        (NULL)
	, m_hUpdateThread	     (NULL)

	, m_hUpdateQuitEvent     (INVALID_HANDLE_VALUE)
	, m_dwUpdateWaitTime     (0)
	, m_dwUpdateOldTime      (0)
	, m_dwUpdateFrame        (10)

	, m_pRecvMsgManager      (NULL)
	, m_sServer		         (INVALID_SOCKET) // Server Socket	
	, m_nStatus		         (S_SERVER_STOP)

	, m_dwCompKey		     (0)
	, m_nPort			     (0)	 // Server port
	, m_PatchVersion	     (0)     // Server version

	, m_nServerGroup	     (0)     // Server group	
	//, m_ServiceProvider     (SP_OFFICE_TEST)

	, m_dwLastDropClient     (0)
	, m_bIsRunning	         (false)
	, m_bUpdateEnd	         (false)
	, m_bTestServer	         (false)
	, m_bAdult		         (false)
	, m_bPK                  (true)
	, m_bPKLess              (false)	

	, m_bUseEventThread		 (false)
	, m_nFPS				 (10)

	, m_dwWorkerThreadNumber (2) // Worker Thread 의 갯수
	, m_bGameGuardInit       (false) // nProtect GameGuard 초기화 성공여부
	, m_bGameGuardAuth       (false) // nProtect GameGuard 인증 사용여부	
	, m_dwCpuCount           (0)
    , m_LanguageSet(language::DEFAULT)
	, m_dwHeartBeatTime(0)
	, m_dwFrameTime(0)
	, m_dwFrameCount(0)
	, m_bAllowIP( false )

	, m_pDbManager(NULL)	//mjeon.dbman
	, m_pAdoManager(NULL)
{	
	// Create Lzo compress library
    sc::CMinLzo::GetInstance().init();
	
	::SecureZeroMemory( m_szAddress, sizeof(char) * (MAX_IP_LENGTH+1) );

	// Set work thread handle to invaild 
	for (int i=0; i < MAX_WORKER_THREAD; i++)
	{
		m_hWorkerThread[i] = INVALID_HANDLE_VALUE;
	}

	sc::net::initializeNetwork();

	::SecureZeroMemory( m_szEncrypt, sizeof( TCHAR ) * (ENCRYPT_KEY+1) );

    m_pAddressChecker = new IpAddressChecker;
}

NetServer::~NetServer()
{
	// Message Queue 종료
	SAFE_DELETE(m_pRecvMsgManager);
	
	SAFE_DELETE(m_pDbManager);

	// [2014.08.25] 메모리 Leak 제거 중 의문점이 생겨서 잠시 보류
	SAFE_DELETE(m_pAdoManager);
	
	CWhiteRock::GetInstance()->ReleaseInstance();
	
	//mjeon
	sc::LogMan::releaseInstance();

	// Clear Winsock2
	sc::net::closeNetwork();
	// Clear Address Checker
	//SERVER_UTIL::CAddressChecker::GetInstance()->ReleaseInstance();

    delete m_pAddressChecker;
    m_pAddressChecker = NULL;
}

// Create CFG file loader and open cfg file
int NetServer::StartCfg(ServerConfigXml* pConfig)
{	
	m_PatchVersion		 = pConfig->GetPatchVersion();	// 버전저장
	m_LauncherVersion    = pConfig->GetLauncherVersion();	// 패치프로그램 버전
    m_PatchCutVersion         = pConfig->GetPatchCutVersion();	// 패치되야할 클라이언트와의 버전 차이
	
	m_nServerGroup	   = pConfig->GetServerGroup();		// 서버 그룹
	m_nServerNum       = pConfig->GetServerNumber();    // 서버 번호
	m_nMaxClient	   = pConfig->GetServerMaxClient(); // 최대 접속자수
	
	// 서버 최대 인원이 올바른지 체크한다.
	if (m_nMaxClient <= 130)
	{		
		sc::writeLogError(std::string("MaxClient > 130"));
		return NET_ERROR;
	}
	
	m_nMaxChannelUser  = pConfig->GetServerChannelMaxClient(); // 채널당 최대 접속자
	
	m_nPort			   = pConfig->GetServicePort();		// 포트 저장	
	
	m_dwCompKey		   = (DWORD) ((m_nMaxClient * 2) + 1);          // Completion Key
	
	m_bTestServer      = pConfig->IsTestServer();	
	m_ServiceProvider  = pConfig->GetServiceProvider(); // 서비스제공업자
	m_bAdult           = pConfig->AdultCheck();         // Server for adult ?
	m_bPK              = pConfig->IsPKServer();         // PK On, Off
	m_bPKLess          = pConfig->IsPKLessServer();     // All attack between character is off

	m_bUseEventThread  = ( pConfig->IsUserEventThread() == 0 ) ? false : true;

	m_nFPS			   = pConfig->GetFPS();

	m_LanguageSet = pConfig->GetLangSet();

	StringCchCopy(m_szAddress, MAX_IP_LENGTH+1, pConfig->GetServerIP());
	
	sc::writeLogInfo(std::string(">Start Configuring the Server..."));

	switch (m_ServiceProvider)
	{
	case SP_KOREA :
		sc::writeLogInfo(std::string("Service Provider:KOREA"));
		break;
	case SP_TAIWAN :
		sc::writeLogInfo(std::string("Service Provider:TAIWAN"));
		break;
	case SP_CHINA :
		sc::writeLogInfo(std::string("Service Provider:CHINA"));
		break;
	case SP_JAPAN :
		sc::writeLogInfo(std::string("Service Provider:JAPAN"));
		break;
	case SP_PHILIPPINES :
		sc::writeLogInfo(std::string("Service Provider:Philippines/Vietnam"));
		break;
	case SP_THAILAND :
		sc::writeLogInfo(std::string("Service Provider:THAILAND"));
		break;
	case SP_GLOBAL:
		sc::writeLogInfo(std::string("Service Provider:Global Service Platform"));
		break;
	case SP_MALAYSIA :
		sc::writeLogInfo(std::string("Service Provider:MALAYSIA"));
		break;
	case SP_GS :
		sc::writeLogInfo(std::string("Service Provider:Global Service"));
		break;
	case SP_INDONESIA:
		sc::writeLogInfo(std::string("Service Provider:Indonesia Service"));
		break;
    case SP_WORLD_BATTLE:
        sc::writeLogInfo(std::string("Service Provider:World Battle"));
        break;
	case SP_EU:
		sc::writeLogInfo(std::string("Service Provider:Games-Masters"));
		break;
	case SP_US:
		sc::writeLogInfo(std::string("Service Provider:GameSamba"));
		break;
	default:
		sc::writeLogInfo(std::string("Service Provider:MINCOMS KOREA"));
		break;
	}

	/*if ( CheckLowFragmentationHeap() )
	{
		sc::writeLogInfo( std::string( "Low Fragmentation Heap has been enabled." ) );
	}
	else
	{
		sc::writeLogWarn( std::string( "Low Fragmentation Heap error." ) );
	}*/

	return NET_OK;
}

// Create message queue
int NetServer::StartMsgManager(int MaxClient)
{
	if (!m_pRecvMsgManager)
	{
        m_pRecvMsgManager = new sc::net::MsgManager(MaxClient * 3);
		return NET_OK;
	}
	else
	{
		m_bIsRunning = false;
		return NET_ERROR;
	}
}

// Create I/O completion port
int NetServer::StartIOCP()
{
	if (!m_hIOServer)
		m_hIOServer = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	
	if (!m_hIOServer) 
	{
		sc::writeLogError(std::string("CreateIoCompletionPort Error"));
		m_bIsRunning = false;
		return NET_ERROR;
	} 
	else 
	{
		sc::writeLogInfo(std::string("CreateIoCompletionPort OK"));
		return NET_OK;
	}
}

// Create IOCP Work Thread
int NetServer::StartWorkThread(ServerConfigXml* pConfig)
{	
	// Determine how many processors are on the system
	// Calcurate number of threads	
	sc::writeLogInfo(sc::string::format("%1%", sc::getOSName()));	
    sc::writeLogInfo(sc::string::format("%1% %2% Mhz", sc::getProcessorName(), sc::calculateCpuSpeed()));	
    sc::writeLogInfo(sc::string::format("%1% System Processor Detected", sc::getProcessorNumber()));
	
	// Work Thread 갯수 결정
	// CPU 갯수 X 2 가 기본값이다.
	m_dwCpuCount = sc::getProcessorNumber();
	m_dwWorkerThreadNumber = m_dwCpuCount * S_HEURISTIC_NUM; // Processor * 2
	
	// 설정파일의 최대값 보다 크면 설정파일에 적힌 최대값으로 제한한다.
	if (m_dwWorkerThreadNumber > (DWORD) pConfig->GetMaxThread())
		m_dwWorkerThreadNumber = (DWORD) pConfig->GetMaxThread();

	// 미리 정의된 최대값보다 크면 프로그램의 정의된 최대값으로 제한한다.
	if (m_dwWorkerThreadNumber > MAX_WORKER_THREAD)
		m_dwWorkerThreadNumber = MAX_WORKER_THREAD;

	// Creaate worker threads based on the number of processors	
	DWORD dwThreadId;
	
	sc::writeLogInfo( sc::string::format("%d Worker Thread", m_dwWorkerThreadNumber));
	
	for (DWORD dwCPU=0; dwCPU < m_dwWorkerThreadNumber; dwCPU++)
    { 		
		HANDLE hThread;
		
		hThread = (HANDLE) ::_beginthreadex(
								NULL, 
								0, 
								CServerWorkProc,
								this, 
								0, 
								(unsigned int*) &dwThreadId);
		if (hThread == NULL)  
		{
			sc::writeLogError( sc::string::format("%d Worker Thread Create Error code : %d", dwCPU+1, GetLastError()) );
			return NET_ERROR;
        } 
		else 
		{
            sc::writeLogInfo( sc::string::format("#%1% Worker Thread Create OK", dwCPU+1));
			// The system schedules threads on their preferred processors whenever possible.
			::SetThreadIdealProcessor(hThread, dwCPU % m_dwCpuCount);
        }		
		// store thread handle
		m_hWorkerThread[dwCPU] = hThread;
		::CloseHandle(hThread);
	} 
	return NET_OK;
}


void NetServer::setUpdateFrame( DWORD dwFrame )
{
	m_dwUpdateFrame = dwFrame;
	m_dwUpdateWaitTime = 1000/m_dwUpdateFrame;
}

// Start game update thread	
int	NetServer::StartUpdateThread()
{
	SAFE_CLOSE_HANDLE( m_hUpdateQuitEvent );

	if( m_bUseEventThread )	m_hUpdateQuitEvent = ::CreateEvent( NULL, TRUE, FALSE, NULL );

	DWORD	dwThreadId = 0;	

	m_hUpdateThread = (HANDLE) ::_beginthreadex(
		NULL,
		0,
		CServerUpdateProc,
		this,
		CREATE_SUSPENDED,
		(unsigned int*) &dwThreadId );

	if ( m_hUpdateThread == NULL )
	{	
		sc::writeLogError( sc::string::format("Server Update Thread Create Failed Error code : %d", GetLastError()) );
		m_bIsRunning = false;

		SAFE_CLOSE_HANDLE( m_hUpdateQuitEvent );

		return NET_ERROR;
	} 
	else 
	{
		if (m_dwCpuCount > 2)
		{
			// Update Thread 의 수행레벨을 높여 성능을 향상시킨다.
			// 서버의 CPU 갯수가 2개 이상일때만 실제로 효과가 있었다.
			::SetThreadPriority(m_hUpdateThread, THREAD_PRIORITY_HIGHEST);
		}
		sc::writeLogInfo(std::string("Server Update Thread Create OK"));

		if( m_bUseEventThread )
		{
			setUpdateFrame( m_nFPS );
			m_dwUpdateOldTime = ::GetTickCount();
		}

		::ResumeThread( m_hUpdateThread );

		return NET_OK;
	}
}

int	NetServer::StartListenThread()
{
	int nRetCode;
	DWORD	dwThreadId;
	///////////////////////////////////////////////////////////////////////////
	// Prepare server socket for TCP
	m_sServer = ::WSASocket(
					PF_INET,
					SOCK_STREAM,
					IPPROTO_TCP,
					NULL,
					0,
					WSA_FLAG_OVERLAPPED );
	if(m_sServer == SOCKET_ERROR) 
	{
		int errCode = WSAGetLastError();
        sc::writeLogError(sc::string::format("StartListenThread WSASocket failed. %1%", sc::net::getLastError( errCode )));
		::WSACleanup();
		m_bIsRunning = false;
		return NET_ERROR;
	} 
	else 
	{
		sc::writeLogInfo(std::string("WSASocket OK"));
	}
		
    //////////////////////////////////////////////////////////////////////////////
    // Server 의 ip Address 를 얻는다.
    char szHostName[128] = {0};	
    if (::gethostname(szHostName, 128) == 0 )
    {
        sc::writeLogInfo(sc::string::format("Host Name:%1%", szHostName));
        // Get host adresses
        struct hostent * pHost;
        int i; 
        pHost = ::gethostbyname(szHostName); 
        for (i=0; pHost && pHost->h_addr_list[i]; i++ )
        {
            char szIP[MAX_IP_LENGTH+1] = "";
            int j; 
            for (j=0; j<pHost->h_length; j++)
            {
                char addr[MAX_IP_LENGTH+1] = {0};
                if (j>0)
                    ::StringCchCat(szIP, MAX_IP_LENGTH, "."); 
                ::StringCchPrintf(addr, MAX_IP_LENGTH, _T("%u"), (unsigned int)((unsigned char*) pHost->h_addr_list[i])[j]); 				
                ::StringCchCat(szIP, MAX_IP_LENGTH, addr);
            }
            sc::writeLogInfo(sc::string::format("Host IP:%1%", szIP));
        }
    }


	//
	//mjeon
	//
	// SO_EXCLUSIVEADDRUSE is set to prevent server hijacking.
	// It is possible to bind another socket to same port already in use with SO_REUSEADDR
	//  if there does not exist SO_EXCLUSIVEADDRUSE for server's socket.
	//
	BOOL val = TRUE;
	if( setsockopt(m_sServer, SOL_SOCKET, 
					SO_EXCLUSIVEADDRUSE, 
					(char*)&val, sizeof(val)) != 0 )
	{
		int errCode = WSAGetLastError();
		sc::writeLogError(sc::string::format("setsockopt Error. Error %1%", sc::net::getLastError( errCode )));
		closesocket(m_sServer);
		::WSACleanup();
		m_bIsRunning = false;
		return NET_ERROR;
	}


	//////////////////////////////////////////////////////////////////////////////
	// bind socket
	// 바인드될 로칼어드레스, 2개 이상의 랜카드일때..
	// 주의 : Ipv4 에 대응, ipv6 으로 변경시 코딩변경 필수~!
    // The inet_addr function converts a string 
    // containing an (Ipv4) Internet Protocol dotted address 
    // into a proper address for the IN_ADDR structure.
	struct sockaddr_in local;
	local.sin_addr.s_addr = ::htonl(INADDR_ANY);	
	local.sin_family = AF_INET;
	local.sin_port = ::htons(m_nPort);

	nRetCode = ::bind(m_sServer, (struct sockaddr*) &local, sizeof(local));
	if (nRetCode == SOCKET_ERROR) 
	{
		int errCode = WSAGetLastError();
		sc::writeLogError(
            sc::string::format(
                "bind Error. Error %1%",
                sc::net::getLastError( errCode )));
		::closesocket(m_sServer);
		::WSACleanup();
		m_bIsRunning = false;
		return NET_ERROR;
	} 
	else 
	{
		sc::writeLogInfo(sc::string::format("Bind Port %1%", m_nPort));
	}

	// Prepare socket for listening
	nRetCode = ::listen(m_sServer, SOMAXCONN/*NET_CLIENT_LISTEN*/);
	if (nRetCode == SOCKET_ERROR) 
	{
		int errCode = WSAGetLastError();
        sc::writeLogError(sc::string::format("Listen error. %1%", sc::net::getLastError( errCode )));
		::closesocket(m_sServer);
		::WSACleanup();
		m_bIsRunning = false;
		return NET_ERROR;
	}	
	
	//////////////////////////////////////////////////////////////////////////////
	// Create Listen (Accept) Thread	
	m_hAcceptThread = (HANDLE) ::_beginthreadex(
									NULL, 
									0, 
									CServerListenProc,
									this,
									0,
									(unsigned int*) &dwThreadId );


	if (m_hAcceptThread == NULL)  
	{
		sc::writeLogError( sc::string::format("Server Accept Thread Create Failed %d", GetLastError()));
		m_bIsRunning = false;
		return NET_ERROR;
	} 
	else 
	{
		sc::writeLogInfo(std::string("Server Accept Thread Create OK"));
		return NET_OK;
	}
}

int	NetServer::StopListenThread()
{	
	// Close socket, will close Listen Thread	
	int nRetCode = ::closesocket(m_sServer);
	
	CloseHandle( m_hAcceptThread );
	m_hAcceptThread = NULL;

	if (nRetCode == SOCKET_ERROR)
	{
		TRACE("StopListenThread closesocket ERROR \n");
		return NET_ERROR;
	}
	else
	{
		return NET_OK;
	}
}

// Stop Work Thread	
int NetServer::StopWorkThread()
{
	for (DWORD dwNum=0; dwNum<m_dwWorkerThreadNumber; dwNum++)
	{
		::PostQueuedCompletionStatus(m_hIOServer, 
							0, 
							m_dwCompKey, 
							NULL);
	}
	// Wait until all worker thread exit...	
	Sleep( 1000 );
	return NET_OK;
}

int	NetServer::StopIOCP()
{	
	DWORD dwExitCode;
	if (m_hIOServer) 
	{
		dwExitCode = ::WaitForSingleObject(m_hIOServer, 2000);
		if (dwExitCode == WAIT_FAILED)
			::CloseHandle(m_hIOServer);
		m_hIOServer = NULL;
	}
	return NET_OK;
}

void NetServer::PrintSystemInfo()
{
	static DWORD dwTickPrev = 0;

	DWORD dwTickNow = GetTickCount();

	if ((dwTickNow - dwTickPrev) >=	FREQUENCY_UPDATE_SYSTEMINFO)
	{
		dwTickPrev = dwTickNow;

		DWORD dwPhysicalMemAvailMB		= static_cast<DWORD> (sc::getMemoryPhysicalAvail() / 1048576); // 1048576 = 1024 * 1024
		DWORD dwPhysicalMemUsePercent	= sc::getMemoryUsagePercent();
        DWORD dwProcessWorkset          = sc::getMemoryWorkingSet();

        if (dwProcessWorkset > 1900) // 프로그램이 1.5G 이상의 메모리를 사용한다.
            sc::writeLogError(sc::string::format("Current WorkSet size of this process is: %1% (MB)", dwProcessWorkset));
        else if (dwProcessWorkset > 1500)
            sc::writeLogWarn(sc::string::format("Current WorkSet size of this process is: %1% (MB)", dwProcessWorkset));
        else
		    sc::writeLogInfo(sc::string::format("Current WorkSet size of this process is: %1% (MB)", dwProcessWorkset));

		sc::writeLogInfo(sc::string::format("Available Physical Memory is: %1% (MB), %2%%%", dwPhysicalMemAvailMB, (100 - dwPhysicalMemUsePercent)));
        
		if (dwPhysicalMemAvailMB <= 100)	//여유 물리 메모리가 100MB 이하
			sc::writeLogWarn(sc::string::format("More memory is required: only %1% (MB) is available !!", dwPhysicalMemAvailMB));

		if (dwPhysicalMemUsePercent >= 90)	//물리 메모리 사용량이 90% 이상
			sc::writeLogWarn(sc::string::format("More memory is required: %1%%% of physical memory is in use !!", dwPhysicalMemUsePercent));
	}
}

BOOL NetServer::CheckLowFragmentationHeap()
{
	/*ULONG heapFragValue = 2;
	if ( HeapSetInformation( GetProcessHeap(), HeapCompatibilityInformation, &heapFragValue, sizeof( heapFragValue ) ) )
	{
		return TRUE;
	}

	return FALSE;

	UINT nSuccessCount = 0, nFailCount = 0;
	HANDLE hHeap[1025];
	UINT uEA = GetProcessHeaps( 1024, hHeap );

	for ( UINT i=0; i<uEA; ++i )
	{
		ULONG HeapFragValue = 2;
		if ( HeapSetInformation( hHeap[i], HeapCompatibilityInformation, &HeapFragValue, sizeof( HeapFragValue ) ) )
		{
			nSuccessCount++;
		}
		else
		{
			nFailCount++;
			sc::writeLogWarn( sc::string::format( "Low Fragmentation Heap set fail. i: %1%, code: %2%",  i, GetLastError() ) );
		}
	}

	return nSuccessCount > nFailCount ? TRUE : FALSE;*/
	return TRUE;
}

int	NetServer::StartRegularScheduleThread()
{
	SAFE_CLOSE_HANDLE( m_hRegularScheduleQuitEvent );

	if( m_hRegularScheduleThread )
	{
		m_hRegularScheduleQuitEvent = ::CreateEvent( NULL, TRUE, FALSE, NULL );
	}

	DWORD	dwThreadId = 0;	

	m_hRegularScheduleThread = (HANDLE) ::_beginthreadex(
		NULL,
		0,
		CServerRegularScheduleProc,
		this,
		CREATE_SUSPENDED,
		(unsigned int*) &dwThreadId );

	if ( m_hRegularScheduleThread == NULL )
	{	
		sc::writeLogError( sc::string::format("Server RegularSchedule Thread Create Failed Error code : %d", GetLastError()) );
		m_bIsRunning = false;

		SAFE_CLOSE_HANDLE( m_hRegularScheduleQuitEvent );

		return NET_ERROR;
	} 
	else 
	{
		sc::writeLogInfo(std::string("Server RegularSchedule Thread Create OK"));

		::ResumeThread( m_hRegularScheduleThread );

		return NET_OK;
	}
}