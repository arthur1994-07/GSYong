#include "pch.h"
#include "ClientRetryTrigger.h"
#include "ClientTriggerFinger.h"
#include "ClientRetryInstance.h"

#include "../GLGaeaClient.h"
#include "../Char/GLCharacter.h"

#include "../../RanLogic/TriggerSystem/TriggerScript.h"
#include "../../RanLogic/TriggerSystem/TriggerInstance.h"

#include "../../SigmaCore/DebugInclude.h"

using namespace TriggerSystem;
using namespace Sqrat;

void CClientRetryTrigger::Bind(CTriggerScript* pScript)
{
	if (pScript)
	{
		pScript->CreateClass<CClientRetryTrigger>(_T("RetryTrigger"));
	}

	CRetryTrigger::Bind(pScript);
}

CTriggerInstance* CClientRetryTrigger::CreateInstance() const
{
    return new CClientRetryInstance;
}
