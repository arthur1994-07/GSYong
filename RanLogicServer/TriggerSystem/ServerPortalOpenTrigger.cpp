#include "pch.h"
#include "ServerPortalOpenTrigger.h"
#include "ServerTriggerFinger.h"
#include "ServerPortalOpenInstance.h"

#include "../Land/GLLandMan.h"
#include "../../RanLogic/Msg/PVEMsg.h"
#include "../../RanLogic/TriggerSystem/TriggerScript.h"

#include "../SigmaCore/DebugInclude.h"

using namespace TriggerSystem;
using namespace Sqrat;

CServerPortalOpenTrigger::CServerPortalOpenTrigger()
:	m_fromGateID(0)
,   m_toMapID(false)
,   m_toGateID(0)
{
	
}

void CServerPortalOpenTrigger::Bind(CTriggerScript* pScript)
{
	if (pScript)
	{
		pScript->CreateClass<CServerPortalOpenTrigger>(_T("PortalOpenTrigger"));
		pScript->Func(_T("SetFromGateID"), &CServerPortalOpenTrigger::SetFromGateID);
        pScript->Func(_T("SetToMapID"), &CServerPortalOpenTrigger::SetToMapID);
        pScript->Func(_T("SetToGateID"), &CServerPortalOpenTrigger::SetToGateID);
	}

	CTrigger::Bind(pScript);
}

CTriggerInstance* CServerPortalOpenTrigger::CreateInstance() const
{
    return new CServerPortalOpenInstance;
}
