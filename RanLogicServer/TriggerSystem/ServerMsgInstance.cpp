#include "pch.h"
#include "ServerMsgInstance.h"
#include "ServerMsgTrigger.h"
#include "ServerEntryMessage.h"

#include "../Land/GLLandMan.h"
#include "../../RanLogic/Msg/PVEMsg.h"

#include "../SigmaCore/DebugInclude.h"

using namespace TriggerSystem;

void CServerMsgInstance::Update(DWORD dwCharDbNum)
{
    const CServerMsgTrigger* pTrigger = 0;

    if (CastTrigger(pTrigger))
    {
        GLMSG::NET_PVE_MSG_FC msg;
        msg.strMsg = pTrigger->GetMessage();
        msg.nTime = static_cast<int>( floor(GetLifeTime()) );

		if (msg.nTime > 0)
		{
			SendMsgClient(dwCharDbNum, NET_MSG_PVE_MSG_FC, msg);
		}
    }
}

void CServerMsgInstance::Start()
{
    CServerTriggerInstance::Start();
	Update();
}

void CServerMsgInstance::EntryPC(SMessage* msg)
{
    SServerEntryMessage* entry = 0;

    if (msg->Cast(entry))
    {
        Update(entry->dwCharDbNum);
    }
}
