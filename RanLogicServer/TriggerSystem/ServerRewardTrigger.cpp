#include "pch.h"
#include "ServerRewardTrigger.h"
#include "ServerRewardInstance.h"

#include "../../RanLogic/TriggerSystem/TriggerScript.h"

#include "../SigmaCore/DebugInclude.h"

using namespace TriggerSystem;
using namespace Sqrat;

CServerRewardTrigger::CServerRewardTrigger()
:	m_waittime(10.f)
,	m_money(0)
,	m_mvp(0.f)
,	m_exp(0.f)
,	m_stage(0)
{

}

void CServerRewardTrigger::Bind(CTriggerScript* pScript)
{
	if (pScript)
	{
		pScript->CreateClass<CServerRewardTrigger>(_T("RewardTrigger"));
        pScript->Func(_T("SetWaitTime"), &CServerRewardTrigger::SetWaitTime);
		pScript->Func(_T("SetMoney"), &CServerRewardTrigger::SetMoney);
		pScript->Func(_T("SetMVPIncrease"), &CServerRewardTrigger::SetMVPIncrease);
		pScript->Func(_T("SetEXPIncrease"), &CServerRewardTrigger::SetEXPIncrease);
		pScript->Func(_T("SetMinStage"), &CServerRewardTrigger::SetMinStage);
	}

	CTrigger::Bind(pScript);
}

CTriggerInstance* CServerRewardTrigger::CreateInstance() const
{
    return new CServerRewardInstance;
}

const LONGLONG CServerRewardTrigger::RewardMoney(const WORD wLevel, const int nStage) const 
{
	if (m_stage > nStage)
	{
		return 0;
	}
	else
	{
		return ( ((m_money * wLevel) * m_mvp) * (nStage * nStage) / 100 );
	}
}

const int CServerRewardTrigger::RewardExp(const WORD wLevel, const int nStage, const WORD wMax, const LONGLONG needexp) const 
{
	if (m_stage > nStage)
	{
		return 0;
	}
	else
	{
		return needexp * (wMax - wLevel) / ( wMax * (1 + (wLevel / 10)) ) * (nStage / m_exp);
	}
}

const int CServerRewardTrigger::RewardMVP(const int nStage, const DWORD dwDamage, const DWORD dwHeal) const 
{
	if (m_stage > nStage)
	{
		return 0;
	}
	else
	{
		return (dwDamage * 100 + dwHeal * 10) + dwDamage;
	}
}
