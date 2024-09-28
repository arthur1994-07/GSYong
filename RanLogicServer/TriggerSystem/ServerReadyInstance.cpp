#include "pch.h"
#include "ServerReadyInstance.h"
#include "ServerReadyTrigger.h"
#include "ServerTriggerFinger.h"
#include "ServerExitMessage.h"
#include "ServerEntryMessage.h"
#include "ServerPartyOutMessage.h"

#include "../Land/GLLandMan.h"
#include "../Character/GLChar.h"

#include "../../RanLogic/Msg/PVEMsg.h"
#include "../../RanLogic/FSM/GLCharMovementID.h"


#include "../SigmaCore/DebugInclude.h"

using namespace TriggerSystem;

void CServerReadyInstance::Start()
{
	const CServerReadyTrigger* pTrigger = 0;

	if (CastTrigger(pTrigger))
	{
		m_all = pTrigger->GetCount();
	}

    GLLandMan* pLandMan = GetLandMan();

    for (CHARDBNUM_LIST_ITER it = m_entrance.begin(); it != m_entrance.end(); ++it)
    {
        GLChar* pChar = pLandMan->GetCharByDbNum(*it);

        if( pChar == NULL )
            continue;

        pChar->SetAlive();
    }

	CServerTriggerInstance::Start();
}

void CServerReadyInstance::SetCur(DWORD dwChaDbNum)
{
	++m_cur;
    m_entrance.insert(dwChaDbNum);	
    
    m_bUpdate = true;
    
}

void CServerReadyInstance::RemoveCur(DWORD dwChaDbNum)
{
	CHARDBNUM_LIST_ITER iter = m_entrance.find( dwChaDbNum );
	if ( iter != m_entrance.end() )
	{
		SetMovement(dwChaDbNum, true);

		--m_cur;
		m_entrance.erase(dwChaDbNum);
		m_bUpdate = true;
	}
}

void CServerReadyInstance::FrameMove(float fAccTime, float fElapsedTime)
{
    CServerTriggerInstance::FrameMove(fAccTime, fElapsedTime);

    if (m_bUpdate)
    {
        Update();
        m_bUpdate = false;
    }
}

void CServerReadyInstance::Exit()
{
    Update();

    // 움직임 불가능 해제
    for (CHARDBNUM_LIST_ITER it = m_entrance.begin(); it != m_entrance.end(); ++it)
    {
        SetMovement(*it, true);
    }

    CServerTriggerInstance::Exit();
}

void CServerReadyInstance::EntryPC(SMessage* msg)
{
    SServerEntryMessage* entry = 0;

    if (msg->Cast(entry))
    {
		SetMovement(entry->dwCharDbNum, false);
        SetCur(entry->dwCharDbNum);
    }
}

void CServerReadyInstance::ExitPC(SMessage* msg)
{
    SServerExitMessage* exit = 0;

    if (msg->Cast(exit))
    {
        RemoveCur(exit->dwCharDbNum);
    }
}

void CServerReadyInstance::PartyOutPC ( SMessage* _pMsg )
{
	SServerPartyOutMessage* pPartyOut = 0;

	if ( _pMsg->Cast( pPartyOut ) )
	{
		RemoveCur( pPartyOut->dwCharDbNum );
	}
}

void CServerReadyInstance::SetMovement(DWORD dwCharDbNum, bool bEnable)
{
    GLLandMan* pLandMan = GetLandMan();

    if (pLandMan)
    {
        GLChar* pChar = pLandMan->GetCharByDbNum(dwCharDbNum);

        if (pChar)
        {
            pChar->SetMovement(CHAR_MOVEMENT_STATE_INSTANTMAP_READY, bEnable);
        }
    }
}

void CServerReadyInstance::Update()
{
	/**
		생명시간이 0 이면 종료된 상태 이다.
		이것이 필요한 이유는 레디 중에 시간 종료로 게임이 끝나면
		더이상 레디 정보를 전달할 필요가 없기 때문이다.

	 */
	if (GetLifeTime() == 0.f)
	{
		return;
	}

	CServerTriggerFinger* pFinger = 0;
	const CServerReadyTrigger* pTrigger = 0;

	//스테이지를 가져오기 위해 핑거 추가
    //if (CastTrigger(pTrigger))
    //{
    //    float fCurTime = GetLifeTime() - GetTime();

    //    GLMSG::NET_PVE_ENTRANCE_STATE_FC msg;
    //    msg.nCur = m_cur;
    //    msg.nAll = m_all;
    //    msg.nTime = static_cast<int>(ceil(fCurTime));

    //    SendMsgClient(0, NET_MSG_PVE_ENTRANCE_STATE_FC, msg);
    //}
	if (CastTrigger(pTrigger) && CastFinger(pFinger))
	{
		float fCurTime = GetLifeTime() - GetTime();

		GLMSG::NET_PVE_ENTRANCE_STATE_FC msg;
		msg.nCur = m_cur;
		msg.nAll = m_all;
		msg.nTime = static_cast<int>(ceil(fCurTime));
		msg.nStage = pFinger->GetStage();

		SendMsgClient(0, NET_MSG_PVE_ENTRANCE_STATE_FC, msg);
	}
}
