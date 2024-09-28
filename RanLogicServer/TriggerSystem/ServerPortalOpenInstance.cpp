#include "pch.h"
#include "ServerPortalOpenInstance.h"
#include "ServerTriggerFinger.h"
#include "ServerPortalOpenTrigger.h"
#include "ServerEntryMessage.h"
#include "ServerExitMessage.h"
#include "ServerPortalMessage.h"
#include "ServerPartyOutMessage.h"

#include "../Land/GLLandMan.h"
#include "../Character/GLChar.h"
#include "../../RanLogic/Msg/PVEMsg.h"

#include "../SigmaCore/DebugInclude.h"

using namespace TriggerSystem;

CServerPortalOpenInstance::CServerPortalOpenInstance()
{

}

void CServerPortalOpenInstance::Update(DWORD dwCharDbNum)
{
    const CServerPortalOpenTrigger* pTrigger = 0;

    if (CastTrigger(pTrigger))
    {
        GLMSG::NET_PVE_PORTAL_STATE_FC msg;
        msg.bOpen = true;
        msg.dwGateID = pTrigger->GetFromGateID();
        msg.dwToMapID = pTrigger->GetToMapID().dwID;
        msg.dwToGateID = pTrigger->GetToGateID();
        SendMsgClient(dwCharDbNum, NET_MSG_PVE_PORTAL_STATE_FC, msg);
    }
}

void CServerPortalOpenInstance::Start()
{
    Update();

    CServerTriggerInstance::Start();
}

void CServerPortalOpenInstance::EntryPC(SMessage* msg)
{
    SServerEntryMessage* entry = 0;

    if (msg->Cast(entry))
    {
        Update(entry->dwCharDbNum);
    }
}

void CServerPortalOpenInstance::ExitPC(SMessage* msg)
{
}

void CServerPortalOpenInstance::PartyOutPC ( SMessage* _pMsg )
{
}

void CServerPortalOpenInstance::DoPortal(SMessage* msg)
{
    GLLandMan* pLandMan = GetLandMan();
    SServerPortalMessage* pPortalMsg = 0;
    const CServerPortalOpenTrigger* pTrigger = 0;

    if (pLandMan && CastTrigger(pTrigger) && msg->Cast(pPortalMsg))
    {
        if (pTrigger->GetFromGateID() != pPortalMsg->dwFromGateID)
        {
            return;
        }

        if (pTrigger->GetToMapID() != pPortalMsg->dwToMapID)
        {
            return;
        }

        if (pTrigger->GetToGateID() != pPortalMsg->dwToGateID)
        {
            return;
        }

        DxLandGateMan* pGateMan = pLandMan->GetLandGateMan();

        if (pGateMan == 0)
        {
            return;
        }

        DxLandGate* pGate = pGateMan->FindLandGate(pPortalMsg->dwToGateID);

        if (pGate == 0)
        {
            return;
        }

		Rebirth();
        Teleport(pPortalMsg);
        Exit();
    }
}

void CServerPortalOpenInstance::Teleport(const SServerPortalMessage* pPortalMsg)
{
    GLLandMan* pLandMan = GetLandMan();

    if (pLandMan == 0)
    {
        return;
    }

    CServerTriggerFinger* pFinger = 0;

    if (CastFinger(pFinger))
    {
        GLMSG::NET_PVE_MOVE_REQ_FA NetMsgFld;
        NetMsgFld.sInDunID = pLandMan->GetPVEIndunID();
        NetMsgFld.dwToMapID = pPortalMsg->dwToMapID;
        NetMsgFld.dwToGateID = pPortalMsg->dwToGateID;
        NetMsgFld.nStage = pFinger->GetStage() + 1;
        SendMsgAgent(NET_MSG_PVE_MOVE_REQ_FA, NetMsgFld);
    }
}

void CServerPortalOpenInstance::Rebirth()
{
	GLLandMan* pLandMan = GetLandMan();

	CHARDBNUM_LIST entrance;

	if (pLandMan)
	{
		pLandMan->CollectPC(entrance);
	}

	for (CHARDBNUM_LIST_ITER it = entrance.begin(); it != entrance.end(); ++it)
	{
		GLChar* pChar = pLandMan->GetCharByDbNum(*it);

		if (pChar && pChar->IsDie())
		{
			pChar->SetAlive_PVE();
		}
	}
}
