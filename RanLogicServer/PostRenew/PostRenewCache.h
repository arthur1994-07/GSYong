#pragma once

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/indexed_by.hpp>
#include <boost/multi_index/composite_key.hpp>

#include "../../RanLogic/Post/PostDefine.h"

namespace GLMSG
{
	struct NET_POST_RENEW_ATTACH_ACK;
}

class CacheServer;

struct Key_PostID{};
struct Key_RecvNum{};

typedef boost::multi_index_container
<
	PostInfo,
	boost::multi_index::indexed_by
	<
		boost::multi_index::ordered_unique
		< 
			boost::multi_index::tag<Key_PostID>, boost::multi_index::member< PostInfo, DWORD, &PostInfo::dwPostID >
		>,
		boost::multi_index::ordered_non_unique
		< 
			boost::multi_index::tag<Key_RecvNum>, boost::multi_index::member< PostInfo, DWORD, &PostInfo::dwRecvChaNum > 
		>
	>
> PostInfoMultiIndex;

static const float POST_CLEANUP_TIME = 30.0f;		// 로그아웃시 30초 후에 지운다.

class PostRenewCache
{
public:
	PostRenewCache( CacheServer* pCacheServer );
	~PostRenewCache(void);

	void LoadPost( DWORD dwChaDbNum );
	void UnLoadPost( DWORD dwChaDbNum );	// 캐릭터 접속 해제
	
	void CleanupProcess( float fElapseTime );
private:
	void GetListReq2db( const DWORD& dwChaDbNum );
	void CleanupPosts( DWORD dwChaDbNum );

public:
	// 클라이언트의 요청
	// void : 로직 결과 필요없음
	// bool : 로직 결과 필요
	void MsgGetListReq( DWORD dwRecvChaNum, std::vector<PostInfo>& vecPostList OUT );
	void MsgSendReq( DWORD dwSendClientSlot, const PostInfo& sendPost );
	bool MsgDeleteReq( DWORD dwSendClientSlot, DWORD dwChaNum, DWORD dwPostID );
	bool MsgDeleteAttachReq( DWORD dwSendClientSlot, DWORD dwChaNum, DWORD dwPostID, GLMSG::NET_POST_RENEW_ATTACH_ACK& SendData OUT );
	void MsgReadReq( DWORD dwPostID, DWORD dwChaNum );
	void MsgReturnReq( DWORD dwFieldSlot, DWORD dwPostID, DWORD dwReturnChaNum, std::string strReturnChaName, DWORD dwReturnUserNum, DWORD dwRecvChaNum );
	bool MsgReturnAck( DWORD dwPostID, int emCode, PostInfo& sendPost OUT );
	void MsgAttachRollbackReq( DWORD dwPostID );

private:
	// DB로 요청
	void SendReq2db( const DWORD& dwReqClientSlot, const PostInfo& sendPost );
	void DeleteReq2db( const DWORD& dwSendClientSlot, const DWORD& dwPostID );
	void SetRead2db( const DWORD& dwPostID );
	void ReturnReq2db( const DWORD& dwFieldSlot, const DWORD& dwPostID, const DWORD& dwReturnChaNum, const std::string& strReturnChaName, const DWORD& dwReturnUserNum, const DWORD& dwRecvChaNum );

	//void DeleteAttachReq2db( const DWORD& dwSendClientSlot, const DWORD& dwPostID );

	

public:
	// Send는 호출부분에서 함에 주의 ( CacheServerMsgGamePost.cpp )
	// DB에서 응답
	void MsgSendAck( const PostInfo, EMPOST_SEND_CODE emCode );
	//void MsgDeleteAck( const DWORD dwPostID, EMPOST_DEL_CODE emCode );
	//void MsgDeleteAttachAck( const DWORD dwPostID, EMPOST_ATTACH_CODE emCode, SITEMCUSTOM& sItemCustom OUT, LONGLONG& llMoney OUT );
	void MsgListAck( std::vector<PostInfo>  m_vecPostList );


private:
	CacheServer* m_pCacheServer;

	PostInfoMultiIndex PostMultyIndex;
	typedef PostInfoMultiIndex::index<Key_PostID>::type				ID_TYPE;
	typedef PostInfoMultiIndex::index<Key_PostID>::type::iterator	ID_IT;
	typedef PostInfoMultiIndex::index<Key_RecvNum>::type			CHANUM_TYPE;
	typedef PostInfoMultiIndex::index<Key_RecvNum>::type::iterator	CHANUM_IT;

	std::list<std::pair<float, DWORD>>	m_listCleanUpList;
};
