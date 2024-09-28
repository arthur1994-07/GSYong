#include "pch.h"
#include "./Stage/DxGlobalStage.h"
#include "./GLGaeaClient.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void GLGaeaClient::NETSENDTOAGENT(NET_MSG_GENERIC* pMsg)
{
    m_pGlobalStage->NetSend(pMsg);
}

void GLGaeaClient::NETSENDTOAGENT(EMNET_MSG MsgType, const msgpack::sbuffer& Buffer, bool UseCrc)
{
    m_pGlobalStage->NetSend(MsgType, Buffer, UseCrc);
}

void GLGaeaClient::NETSENDTOFIELD(NET_MSG_GENERIC* pMsg)
{
    m_pGlobalStage->NetSendToField(pMsg);
}

void GLGaeaClient::NETSENDTOFIELD(EMNET_MSG MsgType, const msgpack::sbuffer& Buffer, bool UseCrc)
{
    m_pGlobalStage->NetSendToField(MsgType, Buffer, UseCrc);
}
void GLGaeaClient::NETSENDTOMYSELF(NET_MSG_GENERIC* pMsg)
{
	m_pGlobalStage->MsgProcess(pMsg);
}
void GLGaeaClient::NETSENDTOMYSELF(EMNET_MSG MsgType, const msgpack::sbuffer& Buffer, bool UseCrc)
{
	NET_MSG_PACK MsgPack;
	if (MsgPack.SetData(MsgType, Buffer, UseCrc))
		NETSENDTOMYSELF(&MsgPack);
}
DWORD GLGaeaClient::GETMYDBNUM()
{
	return m_pGlobalStage->GetGameStage()->GetChaNum();
}
DWORD GLGaeaClient::GETMYGAEAID()
{
    return m_pGlobalStage->GetGameStage()->GetGaeaID();
}

DWORD GLGaeaClient::GETMYCLIENTID()
{
    return m_pGlobalStage->GetGameStage()->GetClientID();
}