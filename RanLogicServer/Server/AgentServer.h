#ifndef _LS_AGENT_SERVER_H_
#define _LS_AGENT_SERVER_H_

#pragma once

#include "../../SigmaCore/Json/MinJson.h"
#include "../../SigmaCore/Util/minTea.h"
#include "../../EngineLib/G-Logic/DxMsgServer.h"

#include "../../RanLogic/Util/s_CMinMd5.h"
#include "../../RanLogic/Util/s_Util.h"
#include "../../RanLogic/Util/ApexCloseList.h"
#include "../../RanLogic/Util/s_CWhiteRock.h"
#include "../../RanLogic/Util/ApexProxyLib.h" // �ű� APEX
#include "../../RanLogic/Util/CPLManager.h"

#include "../Country/GLCountryManServer.h"

#include "../DxServerInstance.h"
#include "../Character/GLChar.h"
#include "../AgentServer/GLAgentServer.h"
#include "../Database/DBAction/DbExecuter.h"
#include "./AgentServerApex.h"
#include "./NetServer.h"

#include <stdio.h>

#include "../../=cURL/include/curl/curl.h"

#ifndef ERROR_MSG_LENGTH 
#define ERROR_MSG_LENGTH 256
#endif

/*
#if defined ( PH_PARAM ) || defined ( VN_PARAM )
	#import "../../=DLL/EGameEncrypt.dll" raw_interfaces_only, raw_native_types, no_namespace, named_guids
#else
	#import "../../=DLL/MyRossoEncrypt.dll" raw_interfaces_only, raw_native_types, no_namespace, named_guids
#endif
*/

#import "../../=DLL/EGameEncrypt.dll" raw_interfaces_only, raw_native_types, no_namespace, named_guids
#import "../../=DLL/MyRossoEncrypt.dll" raw_interfaces_only, raw_native_types, no_namespace, named_guids

//
//mjeon.indonesia
//
//Live Server
#define IDN_VTC_AUTH_URL				"http://103.14.108.2:8088/GameService/BillingService.asmx"
#define IDN_MAKERCODE					"RANONLINE"
#define IDN_SERVICEID					"790019"
#define IDN_SERVICEKEY					"e379390d150409a0f76aa0d7cf53d1a0"
#define IDN_MD5KEY						"b5f97c184034cd03f1c8ed703949f017"
#define IDN_VTC_ACCOUNT_FUNCTION_URL	"http://103.14.108.2:8088/GameService/AccountFunction.asmx"
/* 
//Test Server
#define IDN_VTC_URL		"http://sandbox2.vtcebank.vn/GameServiceAPI/BillingService.asmx"
#define IDN_MAKERCODE	"RANONLINE"
#define IDN_SERVICEID	"880136"
#define IDN_SERVICEKEY	"32874be2a8f1e9722edff1ab442613a8"
#define IDN_MD5KEY		"123456"
*/

typedef std::vector<char> VECBUFFER;
typedef std::vector<char>::iterator VECBUFFER_ITER;

size_t wrapper_curl_write_callback( void *ptr, size_t size, size_t nmemb, void *userp );


class NetAgentClientMan;
class AgentServerConfigXml;

#define MAX_RANDOM_PASSWORD_NUM 30
#define	HTTP_STATUS_ALREADY_LOGIN 533

struct NET_LOGIN_FEEDBACK_DATA2;

struct DAUM_NET_LOGIN_FEEDBACK_DATA2;
struct DAUM_NET_PASSCHECK_FEEDBACK_DATA2;

struct GSP_NET_LOGIN_FEEDBACK_DATA2;

struct TERRA_NET_LOGIN_FEEDBACK_DATA2;
struct TERRA_NET_PASSCHECK_FEEDBACK_DATA2;

struct GS_NET_LOGIN_FEEDBACK_DATA2;

struct EXCITE_NET_LOGIN_FEEDBACK_DATA2;
struct EXCITE_NET_PASSCHECK_FEEDBACK_DATA2;

struct JAPAN_NET_LOGIN_FEEDBACK_DATA2;
struct NET_LOGIN_FEEDBACK_DATA;
struct NET_LOGIN_FEEDBACK_DATA2;

struct GM_NET_LOGIN_FEEDBACK_DATA2;

class GLObservePacketMgr;

namespace sc {
	class CGlobalAuthClientLogic;
}

//
//mjeon.ServiceProvider
//
//  moved below objects into the CAgenServer as its member variables.
//
/*
#if defined ( PH_PARAM ) || defined ( VN_PARAM )
	extern IEGameEncryptPtr    m_pMyRossoEncrypto; //! Terra Decrypt Module
#else
	extern IMyRossoEncryptPtr    m_pMyRossoEncrypto; //! Terra Decrypt Module
#endif
*/


typedef std::vector<char> VECBUFFER;
typedef std::vector<char>::iterator VECBUFFER_ITER;


size_t wrapper_curl_write_callback( void *ptr, size_t size, size_t nmemb, void *userp );


//! 2002-05-30 Jgkim
//! 2011-04-08 Jgkim �������� xml �� ��ü AgentServerConfigXml
class CAgentServer : public NetServer, public DxMsgServer
{
public :
	CAgentServer(HWND hWnd, HWND hEditBox, HWND hEditBoxInfo, SUBPATH* pPath, EMSERVICE_PROVIDER ServiceProvier, const bool bEmulator = false);
	virtual ~CAgentServer();

protected :
	// SOCKET m_sSession; //! Socket for session server
	NetAgentClientMan* m_pClientManager; //! Client ������
	int m_ChannelUser[MAX_CHANNEL_NUMBER]; //! ä�κ� ����� ���� ����
	AgentServerConfigXml* m_pConfig;

	//CDaumGameAuth m_DaumAuth; //! Daum Game Decrypt Module
	SERVER_UTIL::CMinMd5 m_MinMd5; //! Excite Japan MD5
    sc::minTea m_Tea; //! Encrypt algorithm
	bool m_bHeartBeat;
	bool m_bEmulator;

	sc::CGlobalAuthClientLogic* m_pAuthManager; // for global auth

	BOOL	m_bTriggerPostCleanUp;				// mjeon.Post
	BOOL	m_bTriggerAttendanceMaintenance;	// mjeon.attendance
	BOOL	m_bCooledAttendanceTrigger;			// mjeon.attendance - Ư�� �ð��� ������ ������ ���ӵ� maintenance ��û�� �����Ѵ�.
    UINT    m_uCntFieldResponse;                // kykim.attendance - AttendanceMaintananceStart�� ���� Ready ������ �ʵ弭�� ī��Ʈ.
    WORD    m_wAttendanceResetTimeHour;         // kykim.attendance - �������� Maintanance ��û�� ��
    WORD    m_wAttendanceResetTimeMinute;       // kykim.attendance - �������� Maintanance ��û�� ��
	

	// typedef	std::pair<DWORD,DWORD>		APEX_ID_TIME;
	// typedef	std::list<APEX_ID_TIME>		APEX_CLOSE_LIST;
	// typedef	APEX_CLOSE_LIST::iterator	APEX_CLOSE_LIST_ITER;

	ApexCloseList m_ApexClose;

	IEGameEncryptPtr	m_pIEGameEncrypto; //Terra Decrypt Module for Phillipines and Vietnam
	IMyRossoEncryptPtr	m_pMyRossoEncrypto; //! Terra Decrypt Module for Malaysia

    int m_nServerChannelNumber; //! Server Channel Number (Agent �� ���� ���� ä�� ����)

	// APEX_CLOSE_LIST m_listApexClose;

	// INFO:Reached Max Client Number �޽����� �㶧 UpdateProc�� ����� �۵��ϴ����� �˾ƺ��� ���� ����
	bool m_bReachedMAX;

	CTime m_CurrentTime; //! ���� �ð�
    CTime m_KorShutdownCheckTime;

	CCPLManager	m_CPLManager;

    //  CloseClientList
    std::vector<DWORD> m_vecCloseList;

    GLAgentServer* m_pGLAgentServer;

    DWORD m_SessionServerSlot; //! Session Server ���� slot
    DWORD m_CacheServerSlot; //! Cahce Server ���� Slot
	DWORD m_MatchServerSlot;

    F_SERVER_INFO m_FieldServerInfo[MAX_CHANNEL_NUMBER][FIELDSERVER_MAX]; //! Field Server Information
	A_SERVER_INFO m_AgentServerInfo[AGENTSERVER_MAX];

	//CURL		*m_curl;
	//CURLcode	m_result;

	GLObservePacketMgr* m_pObservePacketMgr;

	// �Ͻ������� ���� ����(ĳ���� ���� �ý��ۿ��� ���� ����Ѵ�. �ٸ����� ����ص� ���� �� �Ͻ� ���� ���)
	// ������ȣ / ���Ե� �� �ð�(tickcount)
	std::map< DWORD, DWORD > m_BlockUser;

public:
    GLAgentServer* GetGLAgentServer() { return m_pGLAgentServer; }
    F_SERVER_INFO* GetFieldServerInfo(DWORD ServerID, DWORD Channel);

public:
    //! -----------------------------------------------------------------------
    //! NetServer override functions 
    //! -----------------------------------------------------------------------
    virtual int Start() override;
    virtual int Stop() override;
	virtual void ForceStop( DWORD Reason ) override;
    
    virtual int StartClientManager() override;
    virtual int	WorkProc() override;
    virtual int ListenProc() override;
    virtual int	UpdateProc() override;
	virtual int	RegularScheduleProc() override;

    // I/O operation memory
    virtual PER_IO_OPERATION_DATA* GetFreeOverIO(EMMSG_IO_TYPE nType=NET_SEND_POSTED) override;
    virtual void ReleaseOperationData(PER_IO_OPERATION_DATA* pData) override;

    //! -----------------------------------------------------------------------
    //! DxMsgServer override functions
    //! -----------------------------------------------------------------------
	virtual int GetServerGroup() const override { return m_nServerGroup; }
	virtual int GetServerNum() const override { return m_nServerNum; }

	virtual void SendMySelf(DWORD dwClient, EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc=false) override;
	virtual void SendMySelf(DWORD dwClient, NET_MSG_GENERIC* pBuffer) override;

	virtual int SendClient(NET_MSG_GENERIC* pBuffer, DWORD dwChaNum) override; //mjeon.AF
    virtual int SendClient(EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, DWORD dwChaNum, bool UseCrc=false) override;

    virtual int SendClient(DWORD dwClient, NET_MSG_GENERIC* pBuffer) override;
    virtual int SendClient(DWORD dwClient, EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc=false) override;

	virtual int	SendField(DWORD dwChannel, DWORD dwFieldSvrNum, NET_MSG_GENERIC* pBuffer) override;
    virtual int SendField(DWORD dwChannel, DWORD dwFieldSvrNum, EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc=false) override;

	virtual int SendField(DWORD dwClient, NET_MSG_GENERIC* pBuffer) override;	//mjeon.AF
    virtual int SendField(DWORD dwClient, EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc=false) override;

    virtual int SendFieldSvr(int nSvrNum, NET_MSG_GENERIC* pBuffer, int nServerChannel=0) override;
    virtual int SendFieldSvr(int nSvrNum, EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, int nServerChannel=0, bool UseCrc=false) override;

    virtual void SendAllField(NET_MSG_GENERIC* pBuffer, int nServerChannel=0) override;
    virtual void SendAllField(EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, int nServerChannel=0, bool UseCrc=false) override;

    //! ������Ʈ������ SendAgent()�� ȣ���Ҽ� �����ϴ�. (X)
	//! ������ ȣ���� �� �������� ���弭���� ����鼭 �ٸ� Agent�� ��� �����ϵ��� ��ģ��;
	//! Agent�� �������� ��������� ������ ��� ����� Agent�� Broadcasting �ϵ��� �Ѵ�;
    virtual int SendAgent(NET_MSG_GENERIC* pBuffer) override;
    virtual int SendAgent(EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc=false) override;

	//! Ư�� Agent;
	virtual int SendAgent ( DWORD dwClient, NET_MSG_GENERIC* pBuffer ) override;
	virtual int SendAgent ( DWORD dwClient, EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc = false ) override;
	virtual int SendAgent_ServerGroup ( DWORD dwServerGroup, NET_MSG_GENERIC* pBuffer ) override;
	virtual int SendAgent_ServerGroup ( DWORD dwServerGroup, EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc = false ) override;

    virtual void SendChannel(NET_MSG_GENERIC* pBuffer, int nServerChannel=0) override;
    virtual void SendChannel(EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, int nServerChannel=0, bool UseCrc=false) override;

    virtual int SendAllChannel(NET_MSG_GENERIC* pBuffer) override;
    virtual int SendAllChannel(EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc=false) override;
    
	//mjeon.AF
	//virtual int ConnectFieldSvr(DWORD dwClient, int nFieldServer, DWORD dwGaeaID, int nServerChannel=0) override;
	virtual int SetUpFieldInfo(DWORD dwClient, int nFieldServer, DWORD dwGaeaID, int nServerChannel=0) override;
	virtual int SetUpInstanceInfo(DWORD dwClient, int nInstanceServer, int nServerChannel);
	
	//virtual BOOL IsBoundByInstance(DWORD dwClient, DWORD &dwInstanceSvrNum);

    virtual void SendAllClient(NET_MSG_GENERIC* pBuffer) override;
    virtual void SendAllClient(EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc=false) override;
    
    virtual void CloseClient(DWORD dwClient, net::CLOSE_TYPE emCloseType = net::UNDEFINE, int nLastError = 0) override; //! Close Client Connection

private:
	void CloseClientAgentAgent( DWORD dwClient, net::CLOSE_TYPE emCloseType );
    void CloseClientClient( DWORD dwClient, net::CLOSE_TYPE emCloseType );
    void CloseClientAgentField( DWORD dwClient, net::CLOSE_TYPE emCloseType );
    void CloseClientSession( DWORD dwClient, net::CLOSE_TYPE emCloseType );
    void CloseClientCache( DWORD dwClient, net::CLOSE_TYPE emCloseType );

public:    
    virtual bool IsSlotOnline(DWORD dwClient) override;
    int GetOnlineFieldCount();

    //! -----------------------------------------------------------------------
    int	StartDbManager();
    void WriteNprotectLog( DWORD dwClient, DWORD dwResult );

    void SendAllClientChannel(NET_MSG_GENERIC* pBuffer, int nServerChannel=0);
    void SendAllClientChannel(EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, int nServerChannel=0, bool UseCrc=false);
    
    //void CloseField (DWORD dwClient);
    void CloseAllClient(); 	// close all client connections

    //!  CloseClient ���� ó��
    void ReserveCloseClient(DWORD dwClient);
    void CloseClientFinal();
    bool IsReserveCloseClient(DWORD dwClient);

	Country::SCOUNTRY_INFO GetCountry ( DWORD dwDbNum ) const;

	net::EMSERVERTYPE GetServerType();

	std::string GetIP();
	int	GetPort();

	const bool IsSubServer ();

	// � Client Slot �ΰ�?;
	const bool IsAgentClientSlot ( DWORD _dwClientSlotID );
	const bool IsFieldClientSlot ( DWORD _dwClientSlotID );

	// Agent �������� ���� Client Slot ID ����;
	const DWORD GetServerGroup_ClientSlotID ( DWORD _dwClientSlotID );
	const DWORD GetClientSlotID_ServerGroup ( DWORD _dwServerGroup );

	const DWORD GetWorldBattleServerNumberInfo ();

	//void MsgVersion			(MSG_LIST* pMsg);
	void MsgCreateCharacter	(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);    
    void MsgCreateCharacterBack(int nChaNum, DWORD dwClient, DWORD dwUserNum, const char* szUserIP, USHORT uPort=0);
	void MsgSndPing			(DWORD dwClient);
    void MsgSndPingAnswer	(MSG_LIST* pMsg);

    void ClearBlockIp();

    //! ĳ���� �̸�/ĳ���� DB ��ȣ, ĳ���� DB ��ȣ/ĳ���� �̸�
    void ChaNameDbNumAdd(const std::string& ChaName, DWORD ChaDbNum, DWORD UserDbNum, EMUSERTYPE UserType);
	void ChaNameDbNumAdd(const std::string& ChaName, DWORD ChaDbNum, DWORD UserDbNum, EMUSERTYPE UserType, int iClass, int iLv);

	void ExecuteReservedTask();

	DWORD GetRandomFieldSlot();

public:
	struct AgentPacketFunc
	{
		(void) (CAgentServer::*Msgfunc)(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
		AgentPacketFunc()
		{
			Msgfunc = NULL;
		}
	};
	AgentPacketFunc m_MsgFunc[NET_MSG_GCTRL_MAX];

	void initPacketFunc();

public:
	//
	//mjeon.Post
	//
	void	PostCleanUp(SYSTEMTIME &st);

	//
	//mjeon.attendance.maintenance
	//
	void	DailyMaintenance();
	BOOL	AttendanceDailyMaintenance(SYSTEMTIME &st, BOOL bManually = FALSE);
	void	MsgAttendanceDailyMaintenanceReadyFA(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	void	MsgAttendanceMaintenanceManualStartCA(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);	
	

	//
	//not a message handler
	//
	// �⼮�� ���� ��û�� �������� ������ ���� �����ϱ� ���� flag�� ����.
	//
	
	void	Ready4AttendanceMaintenance(BOOL bReady)
	{
		m_bTriggerAttendanceMaintenance = bReady;			// daily request		
	}
	

	void	MsgReqChaNum(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	void	MsgReqChaNumBack(DWORD SenderClientSlot, DWORD SenderGaeaId, int ReceiverChaDbNum, int ReceiverUserDbNum, int nReqID);
	void	MsgPostGet(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
    void	MsgPostGetState(DWORD dwClient,  DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
    void	MsgPostGetSendState(DWORD dwClient,  DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
    void	MsgPostGetRecvState(DWORD dwClient,  DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	void	MsgPostNotify(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	void	MsgPostOpen(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	void	MsgPostOpenAndTakeCA(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	void	MsgPostDelete(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	void	MsgPostReturn(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	

	//
	//mjeon.Activity
	//	
	void	MsgActivityCompletedFA(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	void	MsgActivityClosedFA(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);


	//
	//mjeon.SNS
	//

	//Get FB-info from DB and keep in AgentServer. Then sends it to the client.
	void	MsgSNSGetInfo(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);	
	void	MsgSNSFBAuthed(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	void	MsgSNSFBUnAuthed(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);	
	void	MsgSNSFBGetUID(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);


	//Get TW-info from DB and keep in AgentServer. Then sends it to the client.
	void	MsgSNSTWGetInfo(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	void	MsgSNSTWAuthed(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);	
	void	MsgSNSTWUnAuthed(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);	
	void	MsgSNSTWGetUID(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);


	//-------------------------------------------------------------------------------------
    void    MsgProtocolInfoCA(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);

	void	MsgSndChatGlobal(const char* szChatMsg);
	void	MsgSndChatNormal(DWORD dwClient, const char* szName, const char* szMsg);
	void	MsgSndCryptKey	(DWORD dwClient);
	
	void	MsgSndLoginInfoToField(DWORD dwClient);

	void	MsgSndChaBasicBAInfo(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);	
	void	MsgSndChaBasicInfo  (DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	void	MsgSndChaBasicInfo  (DWORD dwClient, int nChaNum);
	void	MsgSndChaBasicInfoEnd  (DWORD dwClient);
	void	MsgSndChaBasicInfoBack(NET_MSG_GENERIC* pMsg);
	void	MsgSndChaBasicInfoBackEnd(NET_MSG_GENERIC* pMsg);
	
	void	MsgSndChaDelInfo(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg); // Agent->Client:ĳ���� ���� ��� ����	
	void	MsgSndChaDelInfoBack( int nChaNum, 
		                          int nUserNum, 
								  int nResult, 
								  DWORD dwClient, 
								  const TCHAR* szUserIP, 
								  USHORT uPort=0 ); // ���Ӽ���->Ŭ���̾�Ʈ : ĳ���� ���� ��� ����	

	//! Agent->Client:ĳ���� ����
	void DaumMsgSndChaDelInfo(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);

	//! Agent->Client:ĳ���� ���� ��� ����
	void DaumMsgSndChaDelInfoBack(int nChaNum, int nUserNum, int nResult, DWORD dwClient, const char* szUserIP, USHORT uPort=0);

	//
	//mjeon.indonesia
	//
	void IdnMsgSndChaDelInfo(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	void IdnMsgSndChaDelInfoBack(int nChaNum, int nUserNum, int nResult, DWORD dwClient, const char* szUserIP, USHORT uPort=0);
	    
	//! Client->Agent:ĳ���� ������û
	void TerraMsgSndChaDelInfo(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);

    //! Client->Agent:ĳ���� ������û
	void GspMsgSndChaDelInfo(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);

	//! Client->Agent:ĳ���� ������û
	void ExciteMsgSndChaDelInfo(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);

	//! \param nChaNum 
	//! \param nUserNum 
	//! \param nResult 
	//! \param dwClient 
	//! \param szUserIP 
	//! \param uPort 
	void ExciteMsgSndChaDelInfoBack(
        int nChaNum, 
        int nUserNum, 
        int nResult, 
        DWORD dwClient, 
        const char* szUserIP, 
        USHORT uPort=0);

	//! Client->Agent:ĳ���� ������û
	void JapanMsgSndChaDelInfo(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);

	//! \param nChaNum 
	//! \param nUserNum 
	//! \param nResult 
	//! \param dwClient 
	//! \param szUserIP 
	//! \param uPort 
	void JapanMsgSndChaDelInfoBack(
        int nChaNum, 
        int nUserNum, 
        int nResult, 
        DWORD dwClient, 
        const char* szUserIP, 
        USHORT uPort=0);

	//! Agent->Client:ĳ���� �����������
	//! \param nChaNum ĳ���͹�ȣ
	//! \param nUserNum ������ȣ
	//! \param nResult ���
	//! \param dwClient Ŭ���̾�Ʈ��ȣ
	//! \param szUserIP IP
	//! \param uPort PORT
	void TerraMsgSndChaDelInfoBack(
        int nChaNum, 
        int nUserNum, 
        int nResult, 
        DWORD dwClient, 
        const char* szUserIP, 
        USHORT uPort=0);

	/**
	* Agent->Client:ĳ���� �����������
	* \param nChaNum ĳ���͹�ȣ
	* \param nUserNum ������ȣ
	* \param nResult ���
	* \param dwClient Ŭ���̾�Ʈ��ȣ
	* \param szUserIP IP
	* \param uPort PORT
	*/
	void GspMsgSndChaDelInfoBack(
			int nChaNum, 
	        int nUserNum, 
			int nResult, 
			DWORD dwClient, 
			const TCHAR* szUserIP, 
			USHORT uPort=0 );


	/**
	* Client->Agent:ĳ���� ������û
	* \param pMsg 
	*/
	void	GsMsgSndChaDelInfo(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg); // Agent->Client:ĳ���� ���� ��� ����	

	/**
	* Agent->Client:ĳ���� �����������
	* \param nChaNum ĳ���͹�ȣ
	* \param nUserNum ������ȣ
	* \param nResult ���
	* \param dwClient Ŭ���̾�Ʈ��ȣ
	* \param szUserIP IP
	* \param uPort PORT
	*/
	void	GsMsgSndChaDelInfoBack( int nChaNum, 
		                          int nUserNum, 
								  int nResult, 
								  DWORD dwClient, 
								  const TCHAR* szUserIP, 
								  USHORT uPort=0 ); // ���Ӽ���->Ŭ���̾�Ʈ : ĳ���� ���� ��� ����	
	
	//! Client->Agent : ĳ���� ������ DB���� �о���� ��û.
	void MsgGameJoinCA(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* pMsg); 
	//! DB->Agent : DB���� �о��� ������ �ɸ��� ���� �õ�.
	void MsgAgentReqJoinDA(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* pMsg);
	//void MsgAgentReqJoin( DWORD dwClientID, GLCHARAG_DATA * pCHAR_DATA );
	//! Field->Agent : ĳ���Ͱ� ���ӿ� ��������
	void MsgLobbyCharJoinFA(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* pMsg);

	void MsgChaJoin2ndPassword(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* pMsg); // 2�����

	void MsgJoinInstance(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);	

	void MsgFieldSvrOutFb(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	void MsgResurrectMoveMapFb(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	void MsgReCallFb(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	void MsgPremiumSetFb(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);    	
	void MsgTeleportFb(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);

	void MsgServerLoginMaintenanceAGT(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);

	void MsgUserSystemInfoCA( DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );
	
	void SetUserInfo(DWORD nClient, const char* strUserId, const char* strPassword); 	

	BOOL CheckConnectionIP(const std::string& strIP) { return m_CPLManager.CheckConnectionIP(strIP); }

	//! Execute Command Text
	//! @param strCmd �ĸ��
	int	ExecuteCommand(char* strCmd);

	//////////////////////////////////////////////////////////////////////////
	/// All Connection Agent Server
	//////////////////////////////////////////////////////////////////////////
	void AgentConnectAll ();

	void FindAgentSlot ( DWORD dwClient, DWORD& dwAgentNum );

	int AgentConnect ( DWORD dwAgentNum );
	int	AgentConnect ( unsigned long ulServerIP, int nPort, DWORD dwAgentNum = 0 );

	int	AgentCloseConnect ( DWORD dwSlotNum );
	void AgentCloseConnectAll ();

	DWORD GetAgentSlotNum ( DWORD dwAgentNum );
	void AgentSlotSet ( DWORD dwAgentNum, DWORD dwSlotNum );
	void AgentSlotReset ( DWORD dwAgentNum );

	BOOL AgentMsgProcess ( MSG_LIST* pMsg );

	///////////////////////////////////////////////////////////////////////////
	/// Field Server Communication Member Function
	/// If you do not use Field Server, this functions are unnecessary
	///////////////////////////////////////////////////////////////////////////
	void FieldConnectAll();
	void FindFieldSlot( DWORD dwClient, DWORD& dwChannel, DWORD& dwFieldNum );
	
	int FieldConnect(DWORD FieldNum=0, DWORD Channel=0);	
	// int FieldConnect(const char* szServerIP, int nPort, int nFieldNum=0, int nChannel=0);
	int	FieldConnect(unsigned long ulServerIP, int nPort, DWORD FieldNum=0, DWORD Channel=0);

	int	FieldCloseConnect(DWORD SlotNum);
	void FieldCloseConnectAll();
    
    void FieldSlotSet(DWORD Channel, DWORD FieldNum, DWORD SlotNum);
    void FieldSlotReset(DWORD Channel, DWORD FieldNum);
    void FieldSlotReset(DWORD SlotNum);

	// bool CheckFieldConnection();

	///////////////////////////////////////////////////////////////////////////
	/// �޽��� ó�� ���� �Լ���
	///////////////////////////////////////////////////////////////////////////	
	//! Local Message process function ( gaeaserver�� ���� ���� �޽��� ����. )
    int	InsertMsg(int nClient, void* pMsg);
	int	InsertMsg(int nClient, EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc=false);
	
	//! ��� �޽����� �ϴ� �̰��� ��ģ��.
	void MsgProcess(MSG_LIST* pMsg);

	void MsgAutoRelay(DWORD dwClient, NET_MSG_GENERIC *nmg);	//mjeon.AF
	//! ���������� ����϶��� �̰��� ��ģ��.
	//BOOL ServerMsgProcess(MSG_LIST* pMsg);
	//! ���������� ����� session ������ �ƴҶ��� �̰��� ��ģ��.
	BOOL FieldMsgProcess(MSG_LIST* pMsg);	

	// ���� �޽������� ó���Ѵ�.
	void RecvMsgProcess();
	// ��Ʈ��Ʈ üũ�� �Ѵ�.
	void AgentSrvHeartBeatCheck( DWORD dwUdpateTime );

    void ClientNprotectCheck();
    void ClientHeartBeatCheck();
    void ServerHeartBeatCheck();
    void ServerHeartBeatRequest();

    // ���� ������Ʈ 
	void UpdateClientState();
	// ���� ������ ���¸� ����Ʈ �Ѵ�.
	void PrintDebugMsg(DWORD dwUdpateTime);
	// UpdateThread ����
	void EndUpdateThread();
	// ���Ӱ��� ���� ������ ����
	void EndRegularScheduleThread();

	///////////////////////////////////////////////////////////////////////////
	/// ä�� ���� �Լ���
	///////////////////////////////////////////////////////////////////////////
	int  IncreaseChannelUser(int nChannel);
	int  DecreaseChannelUser(int nChannel);
	int  GetChannelUser     (int nChannel);
	bool IsChannelFull      (int nChannel);
	void ChangeChannelUser ( DWORD dwClient, int nPreChannel, int nAfterChannel );

	//////////////////////////////////////////////////////////////////////////
	/// Agent Msg Process
	//////////////////////////////////////////////////////////////////////////
	void MsgHeartbeatServerRequest ( DWORD dwClient, NET_MSG_GENERIC* pMsg );
	void MsgHeartbeatServerAnswer ( DWORD dwClient, NET_MSG_GENERIC* pMsg );

	void MsgAgentRelayAC ( DWORD _dwClient, NET_MSG_GENERIC_RELAY_AGENT* _pMsg );
	void MsgAgentRelayAF ( DWORD _dwClient, NET_MSG_GENERIC_RELAY_AGENT* _pMsg );
	void MsgAgentRelayAB ( DWORD _dwClient, NET_MSG_GENERIC_RELAY_AGENT* _pMsg );
	
	///////////////////////////////////////////////////////////////////////////
	/// �α��� ���� �Լ���
	///////////////////////////////////////////////////////////////////////////	
	//! �α��� message ( �븸, ȫ��, ���� ) 
	void MsgLogIn(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	void MsgLogInBack( NET_LOGIN_FEEDBACK_DATA2* nlfd2 );

	//
	//mjeon.indonesia
	//
	void IdnMsgLogIn(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	int  IdnAuthVTC(std::string &strID, std::string &strPW);
	int  IdnAuthVTC_2ndPass(std::string &strID, std::string &str2ndPass);
	void IdnMsgLogInBack(NET_LOGIN_FEEDBACK_DATA2* nlfd2);	

    //! World Battle
    void MsgLoginWbCA(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* pMsg);
    //void MsgWorldBattleLogin(DWORD ClientSlot, DWORD GaeaID, const json_spirit::mObject& Obj);
    void MsgWorldBattleLoginBack(DWORD ClientSlot, NET_MSG_GENERIC* pMsg);
    void WorldBattleLoginFail(DWORD ClientSlot, NET_LOGIN_FEEDBACK_DATA2* pPacket, NET_LOGIN_FEEDBACK_DATA &MsgAC);
    void WorldBattleLoginSuccess(NET_LOGIN_FEEDBACK_DATA& MsgAC, NET_LOGIN_FEEDBACK_DATA2* pPacket, DWORD dwClientSlot);
   

	//! China �α��� �޽��� ( �߱� ) 
	void ChinaMsgLogin(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	void ChinaMsgLoginBack( NET_LOGIN_FEEDBACK_DATA2* nlfd2 );

	//! Daum �α��� �޽��� ( �ѱ� ) 
	void DaumMsgLogin(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	void DaumMsgLoginBack(DAUM_NET_LOGIN_FEEDBACK_DATA2* pMsg);

	//! Daum �н����� üũ �޽���
	void DaumMsgPassCheck(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	void DaumMsgPassCheckBack(DAUM_NET_PASSCHECK_FEEDBACK_DATA2* nlfd2);

	//! GSP �α��� �޽���
	void GspMsgLogin(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	void GspMsgLoginBack( GSP_NET_LOGIN_FEEDBACK_DATA2* nlfd2 );

	//! Terra �α��� �޽��� ( �����̽þ�, �ʸ���, ��Ʈ�� )
	void TerraMsgLogin(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	void TerraMsgLoginBack( TERRA_NET_LOGIN_FEEDBACK_DATA2* nlfd2 );

	//! Terra �н����� üũ �޽���
	void TerraMsgPassCheck(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	void TerraMsgPassCheckBack( TERRA_NET_PASSCHECK_FEEDBACK_DATA2* nlfd2 );

	//! Excite Japan �α��� �޽��� ( �Ϻ� )
	void ExciteMsgLogin(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	void ExciteMsgLoginBack(EXCITE_NET_LOGIN_FEEDBACK_DATA2* nlfd2);

	//! Excite Japna �н����� üũ �޽���
	void ExciteMsgPassCheck(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	void ExciteMsgPassCheckBack( EXCITE_NET_PASSCHECK_FEEDBACK_DATA2* nlfd2 );

	//! �Ϻ� Gonzo Japan �α��� �޽���
	void JapanMsgLogin(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	void JapanMsgLoginBack(JAPAN_NET_LOGIN_FEEDBACK_DATA2* nlfd2);

	// �Ϻ� Gonzo Japan �α��� ���� �Լ�
	void JapanWRFailed( DWORD dwClient, int nResult );
	void JapanWRSuccess( DWORD dwClient, CString strUserID, int nServerGroup, int nServerNum, CString strUUID, int nUserNum );

	//! �α��� �޽��� (Thailand) ( �±� ) 
	//! \param pMsg 
	void ThaiMsgLogin(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	void ThaiMsgLogInBack(NET_LOGIN_FEEDBACK_DATA2* nlfd2);	

    //! Encoding �� TID ���� Decoding �� TID �� ��´�.
	//! @param szEncodedTID Encoding �� TID
	//! @param szDecodedTID Decoding �� TID
	//! @return	
	int TerraGetDecodedTID(char* szEncodedTID, char* szDecodedTID);
	
	// GS �������� WB ���� ���� Ȯ��
	void GsMsgWBLogin(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	void GsMsgWBLoginAW(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	void GsMsgWBLoginWA(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);

	//! �α��� message ( �۷ι� ���� ) 
	void GsMsgLogin(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	void GsMsgLoginBack( GS_NET_LOGIN_FEEDBACK_DATA2* nlfd2 );

	//! �α��� message ( EU ) 
	void GmMsgLogin( DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );
	BOOL GmHTTPAuth( CString &strID, CString &strPW, std::string &strGUID, int &nBlock );
	void GmMsgLoginBack( GM_NET_LOGIN_FEEDBACK_DATA2* nlfd2 );

	//! �α��� message ( US ) 
	void UsMsgLogin( DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );
	void UsMsgLoginBack( US_NET_LOGIN_FEEDBACK_DATA2* nlfd2 );

	//! ���� ��ȣ�� ���� ��ȣ�� ������.
	void MsgSndRandomNumber(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);

	//! Apex Data �޼���	// Apex ����
	void MsgApexData(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	void MsgApexReturn(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);	// Apex ����( ȫ�� )

	//! Apex �� ����ڸ� ���� ��ų�� ȣ��� Block
	void ApexUserBlock( DWORD dwClient, WORD dwHour );

	//! Apex �� ����� ä���� ���� ��ų�� ȣ��� 
	void ApexUserChatBlock( DWORD dwClient, WORD dwHour );

	//! Apex �� ����� �α׸� ���涧 ȣ���
	void ApexUserHackLog( DWORD dwClient, TCHAR* szComment );
	
	// APEX�� �̻� ����� �α׾ƿ� ����
	void ApexUserLogout(DWORD dwClient);

	//! Apex �� ����޽����� Ŭ���̾�Ʈ���� ������.
	void ApexSendMessageToClient( DWORD dwClient, WORD wAction, WORD wHour );

	void ApexCloseClient( DWORD dwClient );

	void ApexCloseClientFinal();

	///////////////////////////////////////////////////////////////////////////
	/// nPortect ���� �Լ���
	///////////////////////////////////////////////////////////////////////////	
	
	//! nProtect ������ �޽��� ����
	void MsgSndGameGuard( DWORD dwClient, NET_GAMEGUARD_AUTH& nga );

	//! nProtect Ŭ���̾�Ʈ ���� First
	void MsgSndGameGuardFirstCheck( DWORD dwClient );

	//! nProtect Ŭ���̾�Ʈ ���� Second
	void MsgSndGameGuardSecondCheck( DWORD dwClient );

	//! nProtect ������ �޼��� ���
	void MsgGameGuardAnswer(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);

	//! nProtect ������ �޼��� ���
	void MsgGameGuardAnswerFirst(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);

	//! nProtect ������ �޼��� ���
	void MsgGameGuardAnswerSecond(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);

	//! �ʵ弭���� Ŭ���̾�Ʈ���� ���� ���� �α׸� ��´�.
	void MsgTracingLogPrint(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);

	// ���� ID���� �ƴ��� üũ
	int CheckTracingUser( const char *pUserAccount, const int nUserNum );
	
	///////////////////////////////////////////////////////////////////////////
	/// GM ���ɿ� �޽��� �Լ���
	///////////////////////////////////////////////////////////////////////////	
	void MsgGmPkMode(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	void MsgGmwherePcMap(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);

	// ��Ʈ�� Ÿ�� ���� �ӽ� �Լ�
	void MsgVietTimeReset(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg); //vietnamtest%%% && vietnamtest2

    void MsgGmLogItemReloadCA(DWORD ClientSlot, DWORD GaeaID, NET_MSG_GENERIC* pMsg);
    void MsgGmGameDataUpdateCA(DWORD ClientSlot, DWORD GaeaID, NET_MSG_GENERIC* pMsg);
    void MsgGmGameDataUpdateSS( NET_MSG_GENERIC* nmg );

    void MsgUserChangeCountry(DWORD ClientSlot, DWORD GaeaID, NET_MSG_GENERIC* pMsg);

    void MsgCloseClient(DWORD ClientSlot, DWORD GaeaID, NET_MSG_GENERIC* pMsg);

	//! -----------------------------------------------------------------------
	//! Integration Cache Server
	//! -----------------------------------------------------------------------
	virtual void SendIntegrationCache( NET_MSG_GENERIC* pMsg ) override;
	virtual void SendIntegrationCache( EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc = false ) override;


    //! -----------------------------------------------------------------------
    //! Cache Server
    //! -----------------------------------------------------------------------
    int CacheConnect();
	int CacheCloseConnect();
    void CacheMsgProcess(MSG_LIST* pMsg);    
    virtual void SendCache(NET_MSG_GENERIC* pMsg) override;
    virtual void SendCache(EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc=false) override;

    void CacheSendHeartbeat();

	//mjeon.instance.match
	int MatchConnect();
	int MatchCloseConnect();
    BOOL MatchMsgProcess(MSG_LIST* pMsg);
    void MatchSendHeartbeat();
	virtual int SendMatch(NET_MSG_GENERIC* pMsg) override;
    virtual int SendMatch(EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc=false) override;

    //! Point Shop
    void MsgPointShopDataStart(NET_MSG_GENERIC* pMsg);
    void MsgPointShopDataEnd(NET_MSG_GENERIC* pMsg);
    void MsgPointShopData(NET_MSG_GENERIC* pMsg);
    void MsgPointShopGmReloadCA(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* pMsg);

    //! Game
    //! Agent->Client
    void MsgSndChaBasicBAInfoBack(NET_MSG_GENERIC* pMsg);

    //! -----------------------------------------------------------------------
	//! Session Server Communication Member Function	
	//! -----------------------------------------------------------------------
	int	SessionConnect(unsigned long ulServerIP, int nPort);
	int SessionConnect(const char* szServerIP, int nPort);

	//! Session ������ ����� üũ�ϰ� �̻��� ������ �ٽ� �����Ѵ�.
	int SessionReconnect();
	
	//! Session ������ �����ϰ� ������ ������ �����Ѵ�.
	int SessionConnectSndSvrInfo(const char* szServerIP, int nPort);
	int SessionConnectSndSvrInfo();

	int SessionCloseConnect();	
    int	SendSession(NET_MSG_GENERIC* pMsg);
	void SessionMsgProcess(MSG_LIST* pMsg);

	void SessionMsgHeartbeatServerAns();
	
	//! ���Ǽ���->���Ӽ��� : ä�ø޽���
	void SessionMsgChatProcess(NET_MSG_GENERIC* nmg);
	void SessionMsgSvrCmd(NET_MSG_GENERIC* nmg);

	//  �α��� Ű
	void SessionMsgEncryptKey(NET_MSG_GENERIC* nmg);

    void SessionMsgVersionSS(NET_MSG_GENERIC* nmg);

    void DoNothing(NET_MSG_GENERIC* pMsg);
	
	//! Ping Answer	
	void SessionMsgPingAnswer(NET_MSG_GENERIC* nmg);
	//! Send ping
	// void SessionSndPing();
	//! Agent->Session:������ ��� ������ ������.
	int SessionSndSvrInfo();
	//! Send Heartbeat
    void SessionSndHeartbeat(); 
	//! Agent->Session:������ ���� ���¸� ������.
	int SessionSndSvrCurState();
	//! Agent->Session:������ ���� ä�� ���¸� ������.
	int SessionSndChannelState(int nChannel);

	//
	//mjeon.maintenance
	//
	//! ������ ���۽����� ����ų�, ����ġ���� �ð���(attendanceMaintenance ��� ���� ���� �ð��� ���ļ�) ������ ���
	//! ���� ������ ������ ��, �ش� �۾����� �ϰ� �������ش�.
	//! �� ��, �̷��� �۾����� ������ �̹� ó������ �ʾҴ��� ������ �˻��� ���Ŀ� ����Ǿ�� �� ���̴�.
	void CheckMaintenanceJobs();

	//! ���� �ٿ�� UserLoginState���� ����� �ʱ�ȭ ���� �ʴ� ������ �ִ�.
	//! Agent�� ó�� ������ �� UserDB�� UserLoginState���� 0���� �ʱ�ȭ �����ش�.
	void InitUserLoginState();

	int SessionSndCreateCharacter(int nUserNum);
	int	DaumSessionSndCreateCharacter(int nUserNum);

	int	SessionSndTestCreateCharacter(int nUserNum);
	int	DaumSessionSndTestCreateCharacter(int nUserNum);

	int	SessionSndDelCharacter     (int nUserNum);
	int	DaumSessionSndDelCharacter (int nUserNum);

	int SessionSndTestDelCharacter (int nUserNum);
	int DaumSessionSndTestDelCharacter(int nUserNum);

	BOOL UpdateTracingCharacter(NET_MSG_GENERIC* nmg );

	void MsgHeartbeatClientAns(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
    
    void MsgCreateClub(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);

    void MsgJson(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
    bool MsgJsonParser(DWORD ClientSlot, DWORD GaeaID, const std::string& JsonStr);

    void MsgClubLogCA(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
    void MsgClubLogHA(DWORD ClientSlot, DWORD GaeaID, NET_MSG_GENERIC* nmg);
    void MsgClubLogHA_End(DWORD ClientSlot, DWORD GaeaID, NET_MSG_GENERIC* nmg);
    void MsgClubMemberGradeChangeCA(DWORD ClientSlot, DWORD GaeaID, NET_MSG_GENERIC* pMsg);
    void MsgClubMemberCdCertifyCA(DWORD ClientSlot, DWORD GaeaID, const json_spirit::mObject& Obj);
    
    void MsgClubGradeNameChangeCA(DWORD ClientSlot, DWORD GaeaID, NET_MSG_GENERIC* pMsg);

    void MsgClubMemberLobbyCA(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
    void MsgClubNewLobbyCA(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);

    void MsgGmDbUnlockUser(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);

    void MsgLogAttendanceDS(NET_MSG_GENERIC* pMsg);
    void MsgLogAttendanceTaskDS(NET_MSG_GENERIC* pMsg);

protected:
	void BroadcastToField(NET_MSG_GENERIC* pBuffer, DWORD ServerChannel=0);
	int	BroadcastToField(DWORD FieldNum, NET_MSG_GENERIC* pBuffer, DWORD Channel);
	int	BroadcastToField(DWORD SlotNum, NET_MSG_GENERIC* pBuffer);

public:
    void AddGameDbJob(db::DbActionPtr spJob);
    void AddGameAdoJob(db::DbActionPtr spJob);

	//���λ����˻� �ý��� TEST
	void MsgPmarketSearchReqCA(DWORD ClientSlot, DWORD GaeaID, NET_MSG_GENERIC* pMsg);
	void MsgSearchResultHA(DWORD ClientSlot, DWORD GaeaID, NET_MSG_GENERIC* nmg);

	void MsgPrivateMarketItemNumTotalHA( DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );
	void MsgPrivateMarketItemNumHA( DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );
	void MsgPrivateMarketItemHA( DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );
	void MsgPrivateMarketItemEndHA( DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );
	void MsgPrivateMarketLogNumHA( DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );
	void MsgPrivateMarketLogHA( DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );
	void MsgPrivateMarketLogEndHA( DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );
	void MsgPrivateMarketStateHA( DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );

    //���λ��� �˻�ī�� ���
    void MsgPmarketSearchOpenFA(DWORD dwClient, NET_MSG_GENERIC* nmg);
    void MsgPmarketSearchClose(DWORD ClientSlot, DWORD GaeaID, NET_MSG_GENERIC* pMsg);
public:
    std::string GetClientIP(DWORD dwClient) const override;

public:
	void MsgGmReferenceCountCA( DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );
    void MsgGmLoadIPEventCA( DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );

public:
	int SendAuthInfoToSessionServer(); // ���Ǽ����� ���� ���� ����. ���ǿ��� ���������� ������
	int SendEventInfoToSessionServer(); // ���Ǽ����� �̺�Ʈ ���� ����. ���Ǽ����� ���������� ���� ���� ������
	void MsgAuthCertificationResponse(NET_MSG_GENERIC* pMsg);
	void MsgAuthEventInfoRequest(NET_MSG_GENERIC* pMsg);

public:
	// TexasHoldem
	void MsgTexasHolem( DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );

public:
	void MsgBlockUserFA( NET_MSG_GENERIC* nmg );
	bool IsBlockUser( DWORD dwUserNum );

public:
	void MsgGmCommandLogCA( DWORD ClientSlot, DWORD GaeaID, const json_spirit::mObject& Obj );

public:
	// Country
	void MsgUpdateUserCountryFA ( DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );
	void MsgUpdateUserCountryCA ( DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );
	void MsgUpdateUserCountryDA ( DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );

public:
	void MsgDisconnectCharacterFA( NET_MSG_GENERIC* nmg );
	
/*
 * redmine : #1161 Randombox Ȯ������ �ý��� ó��
 * created : sckim, 2015.12.21
 * modified : 
 */
public:
    // ��� ��û
    void MsgGmSetRandomboxChanceReqHA(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);

    // ��ȸ ��û
    void MsgGmGetRandomboxChanceListHA(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);

    // ���� ��û
    void MsgGmDelRandomboxChanceHA(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);

public :
	void MsgRanMobileCommandRequest( DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );
	void MsgRanMobileCommandResponse( DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );
};

extern CAgentServer* g_pAgentServer;

#endif // _LS_AGENT_SERVER_H_
