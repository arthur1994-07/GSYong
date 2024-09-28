#include "pch.h"
//#include "../../EngineLib/GUInterface/GameTextControl.h"
//#include "../Database/DBAction/DbActionGame.h"
#include "../../RanLogic/Msg//ProductMsg.h"
#include "./GLAgentServer.h"
#include "../Product/GLProductAgent.h"

#include "../Character/GLCharAG.h"


// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

// 기존 메세지
BOOL GLAgentServer::MsgProductReqCA( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
    GLCharAG* pChar = GetChar( dwGaeaID );
    if( pChar == NULL )
        return FALSE;

    pChar->MsgReqProduction(nmg);
    return TRUE;
}


BOOL GLAgentServer::MsgProductLearnBookCA( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
    GLCharAG* pChar = GetChar( dwGaeaID );
    if( pChar == NULL )
        return FALSE;

    pChar->MsgReqProductIsLearnBook(nmg);
    return TRUE;
}

BOOL GLAgentServer::MsgProductBookListCA( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
    GLCharAG* pChar = GetChar( dwGaeaID );
    if( pChar == NULL )
        return FALSE;

    pChar->MsgReqProductBookList(nmg);
    return TRUE;
}

BOOL GLAgentServer::MsgProductTypeListCA( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
    GLCharAG* pChar = GetChar( dwGaeaID );
    if( pChar == NULL )
        return FALSE;

    pChar->MsgReqProductTypeList(nmg);
    return TRUE;
}

BOOL GLAgentServer::MsgProductTypeAddCA( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
    GLCharAG* pChar = GetChar( dwGaeaID );
    if( pChar == NULL )
        return FALSE;

    pChar->MsgReqProductTypeAdd(nmg);
    return TRUE;
}

BOOL GLAgentServer::MsgProductTypeDelCA( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
    GLCharAG* pChar = GetChar( dwGaeaID );
    if( pChar == NULL )
        return FALSE;

    pChar->MsgReqProductTypeDel(nmg);
    return TRUE;
}

BOOL GLAgentServer::MsgProductIsGetExpListCA( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
    GLCharAG* pChar = GetChar( dwGaeaID );
    if( pChar == NULL )
        return FALSE;

    pChar->MsgReqProductIsGetExpList(nmg);
    return TRUE;
}

BOOL GLAgentServer::MsgProductReqFA( NET_MSG_GENERIC* nmg )
{
    GLMSG::SNET_PRODUCT_FA* NetMsg = (GLMSG::SNET_PRODUCT_FA*) nmg;
    
    GLCharAG* pChar = GetCharByDbNum( NetMsg->dwChaDbNum );
    if( pChar == NULL )
        return FALSE;

    pChar->MsgAckProductFA(nmg);
    return TRUE;
}

BOOL GLAgentServer::MsgProductIsLearnBookFA( NET_MSG_GENERIC* nmg )
{
    GLMSG::SNETPC_REQ_LEARN_PRODUCTBOOK_FB* NetMsg = (GLMSG::SNETPC_REQ_LEARN_PRODUCTBOOK_FB*) nmg;

    GLCharAG* pChar = GetCharByDbNum( NetMsg->dwChaDbNum );
    if( pChar == NULL )
        return FALSE;

    pChar->MsgAckLearnBookFA(nmg);
    return TRUE;
}

BOOL GLAgentServer::MsgProductTypeDelFA( NET_MSG_GENERIC* nmg )
{
    NET_MSG_PACK* pPacket = (NET_MSG_PACK*)nmg;
    GLMSG::NET_PRODUCT_TYPE_DEL_ACK RecvData;
    if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
        return FALSE;

    GLCharAG* pChar = GetCharByDbNum( RecvData.m_dwChaDbNum );
    if( pChar == NULL )
        return FALSE;

    pChar->MsgAckTypeDeleteFA(nmg);
    return TRUE;
}

BOOL GLAgentServer::MsgProductTypeListDA( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
    NET_MSG_PACK* pPacket = (NET_MSG_PACK*)nmg;
    GLMSG::NET_PRODUCT_TYPE_LIST_ACK RecvData;
    if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
        return FALSE;

    GLCharAG* pChar = GetCharByDbNum( RecvData.m_dwChaDbNum );
    if( pChar == NULL )
        return FALSE;

    if( pChar->m_pProduct == NULL )
        return FALSE;

    pChar->m_pProduct->SetTypeList( RecvData.m_vecType, this, pChar );

    return TRUE;
}

BOOL GLAgentServer::MsgProductBookListDA( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
    NET_MSG_PACK* pPacket = (NET_MSG_PACK*)nmg;
    GLMSG::NET_PRODUCT_BOOK_LIST_ACK RecvData;
    if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
        return FALSE;

    GLCharAG* pChar = GetCharByDbNum( RecvData.m_dwChaDbNum );
    if( pChar == NULL )
        return FALSE;

    if( pChar->m_pProduct == NULL )
        return FALSE;

    pChar->m_pProduct->SetBookList( RecvData.m_vecBookList, this, pChar );

    return TRUE;
}

BOOL GLAgentServer::MsgProductIsGetExpListDA( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
    NET_MSG_PACK* pPacket = (NET_MSG_PACK*)nmg;
    GLMSG::NET_PRODUCT_ISGETEXP_LIST_ACK RecvData;
    if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
        return FALSE;

    GLCharAG* pChar = GetCharByDbNum( RecvData.m_dwChaDbNum );
    if( pChar == NULL )
        return FALSE;

    if( pChar->m_pProduct == NULL )
        return FALSE;

    pChar->m_pProduct->SetIsGetExpList( RecvData.m_vecIsGetExp, this, pChar, RecvData.m_bFirst, RecvData.m_bLast );

    return TRUE;

}

BOOL GLAgentServer::MsgProductExchangeLogFA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
	GLMSG::NET_PRODUCT_EXCHANGE_LOG_FA* RecvData =
		static_cast<GLMSG::NET_PRODUCT_EXCHANGE_LOG_FA*>( nmg );

	GLCharAG* pChar = GetCharByDbNum( RecvData->m_dwDbNum );
	if( pChar == NULL )
		return FALSE;

	if( pChar->m_pProduct == NULL )
		return FALSE;

	pChar->m_pProduct->LogProductExchangeMaterial( RecvData->m_exchangeLog, this, RecvData->m_dwDbNum );

	return TRUE;
}