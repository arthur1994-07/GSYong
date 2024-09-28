#pragma once
//
//mjeon.attendance
//

#include <vector>
#include "../../EngineLib/G-Logic/GLDefine.h"


#define ATTENDANCE_VERSION			1.0

#define MAX_TASKS_PER_ONE_DAY		4
#define MAX_ITEMS_PER_ONE_TASK		3


enum ENUM_ATTENDANCE_TYPE
{
	ATTENDANCE_TYPE_CONNECTION		= 0, //keep the connection during the specific time (n-minutes)
	ATTENDANCE_TYPE_SPARRING_SOLO	= 1, //do one-vs-one-sparing n-times
	ATTENDANCE_TYPE_SPARRING_PARTY	= 2, //do party-vs-party-sparing n-times	
	ATTENDANCE_TYPE_WHISPER			= 3, //send whisper n-times to other PC
	ATTENDANCE_TYPE_PARTY			= 4, //compose a party with n-PCs
	ATTENDANCE_TYPE_ENCHANTW		= 5, //succeed weapon enchant n-times
	ATTENDANCE_TYPE_ENCHANTWC		= 6, //succeed weapon enchant n-times continuously
	ATTENDANCE_TYPE_ENCHANTA		= 7, //succeed armor enchant n-times
	ATTENDANCE_TYPE_ENCHANTAC		= 8, //succeed armor enchant n-times continuously
	ATTENDANCE_TYPE_POINTITEM		= 9, //put the items into point-item cart as much as n-points
	ATTENDANCE_TYPE_POSTSEND		= 10, //send n-posts to other PCs (with or without a attachment)	
	ATTENDANCE_TYPE_SIZE			= 11,
	//ATTENDANCE_TYPE_SPARRING_CLUB	= 3, //(NOT USED yet!) do club-vs-club-sparing n-times
};


enum ENUM_ATTENDANCE_STATUS			// == SUBJECT01_STATUS
{
	ATTENDANCE_STATUS_INPROGRESS	= 0, //in progress
	ATTENDANCE_STATUS_COMPLETED		= 1, //completed
};


enum ENUM_ATTENDANCE_UPDATE			// RESET or UPGRADE
{
	ATTENDANCE_UPDATE_SYNC			= 0, // DayN -> DayN	//just Sync
	ATTENDANCE_UPDATE_RESET			= 1, // DayN -> Day1
	ATTENDANCE_UPDATE_UPGRADE		= 2, // DayN -> DayN+1
};


class TaskN
{
public:
	TaskN();
	virtual ~TaskN();


	TaskN& operator=(const TaskN &rhs);


	void SyncFromDB();

	void SetCondtion(UINT nN, UINT nType, UINT nValueBase);

	UINT GetN()
	{
		return m_N;
	}

	void SetValue(UINT nValue);
	UINT GetValue()
	{
		return m_nValue;
	}

	void SetRewardPoint(UINT nRewardPoint);
	void SetRewardExp(UINT nRewardExp);
	void SetRewardExpRate(float fRewardExpRate);
	void SetRewardMoney(UINT nRewardMoney);

	UINT GetNumberOfRewardItems()
	{
		return m_nRewardItems;
	}

	void SetRewardItem(UINT idx, DWORD id);
	void SetRewardItem(UINT idx, WORD wMID, WORD wSID);
	UINT GetRewardItem(UINT idx)
	{
		if(idx >= MAX_ITEMS_PER_ONE_TASK)
		{			
			return SNATIVEID(false).dwID;
		}

		return m_arrRewardItems[idx];
	}

	UINT GetRewardPoint()
	{
		return m_nRewardPoint;
	}

	UINT GetRewardExp()
	{
		return m_nRewardExp;
	}

	float GetRewardExpRate()	//by Script
	{
		return m_fRewardExpRate;
	}

	UINT ComputeRewardExp(LONGLONG exp4LevUp);

	UINT GetRewardMoney()
	{
		return m_nRewardMoney;
	}	

	BOOL IsMatched(ENUM_ATTENDANCE_TYPE emType)
	{
		return (m_type == emType);
	}

	BOOL Update(ENUM_ATTENDANCE_TYPE emType, UINT nValue);

	BOOL Reset(ENUM_ATTENDANCE_TYPE emType);
	
	
	ENUM_ATTENDANCE_TYPE GetType()
	{
		return m_type;
	}

	void SetType(ENUM_ATTENDANCE_TYPE emType)
	{
		m_type = emType;
	}

	UINT GetValueBase()
	{
		return m_nValueBase;
	}

	void SetValueBase(UINT nValueBase)
	{
		m_nValueBase = nValueBase;
	}


	ENUM_ATTENDANCE_STATUS& GetStatus()
	{
		return m_status;
	}

	void SetStatus(ENUM_ATTENDANCE_STATUS status)
	{
		m_status = status;
	}

	void Completed()
	{
		m_status = ATTENDANCE_STATUS_COMPLETED;
	}

	BOOL IsCompleted()
	{
		return (m_status == ATTENDANCE_STATUS_COMPLETED);
	}

	void TookRewards(BOOL bTook)
	{
		m_bTookRewards = bTook;
	}

	BOOL IsTookRewards()
	{
		return m_bTookRewards;
	}


protected:
	
	UINT					m_N;			//Task N (1~3)

	//
	// Condition
	//
	ENUM_ATTENDANCE_TYPE	m_type;			//What(Target)
	UINT					m_nValueBase;	//How Many(RequiredValue)
	UINT					m_nValue;		//Current Value
	
	ENUM_ATTENDANCE_STATUS	m_status;


	//
	// Reward
	//
	UINT					m_nRewardPoint;
	float					m_fRewardExpRate;	//Squirrel로 64bit Exp값을 넘기지 않기 위해 레벨업을 위한 요구경험치에 대한 보상 비율만 얻어온다.
	UINT					m_nRewardExp;
	UINT					m_nRewardMoney;
	DWORD					m_arrRewardItems[MAX_ITEMS_PER_ONE_TASK];
	UINT					m_nRewardItems;	//number of items

	BOOL					m_bTookRewards;	//TRUE: the client took the result for this Task

private:
	friend class boost::serialization::access;
	template <typename Archive>
	void serialize(Archive &ar, const unsigned int version)
	{
		ar & m_N;
		ar & m_type;

		ar & m_nValueBase;
		ar & m_nValue;
		ar & m_status;

		ar & m_nRewardPoint;
		ar & m_fRewardExpRate;
		ar & m_nRewardExp;
		ar & m_nRewardMoney;
		ar & m_arrRewardItems;
		ar & m_nRewardItems;

		ar & m_bTookRewards;

		ar & m_bTookRewards;
	}
};