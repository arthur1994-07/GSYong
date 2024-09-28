#include "pch.h"
#include "ServerTriggerFinger.h"
#include "ServerTriggerSystem.h"
#include "ServerFindTriggerMessage.h"
#include "ServerEntryMessage.h"

#include "../../RanLogic/Msg/PVEMsg.h"
#include "../Land/GLLandMan.h"

#include "../SigmaCore/DebugInclude.h"

using namespace TriggerSystem;

CServerTriggerFinger::CServerTriggerFinger()
:   m_pLandMan(0)
,   m_stage(0)
,   m_level(0)
{
    
}

void CServerTriggerFinger::Start()
{
    Stop();

    if (m_pLandMan)
    {
        // ���� �� Ʈ���� ���� ����
        SServerFindStageTriggerMessage msg;
        msg.id = m_pLandMan->GetPVEIndunID().dwMapID;
        msg.stage = m_stage;
        msg.level = m_level;
        msg.trigger = 0;
        TriggerSystem::SendMessage(&msg);

        // ���� �� ���� ����
        SServerMapInfoMessage info;
        info.mapinfo.mapid = msg.id;
        TriggerSystem::SendMessage(&info);
        m_mapinfo = info.mapinfo;

        // Ʈ���� ����
        CTriggerFinger::Start(msg.trigger);
    }

    CTriggerFinger::Start();
}
