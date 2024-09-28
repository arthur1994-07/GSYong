#include "pch.h"
#include "../SigmaCore/String/StringFormat.h"

#include "./Post/GLPostClient.h"
#include "./GLGaeaClient.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void GLGaeaClient::MsgReceivePostNotify(NET_MSG_GENERIC* pMsg) 
{
    m_pPostClient->MsgReceivePostNotify(pMsg);
}

void GLGaeaClient::MsgPostOpenTakeFb(NET_MSG_GENERIC* pMsg) 
{
    m_pPostClient->MsgTakeItem(pMsg);
}

void GLGaeaClient::MsgPostDeleteFb(NET_MSG_GENERIC* pMsg) 
{
    m_pPostClient->MsgDeletePost(pMsg);
}

void GLGaeaClient::MsgPostReturnFb(NET_MSG_GENERIC* pMsg) 
{
    m_pPostClient->MsgReturnPost(pMsg);
}

void GLGaeaClient::MsgPostSendFb(NET_MSG_GENERIC* pMsg) 
{
    m_pPostClient->MsgSendPostFB_Result(pMsg);
}

void GLGaeaClient::MsgPostReqChaNumFb(NET_MSG_GENERIC* pMsg) 
{
    m_pPostClient->MsgSendPostFB_CharID(pMsg);
}

void GLGaeaClient::MsgPostGetStateFb(NET_MSG_GENERIC* pMsg) 
{
    GLGaeaClient::m_pPostClient->MsgReceiveHistory(pMsg);
}

void GLGaeaClient::MsgPostGetSendStateFb(NET_MSG_GENERIC* pMsg) 
{
    GLGaeaClient::m_pPostClient->MsgReceiveSendHistory(pMsg);
}

void GLGaeaClient::MsgPostGetRecvStateFb(NET_MSG_GENERIC* pMsg) 
{
    GLGaeaClient::m_pPostClient->MsgReceiveRecvHistory(pMsg);
}

void GLGaeaClient::MsgPostGetDataFb(NET_MSG_GENERIC* pMsg) 
{
    m_pPostClient->MsgReceivePost(pMsg);
}

void GLGaeaClient::MsgPostGetInfoFb(NET_MSG_GENERIC* pMsg) 
{
    m_pPostClient->MsgReceivePostInfo(pMsg);
}