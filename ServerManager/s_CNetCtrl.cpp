#include "StdAfx.h"
#include <process.h>
#include "../SigmaCore/Log/LogMan.h"
#include "../SigmaCore/Net/NetDefine.h"
#include "../SigmaCore/Util/Lock.h"
#include "../RanLogic/s_NetGlobal.h"
#include "../RanLogic/Util/s_CClientConsoleMsg.h"
#include "../RanLogic/Msg/ServerMsg.h"
#include "s_CNetCtrl.h"
#include "ServerManagerDlg.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

static unsigned int WINAPI NetThread(void* p)
{
	CNetCtrl* pNet = (CNetCtrl*) p;
	pNet->NetThreadProc();
	return 0;
}

CNetCtrl::CNetCtrl(void)
{
	::InitializeCriticalSection(&m_CriticalSection);
	m_hEventWork = NULL;
	m_hEventKill = NULL;
	m_pSndBuffer = NULL;
	m_pRcvBuffer = NULL;
	m_pMsgBuffer = NULL;
	m_pDlg		 = NULL;
	m_hThread	 = NULL;

	m_bIsRunning = FALSE;
	m_nOnline = net::OFFLINE;
	
	SetDefault();
}

CNetCtrl::CNetCtrl(HWND hWnd)
{
	::InitializeCriticalSection(&m_CriticalSection);
	m_hEventWork = NULL;
	m_hEventKill = NULL;
	m_pSndBuffer = NULL;
	m_pRcvBuffer = NULL;
	m_pMsgBuffer = NULL;
	m_pDlg		 = NULL;
	m_hThread	 = NULL;

	m_bIsRunning = FALSE;
	m_nOnline = net::OFFLINE;

	SetDefault();
	SetWndHandle(hWnd);
}

CNetCtrl::~CNetCtrl(void)
{
	SAFE_DELETE_ARRAY(m_pSndBuffer);
	SAFE_DELETE_ARRAY(m_pRcvBuffer);
	SAFE_DELETE_ARRAY(m_pMsgBuffer);	
	::DeleteCriticalSection(&m_CriticalSection);
	CloseConnect();
	CloseHandle( m_hEventWork );
	CloseHandle( m_hEventKill );
}
/*
void CNetCtrl::LockOn()
{
	::EnterCriticalSection(&m_CriticalSection);
}
*/
/*
void CNetCtrl::LockOff()
{
	::LeaveCriticalSection(&m_CriticalSection);
}
*/
void CNetCtrl::SetDefault()
{
	// int i,j;

	m_nOnline = net::OFFLINE;
	m_sClient = INVALID_SOCKET;
	m_hWnd	  = NULL;		

	SAFE_DELETE_ARRAY(m_pSndBuffer);
	SAFE_DELETE_ARRAY(m_pRcvBuffer);
	SAFE_DELETE_ARRAY(m_pMsgBuffer);	

	m_pSndBuffer = new char[NET_DATA_MSG_BUFSIZE];
	m_pRcvBuffer = new char[NET_DATA_MSG_BUFSIZE];
	m_pMsgBuffer = new char[NET_DATA_BUFSIZE];

	::memset(m_pSndBuffer, 0, NET_DATA_MSG_BUFSIZE);
	::memset(m_pRcvBuffer, 0, NET_DATA_MSG_BUFSIZE);
	::memset(m_pMsgBuffer, 0, NET_DATA_BUFSIZE);
	
	m_nRcvBytes				= 0;
	m_nSndBytes				= 0;
	m_nTotalBytes			= sizeof(NET_MSG_GENERIC);	
	m_nNetMode				= NET_PACKET_HEAD;	
	//m_ck.nKeyDirection		= 0;	// 암호키 방향
	//m_ck.nKey				= 0;	// 암호키 shift 량
	
	m_bGameServerInfoEnd	= FALSE;
	m_nLoginResult			= 0; // 로그인 결과 세팅		
	m_hEventWork			= ::CreateEvent(0, FALSE, FALSE, 0);
	m_hEventKill			= ::CreateEvent(0, FALSE, FALSE, 0);
}

void CNetCtrl::SetWndHandle(HWND hWnd)
{
	m_hWnd = hWnd;
}

void CNetCtrl::SetCtrlHandle(CServerManagerDlg* pDlg)
{
	m_pDlg = pDlg;
}

// put the main code of the thread here.
// Must be overloaded
void CNetCtrl::NetThreadProc()
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
		//CClientConsoleMessage::GetInstance()->WriteWithTime("WSAEventSelect Error %d \n", WSAGetLastError());
        sc::writeLogInfo(sc::string::format("WSAEventSelect Error %1% \n", WSAGetLastError()));
		return;
	}

	HANDLE h[] = { m_hEventKill, m_hEventWork, hEventSocket };
		
	m_bIsRunning = TRUE;

	while (m_bIsRunning)
	{
		//이벤트가 발생할때까지 계속기다린다.
		dwRetCode = ::WaitForMultipleObjects(3, h, FALSE, INFINITE);		
		switch (dwRetCode)
		{		
		case WAIT_OBJECT_0:			//  Kill event, kill this thread
			{
				sc::writeLogInfo("KILL EVENT [WAIT_OBJECT_0]");
				m_bIsRunning = FALSE;
				m_nOnline = net::OFFLINE;
			}
			break;
		case WAIT_OBJECT_0 + 1:		// Work event, something to send
			{
				if(SendBuffer() != NET_OK )
				{
					// 연결을 종료하고 다시 재연결을 해야하는데 일단 확인을 위해서 로그 남김
					sc::writeLogInfo("SendBuffer ERROR [WAIT_OBJECT_0 + 1]");
				}
			}
			break;
		case WAIT_OBJECT_0 + 2:		// Socket event
			{
				WSANETWORKEVENTS events;
				nRetCode = ::WSAEnumNetworkEvents(m_sClient, hEventSocket, &events);
				if (nRetCode == SOCKET_ERROR)
				{
					m_bIsRunning = FALSE;
					m_nOnline = net::OFFLINE;
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
						if(SendBuffer() != NET_OK )
						{
							// 연결을 종료하고 다시 재연결을 해야하는데 일단 확인을 위해서 로그 남김
							sc::writeLogInfo("SendBuffer ERROR [events.lNetworkEvents & FD_WRITE]");
						}
					}
					else if (events.lNetworkEvents & FD_CLOSE)
					{						
						CloseConnect();
						// 연결종료됨
						MessageBox(m_pDlg->m_hWnd, 
							       _T("Connection Closed"),
								   _T("Warning"),
								   MB_OK);
						//CClientConsoleMessage::GetInstance()->WriteWithTime(_T("Connection Closed"));
                        sc::writeLogInfo("Connection Closed");
					}
				}
			}
			break;
		default:
			{
				TRACE ("SockThread exceptional case \n");
				sc::writeLogInfo("SockThread exceptional case");
				m_bIsRunning = FALSE;
				m_nOnline = net::OFFLINE;
			}
			break;
		}		
		Sleep( 1 );
	}
	::WSACloseEvent(hEventSocket);
	TRACE ("CNetCtrl::NetThreadProc End \n");
	return;
}

BOOL CNetCtrl::IsOnline()
{
	if (m_nOnline == net::ONLINE)	return TRUE;

	return FALSE;
}

int	CNetCtrl::ConnectServer(const char* cServerIP, int nPort, int nState)
{
	hostent* hst;	
	char strRet[20];
	unsigned long ulIP;		
	struct in_addr inetAddr;

	hst = ::gethostbyname(cServerIP);		
	if (hst == NULL) // ERROR
	{		
		return NET_ERROR;
	}	
	ulIP = *(DWORD*) (*hst->h_addr_list);
	inetAddr.s_addr = ulIP;
	::strcpy(strRet, ::inet_ntoa(inetAddr));
	return ConnectServer(::inet_addr(strRet), (u_short) nPort, nState);
}

int	CNetCtrl::ReConnectServer(int nState)
{
	// 상태 확인을 위한 단순 로그. 안정화 이후 삭제 요망
	if (!IsOnline())
	{
		sc::writeLogInfo("Connection State is OFF");
	}
	sc::writeLogInfo("ReConnectServer start to work");

	return ConnectServer(m_cServerIP, m_cServerPort, nState);
}

int CNetCtrl::ConnectServer(unsigned long cServerIP, u_short nPort, int nState)
{
	int nRetCode;

	if (IsOnline())
	{
		if(CloseConnect() != NET_OK)
		{
			sc::writeLogInfo("CloseConnection ERROR on ConnectServer");
			return NET_ERROR;
		}
	}

	m_sClient = ::socket(
					PF_INET,
					SOCK_STREAM,
					IPPROTO_TCP );
	if (m_sClient == INVALID_SOCKET) return NET_ERROR;

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

	::ZeroMemory(&m_Addr, sizeof(SOCKADDR_IN));
	m_Addr.sin_family = AF_INET;
	m_Addr.sin_port = ::htons(nPort);
	m_Addr.sin_addr.s_addr = cServerIP;

	HANDLE event = ::WSACreateEvent();
	::WSAEventSelect(m_sClient, event, FD_CONNECT);

	// Connect to server
	nRetCode = ::connect(m_sClient, (sockaddr *)&m_Addr, sizeof(SOCKADDR_IN));

	if (SOCKET_ERROR == nRetCode && WSAEWOULDBLOCK != ::WSAGetLastError())
	{
		m_sClient = INVALID_SOCKET;
		return NET_ERROR;
	}
	
	DWORD dwRetCode;
	// wait for connet result
	dwRetCode = ::WSAWaitForMultipleEvents(1, &event, FALSE, INFINITE, FALSE);
	if (WAIT_OBJECT_0 != dwRetCode)
	{
		return NET_ERROR;
	}
	
	WSANETWORKEVENTS events;
	nRetCode = ::WSAEnumNetworkEvents(m_sClient, event, &events);
	if (!(events.lNetworkEvents & FD_CONNECT) ||
		(0 != events.iErrorCode[FD_CONNECT_BIT]))
	{
		// Socket connection fail
		return NET_ERROR;
	}

	::memset(m_pRcvBuffer, 0, NET_DATA_MSG_BUFSIZE);
	::memset(m_pSndBuffer, 0, NET_DATA_MSG_BUFSIZE);
	::memset(m_pMsgBuffer, 0, NET_DATA_BUFSIZE);	
	m_nRcvBytes			= 0;
	m_nSndBytes			= 0;	
	m_nTotalBytes		= sizeof(NET_MSG_GENERIC);
	m_nNetMode			= NET_PACKET_HEAD;
	m_nClientNetState	= nState;
	m_nOnline			= net::ONLINE;

	//CClientConsoleMessage::GetInstance()->WriteWithTime("Connected");	
    sc::writeLogInfo("Connected");
	
	return StartNetThread();
}

int CNetCtrl::StartNetThread()
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
								(unsigned int*) &dwThreadID );
	if (m_hThread == NULL)
	{
		// 쓰래드 생성에 실패하였습니다.
		//CClientConsoleMessage::GetInstance()->WriteWithTime("Create Thread Failed");
        sc::writeLogInfo("Create Thread Failed");
		return NET_ERROR;
	}
	else
	{
		//CClientConsoleMessage::GetInstance()->WriteWithTime("Create Thread Success");
        sc::writeLogInfo("Create Thread Success");
		return NET_OK;
	}
}

int CNetCtrl::CloseConnect(void)
{
	int nRetCode;
	if (m_sClient != INVALID_SOCKET && IsOnline())
	{
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
		// WSAAsyncSelect(m_sClient, m_hWnd, 0, 0);
		nRetCode = ::closesocket(m_sClient);
		m_sClient = INVALID_SOCKET;
		m_nOnline = net::OFFLINE;		
		StopNetThread(true);
	}
	TRACE("CNetCtrl::CloseConnect \n");
	return NET_OK;
}

int	CNetCtrl::StopNetThread(bool wait)
{
	DWORD dwRetCode;
	SetEvent(m_hEventKill);	
	if (wait && m_hThread != NULL)
	{
		dwRetCode = ::WaitForSingleObject(m_hThread, INFINITE);
		if (dwRetCode == WAIT_FAILED) return NET_ERROR;
		::CloseHandle(m_hThread); 
		m_hThread = NULL;
		if (WAIT_OBJECT_0 == dwRetCode)
		{
			return NET_OK;
		}
		return NET_ERROR;
	}
	return NET_OK;
}


int CNetCtrl::SendBuffer()
{
	int nRet = NET_OK;
	// 무한루프의 가능성 있음
	while (m_nSndBytes)
	{
		if(SendBuffer2() != NET_OK) nRet = NET_ERROR;

		Sleep( 1 );
	}

	return nRet;
}

int CNetCtrl::SendBuffer2()
{
	int nTotalSndBytes	= 0;
	int	nSndBytes		= 0;
	int	nRetCode		= 0;
	int	nSize			= 0;
	NET_MSG_GENERIC* nmg;
    	
    sc::CriticalSectionOwnerLock OwnerLock(m_CriticalSection);
	if (m_nSndBytes >= sizeof(NET_MSG_GENERIC))
	{
		nmg = (NET_MSG_GENERIC*) m_pSndBuffer;		
		nSize = nmg->Size();
		nSndBytes = ::send(m_sClient, 
					  	   (const char*) (m_pSndBuffer), 
						   nSize, 
						   0);
		if (nSndBytes == SOCKET_ERROR) // 전송 에러처리 
		{			
			nRetCode = ::WSAGetLastError();
			if (nRetCode == WSAEWOULDBLOCK)	// Send전송 버퍼에 병목현상 발생,  FD_WRITE 이벤트를 기다린다.
			{
				TRACE("WSAEWOULDBLOCK \n");
				sc::writeLogInfo("Send WSAEWOULDBLOCK");
				//LockOff();
				return NET_OK; // Send전송 버퍼에 병목현상 발생, 다음 FD_WRITE 이벤트 발생시에 다시 보낸다.
			}
			else
			{
				//LockOff();
				sc::writeLogInfo("Send SOCKET_ERROR");
				m_nOnline = net::OFFLINE;

				return NET_ERROR;
			}
		}
		else // 전송 성공
		{		
			if (nSndBytes == nSize) // 보내야할 패킷 정확히 전송성공
			{
				::memmove(m_pSndBuffer, m_pSndBuffer+nSndBytes, m_nSndBytes-nSndBytes);
				m_nSndBytes -= nSndBytes;
				// TRACE("%d Bytes send \n", nSndBytes);
			}
			else // 올바르게 전송하지 못함, work 이벤트를 발생시킴
			{
				//CClientConsoleMessage::GetInstance()->WriteWithTime("Send failed");
                sc::writeLogInfo("Send failed");
				::SetEvent(m_hEventWork);
			}
		}
	}

	//LockOff();
	return NET_OK;
}

int CNetCtrl::Send(NET_MSG_GENERIC* nmg)
{
	if (nmg == NULL) 
	{
		return NET_ERROR;
	}
	else
	{
		return Send((char*) nmg, nmg->Size());
	}
}

int	CNetCtrl::Send(char* buff, int nSize)
{
	NET_MSG_GENERIC* nmg;
	if ( !IsOnline() ) return 0;

	sc::CriticalSectionOwnerLock OwnerLock(m_CriticalSection);

	nmg = (NET_MSG_GENERIC*) buff;	

	::memcpy(m_pSndBuffer+m_nSndBytes, buff, nSize);
	m_nSndBytes += nSize;
	//LockOff();
	::SetEvent(m_hEventWork);
	return 0;
}

int CNetCtrl::Recv(void)
{
	int nRcvBytes = 0;
	int nRetCode = 0;
	char pBuff[NET_DATA_BUFSIZE];
	::memset(pBuff,0,NET_DATA_BUFSIZE);
	
	nRcvBytes = ::recv(m_sClient,
					pBuff,
                    NET_DATA_BUFSIZE, // sizeof(NET_MSG_GENERIC),
					0);
	if (nRcvBytes == SOCKET_ERROR)
	{
		TRACE("CNetClient::Recv recv SOCKET_ERROR \n");
		//CClientConsoleMessage::GetInstance()->WriteWithTime("CNetCtrl::Recv recv SOCKET_ERROR");
        sc::writeLogInfo("CNetCtrl::Recv recv SOCKET_ERROR");
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
			sc::CriticalSectionOwnerLock OwnerLock(m_CriticalSection);
			::memcpy(m_pRcvBuffer+m_nRcvBytes, pBuff, nRcvBytes);
			m_nRcvBytes += nRcvBytes;		
			//LockOff();
			// TRACE("Recv %d \n", m_nRcvBytes);
			return NET_OK;
		}
	}
}

char* CNetCtrl::IsValid(void)
{
	NET_MSG_GENERIC* nmg;
	int nSize = 0;
	// Head 를 다 받지 않았다면...
	sc::CriticalSectionOwnerLock OwnerLock(m_CriticalSection);
    if (m_nRcvBytes < sizeof(NET_MSG_GENERIC)) 
	{
		//LockOff();
		return NULL;
	}
	else
	{
		// Body 를 다 받았는지 검사한다.
		nmg = (NET_MSG_GENERIC*) m_pRcvBuffer;
		nSize = (int) nmg->Size();
		if (nSize <= 0)
		{
			//LockOff();
			return NULL;
		}
		// Body 를 다 받지 않았다면...
		if (m_nRcvBytes < nSize)
		{
			//LockOff();
			return NULL;
		}
		else
		{
			// Head 와 Body 를 모두 받았다면...
			// Head 와 Body 를 버퍼로 복사하고...
			::memset(m_pMsgBuffer, 0, NET_DATA_BUFSIZE);	
			::memcpy(m_pMsgBuffer, m_pRcvBuffer, nSize);
			// 옮겨진 버퍼만큼 땡긴다.	
			::memmove(m_pRcvBuffer, m_pRcvBuffer+nSize, m_nRcvBytes-nSize );
			m_nRcvBytes = m_nRcvBytes - nSize;	
			//LockOff();
			return m_pMsgBuffer;
		}
	}
}

// 주 메시지 처리 함수
void CNetCtrl::MessageProcess()
{
	char* pBuffer = NULL;
	NET_MSG_GENERIC* nmg;
	
	pBuffer = IsValid();

	while (pBuffer)
	{
		nmg = reinterpret_cast<NET_MSG_GENERIC*> (pBuffer);
		
		CServerManagerDlg* pDlg;
		pDlg = reinterpret_cast<CServerManagerDlg*> (m_pDlg);
		pDlg->MsgProcess(nmg);
		
		pBuffer = IsValid();		
		Sleep( 1 );
	}
}

// 서버에서 전송되어온 암호키
void CNetCtrl::MsgCryptKey(NET_MSG_GENERIC* nmg)
{	
	NET_CRYPT_KEY* nck;
	nck = (NET_CRYPT_KEY*) nmg;
	//m_ck.nKey			= nck->ck.nKey;
	//m_ck.nKeyDirection	= nck->ck.nKeyDirection;
	m_ck = nck->ck;
}

// 서버에서 전송되어온 버전 정보
int CNetCtrl::MsgVersionInfo(NET_MSG_GENERIC* nmg)
{
	NET_CLIENT_VERSION_SC* pPacket = (NET_CLIENT_VERSION_SC*) nmg;

	if( pPacket->m_PatchVersion == net::INVALID_COUNTRY && pPacket->m_LauncherVersion == net::INVALID_COUNTRY )
	{
		// GeoIP Block 접속이 허용되지 않은 국가이므로 접속 불가
	}

	return 0;
}

void CNetCtrl::MsgGameSvrInfoEnd(NET_MSG_GENERIC* nmg)
{
	m_bGameServerInfoEnd = TRUE;
}

// Login 서버에서 전송되어온 Game 서버 정보
void CNetCtrl::MsgGameSvrInfo(NET_MSG_GENERIC* nmg)
{
	G_SERVER_CUR_INFO_LOGIN gscil;
	NET_CUR_INFO_LOGIN* pPacket = (NET_CUR_INFO_LOGIN*) nmg;
	gscil = pPacket->gscil;
	
	if ((gscil.nServerGroup >= MAX_SERVER_GROUP) || (gscil.nServerNumber >= MAX_SERVER_NUMBER))
		return;

	TRACE("게임서버 정보 전송 받음 \n");
	//CClientConsoleMessage::GetInstance()->WriteWithTime("게임서버 정보 전송 받음");	
    sc::writeLogInfo("Received game servers information");
}

///////////////////////////////////////////////////////////////////////////////
// Message Send Function.
///////////////////////////////////////////////////////////////////////////////

// Client 의 버전 정보를 서버로 전송한다.
int CNetCtrl::SndVersion(int PatchVersion, int LauncherVersion)
{
	NET_CLIENT_VERSION_CS ncv(LauncherVersion, PatchVersion);
	return Send((NET_MSG_GENERIC*) &ncv);
}

int	CNetCtrl::SndVersion(void)
{	
	return 0; 
}

int CNetCtrl::SndReqAllSvrInfo(void)
{	
	NET_MSG_GENERIC nmg;	
	nmg.SetSize( sizeof(NET_MSG_GENERIC) );
	nmg.nType	= NET_MSG_REQ_ALL_SVR_INFO;	
	return Send(&nmg);
}

int CNetCtrl::SessionMsgHeartbeatServerAns()
{
	NET_HEARTBEAT_SERVER_ANS MsgHeartbeatServerAns;
    return Send( (NET_MSG_GENERIC*) &MsgHeartbeatServerAns );
}

// 게임서버의 정보를 서버에 요청한다.
int CNetCtrl::SndReqServerInfo(void)
{
	NET_MSG_GENERIC nmg;	
	nmg.SetSize( sizeof(NET_MSG_GENERIC) );
	nmg.nType	= NET_MSG_REQ_GAME_SVR;
	return Send(&nmg);
}

// 사용자 정보를 요청한다.
int CNetCtrl::SndRequestUserInfo(void)
{	
	NET_MSG_GENERIC nmg;	
	nmg.SetSize( sizeof(NET_MSG_GENERIC) );
	nmg.nType  = NET_MSG_REQ_USER_INFO; // 사용자 정보요청
	return Send(&nmg);
}

// Send to Session server... server command	
// NET_MSG_SVR_PAUSE	서버를 잠시 멈춘다.
// NET_MSG_SVR_RESUME	멈추어진 서버를 다시 가동시킨다.	
// NET_MSG_SVR_RESTART	서버를 완전히 멈추고 새롭게 가동시킨다.
// NET_MSG_SVR_STOP		서버를 완전히 멈춘다.
int CNetCtrl::SndSvrCmd(int nSvrGrp, int nSvrNum, int nFieldNum, int nSvrType, int nCmd)
{
    NET_SVR_CMD_CHANGE_STATE NetMsg;
    NetMsg.nCmdType  = nCmd;
	NetMsg.nSvrType  = nSvrType;
	NetMsg.nSvrGrp   = nSvrGrp;
	NetMsg.nSvrNum   = nSvrNum;
	NetMsg.nSvrField = nFieldNum;

	return Send(reinterpret_cast <char*> (&NetMsg), NetMsg.Size());
}

int CNetCtrl::SendRequestAuthSvrInfo()
{	
	NET_MSG_GENERIC nmg;	
	nmg.SetSize( sizeof(NET_MSG_GENERIC) );
	nmg.nType	= NET_MSG_REQ_AUTH_SVR_INFO;	
	return Send(&nmg);
}