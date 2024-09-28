#include "pch.h"
#include "../../RanLogic/Msg/NotifyMsg.h"
#include "../../RanLogic/Notify/Notify.h"
#include "../Notify/NotifyManAgent.h"
#include "../AgentServer/GLAgentServer.h"
#include "./GLCharAG.h"

void GLCharAG::NotifyReqCA()
{
    const gamelogic::NOTIFY_DATA& Data = m_pNotify->GetData();
    for (gamelogic::NOTIFY_DATA_CITER iter=Data.begin(); iter!=Data.end(); ++iter)
    {
        std::tr1::shared_ptr<gamelogic::NotifyBase> spData = iter->second;        
        GLMSG::NOTIFY_DATA_AC MsgAC(
            spData->GetType(),
            spData->GetGuid(),
            spData->GetDate());
        MsgAC.SetJsonData(spData->GetJsonData());
        m_pServer->SENDTOCLIENT(m_ClientSlot, &MsgAC);
    }

    GLMSG::NOTIFY_DATA_AC_END MsgEnd;
    m_pServer->SENDTOCLIENT(m_ClientSlot, &MsgEnd);
}