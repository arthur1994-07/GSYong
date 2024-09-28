#pragma once
//
//mjeon.activities
//

#include <map>
#include <set>
#include <boost/serialization/map.hpp>
#include "../../RanLogic/Activity/Activity.h"



namespace boost
{
	namespace serialization
	{
		//
		//mjeon.instance.transfer
		// multimap을 통째로 serialize하면 data로 부터 읽어들인 m_mmActivities(serialization target)의 내용을 기본 생성자를 이용해서 생성한 Activity로 덮어써버린다.		
		// 이 경우 Activity의 전체 멤버값들을 직렬화하면 문제가 없을 것 같긴하나, 이는 불필요한 부하와 패킷크기 증가를 불러온다.
		// 이를 방지하기 위해 MMACTIVITES multimap에 대해 serialization method를 override해서 사용한다.
		//
		template <typename Archive>
		inline void save(Archive & ar, const MMACTIVITIES& t, const unsigned int file_version)
		{
			CONST_ITER_MMACTIVITIES iter = t.begin();;
			for (; iter != t.end(); ++iter)
			{
				ar & (*iter).second.m_id;

				ar & (*iter).second;
			}
		}

		template <typename Archive>
		inline void load(Archive &ar, MMACTIVITIES& t, const unsigned int file_version)
		{
			ITER_MMACTIVITIES iter;
			for (iter = t.begin(); iter != t.end(); ++iter)
			{
				SNATIVEID id;
				ar & id;

				GASSERT((*iter).second.m_id == id);
				if ((*iter).second.m_id == id)
				{
					ar & (*iter).second;
				}
				else
				{
					sc::writeLogError(sc::string::format("(CRITICAL) An activity(%1%, %2%) of mmActivities serialization ERROR !!!!", id.Mid(), id.Sid()));

					ITER_MMACTIVITIES iterFix;
					for (iterFix = t.lower_bound(id.Mid()); iterFix != t.upper_bound(id.Mid()); ++iterFix)
					{
						if ((*iterFix).second.m_id == id)
						{
							ar & (*iterFix).second;
							break;
						}
					}

					if (iterFix == t.upper_bound(id.Mid()))
					{
						sc::writeLogError(sc::string::format("(CRITICAL) An activity(%1%, %2%) of mmActivities couldn't b fixed !!!!", id.Mid(), id.Sid()));
					}
				}
			}
		}

		template<class Archive> 
		void serialize(Archive & ar, MMACTIVITIES& t, const unsigned int file_version)
		{
			boost::serialization::split_free(ar, t, file_version);
		}
	}
};


class ActivityBaseData;
class GLChar;
class GLGaeaServer;

//
// Server maintains(caching) the original data of Activities loaded from data files.
//  and each character maintains(caching) its own data of activities that is loaded from DB.
// These two Activities should be compared to check whether the activity has done or not.
//

class ActivityManager
{
public:
	ActivityManager(GLGaeaServer* pGaeaServer, ActivityBaseData &abd, DWORD &dwOwerChaNum);
	~ActivityManager();
	

	void Load(GLChar *pChar, BOOL bTransfer = FALSE);	//from DB to   member-variables(map/set)
	void Save(GLChar *pChar);							//to   DB from member-variables(map/set)

	void Reset();

	UINT GetBaseValue(DWORD dwActivityID);
	
	LONGLONG* GetRewardPoint()
	{
		return m_llRewardPoint;
	}

    LONGLONG GetRewardPoint( ENUM_ACTIVITY_CLASS emClass ) 
    {
        if ( emClass < ACTIVITY_CLASS_LOW || emClass >= ACTIVITY_CLASS_SIZE ) 
            return 0;
        
        return m_llRewardPoint[emClass];
    }

	void CheckAfterLoad();	//first ActivityCheck after ActivityLoad completion.

	//
	//returns how many activities have been updated if the action on the target is valid. 
	//  otherwise, returns 0.
	//
	DWORD CheckLevel(DWORD dwLevel);
	DWORD CheckMobKill(GLChar* pKillChar, GLChar* pChar, DWORD dwTargetID);
	DWORD CheckPK(DWORD dwTargetID);
	DWORD CheckMap(DWORD dwTargetID);
	DWORD CheckItemUse(DWORD dwTargetID);
	DWORD CheckItemTake(DWORD dwTargetID, DWORD dwActivityID);
	
	void  UpdateDBByOrder(Activity &actv);


	void CloseActivity(DWORD dwActivityID);
	BOOL IsClosedActivity(DWORD dwActivityID);

	//
	//completed an activity: process for rewards
	//
	void CompletedActivity(DWORD dwActivityID, ENUM_ACTIVITY_CLASS eClass, UINT nRewardPoint, std::string &strRewardTitleID);

	MMACTIVITIES& GetData()
	{
		return m_mmActivities;
	}

	void SetReady(BOOL bFlag)
	{
		m_bLoaded = bFlag;		
	}

	BOOL GetReady()
	{
		return m_bLoaded;
	}
		
private:
	ActivityManager();	// NOT USED !!!

	//
	//core-method used by other CheckXXX methods.
	//
	DWORD UpdateActivity(WORD wMID, DWORD dwTargetID, DWORD dwActivityID = 0/*Mandatory for ItemTake, Optional for others*/);
	void  AddUpRewardPoint(UINT nRewardPoint, ENUM_ACTIVITY_CLASS eClass);


	//
	//MUST be copied from ActivityBase's mapAllActivities
	//
	std::multimap<WORD, Activity>	m_mmActivities;

	ActivityBaseData&				m_abd;

	LONGLONG						m_llRewardPoint[ACTIVITY_CLASS_SIZE];


	DWORD	&m_dwOwnerChaNum;		//reference to owner characterNum
	GLChar	*m_pCharOwner;			//this pointer is valid after Load() or Save() is called !!
	BOOL	m_bLoaded;				//ActivityLoad() done ?
    GLGaeaServer* m_pGaeaServer;

private:
	friend class boost::serialization::access;
	template <typename Archive>
	void serialize(Archive &ar, const unsigned int version)
	{
		//
		//mjeon.instance.transfer
		// multimap을 통째로 serialize하면 data로 부터 읽어들인 m_mmActivities(serialization target)의 내용을 기본 생성자를 이용해서 생성한 Activity로 덮어써버린다.		
		// 이 경우 Activity의 전체 멤버값들을 직렬화하면 문제가 없을 것 같긴하나, 이는 불필요한 부하와 패킷크기 증가를 불러온다.
		// 이를 방지하기 위해 MMACTIVITES multimap에 대해 serialization method를 override해서 사용한다.
		//
		
		ar & m_mmActivities;
		ar & m_llRewardPoint;
		ar & m_bLoaded;
	}
};



