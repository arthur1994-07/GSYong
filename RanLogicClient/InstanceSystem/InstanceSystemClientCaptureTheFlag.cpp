#include "pch.h"

#include "../../RanLogic/Msg/GLContrlInstanceContentsMsg.h"
#include "../GLGaeaClient.h"

#include "InstanceSystemClient.h"
#include "GLCTFlagEffectMan.h"

namespace InstanceSystem
{
    const bool ManagerClient::_msgProc_CaptureTheFlag_FlagUpdate(const NET_MSG_GENERIC* const _pMessage)
    {
        const GLMSG::NET_INSTANCE_CTF_FLAG_UPDATE_FC* const pMsg = (const GLMSG::NET_INSTANCE_CTF_FLAG_UPDATE_FC* const) _pMessage;
        pGaeaClient->GetCTFlagEffectMan()->SetFlagGuideArrow(pMsg->nFlagType, pMsg->sTargetID, pMsg->bBlink);
        return true;
    }
}