#include "pch.h"
#include "ClientPortalCloseTrigger.h"
#include "ClientTriggerInstance.h"

#include "../GLGaeaClient.h"
#include "../Land/GLLandManClient.h"

#include "../../RanLogic/TriggerSystem/TriggerScript.h"

#include "../../SigmaCore/DebugInclude.h"

using namespace TriggerSystem;
using namespace Sqrat;

void CClientPortalCloseTrigger::Bind(CTriggerScript* pScript)
{
	if (pScript)
	{
		pScript->CreateClass<CClientPortalCloseTrigger>(_T("PortalCloseTrigger"));
	}

	CPortalCloseTrigger::Bind(pScript);
}

// TAG_PVEClientStartStage_4b_4
void CClientPortalCloseTrigger::Start(CTriggerInstance* pInstance) const
{
	CClientTriggerInstance* pClientInstance = 0;

	if (pInstance && pInstance->CastInstance(pClientInstance))
	{
		GLGaeaClient* pGaeaClient = pClientInstance->GetGaeaClient();

		if (pGaeaClient)
		{
			GLLandManClient* pLandMan = pGaeaClient->GetActiveMap();

			if (pLandMan)
			{
				DxLandGateMan* pGateMan = pLandMan->GetLandGateMan();

				if (pGateMan)
				{
					PDXLANDGATE pGate = pGateMan->FindLandGate(m_fromGateID);

					if (pGate)
					{
						pLandMan->SetLandEffect(m_fromGateID, false);

						pGate->SetEvent(false);
						pGate->ResetToGateMapID();
					}
				}
			}
		}

		// 종료 처리
		pInstance->Exit();
	}

	CTrigger::Start(pInstance);
}
