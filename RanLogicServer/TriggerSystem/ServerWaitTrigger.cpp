#include "pch.h"
#include "ServerWaitTrigger.h"

#include "../../RanLogic/TriggerSystem/TriggerScript.h"

#include "../SigmaCore/DebugInclude.h"

using namespace TriggerSystem;
using namespace Sqrat;

void CServerWaitTrigger::Bind(CTriggerScript* pScript)
{
	if (pScript)
	{
		pScript->CreateClass<CServerWaitTrigger>(_T("WaitTrigger"));
		pScript->Func(_T("SetTime"), &CServerWaitTrigger::SetTime);
	}

	CTrigger::Bind(pScript);
}
