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
	//m_ck.nKeyDirection		= 0;	// ��ȣŰ ����
	//m_ck.nKey				= 0;	// ��ȣŰ shift ��
	
	m_bGameServerInfoEnd	= FALSE;
	m_nLoginResult			= 0; // �α��� ��� ����		
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
	
	HANDLE hEventSocket = ::WSACreateEvent(); // �����̺�Ʈ ����

	// ���� �̺�Ʈ ���
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
		//�̺�Ʈ�� �߻��Ҷ����� ��ӱ�ٸ���.
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
					// ������ �����ϰ� �ٽ� �翬���� �ؾ��ϴµ� �ϴ� Ȯ���� ���ؼ� �α� ����
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
							// ������ �����ϰ� �ٽ� �翬���� �ؾ��ϴµ� �ϴ� Ȯ���� ���ؼ� �α� ����
							sc::writeLogInfo("SendBuffer ERROR [events.lNetworkEvents & FD_WRITE]");
						}
					}
					else if (events.lNetworkEvents & FD_CLOSE)
					{						
						CloseConnect();
						// ���������
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
	// ���� Ȯ���� ���� �ܼ� �α�. ����ȭ ���� ���� ���
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
	// Winsock �� ���۸��� ���̰� cpu ��뷮�� ���̷��� �Ʒ� �ɼ��� Ȱ���� ��Ű�� ��.
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
		// ������ ������ �����Ͽ����ϴ�.
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
	// ���ѷ����� ���ɼ� ����
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
		if (nSndBytes == SOCKET_ERROR) // ���� ����ó�� 
		{			
			nRetCode = ::WSAGetLastError();
			if (nRetCode == WSAEWOULDBLOCK)	// Send���� ���ۿ� �������� �߻�,  FD_WRITE �̺�Ʈ�� ��ٸ���.
			{
				TRACE("WSAEWOULDBLOCK \n");
				sc::writeLogInfo("Send WSAEWOULDBLOCK");
				//LockOff();
				return NET_OK; // Send���� ���ۿ� �������� �߻�, ���� FD_WRITE �̺�Ʈ �߻��ÿ� �ٽ� ������.
			}
			else
			{
				//LockOff();
				sc::writeLogInfo("Send SOCKET_ERROR");
				m_nOnline = net::OFFLINE;

				return NET_ERROR;
			}
		}
		else // ���� ����
		{		
			if (nSndBytes == nSize) // �������� ��Ŷ ��Ȯ�� ���ۼ���
			{
				::memmove(m_pSndBuffer, m_pSndBuffer+nSndBytes, m_nSndBytes-nSndBytes);
				m_nSndBytes -= nSndBytes;
				// TRACE("%d Bytes send \n", nSndBytes);
			}
			else // �ùٸ��� �������� ����, work �̺�Ʈ�� �߻���Ŵ
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
	// Head �� �� ���� �ʾҴٸ�...
	sc::CriticalSectionOwnerLock OwnerLock(m_CriticalSection);
    if (m_nRcvBytes < sizeof(NET_MSG_GENERIC)) 
	{
		//LockOff();
		return NULL;
	}
	else
	{
		// Body �� �� �޾Ҵ��� �˻��Ѵ�.
		nmg = (NET_MSG_GENERIC*) m_pRcvBuffer;
		nSize = (int) nmg->Size();
		if (nSize <= 0)
		{
			//LockOff();
			return NULL;
		}
		// Body �� �� ���� �ʾҴٸ�...
		if (m_nRcvBytes < nSize)
		{
			//LockOff();
			return NULL;
		}
		else
		{
			// Head �� Body �� ��� �޾Ҵٸ�...
			// Head �� Body �� ���۷� �����ϰ�...
			::memset(m_pMsgBuffer, 0, NET_DATA_BUFSIZE);	
			::memcpy(m_pMsgBuffer, m_pRcvBuffer, nSize);
			// �Ű��� ���۸�ŭ �����.	
			::memmove(m_pRcvBuffer, m_pRcvBuffer+nSize, m_nRcvBytes-nSize );
			m_nRcvBytes = m_nRcvBytes - nSize;	
			//LockOff();
			return m_pMsgBuffer;
		}
	}
}

// �� �޽��� ó�� �Լ�
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

// �������� ���۵Ǿ�� ��ȣŰ
void CNetCtrl::MsgCryptKey(NET_MSG_GENERIC* nmg)
{	
	NET_CRYPT_KEY* nck;
	nck = (NET_CRYPT_KEY*) nmg;
	//m_ck.nKey			= nck->ck.nKey;
	//m_ck.nKeyDirection	= nck->ck.nKeyDirection;
	m_ck = nck->ck;
}

// �������� ���۵Ǿ�� ���� ����
int CNetCtrl::MsgVersionInfo(NET_MSG_GENERIC* nmg)
{
	NET_CLIENT_VERSION_SC* pPacket = (NET_CLIENT_VERSION_SC*) nmg;

	if( pPacket->m_PatchVersion == net::INVALID_COUNTRY && pPacket->m_LauncherVersion == net::INVALID_COUNTRY )
	{
		// GeoIP Block ������ ������ ���� �����̹Ƿ� ���� �Ұ�
	}

	return 0;
}

void CNetCtrl::MsgGameSvrInfoEnd(NET_MSG_GENERIC* nmg)
{
	m_bGameServerInfoEnd = TRUE;
}

// Login �������� ���۵Ǿ�� Game ���� ����
void CNetCtrl::MsgGameSvrInfo(NET_MSG_GENERIC* nmg)
{
	G_SERVER_CUR_INFO_LOGIN gscil;
	NET_CUR_INFO_LOGIN* pPacket = (NET_CUR_INFO_LOGIN*) nmg;
	gscil = pPacket->gscil;
	
	if ((gscil.nServerGroup >= MAX_SERVER_GROUP) || (gscil.nServerNumber >= MAX_SERVER_NUMBER))
		return;

	TRACE("���Ӽ��� ���� ���� ���� \n");
	//CClientConsoleMessage::GetInstance()->WriteWithTime("���Ӽ��� ���� ���� ����");	
    sc::writeLogInfo("Received game servers information");
}

///////////////////////////////////////////////////////////////////////////////
// Message Send Function.
///////////////////////////////////////////////////////////////////////////////

// Client �� ���� ������ ������ �����Ѵ�.
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

// ���Ӽ����� ������ ������ ��û�Ѵ�.
int CNetCtrl::SndReqServerInfo(void)
{
	NET_MSG_GENERIC nmg;	
	nmg.SetSize( sizeof(NET_MSG_GENERIC) );
	nmg.nType	= NET_MSG_REQ_GAME_SVR;
	return Send(&nmg);
}

// ����� ������ ��û�Ѵ�.
int CNetCtrl::SndRequestUserInfo(void)
{	
	NET_MSG_GENERIC nmg;	
	nmg.SetSize( sizeof(NET_MSG_GENERIC) );
	nmg.nType  = NET_MSG_REQ_USER_INFO; // ����� ������û
	return Send(&nmg);
}

// Send to Session server... server command	
// NET_MSG_SVR_PAUSE	������ ��� �����.
// NET_MSG_SVR_RESUME	���߾��� ������ �ٽ� ������Ų��.	
// NET_MSG_SVR_RESTART	������ ������ ���߰� ���Ӱ� ������Ų��.
// NET_MSG_SVR_STOP		������ ������ �����.
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