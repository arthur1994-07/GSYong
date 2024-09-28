#include "pch.h"
#include "ServerStartStageInstance.h"
#include "ServerTriggerFinger.h"
#include "ServerStartStageTrigger.h"
#include "ServerEntryMessage.h"

#include "../Land/GLLandMan.h"
#include "../../RanLogic/Msg/PVEMsg.h"

#include "../SigmaCore/DebugInclude.h"

using namespace TriggerSystem;

void CServerStartStageInstance::Update(DWORD dwCharDbNum)
{
	CServerTriggerFinger* pFinger = 0; 
    const CServerStartStageTrigger* pTrigger = 0;    

    if (CastFinger(pFinger) && CastTrigger(pTrigger))
    {
        GLMSG::NET_PVE_MSG_START_STAGE_FC msg;
        msg.nStage = pFinger->GetStage();
        msg.nTime = pTrigger->GetLifeTime();
        SendMsgClient(dwCharDbNum, NET_MSG_PVE_MSG_START_STAGE_FC, msg);
    }
}

void CServerStartStageInstance::Start()
{
    Update();
    CServerTriggerInstance::Start();
}

void CServerStartStageInstance::EntryPC(SMessage* msg)
{
    SServerEntryMessage* entry = 0;

    if (msg->Cast(entry))
    {
        Update(entry->dwCharDbNum);
    }
}
