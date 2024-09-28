#include "../pch.h"
#include "./TaskN.h"

#include "../../SigmaCore/Log/LogMan.h"
#include "../../SigmaCore/String/StringFormat.h"
#include "../../SigmaCore/String/StringUtil.h"


TaskN::TaskN()
:m_N(0)
,m_type(ATTENDANCE_TYPE_CONNECTION)
,m_nValueBase(0)
,m_nValue(0)
,m_status(ATTENDANCE_STATUS_INPROGRESS)
,m_nRewardPoint(0)
,m_fRewardExpRate(0.0)
,m_nRewardExp(0)
,m_nRewardMoney(0)
,m_nRewardItems(0)
,m_bTookRewards(FALSE)
{
	for(int i=0; i<MAX_ITEMS_PER_ONE_TASK; i++)
	{
		m_arrRewardItems[i] = SNATIVEID(false).dwID;
	}
}

TaskN::~TaskN()
{
}

void TaskN::SetCondtion(UINT nN, UINT nType, UINT nValueBase)
{
	m_N				= nN;
	m_type			= (ENUM_ATTENDANCE_TYPE)nType;
	m_nValueBase	= nValueBase;
}

void TaskN::SetValue(UINT nValue)
{
	m_nValue		= nValue;
}


void TaskN::SetRewardPoint(UINT nRewardPoint)
{
	m_nRewardPoint = nRewardPoint;
}


void TaskN::SetRewardExp(UINT nRewardExp)
{
	m_nRewardExp = nRewardExp;
}


void TaskN::SetRewardExpRate(float fRewardExpRate)	//by Script
{
	m_fRewardExpRate = fRewardExpRate;
}


UINT TaskN::ComputeRewardExp(LONGLONG exp4LevUp)
{
	m_nRewardExp = (UINT)(exp4LevUp * m_fRewardExpRate);
	return m_nRewardExp;
}


void TaskN::SetRewardMoney(UINT nRewardMoney)
{
	m_nRewardMoney = nRewardMoney;
}


void TaskN::SetRewardItem(UINT idx, DWORD dwID)
{
	if (idx >= MAX_ITEMS_PER_ONE_TASK)
	{
		sc::writeLogError(std::string("TaskN::SetRewardItem(): index is bigger than MAX_ITEMS_PER_ONE_TASK."));
		return;
	}

	m_arrRewardItems[idx] = dwID;   

	if(dwID != SNATIVEID(false).dwID)
		m_nRewardItems++;
}


void TaskN::SetRewardItem(UINT idx, WORD wMID, WORD wSID)
{
	if (idx >= MAX_ITEMS_PER_ONE_TASK)
	{
		sc::writeLogError(std::string("TaskN::SetRewardItem(): index is bigger than MAX_ITEMS_PER_ONE_TASK."));
		return;
	}

	SNATIVEID id(wMID, wSID);

	m_arrRewardItems[idx] = id.dwID;

	if (id != SNATIVEID(false).dwID)
		m_nRewardItems++;
}


BOOL TaskN::Update(ENUM_ATTENDANCE_TYPE emType, UINT nValue)
{
	if (m_status != ATTENDANCE_STATUS_INPROGRESS)
		return FALSE;

	if (! IsMatched(emType))
		return FALSE;

	if (emType == ATTENDANCE_TYPE_PARTY)
	{
		m_nValue = nValue;	//nValueBase 명 이상의 파티를 결성하라
	}
	else if (emType == ATTENDANCE_TYPE_POINTITEM)	/*checked by Client: just trust client's complete request*/
	{
		m_nValue = m_nValueBase;
	}
	else
	{
		m_nValue++;
	}

	if (m_nValue >= m_nValueBase)
		Completed();

	return TRUE;	//updated
}

BOOL TaskN::Reset(ENUM_ATTENDANCE_TYPE emType)
{
	if (m_status != ATTENDANCE_STATUS_INPROGRESS)
		return FALSE;

	if (! IsMatched(emType))
		return FALSE;

	m_nValue = 0;

	return TRUE;
}

TaskN& TaskN::operator=(const TaskN& rhs)
{
	if (this == &rhs)
		return *this;

	m_N				= rhs.m_N;
	m_type			= rhs.m_type;
	m_nValueBase	= rhs.m_nValueBase;
	m_nValue		= rhs.m_nValue;

	m_status		= rhs.m_status;
	
	m_nRewardExp	= rhs.m_nRewardExp;	//nRewardExp will be calculated after calling = operator by ComputeRewardExp() based on fRewardExpRate.
	m_fRewardExpRate= rhs.m_fRewardExpRate;
	m_nRewardPoint	= rhs.m_nRewardPoint;
	m_nRewardMoney	= rhs.m_nRewardMoney;

	m_nRewardItems	= rhs.m_nRewardItems;

	for (int i=0; i<MAX_ITEMS_PER_ONE_TASK; i++)
	{
		m_arrRewardItems[i] = rhs.m_arrRewardItems[i];
	}

	m_bTookRewards	= rhs.m_bTookRewards;

	return *this;
}
