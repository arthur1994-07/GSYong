#include "pch.h"
#include "ClientMsgInstance.h"
#include "ClientTriggerFinger.h"
#include "ClientMsgTrigger.h"
#include "ClientMsgMessage.h"
#include "ClientStageMessage.h"

#include "../GLGaeaClient.h"

#include "../../RanLogic/TriggerSystem/TriggerScript.h"
#include "../../RanLogic/TriggerSystem/TriggerInstance.h"

#include "../../SigmaCore/DebugInclude.h"

#include "../../RanLogicClient/Widget/GLWidgetScript.h"

using namespace TriggerSystem;

CClientMsgInstance::~CClientMsgInstance()
{
	Update(false);
}

void CClientMsgInstance::Message(SMessage* msg)
{
    SClientMsgMessage* message = 0;

    if (msg->Cast(message))
    {
        m_msg = message->strMsg;
        m_time = static_cast<float>(message->nTime);
		Update();
    }
}

void CClientMsgInstance::Stage(SMessage* msg)
{
	SClientStageMessage* stage = 0;

	if (msg->Cast(stage))
	{
		if (stage->nState != GLMSG::STAGE_STATE_ONGOING)
		{
			Update(false);
		}
	}
}

void CClientMsgInstance::FrameMove(float fAccTime, float fElapsedTime)
{
    CClientTriggerInstance::FrameMove(fAccTime, fElapsedTime);

    if (m_time > 0.f)
    {
        m_time -= fElapsedTime;

        if (m_time < 0.f)
        {
            m_time = 0.f;
        }

        if (m_time == 0.f)
        {
            Update(false);
        }
    }
}

void CClientMsgInstance::Update(bool bEnable)
{
    GLGaeaClient* pGaeaClient = GetGaeaClient();

	GLWidgetScript::GetInstance().LuaCallFunc< void >( 
		NSWIDGET_SCRIPT::g_strFunc_PVE_Message, 
		"-s, -b"
		, m_msg.c_str(), bEnable && m_time > 0 );
}