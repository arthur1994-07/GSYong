#include "../pch.h"
#include "../../RanLogic/Activity/TitleManager.h"
#include "../../RanLogic/Msg/GLContrlMsg.h"

#include "../../SigmaCore/Log/LogMan.h"
#include "../../SigmaCore/String/StringFormat.h"
#include "../../SigmaCore/String/StringUtil.h"

#include "../../EngineLib/GUInterface/GameTextControl.h"


// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


//
// TitleManager saves and manages XML-IDs of titles.
//

//
//TitleManager is not thread-safe. There is no lock mechanism inside it.
// Generally, it's used in a GLChar only for the character and so it is not accessed by several threads simultaneously.
//

TitleManager::TitleManager()
:m_idxCurrentSelect(-1)
,m_strNull("")
{
}


TitleManager::~TitleManager()
{
}


//
// called when load from DB
//
BOOL TitleManager::Add(IN std::string &strTitle, IN ENUM_ACTIVITY_CLASS eClass)
{
	TITLEINFO ti;

	ti.nIndex	= static_cast< UINT >( m_mapTitles.size() );	//there must be sequencial indexes in the map.
	ti.nClass	= eClass;
	ti.strTitle = strTitle;

	return (m_mapTitles.insert(std::make_pair(ti.nIndex, ti)).second);
}


//
// called by client to sync from the server
//
BOOL TitleManager::Add(IN std::string &strTitle, IN ENUM_ACTIVITY_CLASS eClass, UINT idx)
{
	TITLEINFO ti;

	ti.nIndex	= idx;
	ti.nClass   = eClass;
	ti.strTitle = strTitle;		

	return (m_mapTitles.insert(std::make_pair(ti.nIndex, ti)).second);
}


TITLEINFO* TitleManager::GetTitleInfo( int idx )
{
    ITER_MAPTITLE pos = m_mapTitles.find( idx );
    if ( pos != m_mapTitles.end() )
        return &pos->second;

    return NULL;
}


UINT TitleManager::GetSize()
{
	return (UINT)(m_mapTitles.size());
}


BOOL TitleManager::GetTitle(int idx, OUT std::string &title, OUT ENUM_ACTIVITY_CLASS &eClass)
{
	if (m_mapTitles.size() > (UINT)idx && idx >= TITLE_INDEX_NONE)	//there must be sequencial indexes in the map.
	{	
		if (idx == TITLE_INDEX_NONE)
		{
			title	= m_strNull;
			eClass	= ACTIVITY_CLASS_LOW;

			return TRUE;			
		}
		
		TITLEINFO ti;
									//NOT required to check the existence of "idx":
		ti = m_mapTitles[idx];		// it's guaranteed that there is a value with the key of "idx"
									//  because there must be sequencial indexes in the map.

		title	= ti.strTitle.c_str();
		eClass	= (ENUM_ACTIVITY_CLASS)ti.nClass;
	}
	else
	{
		sc::writeLogWarn(sc::string::format("TitleManager::GetTitle() failed for index of %d.", idx));
		return FALSE;
	}

	return TRUE;
}


BOOL TitleManager::Select(int idx)
{
	if ( idx > (int)(m_mapTitles.size() - 1) )
	{
		sc::writeLogWarn(sc::string::format("TitleManager::Select(%d) failed.", idx));
		return FALSE;
	}

	if (m_idxCurrentSelect >= 0)
		m_mapTitles[m_idxCurrentSelect].bSelected = FALSE;

	if (idx == TITLE_INDEX_NONE)
	{
		;	//none-title
	}
	else
	{
		m_mapTitles[idx].bSelected = TRUE;
	}

	m_idxCurrentSelect = idx;

	return TRUE;
}

//
//select last one
//
BOOL TitleManager::Select()
{
	if ( m_mapTitles.empty() )
	{
		sc::writeLogWarn(std::string("TitleManager::Select() failed because it's empty."));
		return FALSE;
	}

	UINT idx = (UINT)m_mapTitles.size() - 1;	//last index

	if (m_idxCurrentSelect >= 0)
		m_mapTitles[m_idxCurrentSelect].bSelected = FALSE;

	if (idx == TITLE_INDEX_NONE)
	{
		;	//none-title
	}
	else
	{
		m_mapTitles[idx].bSelected = TRUE;
	}

	m_idxCurrentSelect = idx;

	return TRUE;
}


const std::string TitleManager::GetCurrentTitle()
{
	if (m_idxCurrentSelect >= 0)
	{
        return m_mapTitles[m_idxCurrentSelect].strTitle;
	}
	else
	{
		return m_strNull;
	}
}


BOOL TitleManager::GetCurrentTitle(OUT TITLEINFO& ti)
{
	if (m_idxCurrentSelect >= 0)
	{
		ti = m_mapTitles[m_idxCurrentSelect];
		return TRUE;
	}
	else if (m_idxCurrentSelect == TITLE_INDEX_NONE)
	{
		TITLEINFO t;
		
		t.nIndex	= TITLE_INDEX_NONE;
		t.strTitle  = m_strNull;
		t.bSelected = TRUE;
		t.nClass	= ACTIVITY_CLASS_LOW;

		ti = t;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


BOOL TitleManager::AcquireNewTitle(IN std::string &strTitleID, IN ENUM_ACTIVITY_CLASS eClass)
{
	if ( strTitleID.empty() )
		return FALSE;

	TITLEINFO ti;

	UINT idx	= static_cast< UINT >( m_mapTitles.size() );

	ti.nIndex = idx;
	ti.nClass	= eClass;
	ti.strTitle = strTitleID;
	
	if (m_mapTitles.insert(std::make_pair(ti.nIndex, ti)).second)
	{
		m_vecAcquiredTitles.push_back(ti);

		return TRUE;
	}
	else
	{
		sc::writeLogError(sc::string::format("TitleManager::AcquireNewTitle() failed for index of %d,(titleID: %s).", idx, strTitleID));
		
		return FALSE;
	}
}


BOOL TitleManager::SyncTitle(GLMSG::SNET_ACTIVITY_REWARD_BADGE_SYNC *pMsg)
{
	if (!pMsg)
		return FALSE;

	UINT			nSync	= pMsg->nSync;
	TITLE_SYNC		*pData	= pMsg->GetData();	

	UINT			idx		= 0;

	for (idx = 0; idx < nSync; idx++)
	{
		Add(std::string(pData[idx].szTitle), (ENUM_ACTIVITY_CLASS)pData[idx].dwClass, pData[idx].nIndex);

		if (pData[idx].bSelected)
			Select(pData[idx].nIndex);		
	}

	return (idx == nSync);
}


void TitleManager::Clear()
{
    m_mapTitles.clear();
    m_vecAcquiredTitles.clear();
    m_idxCurrentSelect = -1;
}

