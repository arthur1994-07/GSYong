#include "../pch.h"
#include "../../Enginelib/Common/profile.h"
#include "../../RanLogic/Msg/PointShopMsg.h"
#include "../../RanLogic/Msg/GLContrlPrivateMarket.h"

#include "../Client/NetFieldClientMan.h"
#include "../FieldServer/GLGaeaServer.h"
#include "./FieldServerConfigXml.h"
#include "./NetServerHelper.h"
#include "./s_CFieldServer.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

int CFieldServer::IntegrationCacheConnect()
{
	if ( m_IntegrationCacheServerSlot != net::INVALID_SLOT && INVALID_SOCKET != m_pClientManager->GetSocket( m_IntegrationCacheServerSlot ) )
	{
		sc::writeLogError(
			sc::string::format( "Integration Cache server is online. slot %1%", m_IntegrationCacheServerSlot ) );
		return NET_OK;
	}

	int CacheSlot = m_pClientManager->GetFreeClientID( true );
	if ( CacheSlot == NET_ERROR )
	{
		sc::writeLogError( std::string( "IntegrationCacheConnect Not enough free network slot" ) );
		return NET_ERROR;
	}

	net::CONNECT_RESULT Connect = net::ConnectAndReceive(
		m_pConfig->GetIntegrationCacheServerIP(), m_pConfig->GetIntegrationCacheServerPort(), m_hIOServer, CacheSlot );
	int Result = Connect.get< 0 >();
	SOCKET ConnectSocket = Connect.get< 1 >();
	if ( Result == NET_ERROR )
	{
		m_pClientManager->ReleaseClientID( CacheSlot );
		sc::writeLogError( std::string( "IntegrationCache server connection failed" ) );
		return NET_ERROR;
	}

	if ( NET_ERROR == m_pClientManager->SetAcceptedClient ( CacheSlot, ConnectSocket, net::SLOT_INTEGRATION_CACHE ) )
	{
		::closesocket( ConnectSocket );
		m_pClientManager->ReleaseClientID( CacheSlot );
		sc::writeLogError( std::string( "IntegrationCacheConnect SetAcceptedClient failed" ) );
		return NET_ERROR;
	}

	// If type is cache, all slots are itself.
	m_IntegrationCacheServerSlot = CacheSlot;
	m_pClientManager->SetSlotType( m_IntegrationCacheServerSlot, net::SLOT_INTEGRATION_CACHE );

	PER_IO_OPERATION_DATA* pIOData = m_pClientManager->GetFreeOverIO( NET_RECV_POSTED );
	if ( NULL == pIOData )
	{
		CloseClient( CacheSlot, net::ERROR_IO_OPERATION, WSAGetLastError() );
		sc::writeLogError( std::string( "IntegrationCacheConnect GetFreeOverIO failed" ) );
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
		sc::writeLogError( sc::string::format( "IntegrationCacheConnect WSARecv %1%", WSAGetLastError() ) );
		return NET_ERROR;
	}

	sc::writeLogInfo( 
		sc::string::format( "Connection success to integration cache server. Current integration cache slot is %1%", m_IntegrationCacheServerSlot ) );

	return NET_OK;
}

int CFieldServer::CacheConnect()
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

	if ( NET_ERROR == m_pClientManager->SetAcceptedClient ( CacheSlot, ConnectSocket, net::SLOT_CACHE) )
	{
		::closesocket( ConnectSocket );
		m_pClientManager->ReleaseClientID( CacheSlot );
		sc::writeLogError( std::string( "CacheConnect SetAcceptedClient failed" ) );
		return NET_ERROR;
	}

    // If type is cache, all slots are itself.
    m_CacheServerSlot = CacheSlot;
	m_pClientManager->SetSlotType( m_CacheServerSlot, net::SLOT_CACHE );	// 이전엔 없었다.

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

void CFieldServer::SendCache(EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc)
{
    NET_MSG_PACK MsgPack;
    if (MsgPack.SetData(MsgType, PackBuffer, UseCrc))
        SendCache(&MsgPack);
}

void CFieldServer::SendCache(NET_MSG_GENERIC* pBuffer)
{
    if ( pBuffer && m_CacheServerSlot != net::INVALID_SLOT )
    {
        PROFILE_MSG_COUNT_SEND( pBuffer );
		m_pClientManager->SendClient2( m_CacheServerSlot, pBuffer );
    }
}

void CFieldServer::CacheMsgProcess(MSG_LIST* pMsg)
{
    if (!pMsg)
        return;

    DWORD dwClient = pMsg->dwClient;
    NET_MSG_GENERIC* pPacket = reinterpret_cast<NET_MSG_GENERIC*> (pMsg->Buffer);
    switch (pPacket->Type())
    {
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
    case NET_MSG_CHAR_POINT_HF:
        MsgPointShopCharPointHF(pPacket);
        break;
	case NET_MSG_GET_CHA_EXTRA_INFO_FH_FB:
		MsgChaExtraInfoHF(pPacket);
		break;
	case NET_MSG_POST_RENEW_LIST_ACK:
		m_pGaeaServer->MsgPostRenewListAck( m_CacheServerSlot, GAEAID_NULL, pPacket );
		break;
	case NET_MSG_POST_RENEW_SEND_ACK:
		m_pGaeaServer->MsgPostRenewSendAck( m_CacheServerSlot, GAEAID_NULL, pPacket );
		break;
	case NET_MSG_POST_RENEW_DEL_ACK:
		m_pGaeaServer->MsgPostRenewDelAck( m_CacheServerSlot, GAEAID_NULL, pPacket );
		break;
	case NET_MSG_POST_RENEW_ATTACH_ACK:
		m_pGaeaServer->MsgPostRenewAttachAck( m_CacheServerSlot, GAEAID_NULL, pPacket );
		break;
	case NET_MSG_POST_RENEW_RETURN_ACK:
		m_pGaeaServer->MsgPostRenewReturnAck( m_CacheServerSlot, GAEAID_NULL, pPacket );
		break;
    /*
     * redmine : #1161 Randombox 확률제어 시스템 처리
     * created : sckim, 2015.12.16, Randombox Open시 확률제어 적용 여부 요청 응답
     * modified : 
     */
	case NET_MSG_GCTL_RANDOMBOX_OPEN_CHANCE_FB_HF:
 #ifndef _RELEASE
    sc::writeLogInfo("MsgReqInvenRandomBoxOpenFB receive data!!");
 #endif
        m_pGaeaServer->MsgReqInvenRandomBoxOpenFB(m_CacheServerSlot, GAEAID_NULL, pPacket);
		break;
    default:
        sc::writeLogError(sc::string::format("CacheMsgProcess unknown message type %1%", pPacket->Type()));
        break;
    }
}

void CFieldServer::CacheSendHeartbeat()
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

void CFieldServer::IntegrationCacheSendHeartbeat()
{
#ifdef HEARTBEAT_LOG
	sc::writeLogInfo( std::string( "[HEARTBEAT] Recv Heartbeat Request from Integration Cache Svr" ) );
#endif

	NET_HEARTBEAT_SERVER_ANS Msg;
	SendIntegrationCache( &Msg );
#ifdef HEARTBEAT_LOG
	sc::writeLogInfo( std::string( "[HEARTBEAT] Send Heartbeat Response to Integration Cache Svr" ) );
#endif
}

void CFieldServer::IntegrationCacheMsgProcess( MSG_LIST* pMsg )
{
    if ( !pMsg )
        return;

    DWORD dwClient = pMsg->dwClient;
    NET_MSG_GENERIC* pPacket = reinterpret_cast< NET_MSG_GENERIC* >( pMsg->Buffer );
    switch ( pPacket->Type() )
    {
    case NET_MSG_HEARTBEAT_SERVER_ANS:
#ifdef HEARTBEAT_LOG
		sc::writeLogInfo( sc::string::format( "[HEARTBEAT] Recv Heartbeat Response from Integration Cache Svr ( %1% )", dwClient ) );
#endif
        m_pClientManager->SetHeartBeat( dwClient );
        break;
    case NET_MSG_HEARTBEAT_SERVER_REQ:
        IntegrationCacheSendHeartbeat();
        break;
	case NET_MSG_GCTRL_PMARKET_ITEM_PAY_MONEY_HF:
		MsgPrivateMarketItemPayMoneyHF( pPacket );
		break;
	case NET_MSG_GCTRL_PMARKET_ITEM_PAY_MONEY_ROLLBACK_HF:
		MsgPrivateMarketItemPayMoneyRollbackHF( pPacket );
		break;
	case NET_MSG_GCTRL_PMARKET_ITEM_BUY_HF:
		MsgPrivateMarketItemBuyHF( pPacket );
		break;
	case NET_MSG_GCTRL_PMARKET_ITEM_IS_HOLD_HF:
		MsgPrivateMarketItemIsHoldHF( pPacket );
		break;
	case NET_MSG_GCTRL_PMARKET_COMMON_FB:
		MsgPrivateMarketCommonFB( pPacket );
		break;
    case NET_MSG_CONSIGNMENT_SALE_ITEM_SENDBACK_ACK:
        MsgConsignmentSaleSendBackAck( pPacket );
        break;
    case NET_MSG_CONSIGNMENT_SALE_PURCHASE_REQ:
        MsgConsignmentSalePurchaseReq( pPacket );
        break;
	case NET_MSG_CONSIGNMENT_SALE_PAY_MONEY_REQ:
		MsgConsignmentSalePayMoney( pPacket );
		break;
	case NET_MSG_CONSIGNMENT_SALE_SEND_POST_AND_LOG_REQ:
		MsgConsignmentSaleSendPostAndLog( pPacket );
		break;
	case NET_MSG_GCTRL_PMARKET_ITEM_NUM_TOTAL_HFC:
		MsgPrivateMarketItemNumTotalHF( pPacket );
		break;
	case NET_MSG_GCTRL_PMARKET_ITEM_NUM_HFC:
		MsgPrivateMarketItemNumHF( pPacket );
		break;
	case NET_MSG_GCTRL_PMARKET_ITEM_HFC:
		MsgPrivateMarketItemHF( pPacket );
		break;
	case NET_MSG_GCTRL_PMARKET_ITEM_END_HFC:
		MsgPrivateMarketItemEndHF( pPacket );
		break;
	case NET_MSG_GCTRL_PMARKET_LOG_NUM_HFC:
		MsgPrivateMarketLogNumHF( pPacket );
		break;
	case NET_MSG_GCTRL_PMARKET_LOG_HFC:
		MsgPrivateMarketLogHF( pPacket );
		break;
	case NET_MSG_GCTRL_PMARKET_LOG_END_HFC:
		MsgPrivateMarketLogEndHF( pPacket );
		break;
	case  NET_MSG_GCTRL_PMARKET_STATE_HFC:
		MsgPrivateMarketStateHF( pPacket );
		break;
	case  NET_MSG_GCTRL_PMARKET_POST_SEND_HF:
		MsgPrivateMarketPostSendHF( pPacket );
		break;
    default:
        sc::writeLogError( sc::string::format( "IntegrationCacheMsgProcess unknown message type %1%", pPacket->Type() ) );
        break;
    }
}

void CFieldServer::SendIntegrationCache( EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc )
{
	NET_MSG_PACK MsgPack;
	if ( MsgPack.SetData( MsgType, PackBuffer, UseCrc ) )
		SendIntegrationCache( &MsgPack );
}

void CFieldServer::SendIntegrationCache( NET_MSG_GENERIC* pBuffer )
{
	if ( pBuffer && m_IntegrationCacheServerSlot != net::INVALID_SLOT )
	{
		PROFILE_MSG_COUNT_SEND( pBuffer );
		m_pClientManager->SendClient2( m_IntegrationCacheServerSlot, pBuffer );
	}
}

void CFieldServer::SendPrivateMarketData()
{
	GLMSG::SNETPC_PMARKET_SEND_DATA_FF InnerMsg;
	InsertMsg ( DBACTION_CLIENT_ID, ( char* ) &InnerMsg );
}