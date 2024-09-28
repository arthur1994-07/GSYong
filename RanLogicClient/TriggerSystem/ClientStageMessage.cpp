#include "pch.h"
#include "ClientStageMessage.h"
#include "ClientTriggerInstance.h"
#include "ClientTriggerDecleare.h"

#include "../../RanLogic/TriggerSystem/TriggerInstance.h"

#include "../SigmaCore/DebugInclude.h"

using namespace TriggerSystem;

void SClientStageMessage::Command(CTriggerInstance* pInstance)
{
    CClientTriggerInstance* pClient = 0;

    if (pInstance && pInstance->CastInstance(pClient))
    {
        pClient->Stage(this);
    }
}
