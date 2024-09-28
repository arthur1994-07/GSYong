#ifndef S_AUTHSERVER_H_
#define S_AUTHSERVER_H_

#pragma once

#include "../../SigmaCore/String/StringFormat.h"
#include "../../SigmaCore/Log/LogMan.h"
#include "../../RanLogic/Network/NetLogicDefine.h"
#include "../Client/NetServerDefine.h"
#include "../Database/DBAction/DbExecuter.h"
#include "./NetServer.h"

class NetAuthClientMan;
class AuthServerConfigXml;
struct NET_SERVER_CHANNEL_INFO;
struct NET_MSG_GENERIC;
namespace sc {
	class CGlobalAuthServerLogic;
}

extern size_t wrapper_curl_write_callback( void *ptr, size_t size, size_t nmemb, void *userp );

class CAuthServer : public NetServer
{
public:
	CAuthServer(HWND hWnd, HWND hEditBox, HWND hEditBoxInfo, SUBPATH* pPath, EMSERVICE_PROVIDER ServiceProvider);
	virtual ~CAuthServer();

protected:
	NetAuthClientMan*	m_pClientManager; //! Client Manager
    AuthServerConfigXml* m_pConfig;

public:
	sc::CGlobalAuthServerLogic*	m_pAuthManager; 

public:
	virtual int	Start() override;
	virtual int	Stop() override;
	virtual void ForceStop( DWORD Reason ) override;

    virtual int StartClientManager() override;
    virtual int WorkProc() override;
    virtual int ListenProc() override;
    virtual int UpdateProc() override;
	virtual int	RegularScheduleProc() override;
    virtual int InsertMsg(int nClient, void* pMsg);
    
    virtual PER_IO_OPERATION_DATA* GetFreeOverIO(EMMSG_IO_TYPE nType=NET_SEND_POSTED) override;
    virtual void ReleaseOperationData( PER_IO_OPERATION_DATA* pData ) override;

    int StartDbManager();
    void Send(DWORD dwClient, PER_IO_OPERATION_DATA* PerIoData, DWORD dwSize);    
    
	int SendClient(DWORD dwClient, NET_MSG_GENERIC* pMsg);    
	int SendClient(DWORD dwClient, EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc=false);
    
	void SendAllClient(NET_MSG_GENERIC* pMsg);
    
    void CloseClient(DWORD dwClient);

    void CloseAllClient();
    int  FindServer(CString strIP, int nPort);

    void SetSvrInfo(DWORD dwClient, G_AUTH_INFO *gsi);

public:
	struct AuthPacketFunc
	{
		(void) (CAuthServer::*Msgfunc) (DWORD dwClient, MSG_LIST* pMsg);
		AuthPacketFunc()
		{
			Msgfunc = NULL;
		}
	};
	AuthPacketFunc m_MsgFunc[NET_MSG_GCTRL_MAX];

	void initPacketFunc();

public:
    int MsgProcess(MSG_LIST* pMsg);
	void MsgAuthCertificationRequest(DWORD dwClient, MSG_LIST* pMsg);
	void MsgCloseServer(DWORD dwClient, MSG_LIST* pMsg);
	void MsgAuthSessionStateResponse(DWORD dwClient, MSG_LIST* pMsg);
	void MsgAuthEventInfoResponse(DWORD dwClient, MSG_LIST* pMsg); 

	void ResponseAuthCertification(DWORD dwClient, MSG_LIST* pMsg);
	int SendAuthCertificationToSessionServer(DWORD dwClient, G_AUTH_INFO &authInfo);
	BOOL CertificateAuthDataFromDB(DWORD dwClient);
	void RequestSessionStateInfo();
	void RequestAuthEventInfo();
	// 세션서버에 각각의 서버들 상태 요청
	void RequestServerStateInfo();
	void SendRequestServerStateInfo();
	void MsgAuthServerStateResponse(DWORD dwClient, MSG_LIST* pMsg);

	int SendCountryAccessApprove(DWORD dwClient, const std::vector<int>& vecCountryList );
};

#endif 