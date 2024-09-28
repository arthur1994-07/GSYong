#pragma once
//
//mjeon.attendance
//
#include "../../RanLogic/Attendance/DayN.h"
#include <boost/tr1/memory.hpp>

class GLChar;
class GLGaeaServer;
class AttendanceScriptMan;

//
// TEMPORARY -->   ======= SQUIRREL ======= 로 설정하도록 변경할 것!!
//
// 6:00
//

#define ATTENDANCE_RESET_TIME_H		6
#define ATTENDANCE_RESET_TIME_M		0
#define ATTENDANCE_RESET_TIME_S		0
#define ATTENDANCE_RESET_COOLTIME_M	3
#define ATTENDANCE_RESET_DURATION	86400 // 24 * 60 * 60 secs


#define ATTENDANCE_REWARD_POST_SENDER		0xffffffff	//Max. of UINT (reserved ChaNum for SystemMail)
#define ATTENDANCE_REWARD_POST_DESIGN		0


//
// DayN(RanLogic) manager for Server-side (RanLogicServer)
//
class AttendanceManager: public DayN
{
public:
	AttendanceManager(GLGaeaServer* pGaeaServer, DWORD &dwOwerChaNum);
	virtual ~AttendanceManager();

	//AttendanceManager& operator=(const DayN &rhs);

	//
	// Is currentDatet is between StartDate(db-saved 6:00am) and next StartDate(next 6:00am)
	//
	static BOOL VerifyDate(IN __time64_t& csStartDate, IN CTime &ctCurrentDate, OUT CTime *pctStartDate = NULL);
	static BOOL VerifyDate(IN CTime   &ctStartDate, IN CTime &ctCurrentDate);
	

	void Load(GLChar *pChar, BOOL bTransfer = FALSE);			//from DB to   member-variables(map/set)
	void Save(GLChar *pChar, BOOL bLastOne4Maintenance = FALSE);//to   DB from member-variables(map/set)

	void Reset(CTime &ctCurrentTime);
	void Upgrade();
	void Sync();


	TaskN* GetTasks()
	{
		return m_arrTasks;
	}

	void SetTask(UINT idx, UINT nType, UINT nValueBase);	

	void ComputeStartDate(IN CTime &ctCurrentDate);	//NOTE: parameter is CurrentDate and this method will compute the startDate based on the currentdate !!

	void SetStartDate(IN CTime &ctStartDate)
	{
		m_ctStartDate = ctStartDate;
	}

	CTime& GetStartDate()
	{
		return m_ctStartDate;
	}

	void SetReady(BOOL bFlag)
	{
		m_bLoaded = bFlag;		
	}

	BOOL GetReady()
	{
		return m_bLoaded;
	}

	//
	// Client requested taking rewards.
	//
	BOOL TakeReward(UINT idx);


	//
	// public methods for checking of AttendanceTasks
	//
	void CheckTaskConnection()
	{
		return UpdateAttendance(ATTENDANCE_TYPE_CONNECTION);
	}

	void CheckTaskSparingSolo()
	{	
		return UpdateAttendance(ATTENDANCE_TYPE_SPARRING_SOLO);
	}

	void CheckTaskSparingParty()
	{	
		return UpdateAttendance(ATTENDANCE_TYPE_SPARRING_PARTY);
	}

	/*
	void CheckTaskSparingClub()
	{	
		return UpdateAttendance(ATTENDANCE_TYPE_SPARRING_CLUB);
	}
	*/	
	void CheckTaskWhisper()
	{
		return UpdateAttendance(ATTENDANCE_TYPE_WHISPER);
	}

	void CheckTaskParty(UINT nPartySize)
	{
		return UpdateAttendance(ATTENDANCE_TYPE_PARTY, TRUE, nPartySize);
	}

	void CheckTaskEnchantW()
	{
		return UpdateAttendance(ATTENDANCE_TYPE_ENCHANTW);
	}

	void CheckTaskEnchantWC(BOOL bSuccess)
	{
		return UpdateAttendance(ATTENDANCE_TYPE_ENCHANTWC, bSuccess);
	}

	void CheckTaskEnchantA()
	{
		return UpdateAttendance(ATTENDANCE_TYPE_ENCHANTA);
	}

	void CheckTaskEnchantAC(BOOL bSuccess)
	{
		return UpdateAttendance(ATTENDANCE_TYPE_ENCHANTAC, bSuccess);
	}
	
	//
	//in case of PointShop cart, it will be checked by Client and then be notified when it's completed.
	//
	void CheckTaskPointItem()
	{
		return UpdateAttendance(ATTENDANCE_TYPE_POINTITEM);
	}

	void CheckTaskPostSend()
	{
		return UpdateAttendance(ATTENDANCE_TYPE_POSTSEND);
	}
	
private:
	AttendanceManager();	//NOT USED !!!

	//
	// wrapping method for task-update is only called by several attendance-check methods.
	//
	void UpdateAttendance(ENUM_ATTENDANCE_TYPE emType, BOOL bSuccess = TRUE, UINT nValue = 1);	

	//
	// core method for TimeTask-update is only called by UpdateAttendance().
	//
	void UpdateTimeTask();

	//
	// core method for NonTimeTask-update except time-update is only called by UpdateAttendance().
	//
	void UpdateNonTimeTask(ENUM_ATTENDANCE_TYPE emType, BOOL bSuccess, UINT nValue);


	//
	// wrapping method to call TaskN::Update(emType) for the AttendanceManager.
	//
	BOOL UpdateTask(UINT idx, ENUM_ATTENDANCE_TYPE emType, UINT nValue = 1);

	//
	// reset the task in case of failure of ENCHANTWC & ENCHANTAC
	//
	BOOL ResetTask(UINT idx, ENUM_ATTENDANCE_TYPE emType);

	void CompletedTask(UINT idx);

	
	//
	// Reward handling
	//
	void ProcessRewardPoints(UINT idx);
	void ProcessRewardItemsAndMoney(UINT idx);
	void ProcessRewardExps(UINT idx);



private:

	DWORD		&m_dwOwnerChaNum;		//reference to owner characterNum
	
	GLChar		*m_pCharOwner;			//this pointer is valid after AttendanceLoad() is called !!
	BOOL		m_bLoaded;				//AttendanceLoad() done ?

	CTime		m_ctStartDate;			// When was this Attendance's recent reset? 
										// YYYY-MM-DD 06:00:00


	//
	// members for calculating of elapsed time.
	//
	DWORD		m_dwBaseTime;			//When did the character resume this attendance-mission?
										// i.e.when a character newly connects to the server, then
										//  this time-value is set to current time.

	DWORD		m_dwCurrentTime;
	DWORD		m_dwPrevElapsedMinutes;


	AttendanceScriptMan		*m_pScriptMan;

    GLGaeaServer			*m_pGaeaServer;

private:
	friend class boost::serialization::access;
	template <typename Archive>
	void serialize(Archive &ar, const unsigned int version)
	{
		ar & boost::serialization::base_object<DayN>(*this);

		ar & m_bLoaded;

		ar & m_ctStartDate;

		ar & m_dwBaseTime;
		ar & m_dwPrevElapsedMinutes;
	}
};
