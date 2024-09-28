#ifndef DXMSGSERVER_H_
#define DXMSGSERVER_H_

#pragma once

#include "../../SigmaCore/gassert.h"
#include "../../RanLogic/Network/NetLogicDefine.h"

struct NET_MSG_GENERIC;
enum EMNET_MSG;

namespace msgpack 
{
    class sbuffer;
} // namespace msgpack 

class DxMsgServer
{
public:
    DxMsgServer() {}
    virtual ~DxMsgServer() {}

public:
	virtual int SendClient(NET_MSG_GENERIC* pBuffer, DWORD dwChaNum) = 0; //mjeon.AF
    virtual int SendClient(EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, DWORD dwChaNum, bool UseCrc=false) = 0;

	virtual int SendClient(DWORD dwClient, NET_MSG_GENERIC* pBuffer) = 0;
    virtual int SendClient(DWORD dwClient, EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc=false) = 0;

	virtual int SendField(DWORD dwClient, NET_MSG_GENERIC* pBuffer) = 0;
    virtual int SendField(DWORD dwClient, EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc=false) = 0;

	virtual int SendField(DWORD dwChannel, DWORD dwFieldSvrNum, NET_MSG_GENERIC* pBuffer) = 0;
    virtual int SendField(DWORD dwChannel, DWORD dwFieldSvrNum, EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc=false) = 0;

	virtual int SendFieldSvr(int nSvrNum, NET_MSG_GENERIC* pBuffer, int nServerChannel=0) = 0;
    virtual int SendFieldSvr(int nSvrNum, EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, int nServerChannel=0, bool UseCrc=false) = 0;

	virtual int SendAgent(NET_MSG_GENERIC* pBuffer) = 0;
    virtual int SendAgent(EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc=false) = 0;

	virtual int SendAgent ( DWORD dwClient, NET_MSG_GENERIC* pBuffer ) = 0;
	virtual int SendAgent ( DWORD dwClient, EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc = false ) = 0;
	virtual int SendAgent_ServerGroup ( DWORD dwServerGroup, NET_MSG_GENERIC* pBuffer ) = 0;
	virtual int SendAgent_ServerGroup ( DWORD dwServerGroup, EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc = false ) = 0;
    
	virtual int SendMatch(NET_MSG_GENERIC* pMsg) = 0;
    virtual int SendMatch(EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc=false) = 0;

    virtual void SendCache(NET_MSG_GENERIC* pMsg) = 0;
    virtual void SendCache(EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc=false) = 0;

	virtual void SendIntegrationCache( NET_MSG_GENERIC* pMsg ) = 0;
	virtual void SendIntegrationCache( EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc = false ) = 0;

	virtual void SendAllField(NET_MSG_GENERIC* pBuffer, int nServerChannel=0) = 0;
    virtual void SendAllField(EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, int nServerChannel=0, bool UseCrc=false) = 0;

	virtual void SendChannel(NET_MSG_GENERIC* pBuffer, int nServerChannel=0) = 0;
    virtual void SendChannel(EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, int nServerChannel=0, bool UseCrc=false) = 0;

	virtual int SendAllChannel(NET_MSG_GENERIC* pBuffer) = 0;
    virtual int SendAllChannel(EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc=false) = 0;

	virtual void SendAllClient(NET_MSG_GENERIC* pBuffer) {}
    virtual void SendAllClient(EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc=false) {}

    virtual void SendMySelf(DWORD dwClient, NET_MSG_GENERIC* pBuffer) {}
    virtual void SendMySelf(DWORD dwClient, EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc=false) {}

    virtual int GetServerGroup() const { return 0; }
	virtual int GetServerNum() const { return 0; }

	virtual std::string GetIP() = 0;
	virtual int	GetPort() = 0;

    virtual std::string GetClientIP(DWORD dwClient) const = 0;


	virtual net::EMSERVERTYPE GetServerType() = 0;
	
	//mjeon.AF
	//virtual int ConnectFieldSvr(DWORD dwClient, int nFieldServer, DWORD dwGaeaID, int nServerChannel=0){ GASSERT(0&&"CloseClient()가 제정의되지 않은 상태에서 호출됨"); return 0; }
	virtual int SetUpFieldInfo(DWORD dwClient, int nFieldServer, DWORD dwGaeaID, int nServerChannel=0){ GASSERT(0&&"CloseClient()가 제정의되지 않은 상태에서 호출됨"); return 0; }
	
    virtual void CloseClient(DWORD dwClient, net::CLOSE_TYPE emCloseType = net::UNDEFINE, int nLastError = 0)   { GASSERT(0&&"CloseClient()가 제정의되지 않은 상태에서 호출됨"); }
	virtual void ResetGaeaID(DWORD dwClient)   { GASSERT(0&&"ResetGaeaID()가 제정의되지 않은 상태에서 호출됨"); }	
	virtual bool IsSlotOnline(DWORD dwClient)  { GASSERT(0&&"IsSlotOnline()가 제정의되지 않은 상태에서 호출됨"); return TRUE; }
};

#endif // DXMSGSERVER_H_