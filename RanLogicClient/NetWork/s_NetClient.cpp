#include "../pch.h"
#include <process.h>
#include "../../SigmaCore/Net/NetUtil.h"
#include "../../SigmaCore/Math/Random.h"
#include "../../SigmaCore/String/StringFormat.h"
#include "../../SigmaCore/Log/LogMan.h"
#include "../../EngineLib/DxTools/DebugSet.h"
#include "../../RanLogic/Util/s_CClientConsoleMsg.h"
#include "../../RanLogic/MSG/GLContrlMsg.h"
#include "../../BugTrap/BugTrap.h"

#include "./s_NetClient.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

#include "s_NetLogPacket.h"

//extern const char* GARBAGE_DATA[];

char* g_szClientVerFile		= "cVer.bin";
Packet_Log_Declare();

static unsigned int WINAPI NetThread(void* p)
{
	CNetClient* pNet = (CNetClient*) p;
	pNet->NetThreadProc();
	return 0;
}

CNetClient::CNetClient() 
	: m_pGlobalStage(NULL)
	, m_bThreadProc(FALSE)
	, m_hThread(NULL)
	, m_bKeyReceived(FALSE)
	, m_pRcvManager(NULL)
    //, m_pAes(NULL)
    //, m_bAesKeyReceived(false)
{
	::InitializeCriticalSectionAndSpinCount(&m_CriticalSection, sc::CRITICAL_SECTION_SPIN_COUNT); 
	
	// Create Lzo compress library
    sc::CMinLzo::GetInstance().init();

	m_hEventWork = INVALID_HANDLE_VALUE;
	m_hEventKill = INVALID_HANDLE_VALUE;
	m_pSndBuffer = NULL;
	// m_pRcvBuffer = NULL;
	// m_pMsgBuffer = NULL;
	m_pNetField	 = NULL;
	m_pNetBoard	 = NULL;
	m_dwGaeaID	 = GAEAID_NULL;
	m_nRandomNumber = 0;

	::memset(m_cClientIP, 0, sizeof(char) * (MAX_IP_LENGTH+1));
	::memset(m_cServerIP, 0, sizeof(char) * (MAX_IP_LENGTH+1));	
	::memset(m_szAppPath, 0 , 1024);

	SetDefault();

	::memset( m_szEncryptKey, 0, sizeof( char ) * (ENCRYPT_KEY+1) );
	::memset( m_szEncryptKeyHeart, 0, sizeof( char ) * (ENCRYPT_KEY+1) );
	
	Packet_Log_Init();
}

CNetClient::CNetClient(HWND hWnd)
    //: m_pAes(NULL)
    //, m_bAesKeyReceived(false)
{	
	::InitializeCriticalSectionAndSpinCount(&m_CriticalSection, sc::CRITICAL_SECTION_SPIN_COUNT); 

	m_hEventWork = INVALID_HANDLE_VALUE;
	m_hEventKill = INVALID_HANDLE_VALUE;
	m_pSndBuffer = NULL;
	m_pRcvManager = NULL;
	// m_pRcvBuffer = NULL;
	// m_pMsgBuffer = NULL;
	m_hThread	 = NULL;
	m_pNetField	 = NULL;
	m_pNetBoard	 = NULL;
	m_dwGaeaID	 = GAEAID_NULL;

	m_nRandomNumber = 0;

	SetDefault();
	SetWndHandle(hWnd);
}

CNetClient::~CNetClient()
{	
	if (m_pNetField)
	{
		if (m_pNetField->IsOnline())
			m_pNetField->CloseConnect();
		delete m_pNetField;
		m_pNetField = NULL;
	}
	
	if (m_pNetBoard)
	{
		if (m_pNetBoard->IsOnline())
			m_pNetBoard->CloseConnect();
		delete m_pNetBoard;
		m_pNetBoard = NULL;
	}
	
	CloseConnect();

	SAFE_DELETE_ARRAY(m_pSndBuffer);
	// SAFE_DELETE_ARRAY(m_pRcvBuffer);
	// SAFE_DELETE_ARRAY(m_pMsgBuffer);

	SAFE_DELETE(m_pRcvManager);

    //delete m_pAes;
    //m_pAes = NULL;

	::DeleteCriticalSection(&m_CriticalSection);

	CloseHandle(m_hEventWork);
	CloseHandle(m_hEventKill);

	Packet_Log_Destroy();
}

void CNetClient::SetDefault()
{
	int i,j;

	m_nOnline = net::OFFLINE;
	m_sClient = INVALID_SOCKET;
	m_hWnd = NULL;	

	for (i=0; i<MAX_SERVER_GROUP; i++)
	{
		for (j=0; j<MAX_SERVER_NUMBER; j++)
		{
			::SecureZeroMemory(&m_sGame[i][j], sizeof(G_SERVER_CUR_INFO_LOGIN));
		}
	}

	SAFE_DELETE(m_pRcvManager);
	m_pRcvManager = new CRcvMsgBuffer();

	SAFE_DELETE_ARRAY(m_pSndBuffer);
	// SAFE_DELETE_ARRAY(m_pRcvBuffer);
	// SAFE_DELETE_ARRAY(m_pMsgBuffer);

	m_pSndBuffer = new char[NET_DATA_CLIENT_MSG_BUFSIZE];
	::memset(m_pSndBuffer, 0, NET_DATA_CLIENT_MSG_BUFSIZE);

	// m_pRcvBuffer = new char[NET_DATA_CLIENT_MSG_BUFSIZE];
	// m_pMsgBuffer = new char[NET_DATA_BUFSIZE];

	
	// ::memset(m_pRcvBuffer, 0, NET_DATA_CLIENT_MSG_BUFSIZE);
	// ::memset(m_pMsgBuffer, 0, NET_DATA_BUFSIZE);
	
	// m_nRcvBytes				= 0;
	m_nSndBytes				= 0;
	m_nTotalBytes			= sizeof(NET_MSG_GENERIC);	
	m_nNetMode				= NET_PACKET_HEAD;	
	//m_ck.nKeyDirection		= 1;	// 암호키 방향
	//m_ck.nKey				= 1;	// 암호키 shift 량
	m_nRandomNumber         = 0;
	m_PatchVersion		= -1;
	m_LauncherVersion		= -1;
	m_bGameServerInfoEnd	= FALSE;
	m_nLoginResult			= 0; // 로그인 결과 세팅
	m_hEventWork			= ::CreateEvent(0, FALSE, FALSE, 0);
	m_hEventKill			= ::CreateEvent(0, FALSE, FALSE, 0);
	m_dwGaeaID				= GAEAID_NULL;
	m_bKeyReceived			= FALSE;

    //delete m_pAes;
    //m_pAes = new sc::Aes;
//    m_bAesKeyReceived = false;

//	ZeroMemory( m_szSendGarbageMsg[0], 12 );
//	ZeroMemory( m_szSendGarbageMsg[1], 12 );
//	ZeroMemory( m_szSendGarbageMsg[2], 12 );
//	m_dwGarbageNum = 0;

	m_bWorldBattle = FALSE;
	m_bWorldBattleSvrInfoEnd = FALSE;
	m_bWorldBattleCharSelectEnd = FALSE;

	SetAppPath();
}

// void CNetClient::LockOn()
// {
// 	::EnterCriticalSection(&m_CriticalSection);
// }
// 
// void CNetClient::LockOff()
// {
// 	::LeaveCriticalSection(&m_CriticalSection);
// }

bool CNetClient::IsOnline()
{
	return m_nOnline == net::ONLINE ? true : false;
}

bool CNetClient::IsOnlineField()
{
	if (m_pNetField != NULL && m_pNetField->IsOnline())
		return TRUE;
	else
		return FALSE;
}

bool CNetClient::IsOnlineBoard()
{
	if (m_pNetBoard != NULL && m_pNetBoard->IsOnline())
		return TRUE;
	else
		return FALSE;
}

INT CNetClient::IsLogin()
{
	if (m_nLoginResult != 0)
		return m_nLoginResult;
	else
		return 0;
}

void CNetClient::SetWndHandle ( HWND hWnd, DxMsgProcessor* pGlobalStage )
{
	m_hWnd			= hWnd;
	m_pGlobalStage	= pGlobalStage;
}

void CNetClient::SetAppPath ()
{
	// Note : 실행파일의 경로를 찾아서 저장한다.
	//
	CString m_strAppPath;
	CString strAppPath;

	TCHAR szPath[MAX_PATH] = {0};
	GetModuleFileName(::AfxGetInstanceHandle(), szPath, MAX_PATH);
	strAppPath = szPath;
	
	if ( !strAppPath.IsEmpty() )
	{
		m_strAppPath = strAppPath.Left ( strAppPath.ReverseFind ( '\\' ) );
		
		if ( !m_strAppPath.IsEmpty() )
		if ( m_strAppPath.GetAt(0) == '"' )
			m_strAppPath = m_strAppPath.Right ( m_strAppPath.GetLength() - 1 );

        m_strAppPath += '\\';
		::StringCchCopy(m_szAppPath, MAX_CHAR_LENGTH, m_strAppPath.GetString() );
	}	
}

// 버전파일로 부터 게임버전과, 패치프로그램 버전을 읽는다.
int CNetClient::GetClientVer(int& LauncherVer, int& PatchVer)
{
	CString str;
	str.Format ( "%s%s", m_szAppPath, g_szClientVerFile );
	FILE* sfp = ::_fsopen ( str.GetString(), "rb", _SH_DENYNO );
	if ( !sfp )
	{
		return NET_ERROR;
	}
	//fseek ( sfp, sizeof (int), SEEK_SET );
	if ( 1 != ::fread ( &m_LauncherVersion, sizeof(int), 1, sfp ) )
	{
		::fclose ( sfp );
		return NET_ERROR;
	}	
	if ( 1 != ::fread ( &m_PatchVersion, sizeof(int), 1, sfp ) )
	{
		::fclose ( sfp );
		return NET_ERROR;
	}
    LauncherVer = m_LauncherVersion;
    PatchVer = m_PatchVersion;
	::fclose ( sfp );
	return NET_OK;
}

int CNetClient::GetClientVer()
{
    int temp;
    return GetClientVer( temp, temp );
}

int	CNetClient::GetGameVer()
{
	return m_PatchVersion;
}

int	CNetClient::GetPatchVer()
{
	return m_LauncherVersion;
}

int CNetClient::NetThreadProc()
{		
	DWORD	dwRetCode;
	int		nRetCode;
		
	::ResetEvent(m_hEventKill);
	::ResetEvent(m_hEventWork);
	
	HANDLE hEventSocket = ::WSACreateEvent(); // 소켓이벤트 생성

	// 소켓 이벤트 등록
	dwRetCode = ::WSAEventSelect(m_sClient, hEventSocket, FD_READ | FD_WRITE | FD_CLOSE);
	if (dwRetCode == SOCKET_ERROR)
	{
		TRACE("WSAEventSelect Error %d \n", WSAGetLastError());
		WSACloseEvent ( hEventSocket );
		return NET_ERROR;
	}

	HANDLE h[] = { m_hEventKill, m_hEventWork, hEventSocket };
	
	//SndVersion ();
	//SndReqServerInfo();

	m_bThreadProc = TRUE;
	while (m_bThreadProc)
	{
		//이벤트가 발생할때까지 계속기다린다.
		dwRetCode = ::WaitForMultipleObjects(3, h, FALSE, INFINITE);
		// ::WSAEnumNetworkEvents
		switch (dwRetCode)
		{		
		case WAIT_OBJECT_0:			//  Kill event, kill this thread
			{
				m_bThreadProc = FALSE;
			}
			break;
		case WAIT_OBJECT_0 + 1:		// Work event, something to send
			{
				SendBuffer();
			}
			break;
		case WAIT_OBJECT_0 + 2:		// Socket event
			{
				WSANETWORKEVENTS events;
				nRetCode = ::WSAEnumNetworkEvents(m_sClient, hEventSocket, &events);
				if (nRetCode == SOCKET_ERROR)
				{
					m_bThreadProc = FALSE;
				}
				else
				{
					if (events.lNetworkEvents & FD_READ)
					{						
						Recv();
						MessageProcess();
					}
					else if (events.lNetworkEvents & FD_WRITE)
					{
						SendBuffer();
					}
					else if (events.lNetworkEvents & FD_CLOSE)
					{
						CloseConnect();
					}
				}
			}
			break;		
		default:
			{
				TRACE ("SockThread exceptional case \n");
				m_bThreadProc = FALSE;
			}
			break;
		}	
		// Do not remove this code
		Sleep( 0 );
	}
	::WSACloseEvent(hEventSocket);
	TRACE ("CNetClient::NetThreadProc End \n");
	return 0;
}

int CNetClient::ConnectLoginServer( const char *szAddress, int nPort)
{	
	if (m_nOnline == net::ONLINE)
		CloseConnect();

	return ConnectServer(szAddress, nPort, NET_STATE_LOGIN);
}
/*
int CNetClient::ConnectAgentServer(int nServerGroup, int nServerNumber)
{
	return ConnectGameServer(nServerGroup, nServerNumber);
}
*/
int CNetClient::ConnectGameServer(int nServerGroup, int nServerNumber)
{
	if (m_nOnline == net::ONLINE)
		CloseConnect();

	return ConnectServer(m_sGame[nServerGroup][nServerNumber].szServerIP,
						m_sGame[nServerGroup][nServerNumber].nServicePort,
						NET_STATE_AGENT);
}

int	CNetClient::ConnectFieldServer(char *szAddress, int nPort)
{
	if (!m_pNetField)
	{
		m_pNetField = new CNetClient;
		m_pNetField->SetWndHandle(m_hWnd, m_pGlobalStage);
		m_pNetField->SetGaeaID(m_dwGaeaID);
	}

	if (m_pNetField->IsOnline())
		m_pNetField->CloseConnect();

	return m_pNetField->ConnectServer(szAddress, nPort, NET_STATE_FIELD, m_ck);
}
/*
int	CNetClient::ConnectBoardServer(char *szAddress, int nPort)
{
	if (m_pNetBoard == NULL)
	{
		m_pNetBoard = new CNetClient;
		m_pNetBoard->SetWndHandle(m_hWnd, m_pGlobalStage);
		m_pNetField->SetGaeaID(m_dwGaeaID);
	}
	if (m_pNetBoard->IsOnline())
	{
		m_pNetBoard->CloseConnect();
	}	
	return m_pNetBoard->ConnectServer(szAddress, nPort, NET_STATE_BOARD);
}
*/
int CNetClient::ConnectServer(unsigned long uServerIP, u_short nPort, int nState)
{
	return 0;
}

int	CNetClient::ConnectServer(const char* szServerIP, int nPort, int nState, CRYPT_KEY ck)
{
	int nRT = ConnectServer(szServerIP, nPort, nState);
	m_ck = ck;

    return nRT;
		
}

int	CNetClient::ConnectServer(const char* szServerIP, int nPort, int nState)
{
	// hostent*		hst;	
	// char			strRet[20];
	// unsigned long	ulIP;		
	// struct in_addr	inetAddr;
	int				nRetCode=0;
	DWORD			dwRetCode;
	CString			strTemp;
	bool			bEvent = false;

	// gethostbyname 함수는 호스트 데이터베이스로 부터 
	// 호스트 이름에 대응되는 호스트 정보를 얻어내는 함수이다.
	// inet_ntoa 함수는 네트웍 바이트 오더인 IP 어드레스의 포맷을 10진 문자열로 지정합니다.
	// IP 어드레스는 IN_ADDR 구조체의 형태로 inet_ntoa 함수에 제공됩니다
	// GetAdaptersInfo
	// *******************************************
	// 에러로 인하여 아래코드는 임시로 주석처리됨
	// *******************************************
	// DNS 세팅이 제대로 되지 않은 클라이언트에서 gethostbyname 함수가 계속실패 함에 따라 주석처리됨
	/*
	hst = ::gethostbyname(cServerIP);		
	if (hst == NULL) // ERROR
	{	
		strTemp.Format("gethostbyname Error Code %d", WSAGetLastError());
		sc::writeLogError(strTemp.GetString());
		return NET_ERROR;
	}	
	ulIP = *(DWORD*) (*hst->h_addr_list);
	inetAddr.s_addr = ulIP;	
	::strcpy(strRet, inet_ntoa(inetAddr));
	*/
	
	CloseConnect();
	
	// inet_addr 함수는 10진 형식의 IP 어드레스 문자열을 32-비트 정수(IP어드레스)로 변환합니다. 
	// 어드레스는 네트웍 바이트 오더(빅-인디언)로 변환됩니다.
	::SecureZeroMemory(&m_Addr, sizeof(SOCKADDR_IN));
	m_Addr.sin_family = AF_INET;
	m_Addr.sin_port = ::htons(nPort);
    m_Addr.sin_addr.s_addr = sc::net::inetAddr(szServerIP); //::inet_addr(szServerIP); // inet_addr("211.224.129.220"); 
	m_sClient = ::socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (m_sClient == INVALID_SOCKET)
	{		
		sc::writeLogError(
            sc::string::format(
                "socket Error Code %1%", WSAGetLastError()));
		return NET_ERROR;
	}
	
	// ////////////////////////////////////////////////////////////////////////////
	// Set the socket option
    // Disable send buffering on the socket.  Setting SO_SNDBUF 
    // to 0 causes winsock to stop bufferring sends and perform 
    // sends directly from our buffers, thereby reducing CPU usage. 
	// Winsock 의 버퍼링을 줄이고 cpu 사용량을 줄이려면 아래 옵션을 활성하 시키면 됨.
	// Set SO_SNDBUF to 0
	// Set SO_RCVBUF to 0
	// ///////////////////////////////////////////////////////////////////////////
	
	// ///////////////////////////////////////////////////////////////////////////
    /*
	int nZero = 0; 
	::setsockopt(m_sClient, SOL_SOCKET, SO_SNDBUF, (char *) &nZero, sizeof(nZero)); 
	::setsockopt(m_sClient, SOL_SOCKET, SO_RCVBUF, (char *) &nZero, sizeof(nZero)); 
	*/
	// ///////////////////////////////////////////////////////////////////////////

	/*
	LINGER      lingerStruct;     
	lingerStruct.l_onoff = 1; 
	lingerStruct.l_linger = 5;
	nRetCode = ::setsockopt(m_sClient, 
						    SOL_SOCKET, 
							SO_LINGER, 
							(char *) &lingerStruct, 
							sizeof(lingerStruct));
	*/

	ULONG nonBlk = TRUE;
	if (::ioctlsocket(m_sClient, FIONBIO, &nonBlk) == SOCKET_ERROR)
	{
		int errCode = WSAGetLastError();
        sc::writeLogError(
            sc::string::format(
                "ioctlsocket %1%", sc::net::getLastError( errCode )));
	}
	
	//////////////////////////////////////////////////////////////////////////////

	WSAEVENT event = ::WSACreateEvent();
	if (event == WSA_INVALID_EVENT)
    {
		int errCode = WSAGetLastError();
		sc::writeLogError(
            sc::string::format(
                "WSACreateEvent Error Code %1%", sc::net::getLastError( errCode )));
		::closesocket(m_sClient);
		bEvent = false;
        return NET_ERR_CREATE_EVENT;
    }

	nRetCode = ::WSAEventSelect(m_sClient, event, FD_CONNECT);
	if (nRetCode == SOCKET_ERROR)
	{		
		sc::writeLogError(
            sc::string::format(
                "WSAEventSelect Error Code %1%", WSAGetLastError()));
		::closesocket(m_sClient);
		bEvent = false;
		::WSACloseEvent( event );

		return NET_ERR_EVENT_SELECT;
	}
	
	// Connect to server
	nRetCode = ::connect(m_sClient, 
						(sockaddr *) &m_Addr, 
						sizeof(SOCKADDR_IN));

	if (nRetCode == SOCKET_ERROR)
	{
		nRetCode = ::WSAGetLastError();
		if (nRetCode != WSAEWOULDBLOCK)
		{	
			// 컨넥트에서 실패하였습니다.
			sc::writeLogError(
                sc::string::format(
                    "connect error:Code %1%", nRetCode));
			m_sClient = INVALID_SOCKET;
			::closesocket(m_sClient);
			bEvent = false;
			::WSACloseEvent( event );

			return NET_ERR_CONNECT;
		}
	}	

	// wait for connet result
	// 5 초 동안 3 번 대기한다. (최대 15초간 대기)
	for (int nLoop=0; nLoop < 3; nLoop++)
	{	
		// ********************************************************************
		// Do not remove this code : SleepEx for Windows 98
		// 윈도우즈 98 에서 최초 SleepEx 코드가 없으면 이벤트가 발생하지 않음
		// ********************************************************************		
		::SleepEx( 0, TRUE );
		dwRetCode = ::WSAWaitForMultipleEvents(1, &event, FALSE, 10000, FALSE);
		if (dwRetCode == WSA_WAIT_FAILED)
		{
			int errCode = WSAGetLastError();
            sc::writeLogError(
                sc::string::format(
                    "WSAWaitForMultipleEvents %1%", sc::net::getLastError( errCode )));
			bEvent = false;
		}
		else if (dwRetCode == WSA_WAIT_TIMEOUT)
		{
            sc::writeLogError(std::string("WSAWaitForMultipleEvents Error Code WSA_WAIT_TIMEOUT"));
			bEvent = false;
		}
		else if (dwRetCode == WAIT_OBJECT_0)
		{
			bEvent = true;
			break;
		}
		else
		{			
			// 이벤트 대기에 실패하였습니다.
            sc::writeLogError(
                sc::string::format(
                    "WSAWaitForMultipleEvents Error Code %1%", sc::net::getLastError(dwRetCode)));			
			::closesocket(m_sClient);
			bEvent = false;
			::WSACloseEvent(event);
			return NET_ERR_EVENT_WAIT;
		}
		// ********************************************************************
		// Do not remove this code : SleepEx : Windows 98
		// 윈도우즈 98 OS 에서 SleepEx 없이 컨넥트 이벤트를 대기하면 
		// 무한 대기상태에 빠짐 반드시 삽입하여야 함
		// ********************************************************************
		::SleepEx( 500, FALSE );
	}
		
	if (bEvent == true)
	{
		WSANETWORKEVENTS events;
		nRetCode = ::WSAEnumNetworkEvents(m_sClient, event, &events);
		if (!(events.lNetworkEvents & FD_CONNECT) || (0 != events.iErrorCode[FD_CONNECT_BIT]))
		{
			// 컨텍트 이벤트가 발생하지 않았습니다.
			// Socket connection fail
			nRetCode = ::WSAGetLastError();
			sc::writeLogError(
                sc::string::format(
                    "WSAEnumNetworkEvents - Contact Event Error, NetworkEvent : %1%, nRetCode : %2%, iErrorCode : %3%",
                    events.lNetworkEvents,
                    nRetCode,
                    events.iErrorCode[FD_CONNECT_BIT]));
			::closesocket(m_sClient);
			::WSACloseEvent( event );

			return NET_ERR_CONTACT_EVENT;
		}

		// ::memset(m_pRcvBuffer, 0, NET_DATA_CLIENT_MSG_BUFSIZE);
		// ::memset(m_pMsgBuffer, 0, NET_DATA_BUFSIZE);	
		::memset(m_pSndBuffer, 0, NET_DATA_CLIENT_MSG_BUFSIZE);
		
		// m_nRcvBytes			= 0;
		m_nSndBytes			= 0;	
		m_nTotalBytes		= sizeof(NET_MSG_GENERIC);
		m_nNetMode			= NET_PACKET_HEAD;
		m_nClientNetState	= nState;
		m_nOnline			= net::ONLINE;

		::WSACloseEvent( event );
		
		if (StartNetThread() == NET_OK)
        {
            msg_CS_AES_REQ_SERVER_PUB_KEY();
            return NET_OK;
        }
        else
        {
            return NET_ERROR;
        }
	}
	else
	{
		::closesocket(m_sClient);
		::WSACloseEvent( event );

		return NET_ERROR;
	}
}

int CNetClient::CloseConnect(void)
{
	int nRetCode;
	if (m_sClient != INVALID_SOCKET)
	{		
		if (m_nClientNetState == NET_STATE_AGENT)
		{
			if (m_pNetField != NULL && m_pNetField->IsOnline())
			{
				m_pNetField->CloseConnect();
			}
			if (m_pNetBoard != NULL && m_pNetBoard->IsOnline())
			{
				m_pNetBoard->CloseConnect();
			}
		}
		
		/*
		LINGER      lingerStruct;     
		lingerStruct.l_onoff  = 1;  
		lingerStruct.l_linger = 5;
		nRetCode = ::setsockopt(m_sClient, 
						        SOL_SOCKET, 
							    SO_LINGER, 
							    (char *) &lingerStruct, 
							    sizeof(lingerStruct));
		*/

		// ::shutdown(m_sClient, SD_SEND);		
		nRetCode = ::closesocket(m_sClient);
		m_sClient = INVALID_SOCKET;
		m_nOnline = net::OFFLINE;
		if (nRetCode == SOCKET_ERROR)
			return NET_ERROR;
		StopNetThread(true);

		//m_ck.nKey          = 1;
		//m_ck.nKeyDirection = 1;
		m_bKeyReceived = FALSE;
	}
	TRACE("CNetClient::CloseConnect \n");
#ifdef _DEBUG	
	CDebugSet::ToListView("CNetClient::CloseConnect 연결이 끊어졌습니다.");
#endif	
	return NET_OK;
}

BOOL CNetClient::IsGameSvrInfoEnd() 
{ 
	return m_bGameServerInfoEnd; 
}

BOOL CNetClient::IsKeyReceived()
{
	return m_bKeyReceived;
}

BOOL CNetClient::IsWorldBattle() 
{ 
	return m_bWorldBattle; 
}

void CNetClient::SetWorldBattle(BOOL bWorldBattle) 
{ 
	m_bWorldBattle = bWorldBattle;  
}

BOOL CNetClient::IsWorldBattleSvrInfoEnd() 
{ 
	return m_bWorldBattleSvrInfoEnd; 
}

void CNetClient::SetWorldBattleSvrInfoEnd(BOOL bWorldBattleSvrInfoEnd) 
{ 
	m_bWorldBattleSvrInfoEnd = bWorldBattleSvrInfoEnd;  
}

BOOL CNetClient::IsWorldBattleCharSelectEnd() 
{ 
	return m_bWorldBattleCharSelectEnd; 
}

void CNetClient::SetWorldBattleCharSelectEnd(BOOL bWorldBattleCharSelectEnd) 
{ 
	m_bWorldBattleCharSelectEnd = bWorldBattleCharSelectEnd;  
}



// 클라이언트 윈도우즈 메시지 처리함수
// wParam : 통신이 온 소켓 번호
// lParam : 통신이벤트
void CNetClient::ClientProcess(WPARAM wParam, LPARAM lParam)
{	
}

int CNetClient::StartNetThread()
{	
	DWORD dwThreadID = 101;	

	/*
	m_hThread = ::CreateThread(NULL,
					0, 
					(LPTHREAD_START_ROUTINE) NetThread, 
					this, 
					0, 
					&dwThreadID);
	*/
	m_hThread = (HANDLE) ::_beginthreadex(
								NULL,
								0, 
								NetThread,
								this, 
								0, 
								(unsigned int*) &dwThreadID);
	if (m_hThread == NULL)
	{
		// 쓰래드 생성에 실패하였습니다.
		return NET_ERR_CREATE_THREAD;
	}
	else
	{
		return NET_OK;
	}
}

int	CNetClient::StopNetThread(bool wait)
{
    BT_SetTerminate();

	SetEvent(m_hEventKill);	
	if (wait)
	{
		if (m_hThread)
		{
			DWORD result = ::WaitForSingleObject(m_hThread, INFINITE);
			::CloseHandle(m_hThread); 
			m_hThread = NULL;
			if (WAIT_OBJECT_0 == result)
			{
				return NET_OK;
			}
		}
		return NET_ERROR;
	}
	return NET_OK;
}

INT CNetClient::SendBuffer()
{
	// 무한루프의 가능성 있음
	while (true) 
	{
		if (m_nSndBytes <= 0) 
			break;

		if( SendBuffer2() == NET_ERROR )
			break;

		Sleep( 0 );
	}
	return NET_OK;
}

INT CNetClient::SendBuffer2()
{
	int nTotalSndBytes	= 0;
	int	nSndBytes		= 0;
	int	nRetCode		= 0;
	DWORD dwSize			= 0;
	NET_MSG_GENERIC* nmg;
    	
    sc::CriticalSectionOwnerLock OwnerLock(m_CriticalSection);

	if (m_nSndBytes >= sizeof(NET_MSG_GENERIC))
	{
		nmg = (NET_MSG_GENERIC*) m_pSndBuffer;		
		dwSize = nmg->Size();

		nSndBytes = ::send(m_sClient, 
						(const char*) (m_pSndBuffer),
						(int) dwSize,
						0);
		// 전송 에러처리 
		if (nSndBytes == SOCKET_ERROR) 
		{			
			nRetCode = ::WSAGetLastError();
			// Send 전송 버퍼에 병목현상 발생, FD_WRITE 이벤트를 기다린다.
			// Send 전송 버퍼에 병목현상 발생, 다음 FD_WRITE 이벤트 발생시에 다시 보낸다.
			if (nRetCode == WSAEWOULDBLOCK)	
			{
				TRACE("WSAEWOULDBLOCK \n");
				//LockOff();
				return NET_OK; 
			}
			else
			{
				// 2014.11.21
				// 원래는 무조건 NET_ERROR 를 반환하도록 되어있었음. 하지만 반환값을 받아서 처리하지 않아서 전송이 성공할때까지 무한으로 도는 부분이 발생
				// 따라서 기존의 로직과 동일하게 처리하기 위해서 NET_ERROR를 NET_OK로 수정해서 계족 처리하도록 하고 
				// WSAENOTSOCK 은 INVAILDED SOCKET 이므로 종료 처리하도록 함.
				// 소켓을 정상적이지 않게 닫아서 발생하는 문제이긴한데... 해당 소켓만 정상적으로 닫아도 다른 소켓쪽에서 다시 발생할 여지가 있으므로 아래와 같이 WSAENOTSOCK 일때 중단하도록 함.

				if( nRetCode == WSAENOTSOCK )
				{
					sc::writeLogError(std::string(" SEND ERROR [ CODE : WSAENOTSOCK ]"));
					return NET_ERROR;
				}
				// ::MessageBox(NULL, "SendBuffer2 Error", "Error", MB_OK);
				TRACE("SendBuffer2 Error \n");
				//LockOff();
				return NET_OK;
			}
		}
		else // 전송 성공
		{		
			// 보내야할 패킷 정확히 전송성공
			if (nSndBytes == (int) dwSize) 
			{
                Packet_Log_Write_Begin();
                Packet_Log_Write_End(nmg->Type());

				::memmove(m_pSndBuffer, m_pSndBuffer+nSndBytes, m_nSndBytes-nSndBytes);
				m_nSndBytes -= nSndBytes;
				assert(m_nSndBytes >= 0);
			}
			else // 올바르게 전송하지 못함, work 이벤트를 발생시킴
			{
				::SetEvent(m_hEventWork);
			}
		}
	}
	//LockOff();
	return NET_OK;
}

/*
int CNetClient::GetGarbageMsg()
{
	char szTempChar[12];
	int garbageNum = 0;
	while (1)
	{
		ZeroMemory(szTempChar, 12);
		garbageNum = sc::Random::RandomNumber(0, 4);
		StringCchCopy(szTempChar, 12, GARBAGE_DATA[garbageNum]);

		if (strcmp( szTempChar, m_szSendGarbageMsg[0] ) != 0 &&
			strcmp( szTempChar, m_szSendGarbageMsg[1] ) != 0 )
			//strcmp( szTempChar, m_szSendGarbageMsg[2] ) != 0)
		{
			StringCchCopy(m_szSendGarbageMsg[m_dwGarbageNum], 12, szTempChar);
			return (int) strlen(m_szSendGarbageMsg[m_dwGarbageNum]);
		}
	}
	return -1;
}
*/
/*
char*  CNetClient::SendMsgAddGarbageValue( CHAR* buff, INT &nSize )
{

	DWORD dwHeaderSize = sizeof(NET_MSG_GENERIC);
	DWORD dwBodySize   = nSize-dwHeaderSize;

	int nGarbageLen = GetGarbageMsg();

	char *sendBuffer;
	sendBuffer	 = new char[nSize+nGarbageLen+1];
	ZeroMemory( sendBuffer, nSize+nGarbageLen+1 );

	//	memcpy( headerBuffer, buff, dwHeaderSize);
	NET_MSG_GENERIC* pNmg = new NET_MSG_GENERIC;
	memcpy( pNmg, buff, dwHeaderSize );
	// 중간에 더한 쓰레기 값만큼 더한다.
	pNmg->dwSize += nGarbageLen;
	nSize		 += nGarbageLen;

	memcpy( sendBuffer, pNmg, dwHeaderSize );
	memcpy( &sendBuffer[dwHeaderSize], m_szSendGarbageMsg[m_dwGarbageNum], nGarbageLen );
	memcpy( &sendBuffer[dwHeaderSize+nGarbageLen], &buff[dwHeaderSize], dwBodySize+1 );

	SAFE_DELETE(pNmg);

	m_dwGarbageNum++;
	if( m_dwGarbageNum >= 2 ) m_dwGarbageNum = 0;

	return sendBuffer;
}
*/
int	CNetClient::SendNormal (CHAR* buff, INT nSize)
{
	if (!IsOnline())
        return 0;

    {
	    sc::CriticalSectionOwnerLock OwnerLock(m_CriticalSection);
    	
	    assert(m_nSndBytes+nSize<NET_DATA_CLIENT_MSG_BUFSIZE);
	    assert(nSize>=0);
	    assert(buff != NULL);
    	
        ::memcpy(m_pSndBuffer+m_nSndBytes, buff, nSize);
        m_nSndBytes += nSize;
	}

	::SetEvent(m_hEventWork);

	return 0;
}

int	CNetClient::Send(CHAR* buff)
{
	if (!buff)
        return NET_ERROR;

	NET_MSG_GENERIC* nmg = (NET_MSG_GENERIC*) buff;
	if (nmg->Size() >= sizeof(NET_MSG_GENERIC))
		return Send(buff, (int) nmg->Size());
	else
		return NET_ERROR;
}

int	CNetClient::Send(CHAR* buff, INT nSize)
{	
	if (!IsOnline())
        return 0;
    {
        sc::CriticalSectionOwnerLock OwnerLock(m_CriticalSection);

	    assert(m_nSndBytes+nSize<NET_DATA_CLIENT_MSG_BUFSIZE);
	    assert(nSize>=0);
	    assert(buff != NULL);

	    if ((m_nSndBytes+nSize) >= NET_DATA_CLIENT_MSG_BUFSIZE)
		{
		    return 0;
		}
    	
	    ::memcpy(m_pSndBuffer+m_nSndBytes, buff, nSize);
	    m_nSndBytes += nSize;
    }
	
	::SetEvent(m_hEventWork);
	return 0;
}
/*
int CNetClient::SendAesEncrypt(NET_MSG_GENERIC* pMsg)
{
    if (m_bAesKeyReceived)
        return SendAesEncrypt((BYTE*) pMsg, (int) pMsg->wSize);
    else
        return NET_ERROR;
}
*/
/*
int CNetClient::SendAesEncrypt(BYTE* buff, int nSize)
{
    m_pAes->encrypt(buff+sizeof(NET_MSG_GENERIC), nSize - sizeof(NET_MSG_GENERIC));
    return SendNormal(buff, nSize);
}
*/
/*
void CNetClient::PingToAgent() 
{
	GLMSG::SNET_GM_PING NetMsg;

	NetMsg.dwGaeaID = m_dwGaeaID;
	NetMsg.dwSent = GetTickCount();

	SendToAgent( (CHAR *)&NetMsg, NetMsg.nmg.dwSize );	//To AgentServer
}

void CNetClient::PingToField() 
{
	GLMSG::SNET_GM_PING NetMsg;

	NetMsg.dwGaeaID = m_dwGaeaID;
	NetMsg.dwSent = GetTickCount();

	SendToField ( (CHAR *)&NetMsg, NetMsg.nmg.dwSize);	//To FieldServer 
}
*/

int	CNetClient::SendToAgent(CHAR* buff, INT nSize)
{
	return Send(buff, nSize);
}


int	CNetClient::SendToBoard(CHAR* buff, INT nSize)
{
	if (m_pNetBoard && m_pNetBoard->IsOnline())
		return m_pNetBoard->Send(buff, nSize);
	else
		return NET_ERROR;
}

int	CNetClient::SendToFieldNormal(CHAR* buff, INT nSize)
{
	if (m_pNetField && m_pNetField->IsOnline())
		return m_pNetField->SendNormal(buff, nSize);
	else
		return NET_ERROR;
}

int	CNetClient::SendToField(CHAR* buff, INT nSize)
{
	if (m_pNetField && m_pNetField->IsOnline())
		return m_pNetField->Send(buff, nSize);
	else
		return NET_ERROR;
}

int CNetClient::Recv(void)
{
	int nRcvBytes = 0;
	int nRetCode = 0;
	char pBuff[NET_DATA_BUFSIZE] = {0};	
	
	nRcvBytes = ::recv(m_sClient,
					   pBuff,
                       NET_DATA_BUFSIZE, // sizeof(NET_MSG_GENERIC),
					   0);
	if (nRcvBytes == SOCKET_ERROR)
	{
		TRACE("CNetClient::Recv recv SOCKET_ERROR \n");
		nRetCode = ::WSAGetLastError();
		switch (nRetCode) // recv error
		{
		case WSAETIMEDOUT : // The connection has been dropped because of a network failure or because the peer system failed to respond.
		case WSAECONNABORTED : // The virtual circuit was terminated due to a time-out or other failure.
		case WSAENOTSOCK : // The descriptor is not a socket.
		case WSAENOTCONN : // The socket is not connected.
		case WSAENETDOWN : // The network subsystem has failed.
			{
				CloseConnect();
				return NET_ERROR;
			}
			break;
		default:
			{
			return NET_ERROR;
			break;
			}
		}
	}
	else
	{
		// If the connection has been gracefully closed, the return value is zero.
		// nRcvBytes == 0
		if (nRcvBytes == 0)
		{		
			// Connection Closed	
			CloseConnect();
			return NET_ERROR;
		}
		if (nRcvBytes < 0)
		{
			return NET_OK;
		}
		else
		{
			int nTotalRcvSize = m_pRcvManager->getRcvSize()+nRcvBytes;
			if ( nTotalRcvSize > NET_DATA_CLIENT_MSG_BUFSIZE )
			{
				sc::writeLogError(
					sc::string::format( "Error Recv nRcvBytes : %1%, nTotalRcvSize : ( %2% / %3% )",
					nRcvBytes,
					nTotalRcvSize,
					NET_DATA_CLIENT_MSG_BUFSIZE ) );
			}
			assert(nTotalRcvSize<NET_DATA_CLIENT_MSG_BUFSIZE);
			if (nTotalRcvSize<NET_DATA_CLIENT_MSG_BUFSIZE)
			{
				m_pRcvManager->addRcvMsg(pBuff, nRcvBytes);
			}			
			return NET_OK;
		}
	}
}

void* CNetClient::IsValid(void)
{	
	return m_pRcvManager->getMsg();
}

void CNetClient::EncryptLoginDataWithApexRSA(
    unsigned char * szUserID,
    unsigned char * szUserPassword,
    unsigned char * szRandomPassword)
{
	CString strTemp;

	strTemp = szUserID;
	m_ApexClient.encrypt_data_with_PrivateKey_Key1( strTemp.GetString(), strTemp.GetLength()+1 );
	m_ApexClient.get_data( szUserID );

	strTemp = szUserPassword;
	m_ApexClient.encrypt_data_with_PrivateKey_Key1( strTemp.GetString(), strTemp.GetLength()+1 );
	m_ApexClient.get_data( szUserPassword );

	strTemp = szRandomPassword;
	m_ApexClient.encrypt_data_with_PrivateKey_Key1( strTemp.GetString(), strTemp.GetLength()+1 );
	m_ApexClient.get_data( szRandomPassword );
}

const TCHAR* CNetClient::DoEncryptKey()
{
	m_Tea.encrypt(m_szEncryptKeyHeart, ENCRYPT_KEY+1);
	return m_szEncryptKeyHeart;
}

void CNetClient::msg_SC_AES_SERVER_PUB_KEY(NET_MSG_GENERIC* pMsg)
{
    /*
    BYTE pubkeyServer[256]={0};
    int nKeyLength1=0;

    NET_COMMON::NET_SC_AES_SERVER_PUB_KEY msg;
    nKeyLength1 = msg.getPubKey((BYTE*) pMsg, pubkeyServer);

    delete m_pAes;
    // 서버에서 넘어온 public key 를 세팅한다.
    m_pAes = new sc::Aes(pubkeyServer);

    // 클라이언트의 public key 를 구한다.
    BYTE pubkeyClient[256] = {0};
    int nKeyLength2=0;
    m_pAes->getClientPubKey(pubkeyClient, nKeyLength2);

    // 서버로 pubkey 를 전송한다.
    msg.setPubKeyBufferClient(pubkeyClient, nKeyLength2);
    SendNormal(msg.getBuffer(), msg.wSize);
    */
}

void CNetClient::msg_SC_AES_TEST(NET_MSG_GENERIC* pMsg)
{
    /*
    NET_COMMON::NET_AES_TEST_PACKET* pTestMsg = (NET_COMMON::NET_AES_TEST_PACKET*) pMsg;
    m_pAes->decrypt(
        (BYTE*) (pTestMsg) + sizeof(NET_MSG_GENERIC),
        pTestMsg->wSize-sizeof(NET_MSG_GENERIC));
    m_bAesKeyReceived = true;
    */
}

void CNetClient::msg_SC_AES_TEST2(NET_MSG_GENERIC* pMsg )
{
    /*
    NET_COMMON::NET_AES_TEST_PACKET* pTestMsg = (NET_COMMON::NET_AES_TEST_PACKET*) pMsg;
    m_pAes->decrypt(
        (BYTE*) (pTestMsg) + sizeof(NET_MSG_GENERIC),
        pTestMsg->wSize-sizeof(NET_MSG_GENERIC));

    m_bAesKeyReceived = true;

    // Client->Agent : Send uuid, userid, dbindex and check auth information
    NET_COMMON::NET_CA_AUTH_REQ msg;

    ::memcpy(&msg.guid, &g_GUID, sizeof(GUID));
    msg.nDbUserNum = g_nDbUserNum;
    ::StringCchCopy(msg.szUserID, USR_ID_LENGTH, g_strUserID.c_str());

    // 암호화 전송
    SendAesEncrypt(&msg);
    */
}

//! Client->Server AES 공개키를 요청한다.
void CNetClient::msg_CS_AES_REQ_SERVER_PUB_KEY()
{
    //NET_CS_AES_REQ_SERVER_PUB_KEY msg;
    //SendNormal(&msg);
}