#include "pch.h"
#include "./RanLogic/Msg/NotifyMsg.h"

#include "./Notify/NotifyClientMan.h"
#include "./Notify/NotifyClientFactory.h"
#include "GLGaeaClient.h"

#include "../SigmaCore/DebugInclude.h"

void GLGaeaClient::NotifyClear()
{
    m_pNotify->Clear();
}

void GLGaeaClient::ReqNotifyCA()
{
    GLMSG::NOTIFY_REQ_CA MsgCA;
    NETSENDTOAGENT(&MsgCA);
}

void GLGaeaClient::MsgNotifyDataAc(NET_MSG_GENERIC* pMsg)
{
    GLMSG::NOTIFY_DATA_AC* pPacket = (GLMSG::NOTIFY_DATA_AC*) pMsg;
    if( pPacket != NULL )
    {
        NotifyFactory factory;

        GetNotifyClientMan()->Add(
            factory.MakeNotifyClient( 
                this,
                pPacket->m_Guid,
                pPacket->m_Date,
                static_cast<gamelogic::notify::NOTIFY_TYPE> (pPacket->m_Type),
                std::string(pPacket->m_JsonData) ));
    }    
}

void GLGaeaClient::MsgNotifyDataAcEnd(NET_MSG_GENERIC* pMsg)
{
    //GLMSG::NOTIFY_DATA_AC_END* pPacket = (GLMSG::NOTIFY_DATA_AC_END*) pMsg;
    // UI refresh
}