#ifndef S_CLoginServer_H_
#define S_CLoginServer_H_

#pragma once
#include "../../=GeoIPLib/GeoIP.h"
#include "../../RanLogic/s_NetGlobal.h"
#include "./NetServer.h"

class NetLoginClientMan;
class LoginServerConfigXml;
struct NET_MSG_GENERIC;

namespace sc {
	class CGlobalAuthClientLogic;
}


//! 2002-05-30 Jgkim
//! 2011-04-08 Jgkim 설정파일 xml 로 변경 
class CLoginServer : public NetServer
{
public:
	CLoginServer(HWND hWnd, HWND hEditBox, HWND hEditBoxInfo, SUBPATH* pPath, EMSERVICE_PROVIDER ServiceProvider);
	virtual ~CLoginServer();

protected:
	//! Data member for login server
	// SOCKET				m_sSession;	///< Socket for session server
	NetLoginClientMan* m_pClientManager; ///< Client Manager
	bool m_bInspecting; ///< 서버 점검중 체크 변수
    LoginServerConfigXml* m_pConfig;

	sc::CGlobalAuthClientLogic* m_pAuthManager; // for global auth
		
	DWORD m_dwLastGameSvrUpdateTime;
	G_SERVER_CUR_INFO_LOGIN m_sGame[MAX_SERVER_GROUP][MAX_CHANNEL_NUMBER];
    DWORD m_SessionServerSlot; //! Session Server 연결 slot

	
	// GeoIP를 이용하여 국가별로 접속(클라이언트)을 허용할지 말지를 결정한다.
	GeoIP* m_pGeoIP;
	std::set<std::string> m_setCountryAccessApprove; // 허용할 국가들

public:
    //! -----------------------------------------------------------------------
    //! NetServer override functions 
    //! -----------------------------------------------------------------------
	virtual int	Start() override;
	virtual int	Stop() override;
	virtual void ForceStop( DWORD Reason ) override;

	virtual int StartClientManager() override;	
	virtual int	WorkProc() override;
	virtual int	UpdateProc() override;
    virtual int ListenProc() override;
	virtual int	RegularScheduleProc() override;
	//virtual	int DatabaseProc() override { return 0; }; // Database thread function
	//virtual	int LogDatabaseProc() override { return 0; }; // Log Database thread function
	//virtual	int UserDatabaseProc() override { return 0; }; // User Database thread function
	//virtual int	 WebDatabaseProc()  override { return 0; }; // Web Database thread function
    virtual int InsertMsg(int nClient, void* pMsg) override { return 0; };

    // I/O operation memory
    virtual PER_IO_OPERATION_DATA* GetFreeOverIO(EMMSG_IO_TYPE nType=NET_SEND_POSTED) override;
    virtual void ReleaseOperationData(PER_IO_OPERATION_DATA* pData) override;

    void UpdateProcDisplay(float fFrame);

    //! -----------------------------------------------------------------------
    //!
    //! -----------------------------------------------------------------------
	//void	Send(DWORD nClient, PER_IO_OPERATION_DATA* PerIoData, DWORD dwSize);	
    int		SendClient(DWORD dwClient, NET_MSG_GENERIC* pMsg);
	void	CloseClient(DWORD dwClient);
	void	CloseAllClient();

	void	SetInspecting(bool bStatus);
	bool	GetInspection();

	void	SetVersion(int PatchVer, int LauncherVer, int PatchCutVersion);	

	void	GenerateEncrypt();

	bool	CheckAllowIP( SOCKET socket );

	void	AddCountryAccessApprove( int nIndex );
	bool	CountryAccessApprove( SOCKET socket );

	void SetAllowIP(bool bAllowIP);
	bool GetAllowIP();
    void ClearBlockIp();


	//! -----------------------------------------------------------------------
	//! Msg Function Start 
	//! -----------------------------------------------------------------------
public:
	struct LoginPacketFunc
	{
		(void) (CLoginServer::*Msgfunc) (DWORD dwClient, NET_MSG_GENERIC* nmg);
		LoginPacketFunc()
		{
			Msgfunc = NULL;
		}
	};
	LoginPacketFunc m_MsgFunc[NET_MSG_GCTRL_MAX];

	void initPacketFunc();

public:
	int MsgProcess(MSG_LIST* pMsg);
	void ServerMsgProcess(MSG_LIST* pMsg);

public:
	//! 클라이언트 버전정보
	void MsgVersion(DWORD dwClient, NET_MSG_GENERIC* nmg);

	//! 클라이언트->Login Server
	void MsgHeartbeatClientAns(DWORD dwClient, NET_MSG_GENERIC* nmg);
	
	//! 게임서버정보를 클라이언트에 전송한다.
	void MsgSndGameSvrInfo(DWORD dwClient, NET_MSG_GENERIC* nmg); 
//	void MsgSndPingAnswer(MSG_LIST* pMsg);

	void SessionMsgSvrInfo(NET_MSG_GENERIC* nmg);
	void SessionMsgSvrInfoReset(NET_MSG_GENERIC* nmg);
	void SessionMsgSvrCmd(NET_MSG_GENERIC* nmg);
	void SessionMsgHeartbeatServerAns();

	void SessionSndSvrInfo();
	void SessionSndSvrChannelFullInfo(NET_MSG_GENERIC* nmg);

	void SessionSndSvrLoginMaintenance(NET_MSG_GENERIC* nmg);

    void SessionMsgVersionSS(NET_MSG_GENERIC* nmg);

	//////////////////////////////////////////////////////////////////////////////
	/// Execute Command Text
	//////////////////////////////////////////////////////////////////////////////
	int	ExecuteCommand(char* strCmd);

	//////////////////////////////////////////////////////////////////////////////
	/// Session Server Communication Member Function	
	//////////////////////////////////////////////////////////////////////////////
	int SessionConnect();
	int	SessionConnect(const char* szServerIP, int nPort);

	//! Session 서버에 접속하고 서버의 정보를 전송한다.	
	int SessionConnectSndSvrInfo(void);

	void SessionCloseConnect(void);
	int SendSession(NET_MSG_GENERIC* pBuffer);
	void SessionMsgProcess(MSG_LIST* pMsg);	
	
	//! Ping Answer
	void SessionMsgPingAnswer(NET_MSG_GENERIC* nmg); 
    void SessionSndHeartbeat(void);

	//! Send ping
	// void SessionReqPing(void); 
	//! Request Game Server Info
	void SessionReqSvrInfo(void); 
	void SessionSndKey(void);

    //! Patch/Launcher 버전을 전송한다.
    void SessionSndVersion();

    void DoNothing(NET_MSG_GENERIC* pMsg);

public:
	int SendAuthInfoToSessionServer(); // 세션서버로 인정 정보 전송. 세션에서 인증서버로 릴레이
	void MsgAuthCertificationResponse(NET_MSG_GENERIC* pMsg);
	void MsgCountryAccessApprove(NET_MSG_GENERIC* pMsg);
};

#endif // S_CLoginServer_H_