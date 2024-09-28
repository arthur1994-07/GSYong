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
        m_new �� AddTrigger() �������� iterator �� ���ϹǷ�
        �̸� ���� ���� ���� ���� �Ѵ�.

     */
    if (m_new.empty() == false)
    {
        m_cur.insert(m_cur.end(), m_new.begin(), m_new.end());
        m_new.clear();
    }

    m_fTime += fMaxProcessTime;

    /**
        �ð��� ��� ó������ �ʾ��� ��� ��� ó�� �Ѵ�.
        fMaxProcessTime �� 0�� ���� ���� �۵��ϴ� Ʈ���Ű�
        ���ų� �־ ���� �ð��� ������ Ʈ������ ����̴�.

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
