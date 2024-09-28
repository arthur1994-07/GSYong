#include "pch.h"
#include "ServerRewardInstance.h"
#include "ServerRewardTrigger.h"
#include "ServerTriggerFinger.h"
#include "ServerDamageMessage.h"
#include "ServerHealMessage.h"
#include "ServerEntryMessage.h"
#include "ServerExitMessage.h"
#include "ServerRewardMessage.h"
#include "ServerReturnMessage.h"
#include "ServerStageMessage.h"

#include "../Land/GLLandMan.h"
#include "../Character/GLChar.h"
#include "../../RanLogic/Msg/PVEMsg.h"
#include "../../RanLogic/GLogicData.h"

#include "../SigmaCore/DebugInclude.h"

using namespace TriggerSystem;

CServerRewardInstance::CServerRewardInstance()
:   m_timer(0.f)
,   m_return(false)
,   m_last(false)
,	m_completed(false)
,	m_immediately(false)
,   m_dwMaxMVP(0)
,   m_isReword(false)
{

}

void CServerRewardInstance::Start()
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

		if (pChar)
		{
			m_info[*it].dwCharDbNum = pChar->CharDbNum();
			m_info[*it].strCharName = pChar->GetName();
			m_info[*it].dwClass = pChar->GETCHARINDEX();
		}
    }

    const CServerRewardTrigger* reward = 0;

    if (CastTrigger(reward))
    {
        m_timer = reward->GetWaitTime();
    }

    CServerTriggerInstance::Start();
}

void CServerRewardInstance::Exit()
{
	if (m_return)
    {
		if (m_immediately)
		{
			ReturnImmidiatelyAll();
		}
		else if (m_completed)
		{
			ReturnAll(GLMSG::STAGE_STATE_GAMEROVER_COMPLETED);
		}
		else
		{
			ReturnAll(GLMSG::STAGE_STATE_GAMEROVER_FAILED);
		}

        // 생명 시간을 0 으로 해 
        // 다시 Exit() 로 들어오지 못하게 하고
        // 무한 대기 한다. 이렇게 하는 이유는 에이전트를 통해
		// 귀환 되어야 하기 때문 이다.
        SetLifeTime(0.f);
    }
    else
    {
        CServerTriggerInstance::Exit();
    }
}

void CServerRewardInstance::EntryPC(SMessage* msg)
{
    SServerEntryMessage* entry = 0;

    if (msg->Cast(entry))
    {
		GLChar* pChar = 0;

		if (GetChar(entry->dwCharDbNum, pChar))
		{
			m_info[entry->dwCharDbNum].dwCharDbNum = pChar->CharDbNum();
			m_info[entry->dwCharDbNum].strCharName = pChar->GetName();
			m_info[entry->dwCharDbNum].dwClass = pChar->GETCHARINDEX();
		}
    }
}

void CServerRewardInstance::Damage(SMessage* msg)
{
    SServerDamageMessage* damage = 0;

    if (msg->Cast(damage))
    {
        m_info[damage->dwCharDbNum].dwDamage += damage->dwDamage;
    }
}

void CServerRewardInstance::Heal(SMessage* msg)
{
    SServerHealMessage* heal = 0;

    if (msg->Cast(heal))
    {
        m_info[heal->dwCharDbNum].dwHeal += heal->dwHeal;
    }
}

void CServerRewardInstance::ReturnPC(SMessage* msg)
{
    SServerReturnMessage* returnMsg = 0;

    if (msg->Cast(returnMsg))
    {
		/**
			스테이지가 정상적으로 끝난 경우 이므로 바로 귀환 시킨다.

		 */
        if (returnMsg->nWhy != GLMSG::STAGE_STATE_GAMEROVER_TIMEOVER &&
			returnMsg->nWhy != GLMSG::STAGE_STATE_GAMEROVER_FIREPARTY)
        {
            ReturnImmidiately(returnMsg->dwCharDbNum);
        }

		/**
			게임 종료에 의한 경우만 Exit() 에서 즉시 귀환 한다.
			파티 종료는 현재 완료 상태일지 실패 상태일지 모르므로 에이전트를 경우해 귀환 한다.

		 */
		if (returnMsg->nWhy == GLMSG::STAGE_STATE_GAMEROVER_TIMEOVER)
		{
			m_immediately = true;
		}
    }
}

void CServerRewardInstance::ReturnAllPC(SMessage* msg)
{
    SServerReturnAllMessage* returnAllMsg = 0;

    if (msg->Cast(returnAllMsg))
    {
		ReturnImmidiatelyAll();
    }
}

void CServerRewardInstance::StageExit(SMessage* msg)
{
	SServerStageExitMessage* exitMsg = 0;

	if (msg->Cast(exitMsg))
	{
		GLLandMan* pLandMan = GetLandMan();

		if (pLandMan == 0)
		{
			return;
		}

		ReturnImmidiately(exitMsg->dwCharDbNum);

		CHARDBNUM_LIST entrance;

		if (pLandMan)
		{
			pLandMan->CollectPC(entrance);
		}

		entrance.erase(exitMsg->dwCharDbNum);

		/**
			모든 사람들이 나갔을 경우
			즉시 종료 처리 한다.

		 */
		if (entrance.empty())
		{
			Exit();
		}
	}
}

void CServerRewardInstance::RewardAdd(SMessage* msg)
{
    SServerRewardAddMessage* reward = 0;

    if (msg->Cast(reward))
    {
        BOOST_FOREACH(GLMSG::CharResult& result, reward->results)
        {
			m_info[result.charDbNum].dwCharDbNum = result.charDbNum;
			m_info[result.charDbNum].strCharName = result.chaName;
			m_info[result.charDbNum].dwClass = result.dwClass;
            m_info[result.charDbNum].dwDamage += result.dwDamage;
            m_info[result.charDbNum].dwHeal += result.dwHeal;
        }
    }
}

void CServerRewardInstance::RewardReq(SMessage* msg)
{
    SServerRewardReqMessage* req = 0;

    if (msg->Cast(req))
    {
		if (req->eType == EREWARD_TYPE_STAGE_CLEAR)
		{
			m_last = false;
			m_completed = true;
			RelayRewardInfo();
		}
        else if (req->eType == EREWARD_TYPE_STAGE_COMPLETED)
        {
            m_last = true;
			m_completed = true;
            Reward();
        }
        else
        {
            m_last = false;
			m_completed = false;

			/**
				인던 시간 종료에 의한 경우만 바로 귀환 시킴에 주의 한다.
				파티 종료나 기타는 바로 귀환 시키지 않고 에이전트를 통한다.

			 */
			m_immediately = (req->eType == EREWARD_TYPE_STAGE_FAILED_TIMEOVER);

            Reward();
        }
    }
}

void CServerRewardInstance::Reward()
{
    if( m_isReword == false )
    {
        m_isReword = true;

        CalcReward();
        ApplyServer();
        ApplyClient();

        SetTime(0.f);
        SetLifeTime(m_timer);
        m_return = true;
    }
}

void CServerRewardInstance::CalcReward()
{
    m_dwMaxMVP = 0;

    // 기본적인 보상을 계산한다.
    for (CharRewardInfoIt it = m_info.begin(); it != m_info.end(); ++it)
    {
        it->second.dwExp = RewardExp(it->first);
        it->second.dwMVP = RewardMVP(it->first, it->second.dwDamage, it->second.dwHeal);

        // MVP 를 찾는다.
        if (it->second.dwMVP > m_dwMaxMVP)
        {
            m_dwMaxMVP = it->second.dwMVP;
        }
    }

    // MVP 에게 줄 보상(돈)을 추가로 계산 한다.
    for (CharRewardInfoIt it = m_info.begin(); it != m_info.end(); ++it)
    {
        if (m_dwMaxMVP > 0 && it->second.dwMVP == m_dwMaxMVP)
        {
            it->second.dwMoney = RewardMoney(it->first);
        }
        else
        {
            // MVP 가 아닌 인원은
            // MVP 점수를 제거 한다.
            it->second.dwMVP = 0;
        }
    }
}

const LONGLONG CServerRewardInstance::RewardMoney(const DWORD dwCharDbNum)
{
    GLChar* pChar = 0;
    CServerTriggerFinger* finger = 0;
    const CServerRewardTrigger* reward = 0;

    if (CastFinger(finger) && CastTrigger(reward) && GetChar(dwCharDbNum, pChar))
    {
		return reward->RewardMoney(pChar->GetLevel(), finger->GetStage());
    }

    return 0;
}

const int CServerRewardInstance::RewardExp(const DWORD dwCharDbNum)
{
    GLChar* pChar = 0;
    CServerTriggerFinger* finger = 0;
    const CServerRewardTrigger* reward = 0;

    if (CastFinger(finger) && CastTrigger(reward) && GetChar(dwCharDbNum, pChar))
    {
		WORD wMax = GLCONST_CHAR::wMAX_LEVEL;

		if (pChar->m_emClass & GLCC_EXTREME)
		{
			wMax = GLCONST_CHAR::wMAX_EXTREME_LEVEL;
		}

		return reward->RewardExp(pChar->GetLevel(), finger->GetStage(), wMax, pChar->GET_LEVELUP_EXP());
    }

    return 0;
}

const int CServerRewardInstance::RewardMVP(const DWORD dwCharDbNum, const DWORD cDamage, const DWORD cHeal)
{
	GLChar* pChar = 0;
	CServerTriggerFinger* finger = 0;
    const CServerRewardTrigger* reward = 0;

    if (CastFinger(finger) && CastTrigger(reward) && GetChar(dwCharDbNum, pChar))
    {
		return reward->RewardMVP(finger->GetStage(), cDamage, cHeal);
    }

    return 0;
}

bool CServerRewardInstance::GetChar(const DWORD dwCharDbNum, GLChar*& pChar)
{
    GLLandMan* pLandMan = GetLandMan();

    if (pLandMan)
    {
        pChar = pLandMan->GetCharByDbNum(dwCharDbNum);

        return pChar != 0;
    }

    return false;
}

void CServerRewardInstance::ApplyServer()
{
    GLChar* pChar = 0;

    for (CharRewardInfoIt it = m_info.begin(); it != m_info.end(); ++it)
    {
        const SRewardInfo& sReward = it->second;
        if (GetChar(it->first, pChar))
        {
            pChar->ReceiveExpApply(sReward.dwExp, true);

            if (sReward.dwMoney > 0)
            {
                pChar->AddInvenMoneyAndPickupMessage(sReward.dwMoney, ADD_INVEN_MONEY_INSTANCE_DUNGEON_REWARD);
				pChar->SendInvenMoneyUpdate();
            }

            WriteLogEnd( m_info.size(), pChar->CharDbNum(), pChar->GetLevel(), 0, (m_dwMaxMVP == sReward.dwMVP), sReward.dwMoney, sReward.dwExp );
        }
    }
}

void CServerRewardInstance::ApplyClient()
{
    GLChar* pChar = 0;
    GLMSG::NET_PVE_STAGE_RESULT_FC NetMsg;

    for (CharRewardInfoIt it = m_info.begin(); it != m_info.end(); ++it)
    {
        GLMSG::CharResult result;
		result.charDbNum = it->second.dwCharDbNum;
		result.chaName = it->second.strCharName;
		result.dwClass = it->second.dwClass;
        result.dwDamage = it->second.dwDamage;
        result.dwHeal = it->second.dwHeal;
        result.bExp = it->second.dwExp != 0;
        result.bMoney = it->second.dwMoney != 0;
        result.bMVP = it->second.dwMVP != 0;
        NetMsg.results.push_back(result); 
    }

    CServerTriggerFinger* finger = 0;

    if (CastFinger(finger) && m_pTrigger)
    {
        NetMsg.nStage = finger->GetStage();
        NetMsg.nTimer = int(m_timer);
        NetMsg.bLast = m_last;
    }

    SendMsgClient(0, NET_MSG_PVE_STAGE_RESULT_FC, NetMsg);
}

void CServerRewardInstance::RelayRewardInfo()
{
    GLLandMan* pLandMan = GetLandMan();

    if (pLandMan == 0)
    {
        return;
    }

    GLChar* pChar = 0;
    GLMSG::NET_PVE_STAGE_RESULT_FA_OR_AF NetMsg;
    NetMsg.sInDunID = pLandMan->GetPVEIndunID();

    for (CharRewardInfoIt it = m_info.begin(); it != m_info.end(); ++it)
    {
        GLMSG::CharResult result;
        result.charDbNum = it->second.dwCharDbNum;
        result.chaName = it->second.strCharName;
        result.dwClass = it->second.dwClass;
        result.dwDamage = it->second.dwDamage;
        result.dwHeal = it->second.dwHeal;
        NetMsg.results.push_back(result);
    }

    SendMsgAgent(NET_MSG_PVE_STAGE_RESULT_FA_OR_AF, NetMsg);
}

void CServerRewardInstance::ReturnAll(int nWhy)
{
    GLLandMan* pLandMan = GetLandMan();

    if (pLandMan == 0)
    {
        return;
    }

    GLMSG::NET_PVE_RETURN_REQ_FA NetMsg;
    NetMsg.sInDunID = pLandMan->GetPVEIndunID();
    NetMsg.nWhy = nWhy;
    SendMsgAgent(NET_MSG_PVE_RETURN_REQ_FA, NetMsg);
}

void CServerRewardInstance::ReturnImmidiatelyAll()
{
	GLLandMan* pLandMan = GetLandMan();

	CHARDBNUM_LIST entrance;

	if (pLandMan)
	{
		pLandMan->CollectPC(entrance);
	}

	for (CHARDBNUM_LIST_ITER it = entrance.begin(); it != entrance.end(); ++it)
	{
		ReturnImmidiately(*it);
	}
}

void CServerRewardInstance::ReturnImmidiately(DWORD dwCharDbNum)
{
    GLLandMan* pLandMan = GetLandMan();

    if (pLandMan == 0)
    {
        return;
    }

    GLChar* pChar = pLandMan->GetCharByDbNum(dwCharDbNum);

    if (pChar == 0)
    {
        return;
    }

	if (pChar->IsDie())
	{
		pChar->SetAlive_PVE();
	}

    pLandMan->Teleport(dwCharDbNum);
}
