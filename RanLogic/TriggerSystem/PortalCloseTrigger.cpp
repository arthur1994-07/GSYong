#include "pch.h"
#include "PortalCloseTrigger.h"
#include "TriggerInstance.h"
#include "TriggerScript.h"

using namespace TriggerSystem;
using namespace Sqrat;

CPortalCloseTrigger::CPortalCloseTrigger()
:	m_fromGateID(0)
{

}

void CPortalCloseTrigger::Bind(CTriggerScript* pScript)
{
	if (pScript)
	{
		pScript->Func(_T("SetFromGateID"), &CPortalCloseTrigger::SetFromGateID);
	}

	CTrigger::Bind(pScript);
}
