#include "pch.h"
#include "ServerRetryTrigger.h"
#include "ServerRetryInstance.h"

#include "../../RanLogic/TriggerSystem/TriggerScript.h"

#include "../SigmaCore/DebugInclude.h"

using namespace TriggerSystem;
using namespace Sqrat;

void CServerRetryTrigger::Bind(CTriggerScript* pScript)
{
	if (pScript)
	{
		pScript->CreateClass<CServerRetryTrigger>(_T("RetryTrigger"));
	}

	CRetryTrigger::Bind(pScript);
}

CTriggerInstance* CServerRetryTrigger::CreateInstance() const
{
    return new CServerRetryInstance;
}
