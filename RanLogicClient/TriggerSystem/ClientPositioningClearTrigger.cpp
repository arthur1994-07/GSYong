#include "pch.h"
#include "ClientPositioningClearTrigger.h"
#include "ClientPositioningClearInstance.h"

#include "../../RanLogic/TriggerSystem/TriggerScript.h"

#include "../SigmaCore/DebugInclude.h"

using namespace TriggerSystem;
using namespace Sqrat;

CClientPositioningClearTrigger::CClientPositioningClearTrigger()
{
	
}

void CClientPositioningClearTrigger::Bind(CTriggerScript* pScript)
{
	if (pScript)
	{
		pScript->CreateClass<CClientPositioningClearTrigger>(_T("PositioningClearTrigger"));
	}

	CTrigger::Bind(pScript);
}

CTriggerInstance* CClientPositioningClearTrigger::CreateInstance() const
{
    return new CClientPositioningClearInstance;
}
