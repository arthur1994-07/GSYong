#ifndef S_CSERVER_H_
#define S_CSERVER_H_

#pragma once

#include "../../SigmaCore/Util/Lock.h"
#include "../../SigmaCore/Net/MsgList.h"
#include "../../SigmaCore/Util/CBit.h"
#include "../../SigmaCore/Log/LogMan.h"
#include "../../SigmaCore/String/StringFormat.h"
#include "../../SigmaCore/Util/SystemInfo.h"

#include "../../EngineLib/G-Logic/GLDefine.h"
#include "../../EngineLib/DxTools/DxFontDefine.h"
//#include "../../RanLogic/Util/s_CSystemInfo.h"
#include "../../RanLogic/Util/s_CWhiteRock.h"
#include "../../RanLogic/Character/GLCharDefine.h"
#include "../../RanLogic/Network/NetLogicDefine.h"

#include "../../BugTrap/BugTrap.h"

//
//mjeon.tbb
//
#ifdef _USE_TBB
#include "../../=TBB/include/tbb/task_scheduler_init.h"
#endif

//
//mjeon.dbman
//
namespace db {
    class IDbManager;
}

namespace sc
{
    namespace net
    {
        class Overlapped;
    }
}

class ServerConfigXml;
class SUBPATH;
class IpAddressChecker;

#define S_HEURISTIC_NUM				1	//! IOCP Worker thread number per CPU 
#define S_HEURISTIC_QUEUE_SIZE		20	//! 큐사이즈는 기본 최대사용자 X S_HEURISTIC_QUEUE_SIZE 배를 형성한다.

#define S_SERVER_STOP				1
#define S_SERVER_RUNING				2
#define S_SERVER_PAUSE				3

#define CHA_SAVE_TIME				1800000	//! 1000 * 60 * 30	캐릭터 저장시간   30분
#define CHA_SAVE_CHK_TIME			600000	//! 1000 * 60 * 10	캐릭터 체크시간   10분
#define FPS_UPDATE_TIME				60000	//! 1000 * 60		FPS 표시 갱신시간  1분
#define SERVER_STATE_CHK_TIME		3600000	//! 1000 * 60 * 60	서버 상태 로그 기록 시간  60분
//#define HEARTBEAT_TIME			3600000	//! 1000 * 60 * 60	Heart beat time 60분
#define HEARTBEAT_TIME				120000	//! 1000 * 60 * 2	Heart beat time    2분
#define SESSION_CHECK_TIME			300000	//! 1000 * 60 * 5	Session 상태 체크 5분

//
//mjeon
//
#define FREQUENCY_UPDATE_SYSTEMINFO 300000	// 1000 * 60 * 5	시스템정보(메모리 현황 등) 출력 주기
#define FREQUENCY_UPDATE_MATCHINFO	60000	// 1000 * 60		매칭시스템 상태 출력 주기


//tickcount에 대해 a > b 인지를 판단한다. (설사 a가 overflow되서 wrap되더라도 원하는 결과를 얻을 수 있다.)
#define time_after(a,b) ((int)(b) - (int)(a) < 0)

#define SAFE_CLOSE_HANDLE(P) { if(P) { CloseHandle(P); P = INVALID_HANDLE_VALUE; } }

#ifndef _RELEASED
#define fRESERVATIONTIME 30.0f
#else
#define fRESERVATIONTIME 0.0f
#endif

#define SESSION_CONNECTION_CHECK_TIME	60000


// * History
// 2002.05.30 jgkim Create
//
// Copyright (c) Mincoms. All rights reserved.
class NetServer : public sc::DefaultLock
{
public:
	NetServer(HWND hWnd, HWND hEditBox, HWND hEditBoxInfo, SUBPATH* pPath, EMSERVICE_PROVIDER ServiceProvider);
	virtual ~NetServer();

	typedef std::vector<STracingData> VEC_TRACING;
	typedef VEC_TRACING::iterator	  VEC_TRACING_ITER;

protected:
    SUBPATH* m_pPath;
    db::IDbManager* m_pDbManager; //mjeon.dbman
	db::IDbManager* m_pAdoManager;//mjeon.dbman.ODBC와 ADO를 겸해서 사용하기 위해 AdoManager를 명시적으로 하나 생성한다.
	sc::net::MsgManager* m_pRecvMsgManager;
	sc::CBit m_Bit;
    IpAddressChecker* m_pAddressChecker;

    //IServerCfg* m_pConfigOld;

	DWORD				m_dwHeartBeatTime;
	DWORD				m_dwFrameTime;
	DWORD				m_dwFrameCount;
	
	DWORD m_dwCpuCount; ///< 시스템의 CPU 갯수

	HANDLE				m_hWorkerThread[MAX_WORKER_THREAD];
	HANDLE				m_hIOServer;			// IOCP Service 	
	HANDLE				m_hAcceptThread;		// Listening thread handle

	HANDLE				m_hUpdateThread;		// Game Update thread handle
	HANDLE				m_hUpdateQuitEvent;
	DWORD				m_dwUpdateWaitTime;
	DWORD				m_dwUpdateOldTime;
	DWORD				m_dwUpdateFrame;

	HANDLE				m_hRegularScheduleThread;		// 게임가드 스레드
	HANDLE				m_hRegularScheduleQuitEvent;

	HWND				m_hEditBox;				// Main window, EditBox handle
	HWND				m_hEditBoxInfo;			// Main window, Info EditBox handle
	HWND				m_hWnd;					// Main window handle
	
	// SOCKADDR_IN			m_Addr;
	SOCKET				m_sServer;				// Socket

	bool				m_bTestServer;			// Test Server?
	bool				m_bAdult;				// Server for adult

	//mjeon.volatile
	volatile bool		m_bIsRunning;			// Status flag

	bool				m_bUpdateEnd;			// Update thread end flag
	bool				m_bPK;					// true:PK ON, false:PK OFF
	bool                m_bPKLess;              /// All attack between chatacter disable
	int					m_nStatus;				// Detail status flag
	int					m_nPort;				// Server port

	int					m_PatchVersion;			// Patch version
	int					m_LauncherVersion;		// Launcer Version
    int                 m_PatchCutVersion;           // Version Cut ( 설정된 버전 이하는 접속할 수 없다. )

	int					m_nServerGroup;			// Server Group
	int					m_nServerNum;			// Server Number

	int					m_nMaxClient;			// Maximum numbers of client
	int                 m_nMaxChannelUser;      // Maximum numbers of channel client
	
	//! Service provider
    //! 주의! 생성될때 넘어온 값이 있더라도 설정파일에서 새롭게 읽게되면 값이 바뀐다.
    EMSERVICE_PROVIDER	m_ServiceProvider;
	char				m_szAddress[MAX_IP_LENGTH+1];			// Server IP Address
	char				m_pRecvBuffer[NET_DATA_BUFSIZE];

	DWORD				m_dwWorkerThreadNumber;	// Worker Thread 의 갯수
	DWORD				m_dwCompKey;

	bool				m_bUseEventThread;
	UINT				m_nFPS;					//mjeon.Server FPS

	language::LANGFLAG m_LanguageSet;

	//std::vector< CString > m_vecAllowIPType1;
	//std::vector< CString > m_vecAllowIPType2;
	//std::vector< CString > m_vecAllowIPType3;
	bool					m_bAllowIP;

	// int					m_nPacketInCount;			// Inbound  Packet Counter
	// int					m_nPacketInSize;			// Inbound  Packet Size (Byte)
	// int					m_nPacketOutCount;			// Outbound Packet Counter
	// int					m_nPacketOutSize;			// Outbound Packet Size (Byte)

	DWORD				m_dwLastDropClient;		    // Last Drop Client;

	bool                m_bGameGuardInit;           // nProtect GameGuard 초기화 성공여부
	bool                m_bGameGuardAuth;           // nProtect GameGuard 인증 사용여부

	VEC_TRACING			m_vecTracingData;           // 추척 캐릭터들 리스트

	char				m_szEncrypt[ENCRYPT_KEY+1];
	
public:
    //! -----------------------------------------------------------------------
    //! Virtual functions
    //! -----------------------------------------------------------------------
	virtual int	Start()   = 0;
	virtual int	Stop()    = 0;
	//virtual	int	Pause()   = 0;
	//virtual	int	Resume()  = 0;
	//virtual	int	ReStart() = 0;	
    virtual	int	StartClientManager() = 0;
    virtual	int	WorkProc()        = 0; // I/O Process
    virtual int	ListenProc()      = 0; // Accept new Client connection
    virtual int	UpdateProc()      = 0; // Message Process
	virtual int	RegularScheduleProc()   = 0; // 정기적으로 시행되어야 할 작업을 걸어둠 ( APEX 체크등 )
    //virtual	int DatabaseProc()    = 0; // Database thread function
    //virtual int LogDatabaseProc() = 0; // Log Database thread function
    //virtual int UserDatabaseProc()= 0; // User Database thread function
    //virtual int WebDatabaseProc() = 0; // Web Database thread function
    virtual int	InsertMsg(int nClient, void* pMsg) = 0;
    
    // I/O operation memory
    virtual PER_IO_OPERATION_DATA* GetFreeOverIO(EMMSG_IO_TYPE nType=NET_SEND_POSTED) = 0;
    virtual void ReleaseOperationData(PER_IO_OPERATION_DATA* pData) = 0;

	virtual void ForceStop( DWORD Reason ) = 0;

	EMSERVICE_PROVIDER GetServiceProvider() const { return m_ServiceProvider; }
    //! -----------------------------------------------------------------------
    //!
    //! -----------------------------------------------------------------------
    int ServerGroup() const { return m_nServerGroup; };
    int ServerNum() const { return m_nServerNum; };

    int	StartCfg(ServerConfigXml* pConfig);

    int	StartMsgManager(int MaxClient);
    int	StartIOCP();			
    virtual int StartWorkThread(ServerConfigXml* pConfig);
    virtual int	StartUpdateThread();
	virtual int	StartRegularScheduleThread();
    
	int	StartListenThread();
    int	StopListenThread();
    
    int	StopWorkThread();
    int	StopIOCP();

    int	GetPatchVersion() const { return m_PatchVersion; }
    int	GetLauncherVersion() const { return m_LauncherVersion; }    
    int	GetPatchCutVersion() const { return m_PatchCutVersion; }    
    void setUpdateFrame(DWORD dwFrame);
    bool IsRunning() const { return m_bIsRunning; }
    int	GetStatus() const { return m_nStatus; }
    void SetStatus(int nStatus) { m_nStatus = nStatus; }

	// 메모리 단편화를 방지해기 위해서 LFH 사용하도록 설정
	// http://msdn.microsoft.com/en-us/library/aa366750.aspx
	BOOL CheckLowFragmentationHeap();

	//현재 프로세스의 WorkSet size를 로그/콘솔로 출력한다. 차 후, 추가적인 시스템 정보를 출력할 필요가 있다면 여기에 추가하도록 하자.
	virtual void PrintSystemInfo();
};

static unsigned int WINAPI CServerListenProc(void* pThis)
{ 
	NetServer* pTemp = (NetServer*) pThis;
	return pTemp->ListenProc();
}

static unsigned int WINAPI CServerWorkProc(void* pThis)
{
	BT_SetTerminate();
	//
	//mjeon.tbb
	// all threads that uses tbb MUST init the scheduler.
	//
#ifdef _USE_TBB
	tbb::task_scheduler_init init;
#endif

	NetServer* pTemp = (NetServer*) pThis;
	return pTemp->WorkProc();
}

static unsigned int WINAPI CServerUpdateProc(void* pThis)
{
	BT_SetTerminate();
	//
	//mjeon.tbb
	// all threads that uses tbb MUST init the scheduler.
	//
#ifdef _USE_TBB
	tbb::task_scheduler_init init;
#endif

	NetServer* pTemp = (NetServer*) pThis;
	return pTemp->UpdateProc();
}

static unsigned int WINAPI CServerRegularScheduleProc(void* pThis)
{
	BT_SetTerminate();

#ifdef _USE_TBB
	tbb::task_scheduler_init init;
#endif

	NetServer* pTemp = (NetServer*) pThis;
	return pTemp->RegularScheduleProc();
}
/*
static unsigned int WINAPI CServerDatabaseProc(void* pThis)
{
	NetServer* pTemp = (NetServer*) pThis;
	return pTemp->DatabaseProc(); 
}

static unsigned int WINAPI CServerLogDatabaseProc(void* pThis)
{
	NetServer* pTemp = (NetServer*) pThis;
	return pTemp->LogDatabaseProc();
}

static unsigned int WINAPI CServerUserDatabaseProc(void* pThis)
{
	NetServer* pTemp = (NetServer*) pThis;
	return pTemp->UserDatabaseProc();
}

static unsigned int WINAPI CServerWebDatabaseProc(void* pThis)
{
	NetServer* pTemp = (NetServer*) pThis;
	return pTemp->WebDatabaseProc();
}
*/
#endif // S_CSERVER_H_
