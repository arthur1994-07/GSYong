#include "pch.h"
#include "ServerDamageMessage.h"
#include "ServerTriggerInstance.h"

#include "../../RanLogic/TriggerSystem/TriggerInstance.h"

#include "../SigmaCore/DebugInclude.h"

using namespace TriggerSystem;

void SServerDamageMessage::Command(CTriggerInstance* pInstance)
{
    CServerTriggerInstance* pServerInstance = 0;

    if (pInstance && pInstance->CastInstance(pServerInstance))
    {
        pServerInstance->Damage(this);
    }
}
