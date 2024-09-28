#include "pch.h"
#include "ClientMapInfoTrigger.h"

#include "../../RanLogic/TriggerSystem/TriggerScript.h"

#include "../../SigmaCore/DebugInclude.h"

using namespace TriggerSystem;
using namespace Sqrat;

void CClientMapInfoTrigger::Bind(CTriggerScript* pScript)
{
	if (pScript)
	{
		pScript->CreateClass<CClientMapInfoTrigger>(_T("MapInfoTrigger"));
		pScript->Func(_T("SetMapName"), &CClientMapInfoTrigger::SetMapName);
		pScript->Func(_T("SetObject"), &CClientMapInfoTrigger::SetObject);
		pScript->Func(_T("SetMapInfo"), &CClientMapInfoTrigger::SetMapInfo);
		pScript->Func(_T("SetMapInfoRn"), &CClientMapInfoTrigger::SetMapInfoRn);
		pScript->Func(_T("SetMapReward"), &CClientMapInfoTrigger::SetMapReward);
	}

	CMapInfoTrigger::Bind(pScript);
}
