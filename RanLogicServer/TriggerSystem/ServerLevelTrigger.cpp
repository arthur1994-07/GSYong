#include "pch.h"
#include "ServerLevelTrigger.h"
#include "ServerTriggerSystem.h"

#include "../../RanLogic/TriggerSystem/TriggerScript.h"

#include "../SigmaCore/DebugInclude.h"

using namespace TriggerSystem;
using namespace Sqrat;

void CServerLevelTrigger::Bind(CTriggerScript* pScript)
{
	if (pScript)
	{
		pScript->CreateClass<CServerLevelTrigger>(_T("LevelTrigger"));
	}

	CLevelTrigger::Bind(pScript);
}

void CServerLevelTrigger::Load()
{
	TriggerSystem::CreateServerScript(m_file);
}
