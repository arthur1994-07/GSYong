#include "pch.h"
#include "ClientReadyTrigger.h"
#include "ClientReadyInstance.h"

#include "../../RanLogic/TriggerSystem/TriggerScript.h"

#include "../../SigmaCore/DebugInclude.h"

using namespace TriggerSystem;
using namespace Sqrat;

void CClientReadyTrigger::Bind(CTriggerScript* pScript)
{
	if (pScript)
	{
		pScript->CreateClass<CClientReadyTrigger>(_T("ReadyTrigger"));
	}

	CTrigger::Bind(pScript);
}

CTriggerInstance* CClientReadyTrigger::CreateInstance() const
{
    return new CClientReadyInstance;
}
