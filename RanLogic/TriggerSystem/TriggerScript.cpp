#include "pch.h"
#include "TriggerScript.h"
#include "Trigger.h"
#include "sqstate.h"
#include "sqvm.h"
#include "sqratimport.h"

#include "../SigmaCore/DebugInclude.h"

using namespace TriggerSystem;

CTriggerScript::CTriggerScript()
{
    sqrat_register_importlib(m_vm);
}

CTriggerScript::~CTriggerScript()
{

}

// TAG_PVEClientStartStage_4a_7
CTrigger* CTriggerScript::Find(const TCHAR* pName)
{
	Sqrat::Object obj = Sqrat::RootTable(m_vm).GetSlot(pName);

	if (obj.IsNull() == false)
	{
		return obj.Cast<CTrigger*>();
	}

	return 0;
}
