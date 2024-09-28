#include "../pch.h"

#include "./CaptureTheFieldScriptMan.h"

#include "../../SigmaCore/Log/LogMan.h"
#include "../../SigmaCore/String/StringFormat.h"
#include "../../SigmaCore/String/StringUtil.h"
#include "../../SigmaCore/gassert.h"



// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


using namespace Sqrat;


CaptureTheFieldScriptMan::CaptureTheFieldScriptMan()
:m_ConfigTable(m_vm)
{
	
}


CaptureTheFieldScriptMan::~CaptureTheFieldScriptMan()
{
}


void CaptureTheFieldScriptMan::ToDo()
{
	m_ConfigTable = RootTable(m_vm).GetSlot("Config"); // Via the class or table

	if (m_ConfigTable.IsNull())
	{
		/* function not retrieved. handle appropriately */

		sc::writeLogError(std::string("CaptureTheFieldScriptMan::ToDo() - ConfigTable is NULL."));
	}
	else
	{
		sc::writeLogInfo(std::string("CaptureTheFieldScriptMan::ToDo() done."));
	}
}


