#include "pch.h"
#include "ClientTriggerFinger.h"
#include "ClientTriggerSystem.h"
#include "ClientFindTriggerMessage.h"

#include "../GLGaeaClient.h"
#include "../Land/GLLandManClient.h"

#include "../../SigmaCore/DebugInclude.h"

using namespace TriggerSystem;

CClientTriggerFinger::CClientTriggerFinger()
:   m_pGaeaClient(0)
,   m_stage(0)
,   m_level(0)
{

}

CClientTriggerFinger::~CClientTriggerFinger()
{
    Stop();
}

// TAG_PVEClientStartStage_3
void CClientTriggerFinger::Start()
{
    Stop();

    if (m_pGaeaClient)
    {
        GLLandManClient* pLandMan = m_pGaeaClient->GetActiveMap();

        if (pLandMan == 0)
        {
            return;
        }

        // 실행 할 트리거 파일 추출
        SClientFindTriggerMessage msg;
        msg.id = pLandMan->GetPVEIndunID().dwMapID;
        msg.stage = m_stage;
        msg.level = m_level;
        msg.trigger = 0;
        TriggerSystem::SendMessage(&msg);

        // 트리거 시작
        CTriggerFinger::Start(msg.trigger);
    }

    CTriggerFinger::Start();
}
