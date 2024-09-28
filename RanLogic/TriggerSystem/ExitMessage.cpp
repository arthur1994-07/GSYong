#include "pch.h"
#include "ExitMessage.h"
#include "TriggerInstance.h"
#include "Trigger.h"

using namespace TriggerSystem;

void SExitMessage::Command(CTriggerInstance* pInstance)
{
    if (pInstance)
    {
        const CTrigger* pTrigger = pInstance->GetTrigger();

        if (pTrigger)
        {
            if (pTrigger->GetAttach() == trigger)
            {
                pInstance->Exit();
            }
        }
    }
}
