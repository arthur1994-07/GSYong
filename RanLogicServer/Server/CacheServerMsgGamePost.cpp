#include "pch.h"

#include "../../RanLogic/Msg/PostRnMsg.h"

#include "../PostRenew/PostRenewCache.h"
// #include "../Cache/CacheUserLobby.h"
// #include "../Character/GLCharCache.h"
// #include "../Database/DBAction/DbActionGame.h"
// #include "../Database/DbManager.h"
// #include "../Net/NetCachePacket.h"
// #include "../PrivateMarket/GLPrivateMarketSearchMan.h"

#include "./CacheServer.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void CacheServer::MsgPostRenewListReq( DWORD dwClient, NET_MSG_GENERIC* pMsg )
{
	NET_MSG_PACK* pPacket = (NET_MSG_PACK*)pMsg;

	GLMSG::NET_POST_RENEW_LIST_REQ RecvData;
	if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
		return;


	std::vector<PostInfo>  vecPostList;

	if (m_spPostCache)
		m_spPostCache->MsgGetListReq( RecvData.dwRecvCha, vecPostList );


	// 데이터 전송
	GLMSG::NET_POST_RENEW_LIST_ACK SendData;

	if( vecPostList.empty() )
		return;

	SendData.dwRecvCha = (*vecPostList.begin()).dwRecvChaNum;

	int nIndex = 0;
	while( nIndex != vecPostList.size() )
	{
		SendData.m_vecPostList.push_back( vecPostList[nIndex] );

		DWORD dwNextSize = (SendData.m_vecPostList.size() + 1) * sizeof(PostInfo) + sizeof(SendData);

		if( NET_DATA_BUFSIZE <= dwNextSize || nIndex+1 == vecPostList.size()  )
		{
			msgpack::sbuffer SendBuffer;
			msgpack::pack(SendBuffer, SendData);

			SendClient( dwClient, NET_MSG_POST_RENEW_LIST_ACK, SendBuffer );

			SendData.m_vecPostList.clear();
		}

		nIndex++;
	}

}

void CacheServer::MsgPostRenewListDBAck( DWORD dwClient, NET_MSG_GENERIC* pMsg )
{
	if( DBACTION_CLIENT_ID != dwClient )
		return;

	NET_MSG_PACK* pPacket = (NET_MSG_PACK*)pMsg;

	GLMSG::NET_POST_RENEW_LIST_ACK RecvData;
	if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
		return;

	if (m_spPostCache)
		m_spPostCache->MsgListAck( RecvData.m_vecPostList );
}

void CacheServer::MsgPostRenewSendReq( DWORD dwClient, NET_MSG_GENERIC* pMsg )
{
	NET_MSG_PACK* pPacket = (NET_MSG_PACK*)pMsg;

	GLMSG::NET_POST_RENEW_SEND_REQ RecvData;
	if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
		return;

	if (m_spPostCache)
		m_spPostCache->MsgSendReq( dwClient, RecvData.postInfo );
}

void CacheServer::MsgPostRenewSendDBAck( DWORD dwClient, NET_MSG_GENERIC* pMsg )
{
	if( DBACTION_CLIENT_ID != dwClient )
		return;

	NET_MSG_PACK* pPacket = (NET_MSG_PACK*)pMsg;

	GLMSG::NET_POST_RENEW_SEND_ACK RecvData;
	if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
		return;

	if (m_spPostCache)
	{
		m_spPostCache->MsgSendAck( RecvData.postInfo, (EMPOST_SEND_CODE)RecvData.emCode );
		
		// 필드로 쏜다.
		SendClient( RecvData.dwReqClientSlot, pMsg );
	}	
}

void CacheServer::MsgPostRenewDelReq( DWORD dwClient, NET_MSG_GENERIC* pMsg )
{
	NET_MSG_PACK* pPacket = (NET_MSG_PACK*)pMsg;

	GLMSG::NET_POST_RENEW_DEL_REQ RecvData;
	if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
		return;

	
	if (m_spPostCache)
	{
		bool bOk = false;

		bOk = m_spPostCache->MsgDeleteReq( dwClient, RecvData.dwChaNum, RecvData.dwPostID );
		NET_MSG_PACK* pPacket = (NET_MSG_PACK*)pMsg;

		if( bOk )
		{
			GLMSG::NET_POST_RENEW_DEL_ACK SendData;
			SendData.dwPostID = RecvData.dwPostID;
			SendData.dwChaNum = RecvData.dwChaNum;
			SendData.emCode = (int)EMPOST_DEL_OK;

			msgpack::sbuffer SendBuffer;
			msgpack::pack(SendBuffer, SendData);

			// 필드로 쏜다.
			SendClient( dwClient, NET_MSG_POST_RENEW_DEL_ACK, SendBuffer );
		}
	}
}

void CacheServer::MsgPostRenewAttachReq( DWORD dwClient, NET_MSG_GENERIC* pMsg )
{
	NET_MSG_PACK* pPacket = (NET_MSG_PACK*)pMsg;

	GLMSG::NET_POST_RENEW_ATTACH_REQ RecvData;
	if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
		return;

	if (m_spPostCache)
	{
		GLMSG::NET_POST_RENEW_ATTACH_ACK SendData;

		bool bSendReturn = m_spPostCache->MsgDeleteAttachReq( dwClient, RecvData.dwChaNum, RecvData.dwPostID, SendData );
		
		// 처리할 필요 없는 데이터 무시하면 된다.
		if( (EMPOST_ATTACH_CODE)SendData.emCode != EMPOST_ATTACH_OK )
			return;

		// 첨무물건 꺼내기 성공
		// 필드로 전송하여 필드에서 DB작업 후 유저에게 통보한다.
		if( bSendReturn )
		{
			msgpack::sbuffer SendBuffer;
			msgpack::pack(SendBuffer, SendData);
			
			SendClient( dwClient, NET_MSG_POST_RENEW_ATTACH_ACK, SendBuffer );
		}

	}


}

// void CacheServer::MsgPostRenewAttachDBAck( DWORD dwClient, NET_MSG_GENERIC* pMsg )
// {
// 	if( DBACTION_CLIENT_ID != dwClient )
// 		return;
// 
// 	NET_MSG_PACK* pPacket = (NET_MSG_PACK*)pMsg;
// 
// 	GLMSG::NET_POST_RENEW_ATTACH_ACK RecvData;
// 	if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
// 		return;
// 
// 	if (m_spPostCache)
// 	{
// 		m_spPostCache->MsgDeleteAttachAck( RecvData.dwPostID, (EMPOST_ATTACH_CODE)RecvData.emCode, RecvData.sItem, RecvData.llMoney );
// 
// 		// 필드로 쏜다.
// 		SendClient( RecvData.dwReqClientSlot, pMsg );
// 	}	
// 
// }

void CacheServer::MsgPostRenewReadReq( DWORD dwClient, NET_MSG_GENERIC* pMsg )
{
	NET_MSG_PACK* pPacket = (NET_MSG_PACK*)pMsg;

	GLMSG::NET_POST_RENEW_READ_REQ RecvData;
	if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
		return;

	if (m_spPostCache)
		m_spPostCache->MsgReadReq( RecvData.dwPostID, RecvData.dwChaNum );
}

void CacheServer::MsgPostRenewReturnReq( DWORD dwClient, NET_MSG_GENERIC* pMsg )
{
	NET_MSG_PACK* pPacket = (NET_MSG_PACK*)pMsg;

	GLMSG::NET_POST_RENEW_ATTACH_RETURN_REQ RecvData;
	if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
		return;

	if (m_spPostCache)
		m_spPostCache->MsgReturnReq( dwClient, RecvData.dwPostID, RecvData.dwReturnChaNum, RecvData.strReturnChaName, RecvData.dwReturnUserNum, RecvData.dwRecvChaNum );

}

void CacheServer::MsgPostRenewReturnDBAck( DWORD dwClient, NET_MSG_GENERIC* pMsg )
{
	NET_MSG_PACK* pPacket = (NET_MSG_PACK*)pMsg;

	GLMSG::NET_POST_RENEW_ATTACH_RETURN_ACK RecvData;
	if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
		return;

	if ( m_spPostCache == NULL )
		return;

	PostInfo post;
	if ( m_spPostCache->MsgReturnAck( RecvData.postInfo.dwPostID, RecvData.emCode, post ) )
	{
		SendClient( RecvData.dwReqClientSlot, pMsg );
	}
}

void CacheServer::MsgPostRenewAttachRollbackReq( DWORD dwClient, NET_MSG_GENERIC* pMsg )
{
	NET_MSG_PACK* pPacket = (NET_MSG_PACK*)pMsg;

	GLMSG::NET_POST_RENEW_ATTACH_ROLLBACK_REQ RecvData;
	if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
		return;

	if (m_spPostCache)
		m_spPostCache->MsgAttachRollbackReq( RecvData.dwPostID );
}
