#pragma once
//
//mjeon.activities
//

#include <boost/tr1/memory.hpp>
#include <list>
#include <map>
#include <set>
#include <boost/serialization/string.hpp>
#include <boost/serialization/shared_ptr.hpp>

#include "../../SigmaCore/Util/Lock.h"
#include "../../EngineLib/G-Logic/GLDefine.h"


/*
ActivityManager
	|
	+- Activity01, Activity02, Activity03, ...


ActivityID(MID,SID)		// if( MID >= ACTIVITY_MID_LEVEL && MID <= ACTIVITY_MID_USEITEM )
	|
	+- Condition01  (only 1 condition)
	

Condition :	(Type, TargetID, ActionCount)	 ex> (ACTIVITY_MID_KILLMOB, MobID, Count)
*/


#define ACTIVITY_VERSION	1.0


//
//UI shows some categories based on below values.
//   ACTIVITY_MID_KILLMOB and ACTIVITY_MID_KILLPC belongs to the same category.
//
enum ENUM_ACTIVITY_CATEGORY
{
    ACTIVITY_CATEGORY_LEVEL     = 0,
    ACTIVITY_CATEGORY_KILL      = 1,
    ACTIVITY_CATEGORY_TAKE      = 2,
    ACTIVITY_CATEGORY_MAP       = 3,
    ACTIVITY_CATEGORY_USEITEM   = 4,    
    ACTIVITY_CATEGORY_SIZE		= 5,
};

enum UI_ACTIVITY_CATEGORY
{
	UI_ACTIVITY_CATEGORY_ALL   = 0,
	UI_ACTIVITY_CATEGORY_LEVEL = 1,
	UI_ACTIVITY_CATEGORY_KILL  = 2,
	UI_ACTIVITY_CATEGORY_TAKE  = 3,
	UI_ACTIVITY_CATEGORY_MAP   = 4,
	UI_ACTIVITY_CATEGORY_USE   = 5,
	UI_ACTIVITY_CATEGORY_SIZE  = 6,
};

enum UI_ACTIVITY_PAGE
{
	UI_ACTIVITY_PAGE_ALL      = 0,
	UI_ACTIVITY_PAGE_PROG     = 1,
	UI_ACTIVITY_PAGE_COMP     = 2,
	UI_ACTIVITY_PAGE_NOTCOMP  = 3,
};


enum ENUM_ACTIVITY_MID
{
	ACTIVITY_MID_LEVEL			= 0, //achieve the level
	ACTIVITY_MID_KILLMOB		= 1, //kill mob(s)
	ACTIVITY_MID_KILLPC			= 2, //kill player(s) on the map
	ACTIVITY_MID_MAP			= 3, //come to the map
	ACTIVITY_MID_TAKE			= 4, //take item(s)
	ACTIVITY_MID_USEITEM		= 5, //use item(s)
	ACTIVITY_MID_ACTIVITY		= 6, //complete some Activities
	ACTIVITY_MID_QUEST			= 7, //complete some Quests
	ACTIVITY_MID_PONIT			= 8, //make some points

    ACTIVITY_MID_SIZE           = 9, 
};


enum ENUM_ACTIVITY_CLASS
{
	ACTIVITY_CLASS_LOW			= 0,
	ACTIVITY_CLASS_MEDIUM		= 1,
	ACTIVITY_CLASS_HIGH			= 2,
	ACTIVITY_CLASS_SIZE			= 3,
};


enum ENUM_ACTIVITY_NOTIFY_LEVEL
{
	ACTIVITY_NOTIFY_NONE			= 0, //Non-notify
	ACTIVITY_NOTIFY_AROUND			= 1, //only near the character
	ACTIVITY_NOTIFY_BROADCAST		= 2, //all users in the server-group
	ACTIVITY_NOTIFY_CLUB			= 3, //club members
	ACTIVITY_NOTIFY_CLUBANDAROUND	= 4, //club members and near the character
    ACTIVITY_NOTIFY_SIZE			= 5,
};


enum ENUM_ACTIVITY_STATUS
{
	ACTIVITY_STATUS_INPROGRESS	= 0, //in progress - might be completed or closed by other characters.
	ACTIVITY_STATUS_COMPLETED	= 1, //completed - the character has completed the activity.
	ACTIVITY_STATUS_ORDERING	= 2, //progressing ordering by DB. can be changed its status only by DB to COMPLETED or CLOSED.
	ACTIVITY_STATUS_CLOSED		= 3, //closed (by other characters) - you cannot progress this activity anymore.
};

enum ENUM_ACTIVITY_TEXTID
{
    ENUM_ACTIVITY_TEXTID_TITLE       = 0,
    ENUM_ACTIVITY_TEXTID_DESC        = 1,
    ENUM_ACTIVITY_TEXTID_REWARDTITLE = 2,
    ENUM_ACTIVITY_TEXTID_SIZE 		 = 3,
};

enum ENUM_ACTIVITY_SLOT_TYPE
{
	ENUM_ACTIVITY_SLOT_TYPE_IDLE,         //  일반
	ENUM_ACTIVITY_SLOT_TYPE_CONTINUE,     //  진행중
	ENUM_ACTIVITY_SLOT_TYPE_NOTCOMP,      //  진행불가
	ENUM_ACTIVITY_SLOT_TYPE_COMP,         //  완료
	ENUM_ACTIVITY_SLOT_TYPE_SIZE, 
};

#define ACTIVITY_CLOSER			4	 //this character is the activity's closer. (i.e.last one who has completed this character)

enum ENUM_SIZE_OF_JSON_OBJECT
{
	SIZE_OF_ACTIVITYSTATUS_JSON_OBJECT	= 3,
	SIZE_OF_TITLE_JSON_OBJECT			= 4
};


struct ACTIVITY_LABEL;
struct ACTIVITY_REWARD;

//
//abstraction layer for an item of each activity
//
class Activity
{
public:
	Activity(WORD wMID, WORD wSID, UINT nValueBase, float fVersion = 1.0);
	Activity(DWORD dwID, UINT nValueBase, float fVersion = 1.0);
	virtual ~Activity();

	
	/*

	ACTIVITY_STATUS_INPROGRESS	----->	+- ACTIVITY_STATUS_COMPLETED										
										|
										+- ACTIVITY_STATUS_ORDERING
	*/	
	BOOL Update();//(OUT ACTIVITY_STATUS *pas = NULL);

	BOOL IsCompletedOnMap();

	
	//
	//ACTIVITY_STATUS_ORDERING	----->	+- ACTIVITY_STATUS_CLOSED
	//
	BOOL CloseByOrder()
	{
		if (m_status != ACTIVITY_STATUS_ORDERING)
			return FALSE;
		
		m_status	= ACTIVITY_STATUS_CLOSED;
		m_bUpdated	= TRUE;

		return TRUE;
	}


	//
	//ACTIVITY_STATUS_ORDERING	----->	+- ACTIVITY_STATUS_COMPLETED
	//
	BOOL CompleteByOrder()
	{
		if (m_status != ACTIVITY_STATUS_ORDERING)
			return FALSE;
		
		m_status	= ACTIVITY_STATUS_COMPLETED;
		m_bUpdated	= TRUE;

		return TRUE;
	}
	

	ENUM_ACTIVITY_STATUS GetStatus()
	{
		return m_status;
	}

	//
	//if this method is called while the character is loaded in DbAction, 
	// the activity should not change the updated flag.
	//
	void SetStatus(ENUM_ACTIVITY_STATUS status, BOOL bUpdate = TRUE)
	{
		m_status	= status;
		m_bUpdated	= bUpdate;

		if (status == ACTIVITY_STATUS_COMPLETED)
		{
			SetCompleteDate();
		}
	}

	void SetCompleteDate();

	std::string& GetCompleteDate()
	{
		return m_strCompleteDate;
	}

	UINT GetValue()
	{
		return m_nValue;
	}

	void ResetValue()	//only used by case of counting-items in inventory !!
	{
		m_nValue = 0;
	}

	//
	//if this method is called while the character is loaded in DbAction, 
	// the activity should not change the updated flag.
	//
	void SetValue(UINT nValue, BOOL bUpdate = TRUE)
	{
		m_nValue	= nValue;
		m_bUpdated	= bUpdate;
	}

	BOOL IsUpdated()
	{
		return m_bUpdated;
	}

    BOOL GetValidate() { return m_bValidate; }

    ENUM_ACTIVITY_CATEGORY GetType();

    void ResetData();
	BOOL IsMatched(DWORD dwTargetID);


    bool operator < (const Activity& activity )
    {
        if ( m_id.wMainID < activity.m_id.wMainID ) return true;
        
        if ( m_id.wMainID == activity.m_id.wMainID && 
             m_id.wSubID < activity.m_id.wSubID ) return true;
        
        return false;
    }

	void SetSyncFlag()
	{
		m_flagSync = TRUE;
	}

	BOOL GetSyncFlag()
	{
		return m_flagSync;
	}

private:
	Activity();	// NOT USED!

protected:
	void Initialize();


	/*  structure of an Activity

	(actvMID, actvSID, TargetID, RequiredValue)

	ex>  (2, 5, 13, 100)

	desc> 이 특별활동의 MID는 2, SID는 5이며 
		  이 행위를 취해야할 대상의 SNATIVEID값은 13(체인의 ID를 13이라 가정하자)이다.
		  요구값은 100이므로, 이것은 체인을 100마리 잡는 특별활동이다.
	*/
	

	//
	// There's no Accessor methods for Activity class's members only except smart-pointers. Just use them directly.
	//
public:
	SNATIVEID			    m_id;			//activity id (MID + SID)
	
	//
	//Target
	//
    std::vector<DWORD>      m_vecTargetID;  //TargetID: mobID/itemID(SNATIVEID) or MapID(PK on the map, come to the map)

	
	//members related to the activity-status MUST be set via accessors(methods).	
protected:
//public:	//serialization을 위해서 protected에서 public으로 변경
	BOOL					m_bUpdated;		//Is this activity updated?

	//
	//Action -> needs to be compared with base value to check the condition is TRUE or FALSE;
	//
	// status: m_nCount / m_nCountBase (ex> Mob: 10 kills / 100 kills)
	//	
	UINT					m_nValue;		//Current ActionStatus: How much, Level ...		: FROM db or character's action.
	ENUM_ACTIVITY_STATUS	m_status;		//current status


public:
	UINT					m_nValueBase;	//ActionBase: achieve this value (RequiredValue): FROM data-file.
	
	//
	//Extra-information: From DB
	//
	UINT					m_nByOrder;		//only first (m_nByOrder) characters can complete this activity.


	//
	//Extra-information: From data-file
	//
	float					m_fVersion;		//activity data version
	ENUM_ACTIVITY_CLASS		m_nClass;		//activity class (high, medium, low)
	ENUM_ACTIVITY_NOTIFY_LEVEL m_nNotifyLevel;

	BOOL					m_bValidate;	//validated?

	BOOL					m_flagSync;		//flag whether this activity should be synced between server and client.


//protected:
public:	//serialization을 위해서 protected에서 public으로 변경
	//
	//Only a pointer will be copied to ActivityManager from ActivityBaseData rather than all string-objects.
	//  All copied-activities will point to same object with original.
	//  Initialized and managed by ActivityBaseData. (pimpl의 목적은 아님)
	//
	std::tr1::shared_ptr<ACTIVITY_LABEL>	m_pLabel;

	//
	//Reward
	//
	std::tr1::shared_ptr<ACTIVITY_REWARD>	m_pReward;
	
	std::string				m_strCompleteDate;

public:
	//
	//Reward & Label accessor-methods
	//
	UINT& GetRewardPoint();
	std::string& GetRewardTitle();
	std::string& GetTitle();
	std::string& GetDesc();

	std::string& GetRewardTitleID();
	std::string& GetTitleID();
	std::string& GetDescID();

    void SetTitle( std::string strTitle );
    void SetDesc( std::string strDesc );
    void SetRewardPoint( UINT nPoint );
    void SetRewardTitle( std::string strRewardTitle );

	void SetTitleID( std::string strTitleID );
    void SetDescID( std::string strDescID );
	void SetRewardTitleID( std::string strRewardTitleID );

private:
	friend class boost::serialization::access;
	template <typename Archive>
	void serialize(Archive &ar, const unsigned int version)
	{
		// 일반적인 캐릭터 생성시에 DB로부터 동기화되는 하는 값들
		ar & m_nValue;
		ar & m_status;
		ar & m_bUpdated;
		
		/*
		//고정값 (data에서 읽어오는 값들)
		ar & m_id;
		ar & m_dwTargetID;
		ar & m_nValueBase;
		ar & m_nByOrder;
		ar & m_fVersion;	//not used		
		ar & m_bValidate;
		ar & m_nClass;
		ar & m_nNotifyLevel;		
		ar & m_flagSync;
		ar & m_pLabel;
		ar & m_pReward;
		ar & m_strCompleteDate;
		*/
	}
};


struct ACTIVITY_REWARD
{
	ACTIVITY_REWARD()
		:nRewardPoint(0)
	{
	}

	UINT					nRewardPoint;
	
	std::string				strRewardTitleID;	//(XML ID)
	std::string				strRewardTitle;

private:
	friend class boost::serialization::access;
	template <typename Archive>
	void serialize(Archive &ar, const unsigned int version)
	{
		ar & nRewardPoint;
		ar & strRewardTitleID;
		ar & strRewardTitle;
	}
};


struct ACTIVITY_LABEL
{
	std::string				strTitleID;			//(XML ID)
	std::string				strTitle;			//activity name

	std::string				strDescID;			//(XML ID)
	std::string				strDesc;			//activity description

private:
	friend class boost::serialization::access;
	template <typename Archive>
	void serialize(Archive &ar, const unsigned int version)
	{
		ar & strTitleID;
		ar & strTitle;
		ar & strDescID;
		ar & strDesc;
	}
};


namespace COMMENT
{
	extern std::string ACTIVITY_CLASS[ACTIVITY_CLASS_SIZE];
    extern std::string ACTIVITY_CATEGORY[ACTIVITY_CATEGORY_SIZE];
    extern std::string ACTIVITY_NOTIFY[ACTIVITY_NOTIFY_SIZE];
    extern std::string ACTIVITY_MID[ACTIVITY_MID_SIZE];
    extern std::string ACTIVITY_TEXTID[ENUM_ACTIVITY_TEXTID_SIZE];
};


