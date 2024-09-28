#ifndef _DX_DIRECT_MSG_SERVER_H_
#define _DX_DIRECT_MSG_SERVER_H_

#pragma once

#include "../EngineLib/G-Logic/DxMsgServer.h"

class DxGlobalStage;
class GLGaeaServer;

class DxDirectMsgServer : public DxMsgServer
{
public:
    DxDirectMsgServer(GLGaeaServer* pGaeaServer, DxGlobalStage* pGlobalStage);
    virtual ~DxDirectMsgServer();

protected:
    DxGlobalStage* m_pGlobalStage;
    GLGaeaServer* m_pGaeaServer;

public:
	virtual int SendClient(NET_MSG_GENERIC* pBuffer, DWORD dwChaNum) override;
    virtual int SendClient(EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, DWORD dwChaNum, bool UseCrc=false) override;

    virtual int SendClient(DWORD dwClient, NET_MSG_GENERIC* pBuffer) override;
    virtual int SendClient(DWORD dwClient, EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc=false) override;

    virtual int SendField(DWORD dwClient, NET_MSG_GENERIC* pBuffer) override;
    virtual int SendField(DWORD dwClient, EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc=false) override;

	virtual int SendField(DWORD dwChannel, DWORD dwFieldSvrNum, NET_MSG_GENERIC* pBuffer) override;
    virtual int SendField(DWORD dwChannel, DWORD dwFieldSvrNum, EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc=false) override;

    virtual int SendFieldSvr(int nSvrNum, NET_MSG_GENERIC* pBuffer, int nServerChannel=0) override;
    virtual int SendFieldSvr(int nSvrNum, EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, int nServerChannel=0, bool UseCrc=false) override;
    
    virtual int SendAgent(NET_MSG_GENERIC* pBuffer) override;
    virtual int SendAgent(EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc=false) override;

	virtual int SendAgent ( DWORD dwClient, NET_MSG_GENERIC* pBuffer ) override;
	virtual int SendAgent ( DWORD dwClient, EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc = false ) override;
	virtual int SendAgent_ServerGroup ( DWORD dwServerGroup, NET_MSG_GENERIC* pBuffer ) override;
	virtual int SendAgent_ServerGroup ( DWORD dwServerGroup, EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc = false ) override;

	virtual int SendMatch(NET_MSG_GENERIC* pBuffer) override;
    virtual int SendMatch(EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc=false) override;

	virtual void SendCache(NET_MSG_GENERIC* pBuffer) override;
    virtual void SendCache(EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc=false) override;

	virtual void SendIntegrationCache( NET_MSG_GENERIC* pBuffer ) override;
	virtual void SendIntegrationCache( EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc = false ) override;

	virtual void SendAllField(NET_MSG_GENERIC* pBuffer, int nServerChannel=0) override;
    virtual void SendAllField(EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, int nServerChannel=0, bool UseCrc=false) override;

	virtual void SendChannel(NET_MSG_GENERIC* pBuffer, int nServerChannel=0) override;
    virtual void SendChannel(EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, int nServerChannel=0, bool UseCrc=false) override;

    virtual int SendAllChannel(NET_MSG_GENERIC* pBuffer) override;
    virtual int SendAllChannel(EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc=false) override;

	virtual net::EMSERVERTYPE GetServerType() override;

	virtual std::string GetIP() override;
	virtual int			GetPort() override;

    virtual std::string GetClientIP(DWORD dwClient) const override;
};

#endif // _DX_DIRECT_MSG_SERVER_H_