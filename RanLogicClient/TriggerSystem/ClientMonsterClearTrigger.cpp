#include "pch.h"
#include "ClientMonsterClearTrigger.h"
#include "ClientMonsterClearInstance.h"

#include "../../RanLogic/TriggerSystem/TriggerScript.h"

#include "../SigmaCore/DebugInclude.h"

using namespace TriggerSystem;
using namespace Sqrat;

CClientMonsterClearTrigger::CClientMonsterClearTrigger()
{
	
}

void CClientMonsterClearTrigger::Bind(CTriggerScript* pScript)
{
	if (pScript)
	{
		pScript->CreateClass<CClientMonsterClearTrigger>(_T("MonsterClearTrigger"));
	}

	CTrigger::Bind(pScript);
}

CTriggerInstance* CClientMonsterClearTrigger::CreateInstance() const
{
    return new CClientMonsterClearInstance;
}
