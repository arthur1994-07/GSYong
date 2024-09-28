#include "pch.h"
#include "ClientStageTrigger.h"
#include "ClientStageInstance.h"

#include "../../RanLogic/TriggerSystem/TriggerScript.h"

#include "../SigmaCore/DebugInclude.h"

using namespace TriggerSystem;
using namespace Sqrat;

CClientStageTrigger::CClientStageTrigger()
:   m_end(0)
,   m_range(0)
{
	
}

void CClientStageTrigger::Bind(CTriggerScript* pScript)
{
	if (pScript)
	{
		pScript->CreateClass<CClientStageTrigger>(_T("StageTrigger"));
        pScript->Func(_T("SetEndStage"), &CClientStageTrigger::SetEndStage);
        pScript->Func(_T("SetRange"), &CClientStageTrigger::SetRange);
	}

	CTrigger::Bind(pScript);
}

CTriggerInstance* CClientStageTrigger::CreateInstance() const
{
    return new CClientStageInstance;    
}
