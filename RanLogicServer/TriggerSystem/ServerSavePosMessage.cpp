#include "pch.h"
#include "ServerSavePosMessage.h"
#include "ServerTriggerInstance.h"

#include "../../RanLogic/TriggerSystem/TriggerInstance.h"

#include "../SigmaCore/DebugInclude.h"

using namespace TriggerSystem;

void SServerSavePosMessage::Command(CTriggerInstance* pInstance)
{
    CServerTriggerInstance* pServerInstance = 0;

    if (pInstance && pInstance->CastInstance(pServerInstance))
    {
        pServerInstance->SavePos(this);
    }

    // ���� �δ����� ��ġ�� ������� �ʴ� �����̴�.
    // ���⸦ ����Ѵٴ� ���� �δ� �ȿ� �ִٴ� ���̹Ƿ�
    // false �� ó�� �Ѵ�.
    bSave = false;
}
