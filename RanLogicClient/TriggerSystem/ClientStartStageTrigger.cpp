#include "pch.h"
#include "ClientStartStageTrigger.h"
#include "ClientTriggerFinger.h"
#include "ClientStartStageInstance.h"

#include "../GLGaeaClient.h"

#include "../../RanLogic/TriggerSystem/TriggerScript.h"
#include "../../RanLogic/TriggerSystem/TriggerInstance.h"

#include "../../SigmaCore/DebugInclude.h"

using namespace TriggerSystem;
using namespace Sqrat;

CClientStartStageTrigger::CClientStartStageTrigger()
{
	
}

void CClientStartStageTrigger::Bind(CTriggerScript* pScript)
{
	if (pScript)
	{
		pScript->CreateClass<CClientStartStageTrigger>(_T("StartStageTrigger"));		
	}

	CTrigger::Bind(pScript);
}

CTriggerInstance* CClientStartStageTrigger::CreateInstance() const
{
    return new CClientStartStageInstance;
}
