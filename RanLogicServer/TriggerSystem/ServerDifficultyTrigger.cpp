#include "pch.h"
#include "ServerDifficultyTrigger.h"
#include "ServerTriggerFinger.h"
#include "ServerTriggerInstance.h"

#include "../../RanLogic/TriggerSystem/TriggerScript.h"
#include "../Land/GLLandMan.h"

#include "../SigmaCore/DebugInclude.h"

using namespace TriggerSystem;
using namespace Sqrat;

CServerDifficultyTrigger::CServerDifficultyTrigger()
:	m_fMultipleHP(1.f)
,	m_fMultipleAttack(1.f)
,	m_fMultipleDefense(1.f)
,   m_fMultipleExp(1.f)
{
	
}

void CServerDifficultyTrigger::Bind(CTriggerScript* pScript)
{
	if (pScript)
	{
		pScript->CreateClass<CServerDifficultyTrigger>(_T("DifficultyTrigger"));
		pScript->Func(_T("SetMultipleHP"), &CServerDifficultyTrigger::SetMultipleHP);
		pScript->Func(_T("SetMultipleAttack"), &CServerDifficultyTrigger::SetMultipleAttack);
		pScript->Func(_T("SetMultipleDefense"), &CServerDifficultyTrigger::SetMultipleDefense);
        pScript->Func(_T("SetMultipleExp"), &CServerDifficultyTrigger::SetMultipleExp);
		pScript->Func(_T("SetMultipleDrop"), &CServerDifficultyTrigger::SetMultipleDrop);
	}

	CTrigger::Bind(pScript);
}

void CServerDifficultyTrigger::Start(CTriggerInstance* pInstance) const
{
    CServerTriggerInstance* pServerInstance = 0;

    if (pInstance && pInstance->CastInstance(pServerInstance))
    {
        GLLandMan* pLandMan = pServerInstance->GetLandMan();

        if (pLandMan)
        {
            pLandMan->SetCrowMultipleHP(m_fMultipleHP);
            pLandMan->SetCrowMultipleAttack(m_fMultipleAttack);
            pLandMan->SetCrowMultipleDefense(m_fMultipleDefense);
            pLandMan->SetCrowMultipleExp(m_fMultipleExp);
			pLandMan->SetCrowMultipleDrop(m_fMultipleDrop);
        }

        // 종료 처리
        pInstance->Exit();
    }

    CTrigger::Start(pInstance);
}
