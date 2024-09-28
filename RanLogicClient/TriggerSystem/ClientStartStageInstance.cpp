#include "pch.h"
#include "ClientStartStageInstance.h"
#include "ClientTriggerFinger.h"
#include "ClientStartStageTrigger.h"
#include "ClientStartStageMessage.h"

#include "../GLGaeaClient.h"

#include "../../RanLogic/TriggerSystem/TriggerScript.h"
#include "../../RanLogic/TriggerSystem/TriggerInstance.h"

#include "../../RanLogicClient/Widget/GLWidgetScript.h"

#include "../../SigmaCore/DebugInclude.h"

using namespace TriggerSystem;

CClientStartStageInstance::~CClientStartStageInstance()
{
    GLGaeaClient* pGaeaClient = GetGaeaClient();

    if (pGaeaClient)
    {
		GLWidgetScript::GetInstance().LuaCallFunc< void >( 
			NSWIDGET_SCRIPT::g_strFunc_PVE_Stage, 
			"-n, -b"
			,0 , false );
    }
}

void CClientStartStageInstance::StartStage(SMessage* msg)
{
    SClientStartStageMessage* start = 0;

    if (msg->Cast(start))
    {        
        m_time = start->nTime;
        Update();
    }
}

void CClientStartStageInstance::FrameMove(float fAccTime, float fElapsedTime)
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

void CClientStartStageInstance::Update(bool bEnable)
{
    CClientTriggerFinger* pFinger = 0;

    if (CastFinger(pFinger))
    {
        GLGaeaClient* pGaeaClient = pFinger->GetGaeaClient();

        if (pGaeaClient)
        {
			GLWidgetScript::GetInstance().LuaCallFunc< void >( 
				NSWIDGET_SCRIPT::g_strFunc_PVE_Stage, 
				"-n, -b"
				,pFinger->GetStage() , bEnable );
        }
    }
}
