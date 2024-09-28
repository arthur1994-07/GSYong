#include "../pch.h"
#include "./DayN.h"

#include "../../SigmaCore/Log/LogMan.h"
#include "../../SigmaCore/String/StringFormat.h"
#include "../../SigmaCore/String/StringUtil.h"



DayN::DayN()
:m_N(0)
,m_nTasks(1)	//How many tasks in this attendance (1~4)
,m_status(m_arrTasks[0].GetStatus())
{
}


DayN::~DayN()
{
}


BOOL DayN::SetTaskBasic(UINT iTask, UINT nType, UINT nValueBase)
{
	if (iTask >= m_nTasks || iTask >= MAX_TASKS_PER_ONE_DAY)
	{
		sc::writeLogError(std::string("DayN::SetTask() - Task index exceeds the size of the array."));
		
		return FALSE;
	}

	m_arrTasks[iTask].SetCondtion(iTask, nType, nValueBase);

	return TRUE;
}


BOOL DayN::SetRewardNumeric(UINT iTask, float fExpRate, UINT nPoint, UINT nMoney)
{
	if (iTask >= m_nTasks || iTask >= MAX_TASKS_PER_ONE_DAY)
	{
		sc::writeLogError(std::string("DayN::SetRewardNumeric() - Task index exceeds the size of the array."));
		
		return FALSE;
	}

 	m_arrTasks[iTask].SetRewardExpRate(fExpRate);
	m_arrTasks[iTask].SetRewardPoint(nPoint);
	m_arrTasks[iTask].SetRewardMoney(nMoney);

	return TRUE;
}


BOOL DayN::SetRewardItem(UINT iTask, UINT iItem, WORD wItemMID, WORD wItemSID)
{
	if (iTask >= m_nTasks || iTask >= MAX_TASKS_PER_ONE_DAY)
	{
		sc::writeLogError(std::string("DayN::SetRewardItems() - Task index exceeds the size of the array."));
		
		return FALSE;
	}

	m_arrTasks[iTask].SetRewardItem(iItem, wItemMID, wItemSID);

	return TRUE;
}


void DayN::ComputeRewardExp(LONGLONG exp4LevUp)
{
	for (UINT i=0; i<m_nTasks; i++)
		m_arrTasks[i].ComputeRewardExp(exp4LevUp);
}


DayN& DayN::operator=(const DayN& rhs)
{
	if (this == &rhs)
		return *this;

	m_N			= rhs.m_N;
    if (rhs.m_nTasks > MAX_TASKS_PER_ONE_DAY)
        m_nTasks = MAX_TASKS_PER_ONE_DAY;
    else
	    m_nTasks = rhs.m_nTasks;

	for (UINT i=0; i< m_nTasks; i++)
	{
		m_arrTasks[i] = rhs.m_arrTasks[i];
	}

	return *this;
}