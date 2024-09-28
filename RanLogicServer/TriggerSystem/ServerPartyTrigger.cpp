#include "pch.h"
#include "ServerPartyTrigger.h"
#include "ServerPartyInstance.h"

#include "../../RanLogic/TriggerSystem/TriggerInstance.h"
#include "../../RanLogic/TriggerSystem/TriggerScript.h"

#include "../SigmaCore/DebugInclude.h"

using namespace TriggerSystem;
using namespace Sqrat;

CServerPartyTrigger::CServerPartyTrigger()
:	m_bAutoOut(false)
{

}

void CServerPartyTrigger::Bind(CTriggerScript* pScript)
{
	if (pScript)
	{
		pScript->CreateClass<CServerPartyTrigger>(_T("PartyTrigger"));
		pScript->Func(_T("SetAutoOut"), &CServerPartyTrigger::SetAutoOut);
	}

	CTrigger::Bind(pScript);
}

CTriggerInstance* CServerPartyTrigger::CreateInstance() const
{
    return new CServerPartyInstance;
}
