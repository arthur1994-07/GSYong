#include "pch.h"
#include "ServerMonsterClearTrigger.h"
#include "ServerStageInstance.h"

#include "../../RanLogic/TriggerSystem/TriggerScript.h"

#include "../SigmaCore/DebugInclude.h"

using namespace TriggerSystem;
using namespace Sqrat;

CServerStageTrigger::CServerStageTrigger()
:	m_mission(0)
,   m_last(false)
,   m_delay(0.f)
{
	
}

void CServerStageTrigger::Bind(CTriggerScript* pScript)
{
	if (pScript)
	{
		pScript->CreateClass<CServerStageTrigger>(_T("StageTrigger"));		
        pScript->Func(_T("SetMission"), &CServerStageTrigger::SetMission);
        pScript->Func(_T("SetLastStage"), &CServerStageTrigger::SetLastStage);
        pScript->Func(_T("SetDelayTime"), &CServerStageTrigger::SetDelayTime);
	}

	CTrigger::Bind(pScript);
}

CTriggerInstance* CServerStageTrigger::CreateInstance() const 
{
    return new CServerStageInstance;
}
