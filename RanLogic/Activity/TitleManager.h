#pragma once
//
//mjeon.activities
//
#include <vector>
#include <boost/serialization/map.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>

#include "../../RanLogic/Activity/Activity.h"

struct TITLEINFO
{
	UINT		nIndex;
	std::string strTitle;
	UINT		nClass;
	BOOL		bSelected;	//is this selected?

	TITLEINFO()
		:bSelected(FALSE)
	{
	}

    bool operator < (const TITLEINFO& sTitle )
    {
       if ( nClass > sTitle.nClass ) return true;

       return false;
    }

private:
	friend class boost::serialization::access;
	template <typename Archive>
	void serialize(Archive &ar, const unsigned int version)
	{
		ar & nIndex;
		ar & strTitle;
		ar & nClass;
		ar & bSelected;
	}
};


namespace GLMSG
{
	struct SNET_ACTIVITY_REWARD_BADGE_SYNC;
};


typedef std::vector<TITLEINFO>					VECTITLE;
typedef std::vector<TITLEINFO>::iterator		ITER_VECTITLE;

typedef std::map<UINT, TITLEINFO>				MAPTITLE;
typedef std::map<UINT, TITLEINFO>::iterator		ITER_MAPTITLE;

enum ENUM_TITLE_INDEX_NONE
{
	TITLE_INDEX_NONE	= UINT_MAX
};


//
// TitleManager saves and manages XML-IDs of titles.
//
class TitleManager
{
public:
	TitleManager();
	virtual ~TitleManager();

	// DB <---> TitleManager
	BOOL Add(IN std::string &strTitle, IN ENUM_ACTIVITY_CLASS eClass);


	// called by client to sync from the server	
	BOOL Add(IN std::string &strTitle, IN ENUM_ACTIVITY_CLASS eClass, UINT idx);

	UINT GetSize();

	BOOL GetTitle(int idx, OUT std::string &title, OUT ENUM_ACTIVITY_CLASS &eClass);
    TITLEINFO* GetTitleInfo( int idx );
	
	//
	// the character selected(changed) it's title pointed by idx.
	//
	BOOL Select(int idx);	//select specific one
	BOOL Select();			//select last one


	//
	// GamePlay ---> TitleManager
	// the character acquired a new title by completion an activity.
	//
	BOOL AcquireNewTitle(IN std::string &strTitleID, IN ENUM_ACTIVITY_CLASS eClass);


	//
	// Get entire titles
	//
	MAPTITLE& GetData()
	{
		return m_mapTitles;
	}

	//
	// Get titles to be updated into DB (newly acquired during the game-play)
	//
	VECTITLE& GetDataToUpdate()
	{
		return m_vecAcquiredTitles;
	}

	//
	// Get title that the character has selected
	//
	const std::string GetCurrentTitle();
	BOOL		      GetCurrentTitle(OUT TITLEINFO& ti);
    
    void Clear();

private:
	std::map<UINT, TITLEINFO>	m_mapTitles;			//list of all titles
	std::vector<TITLEINFO>		m_vecAcquiredTitles;	//list of new titles that the player got during the game play.
														//update DB with this list when the player exits.
	std::string					m_strNull;

	int							m_idxCurrentSelect;

public:
	//
	//only used by Client to sync titles (encoded as json) that sent as packets.
	//
	BOOL SyncTitle(GLMSG::SNET_ACTIVITY_REWARD_BADGE_SYNC *pMsg);

private:
	friend class boost::serialization::access;
	template <typename Archive>
	void serialize(Archive &ar, const unsigned int version)
	{
		ar & m_mapTitles;
		ar & m_vecAcquiredTitles;
		ar & m_idxCurrentSelect;
	}
};


