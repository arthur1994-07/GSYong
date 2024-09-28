#include "pch.h"
#include "ServerPortalCloseTrigger.h"
#include "ServerTriggerFinger.h"
#include "ServerTriggerInstance.h"
#include "ServerPortalCloseInstance.h"

#include "../../RanLogic/TriggerSystem/TriggerScript.h"

#include "../Land/GLLandMan.h"

#include "../SigmaCore/DebugInclude.h"

using namespace TriggerSystem;
using namespace Sqrat;

CServerPortalCloseTrigger::CServerPortalCloseTrigger()
:   m_toMapID(false)
,   m_toGateID(0)
{

}

void CServerPortalCloseTrigger::Bind(CTriggerScript* pScript)
{
	if (pScript)
	{
		pScript->CreateClass<CServerPortalCloseTrigger>(_T("PortalCloseTrigger"));        
	}

	CPortalCloseTrigger::Bind(pScript);
}

CTriggerInstance* CServerPortalCloseTrigger::CreateInstance() const
{
    return new CServerPortalCloseInstance;
}
