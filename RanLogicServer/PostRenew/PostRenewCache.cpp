#include "pch.h"
#include "PostRenewCache.h"

#include "../../RanLogic/Post/PostDefine.h"
#include "../../RanLogic/Item/GLItemCustom.h"

#include "../../RanLogic/Msg/PostRnMsg.h"

#include "../Database/DBAction/DbActionGamePost.h"
#include "../Server/CacheServer.h"



PostRenewCache::PostRenewCache( CacheServer* pCacheServer )
	: m_pCacheServer(pCacheServer)
{
}

PostRenewCache::~PostRenewCache(void)
{
}

void PostRenewCache::LoadPost( DWORD dwChaDbNum )
{
	{
		// 종료 예약이 되어있다면 예약 해제
		std::list<std::pair<float, DWORD>>::iterator it;
		for( it = m_listCleanUpList.begin() ; it != m_listCleanUpList.end() ; it++ )
		{
			if( it->second == dwChaDbNum )
			{
				m_listCleanUpList.erase(it);
				break;
			}

		}
	}

	const CHANUM_TYPE& nType = PostMultyIndex.get<Key_RecvNum>();
	CHANUM_IT it = nType.find( dwChaDbNum );
	if( it == nType.end() )
	{
		GetListReq2db( dwChaDbNum );	
	}
	
}

void PostRenewCache::UnLoadPost( DWORD dwChaDbNum )
{
	std::list<std::pair<float, DWORD>>::iterator it;

	for( it = m_listCleanUpList.begin() ; it != m_listCleanUpList.end() ; it++ )
	{
		if( it->second == dwChaDbNum )
			return;
	}

	m_listCleanUpList.push_back( std::make_pair(0.0f, dwChaDbNum) );
}

void PostRenewCache::CleanupProcess( float fElapseTime )
{

	std::list<std::pair<float, DWORD>>::iterator it;

	it = m_listCleanUpList.begin();
	
	while( it != m_listCleanUpList.end() )
	{
		it->first += fElapseTime;

		if( POST_CLEANUP_TIME < it->first )
		{
			CleanupPosts( it->second );
			it = m_listCleanUpList.erase( it );
		}
		else
		{
			it++;
		}
	}
}

void PostRenewCache::GetListReq2db( const DWORD& dwChaDbNum )
{
	m_pCacheServer->AddGameAdoJob( 
		db::DbActionPtr( 
		new db::CPostRnSelectAll( dwChaDbNum ) ) );
}

void PostRenewCache::CleanupPosts( DWORD dwChaDbNum )
{
	CHANUM_TYPE& nType = PostMultyIndex.get<Key_RecvNum>();

	std::pair<CHANUM_IT, CHANUM_IT> range = nType.equal_range( dwChaDbNum );

	for( CHANUM_IT it = range.first ; it != range.second ; it++ )
	{
		PostInfo value = *it;
		nType.erase( value.dwRecvChaNum );
	}
}

void PostRenewCache::MsgGetListReq( DWORD dwRecvChaNum, std::vector<PostInfo>& vecPostList OUT )
{
	CHANUM_TYPE& nType = PostMultyIndex.get<Key_RecvNum>();

	std::pair<CHANUM_IT, CHANUM_IT> range = nType.equal_range( dwRecvChaNum );

	for( CHANUM_IT it = range.first ; it != range.second ; it++ )
		vecPostList.push_back(*it);

}

void PostRenewCache::MsgSendReq( DWORD dwSendClientSlot, const PostInfo& sendPost )
{
	SendReq2db( dwSendClientSlot, sendPost );
}

bool PostRenewCache::MsgDeleteReq( DWORD dwSendClientSlot, DWORD dwChaNum, DWORD dwPostID )
{
	ID_TYPE& nType = PostMultyIndex.get<Key_PostID>();

	ID_IT it = nType.find( dwPostID );

	// 변조 패킷은 무시한다.
	if( it == nType.end() )
		return false;

	const PostInfo& rValue = *it;
	if( rValue.dwRecvChaNum != dwChaNum )
		return false;

	// 우선 삭제 DB에 문제가 있다고 하더라도 치명적인 버그를 발생하지 않음으로 바로 상태를 전송한다.
	nType.erase( dwPostID );

	// DB작업
	DeleteReq2db(dwSendClientSlot, dwPostID);

	return true;

}

bool PostRenewCache::MsgDeleteAttachReq( DWORD dwSendClientSlot, DWORD dwChaNum, DWORD dwPostID, GLMSG::NET_POST_RENEW_ATTACH_ACK& SendData OUT )
{
	ID_TYPE& nType = PostMultyIndex.get<Key_PostID>();

	ID_IT it = nType.find( dwPostID );

	// 변조 패킷은 무시한다.
	if( it == nType.end() )
	{
		SendData.emCode = (int)EMPOST_ATTACH_ERROR;
		return false;
	}

	PostInfo value = *it;
	if( value.dwRecvChaNum != dwChaNum )
	{
		SendData.emCode = (int)EMPOST_ATTACH_ERROR;
		return false;
	}

	// EMPOST_ATTACH_TYPE
	if( value.wAttachType & EMPOST_ATTACH_MONEY || value.wAttachType & EMPOST_ATTACH_ITEM )
	{
		// 멀티인덱스는 상수이다.
		// 값을 변경하려면 삭제 후 삽입하여야 한다.
		nType.erase( dwPostID );


		// 플래그만 바꾸고 실데이터는 삭제하지 않는다. 
		// 롤백할 경우가 있기 때문이다.
		value.wAttachType &= ~EMPOST_ATTACH_MONEY;
		value.wAttachType &= ~EMPOST_ATTACH_ITEM;


		PostMultyIndex.insert( value );

		SendData.dwPostID = dwPostID;
		SendData.dwSendChaNum = value.dwSendChaNum;
		SendData.dwRecvChaNum = dwChaNum;
		SendData.sItem = value.sItemCutom;
		SendData.emMoneyType = value.wMoneyType;
		SendData.llMoney = value.llMoney;
		SendData.emCode = (int)EMPOST_ATTACH_OK;
		
		return true;
	}

	SendData.emCode = (int)EMPOST_ATTACH_ITEM_IS_NULL;
	return false;

}

void PostRenewCache::MsgReadReq( DWORD dwPostID, DWORD dwChaNum )
{
	ID_TYPE& nType = PostMultyIndex.get<Key_PostID>();

	ID_IT it = nType.find( dwPostID );

	if( it == nType.end() )
		return;

	PostInfo value = *it;

	if( value.dwRecvChaNum != dwChaNum )
		return;

	if( value.bRead )
		return;

	// 멀티인덱스는 상수이다.
	// 값을 변경하려면 삭제 후 삽입하여야 한다.
	nType.erase( dwPostID );

	value.bRead = true;

	PostMultyIndex.insert( value );


	// DB작업
	SetRead2db(dwPostID);
}

void PostRenewCache::MsgReturnReq( DWORD dwFieldSlot, DWORD dwPostID, DWORD dwReturnChaNum, std::string strReturnChaName, DWORD dwReturnUserNum, DWORD dwRecvChaNum )
{
	//SendReq2db( dwSendClientSlot, sendPost );
	ID_TYPE& nType = PostMultyIndex.get<Key_PostID>();

	ID_IT it = nType.find( dwPostID );

	if( it == nType.end() )
		return;

	PostInfo value = *it;

	// 우편의 소유주인가?
	// 받은사람만이 반송할 수 있다.
	if( dwReturnChaNum != value.dwRecvChaNum )
		return;

	// 유저 메일이 아니면 반송할 수 없다.
	if( value.wMailType == EMPOST_MAIL_SYSTEM )
		return;

	// 대금 청구 상태인가?
	if( value.wAttachType & EMPOST_ATTACH_MONEY || value.wAttachType & EMPOST_ATTACH_ITEM )
	{
		
		nType.erase( dwPostID );

		// 우편의 소유주를 보낸이로 바꾼뒤 새 우편을 전송한다.
		// 받는 사람 = 보낸 사람
		value.dwRecvChaNum = value.dwSendChaNum;

		// 보내는 사람 = 받은 사람
		value.dwSendChaNum	= dwRecvChaNum;
		value.strSendChaName= strReturnChaName;
		value.dwSendUserNum = dwReturnUserNum;

		value.wMailType = EMPOST_MAIL_RETURN;

		PostMultyIndex.insert( value );
		
		// DB작업
		ReturnReq2db( dwFieldSlot, dwPostID, dwReturnChaNum, strReturnChaName, dwReturnUserNum, dwRecvChaNum );
	}
	



}

bool PostRenewCache::MsgReturnAck( DWORD dwPostID, int emCode, PostInfo& sendPost OUT )
{
	ID_TYPE& nType = PostMultyIndex.get<Key_PostID>();

	ID_IT it = nType.find( dwPostID );

	if( it == nType.end() )
		return false;

	if( emCode != EMPOST_SEND_OK )
	{
		ID_TYPE& nType = PostMultyIndex.get<Key_PostID>();
		nType.erase( dwPostID );
		return false;
	}

	sendPost = *it;

	return true;
	
}

void PostRenewCache::MsgAttachRollbackReq( DWORD dwPostID )
{
	ID_TYPE& nType = PostMultyIndex.get<Key_PostID>();

	ID_IT it = nType.find( dwPostID );

	// 변조 패킷은 무시한다.
	if( it == nType.end() )
		return;

	PostInfo value = *it;
	// 멀티인덱스는 상수이다.
	// 값을 변경하려면 삭제 후 삽입하여야 한다.
	nType.erase( dwPostID );


	// 플래그만 바꾸고 실데이터는 삭제하지 않는다. 
	// 롤백할 경우가 있기 때문이다.
	if( value.sItemCutom != NATIVEID_NULL() )
		value.wAttachType |= EMPOST_ATTACH_ITEM;

	if( 0 != value.llMoney )
	{
		value.wAttachType |= EMPOST_ATTACH_MONEY;
	}

	PostMultyIndex.insert( value );
}

void PostRenewCache::SendReq2db( const DWORD& dwReqClientSlot, const PostInfo& sendPost )
{
	m_pCacheServer->AddGameAdoJob( 
		db::DbActionPtr( 
			new db::CPostRnSend( sendPost, dwReqClientSlot ) ) );
}

void PostRenewCache::DeleteReq2db( const DWORD& dwSendClientSlot, const DWORD& dwPostID )
{
	m_pCacheServer->AddGameAdoJob( 
		db::DbActionPtr( 
		new db::CPostRnDel( dwPostID, dwSendClientSlot ) ) );
}

// void PostRenewCache::DeleteAttachReq2db( const DWORD& dwSendClientSlot, const DWORD& dwPostID )
// {
// 	m_pCacheServer->AddGameAdoJob( 
// 		db::DbActionPtr( 
// 		new db::CPostRnDelAttach( dwPostID, dwSendClientSlot ) ) );
// }

void PostRenewCache::SetRead2db( const DWORD& dwPostID )
{
	m_pCacheServer->AddGameAdoJob( 
		db::DbActionPtr( 
		new db::CPostRnSetRead( dwPostID ) ) );
}

void PostRenewCache::ReturnReq2db( const DWORD& dwFieldSlot, const DWORD& dwPostID, const DWORD& dwReturnChaNum, const std::string& strReturnChaName, const DWORD& dwReturnUserNum, const DWORD& dwRecvChaNum )
{
	m_pCacheServer->AddGameAdoJob( 
		db::DbActionPtr( 
		new db::CPostRnReturn( dwFieldSlot, dwPostID, dwReturnChaNum, strReturnChaName, dwReturnUserNum, dwRecvChaNum ) ) );
}

void PostRenewCache::MsgSendAck( const PostInfo post, EMPOST_SEND_CODE emCode )
{
	// DB 처리 완료
	switch ( emCode )
	{
	case EMPOST_SEND_OK:
		const CHANUM_TYPE& nType = PostMultyIndex.get<Key_RecvNum>();


		// 캐싱되어있다면 추가해야한다.
		if( m_pCacheServer->IsOnline( post.dwRecvChaNum ) )
			PostMultyIndex.insert( post );

		break;
	}
}

// void PostRenewCache::MsgDeleteAck( const DWORD dwPostID, EMPOST_DEL_CODE emCode )
// {
// 	// DB 처리 완료
// 	switch ( emCode )
// 	{
// 	case EMPOST_DEL_OK:
// 		ID_TYPE& nType = PostMultyIndex.get<Key_PostID>();
// 
// 		nType.erase( dwPostID );
// 		break;
// 	}
// }

// void PostRenewCache::MsgDeleteAttachAck( const DWORD dwPostID, EMPOST_ATTACH_CODE emCode, SITEMCUSTOM& sItemCustom OUT, LONGLONG& llMoney OUT )
// {
// 
// 	// 성공했을때 여기에 진입한다.
// 	switch ( emCode )
// 	{
// 	case EMPOST_ATTACH_OK:
// 		ID_TYPE& nType = PostMultyIndex.get<Key_PostID>();
// 
// 		ID_IT it = nType.find( dwPostID );
// 
// 		if( it == nType.end() )
// 			return;
// 		
// 		PostInfo value = *it;
// 
// 		// DB에선 데이터가 삭제되었고
// 		//첨무물을 복사한뒤에 서버 데이터를 지운다.
// 		sItemCustom = value.sItemCutom;
// 		llMoney = value.llMoney;
// 
// 		// 멀티인덱스는 상수이다.
// 		// 값을 변경하려면 삭제 후 삽입하여야 한다.
// 		nType.erase( dwPostID );
// 
// 		value.sItemCutom = NATIVEID_NULL();
// 		value.llMoney = 0;
// 		value.wAttachType &= ~EMPOST_ATTACH_MONEY;
// 		value.wAttachType &= ~EMPOST_ATTACH_ITEM;
// 
// 		PostMultyIndex.insert( value );
// 		break;
// 
// 	}
// }


void PostRenewCache::MsgListAck( std::vector<PostInfo> m_vecPostList )
{
	BOOST_FOREACH( const PostInfo& rValue, m_vecPostList )
	{
		PostMultyIndex.insert( rValue );
	}
}
