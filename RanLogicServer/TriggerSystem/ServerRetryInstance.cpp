#include "pch.h"
#include "ServerRetryInstance.h"
#include "ServerRetryTrigger.h"
#include "ServerExitMessage.h"
#include "ServerEntryMessage.h"
#include "ServerDieMessage.h"
#include "ServerRetryMessage.h"
#include "ServerReturnMessage.h"
#include "ServerTriggerFinger.h"
#include "ServerRewardMessage.h"
#include "ServerPartyOutMessage.h"

#include "../Land/GLLandMan.h"
#include "../Character/GLChar.h"
#include "../PVE/PVESystem.h"
#include "../PVE/PVEInDun.h"
#include "../../RanLogic/Msg/PVEMsg.h"
#include "../../RanLogic/FSM/GLCharRebirthID.h"

#include "../SigmaCore/DebugInclude.h"

using namespace TriggerSystem;

CServerRetryInstance::CServerRetryInstance()
:   m_time(0.f)
,	m_bIsRewardReqest(false)
,	m_bRetring(false)
{

}

void CServerRetryInstance::StageClear ( SMessage* msg )
{
	m_die.clear();
}

void CServerRetryInstance::StageFail ( SMessage* msg )
{
	m_die.clear();
}

void CServerRetryInstance::EntryPC(SMessage* msg)
{
    SServerEntryMessage* entry = 0;

    if (msg->Cast(entry))
    {
		// 입장할 때나 퇴장할 때 설정하면 안된다;
		// 죽을때만 설정하면 된다;
		// 이로인해 퇴장후에도 부활설정이 바뀌는 문제가 생긴다;
        //SetResurrectPVE(entry->dwCharDbNum, false);
		m_entrance.insert(entry->dwCharDbNum);

		if (m_rem.find(entry->dwCharDbNum) != m_rem.end())
		{
			ReqRetry(entry->dwCharDbNum);
		}
    }
}

void CServerRetryInstance::ReturnPC(SMessage* msg)
{
	SServerReturnMessage* returnMsg = 0;

	if (msg->Cast(returnMsg))
	{
		m_time = 0.f;
	}
}

void CServerRetryInstance::ExitPC(SMessage* msg)
{
	SServerExitMessage* exitMsg = 0;

	if (msg->Cast(exitMsg))
	{
		// 입장할 때나 퇴장할 때 설정하면 안된다;
		// 죽을때만 설정하면 된다;
		// 이로인해 퇴장후에도 부활설정이 바뀌는 문제가 생긴다;
		//SetResurrectPVE(exitMsg->dwCharDbNum, true);

		CHARDBNUM_LIST_ITER iter = m_entrance.find( exitMsg->dwCharDbNum );
		if ( iter != m_entrance.end() )
		{
			m_entrance.erase( exitMsg->dwCharDbNum );
		}

		CHARDBNUM_LIST_ITER iterDie = m_die.find( exitMsg->dwCharDbNum );
		if ( iterDie != m_die.end() )
		{
			m_die.erase( exitMsg->dwCharDbNum );
		}

		if ( ( m_entrance.size() == m_die.size() ) && ( m_die.size() != 0 ) )
		{
			SServerRetryReqMessage sMsg;
			SendMessage( &sMsg );
		}
	}
}

void CServerRetryInstance::PartyOutPC ( SMessage* _pMsg )
{
	SServerPartyOutMessage* pPartyOut = NULL;

	if ( _pMsg->Cast( pPartyOut ) )
	{
		CHARDBNUM_LIST_ITER iter = m_entrance.find( pPartyOut->dwCharDbNum );
		if ( iter != m_entrance.end() )
		{
			m_entrance.erase( pPartyOut->dwCharDbNum );
		}

		CHARDBNUM_LIST_ITER iterDie = m_die.find( pPartyOut->dwCharDbNum );
		if ( iterDie != m_die.end() )
		{
			m_die.erase( pPartyOut->dwCharDbNum );
		}

		if ( ( m_entrance.size() == m_die.size() ) && ( m_die.size() != 0 ) )
		{
			SServerRetryReqMessage sMsg;
			SendMessage( &sMsg );
		}
	}
}

void CServerRetryInstance::DiePC ( SMessage* _pMsg )
{
	SServerDieMessage* pDie = NULL;

	if ( _pMsg->Cast( pDie ) )
	{
		if ( pDie->bIsRevival )
		{
			CHARDBNUM_LIST_ITER iterDie = m_die.find( pDie->dwCharDbNum );
			if ( iterDie != m_die.end() )
			{
				m_die.erase( pDie->dwCharDbNum );
			}
		}
		else
		{
			m_die.insert( pDie->dwCharDbNum );
		}
	}
}

void CServerRetryInstance::RetryReq(SMessage* msg)
{
	if ( m_bRetring )
		return;

	sc::writeLogInfo( "[ Trigger System ] [ Request to Retry!! ]" );

	m_bRetring = true;

    const CServerRetryTrigger* tri = 0;
    SServerRetryReqMessage* retry = 0;

    if (msg->Cast(retry) && CastTrigger(tri))
    {
        m_time = tri->GetTime();

        // 재도전 할 때 인원을
		// 바탕으로 체크 한다.
        m_rem = m_entrance;
		m_retry = m_entrance;

        ReqRetry();
    }
}

void CServerRetryInstance::RetryAns(SMessage* msg)
{
    SServerRetryAnsMessage* retry = 0;
    GLLandMan* randman = GetLandMan();


    if (msg->Cast(retry) && randman && m_time > 0.f)
    {
        // 결과를 클라에 전송 한다.
        SendResultMessage( (retry->bOk?GLMSG::STAGE_STATE_GAMEROVER_RETRY:GLMSG::STAGE_STATE_GAMEROVER_NOTTRY)
            , retry->dwCharDbNum );

        if (retry->bOk == false)
        {
            Process(false, GLMSG::STAGE_STATE_GAMEROVER_NOTTRY);
			m_die.clear();
			m_bRetring = false;
        }
        else
        {
            m_rem.erase(retry->dwCharDbNum);

            if (CheckRetry(retry->dwCharDbNum) == false)
            {
                Process(false, GLMSG::STAGE_STATE_GAMEROVER_NOTENOUGH);
				m_die.clear();
				m_bRetring = false;
            }

            if (m_rem.empty())
            {
                Process(true);
				m_die.clear();
				m_bRetring = false;
            }
        }
    }
}

void CServerRetryInstance::Process(bool bOk, int nWhy)
{
    GLLandMan* randman = GetLandMan();

	// 프레임이 늦으면 늦게 들어올 수도 있다?
	if( m_time == 0 && m_bIsRewardReqest )
		return;

    if (randman)
    {
        /** 
            혹시라도 돈이 변경되었을 수 있으므로 
            다시 최종적으로 체크함에 주의 한다.

         */

        if (bOk & AllCheckRetry())
        {
			Rebirth();
            CostRetry();

            GLMSG::NET_PVE_RETRY_REQ_FA msg;
            msg.sInDunID = randman->GetPVEIndunID();
            SendMsgAgent(NET_MSG_PVE_RETRY_REQ_FA, msg);
        }
        else
        {
			// 보상을 처리하도록 요청 한다.
            SServerRewardReqMessage msg;
            msg.eType = EREWARD_TYPE_STAGE_FAILED_RETRY;
            SendMessage(&msg);

			m_bIsRewardReqest = true;
        }
    }

    m_time = 0.f;
}

void CServerRetryInstance::FrameMove(float fAccTime, float fElapsedTime)
{
    CServerTriggerInstance::FrameMove(fAccTime, fElapsedTime);

    if (m_time > 0.f)
    {
        m_time -= fElapsedTime;

        if (m_time <= 0.f)
        {
            Process(false, GLMSG::STAGE_STATE_GAMEROVER_WAITTIME);
        }
    }
}

void CServerRetryInstance::SetResurrectPVE(DWORD dwCharDbNum, bool bEnable)
{
    GLLandMan* pLandMan = GetLandMan();

    if (pLandMan)
    {
        GLChar* pChar = pLandMan->GetCharByDbNum(dwCharDbNum);

        if (pChar)
        {
            // 스킬 빼고 부활을 다 막는다.;
            pChar->SetResurrectionDisable(false);
        }
    }
}

bool CServerRetryInstance::CheckRetry(DWORD dwCharDbNum)
{
    GLLandMan* pLandMan = GetLandMan();

    if (pLandMan)
    {
        GLChar* pChar = pLandMan->GetCharByDbNum(dwCharDbNum);

        if (pChar && pChar->GetInvenMoney() >= RetryMoney(pChar))
        {
            return true;
        }
    }

    return false;
}

bool CServerRetryInstance::AllCheckRetry()
{
	for (CHARDBNUM_LIST_ITER it = m_retry.begin(); it != m_retry.end(); ++it)
	{
		if (CheckRetry(*it) == false)
		{
			return false;
		}
	}

	return true;
}

void CServerRetryInstance::SendResultMessage(int nWhy, DWORD dwChaDbNum)
{
	for (CHARDBNUM_LIST_ITER it = m_entrance.begin(); it != m_entrance.end(); ++it)
	{
		GLMSG::NET_PVE_RETRY_ACK_FC msg;
        msg.dwChaDbNum = dwChaDbNum;

        if( nWhy == GLMSG::STAGE_STATE_GAMEROVER_RETRY )
        {
            if (CheckRetry(dwChaDbNum) == false)
            {
                msg.nResultCode = GLMSG::RESULT_CODE_NOT_ENOUGH;
            }
            else
            {
                msg.nResultCode = GLMSG::RESULT_CODE_OK;
            }
        }
		else if (nWhy == GLMSG::STAGE_STATE_GAMEROVER_NOTENOUGH)
		{
			if (CheckRetry(*it) == false)
			{
				msg.nResultCode = GLMSG::RESULT_CODE_NOT_ENOUGH;
			}
			else
			{
				msg.nResultCode = GLMSG::RESULT_CODE_ANOTHER_NOT_ENOUGH;
			}
		}
		else if (nWhy == GLMSG::STAGE_STATE_GAMEROVER_NOTTRY)
		{
			msg.nResultCode = GLMSG::RESULT_CODE_CANCEL;
		}
		else
		{
			msg.nResultCode = GLMSG::RESULT_CODE_TIMEOVER;
		}

		SendMsgClient(*it, NET_MSG_PVE_RETRY_ACK_FC, msg);
	}
}

void CServerRetryInstance::CostRetry()
{
	GLLandMan* pLandMan = GetLandMan();

	if (pLandMan == 0)
	{
		return;
	}

    for (CHARDBNUM_LIST_ITER it = m_entrance.begin(); it != m_entrance.end(); ++it)
    {
        GLChar* pChar = pLandMan->GetCharByDbNum(*it);

        if (pChar)
        {
            pChar->SubtractInvenMoneyServer(RetryMoney(pChar), SUBTRACT_INVEN_MONEY_INSTANCE_DUNGEON_RETRY_COST);
			pChar->SendInvenMoneyUpdate();
        }
    }
}

void CServerRetryInstance::ReqRetry()
{
    for (CHARDBNUM_LIST_ITER it = m_entrance.begin(); it != m_entrance.end(); ++it)
    {
        ReqRetry(*it);
    }
}

void CServerRetryInstance::ReqRetry(DWORD dwCharDbNum)
{
	CServerTriggerFinger* pFinger = 0;

	if (CastFinger(pFinger))
	{
		GLMSG::NET_PVE_STAGE_STATE_FC msg;
		msg.nStage = pFinger->GetStage();
		msg.nState = GLMSG::STAGE_STATE_RETRY;
		msg.nTime = static_cast<int>(m_time);
		SendMsgClient(dwCharDbNum, NET_MSG_PVE_STAGE_STATE_FC, msg);
	}
}

void CServerRetryInstance::Rebirth()
{
	GLLandMan* pLandMan = GetLandMan();

	if (pLandMan == 0)
	{
		return;
	}

	for (CHARDBNUM_LIST_ITER it = m_entrance.begin(); it != m_entrance.end(); ++it)
	{
		GLChar* pChar = pLandMan->GetCharByDbNum(*it);

		if (pChar && pChar->IsDie())
		{
			pChar->SetAlive_PVE();
		}
	}
}

const LONGLONG CServerRetryInstance::RetryMoney(GLChar* pChar)
{
	CServerTriggerFinger* finger = 0;
	const CServerRetryTrigger* retry = 0;

	if (CastFinger(finger) && CastTrigger(retry) && pChar)
	{
		return retry->RetryMoney(pChar->GetLevel(), finger->GetStage());
	}

	return 0;
}
