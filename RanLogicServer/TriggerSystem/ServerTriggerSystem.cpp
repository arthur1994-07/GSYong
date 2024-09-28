#include "pch.h"
#include "ServerTriggerSystem.h"
#include "ServerTriggerScript.h"
#include "ServerTriggerScriptList.h"
#include "ServerTriggerFinger.h"

#include "../../RanLogic/GLogicDataMan.h"
#include "../../enginelib/Common/SUBPATH.h"
#include "../../sigmacore/util/systeminfo.h"

#include  <io.h>

#include "../SigmaCore/DebugInclude.h"

using namespace TriggerSystem;

bool TriggerSystem::InitServerTriggerSystem()
{
	SUBPATH* pPath = GLogicData::GetInstance().GetPath();

	if (pPath)
	{
		CServerTriggerScriptList kTriggerList;

		TSTRING str = sc::getAppPath() + pPath->GLogicServerScriptsPath() + GLogicData::GetInstance().GetTriggerListFileName();

		kTriggerList.LoadScript(str.c_str());

		return true;
	}

	return false;
}

void TriggerSystem::DestroyServerTriggerSystem()
{
	TriggerSystem::DestroyTriggerSystem();
}

void TriggerSystem::CreateServerScript(const TSTRING& file)
{
	SUBPATH* pPath = GLogicData::GetInstance().GetPath();

	if (pPath && file.empty() == false)
	{
		TSTRING str = sc::getAppPath() + pPath->GLogicServerScriptsPath() + file;
		CreateScript<CServerTriggerScript>(str.c_str());
	}
}

TriggerSystem::SpTriggerFinger TriggerSystem::CreateServerFinger(GLLandMan* pLandMan, GLGaeaServer* pGLGaeaServer, int nStage, int nLevel, const std::string& strGuid)
{
	CServerTriggerFinger* pFinger = new CServerTriggerFinger;

    if (pFinger)
    {
        pFinger->SetLandMan(pLandMan);
        pFinger->SetGaeaServer(pGLGaeaServer);
        pFinger->SetStage(nStage);
        pFinger->SetLevel(nLevel);
        pFinger->SetGuid( strGuid );
    }

    return SpTriggerFinger(pFinger);
}
