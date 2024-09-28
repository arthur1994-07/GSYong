#include "pch.h"

#include "../FieldServer/GLGaeaServer.h"

#include "s_CFieldServer.h"

#include "../../SigmaCore/DebugInclude.h"

void CFieldServer::MsgPrivateMarketItemPayMoneyHF( NET_MSG_GENERIC* nmg )
{
	m_pGaeaServer->MsgPrivateMarketItemPayMoneyHF( m_IntegrationCacheServerSlot, GAEAID_NULL, nmg );
}

void CFieldServer::MsgPrivateMarketItemPayMoneyRollbackHF( NET_MSG_GENERIC* nmg )
{
	m_pGaeaServer->MsgPrivateMarketItemPayMoneyRollbackHF( m_IntegrationCacheServerSlot, GAEAID_NULL, nmg );
}

void CFieldServer::MsgPrivateMarketItemBuyHF( NET_MSG_GENERIC* nmg )
{
	m_pGaeaServer->MsgPrivateMarketItemBuyHF( m_IntegrationCacheServerSlot, GAEAID_NULL, nmg );
}

void CFieldServer::MsgPrivateMarketItemIsHoldHF( NET_MSG_GENERIC* nmg )
{
	m_pGaeaServer->MsgPrivateMarketItemIsHoldHF( m_IntegrationCacheServerSlot, GAEAID_NULL, nmg );
}

void CFieldServer::MsgPrivateMarketCommonFB( NET_MSG_GENERIC* nmg )
{
	m_pGaeaServer->MsgPrivateMarketCommonFB( m_IntegrationCacheServerSlot, GAEAID_NULL, nmg );
}

void CFieldServer::MsgConsignmentSaleSendBackAck( NET_MSG_GENERIC* nmg )
{
	m_pGaeaServer->MsgConsignmentSaleSendBackAck( m_IntegrationCacheServerSlot, GAEAID_NULL, nmg );
}

void CFieldServer::MsgConsignmentSalePurchaseReq( NET_MSG_GENERIC* nmg )
{
	m_pGaeaServer->MsgConsignmentSalePurchaseReq( m_IntegrationCacheServerSlot, GAEAID_NULL, nmg );
}

void CFieldServer::MsgConsignmentSalePayMoney( NET_MSG_GENERIC* nmg )
{
	m_pGaeaServer->MsgConsignmentSalePayMoney( m_IntegrationCacheServerSlot, GAEAID_NULL, nmg );
}

void CFieldServer::MsgConsignmentSaleSendPostAndLog( NET_MSG_GENERIC* nmg )
{
	m_pGaeaServer->MsgConsignmentSaleSendPostAndLog( m_IntegrationCacheServerSlot, GAEAID_NULL, nmg );
}

void CFieldServer::MsgPrivateMarketItemNumTotalHF( NET_MSG_GENERIC* nmg )
{
	m_pGaeaServer->MsgPrivateMarketItemNumTotalHF( m_IntegrationCacheServerSlot, GAEAID_NULL, nmg );
}

void CFieldServer::MsgPrivateMarketItemNumHF( NET_MSG_GENERIC* nmg )
{
	m_pGaeaServer->MsgPrivateMarketItemNumHF( m_IntegrationCacheServerSlot, GAEAID_NULL, nmg );
}

void CFieldServer::MsgPrivateMarketItemHF( NET_MSG_GENERIC* nmg )
{
	m_pGaeaServer->MsgPrivateMarketItemHF( m_IntegrationCacheServerSlot, GAEAID_NULL, nmg );
}

void CFieldServer::MsgPrivateMarketItemEndHF( NET_MSG_GENERIC* nmg )
{
	m_pGaeaServer->MsgPrivateMarketItemEndHF( m_IntegrationCacheServerSlot, GAEAID_NULL, nmg );
}

void CFieldServer::MsgPrivateMarketLogNumHF( NET_MSG_GENERIC* nmg )
{
	m_pGaeaServer->MsgPrivateMarketLogNumHF( m_IntegrationCacheServerSlot, GAEAID_NULL, nmg );
}

void CFieldServer::MsgPrivateMarketLogHF( NET_MSG_GENERIC* nmg )
{
	m_pGaeaServer->MsgPrivateMarketLogHF( m_IntegrationCacheServerSlot, GAEAID_NULL, nmg );
}

void CFieldServer::MsgPrivateMarketLogEndHF( NET_MSG_GENERIC* nmg )
{
	m_pGaeaServer->MsgPrivateMarketLogEndHF( m_IntegrationCacheServerSlot, GAEAID_NULL, nmg );
}

void CFieldServer::MsgPrivateMarketStateHF( NET_MSG_GENERIC* nmg )
{
	m_pGaeaServer->MsgPrivateMarketStateHF( m_IntegrationCacheServerSlot, GAEAID_NULL, nmg );
}

void CFieldServer::MsgPrivateMarketPostSendHF( NET_MSG_GENERIC* nmg )
{
	m_pGaeaServer->MsgPrivateMarketPostSendHF( m_IntegrationCacheServerSlot, GAEAID_NULL, nmg );
}
