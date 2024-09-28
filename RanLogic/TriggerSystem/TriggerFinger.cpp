#include "pch.h"
#include "TriggerFinger.h"
#include "TriggerSystem.h"
#include "TriggerInstance.h"
#include "Trigger.h"

#include "../SigmaCore/DebugInclude.h"

using namespace TriggerSystem;

CTriggerFinger::CTriggerFinger()
:   m_fTime(0.f)
{
	
}

CTriggerFinger::~CTriggerFinger()
{
	
}

void CTriggerFinger::Start()
{
	m_fTime = 0.f;
}

void CTriggerFinger::Stop()
{
	m_cur.clear();
    m_new.clear();
}

void CTriggerFinger::FrameMove(float fAccTime, float fElapsedTime)
{
    float fMaxProcessTime = 0.f;

	for (TriggerInstanceContainerIterator it = m_cur.begin(); it != m_cur.end(); )
	{
		SpTriggerInstance spTriggerInstance = *it;
		if ( !spTriggerInstance )
		{
			it = m_cur.erase( it );
			continue;
		}

		if (spTriggerInstance->IsExit())
		{
			AddTrigger(spTriggerInstance->GetTrigger()->GetChild());
			it = m_cur.erase( it );
		}
		else
		{
		    spTriggerInstance->FrameMove(fAccTime, fElapsedTime);

            float fProcessTime = spTriggerInstance->GetElapsedTime();

            if (fProcessTime > fMaxProcessTime)
            {
                fMaxProcessTime = fProcessTime;
            }

			++it;
		}
	}

    /**
        m_new 는 AddTrigger() 과정에서 iterator 가 변하므로
        이를 막기 위해 따로 존재 한다.

     */
    if (m_new.empty() == false)
    {
        m_cur.insert(m_cur.end(), m_new.begin(), m_new.end());
        m_new.clear();
    }

    m_fTime += fMaxProcessTime;

    /**
        시간을 모두 처리하지 않았을 경우 계속 처리 한다.
        fMaxProcessTime 가 0일 경우는 현재 작동하는 트리거가
        없거나 있어도 무한 시간을 가지는 트리거일 경우이다.

     */
    if (fMaxProcessTime > 0.f && fElapsedTime > fMaxProcessTime)
    {
        FrameMove(fAccTime, fElapsedTime - fMaxProcessTime);
    }
}

// TAG_PVEClientStartStage_4b_2
void CTriggerFinger::AddTrigger(const CTrigger* pTrigger)
{
	if (pTrigger)
	{
        CTriggerInstance* pInstance = pTrigger->CreateInstance();

        if (pInstance)
        {
			pInstance->SetFinger(this);
			pInstance->SetTrigger(pTrigger);
            pInstance->Start();
			m_new.push_back(SpTriggerInstance(pInstance));

			AddTrigger(pTrigger->GetSibling());
		}
	}
}

void CTriggerFinger::SendMessage(SMessage* msg)
{
	for (TriggerInstanceContainerIterator it = m_cur.begin(); it != m_cur.end(); ++it)
	{
		(*it)->SendMessage(msg);
	}

    for (TriggerInstanceContainerIterator it = m_new.begin(); it != m_new.end(); ++it)
    {
        (*it)->SendMessage(msg);
    }
}

void CTriggerFinger::PostMessage(SpMessage msg)
{
	for (TriggerInstanceContainerIterator it = m_cur.begin(); it != m_cur.end(); ++it)
	{
		(*it)->PostMessage(msg);
	}

    for (TriggerInstanceContainerIterator it = m_new.begin(); it != m_new.end(); ++it)
    {
        (*it)->PostMessage(msg);
    }
}

// TAG_PVEClientStartStage_4b_1
void TriggerSystem::CTriggerFinger::Start(CTrigger* start)
{
    if (start)
    {
        AddTrigger(start);
    }
}
