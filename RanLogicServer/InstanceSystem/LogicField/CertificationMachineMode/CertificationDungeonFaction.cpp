#include "pch.h"

#include "../IInstanceFieldMode.h"

#include "./InstanceDungeon_CertificationMachine.h"

#include "./CertificationDungeonFaction.h"

namespace InstanceSystem
{

	void CCertificationDungeonFaction::Initialize ( void )
	{
		// 진영정보 초기화;
		m_dwMemberCount = 0;
		m_dwPoint = 0;
		m_dwCertifyCount = 0;
		m_bAddPoint = false;
		m_vecCertificationMachine.clear();

		// 진영 활성화;
		m_bValidation = true;
	}


	const bool CCertificationDungeonFaction::Join ( void )
	{
		// 최초 입장의 경우 진영정보를 초기화한다;
		if ( false == m_bValidation )
		{	
			Initialize();
		}

		// 멤버수를 증가시킨다;
		m_dwMemberCount++;

		return true;
	}


	void CCertificationDungeonFaction::Out ( void )
	{
		// 멤버수를 감소시킨다;
		m_dwMemberCount--;
	}


	const bool CCertificationDungeonFaction::StartCertification (
		const DWORD _dwMachineID )
	{
		for ( int i=0; i<m_vecCertificationMachine.size(); ++i )
		{
			const DWORD dwTempMachineID = m_vecCertificationMachine.at( i );
			if ( dwTempMachineID == _dwMachineID )
				return false;
		}

		return true;
	}


	void CCertificationDungeonFaction::SuccessCertification (
		const DWORD _dwMachineID )
	{
		// AddPoint 상태를 초기화한다;
		InactivateAddPoint();

		// 인증횟수를 늘린다;
		m_dwCertifyCount++;

		// 인증한 Machine 을 등록한다;
		m_vecCertificationMachine.push_back( _dwMachineID );
	}


	void CCertificationDungeonFaction::Syncronize (
		const DWORD _dwPoint,
		const DWORD _dwCertifyCount )
	{
		// 포인트와 인증횟수를 동기화한다;
		m_dwPoint = _dwPoint;
		m_dwCertifyCount = _dwCertifyCount;
	}


	void CCertificationDungeonFaction::SyncronizeCertificationMachine (
		const DWORD _dwMachineID )
	{
		// 인증한 Machine 을 동기화한다;
		m_vecCertificationMachine.push_back( _dwMachineID );
	}


	void CCertificationDungeonFaction::SendSyncronizeMessage ( void ) const
	{
		if ( NULL == m_pInstanceField )
			return;

		// 포인트와 인증횟수를 동기화할 수 있도록 메시지를 보낸다;
		m_pInstanceField->SendCustomMessage(
			EM_MESSAGE_AGENT_BASE,
			m_pInstanceField->GetKeyMapID().Mid(),
			CInstanceDungeon_CertificationMachine::EMCUSTOM_MSG_CERTIFICATION_MODE_UPDATE_POINT,
			m_dwFactionID,
			m_dwPoint,
			m_dwCertifyCount );
	}


	void CCertificationDungeonFaction::CertificationMachineNeutralization (
		const DWORD _dwMachineID )
	{
		for ( VEC_CERTIFICATION_MACHINE_ITER iter = m_vecCertificationMachine.begin();
			iter != m_vecCertificationMachine.end(); )
		{
			DWORD dwTempMachineID = *iter;
			if ( _dwMachineID == dwTempMachineID )
			{
				iter = m_vecCertificationMachine.erase( iter );
			}
			else
			{
				++iter;
			}
		}
	}


	void CCertificationDungeonFaction::ActivateAddPoint_CertificationMachine ( void )
	{
		if ( NULL == m_pInstanceField )
			return;

		for ( VEC_CERTIFICATION_MACHINE_ITER iter = m_vecCertificationMachine.begin();
			iter != m_vecCertificationMachine.end(); ++iter )
		{
			DWORD dwTempMachineID = *iter;

			m_pInstanceField->RegistTimer(
				0,
				CertificationMachineMode::EMTIMER_ID_CERTIFICATION_MACHINE_ACTIVATE_ADDPOINT,
				dwTempMachineID );
		}
	}


	void CCertificationDungeonFaction::InactivateAddPoint_CertificationMachine ( void )
	{
		if ( NULL == m_pInstanceField )
			return;

		for ( VEC_CERTIFICATION_MACHINE_ITER iter = m_vecCertificationMachine.begin();
			iter != m_vecCertificationMachine.end(); ++iter )
		{
			DWORD dwTempMachineID = *iter;

			m_pInstanceField->RegistTimer(
				0,
				CertificationMachineMode::EMTIMER_ID_CERTIFICATION_MACHINE_INACTIVATE_ADDPOINT,
				dwTempMachineID );
		}
	}




	void CCertificationDungeonFactionManager::Join (
		const DWORD _dwFactionID,
		const bool _bClub )
	{
		bool bFactionInitialize = true;

		MAP_CERTIFICATION_DUNGEON_FACTION_ITER iterFaction = m_mapCertificationDungeonFaction.find( _dwFactionID );
		if ( iterFaction != m_mapCertificationDungeonFaction.end() )
		{
			CCertificationDungeonFaction& sFaction = iterFaction->second;
			sFaction.Join();
		}
		else
		{
			// 처음들어오는 진영의 경우 새로 생성한다;
			CCertificationDungeonFaction sFaction( m_pInstanceField, m_pDungeon, _dwFactionID );
			sFaction.Join();

			CertificationMachineMode::SFACTION_POINT sFactionPoint( _dwFactionID, 0 );
			m_vecFactionPoint.push_back( sFactionPoint );

			// 처음들어오는 진영의 경우 진영관계 설정이 필요하다;
			bFactionInitialize = false;

			m_mapCertificationDungeonFaction.insert(
				MAP_CERTIFICATION_DUNGEON_FACTION_VALUE( _dwFactionID, sFaction ) );
		}

		if ( false == bFactionInitialize )
		{
			_InitailizeRelation( _dwFactionID, _bClub );
		}
	}


	void CCertificationDungeonFactionManager::Out (
		const DWORD _dwFactionID )
	{
		MAP_CERTIFICATION_DUNGEON_FACTION_ITER iterFaction = m_mapCertificationDungeonFaction.find( _dwFactionID );
		if ( iterFaction != m_mapCertificationDungeonFaction.end() )
		{
			CCertificationDungeonFaction& sFaction = iterFaction->second;
			sFaction.Out();
		}
	}


	const bool CCertificationDungeonFactionManager::StartCertification (
		const DWORD _dwFactionID,
		const DWORD _dwMachineID,
		const DWORD _dwCapturedFactionID )
	{
		if ( NULL == m_pInstanceField )
			return false;

		// 같은 진영일 경우 실패;
		if ( _dwFactionID == _dwCapturedFactionID )
			return false;

		// 이미 인증중인 인증기인지 확인;
		MAP_CERTIFICATION_DUNGEON_FACTION_ITER iterFaction = m_mapCertificationDungeonFaction.find( _dwFactionID );
		if ( iterFaction != m_mapCertificationDungeonFaction.end() )
		{
			CCertificationDungeonFaction& sFaction = iterFaction->second;
			return sFaction.StartCertification( _dwMachineID );
		}

		return false;
	}


	void CCertificationDungeonFactionManager::SuccessCertification (
		const DWORD _dwFactionID,
		const DWORD _dwMachineID )
	{
		MAP_CERTIFICATION_DUNGEON_FACTION_ITER iterFaction = m_mapCertificationDungeonFaction.find( _dwFactionID );
		if ( iterFaction != m_mapCertificationDungeonFaction.end() )
		{
			CCertificationDungeonFaction& sFaction = iterFaction->second;
			sFaction.SuccessCertification( _dwMachineID );
		}
	}


	void CCertificationDungeonFactionManager::ActivateAddPoint (
		const DWORD _dwFactionID )
	{
		MAP_CERTIFICATION_DUNGEON_FACTION_ITER iterFaction = m_mapCertificationDungeonFaction.find( _dwFactionID );
		if ( iterFaction != m_mapCertificationDungeonFaction.end() )
		{
			CCertificationDungeonFaction& sFaction = iterFaction->second;
			sFaction.ActivateAddPoint();
		}
	}


	void CCertificationDungeonFactionManager::InactivateAddPoint (
		const DWORD _dwFactionID )
	{
		MAP_CERTIFICATION_DUNGEON_FACTION_ITER iterFaction = m_mapCertificationDungeonFaction.find( _dwFactionID );
		if ( iterFaction != m_mapCertificationDungeonFaction.end() )
		{
			CCertificationDungeonFaction& sFaction = iterFaction->second;
			sFaction.InactivateAddPoint();
		}
	}


	void CCertificationDungeonFactionManager::ActivateAddPointBuff ( const DWORD _dwFactionID )
	{
		MAP_CERTIFICATION_DUNGEON_FACTION_ITER iterFaction = m_mapCertificationDungeonFaction.find( _dwFactionID );
		if ( iterFaction != m_mapCertificationDungeonFaction.end() )
		{
			CCertificationDungeonFaction& sFaction = iterFaction->second;
			sFaction.ActivateAddPointBuff();
		}
	}


	void CCertificationDungeonFactionManager::InactivateAddPointBuff ( const DWORD _dwFactionID )
	{
		MAP_CERTIFICATION_DUNGEON_FACTION_ITER iterFaction = m_mapCertificationDungeonFaction.find( _dwFactionID );
		if ( iterFaction != m_mapCertificationDungeonFaction.end() )
		{
			CCertificationDungeonFaction& sFaction = iterFaction->second;
			sFaction.InactivateAddPointBuff();

			sFaction.InactivateAddPoint_CertificationMachine();
		}
	}


	void CCertificationDungeonFactionManager::ActivateAddPoint_CertificationMachine ( const DWORD _dwFactionID )
	{
		MAP_CERTIFICATION_DUNGEON_FACTION_ITER iterFaction = m_mapCertificationDungeonFaction.find( _dwFactionID );
		if ( iterFaction != m_mapCertificationDungeonFaction.end() )
		{
			CCertificationDungeonFaction& sFaction = iterFaction->second;
			sFaction.ActivateAddPoint_CertificationMachine();
		}
	}


	const bool CCertificationDungeonFactionManager::UpdatePoint (
		const CertificationMachineMode::MAP_FACTION_POINT& _mapFactionPoint )
	{
		if ( NULL == m_pDungeon )
			return false;

		if ( NULL == m_pInstanceField )
			return false;

		// 진영들의 Point 를 갱신한다;
		CertificationMachineMode::MAP_FACTION_POINT_CITER pointIter = _mapFactionPoint.begin();
		CertificationMachineMode::MAP_FACTION_POINT_CITER eIter = _mapFactionPoint.end();
		for ( ; pointIter != eIter; ++pointIter )
		{
			const DWORD dwFactionID = pointIter->first;
			const DWORD dwPoint = pointIter->second;

			MAP_CERTIFICATION_DUNGEON_FACTION_ITER iterFaction = m_mapCertificationDungeonFaction.find( dwFactionID );
			if ( iterFaction != m_mapCertificationDungeonFaction.end() )
			{
				CCertificationDungeonFaction& sFaction = iterFaction->second;
				sFaction.UpdatePoint( dwPoint );

				for ( int i=0; i<m_vecFactionPoint.size(); ++i )
				{
					CertificationMachineMode::SFACTION_POINT& sFactionPoint = m_vecFactionPoint.at( i );
					if ( sFactionPoint.dwFactionID == dwFactionID )
					{
						sFactionPoint.dwFactionPoint = sFaction.GetPoint();
						sFactionPoint.dwFactionRealPoint = sFaction.GetRealPoint();
					}
				}
			}
		}

		return _SortFactionRanking();
	}


	const bool CCertificationDungeonFactionManager::UpdatePointDirect (
		const DWORD _dwFactionID,
		const DWORD _dwPoint )
	{
		if ( NULL == m_pDungeon )
			return false;

		if ( NULL == m_pInstanceField )
			return false;

		MAP_CERTIFICATION_DUNGEON_FACTION_ITER iterFaction = m_mapCertificationDungeonFaction.find( _dwFactionID );
		if ( iterFaction != m_mapCertificationDungeonFaction.end() )
		{
			CCertificationDungeonFaction& sFaction = iterFaction->second;
			sFaction.UpdatePoint( _dwPoint );

			for ( int i=0; i<m_vecFactionPoint.size(); ++i )
			{
				CertificationMachineMode::SFACTION_POINT& sFactionPoint = m_vecFactionPoint.at( i );
				if ( sFactionPoint.dwFactionID == _dwFactionID )
				{
					sFactionPoint.dwFactionPoint = sFaction.GetPoint();
					sFactionPoint.dwFactionRealPoint = sFaction.GetRealPoint();
				}
			}
		}

		return _SortFactionRanking();
	}


	void CCertificationDungeonFactionManager::GameEnd ( void )
	{
		if ( NULL == m_pInstanceField )
			return;

		// 모든 진영 중립화;
		_NeutralizationAllFaction();

		// Result UI;
		m_pInstanceField->UI_CertificationMachineMode_Result_Begin();
		for ( int i=0; i<m_vecFactionPoint.size(); ++i )
		{
			const CertificationMachineMode::SFACTION_POINT& sFactionPoint = m_vecFactionPoint.at( i );

			MAP_CERTIFICATION_DUNGEON_FACTION_ITER iterFaction = m_mapCertificationDungeonFaction.find( sFactionPoint.dwFactionID );
			if ( iterFaction != m_mapCertificationDungeonFaction.end() )
			{
				const CCertificationDungeonFaction& sFaction = iterFaction->second;

				m_pInstanceField->UI_CertificationMachineMode_Result_Info(
					sFactionPoint.dwFactionID,
					sFactionPoint.dwFactionPoint,
					sFactionPoint.dwRanking,
					sFaction.GetCertificationMachineCount(),
					sFaction.GetCertifyCount() );
			}
		}
		m_pInstanceField->UI_CertificationMachineMode_Result_End();

		// Result Log;
		CertificationMachineMode::SRESULT_LOG arrResultLog[ 3 ];

		for ( int i=0; i<m_vecFactionPoint.size(); ++i )
		{
			if ( 3 <= i )
				break;

			const CertificationMachineMode::SFACTION_POINT& sFactionPoint = m_vecFactionPoint.at( i );

			MAP_CERTIFICATION_DUNGEON_FACTION_ITER iterFaction = m_mapCertificationDungeonFaction.find( sFactionPoint.dwFactionID );
			if ( iterFaction != m_mapCertificationDungeonFaction.end() )
			{
				const CCertificationDungeonFaction& sFaction = iterFaction->second;

				arrResultLog[ i ].dwFactionID = sFactionPoint.dwFactionID;
				arrResultLog[ i ].dwPoint = sFactionPoint.dwFactionPoint;
				arrResultLog[ i ].dwCertificationMachineCount = sFaction.GetCertificationMachineCount();
				arrResultLog[ i ].dwCertifyCount = sFaction.GetCertifyCount();
			}
		}

		m_pInstanceField->Log_CertificationMachineMode_Result(
			arrResultLog[ 0 ].dwFactionID,
			arrResultLog[ 0 ].dwPoint,
			arrResultLog[ 0 ].dwCertificationMachineCount,
			arrResultLog[ 0 ].dwCertifyCount,
			arrResultLog[ 1 ].dwFactionID,
			arrResultLog[ 1 ].dwPoint,
			arrResultLog[ 1 ].dwCertificationMachineCount,
			arrResultLog[ 1 ].dwCertifyCount,
			arrResultLog[ 2 ].dwFactionID,
			arrResultLog[ 2 ].dwPoint,
			arrResultLog[ 2 ].dwCertificationMachineCount,
			arrResultLog[ 2 ].dwCertifyCount );
	}


	const bool CCertificationDungeonFactionManager::GameReward ( void )
	{
		if ( NULL == m_pInstanceField )
			return false;

		bool bReturn = false;
		for ( int i=0; i<m_vecFactionPoint.size(); ++i )
		{
			const CertificationMachineMode::SFACTION_POINT& sFactionPoint = m_vecFactionPoint.at( i );
			if ( 1 == sFactionPoint.dwRanking )
			{
				m_pInstanceField->RegistTimer(
					0,
					CertificationMachineMode::EMTIMER_ID_REWARD_WINNER,
					sFactionPoint.dwFactionID );

				bReturn = true;
			}
			else
			{
				if ( m_pDungeon->NUM_MINIMUM_REWARD_POINT <= sFactionPoint.dwFactionPoint )
				{
					m_pInstanceField->RegistTimer(
						0,
						CertificationMachineMode::EMTIMER_ID_REWARD_PARTICIPANT,
						sFactionPoint.dwFactionID,
						sFactionPoint.dwFactionPoint );
				}
			}
		}

		return bReturn;
	}


	void CCertificationDungeonFactionManager::Syncronize (
		const DWORD _dwFactionID,
		const DWORD _dwPoint,
		const DWORD _dwCertifyCount )
	{
		if ( MAXDWORD == _dwFactionID )
			return;

		MAP_CERTIFICATION_DUNGEON_FACTION_ITER iterFaction = m_mapCertificationDungeonFaction.find( _dwFactionID );
		if ( iterFaction != m_mapCertificationDungeonFaction.end() )
		{
			CCertificationDungeonFaction& sFaction = iterFaction->second;
			sFaction.Syncronize( _dwPoint, _dwCertifyCount );

			for ( int i=0; i<m_vecFactionPoint.size(); ++i )
			{
				CertificationMachineMode::SFACTION_POINT& sTempFactionPoint = m_vecFactionPoint.at( i );
				if ( sTempFactionPoint.dwFactionID == _dwFactionID )
				{
					sTempFactionPoint.dwFactionRealPoint = sFaction.GetPoint();
					sTempFactionPoint.dwFactionRealPoint = sFaction.GetRealPoint();
				}
			}
		}
		else
		{
			// 처음들어오는 진영의 경우 새로 생성한다;
			CCertificationDungeonFaction sFaction( m_pInstanceField, m_pDungeon, _dwFactionID );
			sFaction.Join();

			// 동기화한다;
			sFaction.Syncronize( _dwPoint, _dwCertifyCount );

			CertificationMachineMode::SFACTION_POINT sFactionPoint( _dwFactionID, _dwPoint );
			sFactionPoint.dwFactionRealPoint = sFaction.GetRealPoint();
			m_vecFactionPoint.push_back( sFactionPoint );

			// 처음들어오는 진영의 경우 진영관계 설정이 필요하다;
			m_mapCertificationDungeonFaction.insert(
				MAP_CERTIFICATION_DUNGEON_FACTION_VALUE( _dwFactionID, sFaction ) );

			// 관계를 초기화한다;
			_InitailizeRelation( _dwFactionID, m_pDungeon->IsClubFaction() );	
		}
	}


	void CCertificationDungeonFactionManager::SyncronizeCertificationMachine ( const DWORD _dwMachineID, const DWORD _dwFactionID )
	{
		if ( MAXDWORD == _dwFactionID )
			return;

		MAP_CERTIFICATION_DUNGEON_FACTION_ITER iterFaction = m_mapCertificationDungeonFaction.find( _dwFactionID );
		if ( iterFaction != m_mapCertificationDungeonFaction.end() )
		{
			CCertificationDungeonFaction& sFaction = iterFaction->second;
			sFaction.SyncronizeCertificationMachine( _dwMachineID );
		}
		// 무조건 생성되어 있어야 한다;
		// 즉, 진영을 먼저 동기화하고 인증기가 동기화된다;
		/*else
		{
			// 처음들어오는 진영의 경우 새로 생성한다;
			CCertificationDungeonFaction sFaction( m_pInstanceField, m_pDungeon, _dwFactionID );
			sFaction.Join();

			// 동기화한다;
			sFaction.SyncronizeCertificationMachine( _dwMachineID );

			CertificationMachineMode::SFACTION_POINT sFactionPoint( _dwFactionID, 0 );
			m_vecFactionPoint.push_back( sFactionPoint );

			// 처음들어오는 진영의 경우 진영관계 설정이 필요하다;
			m_mapCertificationDungeonFaction.insert(
				MAP_CERTIFICATION_DUNGEON_FACTION_VALUE( _dwFactionID, sFaction ) );

			// 관계를 초기화한다;
			_InitailizeRelation( _dwFactionID, m_pDungeon->IsClubFaction() );	
		}*/
	}


	void CCertificationDungeonFactionManager::SendSyncronizeMessage ( void )
	{
		MAP_CERTIFICATION_DUNGEON_FACTION_ITER iterFaction = m_mapCertificationDungeonFaction.begin();
		MAP_CERTIFICATION_DUNGEON_FACTION_ITER eIter = m_mapCertificationDungeonFaction.end();
		for ( ; iterFaction != eIter; ++iterFaction )
		{
			const CCertificationDungeonFaction& sFaction = iterFaction->second;
			if ( false == sFaction.IsValidation() )
				continue;

			sFaction.SendSyncronizeMessage();
		}
	}


	void CCertificationDungeonFactionManager::CertificationMachineNeutralization (
		const DWORD _dwFactionID,
		const DWORD _dwMachineID )
	{
		MAP_CERTIFICATION_DUNGEON_FACTION_ITER iterFaction = m_mapCertificationDungeonFaction.find( _dwFactionID );
		if ( iterFaction != m_mapCertificationDungeonFaction.end() )
		{
			CCertificationDungeonFaction& sFaction = iterFaction->second;
			sFaction.CertificationMachineNeutralization( _dwMachineID );
		}
	}


	const bool CCertificationDungeonFactionManager::IsAddPoint (
		const DWORD _dwFactionID )
	{
		MAP_CERTIFICATION_DUNGEON_FACTION_ITER iterFaction = m_mapCertificationDungeonFaction.find( _dwFactionID );
		if ( iterFaction != m_mapCertificationDungeonFaction.end() )
		{
			const CCertificationDungeonFaction& sFaction = iterFaction->second;
			return sFaction.IsAddPoint();
		}

		return false;
	}


	const bool CCertificationDungeonFactionManager::IsAddPointBuff ( const DWORD _dwFactionID )
	{
		MAP_CERTIFICATION_DUNGEON_FACTION_ITER iterFaction = m_mapCertificationDungeonFaction.find( _dwFactionID );
		if ( iterFaction != m_mapCertificationDungeonFaction.end() )
		{
			const CCertificationDungeonFaction& sFaction = iterFaction->second;
			return sFaction.IsAddPointBuff();
		}

		return false;
	}


	const DWORD CCertificationDungeonFactionManager::GetPoint (
		const DWORD _dwFactionID )
	{
		MAP_CERTIFICATION_DUNGEON_FACTION_ITER iterFaction = m_mapCertificationDungeonFaction.find( _dwFactionID );
		if ( iterFaction != m_mapCertificationDungeonFaction.end() )
		{
			const CCertificationDungeonFaction& sFaction = iterFaction->second;
			return sFaction.GetPoint();
		}

		return 0;
	}


	const DWORD CCertificationDungeonFactionManager::GetCertifyTime (
		const DWORD _dwFactionID )
	{
		DWORD dwMemberCount = 0;
		MAP_CERTIFICATION_DUNGEON_FACTION_ITER iterFaction = m_mapCertificationDungeonFaction.find( _dwFactionID );
		if ( iterFaction != m_mapCertificationDungeonFaction.end() )
		{
			const CCertificationDungeonFaction& sFaction = iterFaction->second;
			dwMemberCount = sFaction.GetMemberCount();
		}

		if ( 0 <= dwMemberCount && dwMemberCount <= 10 )
			return m_pDungeon->TIME_CERTIFY_TIME;
		else
			return m_pDungeon->TIME_CERTIFY_TIME + ( ( dwMemberCount - 10 ) * 0.5f );
	}


	void CCertificationDungeonFactionManager::_InitailizeRelation (
		const DWORD _dwFactionID,
		const bool _bClub )
	{
		if ( NULL == m_pInstanceField )
			return;

		// 각 팀의 안전진영과는 서로 아군이다;
		m_pInstanceField->SetRelation(
			Faction::FactionID( Faction::EMFACTION_TEAM, _dwFactionID ),
			Faction::FactionID( Faction::EMFACTION_SAFE_TEAM, _dwFactionID ),
			Faction::EMRELATION_ALLY );
		m_pInstanceField->SetRelation(
			Faction::FactionID( Faction::EMFACTION_SAFE_TEAM, _dwFactionID ),
			Faction::FactionID( Faction::EMFACTION_TEAM, _dwFactionID ),
			Faction::EMRELATION_ALLY );

		// 다른 모든 팀들과의 관계를 설정한다;
		MAP_CERTIFICATION_DUNGEON_FACTION_ITER iterFaction = m_mapCertificationDungeonFaction.begin();
		MAP_CERTIFICATION_DUNGEON_FACTION_ITER eIter = m_mapCertificationDungeonFaction.end();
		for ( ; iterFaction != eIter; ++iterFaction )
		{
			const CCertificationDungeonFaction& sFaction = iterFaction->second;
			if ( false == sFaction.IsValidation() )
				continue;
			
			const DWORD dwTargetFactionID = sFaction.GetFactionID();

			// 진영이 같을경우는 무조건 아군이다;
			if ( _dwFactionID == dwTargetFactionID )
			{
				continue;
			}

			// 클럽일 경우 서로의 동맹관계를 파악한다;
			if ( _bClub )
			{
				const bool bAlliance = m_pInstanceField->IsAllianceClub( _dwFactionID, dwTargetFactionID );
				if ( bAlliance )
				{
					m_pInstanceField->SetRelation(
						Faction::FactionID( Faction::EMFACTION_TEAM, _dwFactionID ),
						Faction::FactionID( Faction::EMFACTION_TEAM, dwTargetFactionID ),
						Faction::EMRELATION_ALLY );
					m_pInstanceField->SetRelation(
						Faction::FactionID( Faction::EMFACTION_TEAM, dwTargetFactionID ),
						Faction::FactionID( Faction::EMFACTION_TEAM, _dwFactionID ),
						Faction::EMRELATION_ALLY );
					m_pInstanceField->SetRelation(
						Faction::FactionID( Faction::EMFACTION_TEAM, _dwFactionID ),
						Faction::FactionID( Faction::EMFACTION_SAFE_TEAM, dwTargetFactionID ),
						Faction::EMRELATION_ALLY );
					m_pInstanceField->SetRelation(
						Faction::FactionID( Faction::EMFACTION_SAFE_TEAM, dwTargetFactionID ),
						Faction::FactionID( Faction::EMFACTION_TEAM, _dwFactionID ),
						Faction::EMRELATION_ALLY );
					m_pInstanceField->SetRelation(
						Faction::FactionID( Faction::EMFACTION_SAFE_TEAM, _dwFactionID ),
						Faction::FactionID( Faction::EMFACTION_TEAM, dwTargetFactionID ),
						Faction::EMRELATION_ALLY );
					m_pInstanceField->SetRelation(
						Faction::FactionID( Faction::EMFACTION_TEAM, dwTargetFactionID ),
						Faction::FactionID( Faction::EMFACTION_SAFE_TEAM, _dwFactionID ),
						Faction::EMRELATION_ALLY );
					m_pInstanceField->SetRelation(
						Faction::FactionID( Faction::EMFACTION_SAFE_TEAM, _dwFactionID ),
						Faction::FactionID( Faction::EMFACTION_SAFE_TEAM, dwTargetFactionID ),
						Faction::EMRELATION_ALLY );
					m_pInstanceField->SetRelation(
						Faction::FactionID( Faction::EMFACTION_SAFE_TEAM, dwTargetFactionID ),
						Faction::FactionID( Faction::EMFACTION_SAFE_TEAM, _dwFactionID ),
						Faction::EMRELATION_ALLY );
				}
				else
				{
					m_pInstanceField->SetRelation(
						Faction::FactionID( Faction::EMFACTION_TEAM, _dwFactionID ),
						Faction::FactionID( Faction::EMFACTION_TEAM, dwTargetFactionID ),
						Faction::EMRELATION_ENEMY );
					m_pInstanceField->SetRelation(
						Faction::FactionID( Faction::EMFACTION_TEAM, dwTargetFactionID ),
						Faction::FactionID( Faction::EMFACTION_TEAM, _dwFactionID ),
						Faction::EMRELATION_ENEMY );
					m_pInstanceField->SetRelation(
						Faction::FactionID( Faction::EMFACTION_TEAM, _dwFactionID ),
						Faction::FactionID( Faction::EMFACTION_SAFE_TEAM, dwTargetFactionID ),
						Faction::EMRELATION_NEUTRAL_ALLY );
					m_pInstanceField->SetRelation(
						Faction::FactionID( Faction::EMFACTION_SAFE_TEAM, dwTargetFactionID ),
						Faction::FactionID( Faction::EMFACTION_TEAM, _dwFactionID ),
						Faction::EMRELATION_NEUTRAL_ALLY );
					m_pInstanceField->SetRelation(
						Faction::FactionID( Faction::EMFACTION_SAFE_TEAM, _dwFactionID ),
						Faction::FactionID( Faction::EMFACTION_TEAM, dwTargetFactionID ),
						Faction::EMRELATION_NEUTRAL_ALLY );
					m_pInstanceField->SetRelation(
						Faction::FactionID( Faction::EMFACTION_TEAM, dwTargetFactionID ),
						Faction::FactionID( Faction::EMFACTION_SAFE_TEAM, _dwFactionID ),
						Faction::EMRELATION_NEUTRAL_ALLY );
					m_pInstanceField->SetRelation(
						Faction::FactionID( Faction::EMFACTION_SAFE_TEAM, _dwFactionID ),
						Faction::FactionID( Faction::EMFACTION_SAFE_TEAM, dwTargetFactionID ),
						Faction::EMRELATION_NEUTRAL_ALLY );
					m_pInstanceField->SetRelation(
						Faction::FactionID( Faction::EMFACTION_SAFE_TEAM, dwTargetFactionID ),
						Faction::FactionID( Faction::EMFACTION_SAFE_TEAM, _dwFactionID ),
						Faction::EMRELATION_NEUTRAL_ALLY );
				}
			}
			else
			{
				m_pInstanceField->SetRelation(
					Faction::FactionID( Faction::EMFACTION_TEAM, _dwFactionID ),
					Faction::FactionID( Faction::EMFACTION_TEAM, dwTargetFactionID ),
					Faction::EMRELATION_ENEMY );
				m_pInstanceField->SetRelation(
					Faction::FactionID( Faction::EMFACTION_TEAM, dwTargetFactionID ),
					Faction::FactionID( Faction::EMFACTION_TEAM, _dwFactionID ),
					Faction::EMRELATION_ENEMY );
				m_pInstanceField->SetRelation(
					Faction::FactionID( Faction::EMFACTION_TEAM, _dwFactionID ),
					Faction::FactionID( Faction::EMFACTION_SAFE_TEAM, dwTargetFactionID ),
					Faction::EMRELATION_NEUTRAL_ALLY );
				m_pInstanceField->SetRelation(
					Faction::FactionID( Faction::EMFACTION_SAFE_TEAM, dwTargetFactionID ),
					Faction::FactionID( Faction::EMFACTION_TEAM, _dwFactionID ),
					Faction::EMRELATION_NEUTRAL_ALLY );
				m_pInstanceField->SetRelation(
					Faction::FactionID( Faction::EMFACTION_SAFE_TEAM, _dwFactionID ),
					Faction::FactionID( Faction::EMFACTION_TEAM, dwTargetFactionID ),
					Faction::EMRELATION_NEUTRAL_ALLY );
				m_pInstanceField->SetRelation(
					Faction::FactionID( Faction::EMFACTION_TEAM, dwTargetFactionID ),
					Faction::FactionID( Faction::EMFACTION_SAFE_TEAM, _dwFactionID ),
					Faction::EMRELATION_NEUTRAL_ALLY );
				m_pInstanceField->SetRelation(
					Faction::FactionID( Faction::EMFACTION_SAFE_TEAM, _dwFactionID ),
					Faction::FactionID( Faction::EMFACTION_SAFE_TEAM, dwTargetFactionID ),
					Faction::EMRELATION_NEUTRAL_ALLY );
				m_pInstanceField->SetRelation(
					Faction::FactionID( Faction::EMFACTION_SAFE_TEAM, dwTargetFactionID ),
					Faction::FactionID( Faction::EMFACTION_SAFE_TEAM, _dwFactionID ),
					Faction::EMRELATION_NEUTRAL_ALLY );
			}
		}

		// 기본 진영들과의 관계를 설정한다 ( NPC );
		m_pInstanceField->SetRelation(
			Faction::FactionID( Faction::EMFACTION_TEAM, _dwFactionID ),
			Faction::FactionID( Faction::EMFACTION_BASIC, CertificationMachineMode::EMFACTION_BASIC_ALLY ),
			Faction::EMRELATION_ALLY ); 
		m_pInstanceField->SetRelation(
			Faction::FactionID( Faction::EMFACTION_TEAM, _dwFactionID ),
			Faction::FactionID( Faction::EMFACTION_BASIC, CertificationMachineMode::EMFACTION_BASIC_ENEMY ),
			Faction::EMRELATION_ENEMY );
		m_pInstanceField->SetRelation(
			Faction::FactionID( Faction::EMFACTION_TEAM, _dwFactionID ),
			Faction::FactionID( Faction::EMFACTION_BASIC, CertificationMachineMode::EMFACTION_BASIC_NEUTRAL ),
			Faction::EMRELATION_NEUTRAL_ALLY ); 

		m_pInstanceField->SetRelation(
			Faction::FactionID( Faction::EMFACTION_SAFE_TEAM, _dwFactionID ),
			Faction::FactionID( Faction::EMFACTION_BASIC, CertificationMachineMode::EMFACTION_BASIC_ALLY ),
			Faction::EMRELATION_ALLY ); 
		m_pInstanceField->SetRelation(
			Faction::FactionID( Faction::EMFACTION_SAFE_TEAM, _dwFactionID ),
			Faction::FactionID( Faction::EMFACTION_BASIC, CertificationMachineMode::EMFACTION_BASIC_ENEMY ),
			Faction::EMRELATION_ENEMY );
		m_pInstanceField->SetRelation(
			Faction::FactionID( Faction::EMFACTION_SAFE_TEAM, _dwFactionID ),
			Faction::FactionID( Faction::EMFACTION_BASIC, CertificationMachineMode::EMFACTION_BASIC_NEUTRAL ),
			Faction::EMRELATION_NEUTRAL_ALLY );
	}


	void CCertificationDungeonFactionManager::_NeutralizationAllFaction ( void )
	{
		MAP_CERTIFICATION_DUNGEON_FACTION_ITER iterFaction = m_mapCertificationDungeonFaction.begin();
		MAP_CERTIFICATION_DUNGEON_FACTION_ITER eIter = m_mapCertificationDungeonFaction.end();
		for ( ; iterFaction != eIter; ++iterFaction )
		{
			const CCertificationDungeonFaction& sFaction = iterFaction->second;
			if ( false == sFaction.IsValidation() )
				continue;

			const DWORD dwTargetFactionID = sFaction.GetFactionID();

			m_pInstanceField->SetRelationToAll(
				Faction::FactionID( Faction::EMFACTION_TEAM, dwTargetFactionID ),
				Faction::EMRELATION_NEUTRAL_ALLY );
			m_pInstanceField->SetRelationFromAll(
				Faction::FactionID( Faction::EMFACTION_TEAM, dwTargetFactionID ),
				Faction::EMRELATION_NEUTRAL_ALLY );
		}
	}


	const bool CCertificationDungeonFactionManager::_SortFactionRanking ( void )
	{
		if ( NULL == m_pInstanceField )
			return false;

		if ( NULL == m_pDungeon )
			return false;

		// 정렬한다;
		CertificationMachineMode::CCompare_SFACTION_POINT sComp;
		std::sort ( m_vecFactionPoint.begin(), m_vecFactionPoint.end(), sComp );

		// 랭킹을 세팅한다;
		DWORD dwPrePoint = MAXDWORD;
		DWORD dwRanking = 0;
		bool bDuplicate1stRanking = false;
		for ( int i=0; i<m_vecFactionPoint.size(); ++i )
		{
			bool bChangeRanking = false;

			CertificationMachineMode::SFACTION_POINT& sFactionPoint = m_vecFactionPoint.at( i );
			if ( m_pDungeon->NUM_APPROACH_POINT <= sFactionPoint.dwFactionPoint )
			{
				if ( false == sFactionPoint.bApproachGoal )
				{
					m_pInstanceField->RegistTimer(
						0,
						CertificationMachineMode::EMTIMER_ID_APPROACHED_GOAL_NOTIFY,
						sFactionPoint.dwFactionID );

					sFactionPoint.bApproachGoal = true;
				}
			}

			// 랭킹의 변동 여부를 확인한다;
			if ( dwPrePoint > sFactionPoint.dwFactionRealPoint )
			{
				dwRanking++;

				// 랭킹이 변동되었다;
				bChangeRanking = true;
			}

			// 랭킹이 변동되지 않았는데 랭킹이 1위일 경우에는 1위가 없다;
			if ( false == bChangeRanking && 1 == dwRanking )
			{
				bDuplicate1stRanking = true;
				dwRanking++;
			}

			// 랭킹을 세팅한다;
			sFactionPoint.dwRanking = dwRanking;

			// 다음 진영과의 비교를 위해 포인트를 저장한다;
			dwPrePoint = sFactionPoint.dwFactionRealPoint;
		}

		// 1위가 중복되었을 경우에는 세팅되었던 1위를 2위로 바꾼다;
		if ( bDuplicate1stRanking )
		{
			// 정상적으로 들어올 경우 0번 Index에 값이 무조건 들어가있어서 유효성 검사를 하지 않아도 된다;
			// 그래도 한번 검사해준다;
			if ( false == m_vecFactionPoint.empty() )
			{
				CertificationMachineMode::SFACTION_POINT& sFactionPoint = m_vecFactionPoint.at( 0 );
				sFactionPoint.dwRanking = 2;
			}
		}

		// 랭킹 UI를 출력한다;
		m_pInstanceField->UI_CertificationMachineMode_Rank_Info(
			m_vecFactionPoint );

		// 목표점수 달성여부를 확인한다;
		if ( 1 <= m_vecFactionPoint.size() )
		{
			const CertificationMachineMode::SFACTION_POINT& sFactionPoint = m_vecFactionPoint.at( 0 );
			if ( m_pDungeon->NUM_TARGET_POINT <= sFactionPoint.dwFactionPoint )
				return true;
		}

		return false;
	}

}