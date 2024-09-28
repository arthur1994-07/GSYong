#include "pch.h"
#include "ClientMapInfoMessage.h"
#include "ClientTriggerDecleare.h"
#include "ClientMapInfoTrigger.h"

#include "../../SigmaCore/DebugInclude.h"

using namespace TriggerSystem;

SClientTriggerMapInfo::SClientTriggerMapInfo()
:	gateid(0)
,	dailylimit(0)
,	owntime(0)
,	person(0)
,	waitentrance(0)
{

}

void SClientMapInfoMessage::Command(CTrigger* pTrigger)
{
	if (pTrigger && pTrigger->GetType() == TRIGGER_TYPE_MAPINFO)
	{
		CClientMapInfoTrigger* pMapInfoTrigger = dynamic_cast<CClientMapInfoTrigger*>(pTrigger);

		if (pMapInfoTrigger)
		{
			// 맵 정보 추출
			SClientTriggerMapInfo info;
			info.mapid = pMapInfoTrigger->GetMap();
            info.gateid = pMapInfoTrigger->GetGateID();
			info.mapname = pMapInfoTrigger->GetMapName();
			info.mapObject = pMapInfoTrigger->GetObject();
			info.mapinfo = pMapInfoTrigger->GetMapInfo();
			info.mapinfoRn = pMapInfoTrigger->GetMapInfoRn();
			info.mapreward = pMapInfoTrigger->GetMapReward();
			info.dailylimit = pMapInfoTrigger->GetDailyLimit();
			info.owntime = pMapInfoTrigger->GetOwnTime();
			info.person = pMapInfoTrigger->GetPerson();
            info.waitentrance = pMapInfoTrigger->GetWaitEntrance();

			// 레벨 정보 추출
			SClientLevelMessage msg(info.levelinfo);
			pMapInfoTrigger->SendMessage(&msg);

			// 정보 삽입
			mapinfo.push_back(info);
		}
	}
}
