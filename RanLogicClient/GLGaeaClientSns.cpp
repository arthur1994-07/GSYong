#include "pch.h"
#include "../SigmaCore/String/StringFormat.h"

#include "./Land/GLLandManClient.h"
#include "./SNS/GLSNSClient.h"
#include "./Char/GLCharClient.h"
#include "./GLGaeaClient.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void GLGaeaClient::MsgSnsAuthInfoMsg(NET_MSG_GENERIC* pMsg) 
{
    m_pSNSClient->GetAuthInfoMsg( pMsg );
}

void GLGaeaClient::MsgSnsAuthFb(NET_MSG_GENERIC* pMsg) 
{
    m_pSNSClient->FBAuthedMsg( pMsg );
}

void GLGaeaClient::MsgSnsUnAuthFb(NET_MSG_GENERIC* pMsg) 
{
    m_pSNSClient->FBUnAuthedMsg( pMsg );
}

void GLGaeaClient::MsgSnsFriendAdd(NET_MSG_GENERIC* pMsg) 
{
    m_pSNSClient->FBFriendAddMsg( pMsg );
}

void GLGaeaClient::MsgSnsUpdateBrFb(NET_MSG_GENERIC* pMsg) 
{
    GLMSG::SNET_SNS_FB_UPDATE_BR *pNetMsg = (GLMSG::SNET_SNS_FB_UPDATE_BR*) pMsg;

    std::tr1::shared_ptr<GLCharClient> pChar = m_pLandMClient->GetChar ( pNetMsg->dwGaeaID );
    if ( !pChar )
    {
        return;
    }

    pChar->m_byFB = pNetMsg->byFB;
}

void GLGaeaClient::MsgSnsTwAuthedFb(NET_MSG_GENERIC* pMsg) 
{
    m_pSNSClient->TTAuthedMsg( pMsg );
}

void GLGaeaClient::MsgSnsTwUnAuthedFb(NET_MSG_GENERIC* pMsg) 
{
    m_pSNSClient->TTUnAuthedMsg( pMsg );
}

void GLGaeaClient::MsgSnsTwGetUidFb(NET_MSG_GENERIC* pMsg) 
{
    m_pSNSClient->TTFollowMsg( pMsg );
}

void GLGaeaClient::MsgSnsTwUpdateBr(NET_MSG_GENERIC* pMsg) 
{
    GLMSG::SNET_SNS_TW_UPDATE_BR *pNetMsg = (GLMSG::SNET_SNS_TW_UPDATE_BR*) pMsg;

    std::tr1::shared_ptr<GLCharClient> pChar = m_pLandMClient->GetChar ( pNetMsg->dwGaeaID );
    if ( !pChar )
    {
        return;
    }

    pChar->m_byTW = pNetMsg->byTW;
}
