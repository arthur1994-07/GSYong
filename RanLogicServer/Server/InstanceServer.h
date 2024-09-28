#pragma once

#include "../../EngineLib/G-Logic/DxMsgServer.h"
#include "../../MfcExLib/ExceptionHandler.h"
#include "../../RanLogic/Msg/GLContrlMsg.h"

#include "../DxServerInstance.h"
#include "../FieldServer/GLGaeaServer.h"
#include "../Character/GLChar.h"
#include "../Database/DbAction/DbExecuter.h"
#include "./NetServer.h"

#define WM_FINALCLEANUPFINISH (WM_USER+157)

class NetInstanceClientMan;
class InstanceServerConfigXml;
class GLGaeaServer;


class InstanceServer : public NetServer, public DxMsgServer
{
public:
	InstanceServer(HWND hWnd, HWND hEditBox, HWND hEditBoxInfo, SUBPATH* pPath, EMSERVICE_PROVIDER ServiceProvider);
	virtual ~InstanceServer();

protected:
	// Data member for session server
	NetInstanceClientMan* m_pClientManager;
	InstanceServerConfigXml* m_pConfig;

	db::IDbManager* m_paDbMan[MAX_SERVER_GROUP];
	
	//mjeon.dbman.ODBC�� ADO�� ���ؼ� ����ϱ� ���� AdoManager�� ��������� �ϳ� �����Ѵ�.
	db::IDbManager* m_paAdoMan[MAX_SERVER_GROUP];

	DWORD				m_dwInstanceServerNum;
	
	DWORD				m_dwHeartBeatStart1;
	DWORD				m_dwHeartBeatStart2;
	bool				m_bReservationServerStop;	
	float				m_fReservationtime;			//! ����ð�(5��)
    GLGaeaServer*       m_pGaeaServer;

    DWORD               m_SessionServerSlot;
    DWORD               m_CacheServerSlot;
	DWORD				m_MatchServerSlot;

    //  CloseClientList
    std::vector<DWORD> m_vecCloseList;
    
public:
    //! -----------------------------------------------------------------------
    //! NetServer override functions 
    //! -----------------------------------------------------------------------
	virtual int Start() override;
	virtual int Stop() override;
	virtual void ForceStop( DWORD Reason ) override;

    virtual int StartClientManager() override;
    virtual int WorkProc() override;
    virtual int UpdateProc() override;
    virtual int ListenProc() override;
	virtual int	RegularScheduleProc() override;
    
    //! Local Message process function ( gaeaserver�� ���� ���� �޽��� ����. )
    virtual int InsertMsg(int nClient, void* pMsg) override;

    // I/O operation memory
    virtual PER_IO_OPERATION_DATA* GetFreeOverIO(EMMSG_IO_TYPE nType=NET_SEND_POSTED) override;
    virtual void ReleaseOperationData(PER_IO_OPERATION_DATA* pData) override;

    //! -----------------------------------------------------------------------
    //! DxMsgServer override functions
    //! -----------------------------------------------------------------------
	virtual int GetServerGroup() const override { return m_nServerGroup; }
	virtual int GetServerNum() const override { return m_nServerNum; }
	
	virtual int SendClient(NET_MSG_GENERIC* pBuffer, DWORD dwChaNum) override;
    virtual int SendClient(EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, DWORD dwChaNum, bool UseCrc=false) override;

    virtual int SendClient(DWORD dwClient, NET_MSG_GENERIC* pBuffer) override;
    virtual int SendClient(DWORD dwClient, EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc=false) override;
	
	virtual int SendMatch(NET_MSG_GENERIC* pBuffer);	//Match�� ����
    virtual int SendMatch(EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc=false);	//Match�� ����

    virtual int SendAgent(NET_MSG_GENERIC* pBuffer);	//Match�� ���ļ� �ش� Agent�� �����ϵ��� ����
    virtual int SendAgent(EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc=false);	//Match�� ���ļ� �ش� Agent�� �����ϵ��� ����

	virtual int SendAgent ( DWORD dwClient, NET_MSG_GENERIC* pBuffer ) override;
	virtual int SendAgent ( DWORD dwClient, EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc = false ) override;
	virtual int SendAgent_ServerGroup ( DWORD dwServerGroup, NET_MSG_GENERIC* pBuffer ) override;
	virtual int SendAgent_ServerGroup ( DWORD dwServerGroup, EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc = false ) override;

    //! Do not call this function
    virtual int SendField(DWORD dwClient, NET_MSG_GENERIC* pBuffer) override;
    virtual int SendField(DWORD dwClient, EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc=false) override;

    //! Do not call this function
	virtual int SendField(DWORD dwChannel, DWORD dwFieldSvrNum, NET_MSG_GENERIC* pBuffer) override;
    virtual int SendField(DWORD dwChannel, DWORD dwFieldSvrNum, EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc=false) override;

    //! Do not call this function
    virtual int SendFieldSvr(int nSvrNum, NET_MSG_GENERIC* pBuffer, int nServerChannel=0) override;
    virtual int SendFieldSvr(int nSvrNum, EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, int nServerChannel=0, bool UseCrc=false) override;

    //! Do not call this function
    virtual void SendAllField(NET_MSG_GENERIC* pBuffer, int nServerChannel=0) override;
    virtual void SendAllField(EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, int nServerChannel=0, bool UseCrc=false) override;
	
	virtual void SendAllClient(NET_MSG_GENERIC* pBuffer) override;
    virtual void SendAllClient(EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc=false) override;

    virtual void ResetGaeaID(DWORD dwClient) override;	
	virtual void CloseClient(DWORD dwClient, net::CLOSE_TYPE emCloseType = net::UNDEFINE, int nLastError = 0) override;  // Close Client Connection

    //! Do not call this function
    virtual void SendChannel(NET_MSG_GENERIC* pBuffer, int nServerChannel=0) override;
    virtual void SendChannel(EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, int nServerChannel=0, bool UseCrc=false) override;

    //! Do not call this function
    virtual int SendAllChannel(NET_MSG_GENERIC* pBuffer) override;
    virtual int SendAllChannel(EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc=false) override;

    //! -----------------------------------------------------------------------
    //!
    //! -----------------------------------------------------------------------
    int FinalStop();
    
    void WriteNprotectLog( DWORD dwClient, DWORD dwResult );
    	
    std::string GetClientIP(DWORD dwClient) const;
    //void Send(DWORD nClient,  PER_IO_OPERATION_DATA* PerIoData, DWORD dwSize);
    void CloseAllClient ();

    //  CloseClient ���� ó��
    void ReserveCloseClient( DWORD dwClient );
    void CloseClientFinal();
    bool IsReserveCloseClient( DWORD dwClient );

	// ���� ID���� �ƴ��� üũ
	int CheckTracingUser( const char *pUserAccount, const int nUserNum );

    GLGaeaServer* GetGLGaeaServer() { return m_pGaeaServer; }

    void ClearBlockIp();

	net::EMSERVERTYPE GetServerType();

	std::string GetIP();
	int	GetPort();

	//! -----------------------------------------------------------------------
    //! Message process function
public:
	struct InstancePacketFunc
	{
		( void ) ( InstanceServer::*Msgfunc ) (DWORD dwClient, NET_MSG_GENERIC* nmg);
		InstancePacketFunc()
		{
			Msgfunc = NULL;
		}
	};
	InstancePacketFunc m_MsgFunc[NET_MSG_GCTRL_MAX];

	void initPacketFunc();

public:

	void MsgProcess( MSG_LIST* pMsg );

	void MsgHeartbeatClientAns(DWORD dwClient, NET_MSG_GENERIC* nmg);
	
	void MsgIamMatch(DWORD dwClient, NET_MSG_GENERIC* nmg);
	void MsgTransferChar(DWORD dwClient, NET_MSG_GENERIC* nmg);
	
	void MsgHeartbeatServerAnswer(DWORD dwClient, NET_MSG_GENERIC* nmg);
	void MsgVersion(DWORD dwClient, NET_MSG_GENERIC* nmg);

	void MsgSndPingAnswer(MSG_LIST* pMsg);
	void MsgSndChatGlobal(const char* szChatMsg);
	void MsgSndChatNormal(DWORD dwClient, const char* szName, const char* szMsg);
	void MsgSndCryptKey( DWORD dwClient);
	int MsgSndChaJoinToAgent(DWORD dwClient, int nChaNum, DWORD dwGaeaID, DWORD dwMapID);

	
	//! Ŭ���̾�Ʈ->���Ӽ��� : ó�� Field ������ ����, �ɸ��͸� ����, �ʵ�� Ŭ���̾�Ʈ�� ���� �غ�.	
	void MsgGameJoinChar(DWORD dwClient, NET_MSG_GENERIC* nmg);
	
	//! Client->Field : ���������� �˸�
	void MsgJoinInfoFromClient(DWORD dwClient, NET_MSG_GENERIC* pMsg);


	void MsgCreateInstanceMI(DWORD dwClient, NET_MSG_GENERIC* pMsg);
	void MsgDeleteInstanceMI(DWORD dwClient, NET_MSG_GENERIC* pMsg);	

	
	//! DB ������->�ʵ弭��:DB���� �о��� ������ �ɸ��� ���� �õ�.
	void MsgFieldReqJoin(DWORD dwClient, NET_MSG_GENERIC* nmg);
	//void MsgFieldReqJoin( DWORD dwClientID, SCHARDATA2 * pCHAR_DATA, GLMSG::NET_CHAR_JOIN_DF::SINFO & sINFO );
	void SetUserInfo(DWORD nClient, const char* strUserId, const char* strPassword);
	
	//void ServerMsgProcess( MSG_LIST* pMsg );

	bool GetReservationServerStop() { return m_bReservationServerStop; };

	// Execute Command Text
	int	ExecuteCommand( char* strCmd );


    //
	//Session server
	//
	int	SessionConnect();

	//! Session ������ �����ϰ� ������ ������ �����Ѵ�.	
	int SessionConnectSndSvrInfo();	

	int	SessionCloseConnect(void);
	int SendSession(NET_MSG_GENERIC* pBuffer, DWORD SendSize);
	void SessionMsgProcess(MSG_LIST* pMsg);	
	void SessionMsgPingAnswer(NET_MSG_GENERIC* nmg); // Ping Answer
	void SessionMsgLoginFeedBack(NET_MSG_GENERIC* nmg); // ���Ǽ��� -> ���Ӽ��� : �α��ΰ�� ����Ÿ
	void SessionMsgChatProcess(NET_MSG_GENERIC* nmg); // ���Ǽ���->���Ӽ��� : ä�ø޽���
	void SessionMsgSvrCmd(NET_MSG_GENERIC* nmg);
    void SessionMsgVersionSS(NET_MSG_GENERIC* nmg);
	void SessionMsgHeartbeatServerAns();
    void DoNothing(NET_MSG_GENERIC* pMsg);
	
	int SessionSndSvrInfo();
    void SessionSndHeartbeat();
	int SessionSndSvrCurState();
	int	SessionSndLoginInfo(const char* szUsrID, const char* szUsrPass, int nClient);
	void SessionSndLogOutInfo(const char* szUsrID, int nUserNum=0, int nGameTime=0, int nChaNum=0);
	void SessionSndLogOutInfoSimple(const char* szUsrID);

    //
    //Cache server
	//
    //! Do not call this function
    virtual void SendCache(NET_MSG_GENERIC* pBuffer) override;
    virtual void SendCache(EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc=false) override;

	virtual void SendIntegrationCache(NET_MSG_GENERIC* pBuffer) override {}
	virtual void SendIntegrationCache(EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc=false) override {}

    void MsgPointShopDataStart(NET_MSG_GENERIC* pMsg);
    void MsgPointShopDataEnd(NET_MSG_GENERIC* pMsg);
    void MsgPointShopData(NET_MSG_GENERIC* pMsg);
    void MsgPointShopCharPointHF(NET_MSG_GENERIC* pMsg);
	void MsgChaExtraInfoHF(NET_MSG_GENERIC* pMsg);


	//
	//MatchServer
	//
	int MatchConnect();
	int MatchCloseConnect();



    //! -----------------------------------------------------------------------
	BOOL UpdateTracingCharacter(NET_MSG_GENERIC* nmg );

	// ���� �޽������� ó���Ѵ�.
	void RecvMsgProcess();
	// ��Ʈ��Ʈ üũ�� �Ѵ�.
	void FieldSrvHeartBeatCheck( DWORD dwUdpateTime );

	// ���� ������ ���¸� ����Ʈ �Ѵ�.
	void PrintDebugMsg( DWORD dwUdpateTime );
	// UpdateThread ����
	void EndUpdateThread();
};