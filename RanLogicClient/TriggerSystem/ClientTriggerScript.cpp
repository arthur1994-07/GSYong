#include "pch.h"
#include "ClientTriggerScript.h"
#include "ClientTriggerDecleare.h"

#include "../../SigmaCore/DebugInclude.h"

using namespace TriggerSystem;

// TAG_InitClientTriggerSystem_7a_1 - Trigger들을 만들고 Bind 한다.
void CClientTriggerScript::Init()
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
