#include "pch.h"
#include "ClientRetryInstance.h"
#include "ClientRetryTrigger.h"
#include "ClientTriggerFinger.h"
#include "ClientStageMessage.h"

#include "../GLGaeaClient.h"
#include "../Char/GLCharacter.h"

#include "../../RanLogic/TriggerSystem/TriggerScript.h"
#include "../../RanLogic/TriggerSystem/TriggerInstance.h"

#include "../../SigmaCore/DebugInclude.h"

using namespace TriggerSystem;

CClientRetryInstance::CClientRetryInstance()
:   m_time(0)
{

}

CClientRetryInstance::~CClientRetryInstance()
{
	Retry(false);
}

void CClientRetryInstance::Stage(SMessage* msg)
{
    SClientStageMessage* stage = 0;

    if (msg->Cast(stage))
    {
        if (stage->nState == GLMSG::STAGE_STATE_RETRY)
        {
			m_time = stage->nTime;
			Retry(true);
        }
		else
		{
			Retry(false);
		}
    }
}

void CClientRetryInstance::Retry(bool bEnable)
{
	CClientTriggerFinger* finger = 0;
	const CClientRetryTrigger* trigger = 0;

	if (CastFinger(finger) && CastTrigger(trigger))
	{
		GLGaeaClient* pGaeaClient = finger->GetGaeaClient();		

		if (pGaeaClient)
		{
			GLCharacter* pChar = pGaeaClient->GetCharacter();

			if( pChar )
			{
				GLWidgetScript::GetInstance().LuaCallFunc< void >( 
					NSWIDGET_SCRIPT::g_strFunc_PVE_PVERetry, 
					"-l, -n, -b"
					,trigger->RetryMoney(pChar->GetLevel(), finger->GetStage()), m_time, bEnable );
			}
		}
	}
}
