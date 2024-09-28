#include "pch.h"
#include "ServerLayerTrigger.h"
#include "ServerTriggerFinger.h"
#include "ServerTriggerInstance.h"

#include "../Land/GLLandMan.h"

#include "../../RanLogic/TriggerSystem/TriggerScript.h"

#include "../SigmaCore/DebugInclude.h"

using namespace TriggerSystem;
using namespace Sqrat;

CServerLayerTrigger::CServerLayerTrigger()
:	m_layer(0)
{
	
}

void CServerLayerTrigger::Bind(CTriggerScript* pScript)
{
	if (pScript)
	{
		pScript->CreateClass<CServerLayerTrigger>(_T("LayerTrigger"));
		pScript->Func(_T("SetLayer"), &CServerLayerTrigger::SetLayer);
	}

	CTrigger::Bind(pScript);
}

void CServerLayerTrigger::Start(CTriggerInstance* pInstance) const
{
    CServerTriggerInstance* pServerInstance = 0;

    if (pInstance && pInstance->CastInstance(pServerInstance))
    {
        GLLandMan* pLandMan = pServerInstance->GetLandMan();

        if (pLandMan)
        {
            pLandMan->SetMobSchManRegenLayer(m_layer);
        }

        // 종료 처리
        pInstance->Exit();
    }

    CTrigger::Start(pInstance);
}
