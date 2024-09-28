#include "pch.h"
#include "ClientMsgTrigger.h"
#include "ClientTriggerFinger.h"
#include "ClientMsgInstance.h"

#include "../GLGaeaClient.h"

#include "../../RanLogic/TriggerSystem/TriggerInstance.h"
#include "../../RanLogic/TriggerSystem/TriggerScript.h"

#include "../../SigmaCore/DebugInclude.h"

using namespace TriggerSystem;
using namespace Sqrat;

void CClientMsgTrigger::Bind(CTriggerScript* pScript)
{
	if (pScript)
	{
		pScript->CreateClass<CClientMsgTrigger>(_T("MsgTrigger"));
	}

	CTrigger::Bind(pScript);
}

CTriggerInstance* CClientMsgTrigger::CreateInstance() const
{
    return new CClientMsgInstance;
}
