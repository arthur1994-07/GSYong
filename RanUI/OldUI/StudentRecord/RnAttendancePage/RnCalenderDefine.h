#pragma once

const int DAYNUM = 2; //10�ڸ� 1�ڸ� �ΰ�
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
	int m_nDays; // ��¥
	int m_nMonth; // ��
	int m_nYear; //��
	DWORD m_emBoxState; //��¥ �ڽ��� ����
	bool m_bIsAttend; //�⼮ ����
	std::string m_strEvevt;

	SDAYDATA::SDAYDATA()
	: m_nAnyDay(0)
	, m_nDayofWeek(INT_MAX)
	, m_nDays(0)
	, m_nMonth(0) // ��
	, m_nYear(0) //��
	, m_emBoxState(EM_ACTIVE_DAY)
	, m_bIsAttend(false)
	, m_strEvevt("")
	{
	}
	//�̺�Ʈ �̸�

	
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