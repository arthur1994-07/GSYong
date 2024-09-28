#include "pch.h"

#include "./MatchingManager.h"

#include "./MatchingGameBase.h"
#include "./MatchingInstanceBase.h"

#include "./MatchingModeSustained.h"

namespace MatchingSystem
{
	const std::string MATCHING_MODE_MINIMUM_MEMBER	= "MinimumTeamMember";
	const std::string MATCHING_MODE_MAX_TEAM_MEMBER = "MaxTeamMember";
	const std::string MATCHING_MODE_CONFIRM_JOIN	= "ConfirmJoinState";

	const std::string MATCHING_MODE_RATING_TABLE	= "RatingTable";
	const std::string MATCHING_MODE_MAP_ID			= "BattleMapID";

	CMatchingModeSustained::CMatchingModeSustained ( MatchingInstanceBase* _pMatchingInstance
		, sc::LuaInstance* pScript )
		: IMatchingMode ()
		, m_pMatchingInstance ( _pMatchingInstance )
		, m_pScript ( pScript )
		, m_dwMinimumGameParticipant ( 2 )
		, m_dwMaxTeamParticipant ( 1 )
		, m_bConfirmJoin ( false )
	{

	}


	CMatchingModeSustained::~CMatchingModeSustained ()
	{

	}


	void CMatchingModeSustained::OnInitialize ()
	{
		// Minimum Game Participant 세팅;
		{
			LuaPlus::LuaObject objMinimumMember =
				m_pScript->GetGlobalValue_Lua( MATCHING_MODE_MINIMUM_MEMBER.c_str() );

			if ( false == objMinimumMember.IsNil() )
			{
				if ( true == objMinimumMember.IsNumber() )
				{
					m_dwMinimumGameParticipant = objMinimumMember.GetNumber();

					sc::writeLogInfo( sc::string::format(
						"[ Matching Log ] [ MATCHING_MODE_MINIMUM_MEMBER = %1% ]",
						m_dwMinimumGameParticipant ) );
				}
				else
				{
					m_dwMinimumGameParticipant = 2;

					sc::writeLogInfo( "[ Matching Log ] [ MATCHING_MODE_MINIMUM_MEMBER dose not Exist. ]" );
					sc::writeLogInfo( "[ Matching Log ] -> [ Setting MATCHING_MODE_MINIMUM_MEMBER default = 2 ]" );
				}
			}
			else
			{
				m_dwMinimumGameParticipant = 2;

				sc::writeLogInfo( "[ Matching Log ] [ MATCHING_MODE_MINIMUM_MEMBER dose not Exist. ]" );
				sc::writeLogInfo( "[ Matching Log ] -> [ MATCHING_MODE_MINIMUM_MEMBER default = 2 ]" );
			}		
		}

		// Max Team Participant 세팅;
		{
			LuaPlus::LuaObject objMaxTeamMember =
				m_pScript->GetGlobalValue_Lua( MATCHING_MODE_MAX_TEAM_MEMBER.c_str() );

			if ( false == objMaxTeamMember.IsNil() )
			{
				if ( true == objMaxTeamMember.IsNumber() )
				{
					m_dwMaxTeamParticipant = objMaxTeamMember.GetNumber();

					sc::writeLogInfo( sc::string::format(
						"[ Matching Log ] [ MATCHING_MODE_MAX_TEAM_MEMBER = %1% ]",
						m_dwMaxTeamParticipant ) );
				}
				else
				{
					m_dwMaxTeamParticipant = 1;

					sc::writeLogInfo( "[ Matching Log ] [ MATCHING_MODE_MAX_TEAM_MEMBER dose not Exist. ]" );
					sc::writeLogInfo( "[ Matching Log ] -> [ Setting MATCHING_MODE_MAX_TEAM_MEMBER default = 1 ]" );
				}
			}
			else
			{
				m_dwMaxTeamParticipant = 1;

				sc::writeLogInfo( "[ Matching Log ] [ MATCHING_MODE_MAX_TEAM_MEMBER dose not Exist. ]" );
				sc::writeLogInfo( "[ Matching Log ] -> [ MATCHING_MODE_MAX_TEAM_MEMBER default = 1 ]" );
			}		
		}

		// Contents Map ID 세팅;
		{
			LuaPlus::LuaObject objMapIDTable =
				m_pScript->GetGlobalValue_Lua( MATCHING_MODE_MAP_ID.c_str() );

			if ( false == objMapIDTable.IsNil() )
			{
				int scriptid = 0;

				WORD wMainMapID = 0;
				WORD wSubMapID = 0;

				LuaPlus::LuaTableIterator iter( objMapIDTable );
				wMainMapID = iter.GetValue().GetNumber();
				iter.Next();
				wSubMapID = iter.GetValue().GetNumber();
				iter.Next();

				m_sMapID = SNATIVEID( wMainMapID, wSubMapID );

				sc::writeLogInfo( sc::string::format(
					"[ Matching Log ] [ MATCHING_MODE_MAP_ID = ( %1%, %2% ) ]",
					wMainMapID, wSubMapID ) );
			}
			else
			{
				m_sMapID = SNATIVEID( 190, 0 );

				sc::writeLogInfo( "[ Matching Log ] [ MATCHING_MODE_MAP_ID dose not Exist. ]" );
				sc::writeLogInfo( "[ Matching Log ] -> [ MATCHING_MODE_MAP_ID default = ( 190, 0 ) ]" );
			}
		}

		// Confirm Join 세팅;
		{
			LuaPlus::LuaObject objConfirmJoin =
				m_pScript->GetGlobalValue_Lua( MATCHING_MODE_CONFIRM_JOIN.c_str() );

			if ( false == objConfirmJoin.IsNil() )
			{
				if ( true == objConfirmJoin.IsBoolean() )
				{
					m_bConfirmJoin = objConfirmJoin.GetBoolean();

					sc::writeLogInfo( sc::string::format(
						"[ Matching Log ] [ MATCHING_MODE_CONFIRM_JOIN = %1% ]",
						m_bConfirmJoin ? "true" : "false" ) );
				}
				else
				{
					m_bConfirmJoin = false;

					sc::writeLogInfo( "[ Matching Log ] [ MATCHING_MODE_CONFIRM_JOIN dose not Exist. ]" );
					sc::writeLogInfo( "[ Matching Log ] -> [ Setting MATCHING_MODE_CONFIRM_JOIN default = false ]" );
				}
			}
			else
			{
				m_bConfirmJoin = false;

				sc::writeLogInfo( "[ Matching Log ] [ MATCHING_MODE_MAX_TEAM_MEMBER dose not Exist. ]" );
				sc::writeLogInfo( "[ Matching Log ] -> [ MATCHING_MODE_MAX_TEAM_MEMBER default = false ]" );
			}		
		}

		// Rating Table 세팅;
		/*{
			LuaPlus::LuaObject objRatingTable =
				m_pScript->GetGlobalValue_Lua( MATCHING_MODE_RATING_TABLE.c_str() );

			if ( false == objRatingTable.IsNil() )
			{
				int scriptid = 0;

				WORD wMainMapID = 0;
				WORD wSubMapID = 0;

				for ( LuaPlus::LuaTableIterator iter( objRatingTable );
					iter; iter.Next() )
				{
					DWORD dwRatingMaxBase = iter.GetValue().GetNumber();
					AddRatingTable( dwRatingMaxBase );

					sc::writeLogInfo( sc::string::format(
						"[ Matching Log ] [ MATCHING_MODE_RATING_TABLE = %1% ]",
						dwRatingMaxBase ) );
				}
			}
			else
			{
				m_vecRatingTable.clear();

				sc::writeLogInfo( "[ Matching Log ] [ MATCHING_MODE_RATING_TABLE dose not Exist. ]" );
			}
		}*/
	}


	void CMatchingModeSustained::OnFrameMove ( float _fElapsedTime )
	{
		static float fTime = 0.f;
		fTime += _fElapsedTime;

		// 우선은 필요없는 코드;
		// Error Log 가 발생할 경우 적용해야 한다;
		// 60초에 한번씩 자동 Matching을 한다;
		/*if ( fTime >= 60.f )
		{
			_MatchingGame ();

			fTime = 0.f;
		}*/
	}


	bool CMatchingModeSustained::OnEventCustomMsg (
		const InstanceSystem::InstanceMapID _nInstanceMapID,
		const double _fParam1,
		const double _fParam2,
		const double _fParam3,
		const double _fParam4 )
	{
		return true;
	}


	void CMatchingModeSustained::OnMapCreate (
		DWORD _dwInstanceMapID,
		bool _bDestroyComplete )
	{

	}


	void CMatchingModeSustained::OnMapMultiCreate (
		RoomMultyReceipt* _RoomReceipt )
	{
		if ( NULL == m_pMatchingInstance )
			return;

		bool bMapCreate = false;
		DWORD dwInstanceMapID = 0;

		BOOST_FOREACH ( DWORD dwTempInstanceMApID, _RoomReceipt->_instanceMapID )
		{
			sc::writeLogInfo(
				sc::string::format(
				"[ Matching Log ] [ Completed Create Map / Instance Map ID = %1% ]",
				dwTempInstanceMApID ).c_str() );

			bMapCreate = true;
			dwInstanceMapID = dwTempInstanceMApID;
		}

		if ( false == bMapCreate )
		{
			sc::writeLogInfo( "[ Matching Log ] [ Failed Create Map ]" );
			return;
		}

		for ( int i=0; i<m_vecTempMatchingTeam.size(); ++i )
		{
			DWORD& dwTempGroupID = m_vecTempMatchingTeam.at( i );
			if ( i < m_dwMaxTeamParticipant )
			{
				m_pMatchingInstance->SendFaction( dwTempGroupID, 0, dwInstanceMapID );
				sc::writeLogInfo( "[ Matching Log ] [ Apply Faction 0 ]" );
			}
			else if ( m_dwMaxTeamParticipant <= i && i < m_dwMinimumGameParticipant )
			{
				m_pMatchingInstance->SendFaction( dwTempGroupID, 1, dwInstanceMapID );
				sc::writeLogInfo( "[ Matching Log ] [ Apply Faction 1 ]" );
			}
			else
			{
				sc::writeLogError( sc::string::format(
					"[ Matching Log ] [ Matching Team is overflow!!! %1%/%2% ]",
					i+1,
					m_dwMinimumGameParticipant ) );

				break;
			}

			m_pMatchingInstance->SetGroupToPlay( dwTempGroupID );
			m_pMatchingInstance->SendJoin(
				dwTempGroupID,
				dwInstanceMapID,
				Faction::GLActorID(),
				0.f, 0.f );

			m_pMatchingInstance->FinishGroup( dwTempGroupID );

			dwTempGroupID = MatchingSystem::INVALID_MATCHING_GROUP_ID;
		}

		// 처리가 완료되었으므로 임시 팀을 해제한다;
		m_vecTempMatchingTeam.clear();

		// 락을 해제한다;
		MatchingModeResume();
	}


	const bool CMatchingModeSustained::OnDestroy (
		DWORD dwInstanceMapID,
		bool bDestroyComplete )
	{
		// 이 맵에 있는 모든 녀석들을 나가게 한다;
		return false;
	}


	const bool CMatchingModeSustained::OnReset ()
	{
		return true;
	}


	void CMatchingModeSustained::OnReadyGroupAnswer (
		DWORD dwGroupID,
		bool bOK )
	{

	}


	void CMatchingModeSustained::OnReadyAnswer ( )
	{
		if ( NULL == m_pMatchingInstance )
			return;

		MatchingGameBase* pMatchingBase =
			m_pMatchingInstance->GetMatchingGameBase();
		if ( NULL == pMatchingBase )
			return;

		MatchingSystem::GLMatchingGroupManager* pGroupManager =
			pMatchingBase->getGroupManager();
		if ( NULL == pGroupManager )
			return;

		VEC_MATCHING_GROUP_ID vecMatchingGroupID;
		pGroupManager->SearchMatchingGroup_MatchingID(
			vecMatchingGroupID,
			m_pMatchingInstance->GetMatchingInstanceID() );
		for ( int i=0; i<vecMatchingGroupID.size(); ++i )
		{
			DWORD dwGroupID = vecMatchingGroupID.at( i );
			if ( -1 == dwGroupID )
			{
				// 실패;
				return;
			}

			if ( m_dwMinimumGameParticipant <= i )
			{
				sc::writeLogError( sc::string::format(
					"[ Matching Log ] [ Matching's group size is overflow!!! %1%/%2% ]",
					i+1,
					m_dwMinimumGameParticipant ) );

				break;
			}

			// 팀 세팅;
			m_vecTempMatchingTeam.push_back( dwGroupID );
		}

		// 맵을 생성한다 -> OnMapMultiCreate Callback 호출;
		m_pMatchingInstance->MapMultiCreate( m_sMapID.dwID, 1 );
	}


	const bool CMatchingModeSustained::OnReceiveError (
		DWORD dwErrorCode )
	{
		return true;
	}


	const bool CMatchingModeSustained::OnNoticeResult (
		const InstanceSystem::InstanceMapID _instanceID,
		const DWORD _FactionID,
		const BOOL _IsWinner )
	{
		return true;
	}


	void CMatchingModeSustained::StartMatching ()
	{
		if ( NULL == m_pMatchingInstance )
			return;

		//sc::writeLogInfo( "[ Matching Log ] [ Request for a 'Capture The Flag' ]" );

		// 아직 대기열에 인원이 부족할 경우 Matching 을 보류한다;
		int nWaitingQueueSize = m_pMatchingInstance->GetMatchingWaitingGroupCount();
		if ( nWaitingQueueSize < m_dwMinimumGameParticipant )
			return;
		else if ( nWaitingQueueSize > m_dwMinimumGameParticipant )
		{
			sc::writeLogInfo( sc::string::format(
				"[ Matching Log ] [ WaitingQueue : %1%, MinimumGameParticipant : %2%, WaitingQueue is big size. ]",
				nWaitingQueueSize,
				m_dwMinimumGameParticipant ) );
		}

		if ( m_bConfirmJoin )
		{
			// Matching 중일 경우 Matching 을 보류한다;
			if ( GetMatchingModeState() )
				return;

			// Matching 진행중 설정;
			MatchingModeYield();

			// 매칭에 저장;
			for ( int i=0; i<m_dwMinimumGameParticipant; ++i )
			{
				m_pMatchingInstance->GetGroupToMatchingNotPlay();
			}

			m_pMatchingInstance->SendToGroupAskJoin( 60.f );
		}
		else
		{
			MatchingGameBase* pMatchingBase =
				m_pMatchingInstance->GetMatchingGameBase();
			if ( NULL == pMatchingBase )
				return;

			MatchingSystem::GLMatchingGroupManager* pGroupManager =
				pMatchingBase->getGroupManager();
			if ( NULL == pGroupManager )
				return;

			// 대기열에서 그룹을 가져온다;
			for ( int i=0; i<m_dwMinimumGameParticipant; ++i )
			{
				DWORD dwGroupID = m_pMatchingInstance->GetGroupToMatchingNotPlay();
				if ( -1 == dwGroupID )
				{
					// 실패;
					return;
				}

				// 팀 세팅;
				m_vecTempMatchingTeam.push_back( dwGroupID );
			}

			// 락을 건다;
			// OnMapMultiCreate Callback의 처리가 완료되면;
			// 락이 풀릴 것이다;
			MatchingModeYield();

			// 맵을 생성한다 -> OnMapMultiCreate Callback 호출;
			m_pMatchingInstance->MapMultiCreate( m_sMapID.dwID, 1 );
		}
	}

	/*void CMatchingModeSustained::_CalculateRating ( DWORD _dwGroupID )
	{
		if ( NULL == m_pMatchingInstance )
			return;

		MatchingGameBase* pMatchingBase =
			m_pMatchingInstance->GetMatchingGameBase();
		if ( NULL == pMatchingBase )
			return;

		int nRatingSum = 0;
		int nRatingMemberCount = 0;
		int nRatingAverage = 0;

		MatchingSystem::GLMatchingGroupManager* pGroupManager =
			pMatchingBase->getGroupManager();
		if ( NULL == pGroupManager )
			return;

		// 그룹의 각 멤버들을 얻어온다;
		std::vector<DWORD> vecGroupMember;
		pGroupManager->SearchCharacters_MatchingGroupID( vecGroupMember, _dwGroupID );

		// 각 멤버들의 레이팅 평균값을 구한다;
		for ( int i=0; i<vecGroupMember.size(); ++i )
		{
			DWORD dwDbNum = vecGroupMember.at( i );

			int nRatingTemp = pMatchingBase->GetRating( dwDbNum );
			nRatingSum += nRatingTemp;

			if ( nRatingTemp != 0 )
				nRatingMemberCount++;
		}

		nRatingAverage = nRatingSum / nRatingMemberCount;

		for ( int i=0; i<m_vecRatingTable.size(); ++i )
		{
			SRATING_INFO& sRatingInfo = m_vecRatingTable.at( i );
			if ( sRatingInfo.dwMaxRating >= nRatingAverage )
			{
				sRatingInfo.vecGroupList.push_back( _dwGroupID );

				sc::writeLogInfo(
					sc::string::format(
					"[ Matching Log ] -> [ GroupID = %1% / Rating Group = %2% ]",
					_dwGroupID, sRatingInfo.dwMaxRating ).c_str() );
				break;
			}
		}
	}

	void CMatchingModeSustained::_MatchingGame ( )
	{
		if ( NULL == m_pMatchingInstance )
			return;

		// 락이 걸린 상태라면 매칭하지 않는다;
		if ( GetMatchingModeState() )
			return;

		MatchingGameBase* pMatchingBase =
			m_pMatchingInstance->GetMatchingGameBase();
		if ( NULL == pMatchingBase )
			return;

		MatchingSystem::GLMatchingGroupManager* pGroupManager =
			pMatchingBase->getGroupManager();
		if ( NULL == pGroupManager )
			return;

		sc::writeLogInfo(
			"[ Matching Log ] [ Matching Game ... / Capture The Flag ]" );

		// 전체 레이팅 테이블 확인;
		for ( int i=0; i<m_vecRatingTable.size(); ++i )
		{
			SRATING_INFO& sRatingInfo = m_vecRatingTable.at( i );

			// 레이팅 테이블의 유효한 그룹의 수를 확인한다;
			int nValidGroupCount = 0;
			for ( int j=0; j<sRatingInfo.vecGroupList.size(); ++j )
			{
				DWORD dwTempGroupID = sRatingInfo.vecGroupList.at( j );

				if ( pGroupManager->IsValidMatchingGroup( dwTempGroupID ) )
				{
					nValidGroupCount++;
				}
			}

			// 레이팅 테이블의 그룹 수가 최소 게임 참가자를 넘지 못하면;
			// 검사를 하지 않는다;
			if ( nValidGroupCount < m_dwMinimumGameParticipant )
				continue;

			std::vector< DWORD > vecTempMatchingTeam;

			// 각 레이팅 그룹 확인;
			for ( SRATING_INFO::VEC_GROUP_LIST_ITER iter =
				sRatingInfo.vecGroupList.begin();
				iter != sRatingInfo.vecGroupList.end(); )
			{
				DWORD dwTempGroupID = *iter;

				// 특정 레이팅 테이블에 속한 그룹이 유효한가?;
				if ( pGroupManager->IsValidMatchingGroup( dwTempGroupID ) )
				{
					vecTempMatchingTeam.push_back( dwTempGroupID );
				}

				// 그룹을 임시 팀에 넣었거나 유효하지 않거나;
				// 둘 중 하나이므로 초기화한다;
				iter = sRatingInfo.vecGroupList.erase( iter );

				// 만약 임시 팀이 갖추어 진다면;
				if ( vecTempMatchingTeam.size() >= m_dwMinimumGameParticipant )
				{
					// 만약을 대비해 한번 더 클리어;
					m_vecTempMatchingTeam.clear();

					// 갖추어진 임시 팀을 세팅한다;
					for ( int j=0; j<vecTempMatchingTeam.size(); ++j )
					{
						m_vecTempMatchingTeam.push_back(
							vecTempMatchingTeam.at( j ) );
					}

					// 락을 건다;
					// OnMapMultiCreate Callback의 처리가 완료되면;
					// 락이 풀릴 것이다;
					MatchingModeYield();

					// 맵을 생성한다 -> OnMapMultiCreate Callback 호출;
					m_pMatchingInstance->MapMultiCreate( m_sMapID.dwID, 1, 0 );

					return;
				}
			}

			/// 올 수 없다 -> 만약에 온다면 버그;
			sc::writeLogError( "[ Matching Log ] [ Error Matching Game!!! ]" );

			/// 원래대로 되돌린다 -> 버그 After Plan;
			for ( int j=0; j<vecTempMatchingTeam.size(); ++j )
			{
				sRatingInfo.vecGroupList.push_back(
					vecTempMatchingTeam.at( j ) );
			}
		}
	}*/

	void CMatchingModeSustained::SetInfoMatchingSustained (
		DWORD dwMaxTeamParticipant,
		DWORD dwMinimumGameParticipant,
		SNATIVEID sMapID,
		bool bConfirmJoin )
	{
		m_dwMaxTeamParticipant = dwMaxTeamParticipant;
		m_dwMinimumGameParticipant = dwMinimumGameParticipant;
		m_sMapID = sMapID;
		m_bConfirmJoin = bConfirmJoin;
	}

	/*bool CMatchingModeSustained::AddRatingTable ( DWORD dwMaxRating )
	{
		for ( int i=0; i<m_vecRatingTable.size(); ++i )
		{
			SRATING_INFO sTempRatingInfo = m_vecRatingTable.at( i );
			if ( dwMaxRating == sTempRatingInfo.dwMaxRating )
				return false;
		}

		SRATING_INFO sRatingInfo;
		sRatingInfo.dwMaxRating = dwMaxRating;

		m_vecRatingTable.push_back( sRatingInfo );

		return true;
	}

	bool CMatchingModeSustained::AddRatingTableMember (
		DWORD dwMaxRating, DWORD dwGroupID )
	{
		for ( int i=0; i<m_vecRatingTable.size(); ++i )
		{
			SRATING_INFO& sRatingInfo = m_vecRatingTable.at( i );
			if ( dwMaxRating == sRatingInfo.dwMaxRating )
			{
				sRatingInfo.vecGroupList.push_back( dwGroupID );

				return true;
			}
		}

		return false;
	}*/

}