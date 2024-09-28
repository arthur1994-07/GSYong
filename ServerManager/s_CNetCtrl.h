#ifndef S_NETCTRL_H_
#define S_NETCTRL_H_

#pragma once

#include <winsock2.h>
#include "../SigmaCore/Util/CBit.h"
#include "../RanLogic/Network/NetLogicDefine.h"

// #include "ServerManagerDlg.h"

#ifndef NET_CLIENT_IP
#define NET_CLIENT_IP	20
#endif

#ifndef NET_ERROR
#define NET_ERROR		-1
#endif

#ifndef NET_OK
#define NET_OK			1
#endif

struct NET_MSG_GENERIC;
class CServerManagerDlg;

class CNetCtrl
{
public:
	CNetCtrl(void);
	CNetCtrl(HWND hWnd);
	~CNetCtrl(void);

protected:
	CRITICAL_SECTION m_CriticalSection;

public:
	HWND		m_hWnd;
	SOCKET		m_sClient;
	SOCKADDR_IN	m_Addr;	
	sc::CBit	m_Bit;

	BOOL		m_bGameServerInfoEnd; ///< 서버리스트를 전부 받았는지 상태체크
	INT			m_nLoginResult;		  ///< 로그인 결과 세팅
	CRYPT_KEY	m_ck;
	// USHORT		m_nKeyDirection;	  // 암호키 방향
	// USHORT		m_nKey;			      // 암호키 shift 량
	BOOL		m_bWorldBattle; // 통합전장 입장인지

	char		m_cClientIP[NET_CLIENT_IP+1];
	char		m_cServerIP[NET_CLIENT_IP+1];
	char		m_szAppPath[1024];		
	int			m_cServerPort;

	int			m_nOnline; // m_nOnline 과 m_bIsRunning 의 역활이 중복이 될수 있음. 기존 소스 유지를 위해서 병행 사용
	BOOL		m_bIsRunning; // m_nOnline 과 m_bIsRunning 의 역활이 중복이 될수 있음. 기존 소스 유지를 위해서 병행 사용

	int			m_nClientNetState;
	char*		m_pSndBuffer;
	char*		m_pRcvBuffer;
	char*		m_pMsgBuffer;
	
	int			m_nRcvBytes;
	int			m_nSndBytes;
	int			m_nTotalBytes;
	int			m_nNetMode;

	HANDLE		m_hEvent;
	HANDLE		m_hThread;
	HANDLE		m_hEventWait;
	HANDLE		m_hEventWork;
	HANDLE		m_hEventKill;
	CServerManagerDlg* m_pDlg;
	
public:
	//void	LockOn();
	//void	LockOff();
	int		StopNetThread(bool wait);
	void	NetThreadProc();
	int		StartNetThread();
	int		NetProc();
	void	SetWndHandle(HWND hWnd);
	void	SetCtrlHandle(CServerManagerDlg* pDlg);
	void	SetDefault();
	BOOL	IsOnline();
	int		ConnectServer(const char* cServerIP,	int nPort=5003,				int nState=NET_STATE_CTRL);	
	int		ConnectServer(unsigned long cServerIP,	unsigned short nPort=5003,	int nState=NET_STATE_CTRL);
	int		ReConnectServer(int nState=NET_STATE_CTRL);	

	int		CloseConnect(void);
	int		SendBuffer();
	int		SendBuffer2();
	int     Send(NET_MSG_GENERIC* nmg);
	int		Send(char* buff, int nSize);
	int		Recv(void);
	char*	IsValid(void);
	void	MessageProcess();

	int		SndPing(void);
	int		SndRequestUserInfo(void);
	int		SndReqServerInfo(void);
	int		SndReqAllSvrInfo(void);
	int		SndVersion(void);
	int		SndVersion(int PatchVersion, int LauncherVersion);
	int		SndSvrCmd(int nSvrGrp, int nSvrNum, int nFieldNum, int nSvrType, int nCmd);
	int		SendRequestAuthSvrInfo();
	
	int		SessionMsgHeartbeatServerAns();

	void	MsgGameSvrInfo(NET_MSG_GENERIC* nmg);
	void	MsgGameSvrInfoEnd(NET_MSG_GENERIC* nmg);
	int		MsgVersionInfo(NET_MSG_GENERIC* nmg);
	void	MsgPingAnswer(NET_MSG_GENERIC* nmg);
	void	MsgCryptKey(NET_MSG_GENERIC* nmg);
};

#endif // S_NETCTRL_H_
