#include "pch.h"
#include "ServerFindTriggerMessage.h"
#include "ServerTriggerDecleare.h"

#include "../../RanLogic/TriggerSystem/TriggerSystem.h"
#include "../../RanLogic/TriggerSystem/TriggerInstance.h"

#include "../SigmaCore/DebugInclude.h"

using namespace TriggerSystem;

/**	
	�������� Ʈ���Ÿ� ã�µ� ���̴� ���� �޽���
    ���� Ž���� ���� ��� �ȴ�.
    ���������� ���ǹǷ� �ܺο����� ����� �� ����.
	
 */
struct SServerSubFindStageTriggerMessage
	: public SSystemMessage
{
	SServerSubFindStageTriggerMessage() : level(0), stage(0), trigger(0) {}

	void Command(CTrigger* pTrigger);
    
    int         level;      //@ ã�� �� ����
    int         stage;      //@ ã�� ��������
    CTrigger*   trigger;    //@ ã�� Ʈ����
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
                �츮�� ���� �������� Ʈ���Ÿ��� stage%d �������� ��� �Ѵ�.
                �� �κ��� ��� �������� �������� ���� �� �����Ƿ� ���� Ȯ���
                ����ؾ� �Ѵ�.

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
