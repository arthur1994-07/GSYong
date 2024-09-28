#include "pch.h"
#include "ClientStageInstance.h"
#include "ClientStageTrigger.h"
#include "ClientTriggerFinger.h"
#include "ClientStageMessage.h"
#include "ClientActionMessage.h"
#include "ClientRebirthMessage.h"

#include "../GLGaeaClient.h"
#include "../Char/GLCharacter.h"

#include "../../RanLogic/TriggerSystem/TriggerScript.h"
#include "../../RanLogic/TriggerSystem/TriggerInstance.h"

#include "../../SigmaCore/DebugInclude.h"

using namespace TriggerSystem;

CClientStageInstance::CClientStageInstance()
:   m_stage(0)
,   m_time(0)
,   m_visible(false)
{

}

CClientStageInstance::~CClientStageInstance()
{
    Update(false);
	StageState(GLMSG::STAGE_STATE_DIED, false);
}

void CClientStageInstance::Stage(SMessage* msg)
{
    SClientStageMessage* stage = 0;
	CClientTriggerFinger* finger = 0;

    if (msg->Cast(stage) && CastFinger(finger))
    {
        m_visible = stage->nState == GLMSG::STAGE_STATE_ONGOING;

		StageState(stage->nState, !m_visible);

		m_stage = stage->nStage;
        m_time = static_cast<float>(stage->nTime);

		Update( m_visible );
    }
}

void CClientStageInstance::Action(SMessage* msg)
{
	if (m_visible == false)
	{
		return;
	}

	SClientActionMessage* act = 0;
	CClientTriggerFinger* finger = 0;

	if (msg->Cast(act) && CastFinger(finger))
	{
		GLGaeaClient* pGaeaClient = finger->GetGaeaClient();

		if (pGaeaClient == 0)
		{
			return;
		}

		GLCharacter* pChar = pGaeaClient->GetCharacter();

		if (pChar == 0 || pChar->GetCharID() != act->dwCharDbNum)
		{
			return;
		}

		if (m_visible == false)
		{
			return;
		}

		if (act->eNextAction == GLAT_DIE)
		{
			StageState(GLMSG::STAGE_STATE_DIED, true);
		}
		else if (act->ePreAction == GLAT_DIE)
		{
			StageState(GLMSG::STAGE_STATE_DIED, false);
		}
	}
}

void CClientStageInstance::Rebirth(SMessage* msg)
{
	if (m_visible == false)
	{
		return;
	}

	SClientRebirthMessage* rebirth = 0;
	CClientTriggerFinger* finger = 0;

	if (msg->Cast(rebirth) && CastFinger(finger))
	{
		StageState(GLMSG::STAGE_STATE_DIED, false);
	}
}

void CClientStageInstance::Update(bool bEnable)
{
    CClientTriggerFinger* pFinger = 0;
    const CClientStageTrigger* pTrigger = 0;

    if (CastFinger(pFinger) && CastTrigger(pTrigger))
    {
        GLGaeaClient* pGaeaClient = pFinger->GetGaeaClient();

        if (pGaeaClient)
        {
            int nStage = m_stage;
            int nEnd = pTrigger->GetEndStage();
            int nRange = pTrigger->GetRange();
            int nTime = static_cast<int>( ceil(m_time) );
			bool bBlank = nTime > 0 && nTime <= 10;

			GLWidgetScript::GetInstance().LuaCallFunc< void >( 
				NSWIDGET_SCRIPT::g_strFunc_PVE_StageIndicator, 
				"-n, -n, -n, -n, -b, -b"
				, nStage, nEnd, nRange, nTime, bBlank, bEnable );
        }
    }
}

void CClientStageInstance::StageState(int nStage, bool bEnable)
{
	CClientTriggerFinger* finger = 0;

	if (CastFinger(finger) == false)
	{
		return;
	}

	GLWidgetScript::GetInstance().LuaCallFunc< void >( 
		NSWIDGET_SCRIPT::g_strFunc_PVE_StageState, 
		"-n, -b"
		, nStage, bEnable );
}

void CClientStageInstance::FrameMove(float fAccTime, float fElapsedTime)
{
    CClientTriggerInstance::FrameMove(fAccTime, fElapsedTime);

	/*int nBeforTime = (int)m_time;
    if (m_time > 0.f)
    {
        m_time -= fElapsedTime;

        if (m_time < 0.f)
        {
            m_time = 0.f;
        }
	}*/

	// ¶«»§ ÄÚµå
	// ÃÊ°¡ ¹Ù²î¸é ÇÑ¹ø¾¿ È£ÃâÇÑ´Ù.
	/*if( nBeforTime != (int)m_time )
		Update(m_visible);*/
}
