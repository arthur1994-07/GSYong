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

		// Timer �� �̹� ���� ���¸� 1�ÿ� �ٽ� �ʱ�ȭ��Ų��;
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

		// Timer �� ������ ���� ���¸� 0�ÿ� ����Ų��;
		// Timer �� ����Ѵٴ� ���� Join Count List �� �ʱ�ȭ�Ѵٴ� ���̴�;
		if ( ( 0 != _sCurrentTime.GetHour() ) ||
			( 0 != _sCurrentTime.GetMinute() ) ||
			( 0 != _sCurrentTime.GetSecond() ) )
		{
			m_bUsedTimer = false;

			return;
		}

		m_bUsedTimer = true;

		// Join Count List �� �ʱ�ȭ�Ѵ�;
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

		// ���� Default Message �� ������ Client �� Joint Count �� �ʱ�ȭ���ش�;
		GLMSG::NET_MATCHING_UPDATE_JOIN_COUNT_AC sMsg;
		m_pServer->SENDTOCLIENT( pChar->ClientSlot(), &sMsg );

		// ĳ������ Join Count ������ ������;
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
					// ĳ���Ͱ� ��Ī�� �� Ƚ���� �����Ѵ�;
					return sTempJoinCount.m_dwJoinCount;
				}
			}

			// ĳ���Ͱ� �ѹ��� �� ���� ���� ��Ī�̶�� 0 �� ��ȯ�Ѵ�;
			return 0;
		}

		// ĳ���Ͱ� ��ü ��Ī�� �ѹ��� �� ���� ���ٸ� 0 �� ��ȯ�Ѵ�;
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

		// Server ó��;
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
					// ĳ���Ͱ� ��Ī�� �� Ƚ���� �ø���;
					sTempJoinCount.m_dwJoinCount++;

					// Update DB;
					DBActionUpdateJoinCount( _dwChaDBNum, sTempJoinCount );

					// Client �� ����ȭ;
					GLMSG::NET_MATCHING_UPDATE_JOIN_COUNT_AC sMsg;
					sMsg.dwMatchingBaseID = sTempJoinCount.m_dwMatchingBaseID;
					sMsg.dwJoinCount = sTempJoinCount.m_dwJoinCount;

					m_pServer->SENDTOCLIENT( pChar->ClientSlot(), &sMsg );

					return;
				}
			}

			// ĳ���Ͱ� �ѹ��� �� ���� ���� ��Ī�̶�� ó���Ѵ�;
			sJoinCount.m_dwMatchingBaseID = _dwMatchingGameID;
			sJoinCount.m_dwJoinCount = 1;	// �ѹ��� ���� ����;

			vecTempJoinCount.push_back( sJoinCount );

			// Update DB;
			DBActionUpdateJoinCount( _dwChaDBNum, sJoinCount );

			// Client �� ����ȭ;
			GLMSG::NET_MATCHING_UPDATE_JOIN_COUNT_AC sMsg;
			sMsg.dwMatchingBaseID = sJoinCount.m_dwMatchingBaseID;
			sMsg.dwJoinCount = sJoinCount.m_dwJoinCount;

			m_pServer->SENDTOCLIENT( pChar->ClientSlot(), &sMsg );

			return;
		}

		// ĳ���Ͱ� ��ü ��Ī�� �ѹ��� �� ���� ���ٸ� ó���Ѵ�;
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

		// Client �� ����ȭ;
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

		// Server ó��;
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
					// �ѹ��� �� ���� ���ٸ�;
					if ( 0 == sTempJoinCount.m_dwJoinCount )
						return false;

					// ĳ���Ͱ� ��Ī�� �� Ƚ���� ������;
					sTempJoinCount.m_dwJoinCount--;

					// Update DB;
					DBActionUpdateJoinCount( _dwChaDBNum, sTempJoinCount );

					// Client �� ����ȭ;
					GLMSG::NET_MATCHING_UPDATE_JOIN_COUNT_AC sMsg;
					sMsg.dwMatchingBaseID = sTempJoinCount.m_dwMatchingBaseID;
					sMsg.dwJoinCount = sTempJoinCount.m_dwJoinCount;

					m_pServer->SENDTOCLIENT( pChar->ClientSlot(), &sMsg );

					return true;
				}
			}
		}

		// �̹� ������ �������� ����ߴ�;
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