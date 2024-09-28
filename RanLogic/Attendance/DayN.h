#pragma once
//
//mjeon.attendance
//

#include "../../=Squirrel/include/sqrat/sqratClassType.h"
#include "./TaskN.h"


/*

Attendance Manager
	|
	+- DayN	(Day01, Day02, ... , Day07)



DayN
	|
	+- Task01, Task02, Task03	



TaskN
	|
	+- Condition01  (only 1 condition)

	

Condition :	(Type, ActionCount)	 ex> (ATTENDANCE_TYPE_SPARRING, Count)	: Type includes the target information	

*/


//
//Day1 ~ Day7
//
class DayN : public Sqrat::SqObjectType
{
public:	
	DayN();
	DayN(UINT dayN);
	virtual ~DayN();
	

	DayN& operator=(const DayN &rhs);


	void SetN(UINT N)
	{
		m_N = N;
	}

	UINT GetN()
	{
		return m_N;
	}

	void SetNumOfTasks(UINT n)
	{
        if (n > MAX_TASKS_PER_ONE_DAY)
		    m_nTasks = MAX_TASKS_PER_ONE_DAY;
        else
            m_nTasks = n;
	}

	UINT GetNumOfTasks()
	{
		return m_nTasks;
	}

	ENUM_ATTENDANCE_STATUS GetStatus()
	{
		return m_arrTasks[0].GetStatus();
	}


	//
	// SET-methods called by script
	//
	BOOL SetTaskBasic(UINT iTask, UINT nType, UINT nValueBase);
	BOOL SetRewardNumeric(UINT iTask, float fExpRate, UINT nPoint, UINT nMoney);	
	BOOL SetRewardItem(UINT iTask, UINT iItem, WORD wItemMID, WORD wItemSID);

	void ComputeRewardExp(LONGLONG exp4LevUp);

	
protected:

	UINT					m_N;			// Day N (0~6)

	UINT					m_nTasks;		// How many tasks does this DayN include?

	ENUM_ATTENDANCE_STATUS	&m_status;
	
	TaskN					m_arrTasks[MAX_TASKS_PER_ONE_DAY];

	DWORD					m_UIUpdatedTime;

private:
	friend class boost::serialization::access;
	template <typename Archive>
	void serialize(Archive &ar, const unsigned int version)
	{
		ar & m_N;
		ar & m_nTasks;

		//ar & m_status;	//복사된 DayN에서 해당 Task1의 status를 참조하고 있을 것이다.
		ar & m_arrTasks;
	}
};

