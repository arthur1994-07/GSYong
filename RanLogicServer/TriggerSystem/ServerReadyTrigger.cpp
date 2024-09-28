#include "pch.h"
#include "ServerReadyTrigger.h"
#include "ServerReadyInstance.h"

#include "../../RanLogic/TriggerSystem/TriggerScript.h"

#include "../SigmaCore/DebugInclude.h"

using namespace TriggerSystem;
using namespace Sqrat;

void CServerReadyTrigger::Bind(CTriggerScript* pScript)
{
	if (pScript)
	{
		pScript->CreateClass<CServerReadyTrigger>(_T("ReadyTrigger"));        
        pScript->Func(_T("SetCount"), &CServerReadyTrigger::SetCount);
	}

	CTrigger::Bind(pScript);
}

CTriggerInstance* CServerReadyTrigger::CreateInstance() const
{
    return new CServerReadyInstance;
}
