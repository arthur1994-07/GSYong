#pragma once

//--------------------------------------------------------------------------------------------//
#include "../../RanUI/OldUI/StudentRecord/RnAttendancePage/RnCalenderDefine.h"
#include <string>

//--------------------------------------------------------------------------------------------//

class GLGaeaClient;
class GLQUEST;
//class CInnerInterface;

//--------------------------------------------------------------------------------------------//

class RnAttendanceClient
{

public:
	RnAttendanceClient  ( GLGaeaClient* pGaeaClient );
	~RnAttendanceClient ();

	//---------------- UI ----------------//

public:
	const bool          UILoad();
	void                UIClear();

	//---------------- DATA ----------------//
public:
	//int m_nYear;
	//int m_nMonth;
	//int m_nYunbonus;
	int m_nDayCount;
	//int m_nMonth_days[12];
	SDAYDATA m_sSelectDay;

	std::vector<DWORD> m_vecAttendanceQuestIdServerToClient;
	void ClearAttendanceQuestIdServerToClient();
	std::vector<DWORD> m_vecAttendanceQuestId;
	GLQUEST* m_pAttendanceQuest;
	GLQUESTPROG* m_AttendanceProg;
	DWORD m_dwAttendanceQuestID;
	bool m_bTodayCompleted;


	//DWORD 퀘스트ID3개

	void SetQuestTitle();
	void SetAttendanceQuestData( DWORD dwQuestID );
	void SetAttendanceQuest();
	void SetCondition();
	void SetSelectCondition(GLQuestPlay::MAPQUEST& sQuest);
	void SetGiftItem();
	void SetUItDisplayInfo();

	std::vector<int> m_vecAttendanceCompletedDay;
	std::vector<SATTENDANCEQUESTDATA> m_sAttendanceQuestData;

	std::vector<SATTENDANCEQUESTDATA>GetAttendanceQuestData();
	

	int GetDayCount();
	int GetAttendanceCount();

	void SetTotalDay(int nTotaDay);
	void SetWeekSchdule();
	void SetDaySchdule();
	void GetAttendanceCompleted();
	void AttendanceQuestFind(GLQuestPlay::MAPQUEST& sQuest);

	int IsAttendanceDay(int nDate);
	bool IsTodayCompleted();
	SDAYDATA GetSelectDay();
	void SetSelectDay(int nDayofWeek, 
					  int nDays, 
					  int nMonth, 
					  int nYear,
					  int nAnyDay);
	void ClearSlot();

	// cdm 정보?
	// 선도전 정보?
	// 엑스트라 스테이지 정보?
	// 

	void MsgRnattendanceProcess( NET_MSG_GENERIC* pMsg );
	void MsgRnattendanceQuestList( NET_MSG_GENERIC* pMsg );

	void ResetData();

private:
	GLGaeaClient*       m_pGaeaClient;

};
