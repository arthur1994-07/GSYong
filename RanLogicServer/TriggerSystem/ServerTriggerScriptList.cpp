#include "pch.h"
#include "ServerTriggerScriptList.h"
#include "ServerTriggerSystem.h"

#include "../SigmaCore/DebugInclude.h"

using namespace TriggerSystem;

void CServerTriggerScriptList::Load(const TSTRING& file)
{
	CreateServerScript(file);
}
