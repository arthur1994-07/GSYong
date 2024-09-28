#include "../pch.h"
#include "../../RanLogic/Msg/PointShopMsg.h"
#include "./SessionServer.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

//! Agent->Session:Point Shop Relaod
void CSessionServer::MsgPsReloadAS(DWORD dwClient, MSG_LIST* pMsg)
{
	// 서버 그룹 개수 많큼 cache 서버가 존재한다.
	// 그래서 기존 로직 수정함
	// http://crm.mincoms.com/Ticket/TicketView/2784
	if ( 0 == GetServerTypeCount( net::SERVER_CACHE ) )
	{
		sc::writeLogError( std::string( "MsgPsReloadAS. Can't Find Cache Server" ) );
		return;
	}
	int nMaxClient = m_pClientManager->GetMaxClient();
	for ( int Client = 0; Client < nMaxClient; ++Client )
	{
		if ( net::SERVER_CACHE == m_pClientManager->GetSvrType( Client ) )
		{
			pointshop::NET_RELOAD_SS MsgSS;
			SendClient( Client, &MsgSS );
			sc::writeLogInfo( std::string( "MsgPsReloadAS. send NET_RELOAD_SS to Cache Server" ) );
		}
	}

	/*
	if ( GetServerTypeCount(net::SERVER_CACHE) == 1 )
	{
		DWORD CacheSlot = FindServer(net::SERVER_CACHE);
		if (CacheSlot == NET_ERROR) 
		{
			// 해당 서버를 찾지 못함
			sc::writeLogError(std::string("MsgPsReloadAS. Can't Find Cache Server"));
			return;
		}

		// Cache 에 명령
		//pointshop::NET_RELOAD_AS* pPacket = (pointshop::NET_RELOAD_AS*) pMsg;
		pointshop::NET_RELOAD_SS MsgSS;
		SendClient(CacheSlot, &MsgSS);
	}
	else
	{
		sc::writeLogError(std::string("MsgPsReloadAS. Cache Server Count is OVER than 1"));
	}
	*/
}