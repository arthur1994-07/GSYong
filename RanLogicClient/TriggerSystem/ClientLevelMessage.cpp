#include "pch.h"
#include "ClientLevelMessage.h"
#include "ClientTriggerDecleare.h"

#include "../../RanLogic/TriggerSystem/TriggerInstance.h"

#include "../../SigmaCore/DebugInclude.h"

using namespace TriggerSystem;

ClientLevelInfo::ClientLevelInfo()
:	level(0)
,	minlevel(INT_MAX)
,	maxlevel(INT_MIN)
,	possible(false)
{

}

void SClientLevelMessage::Command(CTrigger* pTrigger)
{
    if (pTrigger && pTrigger->GetType() == TRIGGER_TYPE_LEVEL)
    {
        CClientLevelTrigger* pLevelTrigger = dynamic_cast<CClientLevelTrigger*>(pTrigger);

        if (pLevelTrigger)
        {
			int nLevel = pLevelTrigger->GetLevel();
			levelinfo[nLevel].level = nLevel;
			levelinfo[nLevel].minlevel = pLevelTrigger->GetUserMinLevel();
			levelinfo[nLevel].maxlevel = pLevelTrigger->GetUserMaxLevel();
			levelinfo[nLevel].possible = false;
        }
    }
}
