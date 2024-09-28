#include "pch.h"
#include "ServerStageMessage.h"
#include "ServerTriggerInstance.h"

#include "../../RanLogic/TriggerSystem/TriggerInstance.h"

#include "../SigmaCore/DebugInclude.h"

using namespace TriggerSystem;

void SServerMissionCompletedMessage::Command(CTriggerInstance* pInstance)
{
    CServerTriggerInstance* pServerInstance = 0;

    if (pInstance && pInstance->CastInstance(pServerInstance))
    {
        pServerInstance->MissionCompleted(this);
    }
}

void SServerStageClearMessage::Command(CTriggerInstance* pInstance)
{
    CServerTriggerInstance* pServerInstance = 0;

    if (pInstance && pInstance->CastInstance(pServerInstance))
    {
        pServerInstance->StageClear(this);
    }
}

void SServerStageExitMessage::Command(CTriggerInstance* pInstance)
{
	CServerTriggerInstance* pServerInstance = 0;

	if (pInstance && pInstance->CastInstance(pServerInstance))
	{
		pServerInstance->StageExit(this);
	}
}
