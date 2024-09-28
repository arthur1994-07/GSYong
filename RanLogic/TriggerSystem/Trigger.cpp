#include "pch.h"
#include "Trigger.h"
#include "TriggerScript.h"
#include "Message.h"
#include "TriggerInstance.h"

#include "../SigmaCore/DebugInclude.h"

using namespace TriggerSystem;

CTrigger::CTrigger() :
	m_sibling(0)
,	m_child(0)
,   m_attach(0)
,	m_id(-1)
,   m_lifetime(0)
{

}

// TAG_InitClientTriggerSystem_7a_4
void CTrigger::Bind(CTriggerScript* pScript)
{
	if (pScript)
	{
		pScript->Func(_T("InsertSibling"), &CTrigger::InsertSibling);
		pScript->Func(_T("InsertChild"), &CTrigger::InsertChild);
		pScript->Func(_T("SetID"), &CTrigger::SetID);
        pScript->Func(_T("SetLifeTime"), &CTrigger::SetLifeTime);
        pScript->Func(_T("SetAttach"), &CTrigger::SetAttach);
	}
}

void CTrigger::InsertSibling(CTrigger* pTrigger)
{
	if (m_sibling)
	{
		m_sibling->InsertSibling(pTrigger);
	}
	else
	{
		m_sibling = pTrigger;
	}
}

void CTrigger::InsertChild(CTrigger* pTrigger)
{
	if (m_child)
	{
		m_child->InsertSibling(pTrigger);
	}
	else
	{
		m_child = pTrigger;
	}
}

// TAG_PVEClientStartStage_4a_2
// TAG_PVEClientStartStage_4a_4
void CTrigger::SendMessage(SSystemMessage* msg)
{
	if (msg)
	{
		msg->Command(this);
	}

	if (m_sibling)
	{
		m_sibling->SendMessage(msg);
	}

	if (m_child)
	{
		m_child->SendMessage(msg);
	}
}

CTriggerInstance* TriggerSystem::CTrigger::CreateInstance() const
{
    // 기본적인 인스턴스를 생성 한다.
    return new CTriggerInstance();
}
