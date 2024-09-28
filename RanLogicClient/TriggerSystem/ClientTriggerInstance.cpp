#include "pch.h"
#include "ClientTriggerInstance.h"
#include "ClientTriggerFinger.h"

#include "../SigmaCore/DebugInclude.h"

using namespace TriggerSystem;

GLGaeaClient* CClientTriggerInstance::GetGaeaClient()
{
    CClientTriggerFinger* pFinger = 0;

    if (CastFinger(pFinger))
    {
        return pFinger->GetGaeaClient();
    }

    return 0;
}
