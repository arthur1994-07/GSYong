#include "pch.h"
#include "ServerPostStageInstance.h"
#include "ServerPostStageTrigger.h"
#include "ServerStageMessage.h"
#include "ServerRetryMessage.h"
#include "ServerRewardMessage.h"
#include "ServerReturnMessage.h"
#include "ServerTriggerFinger.h"

#include "../SigmaCore/DebugInclude.h"

#include "../Character/GLChar.h"

using namespace TriggerSystem;

CServerPostStageInstance::CServerPostStageInstance()
:   m_fTime(0.f)
,   m_bLast(false)
,   m_bCompleted(false)
,   m_bTimeOver(false)
,	m_bFireParty(false)
{

}

void CServerPostStageInstance::Exit()
{
	CServerTriggerFinger* finger = 0;

	if (CastFinger(finger))
	{
		finger->AddTrigger(m_pTrigger->GetChild());
	}
}

void CServerPostStageInstance::StageClear(SMessage* msg)
{
    SServerStageClearMessage* clear = 0;

    if (msg->Cast(clear))
    {
        m_bLast = clear->bLast;
        m_bCompleted = clear->bCompleted;
    }

    Run();
}

void CServerPostStageInstance::ReturnPC(SMessage* msg)
{
    SServerReturnMessage* returnMsg = 0;

    if (msg->Cast(returnMsg))
    {
		/**
			������ �������� �Ϸ� ���¿�����
			��ȯ�� ������ ������ ��ȯ �� ���̹Ƿ�
			���� �Ѵ�.

		 */
		if (m_bLast && m_bCompleted)
		{
			return;
		}

        /**
            �ͼ� �ð��� �Ϸ� �ǰų� ��Ƽ �ı��� 
            �ٷ� ��ȯ ��Ű�� �ʰ� ���� ó�� ��
            ��ȯ ��Ų��.

         */
		m_bTimeOver = (returnMsg->nWhy == GLMSG::STAGE_STATE_GAMEROVER_TIMEOVER);
		m_bFireParty = (returnMsg->nWhy == GLMSG::STAGE_STATE_GAMEROVER_FIREPARTY);
		m_bCompleted = false;

		if (m_bTimeOver || m_bFireParty)
		{
			GLMSG::NET_PVE_STAGE_STATE_FC msg;
			msg.nStage = 0;
			msg.nState = returnMsg->nWhy;
			msg.nTime = 0;
			SendMsgClient(returnMsg->dwCharDbNum, NET_MSG_PVE_STAGE_STATE_FC, msg);

			Run();
		}
    }
}

void CServerPostStageInstance::PostStage()
{
    if (m_bCompleted == false)
    {
        if (m_bTimeOver || m_bFireParty)
        {
			// �ͼ� �ð� ���� or ��Ƽ �ı��� ���� �����
			// ���� ó���� ��û �Ѵ�.
            SServerRewardReqMessage msg;
			msg.eType = m_bTimeOver ? EREWARD_TYPE_STAGE_FAILED_TIMEOVER : EREWARD_TYPE_STAGE_FAILED_FIREPARTY;
            SendMessage(&msg);
        }
        else
        {
            // �Ϲ����� ����(��� ���� or �������� �ð� ����) ��
            // �絵���� ��û �Ѵ�.
            SServerRetryReqMessage msg;
            SendMessage(&msg);
        }
    }
    else
    {
		// �������� ���� �̸�
		// ���� ó���� ��û �Ѵ�.
        SServerRewardReqMessage msg;
        msg.eType = m_bLast ? EREWARD_TYPE_STAGE_COMPLETED : EREWARD_TYPE_STAGE_CLEAR;
        SendMessage(&msg);

		//if (m_bLast == false)
		{
			Exit();
		}
    }
}

void CServerPostStageInstance::FrameMove(float fAccTime, float fElapsedTime)
{
    CServerTriggerInstance::FrameMove(fAccTime, fElapsedTime);

    if (m_fTime > 0.f)
    {
        m_fTime -= fElapsedTime;

        if (m_fTime <= 0.f)
        {
            PostStage();
        }
    }
}

void CServerPostStageInstance::Run()
{
    const CServerPostStageTrigger* pTrigger = 0;

    if (CastTrigger(pTrigger))
    {
        m_fTime = pTrigger->GetTime();
    }	
}
