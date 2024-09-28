#include "pch.h"
#include "../Product/GLProductField.h"
#include "./GLGaeaServer.h"
#include "../../RanLogic/Msg/ProductMsg.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ------------------ ----------------------------------------------------------

BOOL GLGaeaServer::MsgProductReqAF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
    if(dwClientID != m_dwAgentSlot)
    {
        sc::writeLogError( sc::string::format("MsgProductReqAF Wrong Try [IP:%s]", GetClientIP(dwClientID) ) );
        return FALSE;
    }

    GLMSG::SNET_PRODUCT_REQ* pNetMsg = (GLMSG::SNET_PRODUCT_REQ*)nmg;
    GLChar* pChar = GetCharByDbNum( pNetMsg->dwChaDbNum );
    if ( !pChar )
        return FALSE;
    
    pChar->GetProduct()->ReqProduct( pNetMsg, pChar );

    return TRUE;
}

BOOL GLGaeaServer::MsgProductLearnBookAF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
    if(dwClientID != m_dwAgentSlot)
    {
        sc::writeLogError( sc::string::format("MsgProductReqAF Wrong Try [IP:%s]", GetClientIP(dwClientID) ) );
        return FALSE;
    }

    GLMSG::SNETPC_REQ_LEARN_PRODUCTBOOK* pNetMsg = (GLMSG::SNETPC_REQ_LEARN_PRODUCTBOOK*)nmg;
    GLChar* pChar = GetCharByDbNum( pNetMsg->m_dwChaDbNum );
    if ( !pChar )
        return FALSE;

    pChar->GetProduct()->LearnProductBook( nmg, pChar );
    return TRUE;
}

BOOL GLGaeaServer::MsgProductTypeDelAF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
    NET_MSG_PACK* pPacket = (NET_MSG_PACK*)nmg;
    GLMSG::NET_PRODUCT_TYPE_DEL_REQ RecvData;
    if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
        return FALSE;

    GLChar* pChar = GetCharByDbNum( RecvData.m_dwChaDbNum );
    if ( !pChar )
        return FALSE;

    pChar->GetProduct()->TypeReset( nmg, pChar );
    return FALSE;
}
