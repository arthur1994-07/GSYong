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
		// Minimum Game Participant ����;
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

		// Max Team Participant ����;
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

		// Contents Map ID ����;
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

		// Confirm Join ����;
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

		// Rating Table ����;
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

		// �켱�� �ʿ���� �ڵ�;
		// Error Log �� �߻��� ��� �����ؾ� �Ѵ�;
		// 60�ʿ� �ѹ��� �ڵ� Matching�� �Ѵ�;
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

		// ó���� �Ϸ�Ǿ����Ƿ� �ӽ� ���� �����Ѵ�;
		m_vecTempMatchingTeam.clear();

		// ���� �����Ѵ�;
		MatchingModeResume();
	}


	const bool CMatchingModeSustained::OnDestroy (
		DWORD dwInstanceMapID,
		bool bDestroyComplete )
	{
		// �� �ʿ� �ִ� ��� �༮���� ������ �Ѵ�;
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
				// ����;
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

			// �� ����;
			m_vecTempMatchingTeam.push_back( dwGroupID );
		}

		// ���� �����Ѵ� -> OnMapMultiCreate Callback ȣ��;
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

		// ���� ��⿭�� �ο��� ������ ��� Matching �� �����Ѵ�;
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
			// Matching ���� ��� Matching �� �����Ѵ�;
			if ( GetMatchingModeState() )
				return;

			// Matching ������ ����;
			MatchingModeYield();

			// ��Ī�� ����;
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

			// ��⿭���� �׷��� �����´�;
			for ( int i=0; i<m_dwMinimumGameParticipant; ++i )
			{
				DWORD dwGroupID = m_pMatchingInstance->GetGroupToMatchingNotPlay();
				if ( -1 == dwGroupID )
				{
					// ����;
					return;
				}

				// �� ����;
				m_vecTempMatchingTeam.push_back( dwGroupID );
			}

			// ���� �Ǵ�;
			// OnMapMultiCreate Callback�� ó���� �Ϸ�Ǹ�;
			// ���� Ǯ�� ���̴�;
			MatchingModeYield();

			// ���� �����Ѵ� -> OnMapMultiCreate Callback ȣ��;
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

		// �׷��� �� ������� ���´�;
		std::vector<DWORD> vecGroupMember;
		pGroupManager->SearchCharacters_MatchingGroupID( vecGroupMember, _dwGroupID );

		// �� ������� ������ ��հ��� ���Ѵ�;
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

		// ���� �ɸ� ���¶�� ��Ī���� �ʴ´�;
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

		// ��ü ������ ���̺� Ȯ��;
		for ( int i=0; i<m_vecRatingTable.size(); ++i )
		{
			SRATING_INFO& sRatingInfo = m_vecRatingTable.at( i );

			// ������ ���̺��� ��ȿ�� �׷��� ���� Ȯ���Ѵ�;
			int nValidGroupCount = 0;
			for ( int j=0; j<sRatingInfo.vecGroupList.size(); ++j )
			{
				DWORD dwTempGroupID = sRatingInfo.vecGroupList.at( j );

				if ( pGroupManager->IsValidMatchingGroup( dwTempGroupID ) )
				{
					nValidGroupCount++;
				}
			}

			// ������ ���̺��� �׷� ���� �ּ� ���� �����ڸ� ���� ���ϸ�;
			// �˻縦 ���� �ʴ´�;
			if ( nValidGroupCount < m_dwMinimumGameParticipant )
				continue;

			std::vector< DWORD > vecTempMatchingTeam;

			// �� ������ �׷� Ȯ��;
			for ( SRATING_INFO::VEC_GROUP_LIST_ITER iter =
				sRatingInfo.vecGroupList.begin();
				iter != sRatingInfo.vecGroupList.end(); )
			{
				DWORD dwTempGroupID = *iter;

				// Ư�� ������ ���̺� ���� �׷��� ��ȿ�Ѱ�?;
				if ( pGroupManager->IsValidMatchingGroup( dwTempGroupID ) )
				{
					vecTempMatchingTeam.push_back( dwTempGroupID );
				}

				// �׷��� �ӽ� ���� �־��ų� ��ȿ���� �ʰų�;
				// �� �� �ϳ��̹Ƿ� �ʱ�ȭ�Ѵ�;
				iter = sRatingInfo.vecGroupList.erase( iter );

				// ���� �ӽ� ���� ���߾� ���ٸ�;
				if ( vecTempMatchingTeam.size() >= m_dwMinimumGameParticipant )
				{
					// ������ ����� �ѹ� �� Ŭ����;
					m_vecTempMatchingTeam.clear();

					// ���߾��� �ӽ� ���� �����Ѵ�;
					for ( int j=0; j<vecTempMatchingTeam.size(); ++j )
					{
						m_vecTempMatchingTeam.push_back(
							vecTempMatchingTeam.at( j ) );
					}

					// ���� �Ǵ�;
					// OnMapMultiCreate Callback�� ó���� �Ϸ�Ǹ�;
					// ���� Ǯ�� ���̴�;
					MatchingModeYield();

					// ���� �����Ѵ� -> OnMapMultiCreate Callback ȣ��;
					m_pMatchingInstance->MapMultiCreate( m_sMapID.dwID, 1, 0 );

					return;
				}
			}

			/// �� �� ���� -> ���࿡ �´ٸ� ����;
			sc::writeLogError( "[ Matching Log ] [ Error Matching Game!!! ]" );

			/// ������� �ǵ����� -> ���� After Plan;
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