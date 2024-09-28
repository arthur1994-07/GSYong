#include "pch.h"
#include "ServerPositioningClearTrigger.h"
#include "ServerPositioningClearInstance.h"

#include "../../RanLogic/TriggerSystem/TriggerScript.h"

#include "../SigmaCore/DebugInclude.h"

using namespace TriggerSystem;
using namespace Sqrat;

CServerPositioningClearTrigger::CServerPositioningClearTrigger()
:	m_min(0, 0)
,	m_max(0, 0)
,   m_count(0)
{
	
}

void CServerPositioningClearTrigger::Bind(CTriggerScript* pScript)
{
	if (pScript)
	{
		pScript->CreateClass<CServerPositioningClearTrigger>(_T("PositioningClearTrigger"));
		pScript->Func(_T("SetMin"), &CServerPositioningClearTrigger::SetMin);
		pScript->Func(_T("SetMax"), &CServerPositioningClearTrigger::SetMax);
        pScript->Func(_T("SetCount"), &CServerPositioningClearTrigger::SetCount);
	}

	CTrigger::Bind(pScript);
}

CTriggerInstance* CServerPositioningClearTrigger::CreateInstance() const
{
    return new CServerPositioningClearInstance;
}
