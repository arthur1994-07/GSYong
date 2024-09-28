#include "pch.h"

#include "../../RanLogic/Msg/GLContrlMatching.h"

#include "../Database/DBAction/DbActionMatchingSystem.h"

#include "../AgentServer/GLAgentServer.h"

#include "./MatchingJoinManager.h"

namespace MatchingSystem
{

	void CALLBACK CMatchingJoinManager::OnLoad ()
	{
		if ( false == GLUseFeatures::GetInstance().IsUsingTournament() )
			return;

		// Load from DB;
		DBActionLoadJoinCountList( m_mapCharJoinCount );
	}

	void CALLBACK CMatchingJoinManager::OnFrameMove (
		const float _fElapsedTime,
		const CTime& _sCurrentTime )
	{
		if ( false == GLUseFeatures::GetInstance().IsUsingTournament() )
			return;

		// Timer 가 이미 사용된 상태면 1시에 다시 초기화시킨다;
		if ( m_bUsedTimer )
		{
			if ( ( 1 != _sCurrentTime.GetHour() ) ||
				( 0 != _sCurrentTime.GetMinute() ) ||
				( 0 != _sCurrentTime.GetSecond() ) )
			{
				return;
			}

			m_bUsedTimer = false;
		}

		// Timer 가 사용되지 않은 상태면 0시에 사용시킨다;
		// Timer 를 사용한다는 것은 Join Count List 를 초기화한다는 것이다;
		if ( ( 0 != _sCurrentTime.GetHour() ) ||
			( 0 != _sCurrentTime.GetMinute() ) ||
			( 0 != _sCurrentTime.GetSecond() ) )
		{
			m_bUsedTimer = false;

			return;
		}

		m_bUsedTimer = true;

		// Join Count List 를 초기화한다;
		ResetJoinCountMap();
	}

	void CMatchingJoinManager::SyncState (
		const DWORD _dwClientID,
		const DWORD _dwChaDBNum )
	{
		if ( false == GLUseFeatures::GetInstance().IsUsingTournament() )
			return;

		if ( NULL == m_pServer )
			return;

		GLCharAG* pChar = m_pServer->GetCharByDbNum( _dwChaDBNum );
		if ( NULL == pChar )
			return;

		// 먼저 Default Message 를 보내서 Client 의 Joint Count 를 초기화해준다;
		GLMSG::NET_MATCHING_UPDATE_JOIN_COUNT_AC sMsg;
		m_pServer->SENDTOCLIENT( pChar->ClientSlot(), &sMsg );

		// 캐릭터의 Join Count 정보를 보낸다;
		MAP_CHAR_JOIN_COUNT_LIST_CITER citer = m_mapCharJoinCount.find( _dwChaDBNum );
		if ( citer != m_mapCharJoinCount.end() )
		{
			const VEC_JOIN_COUNT vecTempJoinCount = citer->second;

			for ( unsigned int i=0; i<vecTempJoinCount.size(); ++i )
			{
				const SJOIN_COUNT sTempJoinCount = vecTempJoinCount.at( i );
				
				GLMSG::NET_MATCHING_UPDATE_JOIN_COUNT_AC sMsg;
				sMsg.dwMatchingBaseID = sTempJoinCount.m_dwMatchingBaseID;
				sMsg.dwJoinCount = sTempJoinCount.m_dwJoinCount;

				m_pServer->SENDTOCLIENT( pChar->ClientSlot(), &sMsg );
			}
		}
	}

	const DWORD CMatchingJoinManager::GetJoinCount_DBNum (
		const MatchingBaseID _dwMatchingGameID,
		const DWORD _dwChaDBNum ) const
	{
		if ( false == GLUseFeatures::GetInstance().IsUsingTournament() )
			return 0;

		MAP_CHAR_JOIN_COUNT_LIST_CITER citer = m_mapCharJoinCount.find( _dwChaDBNum );
		if ( citer != m_mapCharJoinCount.end() )
		{
			const VEC_JOIN_COUNT vecTempJoinCount = citer->second;
			
			for ( unsigned int i=0; i<vecTempJoinCount.size(); ++i )
			{
				const SJOIN_COUNT sTempJoinCount = vecTempJoinCount.at( i );
				if ( _dwMatchingGameID == sTempJoinCount.m_dwMatchingBaseID )
				{
					// 캐릭터가 매칭에 들어간 횟수를 리턴한다;
					return sTempJoinCount.m_dwJoinCount;
				}
			}

			// 캐릭터가 한번도 들어간 적이 없던 매칭이라면 0 을 반환한다;
			return 0;
		}

		// 캐릭터가 전체 매칭에 한번도 들어간 적이 없다면 0 을 반환한다;
		return 0;
	}

	void CMatchingJoinManager::JoinMatching_DBNum (
		const MatchingBaseID _dwMatchingGameID,
		const DWORD _dwChaDBNum )
	{
		if ( false == GLUseFeatures::GetInstance().IsUsingTournament() )
			return;

		if ( NULL == m_pServer )
			return;

		GLCharAG* pChar = m_pServer->GetCharByDbNum( _dwChaDBNum );
		if ( NULL == pChar )
			return;

		// Server 처리;
		SJOIN_COUNT sJoinCount;

		MAP_CHAR_JOIN_COUNT_LIST_ITER iter = m_mapCharJoinCount.find( _dwChaDBNum );
		if ( iter != m_mapCharJoinCount.end() )
		{
			VEC_JOIN_COUNT& vecTempJoinCount = iter->second;

			for ( unsigned int i=0; i<vecTempJoinCount.size(); ++i )
			{
				SJOIN_COUNT& sTempJoinCount = vecTempJoinCount.at( i );
				if ( _dwMatchingGameID == sTempJoinCount.m_dwMatchingBaseID )
				{
					// 캐릭터가 매칭에 들어간 횟수를 올린다;
					sTempJoinCount.m_dwJoinCount++;

					// Update DB;
					DBActionUpdateJoinCount( _dwChaDBNum, sTempJoinCount );

					// Client 와 동기화;
					GLMSG::NET_MATCHING_UPDATE_JOIN_COUNT_AC sMsg;
					sMsg.dwMatchingBaseID = sTempJoinCount.m_dwMatchingBaseID;
					sMsg.dwJoinCount = sTempJoinCount.m_dwJoinCount;

					m_pServer->SENDTOCLIENT( pChar->ClientSlot(), &sMsg );

					return;
				}
			}

			// 캐릭터가 한번도 들어간 적이 없던 매칭이라면 처리한다;
			sJoinCount.m_dwMatchingBaseID = _dwMatchingGameID;
			sJoinCount.m_dwJoinCount = 1;	// 한번의 입장 세팅;

			vecTempJoinCount.push_back( sJoinCount );

			// Update DB;
			DBActionUpdateJoinCount( _dwChaDBNum, sJoinCount );

			// Client 와 동기화;
			GLMSG::NET_MATCHING_UPDATE_JOIN_COUNT_AC sMsg;
			sMsg.dwMatchingBaseID = sJoinCount.m_dwMatchingBaseID;
			sMsg.dwJoinCount = sJoinCount.m_dwJoinCount;

			m_pServer->SENDTOCLIENT( pChar->ClientSlot(), &sMsg );

			return;
		}

		// 캐릭터가 전체 매칭에 한번도 들어간 적이 없다면 처리한다;
		sJoinCount.m_dwMatchingBaseID = _dwMatchingGameID;
		sJoinCount.m_dwJoinCount = 1;

		VEC_JOIN_COUNT vecJoinCount;
		vecJoinCount.push_back( sJoinCount );

		m_mapCharJoinCount.insert(
			MAP_CHAR_JOIN_COUNT_LIST_VALUE(
			_dwChaDBNum,
			vecJoinCount ) );

		// Update DB;
		DBActionUpdateJoinCount( _dwChaDBNum, sJoinCount );

		// Client 와 동기화;
		GLMSG::NET_MATCHING_UPDATE_JOIN_COUNT_AC sMsg;
		sMsg.dwMatchingBaseID = sJoinCount.m_dwMatchingBaseID;
		sMsg.dwJoinCount = sJoinCount.m_dwJoinCount;

		m_pServer->SENDTOCLIENT( pChar->ClientSlot(), &sMsg );
	}

	const bool CMatchingJoinManager::ChargeJoinCount_DBNum (
		const MatchingBaseID _dwMatchingGameID,
		const DWORD _dwChaDBNum )
	{
		if ( false == GLUseFeatures::GetInstance().IsUsingTournament() )
			return false;

		if ( NULL == m_pServer )
			return false;

		GLCharAG* pChar = m_pServer->GetCharByDbNum( _dwChaDBNum );
		if ( NULL == pChar )
			return false;

		// Server 처리;
		SJOIN_COUNT sJoinCount;

		MAP_CHAR_JOIN_COUNT_LIST_ITER iter = m_mapCharJoinCount.find( _dwChaDBNum );
		if ( iter != m_mapCharJoinCount.end() )
		{
			VEC_JOIN_COUNT& vecTempJoinCount = iter->second;

			for ( unsigned int i=0; i<vecTempJoinCount.size(); ++i )
			{
				SJOIN_COUNT& sTempJoinCount = vecTempJoinCount.at( i );
				if ( _dwMatchingGameID == sTempJoinCount.m_dwMatchingBaseID )
				{
					// 한번도 들어간 적이 없다면;
					if ( 0 == sTempJoinCount.m_dwJoinCount )
						return false;

					// 캐릭터가 매칭에 들어간 횟수를 내린다;
					sTempJoinCount.m_dwJoinCount--;

					// Update DB;
					DBActionUpdateJoinCount( _dwChaDBNum, sTempJoinCount );

					// Client 와 동기화;
					GLMSG::NET_MATCHING_UPDATE_JOIN_COUNT_AC sMsg;
					sMsg.dwMatchingBaseID = sTempJoinCount.m_dwMatchingBaseID;
					sMsg.dwJoinCount = sTempJoinCount.m_dwJoinCount;

					m_pServer->SENDTOCLIENT( pChar->ClientSlot(), &sMsg );

					return true;
				}
			}
		}

		// 이미 가득찬 아이템을 사용했다;
		return false;
	}

	void CMatchingJoinManager::ResetJoinCountMap ()
	{
		if ( false == GLUseFeatures::GetInstance().IsUsingTournament() )
			return;

		m_mapCharJoinCount.clear();

		// Reset DB;
		DBActionResetJoinCountList();
	}

	void CMatchingJoinManager::DBActionLoadJoinCountList (
		MAP_CHAR_JOIN_COUNT_LIST& _refMapCharJoinCountList )
	{
		if ( NULL == m_pServer )
			return;

		m_pServer->AddGameAdoJob( db::DbActionPtr(
			new db::CMatchingJoinCountLoadAction( _refMapCharJoinCountList ) ) );
	}

	void CMatchingJoinManager::DBActionResetJoinCountList ()
	{
		if ( NULL == m_pServer )
			return;

		m_pServer->AddGameAdoJob( db::DbActionPtr(
			new db::CMatchingJoinCountResetAction( ) ) );
	}

	void CMatchingJoinManager::DBActionUpdateJoinCount (
		const DWORD _dwChaDBNum,
		const SJOIN_COUNT _sJoinCount )
	{
		if ( NULL == m_pServer )
			return;

		m_pServer->AddGameAdoJob( db::DbActionPtr(
			new db::CMatchingJoinCountUpdateAction( _dwChaDBNum, _sJoinCount ) ) );
	}

}