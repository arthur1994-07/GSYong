#include "../pch.h"
#include "../../SigmaCore/Json/MinJson.h"
#include "../../EngineLib/Common/profile.h"
#include "../../RanLogic/Msg/JsonMsgDefine.h"
#include "./GLGaeaServer.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


void GLGaeaServer::SENDTOAGENT(NET_MSG_GENERIC* nmg)
{
	//InstanceServer는 Agent와 연동해서 동작해서는 안된다. Instance 자체적으로 모든걸 처리할 수 있도록 하라

	GASSERT(m_pMsgServer->GetServerType() != net::SERVER_INSTANCE);
	if (m_pMsgServer->GetServerType() == net::SERVER_INSTANCE)
	{
		sc::writeLogError(std::string("InstanceServer cannot send a msg to AgentServer!"));
		return;
	}

	if (m_pMsgServer)
		m_pMsgServer->SendAgent(nmg);
}

void GLGaeaServer::SENDTOAGENT(EMNET_MSG MsgType, const msgpack::sbuffer& Buffer, bool UseCrc)
{
    //InstanceServer는 Agent와 연동해서 동작해서는 안된다. Instance 자체적으로 모든걸 처리할 수 있도록 하라

    GASSERT(m_pMsgServer->GetServerType() != net::SERVER_INSTANCE);
    if (m_pMsgServer->GetServerType() == net::SERVER_INSTANCE)
    {
        sc::writeLogError(std::string("InstanceServer cannot send a msg to AgentServer!"));
        return;
    }

    if (m_pMsgServer)
        m_pMsgServer->SendAgent(MsgType, Buffer, UseCrc);
}

void GLGaeaServer::SENDTOAGENT(EMNET_MSG MsgType, const std::vector<std::tr1::shared_ptr<msgpack::sbuffer>>& vecSpBuffer, bool UseCrc)
{
    BOOST_FOREACH( const std::tr1::shared_ptr<msgpack::sbuffer>& spBuffer, vecSpBuffer)
        SENDTOAGENT( MsgType, *spBuffer, UseCrc);
}


void GLGaeaServer::SENDTOCACHE(NET_MSG_GENERIC* pMsg)
{
	GASSERT(m_pMsgServer->GetServerType() != net::SERVER_INSTANCE);
	if (m_pMsgServer->GetServerType() == net::SERVER_INSTANCE)
	{
		sc::writeLogError(std::string("InstanceServer cannot send a msg to CacheServer!"));
		return;
	}

    if (m_pMsgServer)
        m_pMsgServer->SendCache(pMsg);
}

void GLGaeaServer::SENDTOCACHE(EMNET_MSG MsgType, const msgpack::sbuffer& Buffer, bool UseCrc)
{
    GASSERT(m_pMsgServer->GetServerType() != net::SERVER_INSTANCE);
    if (m_pMsgServer->GetServerType() == net::SERVER_INSTANCE)
    {
        sc::writeLogError(std::string("InstanceServer cannot send a msg to CacheServer!"));
        return;
    }

    if (m_pMsgServer)
        m_pMsgServer->SendCache(MsgType, Buffer, UseCrc);
}

void GLGaeaServer::SENDTOCACHE(EMNET_MSG MsgType, const std::vector<std::tr1::shared_ptr<msgpack::sbuffer>>& vecSpBuffer, bool UseCrc)
{
    BOOST_FOREACH( const std::tr1::shared_ptr<msgpack::sbuffer>& spBuffer, vecSpBuffer)
        SENDTOCACHE( MsgType, *spBuffer, UseCrc);
}

void GLGaeaServer::SENDTOINTEGRATIONCACHE( NET_MSG_GENERIC* pMsg )
{
	GASSERT( m_pMsgServer->GetServerType() != net::SERVER_INSTANCE );
	if ( m_pMsgServer->GetServerType() == net::SERVER_INSTANCE )
	{
		sc::writeLogError( std::string( "InstanceServer cannot send a msg to CacheServer!" ) );
		return;
	}

	if ( m_pMsgServer )
		m_pMsgServer->SendIntegrationCache( pMsg );
}

void GLGaeaServer::SENDTOINTEGRATIONCACHE( EMNET_MSG MsgType, const msgpack::sbuffer& Buffer, bool UseCrc )
{
	GASSERT( m_pMsgServer->GetServerType() != net::SERVER_INSTANCE );
	if ( m_pMsgServer->GetServerType() == net::SERVER_INSTANCE )
	{
		sc::writeLogError( std::string( "InstanceServer cannot send a msg to CacheServer!" ) );
		return;
	}

	if ( m_pMsgServer )
		m_pMsgServer->SendIntegrationCache( MsgType, Buffer, UseCrc );
}

void GLGaeaServer::SENDTOINTEGRATIONCACHE( EMNET_MSG MsgType, const std::vector< std::tr1::shared_ptr< msgpack::sbuffer > >& vecSpBuffer, bool UseCrc )
{
	BOOST_FOREACH( const std::tr1::shared_ptr< msgpack::sbuffer >& spBuffer, vecSpBuffer )
		SENDTOINTEGRATIONCACHE( MsgType, *spBuffer, UseCrc );
}

void GLGaeaServer::SENDCLUBLOG(DWORD ClubDbNum, club::EM_LOG_TYPE LogType, const std::string& LogMsg)
{
    if (!m_pMsgServer)
        return;

    NET_CLUB_LOG SendData;
    SendData.m_ClubDbNum = ClubDbNum;
    SendData.m_Type      = static_cast<BYTE> (LogType);
    SendData.m_LogMsg    = LogMsg;

    msgpack::sbuffer SendBuffer;
    msgpack::pack(SendBuffer, SendData);

    m_pMsgServer->SendCache(NET_MSG_CLUB_LOG, SendBuffer);
}