#include "pch.h"
#include "ClientPositioningClearInstance.h"
#include "ClientPositioningClearTrigger.h"
#include "ClientTriggerFinger.h"
#include "ClientPositioningClearMessage.h"
#include "ClientStageMessage.h"

#include "../GLGaeaClient.h"

#include "../../RanLogic/TriggerSystem/TriggerScript.h"
#include "../../RanLogic/TriggerSystem/TriggerInstance.h"

#include "../../SigmaCore/DebugInclude.h"

#include "../../RanLogicClient/Widget/GLWidgetScript.h"

using namespace TriggerSystem;

CClientPositioningClearInstance::CClientPositioningClearInstance()
:   m_x(0)
,   m_y(0)
,	m_nNow(0)
,	m_nNeed(0)
{
    
}

CClientPositioningClearInstance::~CClientPositioningClearInstance()
{
    Update(false);
}

void CClientPositioningClearInstance::Stage(SMessage* msg)
{
    SClientStageMessage* stage = 0;

    if (msg->Cast(stage))
    {
        if (stage->nState != GLMSG::STAGE_STATE_ONGOING)
        {
            Update(false);
        }
		else
		{
			if ( ( m_nNow == 0 ) && ( m_nNeed == 0 ) )
				return;

			Update( true );
		}
    }
}

void CClientPositioningClearInstance::PositioningClear(SMessage* msg)
{
    SClientPositioningClearMessage* clear = 0;

    if (msg->Cast(clear))
    {
        m_x = clear->nX;
        m_y = clear->nY;

        m_nNow = clear->nNow;
        m_nNeed = clear->nNeed;

        Update(clear->bCompleted == false);
    }
}

void CClientPositioningClearInstance::Update(bool bEnable)
{
	GLWidgetScript::GetInstance().LuaCallFunc< void >( 
		NSWIDGET_SCRIPT::g_strFunc_PVE_PositioningIndicator, 
		"-n, -n, -n, -n, -b"
		, m_x, m_y, m_nNow, m_nNeed, bEnable );
}


