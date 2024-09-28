#include "pch.h"
#include "LevelTrigger.h"
#include "TriggerInstance.h"
#include "TriggerScript.h"

using namespace TriggerSystem;
using namespace Sqrat;

CLevelTrigger::CLevelTrigger()
:	m_level(0)
,	m_minlevel(0)
,	m_maxlevel(0)
{

}

// TAG_InitClientTriggerSystem_7a_3
void CLevelTrigger::Bind(CTriggerScript* pScript)
{
	if (pScript)
	{
		pScript->Func(_T("SetLevel"), &CLevelTrigger::SetLevel);
		pScript->Func(_T("SetUserLevel"), &CLevelTrigger::SetUserLevel);
		pScript->Func(_T("SetFile"), &CLevelTrigger::SetFile);
		pScript->Func(_T("Load"), &CLevelTrigger::Load);
	}

	CTrigger::Bind(pScript);
}
