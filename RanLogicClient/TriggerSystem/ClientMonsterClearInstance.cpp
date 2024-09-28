#include "pch.h"
#include "ClientMonsterClearInstance.h"
#include "ClientMonsterClearTrigger.h"
#include "ClientTriggerFinger.h"
#include "ClientMonsterClearMessage.h"
#include "ClientStageMessage.h"

#include "../GLGaeaClient.h"

#include "../../RanLogic/TriggerSystem/TriggerScript.h"
#include "../../RanLogic/TriggerSystem/TriggerInstance.h"

#include "../../SigmaCore/DebugInclude.h"

#include "../../RanLogicClient/Widget/GLWidgetScript.h"

using namespace TriggerSystem;

CClientMonsterClearInstance::CClientMonsterClearInstance()
:   m_cur(0)
,   m_all(0)
{

}

CClientMonsterClearInstance::~CClientMonsterClearInstance()
{
    Update(false);
}

void CClientMonsterClearInstance::Stage(SMessage* msg)
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
			if ( ( m_cur == 0 ) && ( m_all == 0 ) )
				return;

			Update( true );
		}
    }
}

void CClientMonsterClearInstance::MonsterClear(SMessage* msg)
{
    SClientMonsterClearMessage* clear = 0;

    if (msg->Cast(clear))
    {
        m_cur = clear->nCur;
        m_all = clear->nAll;

        Update( m_cur != m_all );
    }
}

void CClientMonsterClearInstance::Update(bool bEnable)
{
	GLWidgetScript::GetInstance().LuaCallFunc< void >( 
		NSWIDGET_SCRIPT::g_strFunc_PVE_MonsterIndicator, 
		"-n, -n, -b"
		, m_cur, m_all, bEnable );
}


