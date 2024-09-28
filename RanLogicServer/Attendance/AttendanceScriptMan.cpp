#include "../pch.h"

#include "./AttendanceScriptMan.h"

#include "../../SigmaCore/Log/LogMan.h"
#include "../../SigmaCore/String/StringFormat.h"
#include "../../SigmaCore/String/StringUtil.h"
#include "../../SigmaCore/gassert.h"



// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


using namespace Sqrat;




AttendanceScriptMan::AttendanceScriptMan()
//:m_setUpDayN(RootTable(m_vm),"SetUpDayN")
{
	//SquirrelMan();

	ClassType<DayN>::Initialized() = false;
	Class<DayN> dayN(m_vm);
	
	dayN.Func("SetN",				&DayN::SetN);
	dayN.Func("SetNumOfTasks",		&DayN::SetNumOfTasks);
	dayN.Func("SetTaskBasic",		&DayN::SetTaskBasic);
	dayN.Func("SetRewardNumeric",	&DayN::SetRewardNumeric);
	dayN.Func("SetRewardItem",		&DayN::SetRewardItem);

		
	RootTable(m_vm).Bind("DayN", dayN);	
}


AttendanceScriptMan::~AttendanceScriptMan()
{
}


void AttendanceScriptMan::ToDo()
{
	m_setUpDayN	= RootTable(m_vm).GetFunction("SetUpDayN"); // Via the class or table

	if (m_setUpDayN.IsNull())
	{
		/* function not retrieved. handle appropriately */

		sc::writeLogError(std::string("AttendanceScriptMan::ToDo() - SetUpDayN is NULL."));
	}
	else
	{
		sc::writeLogInfo(std::string("AttendanceScriptMan::ToDo() done."));
	}
}


DayN AttendanceScriptMan::SetDayN(UINT nDay/*0~6*/, UINT nChaLev/*character's level*/, UINT nMaxLevel/*MaxLevel*/)
{
	//sc::writeLogInfo(std::string("Debug - SetDayN() begins."));

	//GASSERT(nDay >= 0);
	//GASSERT(nChaLev >= 0);
	//GASSERT(nMaxLevel >= 0);

	if (m_setUpDayN.IsNull())
	{
		sc::writeLogError(std::string("AttendanceScriptMan::SetDayN()::m_setUpDayN is NULL."));

        return DayN();
	}
	/*else
	{
		sc::writeLogInfo(std::string("AttendanceScriptMan::m_setUpDayN is OK."));
	}*/


	//DayN *pDayN = setUpDayN.Evaluate<DayN *>(nDay, nChaLev, nMaxLevel);
	DayN dayn = m_setUpDayN.Evaluate<DayN>(nDay, nChaLev, nMaxLevel);

	//return pDayN;

	//sc::writeLogInfo(std::string("Debug - SetDayN() ends."));
	return dayn;
}

