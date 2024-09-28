#include "pch.h"

#include "./AgentServer.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void CAgentServer::MsgPrivateMarketItemNumTotalHA( DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	m_pGLAgentServer->MsgPrivateMarketItemNumTotalHA( nmg, dwClient, dwGaeaID );
}

void CAgentServer::MsgPrivateMarketItemNumHA( DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	m_pGLAgentServer->MsgPrivateMarketItemNumHA( nmg, dwClient, dwGaeaID );
}

void CAgentServer::MsgPrivateMarketItemHA( DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	m_pGLAgentServer->MsgPrivateMarketItemHA( nmg, dwClient, dwGaeaID );
}

void CAgentServer::MsgPrivateMarketItemEndHA( DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	m_pGLAgentServer->MsgPrivateMarketItemEndHA( nmg, dwClient, dwGaeaID );
}

void CAgentServer::MsgPrivateMarketLogNumHA( DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	m_pGLAgentServer->MsgPrivateMarketLogNumHA( nmg, dwClient, dwGaeaID );
}

void CAgentServer::MsgPrivateMarketLogHA( DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	m_pGLAgentServer->MsgPrivateMarketLogHA( nmg, dwClient, dwGaeaID );
}

void CAgentServer::MsgPrivateMarketLogEndHA( DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	m_pGLAgentServer->MsgPrivateMarketLogEndHA( nmg, dwClient, dwGaeaID );
}

void CAgentServer::MsgPrivateMarketStateHA( DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	m_pGLAgentServer->MsgPrivateMarketStateHA( nmg, dwClient, dwGaeaID );
}
