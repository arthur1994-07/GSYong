#include "pch.h"
#include "TriggerScriptList.h"

using namespace TriggerSystem;

// TAG_InitClientTriggerSystem_3 - "TriggerList" �� ��ϵ� squirrel �� Load �ϱ� ���� ������.
void CTriggerScriptList::ToDo()
{
	Sqrat::Object obj = Sqrat::RootTable(m_vm).GetSlot(STRING_SYMBOL_ROOT_TRIGGERLIST);

	if (obj.IsNull())
	{
		return;
	}

	Sqrat::Array& arr = obj.Cast<Sqrat::Array>();

	if (arr.IsNull())
	{
		return;
	}

	std::vector<TSTRING> triggers;

	Sqrat::GetVectorFromArray(arr, &triggers);

	std::vector<TSTRING>::iterator it = triggers.begin();

	for ( ; it != triggers.end(); ++it)
	{
		Load(*it);
	}
}