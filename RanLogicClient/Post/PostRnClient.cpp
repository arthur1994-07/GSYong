#include "pch.h"
// #include "../../SigmaCore/String/StringUtils.h"
// #include "../../SigmaCore/String/StringFormat.h"
// #include "../../EngineLib/GUInterface/UITextControl.h"
// #include "../../EngineLib/GUInterface/GameTextControl.h"
// 
// #include "../../RanLogic/GLogicData.h"
// #include "../../RanLogic/GLUseFeatures.h"
// #include "../../RanLogic/Item/GLItemMan.h"
// #include "../../RanLogic/GLogicDataMan.h"
#include "../../RanLogic/MSG/PostRnMsg.h"
// 
// #include "../Stage/DxGlobalStage.h"
// #include "../GLGaeaClient.h"
// #include "../Char/GLCharacter.h"
#include "./PostRnClient.h"
// #include "./GLPostClientDefine.h"

#include "../../SigmaCore/DebugInclude.h"


void PostRnClient::Reset()
{
	m_mapPostInfo.clear();
}

void PostRnClient::ReqGetList()
{
	GLMSG::NET_POST_RENEW_LIST_REQ SendData;

	msgpack::sbuffer SendBuffer;
	msgpack::pack(SendBuffer, SendData);

	m_pGaeaClient->NETSENDTOFIELD( NET_MSG_POST_RENEW_LIST_REQ, SendBuffer );
}

void PostRnClient::MsgListAck( std::vector<PostInfo> vecPostInfo )
{
	BOOST_FOREACH( const PostInfo& rValue, vecPostInfo )
	{
		m_mapPostInfo.insert( std::make_pair(rValue.dwPostID, rValue) );
	}

}

void PostRnClient::ReqSend( const DWORD dwRecvChaNum , const std::string strTitle , const std::string strContent , const WORD wStyle , const WORD wMoneyType , const LONGLONG llMoney , const SINVENITEM* sInvenItem )
{
	GLMSG::NET_POST_RENEW_SEND_REQ_FOR_CLIENT SendData;
	SendData.dwRecvChaNum = dwRecvChaNum;
	SendData.strTitle = strTitle;
	SendData.strContent = strContent;
	SendData.wStyle = wStyle;
	SendData.wMoneyType = (int)EMPOST_MONEY_GOLD;
	SendData.llMoney = llMoney;
	if( sInvenItem )
		SendData.sItem = sInvenItem->sItemCustom;
	else
		SendData.sItem = SITEMCUSTOM(NATIVEID_NULL());

	msgpack::sbuffer SendBuffer;
	msgpack::pack(SendBuffer, SendData);

	m_pGaeaClient->NETSENDTOFIELD( NET_MSG_POST_RENEW_SEND_REQ, SendBuffer );
}

void PostRnClient::ReqReturn( DWORD dwPostID )
{
	GLMSG::NET_POST_RENEW_ATTACH_RETURN_REQ SendData;
	SendData.dwPostID = dwPostID;

	msgpack::sbuffer SendBuffer;
	msgpack::pack(SendBuffer, SendData);

	m_pGaeaClient->NETSENDTOFIELD( NET_MSG_POST_RENEW_RETURN_REQ, SendBuffer );
}

void PostRnClient::ReqGetAttach( DWORD dwPostID )
{
	GLMSG::NET_POST_RENEW_ATTACH_REQ SendData;
	SendData.dwPostID = dwPostID;

	msgpack::sbuffer SendBuffer;
	msgpack::pack(SendBuffer, SendData);

	m_pGaeaClient->NETSENDTOFIELD( NET_MSG_POST_RENEW_ATTACH_REQ, SendBuffer );
}

void PostRnClient::ReqDel( DWORD dwPostID )
{
	GLMSG::NET_POST_RENEW_DEL_REQ SendData;
	SendData.dwPostID = dwPostID;

	msgpack::sbuffer SendBuffer;
	msgpack::pack(SendBuffer, SendData);

	m_pGaeaClient->NETSENDTOFIELD( NET_MSG_POST_RENEW_DEL_REQ, SendBuffer );
}

void PostRnClient::ReqRead( DWORD dwPostID )
{
	GLMSG::NET_POST_RENEW_READ_REQ SendData;
	SendData.dwPostID = dwPostID;

	msgpack::sbuffer SendBuffer;
	msgpack::pack(SendBuffer, SendData);

	m_pGaeaClient->NETSENDTOFIELD( NET_MSG_POST_RENEW_READ_REQ, SendBuffer );
}
