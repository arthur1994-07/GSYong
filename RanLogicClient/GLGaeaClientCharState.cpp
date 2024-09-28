#include "pch.h"
#include "./GLGaeaClient.h"
#include "../RanLogic/Msg/CharStateMsg.h"
#include "../RanLogic/Msg/PVEMsg.h"
#include "./Char/GLCharacter.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void GLGaeaClient::MsgCharState(NET_MSG_GENERIC* pMsg)
{
    if (m_pCharacter == 0)
    {
        return;
    }

    GLMSG::NET_CHAR_STATE_FC NetMsg;

    if (CastNetMsg(pMsg, NetMsg))
    {
        switch (NetMsg.idx)
        {
        case CHAR_STATE_INDEX_MOVEMENT:
            m_pCharacter->SetMovement(NetMsg.state);
            break;
        }
    }
}
