#pragma once

#include "../GLGaeaClient.h"
#include "../../RanLogic/GLogicData.h"
#include "../../RanLogic/Post/GLPostData.h"
#include "../../RanLogic/MSG/GLContrlMsg.h"

#include "../Widget/GLWidgetScript.h"

class GLGaeaClient;

class PostRnClient
{
public:
	typedef std::tr1::shared_ptr<PostRnClient>		PostRnClientPtr;
	typedef std::map<DWORD, PostInfo>				MAP_POST_INFO;
	typedef std::map<DWORD, PostInfo>::iterator		MAP_POST_INFO_IT;

	PostRnClient(GLGaeaClient* pGaeaClient) : m_pGaeaClient(pGaeaClient) {}
	~PostRnClient(){};

	void Reset();

	void ReqGetList();						// 리스트 요청
	void ReqSend( const DWORD dwRecvChaNum
				, const std::string strTitle
				, const std::string strContent
				, const WORD wStyle
				, const WORD wMoneyType
				, const LONGLONG llMoney
				, const SINVENITEM* sInvenItem  );		// 보내기
	void ReqReturn( DWORD dwPostID );		// 반송( 보내기 )
	void ReqGetAttach( DWORD dwPostID );	// 첨부물 받기
	void ReqDel( DWORD dwPostID );			// 삭제
	void ReqRead( DWORD dwPostID );			// 읽음

	void MsgListAck( std::vector<PostInfo> vecPostInfo );
	void MsgSendAck();
	void MsgReturn();
	void MsgAttachAck();
	void MsgDelAck();
	void MsgReadAck();

private:
	GLGaeaClient* m_pGaeaClient;
	MAP_POST_INFO m_mapPostInfo;			// 우편들
	
};
