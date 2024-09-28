#include "pch.h"
#include "ServerTriggerScript.h"
#include "ServerTriggerDecleare.h"

#include "../SigmaCore/DebugInclude.h"

using namespace TriggerSystem;

void CServerTriggerScript::Init()
{
	TriggerCreator creator;

	for (TriggerType i = TRIGGER_TYPE_MSG; i < TRIGGER_TYPE_QUANTITY; ++i)
	{
		SpTrigger spTriger(creator.Create<CTrigger>(i));

		if (spTriger)
		{
			spTriger->Bind(this);
		}
	}
}
