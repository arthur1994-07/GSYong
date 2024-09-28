#pragma once
//
//mjeon.attendance
//
#include "../../RanLogic/Script/SquirrelMan.h"
#include "../../RanLogic/Attendance/DayN.h"


//
// Squirrel Manager. MUST be constructed only once.
//
class AttendanceScriptMan : public SquirrelMan
{
public:
	AttendanceScriptMan();
	virtual ~AttendanceScriptMan();

	//virtual DayN* SetDayN(UINT nDay/* 0~6 */, UINT nChaLev, UINT nMaxLevel);
	virtual DayN SetDayN(UINT nDay/* 0~6 */, UINT nChaLev, UINT nMaxLevel);

	virtual void ToDo();

	Sqrat::Function m_setUpDayN;

private:
};
