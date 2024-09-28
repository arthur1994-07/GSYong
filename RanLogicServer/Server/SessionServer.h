#ifndef S_SESSIONSERVER_H_
#define S_SESSIONSERVER_H_

#pragma once

#include "../../SigmaCore/String/StringFormat.h"
#include "../../SigmaCore/Log/LogMan.h"

#include "../../RanLogic/Network/NetLogicDefine.h"
#include "../Client/NetServerDefine.h"
#include "../Database/DBAction/DbExecuter.h"
#include "./NetServer.h"

class NetSessionClientMan;
class SessionServerConfigXml;
struct NET_SERVER_CHANNEL_INFO;
struct NET_MSG_GENERIC;
namespace sc {
	class CGlobalAuthClientLogic;
}

//! 2002-05-30 jgkim
class CSessionServer : public NetServer
{
public:
	CSessionServer(HWND hWnd, HWND hEditBox, HWND hEditBoxInfo, SUBPATH* pPath, EMSERVICE_PROVIDER ServiceProvider);
	virtual ~CSessionServer();

protected:
	NetSessionClientMan*	m_pClientManager; //! Client Manager
    SessionServerConfigXml* m_pConfig;
	G_SERVER_CUR_INFO_LOGIN m_sServerChannel[MAX_SERVER_GROUP][MAX_CHANNEL_NUMBER];
	bool					m_bServerChannelFull[MAX_SERVER_GROUP][MAX_CHANNEL_NUMBER];
	int						m_dwLoginServerID;	

	DWORD					m_nAuthServerSlot; // 인증서버로 연결된 클라이언트ID
	bool					m_bConnectedAuthServer;
	int						m_nSessionSvrID;
	sc::CGlobalAuthClientLogic* m_pAuthManager; // for global auth
	BOOL					m_bReconnect;
public:
    //! -----------------------------------------------------------------------
    //! NetServer override functions 
    //! -----------------------------------------------------------------------
	virtual int	Start() override;
	virtual int	Stop() override;
	virtual void ForceStop( DWORD Reason ) override;

    virtual int StartClientManager() override;
    virtual int WorkProc() override;
    virtual int ListenProc() override;
    virtual int UpdateProc() override;
	virtual int	RegularScheduleProc() override;
    //virtual	int DatabaseProc() override;
    //virtual	int LogDatabaseProc() override { return 0; }; //! Log Database thread function
    //virtual	int UserDatabaseProc() override { return 0; };	//! User Database thread function
    //virtual int WebDatabaseProc() override { return 0; }; // Web Database thread function
    virtual int InsertMsg(int nClient, void* pMsg);
    
    // I/O operation memory
    virtual PER_IO_OPERATION_DATA* GetFreeOverIO(EMMSG_IO_TYPE nType=NET_SEND_POSTED) override;
    virtual void ReleaseOperationData( PER_IO_OPERATION_DATA* pData ) override;

    //! -----------------------------------------------------------------------
    int StartDbManager();
    void Send(DWORD dwClient, PER_IO_OPERATION_DATA* PerIoData, DWORD dwSize);    
    int SendClient(DWORD dwClient, NET_MSG_GENERIC* pMsg);    
    void SendAllClient(NET_MSG_GENERIC* pMsg);
    
    void CloseClient(DWORD dwClient);
    void MsgCloseClient(DWORD dwClient, MSG_LIST* pMsg);

    //! close all client connections
    void CloseAllClient();
    int  FindServer(CString strIP, int nPort);
    int  FindServer(int nSvrType, int nSvrGrp, int nSvrNum, int nFieldNum);

	int FindServer(int nSvrType, int nIndex = 0); // 원하는 서버타입중에 n번째 서버를 얻어옴 ( GetServerTypeCount를 이용해서 타입 총 수 구함 )
	int GetServerTypeCount(int nSvrType);

    void UserInsert(const CString& strUsrID, LPCNT_USR_INFO pData);
    void SetSvrInfo(DWORD dwClient, G_SERVER_INFO *gsi);
    void SetChannelInfo(NET_SERVER_CHANNEL_INFO* pMsg);			

    //! -----------------------------------------------------------------------
    //! Message process functions
    //! -----------------------------------------------------------------------
public:
	struct SessionPacketFunc
	{
		(void) (CSessionServer::*Msgfunc) (DWORD dwClient, MSG_LIST* pMsg);
		SessionPacketFunc()
		{
			Msgfunc = NULL;
		}
	};
	SessionPacketFunc m_MsgFunc[NET_MSG_GCTRL_MAX];

	void initPacketFunc();

public:
    int  MsgProcess				(MSG_LIST* pMsg);
    void MsgChatProcess			(DWORD dwClient, MSG_LIST* pMsg);
    //! Control program->Session server : command message
    void MsgSvrCmdProcess       (DWORD dwClient, MSG_LIST* pMsg);

	//< 추적할 캐릭터를 특정 캐릭터만 업데이트한다.
    void MsgTracingChar			(DWORD dwClient, MSG_LIST* pMsg);

	//< 추적할 캐릭터를 전부 업데이트한다.
	void MsgTracingCharAll		(DWORD dwClient, MSG_LIST* pMsg);
    void MsgTracingLoginout		(MSG_LIST* pMsg);

	//! Hearbeat Answer
    void MsgHeartbeatServerAns(DWORD dwClient, MSG_LIST* pMsg);
    //! Hearbeat Request
    void MsgServerHeartbeatReq(DWORD dwClient, MSG_LIST* pMsg);

    void MsgSndCurSvrInfo		(DWORD dwClient, MSG_LIST* pMsg);
    void MsgSndSvrInfoReset		(DWORD nClient);
    void MsgSndCurSvrInfoToAll	(void);
    void MsgSndAllSvrInfo		(DWORD dwClient, MSG_LIST* pMsg);
    void MsgSndChatGlobal		(char* szChatMsg);
    void MsgSndPingAnswer		(MSG_LIST* pMsg);	

    void MsgSndLoginResult		(MSG_LIST* pMsg);	
    //void MsgSndLoginResultBack  (DWORD dwClient, NET_LOGIN_FEEDBACK_DATA2* pData);

    void DaumMsgSndLoginResult   (MSG_LIST* pMsg);
    //void DaumMsgSndLoginResultBack(DWORD dwClient, DAUM_NET_LOGIN_FEEDBACK_DATA2* pData);

    void MsgServerInfo			(DWORD dwClient, MSG_LIST* pMsg);
    void MsgEncryptKey			(DWORD dwClient, MSG_LIST* pMsg);
    void MsgSndEncryptKey		( DWORD dwClient );

    
    //          void MsgHeartBeat           (MSG_LIST* pMsg);
    void MsgServerCurInfo		(DWORD dwClient, MSG_LIST* pMsg);	
    void MsgServerChannelInfo   (DWORD dwClient, MSG_LIST* pMsg);
    void MsgSndChatCtrlGlobal	(MSG_LIST* pMsg);
    void MsgSndChatCtrlGlobal2	(MSG_LIST* pMsg);

    //! Session->Agent, Field
    int	 MsgSndServerStateChange(int nClient, int nState); 

    //! Agent->Session : 사용자 로그아웃
    void MsgUsrLogout			(MSG_LIST* pMsg);
    void DaumMsgUsrLogout		(MSG_LIST* pMsg);
    void MsgUsrLogoutSimple		(MSG_LIST* pMsg);

    //! Agent->Session : 캐릭터가 게임에 조인했음
    void MsgGameJoinOK			(DWORD dwClient, MSG_LIST* pMsg);

    void MsgReqAllSvrFullInfo	(void);
    void MsgReqSvrFullInfo		(DWORD nClient);

    void MsgReqAllSvrCurInfo	(void);
    void MsgReqSvrCurInfo		(DWORD nClient);

    //! Agent->Session : 캐릭터 생성, 갯수 감소
    int MsgChaDecrease(MSG_LIST* pMsg);
    //! Agent->Session :TEST 서버  캐릭터 생성, 갯수 감소
    int MsgTestChaDecrease(MSG_LIST* pMsg);
    //! DAUM:Agent->Session : 캐릭터 생성, 갯수 감소
    int DaumMsgChaDecrease(MSG_LIST* pMsg);
    //! DAUM:Agent->Session :TEST 서버  캐릭터 생성, 갯수 감소
    int DaumMsgTestChaDecrease(MSG_LIST* pMsg);

    //! Agent->Session:캐릭터 삭제, 갯수 증가
    int MsgChaIncrease(MSG_LIST* pMsg);
    //! Agent->Session:TEST 서버 캐릭터 삭제, 갯수 증가
    int MsgTestChaIncrease(MSG_LIST* pMsg);
    //! DAUM:Agent->Session : 캐릭터 삭제, 갯수 증가
    int	DaumMsgChaIncrease(MSG_LIST* pMsg);
    //! DAUM:Agent->Session :TEST 서버  캐릭터 삭제, 갯수 증가
    int DaumMsgTestChaIncrease(MSG_LIST* pMsg);
    int ExecuteCommand(char* strCmd);

    void MsgLoginMaintenance(DWORD dwClient, MSG_LIST* pMsg);
    //! Agent->Session:Point Shop Relaod
    void MsgPsReloadAS(DWORD dwClient, MSG_LIST* pMsg);
    void MsgGmLogItemReloadAS(DWORD ClientSlot, MSG_LIST* pMsg);
    void MsgGmGameDataUpdateAS(DWORD ClientSlot, MSG_LIST* pMsg);

    //! Login->Session : Version information
    void MsgVersionLS(DWORD dwClient, MSG_LIST* pMsg);
    void SendVersionInfo();

    void MsgEventExpMS(DWORD ClientSlot, MSG_LIST* pMsg);
    void MsgEventExpEndMS(DWORD ClientSlot, MSG_LIST* pMsg);

public:
	void MsgProcessForAuth(MSG_LIST* pMsg);

	//void MsgAuthCertificateFromAuthServer(NET_MSG_GENERIC* nmg);
	void MsgAuthCertificationRequest(DWORD dwClient, MSG_LIST* pMsg);
	void MsgAuthCertificationResponse(NET_MSG_GENERIC* nmg);
	void MsgAuthSessionStateRequest();
	void MsgAuthEventInfoRequest(NET_MSG_GENERIC* nmg);
	void MsgAuthEventInfoResponse(DWORD dwClient, MSG_LIST* pMsg);

	int	ConnectAuthServer();
	void CheckAuthServerSocket();
	bool CertificationInternalAuth();
	int	ConnectAuthServer(const char* szServerIP, int nPort);
	int SendAuthInfoToAuthServer();
	int SendAuthServer( NET_MSG_GENERIC* pBuffer, DWORD SendSize );
	void ResponseAuthSessionState();

	void MsgAuthServerStateRequest();	
	void ResponseAuthServerState();	

	void MsgSendAuthServerInfo(DWORD dwClient, MSG_LIST* pMsg);

	void CountryAccessApprove( NET_MSG_GENERIC* nmg );
public:
    void MsgServerMansgerMsg(DWORD dwClient, MSG_LIST* pMsg);
};

#endif // S_SESSIONSERVER_H_