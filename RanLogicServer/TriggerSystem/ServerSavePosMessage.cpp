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

    // 현재 인던에서 위치를 저장되지 않는 설정이다.
    // 여기를 통과한다는 것은 인던 안에 있다는 것이므로
    // false 로 처리 한다.
    bSave = false;
}
