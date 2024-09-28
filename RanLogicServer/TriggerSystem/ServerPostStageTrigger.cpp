#include "pch.h"
#include "ServerPostStageTrigger.h"
#include "ServerPostStageInstance.h"

#include "../../RanLogic/TriggerSystem/TriggerScript.h"

#include "../SigmaCore/DebugInclude.h"

using namespace TriggerSystem;
using namespace Sqrat;

void CServerPostStageTrigger::Bind(CTriggerScript* pScript)
{
	if (pScript)
	{
		pScript->CreateClass<CServerPostStageTrigger>(_T("PostStageTrigger"));
        pScript->Func(_T("SetTime"), &CServerPostStageTrigger::SetTime);
	}

	CTrigger::Bind(pScript);
}

CTriggerInstance* CServerPostStageTrigger::CreateInstance() const
{
    return new CServerPostStageInstance;
}
