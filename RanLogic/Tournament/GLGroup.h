#pragma once

#include <math.h>
#include <boost/foreach.hpp>

#include "./GLTournamentTreeDefine.h"

namespace MatchingSystem
{

	/************************************************************************/
	/* Matching Group;                                                      */
	/************************************************************************/
	class GLMatchingGroup
	{
	public:
		GLMatchingGroup ( DWORD dwMatchingGroupID )
			: m_dwMatchingID ( INVALID_MATCHING_ID )
			, m_dwMatchingGroupID ( INVALID_MATCHING_GROUP_ID )
		{
			m_dwMatchingGroupID = dwMatchingGroupID;
		}

		~GLMatchingGroup ( )
		{
			m_vecCharacter.clear();
		}

	public:
		// Matching Group 파괴 Callback Function;
		inline void OnDestroy ( )
		{
			m_vecCharacter.clear();

			m_dwMatchingID = INVALID_MATCHING_ID;
			m_dwMatchingGroupID = INVALID_MATCHING_GROUP_ID;
		}

		// Matching Group 의 유효성 검사;
		inline bool IsValidMatchingGroup ( )
		{
			if ( INVALID_MATCHING_ID == m_dwMatchingID )
				return false;

			if ( INVALID_MATCHING_GROUP_ID == m_dwMatchingGroupID )
				return false;

			return true;
		}
		
	public:
		/************************************************************************/
		/* Managing Character In Matching Group                                 */
		/************************************************************************/
		inline bool IsExistCharacter ( DWORD dwChaDBNum )
		{
			BOOST_FOREACH ( DWORD dwTempChaDBNum, m_vecCharacter )
			{
				if ( dwTempChaDBNum == dwChaDBNum )
					return true;
			}

			return false;
		}

		inline bool JoinCharacter ( DWORD dwChaDBNum )
		{
			if ( IsExistCharacter( dwChaDBNum ) )
				return false;

			m_vecCharacter.push_back( dwChaDBNum );

			return true;
		}

		inline bool DropOutCharacter ( DWORD dwChaDBNum )
		{
			VEC_CHARACTER_DBNUM_ITER biter = m_vecCharacter.begin();
			VEC_CHARACTER_DBNUM_ITER eiter = m_vecCharacter.end();

			for ( ; biter != eiter; ++biter )
			{
				if ( *biter == dwChaDBNum )
				{
					m_vecCharacter.erase( biter );
					return true;
				}
			}

			return false;
		}

		inline VEC_CHARACTER_DBNUM& GetCharacterList ( )
		{
			return m_vecCharacter;
		}

		inline DWORD GetMatchingGroupChairman ( )
		{
			if ( m_vecCharacter.empty() )
				return INVALID_MATCHING_GROUP_ID;

			return m_vecCharacter.at( 0 );	
		}

		inline DWORD GetCountCharacter ( )
		{
			return m_vecCharacter.size();
		}

	public:
		/************************************************************************/
		/* Getter                                                               */
		/************************************************************************/
		inline const DWORD GetMatchingGroupID ( ) { return m_dwMatchingGroupID; }
		inline const DWORD GetMatchingID ( ) { return m_dwMatchingID; }

	public:
		/************************************************************************/
		/* Setter                                                               */
		/************************************************************************/
		inline void SetMatchingGroupID ( DWORD dwMatchingGroupID ) { m_dwMatchingGroupID = dwMatchingGroupID; }
		inline void SetMatchingID ( DWORD dwMatchingID ) { m_dwMatchingID = dwMatchingID; }

	private:
		// Matching Group 의 Character List;
		VEC_CHARACTER_DBNUM		m_vecCharacter;

		// Matching Group 이 속하는 Matching 의 ID;
		DWORD					m_dwMatchingID;

		// Matching Group 의 ID;
		DWORD					m_dwMatchingGroupID;
	};

}