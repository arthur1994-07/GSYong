#include "pch.h"
#include "ClientRebirthMessage.h"
#include "ClientTriggerInstance.h"
#include "ClientTriggerDecleare.h"

#include "../../RanLogic/TriggerSystem/TriggerInstance.h"

#include "../../SigmaCore/DebugInclude.h"

using namespace TriggerSystem;

void SClientRebirthMessage::Command(CTriggerInstance* pInstance)
{
    CClientTriggerInstance* pClient = 0;

    if (pInstance && pInstance->CastInstance(pClient))
    {
        pClient->Rebirth(this);
    }
}
