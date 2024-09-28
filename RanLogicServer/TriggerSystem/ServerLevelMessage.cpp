#include "pch.h"
#include "ServerLevelMessage.h"
#include "ServerTriggerDecleare.h"

#include "../../RanLogic/TriggerSystem/TriggerInstance.h"

#include "../../SigmaCore/DebugInclude.h"

using namespace TriggerSystem;

ServerLevelInfo::ServerLevelInfo()
:	level(0)
,	minlevel(INT_MAX)
,	maxlevel(INT_MIN)
{

}

void SServerLevelMessage::Command(CTrigger* pTrigger)
{
    if (pTrigger && pTrigger->GetType() == TRIGGER_TYPE_LEVEL)
    {
        CServerLevelTrigger* pLevelTrigger = dynamic_cast<CServerLevelTrigger*>(pTrigger);

        if (pLevelTrigger)
        {
			int nLevel = pLevelTrigger->GetLevel();
			levelinfo[nLevel].level = nLevel;
			levelinfo[nLevel].minlevel = pLevelTrigger->GetUserMinLevel();
			levelinfo[nLevel].maxlevel = pLevelTrigger->GetUserMaxLevel();			
        }
    }
}
