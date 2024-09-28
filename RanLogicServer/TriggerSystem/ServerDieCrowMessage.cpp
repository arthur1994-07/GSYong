#include "pch.h"
#include "ServerDieCrowMessage.h"
#include "ServerTriggerInstance.h"

#include "../../RanLogic/TriggerSystem/TriggerInstance.h"

#include "../SigmaCore/DebugInclude.h"

using namespace TriggerSystem;

void SServerDieCrowMessage::Command(CTriggerInstance* pInstance)
{
    CServerTriggerInstance* pServerInstance = 0;

    if (pInstance && pInstance->CastInstance(pServerInstance))
    {
        pServerInstance->DieCrow(this);
    }
}
