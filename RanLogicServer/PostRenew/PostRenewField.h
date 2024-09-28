#pragma once

#include "../../RanLogic/Post/PostDefine.h"


class GLGaeaServer;

class PostRenewField
{
public:
	PostRenewField( GLGaeaServer* pGaeaServer ) : m_pGaeaServer(pGaeaServer){};
	~PostRenewField(void){};

	// 수신자는 여기서 체크하지 않음에 주의.
	void SendPost( 
		DWORD dwChaNumSend, 
		DWORD dwChaNumRecv, 
		std::string strTitle, 
		std::string strConTent, 
		WORD wPostStyle =0,
		EMPOST_MONEY_TYPE wMoneyType = EMPOST_MONEY_GOLD,
		LONGLONG llMoney=0, 
		SITEMCUSTOM sItemCustom=SITEMCUSTOM(NATIVEID_NULL()) );

	// 클라이언트의 요청
	void MsgSendReq( DWORD dwChaDbNum, const PostInfo& sendPost );
	//void MsgDeleteReq( DWORD dwSendClientSlot, DWORD dwPostID );
	//void MsgDeleteAttachReq( DWORD dwSendClientSlot, DWORD dwPostID );
	//void MsgGetListReq( DWORD dwRecvChaNum, std::vector<PostInfo>& vecPostList OUT );
	//void MsgReadReq( DWORD dwPostID );

	void MsgSendAck( DWORD dwChaNum, const PostInfo& postInfo );
	void MsgDeleteAck( DWORD dwChaNum, const PostInfo& postInfo );
	EMPOST_ATTACH_CODE MsgTakeAttachAck( const DWORD& dwSendChaNum, const DWORD& dwRecvChaNum, const DWORD& dwPostID, const SITEMCUSTOM& sItem, const EMPOST_MONEY_TYPE& emMoneyType, const LONGLONG& llMoney, const EMPOST_ATTACH_CODE& emCode );
	void MsgGetListReq( DWORD dwChaNum, const PostInfo& postInfo );
	void MsgReadAck( DWORD dwChaNum, const PostInfo& postInfo );

private:
	GLGaeaServer* m_pGaeaServer;

};
