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

	void ReqGetList();						// ����Ʈ ��û
	void ReqSend( const DWORD dwRecvChaNum
				, const std::string strTitle
				, const std::string strContent
				, const WORD wStyle
				, const WORD wMoneyType
				, const LONGLONG llMoney
				, const SINVENITEM* sInvenItem  );		// ������
	void ReqReturn( DWORD dwPostID );		// �ݼ�( ������ )
	void ReqGetAttach( DWORD dwPostID );	// ÷�ι� �ޱ�
	void ReqDel( DWORD dwPostID );			// ����
	void ReqRead( DWORD dwPostID );			// ����

	void MsgListAck( std::vector<PostInfo> vecPostInfo );
	void MsgSendAck();
	void MsgReturn();
	void MsgAttachAck();
	void MsgDelAck();
	void MsgReadAck();

private:
	GLGaeaClient* m_pGaeaClient;
	MAP_POST_INFO m_mapPostInfo;			// �����
	
};
