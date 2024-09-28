#pragma once
//
//mjeon.activities
//

#include <map>
#include "./Activity.h"
#include "./ActivitySalesItem.h"


/*
ActivityManager
	|
	+- Activity01, Activity02, Activity03, ...


ActivityID(MID,SID)		// if( MID >= ACTIVITY_MID_LEVEL && MID <= ACTIVITY_MID_USEITEM )
	|
	+- Condition01  (only 1 condition)
	

Condition :	(Type, TargetID, ActionCount)	 ex> (ACTIVITY_MID_KILLMOB, MobID, Count)
*/


namespace ticpp
{
class Document;
}

class ActivityBaseData;
class ActivityVerifier;

namespace GLMSG
{
	struct SNET_ACTIVITY_SYNC;
};


typedef std::multimap<WORD, Activity>				                    MMACTIVITIES;
typedef std::multimap<WORD, Activity>::iterator		                    ITER_MMACTIVITIES;
typedef std::multimap<WORD, Activity>::const_iterator                   CONST_ITER_MMACTIVITIES;

typedef std::set<DWORD>								                    SETTARGETS;
typedef std::set<DWORD>::iterator					                    ITER_SETTARGETS;

typedef std::vector<Activity>       			                        VEC_ACTIVITIES;
typedef VEC_ACTIVITIES::iterator	                                    ITER_VEC_ACTIVITIES;

typedef std::multimap<DWORD,SACTIVITYSALESITEM>                         MMACTIVITYSALESITEM;
typedef MMACTIVITYSALESITEM::iterator                                   ITER_MMACTIVITYSALESITEM;
typedef std::pair<ITER_MMACTIVITYSALESITEM, ITER_MMACTIVITYSALESITEM>   RANGE_MMACTIVITYSALESITEM;


enum ENUM_XML_ENCODING_TYPE
{
	XML_ENCODING_UTF8		= 0,
	XML_ENCODING_ANSI		= 1	
};


//
// DO NOT modify base information. base information is loaded from data file.
//
class ActivityBaseData //: public sc::DefaultLock		// SINGLETON
{
public:
	static ActivityBaseData* GetInstance();
	static void ReleaseInstance();
	
	~ActivityBaseData();

	
	/*
		.XML : Decrypted(raw) XML text

		.BIN : Encrypted	XML

		.RCC : Zipped .BIN
	*/

	//
	// plain XML --> encrypted .bin --> zip to .rcc
	//
	BOOL xml2rcc(std::string &pathXML, std::string &pathZip, std::string &pathBin);

	

	BOOL SaveRCC(std::string &pathZip, std::string &pathBin);			//to .rcc	from m_mmActivities.
    BOOL SaveBIN(std::string &pathBin);									//to .bin	from m_mmActivities.	
    BOOL SaveXML(const std::string& fileXml);							//to .xml	from m_mmActivities.
	BOOL SaveXML(std::string &pathBin, IN std::string &xml);			//to .xml	from buffer.
    BOOL SaveCSV(std::string &pathCSV);                                 //to .csv   from m_mmActivities.
	BOOL WriteFile(const std::string& strPath, const std::string& strWhat);


	BOOL Load(std::string &nameBin);									//calls either LoadRCC or LoadBIN
	
    BOOL LoadRCC(std::string &pathZip, std::string &nameBin);			//from .rcc		to m_mmActivities.
    BOOL LoadBIN(std::string &nameBin);									//from .bin		to m_mmActivities.	
    BOOL LoadXML(IN const std::string& pathXML);						//from .xml		to m_mmActivities.
	BOOL LoadXML(IN const std::string &pathXML, OUT std::string &xml);		//from .xml		to buffer.
	BOOL ReadFile(const std::string& strPath, OUT std::string& strTarget);

	BOOL ParseXML(std::string &xml, BOOL bVerify = FALSE, ENUM_XML_ENCODING_TYPE encoded = XML_ENCODING_UTF8);	//from XML-buffer to m_mmActivities.

    BOOL CheckVerifier ();
	
	MMACTIVITIES& GetData()
	{
		return m_mmActivities;
	}

    Activity* GetActivity( SNATIVEID sID );
	Activity* GetActivity( DWORD dwID );

    bool InsertActivity( const Activity& cActivity );
    bool DeleteActivity( SNATIVEID sID );

    void CleanUp();
    void ResetData();

private:
	ActivityBaseData();
	static ActivityBaseData* SelfInstance;
	
	
	BOOL ParseData(IN ticpp::Document &doc, ENUM_XML_ENCODING_TYPE encoded = XML_ENCODING_UTF8);
	BOOL ComposeXML(OUT std::string &xml, BOOL bTagUnicode = FALSE);	//bTagUnicode: in case of exporting to a file, Unicode Tag MUST be written first in the file.

	BOOL Encrypt(LPBYTE pPlain, std::string &strEncrypted);
	BOOL Encrypt(std::string &strPlain, std::string &strEncrypted);
	BOOL Encrypt(CString &csPlain, CString &csEncrypted);

	BOOL Decrypt(LPBYTE pEncrypted, std::string &strPlain);
	BOOL Decrypt(std::string &strEncrypted, std::string &strPlain);
	BOOL Decrypt(CString &csEncrypted, CString &csPlain);


	std::string						m_path;		//activity-data path
	LPBYTE							m_pBuffer;	//unzipped into memory

	//
	// std::multimap<MID, ActivityObject>
	//
	std::multimap<WORD, Activity>	m_mmActivities;
	//						|
	//						+- condition's base values are initialized with base value from the data file.

	
	//
	// m_mmActivities is base-data that means it is NOT changed during game-play.
	//

/*
	//
	// std::map<ActivityID, ITER_MMACTIVITIES>
	//
	std::map<DWORD, ITER_MMACTIVITIES>		m_mapActivities;
	//					|
	//					+- refer to Activity object in m_mmActivities.						
*/

	//
	// set of Targets: for quick-check & fast-iteration
	//
	std::set<DWORD>					m_setMob;		//kill the mob
	std::set<DWORD>					m_setMap;		//come to the map(s)
	std::set<DWORD>					m_setMapPK;		//PK on the Map
	std::set<DWORD>					m_setItemUse;	//use the item(s)
	std::set<DWORD>					m_setItemTake;	//take the item(s)	

	std::set<DWORD>					m_setClosedActivities;	//maintains Closed Activities temporarily. DB already has permanent information about closed-activities.

public:
    // bjju 클라이언트에서 사용 (Sync 받았는지여부)
    bool                            m_bSync;

public:
	enum
    {
        MAX_SID = 1024,
    };

	ActivityVerifier				*m_pVerifier;

public:
    // (bjju) 특별활동 판매 아이템 리스트
    MMACTIVITYSALESITEM  m_mmActivitySalesItem;

    MMACTIVITYSALESITEM& GetSalesItemData()
    {
        return m_mmActivitySalesItem;
    }

public:	

	//
	//methods for checking existance of the TargetID in the sets
	//
	BOOL IsValidMob(DWORD dwTargetID)
	{
		return (m_setMob.count(dwTargetID) > 0 ? TRUE : FALSE);
	}

	BOOL IsValidMap(DWORD dwTargetID)
	{
		return (m_setMap.count(dwTargetID) > 0 ? TRUE : FALSE);
	}

	BOOL IsValidPKMap(DWORD dwTargetID)
	{
		return (m_setMapPK.count(dwTargetID) > 0 ? TRUE : FALSE);
	}

	BOOL IsValidItemUse(DWORD dwTargetID)
	{
		return (m_setItemUse.count(dwTargetID) > 0 ? TRUE : FALSE);
	}

	BOOL IsValidItemTake(DWORD dwTargetID)
	{
		return (m_setItemTake.count(dwTargetID) > 0 ? TRUE : FALSE);
	}

/*	in case of checking level, it should be done every time a character lev-up because
	the updated level might be greater than dwLevel(baseValue).

	ex> ValueBase = 10 (lv), updated level = 11 (lv), then the character will not be notified even though 
		the character completed the activity.

	BOOL IsValidLevel(DWORD dwLevel)
	{
		return (m_setLevel.count(dwLevel) > 0 ? TRUE : FALSE);
	}
*/
	//
	//check and update closed activity
	//
	void CloseActivity(DWORD dwActivityID)
	{
		//
		//NOT need to lock even though several db-threads may call this function simultaneously.
		//
		m_setClosedActivities.insert(dwActivityID);
	}

	
	/* mjeon
		CloseActivity와 IsClosedActivity는 lock할 필요없다.
		설사, lock을 하지 않아 순서가 바뀌더라도 정상적으로 처리가 되도록 디자인 되었다.
		이 두 군데에서 lock을 하면 많은 성능손실을 보게 될 것이다.
		IsClosedActivity의 역할은 불필요한 연산을 더 이상 진행하지 않아도 되도록 미리 확인해서 
		루틴을 탈출하도록 도와주는 역할일 뿐이지, 실제 연산의 결과를 좌우하지는 않는다.
	*/	
	BOOL IsClosedActivity(DWORD dwActivityID)
	{
		return (m_setClosedActivities.count(dwActivityID) > 0 ? TRUE : FALSE);
	}


public:
	
	//
	//only used by Client to sync the status of activities (encoded as json) that sent as packets.
	//
    DWORD GetComplateCount( ENUM_ACTIVITY_CLASS emClass );
	BOOL SyncActivityStatus(GLMSG::SNET_ACTIVITY_SYNC *pMsg);
};


/*
						  (copy)
Data  ->  ActivityBase  --------->  ActivityMananger  <-----------  DB

*/
