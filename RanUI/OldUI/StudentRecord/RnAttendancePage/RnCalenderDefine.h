#pragma once

const int DAYNUM = 2; //10자리 1자리 두개
const int ATTENDDANCE_WEEK = 7;
const int ATTENDDANCE_MONTH = 42;

enum EM_DAY_OF_WEEK 
{
	SUN = 0,
	MON = 1,
	TUE = 2,
	WED = 3,
	THU = 4,
	FRI = 5,
	SAT = 6,
};

enum EM_DAY_BOX_STATE
{
	EM_ACTIVE_DAY = 0x001,
	EM_TO_DAY = 0x002,
	EM_SELECT_DAY = 0x004,
};

struct SDAYDATA
{
	int m_nAnyDay;
	int m_nDayofWeek;
	int m_nDays; // 날짜
	int m_nMonth; // 달
	int m_nYear; //년
	DWORD m_emBoxState; //날짜 박스의 상태
	bool m_bIsAttend; //출석 상태
	std::string m_strEvevt;

	SDAYDATA::SDAYDATA()
	: m_nAnyDay(0)
	, m_nDayofWeek(INT_MAX)
	, m_nDays(0)
	, m_nMonth(0) // 달
	, m_nYear(0) //년
	, m_emBoxState(EM_ACTIVE_DAY)
	, m_bIsAttend(false)
	, m_strEvevt("")
	{
	}
	//이벤트 이름

	
	SDAYDATA& operator&=( const SDAYDATA& t )
	{
		m_emBoxState = m_emBoxState & t.m_emBoxState;
		return *this;
	}

	SDAYDATA& operator|=( const SDAYDATA& t )
	{
		m_emBoxState = m_emBoxState | t.m_emBoxState;
		return *this;
	}

	//SDAYDATA& operator~=( const SDAYDATA& t )
	//{
	//	m_emBoxState = m_emBoxState ~ t.m_emBoxState;
	//	return *this;
	//}
};

struct SATTENDANCEQUESTDATA
{
	DWORD dwQuestID;
	std::string strCourse;
	std::string strTitle;
	std::string strProgress;
	DWORD dwMoney;
	LONGLONG lnExp;
	DWORD dwItemID;
	bool bComplete;


	SATTENDANCEQUESTDATA::SATTENDANCEQUESTDATA()
		: dwQuestID(UINT_MAX)
		, dwItemID(UINT_MAX)
		, bComplete(false)
		, dwMoney(0)
		, lnExp(0)
	{
	}
};