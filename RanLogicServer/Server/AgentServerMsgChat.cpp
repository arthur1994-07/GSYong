#include "../pch.h"

#include "./AgentServer.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

// 게임서버->클라이언트
// 일반적 채팅 메시지 전송
void CAgentServer::MsgSndChatNormal(DWORD dwClient, const char* szName, const char* szMsg)
{
    if (!szName || !szMsg)
        return;

    GLMSG::NET_CHAT_LINK_FB Msg;
    Msg.SetData(NET_MSG_CHAT_LINK_FB, CHAT_TYPE_NORMAL, szName, szMsg);
    SendClient(dwClient, &Msg);
}

void CAgentServer::MsgSndChatGlobal(const char* szChatMsg)
{
    if (!szChatMsg)
        return;

    DWORD dwSndBytes = 0;

    sc::writeLogInfo(std::string(szChatMsg));

    NET_CHAT_CTRL_FB ncf;
    ncf.emType = CHAT_TYPE_GLOBAL;
    ::StringCchCopy(ncf.szChatMsg, GLOBAL_CHAT_MSG_SIZE+1, szChatMsg);
    SendAllClient(&ncf);
}
