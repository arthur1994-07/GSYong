#include "pch.h"
#include "ServerMonsterClearTrigger.h"
#include "ServerMonsterClearInstance.h"

#include "../../RanLogic/TriggerSystem/TriggerScript.h"

#include "../SigmaCore/DebugInclude.h"

using namespace TriggerSystem;
using namespace Sqrat;

CServerMonsterClearTrigger::CServerMonsterClearTrigger()
:	m_count(0)
{
	
}

void CServerMonsterClearTrigger::Bind(CTriggerScript* pScript)
{
	if (pScript)
	{
		pScript->CreateClass<CServerMonsterClearTrigger>(_T("MonsterClearTrigger"));
		pScript->Func(_T("SetCount"), &CServerMonsterClearTrigger::SetCount);
	}

	CTrigger::Bind(pScript);
}

CTriggerInstance* CServerMonsterClearTrigger::CreateInstance() const
{
    return new CServerMonsterClearInstance;
}
