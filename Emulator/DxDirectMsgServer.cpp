#include "stdafx.h"
#include "../RanLogicClient/Stage/DxGlobalStage.h"
#include "../RanLogicServer/FieldServer/GLGaeaServer.h"
#include "./DxDirectMsgServer.h"

#include "../SigmaCore/DebugInclude.h"

DxDirectMsgServer::DxDirectMsgServer(GLGaeaServer* pGaeaServer, DxGlobalStage* pGlobalStage)
    : m_pGaeaServer(pGaeaServer)
    , m_pGlobalStage(pGlobalStage)
{
}

DxDirectMsgServer::~DxDirectMsgServer()
{
}

int DxDirectMsgServer::SendClient(EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, DWORD dwChaNum, bool UseCrc)
{
    NET_MSG_PACK MsgPack;
    if (MsgPack.SetData(MsgType, PackBuffer, UseCrc))
        return SendClient(&MsgPack, dwChaNum);
    else
        return NET_ERROR;
}

int DxDirectMsgServer::SendClient(NET_MSG_GENERIC* pBuffer, DWORD dwChaNum)
{
    DxGameStage* pGameStage = m_pGlobalStage->GetGameStage();
	if (pGameStage->GetChaNum() == dwChaNum)
    {
        m_pGlobalStage->MsgProcess(pBuffer);
        return NET_OK;
    }
    else
    {
        return NET_ERROR;
    }
}

int DxDirectMsgServer::SendClient(DWORD dwClient, EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc)
{
    NET_MSG_PACK MsgPack;
    if (MsgPack.SetData(MsgType, PackBuffer, UseCrc))
        return SendClient(dwClient, &MsgPack);
    else
        return NET_ERROR;
}

int DxDirectMsgServer::SendClient(DWORD dwClient, NET_MSG_GENERIC* pBuffer)
{
    DxGameStage* pGameStage = m_pGlobalStage->GetGameStage();    
    if (pGameStage->GetClientID() == dwClient)
    {
        m_pGlobalStage->MsgProcess(pBuffer);
        return NET_OK;
    }
    else
    {
        return NET_ERROR;
    }
}

int DxDirectMsgServer::SendField(DWORD dwClient, EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc)
{
    NET_MSG_PACK MsgPack;
    if (MsgPack.SetData(MsgType, PackBuffer, UseCrc))
        return SendField(dwClient, &MsgPack);
    else
        return NET_ERROR;
}

int DxDirectMsgServer::SendField(DWORD dwClient, NET_MSG_GENERIC* pBuffer)
{
    GASSERT( 0 && "SendField()" );
	return NET_ERROR;
}

int DxDirectMsgServer::SendField(DWORD dwChannel, DWORD dwFieldSvrNum, EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc)
{
    NET_MSG_PACK MsgPack;
    if (MsgPack.SetData(MsgType, PackBuffer, UseCrc))
        return SendField(dwChannel, dwFieldSvrNum, &MsgPack);
    else
        return NET_ERROR;
}

int DxDirectMsgServer::SendField(DWORD dwChannel, DWORD dwFieldSvrNum, NET_MSG_GENERIC* pBuffer)
{
    GASSERT( 0 && "SendField()" );
    return NET_ERROR;
}

int DxDirectMsgServer::SendFieldSvr(int nSvrNum, EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, int nServerChannel, bool UseCrc)
{
    NET_MSG_PACK MsgPack;
    if (MsgPack.SetData(MsgType, PackBuffer, UseCrc))
        return SendFieldSvr(nSvrNum, &MsgPack, nServerChannel);
    else
        return NET_ERROR;
}

int DxDirectMsgServer::SendFieldSvr(int nSvrNum, NET_MSG_GENERIC* pBuffer, int nServerChannel)
{
    GASSERT( 0 && "SendFieldSvr()" );
    return NET_ERROR;
}

int DxDirectMsgServer::SendAgent(EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc)
{
    NET_MSG_PACK MsgPack;
    if (MsgPack.SetData(MsgType, PackBuffer, UseCrc))
        return SendAgent(&MsgPack);
    else
        return NET_ERROR;
}

int DxDirectMsgServer::SendAgent(NET_MSG_GENERIC* pBuffer)
{
	if (!m_pGaeaServer->MsgProcess(pBuffer, 0, 0))
    {
		m_pGlobalStage->MsgProcess(pBuffer);
        return NET_OK;
    }
    else
    {
        return NET_ERROR;
    }
}

int DxDirectMsgServer::SendAgent ( DWORD dwClient, NET_MSG_GENERIC* pBuffer )
{
	return NET_ERROR;
}

int DxDirectMsgServer::SendAgent ( DWORD dwClient, EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc )
{
	return NET_ERROR;
}

int DxDirectMsgServer::SendAgent_ServerGroup ( DWORD dwServerGroup, NET_MSG_GENERIC* pBuffer )
{
	return NET_ERROR;
}

int DxDirectMsgServer::SendAgent_ServerGroup ( DWORD dwServerGroup, EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc )
{
	return NET_ERROR;
}

int DxDirectMsgServer::SendMatch(EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc)
{
    NET_MSG_PACK MsgPack;
    if (MsgPack.SetData(MsgType, PackBuffer, UseCrc))
        return SendMatch(&MsgPack);
    else
        return NET_ERROR;
}

int DxDirectMsgServer::SendMatch(NET_MSG_GENERIC* pBuffer)
{
    return NET_ERROR;
}

void DxDirectMsgServer::SendCache(EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc)
{
    NET_MSG_PACK MsgPack;
    if (MsgPack.SetData(MsgType, PackBuffer, UseCrc))
        SendCache(&MsgPack);
}

void DxDirectMsgServer::SendCache(NET_MSG_GENERIC* pBuffer)
{
}

void DxDirectMsgServer::SendIntegrationCache( EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc )
{
	NET_MSG_PACK MsgPack;
	if ( MsgPack.SetData( MsgType, PackBuffer, UseCrc ) )
		SendIntegrationCache( &MsgPack );
}

void DxDirectMsgServer::SendIntegrationCache( NET_MSG_GENERIC* pBuffer )
{
}

void DxDirectMsgServer::SendAllField(EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, int nServerChannel, bool UseCrc)
{
    NET_MSG_PACK MsgPack;
    if (MsgPack.SetData(MsgType, PackBuffer, UseCrc))
        SendAllField(&MsgPack, nServerChannel);
}

void DxDirectMsgServer::SendAllField(NET_MSG_GENERIC* pBuffer, int nServerChannel)
{
	GASSERT( 0 && "SendAllField()" );
}

void DxDirectMsgServer::SendChannel(EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, int nServerChannel, bool UseCrc)
{
    NET_MSG_PACK MsgPack;
    if (MsgPack.SetData(MsgType, PackBuffer, UseCrc))
        SendChannel(&MsgPack, nServerChannel);
}

void DxDirectMsgServer::SendChannel(NET_MSG_GENERIC* pBuffer, int nServerChannel)
{
	GASSERT( 0 && "SendAllField()" );
}

int DxDirectMsgServer::SendAllChannel(EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc)
{
    NET_MSG_PACK MsgPack;
    if (MsgPack.SetData(MsgType, PackBuffer, UseCrc))
        return SendAllChannel(&MsgPack);
}

int DxDirectMsgServer::SendAllChannel(NET_MSG_GENERIC* pBuffer)
{
	GASSERT( 0 && "SendAllField()" );
	return 0;
}

net::EMSERVERTYPE DxDirectMsgServer::GetServerType()
{
	return net::SERVER_UNKNOWN;
}

std::string DxDirectMsgServer::GetIP()
{
 return std::string(""); 
}

int	DxDirectMsgServer::GetPort()
{
	return -1;
}

std::string DxDirectMsgServer::GetClientIP( DWORD dwClient ) const
{
    return std::string("");
}
