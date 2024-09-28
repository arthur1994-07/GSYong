#include "pch.h"

#include "../IInstanceFieldMode.h"

#include "./InstanceDungeon_CertificationMachine.h"

#include "./CertificationDungeonFaction.h"

namespace InstanceSystem
{

	void CCertificationDungeonFaction::Initialize ( void )
	{
		// �������� �ʱ�ȭ;
		m_dwMemberCount = 0;
		m_dwPoint = 0;
		m_dwCertifyCount = 0;
		m_bAddPoint = false;
		m_vecCertificationMachine.clear();

		// ���� Ȱ��ȭ;
		m_bValidation = true;
	}


	const bool CCertificationDungeonFaction::Join ( void )
	{
		// ���� ������ ��� ���������� �ʱ�ȭ�Ѵ�;
		if ( false == m_bValidation )
		{	
			Initialize();
		}

		// ������� ������Ų��;
		m_dwMemberCount++;

		return true;
	}


	void CCertificationDungeonFaction::Out ( void )
	{
		// ������� ���ҽ�Ų��;
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
		// AddPoint ���¸� �ʱ�ȭ�Ѵ�;
		InactivateAddPoint();

		// ����Ƚ���� �ø���;
		m_dwCertifyCount++;

		// ������ Machine �� ����Ѵ�;
		m_vecCertificationMachine.push_back( _dwMachineID );
	}


	void CCertificationDungeonFaction::Syncronize (
		const DWORD _dwPoint,
		const DWORD _dwCertifyCount )
	{
		// ����Ʈ�� ����Ƚ���� ����ȭ�Ѵ�;
		m_dwPoint = _dwPoint;
		m_dwCertifyCount = _dwCertifyCount;
	}


	void CCertificationDungeonFaction::SyncronizeCertificationMachine (
		const DWORD _dwMachineID )
	{
		// ������ Machine �� ����ȭ�Ѵ�;
		m_vecCertificationMachine.push_back( _dwMachineID );
	}


	void CCertificationDungeonFaction::SendSyncronizeMessage ( void ) const
	{
		if ( NULL == m_pInstanceField )
			return;

		// ����Ʈ�� ����Ƚ���� ����ȭ�� �� �ֵ��� �޽����� ������;
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
			// ó�������� ������ ��� ���� �����Ѵ�;
			CCertificationDungeonFaction sFaction( m_pInstanceField, m_pDungeon, _dwFactionID );
			sFaction.Join();

			CertificationMachineMode::SFACTION_POINT sFactionPoint( _dwFactionID, 0 );
			m_vecFactionPoint.push_back( sFactionPoint );

			// ó�������� ������ ��� �������� ������ �ʿ��ϴ�;
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

		// ���� ������ ��� ����;
		if ( _dwFactionID == _dwCapturedFactionID )
			return false;

		// �̹� �������� ���������� Ȯ��;
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

		// �������� Point �� �����Ѵ�;
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

		// ��� ���� �߸�ȭ;
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
			// ó�������� ������ ��� ���� �����Ѵ�;
			CCertificationDungeonFaction sFaction( m_pInstanceField, m_pDungeon, _dwFactionID );
			sFaction.Join();

			// ����ȭ�Ѵ�;
			sFaction.Syncronize( _dwPoint, _dwCertifyCount );

			CertificationMachineMode::SFACTION_POINT sFactionPoint( _dwFactionID, _dwPoint );
			sFactionPoint.dwFactionRealPoint = sFaction.GetRealPoint();
			m_vecFactionPoint.push_back( sFactionPoint );

			// ó�������� ������ ��� �������� ������ �ʿ��ϴ�;
			m_mapCertificationDungeonFaction.insert(
				MAP_CERTIFICATION_DUNGEON_FACTION_VALUE( _dwFactionID, sFaction ) );

			// ���踦 �ʱ�ȭ�Ѵ�;
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
		// ������ �����Ǿ� �־�� �Ѵ�;
		// ��, ������ ���� ����ȭ�ϰ� �����Ⱑ ����ȭ�ȴ�;
		/*else
		{
			// ó�������� ������ ��� ���� �����Ѵ�;
			CCertificationDungeonFaction sFaction( m_pInstanceField, m_pDungeon, _dwFactionID );
			sFaction.Join();

			// ����ȭ�Ѵ�;
			sFaction.SyncronizeCertificationMachine( _dwMachineID );

			CertificationMachineMode::SFACTION_POINT sFactionPoint( _dwFactionID, 0 );
			m_vecFactionPoint.push_back( sFactionPoint );

			// ó�������� ������ ��� �������� ������ �ʿ��ϴ�;
			m_mapCertificationDungeonFaction.insert(
				MAP_CERTIFICATION_DUNGEON_FACTION_VALUE( _dwFactionID, sFaction ) );

			// ���踦 �ʱ�ȭ�Ѵ�;
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

		// �� ���� ������������ ���� �Ʊ��̴�;
		m_pInstanceField->SetRelation(
			Faction::FactionID( Faction::EMFACTION_TEAM, _dwFactionID ),
			Faction::FactionID( Faction::EMFACTION_SAFE_TEAM, _dwFactionID ),
			Faction::EMRELATION_ALLY );
		m_pInstanceField->SetRelation(
			Faction::FactionID( Faction::EMFACTION_SAFE_TEAM, _dwFactionID ),
			Faction::FactionID( Faction::EMFACTION_TEAM, _dwFactionID ),
			Faction::EMRELATION_ALLY );

		// �ٸ� ��� ������� ���踦 �����Ѵ�;
		MAP_CERTIFICATION_DUNGEON_FACTION_ITER iterFaction = m_mapCertificationDungeonFaction.begin();
		MAP_CERTIFICATION_DUNGEON_FACTION_ITER eIter = m_mapCertificationDungeonFaction.end();
		for ( ; iterFaction != eIter; ++iterFaction )
		{
			const CCertificationDungeonFaction& sFaction = iterFaction->second;
			if ( false == sFaction.IsValidation() )
				continue;
			
			const DWORD dwTargetFactionID = sFaction.GetFactionID();

			// ������ �������� ������ �Ʊ��̴�;
			if ( _dwFactionID == dwTargetFactionID )
			{
				continue;
			}

			// Ŭ���� ��� ������ ���Ͱ��踦 �ľ��Ѵ�;
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

		// �⺻ ��������� ���踦 �����Ѵ� ( NPC );
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

		// �����Ѵ�;
		CertificationMachineMode::CCompare_SFACTION_POINT sComp;
		std::sort ( m_vecFactionPoint.begin(), m_vecFactionPoint.end(), sComp );

		// ��ŷ�� �����Ѵ�;
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

			// ��ŷ�� ���� ���θ� Ȯ���Ѵ�;
			if ( dwPrePoint > sFactionPoint.dwFactionRealPoint )
			{
				dwRanking++;

				// ��ŷ�� �����Ǿ���;
				bChangeRanking = true;
			}

			// ��ŷ�� �������� �ʾҴµ� ��ŷ�� 1���� ��쿡�� 1���� ����;
			if ( false == bChangeRanking && 1 == dwRanking )
			{
				bDuplicate1stRanking = true;
				dwRanking++;
			}

			// ��ŷ�� �����Ѵ�;
			sFactionPoint.dwRanking = dwRanking;

			// ���� �������� �񱳸� ���� ����Ʈ�� �����Ѵ�;
			dwPrePoint = sFactionPoint.dwFactionRealPoint;
		}

		// 1���� �ߺ��Ǿ��� ��쿡�� ���õǾ��� 1���� 2���� �ٲ۴�;
		if ( bDuplicate1stRanking )
		{
			// ���������� ���� ��� 0�� Index�� ���� ������ ���־ ��ȿ�� �˻縦 ���� �ʾƵ� �ȴ�;
			// �׷��� �ѹ� �˻����ش�;
			if ( false == m_vecFactionPoint.empty() )
			{
				CertificationMachineMode::SFACTION_POINT& sFactionPoint = m_vecFactionPoint.at( 0 );
				sFactionPoint.dwRanking = 2;
			}
		}

		// ��ŷ UI�� ����Ѵ�;
		m_pInstanceField->UI_CertificationMachineMode_Rank_Info(
			m_vecFactionPoint );

		// ��ǥ���� �޼����θ� Ȯ���Ѵ�;
		if ( 1 <= m_vecFactionPoint.size() )
		{
			const CertificationMachineMode::SFACTION_POINT& sFactionPoint = m_vecFactionPoint.at( 0 );
			if ( m_pDungeon->NUM_TARGET_POINT <= sFactionPoint.dwFactionPoint )
				return true;
		}

		return false;
	}

}