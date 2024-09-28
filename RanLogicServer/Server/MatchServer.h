#pragma once

#include "../../EngineLib/G-Logic/DxMsgServer.h"
#include "../../MfcExLib/ExceptionHandler.h"
#include "../../RanLogic/Msg/GLContrlMsg.h"

#include "../DxServerInstance.h"
#include "../../RanLogicServer/MatchSystem/MatchSystem.h"

#include "../Character/GLChar.h"
#include "../Database/DbAction/DbExecuter.h"
#include "./NetServer.h"

#define WM_FINALCLEANUPFINISH (WM_USER+157)

class NetMatchClientMan;
class MatchServerConfigXml;
//class GLGaeaServer;


class MatchServer : public NetServer
{
public:
	MatchServer(HWND hWnd, HWND hEditBox, HWND hEditBoxInfo, SUBPATH* pPath, EMSERVICE_PROVIDER ServiceProvider);
	virtual ~MatchServer();

protected:
	NetMatchClientMan* m_pClientManager;
    
	MatchServerConfigXml *m_pConfig;

	MatchSystem *m_pMatchSystem;
	
	DWORD	m_HeartbeatTimer;
    DWORD	m_SessionCheckTimer;
    BOOL	m_bHeartBeat;
    DWORD	m_SessionSlot;
    EMSERVICE_PROVIDER m_ServiceProvider;

    DWORD	m_SessionServerSlot;

	A_SERVER_INFO m_AgentServerInfo[MAXNUM_OF_AGENTSERVER_TO_MATCH];

	DWORD	m_InstanceServerSlots[MAXNUM_OF_INSTANCESERVER_TO_MATCH];
    
public:    
    virtual int	Start() override;
    virtual int	Stop() override;
	virtual void ForceStop( DWORD Reason ) override;

    virtual int StartClientManager() override;
    virtual int StartWorkThread(ServerConfigXml* pConfig) override;
    virtual int StartUpdateThread() override;

    virtual int WorkProc() override;
    virtual int UpdateProc() override;
    virtual int ListenProc() override;
	virtual int	RegularScheduleProc() override;
    
    virtual int InsertMsg(int nClient, void* pMsg) override;

    virtual PER_IO_OPERATION_DATA* GetFreeOverIO(EMMSG_IO_TYPE nType=NET_SEND_POSTED) override;
    virtual void ReleaseOperationData(PER_IO_OPERATION_DATA* pData) override;

	int SessionConnect();
	int SessionCloseConnect();

private:    
    BOOL ReadConfig();
	void SetUpAgentServerInfo();
        
	void RecvMsgProcess();
	void MsgProcess(MSG_LIST* pMsg);

	void CloseClient(DWORD dwClient);

public:
	//
	//MatchServer's unique methods
	//
	virtual int SendClient(DWORD dwClient, NET_MSG_GENERIC* pMsg);
	
	//virtual void SendAllClient(NET_MSG_GENERIC* pMsg);

	//
	//NET_MSG_CHARACTER에 정의된 수신할 캐릭터로 해당 메시지를 발송 (Key = SvrGroupNum + ChaNum)
	//
	void SendClient(NET_MSG_CHARACTER *nmc, DWORD dwSvrGroupNum);

	void SendClient(NET_MSG_UNIQUEKEY *nmu);
	
	virtual void SendSession(NET_MSG_GENERIC* nmg);
	virtual void SendAgent(DWORD dwSvrGroupNum, NET_MSG_GENERIC* pBuffer);
	
    virtual BOOL SendInstance(DWORD dwInstanceSvrNum, NET_MSG_GENERIC *nmg);
    virtual void SendAllInstance(NET_MSG_GENERIC* pBuffer)		{GASSERT(0&&"구현중 - SendAllInstance()");}
	

protected:
	//virtual int SendSession(NET_MSG_GENERIC* pBuffer, DWORD SendSize);

    void HeartbeatCheck();    
    void HeartbeatSend();
    void HeartbeatRecvCheck();
    void SessionServerConnectionCheck();
	void SessionSndSvrInfo();
	
	void PrintDebugMsg( DWORD dwUdpateTime );

	void Request(MATCH_REQUEST *pRequest)
	{
		m_pMatchSystem->Request(pRequest);
	}

	BOOL Cancel(DWORD dwSvrGroupNum, DWORD dwChaNum)
	{
		UNIQUEID id(dwSvrGroupNum, dwChaNum);
		m_pMatchSystem->Cancel(id.dw64id);
	}

	//
	//Msg Process
	//
	struct MatchPacketFunc
	{
		( void ) ( MatchServer::*Msgfunc ) (DWORD dwClient, NET_MSG_GENERIC* nmg);
		MatchPacketFunc()
		{
			Msgfunc = NULL;
		}
	};
	MatchPacketFunc m_MsgFunc[NET_MSG_GCTRL_MAX];
	void initPacketFunc();

	void MsgHeartbeatReq(DWORD dwClient, NET_MSG_GENERIC* nmg);
    void MsgHeartbeatAns(DWORD dwClient, NET_MSG_GENERIC* nmg);
	void MsgIamAgent(DWORD dwClient, NET_MSG_GENERIC* nmg);
	void MsgNewInstance(DWORD dwClient, NET_MSG_GENERIC* nmg);

	void MsgMatchRequestAM(DWORD dwClient, NET_MSG_GENERIC* nmg);
	void MsgMatchCancelCAM(DWORD dwClient, NET_MSG_GENERIC* nmg);
	void MsgUnMatchRequestIM(DWORD dwClient, NET_MSG_GENERIC* nmg);


	void MsgCreateInstanceFB(DWORD dwClient, NET_MSG_GENERIC* nmg);
	void MsgDeleteInstanceFB(DWORD dwClient, NET_MSG_GENERIC* nmg);

	
	void MsgTransferChar(DWORD dwClient, NET_MSG_GENERIC* nmg);
	void MsgTransferCharDone(DWORD dwClient, NET_MSG_GENERIC* nmg);
	void MsgTransferCharJoin(DWORD dwClient, NET_MSG_GENERIC* nmg);
	void MsgJoinInstanceInfo(DWORD dwClient, NET_MSG_GENERIC* nmg);

	//
	//Relay
	//
	void MsgRelayToInstance(DWORD dwClient, NET_MSG_GENERIC* nmg);
	void MsgRelayToAgent(DWORD dwClient, NET_MSG_GENERIC* nmg);	
};
