#include "pch.h"
#include "../../RanLogic/Msg/NotifyMsg.h"
#include "./GLAgentServer.h"

#include "../../SigmaCore/DebugInclude.h"

BOOL GLAgentServer::MsgNotifyReqCA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
    // GLMSG::NOTIFY_REQ_CA* pPacket = (GLMSG::NOTIFY_REQ_CA*) nmg;
    GLCharAG* pChar = GetChar(dwGaeaID);
    if (pChar)
    {        
        pChar->NotifyReqCA();
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}