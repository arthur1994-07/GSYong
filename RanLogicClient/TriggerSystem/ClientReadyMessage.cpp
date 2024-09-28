#include "pch.h"
#include "ClientReadyMessage.h"
#include "ClientTriggerInstance.h"
#include "ClientTriggerDecleare.h"

#include "../../RanLogic/TriggerSystem/TriggerInstance.h"

#include "../../SigmaCore/DebugInclude.h"

using namespace TriggerSystem;

void SClientReadyMessage::Command(CTriggerInstance* pInstance)
{
    CClientTriggerInstance* pClient = 0;

    if (pInstance && pInstance->CastInstance(pClient))
    {
        pClient->Ready(this);
    }
}
