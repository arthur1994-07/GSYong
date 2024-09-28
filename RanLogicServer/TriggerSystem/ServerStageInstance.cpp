#include "pch.h"
#include "ServerStageInstance.h"
#include "ServerTriggerFinger.h"
#include "ServerStageTrigger.h"
#include "ServerEntryMessage.h"
#include "ServerStageMessage.h"
#include "ServerExitMessage.h"
#include "ServerDieMessage.h"
#include "ServerReturnMessage.h"
#include "ServerPartyOutMessage.h"
#include "ServerActionMessage.h"

#include "../Land/GLLandMan.h"
#include "../Character/GLChar.h"

#include "../SigmaCore/DebugInclude.h"

using namespace TriggerSystem;

CServerStageInstance::CServerStageInstance()
: m_completed(0)
, m_stage(0)
, m_playtime(0)
, m_mission(0)
, m_last(false)
, m_exit(false)
{

}

void CServerStageInstance::Update(DWORD dwCharDbNum, int nState)
{
	sc::writeLogInfo( sc::string::format(
		"[ Trigger System ] [ Entry PC, ChaNum : %1%, Stage : %2%, State : %3%, Time : %4% ]",
		dwCharDbNum,
		m_stage,
		nState,
		static_cast<int>(ceil(m_playtime - GetTime())) ) );

    GLMSG::NET_PVE_STAGE_STATE_FC msg;
    msg.nStage = m_stage;
    msg.nState = nState;
    msg.nTime = static_cast<int>(ceil(m_playtime - GetTime()));
    SendMsgClient(dwCharDbNum, NET_MSG_PVE_STAGE_STATE_FC, msg);
}

void CServerStageInstance::Start()
{
    GetInfo();
    Update();
    CServerTriggerInstance::Start();

    GLLandMan* pLandMan = GetLandMan();
    for (CHARDBNUM_LIST_ITER it = m_entrance.begin(); it != m_entrance.end(); ++it)
    {
        GLChar* pChar = pLandMan->GetCharByDbNum(*it);
        if( pChar )
            WriteLogPlay( m_entrance.size(), pChar->CharDbNum(), pChar->GetLevel(), m_stage );
    }
}

void CServerStageInstance::Exit()
{
    if (m_exit == false)
    {
        Update(0, GLMSG::STAGE_STATE_TIMEOVER);
        StageFailed();

        // 생명 시간을 0 으로 해 
        // 다시 Exit() 로 들어오지 못하게 하고
        // 무한 대기 한다.
        SetLifeTime(0.f);
    }
    else
    {
        CServerTriggerInstance::Exit();
    }
}

void CServerStageInstance::ReturnPC(SMessage* msg)
{
    SServerReturnMessage* returnMsg = 0;

    if (msg->Cast(returnMsg))
    {
        if (returnMsg->nWhy == GLMSG::STAGE_STATE_GAMEROVER_TIMEOVER ||
			returnMsg->nWhy == GLMSG::STAGE_STATE_GAMEROVER_FIREPARTY)
        {
            m_exit = true;
            Exit();
        }
    }
}

void CServerStageInstance::FrameMove(float fAccTime, float fElapsedTime)
{
    CServerTriggerInstance::FrameMove(fAccTime, fElapsedTime);
}

void CServerStageInstance::EntryPC(SMessage* msg)
{
    SServerEntryMessage* entry = 0;

    if (msg->Cast(entry))
    {
        m_entrance.insert(entry->dwCharDbNum);
        //m_die.insert(entry->dwCharDbNum);
		
		Update( entry->dwCharDbNum );
    }
}

void CServerStageInstance::ExitPC(SMessage* msg)
{
    SServerExitMessage* exit = 0;

    if (msg->Cast(exit))
    {
		CHARDBNUM_LIST_ITER iterEntrance = m_entrance.find( exit->dwCharDbNum );
		if ( iterEntrance != m_entrance.end() )
		{
			m_entrance.erase(exit->dwCharDbNum);
		}

		CHARDBNUM_LIST_ITER iterDie = m_die.find( exit->dwCharDbNum );
		if ( iterDie != m_die.end() )
		{
			m_die.erase(exit->dwCharDbNum);
		}
    }
}

void CServerStageInstance::PartyOutPC ( SMessage* _pMsg )
{
	SServerPartyOutMessage* pPartyOut = 0;

	if ( _pMsg->Cast( pPartyOut ) )
	{
		CHARDBNUM_LIST_ITER iterEntrance = m_entrance.find( pPartyOut->dwCharDbNum );
		if ( iterEntrance != m_entrance.end() )
		{
			m_entrance.erase( pPartyOut->dwCharDbNum );
		}

		CHARDBNUM_LIST_ITER iterDie = m_die.find( pPartyOut->dwCharDbNum );
		if ( iterDie != m_die.end() )
		{
			m_die.erase( pPartyOut->dwCharDbNum );
		}
	}
}

void CServerStageInstance::ActionPC ( SMessage* _pMsg )
{
	SServerActionMessage* pAction = 0;

	if ( _pMsg->Cast( pAction ) )
	{
		if ( pAction->emNextAction == GLAT_DIE )
		{
			Update( pAction->dwCharDbNum, GLMSG::STAGE_STATE_DIED );
		}
		else if ( pAction->emCurrentAction == GLAT_DIE )
		{
			Update( pAction->dwCharDbNum, GLMSG::STAGE_STATE_ONGOING );
		}
	}
}

void CServerStageInstance::DiePC(SMessage* msg)
{
    SServerDieMessage* die = 0;

    if (msg->Cast(die))
    {
        if( die->bIsRevival == true )
		{
            m_die.erase(die->dwCharDbNum);	
		}
        else
		{
			GLLandMan* pLandMan = GetLandMan();
			if ( pLandMan )
			{
				GLChar* pChar = pLandMan->GetCharByDbNum( die->dwCharDbNum );
				if ( pChar )
				{
					// 스킬 빼고 부활을 다 막는다.;
					pChar->SetResurrectionDisable(false);
				}
			}

            m_die.insert(die->dwCharDbNum);
		}

        if (m_die.size() == m_entrance.size())
        {
            Update(0, GLMSG::STAGE_STATE_ALLDIED);
            StageFailed();
        }
    }
}


bool CServerStageInstance::GetInfo()
{
    GLLandMan* pLandMan = GetLandMan();

    if (pLandMan)
    {
        pLandMan->CollectPC(m_entrance);
    }
	
    CServerTriggerFinger* pFinger = 0;
	const CServerStageTrigger* pTrigger = 0;

    if (CastTrigger(pTrigger) && CastFinger(pFinger))
    {
        m_stage = pFinger->GetStage();
        m_playtime = pTrigger->GetLifeTime();
        m_mission = pTrigger->GetMission();
        m_last = pTrigger->GetLastStage();
        return true;
    }

    return false;
}

void CServerStageInstance::MissionCompleted(SMessage* msg)
{

    //SServerMissionCompletedMessage* msgMission;
//     GLLandMan* pLand = GetLandMan();
//     if (msg->Cast(msgMission))
//         if( m_last && msgMission->eType == MISSION_MONSTERCLEAR )

    ++m_completed;

    if (m_completed == m_mission)
    {
        StageCompleted();
    }
}

void CServerStageInstance::StageCompleted()
{
    if (m_last)
    {
        Update(0, GLMSG::STAGE_STATE_GAMEROVER_COMPLETED);
    }
    else
    {
        Update(0, GLMSG::STAGE_STATE_STAGECLEAR);
    }

    m_exit = true;
    SendStageClear(true);
    Exit();
}

void CServerStageInstance::StageFailed()
{
    /**
        시간이 오버 되거나
        모두 죽을 경우 실패 된다.

     */	
    m_exit = true;
	SendStageClear(false);
    Exit();
}

void CServerStageInstance::SendStageClear(bool bCompleted)
{
    GLLandMan* pLand = GetLandMan();
    if( pLand )
        pLand->DelAllMobSchLayer();

    SServerStageClearMessage msg;
    msg.bLast = m_last;
    msg.bCompleted = bCompleted;
    SendMessage(&msg);
}

