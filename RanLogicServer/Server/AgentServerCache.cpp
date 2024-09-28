#include "../pch.h"
#include "../../Enginelib/Common/profile.h"
#include "../../RanLogic/Msg/PointShopMsg.h"

#include "../Net/NetLogDefine.h"
#include "../Client/NetAgentClientMan.h"
#include "./AgentServerConfigXml.h"
#include "./NetServerHelper.h"
#include "./AgentServer.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

int CAgentServer::CacheConnect()
{    
    if ( m_CacheServerSlot != net::INVALID_SLOT && INVALID_SOCKET != m_pClientManager->GetSocket( m_CacheServerSlot ) )
    {
        sc::writeLogError(
            sc::string::format( "Cache server is online. slot %1%", m_CacheServerSlot ) );
        return NET_OK;
    }

    int CacheSlot = m_pClientManager->GetFreeClientID( true );
    if ( CacheSlot == NET_ERROR )
    {
        sc::writeLogError( std::string( "CacheConnect Not enough free network slot" ) );
        return NET_ERROR;
    }
    
    net::CONNECT_RESULT Connect = net::ConnectAndReceive(
        m_pConfig->GetCacheServerIP(),
        m_pConfig->GetCacheServerPort(),
        m_hIOServer,
        CacheSlot );
    int Result = Connect.get<0> ();
    SOCKET ConnectSocket = Connect.get<1> ();
    if ( Result == NET_ERROR )
    {
        m_pClientManager->ReleaseClientID( CacheSlot );
        sc::writeLogError( std::string( "Cache server connection failed" ) );
        return NET_ERROR;
    }

	if ( NET_ERROR == m_pClientManager->SetAcceptedClient( CacheSlot, ConnectSocket, net::SLOT_CACHE ) )
	{
		::closesocket( ConnectSocket );
		m_pClientManager->ReleaseClientID( CacheSlot );
		sc::writeLogError( std::string( "CacheConnect SetAcceptedClient failed" ) );
		return NET_ERROR;
	}

	// If type is cache, all slots are itself.
	m_CacheServerSlot = CacheSlot;
	m_pClientManager->SetSlotType( m_CacheServerSlot, net::SLOT_CACHE );    

	PER_IO_OPERATION_DATA* pIOData = m_pClientManager->GetFreeOverIO( NET_RECV_POSTED );
	if ( NULL == pIOData )
	{
		CloseClient( CacheSlot, net::ERROR_IO_OPERATION, WSAGetLastError() );
		sc::writeLogError( std::string( "CacheConnect GetFreeOverIO failed" ) );
		return NET_ERROR;
	}

	// add reference
	m_pClientManager->AddReference( CacheSlot );

	DWORD Flags = 0;
	DWORD dwRcvBytes = sizeof( NET_MSG_GENERIC );
	int nRetCode = WSARecv( ConnectSocket, &( pIOData->DataBuf ), 1, &dwRcvBytes, &Flags, &( pIOData->Overlapped ), NULL );
	if ( nRetCode == SOCKET_ERROR && ( WSAGetLastError() != ERROR_IO_PENDING ) )
	{
		// release reference
		m_pClientManager->ReleaseReference( CacheSlot );
		m_pClientManager->ReleaseOperationData( pIOData );
		CloseClient( CacheSlot, net::ERROR_IO_OPERATION, WSAGetLastError() );
		sc::writeLogError( sc::string::format( "CacheConnect WSARecv %1%", WSAGetLastError() ) );
		return NET_ERROR;
	}
    
    sc::writeLogInfo( 
        sc::string::format( "Connection success to cache server. Current cache slot is %1%", m_CacheServerSlot ) );

    // Point Shop 정보 요청
    pointshop::NET_REQ_DATA_SH MsgSH;
    SendCache( &MsgSH );

    return NET_OK;
}

int CAgentServer::CacheCloseConnect()
{
	return m_pClientManager->CloseClient( m_CacheServerSlot );
}

void CAgentServer::SendCache(EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc)
{
    NET_MSG_PACK MsgPack;
    if (MsgPack.SetData(MsgType, PackBuffer, UseCrc))
        SendCache(&MsgPack);
}

void CAgentServer::SendCache( NET_MSG_GENERIC* pMsg )
{
    if ( pMsg && m_CacheServerSlot != net::INVALID_SLOT )
    {
        PROFILE_MSG_COUNT_SEND( pMsg );
		m_pClientManager->SendClient2( m_CacheServerSlot, pMsg );
    }
}

void CAgentServer::SendIntegrationCache( EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc )
{
	NET_MSG_PACK MsgPack;
	if ( MsgPack.SetData( MsgType, PackBuffer, UseCrc ) )
		SendIntegrationCache( &MsgPack );
}

void CAgentServer::SendIntegrationCache( NET_MSG_GENERIC* pMsg )
{
}

void CAgentServer::CacheMsgProcess(MSG_LIST* pMsg)
{
    if (!pMsg)
        return;

    DWORD dwClient = pMsg->dwClient;
    NET_MSG_GENERIC* pPacket = reinterpret_cast<NET_MSG_GENERIC*> (pMsg->Buffer);
    switch (pPacket->Type())
    {
    case NET_MSG_JSON:
        MsgJson(dwClient, GAEAID_NULL, pPacket);
        break;
    case NET_MSG_HEARTBEAT_SERVER_ANS:
#ifdef HEARTBEAT_LOG
		sc::writeLogInfo(sc::string::format("[HEARTBEAT] Recv Heartbeat Response from Cache Svr ( %1% )", dwClient));
#endif

        m_pClientManager->SetHeartBeat(dwClient);
        break;
    case NET_MSG_HEARTBEAT_SERVER_REQ:
        CacheSendHeartbeat();        
        break;
    case NET_MSG_PS_DATA_START_HS:
        MsgPointShopDataStart(pPacket);
        break;    
    case NET_MSG_PS_DATA_END_HS:
        MsgPointShopDataEnd(pPacket);
        break;
    case NET_MSG_PS_DATA_HS:
        MsgPointShopData(pPacket);
        break;
    case NET_MSG_CHA_BAINFO_HA:
        MsgSndChaBasicBAInfoBack(pPacket);
        break;
    case NET_MSG_LOBBY_CHAR_INFO_HA:
        MsgSndChaBasicInfoBack(pPacket);
        break;
    case NET_MSG_LOBBY_CHAR_INFO_HA_END:
        MsgSndChaBasicInfoBackEnd(pPacket);
        break;
    case NET_MSG_CLUB_LOG_HA:
        MsgClubLogHA(dwClient, GAEAID_NULL, pPacket);
        break;
    case NET_MSG_CLUB_LOG_HA_END:
        MsgClubLogHA_End(dwClient, GAEAID_NULL, pPacket);
        break;
	case  NET_MSG_GCTRL_PMARKET_SEARCH_RESULT_HAC:
		MsgSearchResultHA(dwClient, GAEAID_NULL, pPacket);
		break;
// 	case  NET_MSG_GCTRL_PMARKET_ITEM_NUM_HAC:
// 		MsgPrivateMarketItemNumHA( dwClient, GAEAID_NULL, pPacket );
// 		break;
// 	case  NET_MSG_GCTRL_PMARKET_ITEM_HAC:
// 		MsgPrivateMarketItemHA( dwClient, GAEAID_NULL, pPacket );
// 		break;
// 	case  NET_MSG_GCTRL_PMARKET_ITEM_END_HAC:
// 		MsgPrivateMarketItemEndHA( dwClient, GAEAID_NULL, pPacket );
// 		break;
// 	case  NET_MSG_GCTRL_PMARKET_LOG_NUM_HAC:
// 		MsgPrivateMarketLogNumHA( dwClient, GAEAID_NULL, pPacket );
// 		break;
// 	case  NET_MSG_GCTRL_PMARKET_LOG_HAC:
// 		MsgPrivateMarketLogHA( dwClient, GAEAID_NULL, pPacket );
// 		break;
// 	case  NET_MSG_GCTRL_PMARKET_LOG_END_HAC:
// 		MsgPrivateMarketLogEndHA( dwClient, GAEAID_NULL, pPacket );
// 		break;
// 	case  NET_MSG_GCTRL_PMARKET_STATE_HAC:
// 		MsgPrivateMarketStateHA( dwClient, GAEAID_NULL, pPacket );
// 		break;
// 	case  NET_MSG_GCTRL_PMARKET_ITEM_NUM_TOTAL_HFC:
// 		MsgPrivateMarketItemNumTotalHA( dwClient, GAEAID_NULL, pPacket );
// 		break;
    /*
     * redmine : #1161 Randombox 확률제어 시스템 처리
     * created : sckim, 2015.12.21, Cache server feedback 값
     * modified : 
     */
	case  NET_MSG_GM_RANDOMBOX_CHANCE_REGISTER_FB_HA:
        MsgGmSetRandomboxChanceReqHA(dwClient, GAEAID_NULL, pPacket);
		break;
	case  NET_MSG_GM_RANDOMBOX_CHANCE_LIST_FB_HA:
		MsgGmGetRandomboxChanceListHA(dwClient, GAEAID_NULL, pPacket);
		break;
	case  NET_MSG_GM_RANDOMBOX_CHANCE_DELETE_FB_HA:
		MsgGmDelRandomboxChanceHA(dwClient, GAEAID_NULL, pPacket);
		break;
    default:
        sc::writeLogError(sc::string::format("CacheMsgProcess unknown message type %1%", pPacket->Type()));
        break;
    }
}

void CAgentServer::CacheSendHeartbeat()
{
#ifdef HEARTBEAT_LOG
	sc::writeLogInfo(std::string("[HEARTBEAT] Recv Heartbeat Request from Cache Svr"));
#endif

    NET_HEARTBEAT_SERVER_ANS Msg;
    SendCache(&Msg);

#ifdef HEARTBEAT_LOG
	sc::writeLogInfo(std::string("[HEARTBEAT] Send Heartbeat Response to Cache Svr"));
#endif
}

void CAgentServer::MsgLogAttendanceDS(NET_MSG_GENERIC* pMsg)
{
    NET_MSG_PACK* pPacket = (NET_MSG_PACK*) pMsg;
    NET_LOG_ATTENDANCE DataDS;
    if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), DataDS))
    {
        sc::writeLogDebug("ERROR CAgentServer::MsgLogAttendanceDS");
        return;
    }

    msgpack::sbuffer BufferAH;
    msgpack::pack(BufferAH, DataDS);

    SendCache(NET_LOG_ATTENDANCE_SH, BufferAH);
}

void CAgentServer::MsgLogAttendanceTaskDS(NET_MSG_GENERIC* pMsg)
{
    NET_MSG_PACK* pPacket = (NET_MSG_PACK*) pMsg;
    NET_LOG_ATTENDANCE_TASK DataDS;
    if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), DataDS))
    {
        sc::writeLogDebug("ERROR CAgentServer::MsgLogAttendanceDS");
        return;
    }

    msgpack::sbuffer BufferAH;
    msgpack::pack(BufferAH, DataDS);

    SendCache(NET_LOG_ATTENDANCE_TASK_SH, BufferAH);
}

/*
 * redmine : #1161 Randombox 확률제어 시스템 처리
 * created : sckim, 2015.12.21, Cache server feedback 처리
 * modified : 
 */
void CAgentServer::MsgGmSetRandomboxChanceReqHA(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	m_pGLAgentServer->MsgGmSetRandomboxChanceReqHA(nmg, dwClient, dwGaeaID);
}

void CAgentServer::MsgGmGetRandomboxChanceListHA(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	m_pGLAgentServer->MsgGmGetRandomboxChanceListHA(nmg, dwClient, dwGaeaID);
}

void CAgentServer::MsgGmDelRandomboxChanceHA(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	m_pGLAgentServer->MsgGmDelRandomboxChanceHA(nmg, dwClient, dwGaeaID);
}
