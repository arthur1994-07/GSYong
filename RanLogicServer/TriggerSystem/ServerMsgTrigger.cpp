#include "pch.h"
#include "ServerMsgTrigger.h"
#include "ServerMsgInstance.h"

#include "../../RanLogic/TriggerSystem/TriggerInstance.h"
#include "../../RanLogic/TriggerSystem/TriggerScript.h"

#include "../SigmaCore/DebugInclude.h"

using namespace TriggerSystem;
using namespace Sqrat;

void CServerMsgTrigger::Bind(CTriggerScript* pScript)
{
	if (pScript)
	{
		pScript->CreateClass<CServerMsgTrigger>(_T("MsgTrigger"));
        pScript->Func(_T("SetMessage"), &CServerMsgTrigger::SetMessage);
	}

	CTrigger::Bind(pScript);
}

CTriggerInstance* CServerMsgTrigger::CreateInstance() const
{
    return new CServerMsgInstance;
}
