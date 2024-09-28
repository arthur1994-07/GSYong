#include "pch.h"
#include "ClientReadyInstance.h"
#include "ClientReadyTrigger.h"
#include "ClientTriggerFinger.h"
#include "ClientReadyMessage.h"
#include "ClientStageMessage.h"

#include "../GLGaeaClient.h"

#include "../../RanLogic/RANPARAM.h"
#include "../../RanLogic/TriggerSystem/TriggerScript.h"
#include "../../RanLogic/TriggerSystem/TriggerInstance.h"

#include "../../SigmaCore/DebugInclude.h"

#include "../../RanLogicClient/Widget/GLWidgetScript.h"


using namespace TriggerSystem;

CClientReadyInstance::CClientReadyInstance()
:   m_cur(0)
,   m_all(0)
,   m_time(0)
,	m_showtip(false)
{
	
}

CClientReadyInstance::~CClientReadyInstance()
{
	ShowTip(m_showtip);
    Update(false);
}

void CClientReadyInstance::Start()
{
	m_showtip = RANPARAM::bSHOW_TIP;
	CClientTriggerInstance::Start();
}

void CClientReadyInstance::Ready(SMessage* msg)
{
    SClientReadyMessage* ready = 0;

    if (msg->Cast(ready))
    {
        m_cur = ready->nCur;
        m_all = ready->nAll;
        m_time = float(ready->nTime);

        Update(m_time != 0.f && m_cur <= m_all);        
    }
}

void CClientReadyInstance::Stage(SMessage* msg)
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

void CClientReadyInstance::Update(bool bEnable)
{
    GLGaeaClient* pGaeaClient = GetGaeaClient();

    if (pGaeaClient)
    {
        int nCur = m_cur;
        int nAll = m_all;
        int nTime = static_cast<int>( ceil(m_time) );

		GLWidgetScript::GetInstance().LuaCallFunc< void >( 
			NSWIDGET_SCRIPT::g_strFunc_PVE_EntranceState, 
			"-n, -n, -n, -b"
			, nCur, nAll, nTime, bEnable );
    }

	if (bEnable == false)
	{
		m_time = 0.f;
	}
}

void CClientReadyInstance::FrameMove(float fAccTime, float fElapsedTime)
{
    CClientTriggerInstance::FrameMove(fAccTime, fElapsedTime);

    if (m_time > 0.f)
    {
		int nTimeTemp1 = m_time;
        m_time -= fElapsedTime;

		int nTimeTemp2 = m_time;

        if (m_time <= 0.f)
        {
            m_time = 0.f;
        }

		if( nTimeTemp1 != nTimeTemp2 )
			Update(true);
    }

	ShowTip(false);	
}

void CClientReadyInstance::ShowTip(bool bEnable)
{
	RANPARAM::bSHOW_TIP = bEnable;	
}
