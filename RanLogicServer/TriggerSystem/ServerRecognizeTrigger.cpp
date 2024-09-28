#include "pch.h"
#include "ServerRecognizeTrigger.h"
#include "ServerRecognizeInstance.h"

#include "../../RanLogic/TriggerSystem/TriggerScript.h"

#include "../SigmaCore/DebugInclude.h"

using namespace TriggerSystem;
using namespace Sqrat;

void CServerRecognizeTrigger::Bind(CTriggerScript* pScript)
{
	if (pScript)
	{
		pScript->CreateClass<CServerRecognizeTrigger>(_T("RecognizeTrigger"));
		pScript->Func(_T("SetRecognizeTime"), &CServerRecognizeTrigger::SetRecognizeTime);
	}

	CTrigger::Bind(pScript);
}

CTriggerInstance* CServerRecognizeTrigger::CreateInstance() const
{
	return new CServerRecognizeInstance;
}
