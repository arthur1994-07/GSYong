#include "pch.h"
#include "ClientTriggerSystem.h"
#include "ClientTriggerScript.h"
#include "ClientTriggerScriptList.h"
#include "ClientTriggerFinger.h"

#include "../../RanLogic/GLogicDataMan.h"
#include "../../enginelib/Common/SUBPATH.h"
#include "../../sigmacore/util/systeminfo.h"

#include  <io.h>

#include "../../SigmaCore/DebugInclude.h"

using namespace TriggerSystem;

// TAG_InitClientTriggerSystem_1
bool TriggerSystem::InitClientTriggerSystem()
{
	SUBPATH* pPath = GLogicData::GetInstance().GetPath();

	if (pPath)
	{
		CClientTriggerScriptList kTriggerList;

		TSTRING str = sc::getAppPath() + pPath->GLogicScriptsPath() + GLogicData::GetInstance().GetTriggerListFileName();

		kTriggerList.LoadScript(str.c_str());

		return true;
	}

	return false;
}

void TriggerSystem::DestroyClientTriggerSystem()
{
	TriggerSystem::DestroyTriggerSystem();
}

// TAG_InitClientTriggerSystem_5
void TriggerSystem::CreateClientScript(const TSTRING& file)
{
	SUBPATH* pPath = GLogicData::GetInstance().GetPath();

	if (pPath && file.empty() == false)
	{
		TSTRING str = sc::getAppPath() + pPath->GLogicScriptsPath() + file;
		CreateScript<CClientTriggerScript>(str.c_str());
	}
}

TriggerSystem::SpTriggerFinger TriggerSystem::CreateClientFinger(GLGaeaClient* pGaeaClient, int nStage, int nLevel)
{	
    CClientTriggerFinger* pFinger = new CClientTriggerFinger;

    if (pFinger)
    {
        pFinger->SetGaeaClient(pGaeaClient);
        pFinger->SetStage(nStage);
        pFinger->SetLevel(nLevel);
    }

    return SpTriggerFinger(pFinger);
}
