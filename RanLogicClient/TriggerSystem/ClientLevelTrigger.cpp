#include "pch.h"
#include "ClientLevelTrigger.h"
#include "ClientTriggerSystem.h"

#include "../../RanLogic/TriggerSystem/TriggerScript.h"

#include "../../SigmaCore/DebugInclude.h"

using namespace TriggerSystem;
using namespace Sqrat;

// TAG_InitClientTriggerSystem_7a_2
void CClientLevelTrigger::Bind(CTriggerScript* pScript)
{
	if (pScript)
	{
		pScript->CreateClass<CClientLevelTrigger>(_T("LevelTrigger"));
	}

	CLevelTrigger::Bind(pScript);
}

// TAG_InitClientTriggerSystem_7b_2 - script1.Run()���� ȣ�� �ɰ����� ����Ѵ�.
void CClientLevelTrigger::Load()
{
	TriggerSystem::CreateClientScript(m_file);
}
