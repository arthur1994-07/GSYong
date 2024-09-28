#include "pch.h"
#include "ServerMapInfoMessage.h"
#include "ServerTriggerDecleare.h"
#include "ServerMapInfoTrigger.h"
#include "ServerTriggerFinger.h"

#include "../SigmaCore/DebugInclude.h"

using namespace TriggerSystem;

SServerTriggerMapInfo::SServerTriggerMapInfo()
:	gateid(0)
,	dailylimit(0)
,	owntime(0)
,	person(0)
,	waitentrance(0)
{

}

static void CopyMapInfo(SServerTriggerMapInfo& mapinfo, const CServerMapInfoTrigger& trigger)
{
    mapinfo.mapid = trigger.GetMap();
    mapinfo.gateid = trigger.GetGateID();
    mapinfo.dailylimit = trigger.GetDailyLimit();
    mapinfo.owntime = trigger.GetOwnTime();
    mapinfo.person = trigger.GetPerson();
    mapinfo.waitentrance = trigger.GetWaitEntrance();
}

void SServerAllMapInfoMessage::Command(CTrigger* pTrigger)
{
	if (pTrigger && pTrigger->GetType() == TRIGGER_TYPE_MAPINFO)
	{
		CServerMapInfoTrigger* pMapInfoTrigger = dynamic_cast<CServerMapInfoTrigger*>(pTrigger);

		if (pMapInfoTrigger)
		{
			// 맵 정보 추출
            SNATIVEID mid = pMapInfoTrigger->GetMap();
            CopyMapInfo(mapinfo[mid], *pMapInfoTrigger);

			// 레벨 정보 추출
			SServerLevelMessage msg(mapinfo[mid].levelinfo);
			pMapInfoTrigger->SendMessage(&msg);
		}
	}
}

void SServerMapInfoMessage::Command(CTrigger* pTrigger)
{
    if (pTrigger && pTrigger->GetType() == TRIGGER_TYPE_MAPINFO)
    {
        CServerMapInfoTrigger* pMapInfoTrigger = dynamic_cast<CServerMapInfoTrigger*>(pTrigger);

        if (pMapInfoTrigger && pMapInfoTrigger->GetMap().wMainID == mapinfo.mapid.wMainID)
        {
			// 맵 정보 추출
            CopyMapInfo(mapinfo, *pMapInfoTrigger);

			// 레벨 정보 추출
			SServerLevelMessage msg(mapinfo.levelinfo);
			pMapInfoTrigger->SendMessage(&msg);
        }
    }
}
