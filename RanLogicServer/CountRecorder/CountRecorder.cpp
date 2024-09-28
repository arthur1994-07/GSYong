#include "pch.h"
#include "../Land/GLLandMan.h"

#include "./CountRecorder.h"

GLCountRecorder::GLCountRecorder ( GLLandMan* const pLandMan )
: m_pLandMan ( pLandMan )
, m_lDropMoneyCount ( 0L )
, m_lEnterPlayerCount ( 0L )
, m_dwCurPlayerCount ( 0 )
, m_dTime ( 0.0f )
{
	m_tRecordStartTime = CTime::GetCurrentTime();
}

GLCountRecorder::~GLCountRecorder ( void )
{
}

void GLCountRecorder::FrameMove ( const float fElapsedTime )
{
	m_dTime += static_cast< double >( fElapsedTime );
}

void GLCountRecorder::Clear ( void )
{
	MAP_COUNTER_ITR itr = m_mapDropOutCrowCount.begin();
	while ( itr != m_mapDropOutCrowCount.end() )
	{
		m_mapDropOutCrowCount.erase(itr++);
	}

	itr = m_mapDropItemCount.begin();
	while ( itr != m_mapDropItemCount.end() )
	{
		m_mapDropItemCount.erase(itr++);
	}
	m_lDropMoneyCount = m_lEnterPlayerCount = 0;
	m_dTime = 0;
}

void GLCountRecorder::Clear_UserInfo ( void )
{
	m_dwCurPlayerCount = 0;
	m_mapCurPartyPlayerCount.clear();
	m_mapCurClubPlayerCount.clear();
	m_mapCurCountryPlayerCount.clear();
	m_mapCurSchoolPlayerCount.clear();
	m_mapCurFactionPlayerCount.clear();
}

void GLCountRecorder::add_DropOutCrowCount ( const DWORD dwID )
{
	MAP_COUNTER_ITR itr = m_mapDropOutCrowCount.find(dwID);
	if ( m_mapDropOutCrowCount.end() == itr )
	{
		m_mapDropOutCrowCount.insert( std::make_pair(dwID, 0L) );
		itr = m_mapDropOutCrowCount.find(dwID);
	}

	(*itr).second = (*itr).second+1;
}

void GLCountRecorder::add_DropItemCount ( const DWORD dwID )
{
	MAP_COUNTER_ITR itr = m_mapDropItemCount.find(dwID);
	if ( m_mapDropItemCount.end() == itr )
	{
		m_mapDropItemCount.insert( std::make_pair(dwID, 0L) );
		itr = m_mapDropItemCount.find(dwID);
	}

	(*itr).second = (*itr).second+1;
}

void GLCountRecorder::add_DropMoneyCount ( const LONGLONG& lMoney )
{
	m_lDropMoneyCount += lMoney;
}

void GLCountRecorder::add_EnterPlayerCount ( )
{
	++m_lEnterPlayerCount;
}

void GLCountRecorder::add_CurPlayerCount ( void )
{
	++m_dwCurPlayerCount;
}

void GLCountRecorder::add_CurPartyPlayerCount ( const DWORD dwID )
{
	if ( PARTY_NULL == dwID )
		return;

	MAP_COUNTER_ITR itr = m_mapCurPartyPlayerCount.find(dwID);
	if ( m_mapCurPartyPlayerCount.end() == itr )
	{
		m_mapCurPartyPlayerCount.insert( std::make_pair(dwID, 0L) );
		itr = m_mapCurPartyPlayerCount.find(dwID);
	}

	(*itr).second = (*itr).second+1;
}

void GLCountRecorder::add_CurClubPlayerCount ( const DWORD dwID )
{
	if ( CLUB_NULL == dwID )
		return;

	MAP_COUNTER_ITR itr = m_mapCurClubPlayerCount.find(dwID);
	if ( m_mapCurClubPlayerCount.end() == itr )
	{
		m_mapCurClubPlayerCount.insert( std::make_pair(dwID, 0L) );
		itr = m_mapCurClubPlayerCount.find(dwID);
	}

	(*itr).second = (*itr).second+1;
}

void GLCountRecorder::add_CurCountryPlayerCount ( const DWORD dwID )
{
	// 존재하는 국가인지도 확인하는게 좋지만 단순로그이기 때문에 가볍게 체크한다;
	if ( Country::UNKNOWN_COUNTRY == dwID )
		return;

	MAP_COUNTER_ITR itr = m_mapCurCountryPlayerCount.find(dwID);
	if ( m_mapCurCountryPlayerCount.end() == itr )
	{
		m_mapCurCountryPlayerCount.insert( std::make_pair(dwID, 0L) );
		itr = m_mapCurCountryPlayerCount.find(dwID);
	}

	(*itr).second = (*itr).second+1;
}

void GLCountRecorder::add_CurSchoolPlayerCount ( const DWORD dwID )
{
	if ( SCHOOL_SM != dwID && SCHOOL_HA != dwID && SCHOOL_BH != dwID )
		return;

	MAP_COUNTER_ITR itr = m_mapCurSchoolPlayerCount.find(dwID);
	if ( m_mapCurSchoolPlayerCount.end() == itr )
	{
		m_mapCurSchoolPlayerCount.insert( std::make_pair(dwID, 0L) );
		itr = m_mapCurSchoolPlayerCount.find(dwID);
	}

	(*itr).second = (*itr).second+1;
}

void GLCountRecorder::del_CurPlayerCount ( void )
{
	--m_dwCurPlayerCount;
}

void GLCountRecorder::del_CurPartyPlayerCount ( const DWORD dwID )
{
	MAP_COUNTER_ITR itr = m_mapCurPartyPlayerCount.find(dwID);
	if ( m_mapCurPartyPlayerCount.end() != itr )
	{
		(*itr).second = (*itr).second-1;
		if ( (*itr).second < 0 )
		{
			(*itr).second = 0;

			sc::writeLogError( "[ Country Recorder ] [ Failed GLCountRecorder::remove_CurPartyPlayerCount!! ]" );
		}

		if ( (*itr).second == 0 )
		{
			m_mapCurPartyPlayerCount.erase( itr );
		}
	}
}

void GLCountRecorder::del_CurClubPlayerCount ( const DWORD dwID )
{
	MAP_COUNTER_ITR itr = m_mapCurClubPlayerCount.find(dwID);
	if ( m_mapCurClubPlayerCount.end() != itr )
	{
		(*itr).second = (*itr).second-1;
		if ( (*itr).second < 0 )
		{
			(*itr).second = 0;

			sc::writeLogError( "[ Country Recorder ] [ Failed GLCountRecorder::remove_CurClubPlayerCount!! ]" );
		}

		if ( (*itr).second == 0 )
		{
			m_mapCurClubPlayerCount.erase( itr );
		}
	}
}

void GLCountRecorder::del_CurCountryPlayerCount ( const DWORD dwID )
{
	MAP_COUNTER_ITR itr = m_mapCurCountryPlayerCount.find(dwID);
	if ( m_mapCurCountryPlayerCount.end() != itr )
	{
		(*itr).second = (*itr).second-1;
		if ( (*itr).second < 0 )
		{
			(*itr).second = 0;

			sc::writeLogError( "[ Country Recorder ] [ Failed GLCountRecorder::remove_CurCountryPlayerCount!! ]" );
		}

		if ( (*itr).second == 0 )
		{
			m_mapCurCountryPlayerCount.erase( itr );
		}
	}	
}

void GLCountRecorder::del_CurSchoolPlayerCount ( const DWORD dwID )
{
	MAP_COUNTER_ITR itr = m_mapCurSchoolPlayerCount.find(dwID);
	if ( m_mapCurSchoolPlayerCount.end() != itr )
	{
		(*itr).second = (*itr).second-1;
		if ( (*itr).second < 0 )
		{
			(*itr).second = 0;

			sc::writeLogError( "[ Country Recorder ] [ Failed GLCountRecorder::remove_CurSchoolPlayerCount!! ]" );
		}

		if ( (*itr).second == 0 )
		{
			m_mapCurSchoolPlayerCount.erase( itr );
		}
	}
}


void GLCountRecorder::add_CurFactionPlayerCount ( const Faction::FactionID& sFactionID )
{
	MAP_FACTION_COUNTER_ITR itr = m_mapCurFactionPlayerCount.find(sFactionID);
	if ( m_mapCurFactionPlayerCount.end() == itr )
	{
		m_mapCurFactionPlayerCount.insert( std::make_pair(sFactionID, 0L) );
		itr = m_mapCurFactionPlayerCount.find(sFactionID);
	}

	(*itr).second = (*itr).second+1;
}

void GLCountRecorder::del_CurFactionPlayerCount ( const Faction::FactionID& sFactionID )
{
	MAP_FACTION_COUNTER_ITR itr = m_mapCurFactionPlayerCount.find(sFactionID);
	if ( m_mapCurFactionPlayerCount.end() != itr )
	{
		(*itr).second = (*itr).second-1;
		if ( (*itr).second < 0 )
		{
			(*itr).second = 0;

			sc::writeLogError( "[ Country Recorder ] [ Failed GLCountRecorder::remove_CurFactionPlayerCount!! ]" );
		}

		if ( (*itr).second == 0 )
		{
			m_mapCurFactionPlayerCount.erase( itr );
		}
	}
}