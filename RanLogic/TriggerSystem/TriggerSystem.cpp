#include "pch.h"
#include "TriggerSystem.h"
#include "TriggerScript.h"
#include "Trigger.h"

#include "../SigmaCore/DebugInclude.h"
#include "../SigmaCore/String/StringUtil.h"

using namespace TriggerSystem;

static TriggerScriptContainer s_scripts;

bool TriggerSystem::InitTriggerSystem()
{
	return true;
}

void TriggerSystem::DestroyTriggerSystem()
{
	s_scripts.clear();
}

// TAG_PVEClientStartStage_4a_1
void TriggerSystem::SendMessage(SSystemMessage* msg)
{
	for (TriggerScriptContainerIterator it = s_scripts.begin(); it != s_scripts.end(); ++it)
	{
		CTrigger* pTrigger = it->second->Find(STRING_SYMBOL_ROOT_TRIGGER);

		if (pTrigger)
		{
			pTrigger->SendMessage(msg);
		}
	}
}

// TAG_InitClientTriggerSystem_7b_3
void TriggerSystem::InsertTrigger(const TCHAR* pFile, SpTriggerScript script)
{
    TSTRING file;
    sc::string::getFullFileName(pFile, file);

	if (file.empty() == false && script)
	{
		s_scripts[file] = script;
	}
}

// TAG_PVEClientStartStage_4a_6
CTrigger* TriggerSystem::FindTrigger(const TCHAR* pFile, const TCHAR* pRoot)
{
	TriggerScriptContainerIterator it = s_scripts.find(pFile);

	if (it != s_scripts.end())
	{
		return it->second->Find(pRoot);
	}

	return 0;
}

bool TriggerSystem::FindScript(const TCHAR* pFile)
{
    return s_scripts.find(pFile) != s_scripts.end();
}
