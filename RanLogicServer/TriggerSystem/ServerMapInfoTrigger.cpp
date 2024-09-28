#include "pch.h"
#include "ServerMapInfoTrigger.h"

#include "../../RanLogic/TriggerSystem/TriggerScript.h"

#include "../SigmaCore/DebugInclude.h"

using namespace TriggerSystem;
using namespace Sqrat;

void CServerMapInfoTrigger::Bind(CTriggerScript* pScript)
{
	if (pScript)
	{
		pScript->CreateClass<CServerMapInfoTrigger>(_T("MapInfoTrigger"));
	}

	CMapInfoTrigger::Bind(pScript);
}
