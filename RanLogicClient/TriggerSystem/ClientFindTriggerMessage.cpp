#include "pch.h"
#include "ClientFindTriggerMessage.h"
#include "ClientTriggerDecleare.h"

#include "../../RanLogic/TriggerSystem/TriggerSystem.h"
#include "../../RanLogic/TriggerSystem/TriggerInstance.h"

#include "../../SigmaCore/DebugInclude.h"

using namespace TriggerSystem;

/**	
	Ʈ���Ÿ� ã�µ� ���̴� ���� �޽���
    ���� Ž���� ���� ��� �ȴ�.
    ���������� ���ǹǷ� �ܺο����� ����� �� ����.
	
 */
struct SClientSubFindTriggerMessage
	: public SSystemMessage
{
	void Command(CTrigger* pTrigger);
    
    int         level;    //@ ã�� �� ����
    int         stage;  //@ ã�� ��������
    CTrigger*   trigger;  //@ ������ Ʈ����
};

// TAG_PVEClientStartStage_4a_3
void SClientFindTriggerMessage::Command(CTrigger* pTrigger)
{
    if (pTrigger && pTrigger->IsType(TRIGGER_TYPE_MAPINFO))
    {
        CClientMapInfoTrigger* pMapInfoTrigger = dynamic_cast<CClientMapInfoTrigger*>(pTrigger);

        if (pMapInfoTrigger && SNATIVEID(pMapInfoTrigger->GetMap().dwID).wMainID == SNATIVEID(id).wMainID)
        {
            SClientSubFindTriggerMessage msg;
            msg.level = level;
            msg.stage = stage;
            pMapInfoTrigger->SendMessage(&msg);            
            trigger = msg.trigger;
        }
    }
}

// TAG_PVEClientStartStage_4a_5
void SClientSubFindTriggerMessage::Command(CTrigger* pTrigger)
{
    if (pTrigger && pTrigger->IsType(TRIGGER_TYPE_LEVEL))
    {
        CClientLevelTrigger* pLevelTrigger = dynamic_cast<CClientLevelTrigger*>(pTrigger);

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
