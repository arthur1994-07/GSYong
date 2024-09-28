#include "pch.h"
#include "TriggerInstance.h"
#include "Trigger.h"
#include "ExitMessage.h"
#include "TriggerFinger.h"

#include "../SigmaCore/DebugInclude.h"

using namespace TriggerSystem;

CTriggerInstance::CTriggerInstance() :	
	m_fElapsedTime(0.f)
,	m_fTime(0.f)
,	m_state(TRIGGER_STATE_RUN)
,	m_pFinger(0)
,	m_pTrigger(0)
,   m_fLifeTime(0.f)
{
	
}

CTriggerInstance::~CTriggerInstance()
{

}

bool CTriggerInstance::IsTriggerType(TriggerType type)
{
	if (m_pTrigger)
	{
		return m_pTrigger->IsType(type);
	}

	return false;
}

void CTriggerInstance::ProcessTime(float fElapsedTime)
{
    float fLifetime= GetLifeTime();
    float fProcessTime = fElapsedTime;
    float fCurTime = GetTime();

    if (fCurTime + fProcessTime > fLifetime)
    {
        fProcessTime = fLifetime - fCurTime;
    }

    SetTime(m_fTime + fProcessTime);
    SetElapsedTime(fProcessTime);
}

void CTriggerInstance::FrameMove(float fAccTime, float fElapsedTime)
{
	assert(IsState(TRIGGER_STATE_END) == false && _T("종료 상태 일 떄는 이곳을 들어오면 절대 안된다."));

    ProcessTime(fElapsedTime);

	for (MessageContainerIterator it = m_msgs.begin(); it != m_msgs.end(); )
	{
		MessageProc((*it).get());
		m_msgs.erase(it++);
	}

	if (m_pTrigger)
	{
		m_pTrigger->FrameMove(this);

        if (IsTimeOver(m_fTime))
        {
            Exit();
        }
    }
}

void CTriggerInstance::MessageProc(SMessage* msg)
{
	if (msg && m_pTrigger)
	{
		msg->Command(this);
	}
}

void CTriggerInstance::SendMessage(SMessage* msg)
{
	MessageProc(msg);
}

void CTriggerInstance::PostMessage(SpMessage msg)
{
	m_msgs.push_back(msg);
}

// TAG_PVEClientStartStage_4b_3
void CTriggerInstance::Start()
{
    if (m_pTrigger)
    {
        m_pTrigger->Start(this);
        SetLifeTime(m_pTrigger->GetLifeTime());
    }
}

void CTriggerInstance::Exit()
{
    if (m_pTrigger)
    {
        m_pTrigger->Exit(this);

        if (m_pFinger)
        {
            // 삭제 됨을 알린다.
            SExitMessage msg;
            msg.trigger = m_pTrigger;
            m_pFinger->SendMessage(&msg);
        }
    }

    // 종료 상태로 만듬으로써 
    // 삭제 처리가 수행 된다.
    SetState(TRIGGER_STATE_END);
}

bool CTriggerInstance::IsExit()
{
    return IsState(TRIGGER_STATE_END);
}

bool CTriggerInstance::IsTimeOver(const float fTime) const
{
    if (m_fLifeTime > 0 && fTime >= m_fLifeTime)
    {
        return true;
    }

    return false;
}
