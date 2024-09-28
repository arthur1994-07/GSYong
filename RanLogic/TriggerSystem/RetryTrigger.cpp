#include "pch.h"
#include "RetryTrigger.h"
#include "TriggerInstance.h"
#include "TriggerScript.h"

using namespace TriggerSystem;
using namespace Sqrat;

CRetryTrigger::CRetryTrigger()
:	m_time(0)
,	m_money(0)
,	m_contrl(0.f)
{
	
}

void CRetryTrigger::Bind(CTriggerScript* pScript)
{
	if (pScript)
	{		
		pScript->Func(_T("SetTime"), &CRetryTrigger::SetTime);
		pScript->Func(_T("SetMoney"), &CRetryTrigger::SetMoney);
		pScript->Func(_T("SetControl"), &CRetryTrigger::SetControl);
	}

	CTrigger::Bind(pScript);
}

const LONGLONG CRetryTrigger::RetryMoney(const WORD wLevel, const int nStage) const
{
	if (m_contrl > 0)
	{
		return m_money * (wLevel * nStage / m_contrl);
	}
	else
	{
		return 0;
	}
}
