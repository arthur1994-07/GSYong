#include "pch.h"
#include "ClientMsgMessage.h"
#include "ClientTriggerInstance.h"
#include "ClientTriggerDecleare.h"

#include "../../RanLogic/TriggerSystem/TriggerInstance.h"

#include "../../SigmaCore/DebugInclude.h"

using namespace TriggerSystem;

void SClientMsgMessage::Command(CTriggerInstance* pInstance)
{
    CClientTriggerInstance* pClient = 0;

    if (pInstance && pInstance->CastInstance(pClient))
    {
        pClient->Message(this);
    }
}
