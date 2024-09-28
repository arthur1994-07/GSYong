#include "pch.h"

#include "./GroupChatScriptMan.h"

#include "../../SigmaCore/Log/LogMan.h"
#include "../../SigmaCore/String/StringFormat.h"
#include "../../SigmaCore/String/StringUtil.h"
#include "../../SigmaCore/gassert.h"
#include "../../enginelib/DxLand/DxLandDef.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


using namespace Sqrat;


ChatScriptMan::ChatScriptMan()
:m_ConfigTable(m_vm)
{
	
}


ChatScriptMan::~ChatScriptMan()
{
}


void ChatScriptMan::ToDo()
{
	m_ConfigTable = RootTable(m_vm).GetSlot("Config"); // Via the class or table

	if (m_ConfigTable.IsNull())
	{
		/* function not retrieved. handle appropriately */

		sc::writeLogError(std::string("ChatScriptMan::ToDo() - ConfigTable is NULL."));
	}	
	
	sc::writeLogInfo(std::string("ChatScriptMan::ToDo() done."));
}
