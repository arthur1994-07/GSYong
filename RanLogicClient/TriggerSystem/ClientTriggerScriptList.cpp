#include "pch.h"
#include "ClientTriggerScriptList.h"
#include "ClientTriggerSystem.h"

#include "../../SigmaCore/DebugInclude.h"

using namespace TriggerSystem;

// TAG_InitClientTriggerSystem_4
void CClientTriggerScriptList::Load(const TSTRING& file)
{
	CreateClientScript(file);
}
