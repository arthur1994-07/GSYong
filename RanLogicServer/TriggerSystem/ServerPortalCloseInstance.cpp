#include "pch.h"
#include "ServerPortalCloseInstance.h"
#include "ServerPortalCloseTrigger.h"
#include "ServerEntryMessage.h"

#include "../Land/GLLandMan.h"
#include "../../RanLogic/Msg/PVEMsg.h"

#include "../SigmaCore/DebugInclude.h"

using namespace TriggerSystem;

CServerPortalCloseInstance::CServerPortalCloseInstance()
{

}

void CServerPortalCloseInstance::Update(DWORD dwCharDbNum)
{
    const  CServerPortalCloseTrigger* pTrigger = 0;

    if (CastTrigger(pTrigger))
    {
        GLMSG::NET_PVE_PORTAL_STATE_FC msg;
        msg.bOpen = false;
        msg.dwGateID = pTrigger->GetFromGateID();
        msg.dwToMapID = pTrigger->GetToMapID().dwID;
        msg.dwToGateID = pTrigger->GetToGateID();
        SendMsgClient(dwCharDbNum, NET_MSG_PVE_PORTAL_STATE_FC, msg);
    }
}

void CServerPortalCloseInstance::Start()
{
    Update();
    CServerTriggerInstance::Start();
}

void CServerPortalCloseInstance::EntryPC(SMessage* msg)
{
    SServerEntryMessage* entry = 0;

    if (msg->Cast(entry))
    {
        Update(entry->dwCharDbNum);
    }
}
