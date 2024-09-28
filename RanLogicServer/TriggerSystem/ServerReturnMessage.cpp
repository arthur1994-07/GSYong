#include "pch.h"
#include "ServerReturnMessage.h"
#include "ServerTriggerInstance.h"

#include "../../RanLogic/TriggerSystem/TriggerInstance.h"

#include "../SigmaCore/DebugInclude.h"

using namespace TriggerSystem;

void SServerReturnMessage::Command(CTriggerInstance* pInstance)
{    
    CServerTriggerInstance* pServerInstance = 0;

    if (pInstance && pInstance->CastInstance(pServerInstance))
    {
        pServerInstance->ReturnPC(this);
    }
}

void SServerReturnAllMessage::Command(CTriggerInstance* pInstance)
{
	CServerTriggerInstance* pServerInstance = 0;

	if (pInstance && pInstance->CastInstance(pServerInstance))
	{
		pServerInstance->ReturnAllPC(this);
	}
}
