#include "pch.h"
#include "ServerMonsterClearInstance.h"
#include "ServerMonsterClearTrigger.h"
#include "ServerTriggerFinger.h"
#include "ServerDieCrowMessage.h"
#include "ServerEntryMessage.h"
#include "ServerStageMessage.h"

#include "../Land/GLLandMan.h"
#include "../../RanLogic/Msg/PVEMsg.h"

#include "../SigmaCore/DebugInclude.h"

using namespace TriggerSystem;

CServerMonsterClearInstance::CServerMonsterClearInstance()
:   m_count(0)
,   m_update(false)
{

}

void CServerMonsterClearInstance::IncCur()
{
    ++m_count;
    m_update = true;
}

int CServerMonsterClearInstance::GetCur()
{
    return m_count;
}

void CServerMonsterClearInstance::FrameMove(float fAccTime, float fElapsedTime)
{
    CServerTriggerInstance::FrameMove(fAccTime, fElapsedTime);

    if (m_update)
    {
        Update();
        m_update = false;
    }
}

void CServerMonsterClearInstance::Update(DWORD dwCharDbNum)
{
    const CServerMonsterClearTrigger* pTrigger = 0;

    if (CastTrigger(pTrigger))
    {
        int nCur = GetCur();
        int nAll = pTrigger->GetCount();
 
        if (nCur >= nAll)
        {
            SServerMissionCompletedMessage msg;
            msg.eType = MISSION_MONSTERCLEAR;
            m_pFinger->SendMessage(&msg);
			Exit();
        }

        GLMSG::NET_PVE_MONSTERCLEAR_STATE_FC msg;
        msg.nCur = nCur;
        msg.nAll = nAll;
        SendMsgClient(dwCharDbNum, NET_MSG_PVE_MONSTERCLEAR_STATE_FC, msg);
    }
}

void CServerMonsterClearInstance::Start()
{
    Update();
    CServerTriggerInstance::Start();
}

void CServerMonsterClearInstance::EntryPC(SMessage* msg)
{
    SServerEntryMessage* entry = 0;

    if (msg->Cast(entry))
    {
        Update(entry->dwCharDbNum);
    }
}

void CServerMonsterClearInstance::DieCrow(SMessage* msg)
{
    SServerDieCrowMessage* crowexit = 0;

    if (msg->Cast(crowexit))
    {
        if (crowexit->eCromType == CROW_MOB)
        {
            IncCur();
            m_update = true;
        }
    }
}
