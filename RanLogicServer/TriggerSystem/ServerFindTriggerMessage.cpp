#include "pch.h"
#include "ServerFindTriggerMessage.h"
#include "ServerTriggerDecleare.h"

#include "../../RanLogic/TriggerSystem/TriggerSystem.h"
#include "../../RanLogic/TriggerSystem/TriggerInstance.h"

#include "../SigmaCore/DebugInclude.h"

using namespace TriggerSystem;

/**	
	스테이지 트리거를 찾는데 쓰이는 서브 메시지
    서브 탐색을 위해 사용 된다.
    내부적으로 사용되므로 외부에서는 사용할 수 없다.
	
 */
struct SServerSubFindStageTriggerMessage
	: public SSystemMessage
{
	SServerSubFindStageTriggerMessage() : level(0), stage(0), trigger(0) {}

	void Command(CTrigger* pTrigger);
    
    int         level;      //@ 찾을 맵 레벨
    int         stage;      //@ 찾을 스테이지
    CTrigger*   trigger;    //@ 찾은 트리거
};

void SServerFindStageTriggerMessage::Command(CTrigger* pTrigger)
{
    if (pTrigger && pTrigger->IsType(TRIGGER_TYPE_MAPINFO))
    {
        CServerMapInfoTrigger* pMapInfoTrigger = dynamic_cast<CServerMapInfoTrigger*>(pTrigger);

        if (pMapInfoTrigger && SNATIVEID(pMapInfoTrigger->GetMap().dwID).wMainID == SNATIVEID(id).wMainID)
        {
            SServerSubFindStageTriggerMessage msg;			
            msg.level = level;
            msg.stage = stage;
            pMapInfoTrigger->SendMessage(&msg);
            trigger = msg.trigger;
        }
    }
}

void SServerSubFindStageTriggerMessage::Command(CTrigger* pTrigger)
{
    if (pTrigger && pTrigger->IsType(TRIGGER_TYPE_LEVEL))
    {
        CServerLevelTrigger* pLevelTrigger = dynamic_cast<CServerLevelTrigger*>(pTrigger);

        if (pLevelTrigger && pLevelTrigger->GetLevel() == level)
        {
            /**
                우리는 현재 스테이지 트리거명을 stage%d 형식으로 사용 한다.
                이 부분이 몇몇 컨텐츠에 응집력이 생길 수 있으므로 향후 확장시
                고려해야 한다.

             */
            TCHAR temp[256] = {0};
            _sntprintf_s(temp, 256, _T("stage%d"), stage);
            trigger = TriggerSystem::FindTrigger(pLevelTrigger->GetFile().c_str(), temp);
        }
    }
}

void SServerFindPortalOpenTriggerMessage::Command(CTrigger* pTrigger)
{
    if (pTrigger && pTrigger->IsType(TRIGGER_TYPE_PORTALOPEN))
    {
		trigger = pTrigger;        
    }
}
