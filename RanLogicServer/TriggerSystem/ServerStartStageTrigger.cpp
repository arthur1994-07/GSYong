#include "pch.h"
#include "ServerStartStageTrigger.h"
#include "ServerStartStageInstance.h"

#include "../../RanLogic/TriggerSystem/TriggerInstance.h"
#include "../../RanLogic/TriggerSystem/TriggerScript.h"

#include "../SigmaCore/DebugInclude.h"

using namespace TriggerSystem;
using namespace Sqrat;

void CServerStartStageTrigger::Bind(CTriggerScript* pScript)
{
	if (pScript)
	{
		pScript->CreateClass<CServerStartStageTrigger>(_T("StartStageTrigger"));
	}

	CTrigger::Bind(pScript);
}

CTriggerInstance* CServerStartStageTrigger::CreateInstance() const
{
    return new CServerStartStageInstance;
}
