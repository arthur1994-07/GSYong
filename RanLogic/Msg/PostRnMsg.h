#pragma once

#include "../s_NetGlobal.h"

#include "../Post/PostDefine.h"

namespace GLMSG
{
	// 목록
	struct NET_POST_RENEW_LIST_REQ
	{
		DWORD dwRecvCha;
		
		//NET_MSG_POST_RENEW_LIST_REQ,

		MSGPACK_DEFINE( dwRecvCha )
		
		NET_POST_RENEW_LIST_REQ() : dwRecvCha(0) {}
	};
    struct NET_POST_RENEW_LIST_ACK
    {
		DWORD dwRecvCha;
        std::vector<PostInfo>  m_vecPostList;
        //NET_MSG_POST_RENEW_LIST_ACK,
		
		MSGPACK_DEFINE( dwRecvCha, m_vecPostList )

        NET_POST_RENEW_LIST_ACK() : dwRecvCha(0) {}
    };


	// 우편 전송
	struct NET_POST_RENEW_SEND_REQ
	{
		PostInfo postInfo;
		//NET_MSG_POST_RENEW_SEND_REQ

		MSGPACK_DEFINE( postInfo );

		NET_POST_RENEW_SEND_REQ()
		{}
	};

	struct NET_POST_RENEW_SEND_ACK
	{
		DWORD dwReqClientSlot;	// 보낸 사람(캐쉬서버용)
		PostInfo postInfo;
		int emCode;			// EMPOST_SEND_CODE
		//NET_MSG_POST_RENEW_SEND_ACK

		MSGPACK_DEFINE( dwReqClientSlot, postInfo, emCode );

		NET_POST_RENEW_SEND_ACK() : dwReqClientSlot(0) {}
	};

	struct NET_POST_RENEW_CHA_CHECH_ACK
	{
		PostInfo postInfo;
		int emCode;			// EMPOST_SEND_CODE
		//NET_MSG_POST_RENEW_CHA_CHECK_ACK

		MSGPACK_DEFINE( postInfo, emCode );
	};


	// 삭제
	struct NET_POST_RENEW_DEL_REQ
	{
		DWORD dwPostID;
		DWORD dwChaNum;
		//NET_MSG_POST_RENEW_DEL_REQ

		MSGPACK_DEFINE( dwPostID, dwChaNum )

		NET_POST_RENEW_DEL_REQ() : dwPostID(0), dwChaNum(0) {}
	};
	struct NET_POST_RENEW_DEL_ACK
	{
		DWORD dwPostID;
		DWORD dwChaNum;
		int emCode;			// EMPOST_DEL_CODE
		//NET_MSG_POST_RENEW_DEL_ACK

		MSGPACK_DEFINE( dwPostID, dwChaNum, emCode )

		NET_POST_RENEW_DEL_ACK() : dwChaNum(0), dwPostID(0) {}
	};

	// 첨부 물품 받기
	struct NET_POST_RENEW_ATTACH_REQ
	{
		DWORD dwPostID;
		DWORD dwChaNum;
		//NET_MSG_POST_RENEW_ATTACH_REQ

		MSGPACK_DEFINE( dwPostID, dwChaNum )
		
		NET_POST_RENEW_ATTACH_REQ() : dwPostID(0), dwChaNum(0) {}
	};

	struct NET_POST_RENEW_ATTACH_ACK
	{
		DWORD dwSendChaNum;		// 수신자.
		DWORD dwRecvChaNum;		// 수신자.
		DWORD dwPostID;
		SITEMCUSTOM sItem;
		int emMoneyType;	// EMPOST_MONEY_TYPE
		LONGLONG llMoney;
		int emCode;			// EMPOST_ATTACH_CODE
		//NET_MSG_POST_RENEW_ATTACH_ACK

		MSGPACK_DEFINE( dwSendChaNum, dwRecvChaNum, dwPostID, sItem, emMoneyType, llMoney, emCode )
		
		NET_POST_RENEW_ATTACH_ACK() : llMoney(0) {}
	};

	// 읽음 처리
	struct NET_POST_RENEW_READ_REQ
	{
		DWORD dwPostID;
		DWORD dwChaNum;
		//NET_MSG_POST_RENEW_READ_REQ

		MSGPACK_DEFINE( dwPostID, dwChaNum )
		
		NET_POST_RENEW_READ_REQ() : dwPostID(0), dwChaNum(0) {}
	};

	// 첨부물 롤백
	struct NET_POST_RENEW_ATTACH_ROLLBACK_REQ
	{
		DWORD dwPostID;

		//NET_MSG_POST_RENEW_ATTACH_ROLLBACK_REQ

		MSGPACK_DEFINE( dwPostID )

		NET_POST_RENEW_ATTACH_ROLLBACK_REQ() : dwPostID(0) {}
	};


	// 첨부물 롤백
	struct NET_POST_RENEW_ATTACH_RETURN_REQ
	{
		DWORD dwPostID;
		DWORD			dwReturnChaNum;
		std::string		strReturnChaName;
		DWORD			dwReturnUserNum;
		DWORD			dwRecvChaNum;

		//NET_MSG_POST_RENEW_RETURN_REQ

		MSGPACK_DEFINE( dwPostID, dwReturnChaNum, strReturnChaName, dwReturnUserNum, dwRecvChaNum );

		NET_POST_RENEW_ATTACH_RETURN_REQ() : dwPostID(0) {}
	};

	// 첨부물 롤백
	struct NET_POST_RENEW_ATTACH_RETURN_ACK
	{
		DWORD dwReqClientSlot;	// 보낸 사람(캐쉬서버용)
		PostInfo postInfo;
		int emCode;			// EMPOST_RETURN_CODE
		//NET_MSG_POST_RENEW_RETURN_ACK

		MSGPACK_DEFINE( dwReqClientSlot, postInfo, emCode );
	};


	//////////////////////////////////////////////////////////////////////////
	// 패킷 타입(EMNET_MSG)은 같은 타입을 사용함에 유의
	// 클라에서 전송한 메시지
	struct NET_POST_RENEW_SEND_REQ_FOR_CLIENT
	{
		DWORD dwRecvChaNum;
		std::string strTitle;
		std::string strContent;
		WORD wStyle;
		WORD wMoneyType;
		LONGLONG llMoney;
		SITEMCUSTOM sItem;

		//NET_MSG_POST_RENEW_SEND_REQ

		MSGPACK_DEFINE( dwRecvChaNum, strTitle, strContent, wStyle, wMoneyType, llMoney, sItem );

		NET_POST_RENEW_SEND_REQ_FOR_CLIENT()
		{}
	};

	// 클라에서 받을 메시지
	struct NET_POST_RENEW_ATTACH_ACK_FOR_CLIENT
	{
		DWORD dwPostID;
		int emCode;	// EMPOST_ATTACH_CODE
		//NET_MSG_POST_RENEW_ATTACH_ACK

		MSGPACK_DEFINE( dwPostID,emCode );

		NET_POST_RENEW_ATTACH_ACK_FOR_CLIENT(){}
	};

	struct NET_POST_RENEW_SEND_ACK_FOR_CLIENT
	{
		int emCode;			// EMPOST_SEND_CODE
		//NET_MSG_POST_RENEW_SEND_ACK

		MSGPACK_DEFINE( emCode );
	};

	// 첨부물 롤백
	struct NET_POST_RENEW_ATTACH_RETURN_ACK_FOR_CLIENT
	{
		int emCode;			// EMPOST_RETURN_CODE
		//NET_MSG_POST_RENEW_RETURN_ACK

		MSGPACK_DEFINE( emCode );
	};

	struct NET_POST_RENEW_NEW_POST
	{
		PostInfo postInfo;

		//NET_MSG_POST_RENEW_NEW_POST

		MSGPACK_DEFINE( postInfo );
	};


} // namespace GLMSG