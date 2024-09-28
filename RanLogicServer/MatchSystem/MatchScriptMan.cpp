#include "pch.h"

#include "./MatchScriptMan.h"

#include "../../SigmaCore/Log/LogMan.h"
#include "../../SigmaCore/String/StringFormat.h"
#include "../../SigmaCore/String/StringUtil.h"
#include "../../SigmaCore/gassert.h"
#include "../../enginelib/DxLand/DxLandDef.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


using namespace Sqrat;


MatchScriptMan::MatchScriptMan()
:m_ConfigTable(m_vm)
{
}

MatchScriptMan::~MatchScriptMan()
{
}

void MatchScriptMan::ToDo()
{
	m_ConfigTable = RootTable(m_vm).GetSlot("Config"); // Via the class or table

	if (m_ConfigTable.IsNull())
	{
		/* function not retrieved. handle appropriately */

		sc::writeLogError(std::string("MatchScriptMan::ToDo() - ConfigTable is NULL."));
	}

	m_IsInstanceMap	= RootTable(m_vm).GetFunction("IsInstanceMap"); // Via the class or table

	if (m_IsInstanceMap.IsNull())
	{
		/* function not retrieved. handle appropriately */

		sc::writeLogError(std::string("MatchScriptMan::ToDo() - IsInstanceMap is NULL."));
	}

	m_IsPvPMap	= RootTable(m_vm).GetFunction("IsPvPMap"); // Via the class or table

	if (m_IsPvPMap.IsNull())
	{
		/* function not retrieved. handle appropriately */

		sc::writeLogError(std::string("MatchScriptMan::ToDo() - m_IsPvPMap is NULL."));
	}

	m_GetExpireM	= RootTable(m_vm).GetFunction("GetExpireM"); // Via the class or table

	if (m_GetExpireM.IsNull())
	{
		/* function not retrieved. handle appropriately */

		sc::writeLogError(std::string("MatchScriptMan::ToDo() - GetExpireM is NULL."));
	}	
	
	sc::writeLogInfo(std::string("MatchScriptMan::ToDo() done."));
}

DWORD MatchScriptMan::IsInstanceMap(WORD mid, WORD sid)
{
    GASSERT(mid >= 0 && mid < MAX_LAND_MID);
    GASSERT(sid >= 0 && sid < MAX_LAND_SID);

	if (m_IsInstanceMap.IsNull())
	{
		sc::writeLogError(std::string("MatchScriptMan::IsInstanceMap() - m_IsInstanceMap is NULL."));

		return 0;
	}
	
	DWORD dwTO = m_IsInstanceMap.Evaluate<DWORD>(mid, sid);

	return dwTO;
}

BOOL MatchScriptMan::IsPvPMap(WORD mid, WORD sid)
{
    GASSERT(mid >= 0 && mid < MAX_LAND_MID);
    GASSERT(sid >= 0 && sid < MAX_LAND_SID);

	if (m_IsPvPMap.IsNull())
	{
		sc::writeLogError(std::string("MatchScriptMan::IsInstanceMap() - m_IsPvPMap is NULL."));

		return 0;
	}
	
	BOOL bPvP = m_IsPvPMap.Evaluate<BOOL>(mid, sid);

	return bPvP;
}

DWORD MatchScriptMan::GetExpireM(WORD mid, WORD sid)
{
    GASSERT(mid >= 0 && mid < MAX_LAND_MID);
    GASSERT(sid >= 0 && sid < MAX_LAND_SID);

	if (m_GetExpireM.IsNull())
	{
		sc::writeLogError(std::string("MatchScriptMan::GetExpireM() - m_IsInstanceMap is NULL."));

		return 0;
	}
	
	DWORD dwExpireM = m_GetExpireM.Evaluate<DWORD>(mid, sid);

	return dwExpireM;
}


