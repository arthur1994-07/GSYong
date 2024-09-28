#include "pch.h"
#include "MapInfoTrigger.h"
#include "TriggerInstance.h"
#include "TriggerScript.h"

using namespace TriggerSystem;
using namespace Sqrat;

CMapInfoTrigger::CMapInfoTrigger()
:	m_dailylimit(0)
,   m_gateid(0)
,	m_owntime(0)
,	m_person(0)
,   m_waitentrance(0)
{

}

void CMapInfoTrigger::Bind(CTriggerScript* pScript)
{
	if (pScript)
	{
		pScript->Func(_T("SetMap"), &CMapInfoTrigger::SetMap);
        pScript->Func(_T("SetGateID"), &CMapInfoTrigger::SetGateID);
		pScript->Func(_T("SetDailyLimit"), &CMapInfoTrigger::SetDailyLimit);
		pScript->Func(_T("SetOwnTime"), &CMapInfoTrigger::SetOwnTime);
		pScript->Func(_T("SetPerson"), &CMapInfoTrigger::SetPerson);
        pScript->Func(_T("SetWaitEntrance"), &CMapInfoTrigger::SetWaitEntrance);
	}

	CTrigger::Bind(pScript);
}
