#ifndef S_CFieldServer_H_
#define S_CFieldServer_H_

#pragma once

#include "../../SigmaCore/Json/MinJson.h"
#include "../../EngineLib/G-Logic/DxMsgServer.h"
#include "../../MfcExLib/ExceptionHandler.h"
#include "../../RanLogic/Msg/GLContrlMsg.h"

#include "../Country/GLCountryManServer.h"

#include "../DxServerInstance.h"
//#include "../FieldServer/GLGaeaServer.h"
#include "../Character/GLChar.h"
#include "../Database/DbAction/DbExecuter.h"
#include "./NetServer.h"

#define WM_FINALCLEANUPFINISH (WM_USER+157)

class NetFieldClientMan;
class FieldServerConfigXml;
class GLGaeaServer;

namespace sc {
	class CGlobalAuthClientLogic;
}


//! class CFieldServer
//!
//! * History
//! 2002.05.30 jgkim Create
//! 
//! * Note
//!
class CFieldServer : public NetServer, public DxMsgServer
{
public :
	CFieldServer(HWND hWnd, HWND hEditBox, HWND hEditBoxInfo, SUBPATH* pPath, EMSERVICE_PROVIDER ServiceProvider, const bool bEmulator = false);
	virtual ~CFieldServer();

protected :	
	// Data member for session server
	NetFieldClientMan* m_pClientManager;
    FieldServerConfigXml* m_pConfig;

	sc::CGlobalAuthClientLogic* m_pAuthManager; // for global auth

	DWORD				m_dwFieldServerID;
	DWORD				m_dwAgentSlot;
	DWORD				m_dwHeartBeatStart1;
	DWORD				m_dwHeartBeatStart2;
	bool				m_bReservationServerStop;
	bool				m_bEmulator;
	float				m_fReservationtime;			//! ����ð�(5��)
    GLGaeaServer*       m_pGaeaServer;

    DWORD               m_SessionServerSlot; //! Session server slot
    DWORD               m_CacheServerSlot; //! Cache server slot
	DWORD               m_IntegrationCacheServerSlot; //! Integration Cache server slot

    //  CloseClientList
    std::vector<DWORD> m_vecCloseList;

	DWORD				m_dwCompackHeapTime;

	float				m_fChaJoinRandom;	// 

	DWORD				m_dwErrorCode;	// for ado e_out_of_memory force server stop

	DWORD				m_dwMemoryCheckTime;
	DWORD				m_dwForceServerStopCheckTime;
	bool				m_bCheckMemoryAndReservationForceServerStop;
	DWORD				m_dwServerRestartCheckTime;
    
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
	virtual int	InsertMsg(int nClient, EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc=false);

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

    virtual int SendAgent(NET_MSG_GENERIC* pBuffer) override;
    virtual int SendAgent(EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc=false) override;

	virtual int SendAgent ( DWORD dwClient, NET_MSG_GENERIC* pBuffer ) override;
	virtual int SendAgent ( DWORD dwClient, EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc = false ) override;
	virtual int SendAgent_ServerGroup ( DWORD dwServerGroup, NET_MSG_GENERIC* pBuffer ) override;
	virtual int SendAgent_ServerGroup ( DWORD dwServerGroup, EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc = false ) override;

    virtual void SendMySelf(DWORD dwClient, NET_MSG_GENERIC* pBuffer) override;
    virtual void SendMySelf(DWORD dwClient, EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc=false) override;

    //! Do not call this function
    virtual int SendField(DWORD dwClient, NET_MSG_GENERIC* pBuffer) override;
    virtual int SendField(DWORD dwClient, EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc=false) override;
	
    //! Do not call this function
    virtual int SendField(DWORD dwChannel, DWORD dwFieldSvrNum, NET_MSG_GENERIC* pBuffer) override;
    virtual int SendField(DWORD dwChannel, DWORD dwFieldSvrNum, EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc=false) override;
    
    //! Do not call this function
    virtual int SendFieldSvr(int nSvrNum, NET_MSG_GENERIC* pBuffer, int nServerChannel=0) override;
    virtual int SendFieldSvr(int nSvrNum, EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, int nServerChannel=0, bool UseCrc=false) override;

    virtual void SendAllClient(NET_MSG_GENERIC* pBuffer) override;
    virtual void SendAllClient(EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc=false) override;

    //! Do not call this function
    virtual void SendAllField(NET_MSG_GENERIC* pBuffer, int nServerChannel=0) override;
    virtual void SendAllField(EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, int nServerChannel=0, bool UseCrc=false) override;

    virtual void ResetGaeaID(DWORD dwClient) override;	
	virtual void CloseClient(DWORD dwClient, net::CLOSE_TYPE emCloseType = net::UNDEFINE, int nLastError = 0 ) override;  // Close Client Connection

    //!  Do not call this function
    virtual void SendChannel(NET_MSG_GENERIC* pBuffer, int nServerChannel=0) override;
    virtual void SendChannel(EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, int nServerChannel=0, bool UseCrc=false) override;

    //!  Do not call this function
    virtual int SendAllChannel(NET_MSG_GENERIC* pBuffer) override;
    virtual int SendAllChannel(EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc=false) override;

	//! Do not call this function
    virtual int SendMatch(NET_MSG_GENERIC* pMsg) override;
    virtual int SendMatch(EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc=false) override;

    //! -----------------------------------------------------------------------
    //!
    //! -----------------------------------------------------------------------
    int FinalStop();	         
    int StartDbManager();
    int Update();
    void WriteNprotectLog( DWORD dwClient, DWORD dwResult );

	Country::SCOUNTRY_INFO GetCountry ( DWORD dwDBNum ) const;
	void InitializeCountry ( DWORD dwClientID );
    	
    std::string GetClientIP(DWORD dwClient) const;
    //void Send(DWORD nClient,  PER_IO_OPERATION_DATA* PerIoData, DWORD dwSize);
    void CloseAllClient ();

    //  CloseClient ���� ó��
    void ReserveCloseClient( DWORD dwClient );
    void CloseClientFinal();
    bool IsReserveCloseClient( DWORD dwClient );

	net::EMSERVERTYPE GetServerType();

	std::string GetIP();
	int			GetPort();

	// ���� ID���� �ƴ��� üũ
	int CheckTracingUser( const char *pUserAccount, const int nUserNum );

    GLGaeaServer* GetGLGaeaServer() { return m_pGaeaServer; }

    void ClearBlockIp();

    const FieldServerConfigXml* GetConfigXml() const { return m_pConfig; };

	//void ServerForceStop( DWORD Reason );
	void SaveCharacterInSqlite( SCHARDATA2* pChadata );
	int ForceFinalStop();
	DWORD GetErrorCode() { return m_dwErrorCode; }

	void TraceHang();

	void CheckMemory( DWORD dwUdpateTime );
	void ReserveServerForceStop();
	void CheckServerAutoStart( DWORD dwUdpateTime );

	void SendPrivateMarketData();

	//! -----------------------------------------------------------------------
    //! Message process function
public:
	struct FieldPacketFunc
	{
		( void ) ( CFieldServer::*Msgfunc ) (DWORD dwClient, NET_MSG_GENERIC* nmg);
		FieldPacketFunc()
		{
			Msgfunc = NULL;
		}
	};
	FieldPacketFunc m_MsgFunc[NET_MSG_GCTRL_MAX];

	void initPacketFunc();

public:

	void MsgProcess(MSG_LIST* pMsg);
    void MsgJson(DWORD ClientSlot, NET_MSG_GENERIC* pMsg);
    bool MsgJsonParser(DWORD ClientSlot, DWORD GaeaID, const std::string& JsonStr);

	void MsgHeartbeatClientAns(DWORD dwClient, NET_MSG_GENERIC* nmg);
	void MsgIamAgent(DWORD dwClient, NET_MSG_GENERIC* nmg);
	void MsgHeartbeatServerAnswer(DWORD dwClient, NET_MSG_GENERIC* nmg);
	void MsgVersion(DWORD dwClient, NET_MSG_GENERIC* nmg);
	//void MsgLoginInfoAgent(DWORD dwClient, NET_MSG_GENERIC* nmg);
	void MsgSndPingAnswer(MSG_LIST* pMsg);
	void MsgSndChatGlobal(const char* szChatMsg);
	void MsgSndChatNormal(DWORD dwClient, const char* szName, const char* szMsg);
	void MsgSndCryptKey( DWORD dwClient);
	int MsgSndChaJoinToAgent(DWORD dwClient, int nChaNum, DWORD dwGaeaID, DWORD dwMapID);

	
	//! Ŭ���̾�Ʈ->���Ӽ��� : ó�� Field ������ ����, �ɸ��͸� ����, �ʵ�� Ŭ���̾�Ʈ�� ���� �غ�.	
	void MsgGameJoinChar(DWORD dwClient, NET_MSG_GENERIC* nmg);
	
	//! Client->Field : ���������� �˸�
	void MsgJoinInfoFromClient(DWORD dwClient, NET_MSG_GENERIC* pMsg);
	
	//! DB ������->�ʵ弭��:DB���� �о��� ������ �ɸ��� ���� �õ�.
	void MsgFieldReqJoin(DWORD dwClient, NET_MSG_GENERIC* nmg);
	//void MsgFieldReqJoin( DWORD dwClientID, SCHARDATA2 * pCHAR_DATA, GLMSG::NET_CHAR_JOIN_DF::SINFO & sINFO );
	void SetUserInfo(DWORD nClient, const char* strUserId, const char* strPassword);
	
	//void ServerMsgProcess( MSG_LIST* pMsg );

	bool GetReservationServerStop() { return m_bReservationServerStop; };

	// Execute Command Text
	int	ExecuteCommand( char* strCmd );

    //! -----------------------------------------------------------------------
	//! Session server Communication Member Function

	int	SessionConnect();
    //int SessionConnect(unsigned long ulServerIP, int nPort);

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

	// void SessionSndPing(); // Send ping
	int SessionSndSvrInfo();
    void SessionSndHeartbeat();
	int SessionSndSvrCurState();
	int	SessionSndLoginInfo(const char* szUsrID, const char* szUsrPass, int nClient);
	void SessionSndLogOutInfo(const char* szUsrID, int nUserNum=0, int nGameTime=0, int nChaNum=0);
	void SessionSndLogOutInfoSimple(const char* szUsrID);

	//! -----------------------------------------------------------------------
	//! Integration Cache server
	int IntegrationCacheConnect();
	void IntegrationCacheMsgProcess( MSG_LIST* pMsg );
	void IntegrationCacheSendHeartbeat();
	virtual void SendIntegrationCache( NET_MSG_GENERIC* pBuffer ) override;
	virtual void SendIntegrationCache( EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc = false ) override;

    //! -----------------------------------------------------------------------
    //! Cache server
    int CacheConnect();
    virtual void SendCache(NET_MSG_GENERIC* pBuffer) override;
    virtual void SendCache(EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc=false) override;
    void CacheMsgProcess(MSG_LIST* pMsg);
    void CacheSendHeartbeat();

    void MsgPointShopDataStart(NET_MSG_GENERIC* pMsg);
    void MsgPointShopDataEnd(NET_MSG_GENERIC* pMsg);
    void MsgPointShopData(NET_MSG_GENERIC* pMsg);
    void MsgPointShopCharPointHF(NET_MSG_GENERIC* pMsg);
	void MsgChaExtraInfoHF(NET_MSG_GENERIC* pMsg);
    void LogAttendanceDS(DWORD ClientSlot, NET_MSG_GENERIC* pMsg);
    void LogAttendanceTaskDS(DWORD ClientSlot, NET_MSG_GENERIC* pMsg);

    // ���λ��� �˻�
	void MsgPrivateMarketItemPayMoneyHF( NET_MSG_GENERIC* nmg );
	void MsgPrivateMarketItemPayMoneyRollbackHF( NET_MSG_GENERIC* nmg );
	void MsgPrivateMarketItemBuyHF( NET_MSG_GENERIC* nmg );
	void MsgPrivateMarketItemIsHoldHF( NET_MSG_GENERIC* nmg );
	void MsgPrivateMarketCommonFB( NET_MSG_GENERIC* nmg );
	void MsgPrivateMarketItemNumTotalHF( NET_MSG_GENERIC* nmg );
	void MsgPrivateMarketItemNumHF( NET_MSG_GENERIC* nmg );
	void MsgPrivateMarketItemHF( NET_MSG_GENERIC* nmg );
	void MsgPrivateMarketItemEndHF( NET_MSG_GENERIC* nmg );
	void MsgPrivateMarketLogNumHF( NET_MSG_GENERIC* nmg );
	void MsgPrivateMarketLogHF( NET_MSG_GENERIC* nmg );
	void MsgPrivateMarketLogEndHF( NET_MSG_GENERIC* nmg );
	void MsgPrivateMarketStateHF( NET_MSG_GENERIC* nmg );
	void MsgPrivateMarketPostSendHF( NET_MSG_GENERIC* nmg );

    // ��Ź�Ǹ�
    void MsgConsignmentSaleSendBackAck( NET_MSG_GENERIC* nmg );
    void MsgConsignmentSalePurchaseReq( NET_MSG_GENERIC* nmg );
	void MsgConsignmentSalePayMoney( NET_MSG_GENERIC* nmg );
	void MsgConsignmentSaleSendPostAndLog( NET_MSG_GENERIC* nmg );

	// Country
	void MsgUpdateUserCountryAF ( DWORD ClientSlot, NET_MSG_GENERIC* pMsg );

    /*
     * redmine : #1161 Randombox Ȯ������ �ý��� ó��
     * created : sckim, 2015.12.16, Randombox Open�� Ȯ������ ���� ���� ��û ����
     * modified : 
     */
    void MsgReqInvenRandomBoxOpenFB(DWORD ClientSlot, NET_MSG_GENERIC* nmg);

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

    void ResetCreatingCharUserID(DWORD dwUserID);
	void ResetSaveDBUserID( DWORD dwUserID );

    void ClubMemberGradeChange(DWORD ClientSlot, NET_MSG_GENERIC* pMsg);
    void ClubGradeNameChangeAF(DWORD ClientSlot, NET_MSG_GENERIC* pMsg);

    //! Database --------------------------------------------------------------
    void AddGameJob(db::DbActionPtr spJob);
    void AddGameAdoJob(db::DbActionPtr spJob);

    //! -----------------------------------------------------------------------
    //! Agent Server
    void AgentMsgProcess(MSG_LIST* pMsg);

	void MsgReferenceCountAF( DWORD dwClient, NET_MSG_GENERIC* nmg );

	BOOL FindSaveDBUserID( DWORD dwUserDbNum );

public:
	int SendAuthInfoToSessionServer(); // ���Ǽ����� ���� ���� ����. ���ǿ��� ���������� ������
	void MsgAuthCertificationResponse(NET_MSG_GENERIC* pMsg);
};

#endif // S_CFieldServer_H_
