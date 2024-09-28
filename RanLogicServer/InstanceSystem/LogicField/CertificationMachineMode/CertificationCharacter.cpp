#include "pch.h"

#include "../../RanLogicServer/Character/GLChar.h"

#include "../IInstanceFieldMode.h"

#include "./InstanceDungeon_CertificationMachine.h"

#include "./CertificationCharacter.h"

namespace InstanceSystem
{

	void CCertificationCharacter::EnterMap (
		const SNATIVEID _sEnterMapID,
		const bool _bAddPoint )
	{
		if ( NULL == m_pDungeon )
			return;

		if ( false == m_bValidation )
			return;

		// �� ���� ���ʿ��� ������������ ���õȴ�;
		m_pInstanceField->SetFaction(
			Faction::GLActorID( CROW_PC, m_dwChaDBNum ),
			Faction::FactionID( Faction::EMFACTION_SAFE_TEAM, m_dwFactionID ) );

		const DWORD dwAllTime = m_pDungeon->GetAllTime();
		const DWORD dwDurationTime = m_pDungeon->GetDurationTime();
		DWORD dwPastTime = dwAllTime - dwDurationTime;

		// ���� ������ Ȱ��ȭ��Ų��;
		if ( m_pDungeon->IsJoinBuff() &&
			dwPastTime < m_pDungeon->TIME_JOIN_BUFFTIME )
		{
			const DWORD dwTime = m_pDungeon->TIME_JOIN_BUFFTIME - dwPastTime;
			ActivateJoinBuff( dwTime );
		}

		// ���� ������� Ȱ��ȭ��Ų��;
		if ( m_pDungeon->IsJoinDebuff() &&
			dwPastTime < m_pDungeon->TIME_JOIN_DEBUFFTIME )
		{
			const DWORD dwTime = m_pDungeon->TIME_JOIN_DEBUFFTIME - dwPastTime;
			ActivateJoinDebuff( dwTime );
		}

		// ��������� ������ ��쿡�� �ٸ�ó���� �Ѵ�;
		if ( _sEnterMapID == m_pDungeon->MAP_BATTLE )
		{
			// ���� �ð��� ���� �� ������ �ٲ�� Ÿ�̸Ӹ� ����Ѵ�;
			m_pInstanceField->RegistTimer(
				m_pDungeon->TIME_SAFE_TIME,
				CertificationMachineMode::EMTIMER_ID_SAFETIME,
				m_dwChaDBNum );

			// ���� ������ AddPoint �����̸� ���ӱ� ������ Ȱ��ȭ��Ų��;
			if ( _bAddPoint )
			{
				ActivateAcceleratorBuff();
			}
		}
	}


	void CCertificationCharacter::ReleaseSafeFaction ( void )
	{
		if ( false == m_bValidation )
			return;

		// �����������¸� �����Ѵ�;
		m_pInstanceField->SetFaction(
			Faction::GLActorID( CROW_PC, m_dwChaDBNum ),
			Faction::FactionID( Faction::EMFACTION_TEAM, m_dwFactionID ) );
	}


	const bool CCertificationCharacter::StartCertification (
		const DWORD _dwMachineID,
		const DWORD _dwCertifyTime )
	{
		if ( NULL == m_pInstanceField )
			return false;

		if ( NULL == m_pDungeon )
			return false;

		if ( false == m_bValidation )
			return false;

		if ( false == m_bAlive )
			return false;

		// ���� �������� �����Ⱑ ���� ������ ������ �����̸� ������ �����ϸ� ó���Ѵ�;
		if ( ( MAXDWORD == m_dwCertifyingMachine ) &&
			m_bCanCertification &&
			( 0 != m_dwFactionID ) )
		{
			m_dwCertifyingMachine = _dwMachineID;
			m_bCanCertification = false;

			// �����ð� ���� ������ �ִٸ� �����ð��� ���ҽ�Ų��;
			DWORD dwCertifyTime = _dwCertifyTime;
			if ( m_pInstanceField->IsHaveSkillFact(
				Faction::GLActorID( CROW_PC, m_dwChaDBNum ),
				m_pDungeon->SKILL_CERTIFY_TIME_REDUCE_BUFF ) )
			{
				dwCertifyTime /= 2;
			}

			// ������ ����� �ο��Ѵ�;
			m_pInstanceField->SetMotion(
				Faction::GLActorID( CROW_PC, m_dwChaDBNum ),
				m_pDungeon->MOTION_CERTIFY,
				dwCertifyTime );

			// ���� ����� �ο��Ǿ��ٸ� ���� UI�� �����ش�;
			if ( m_pInstanceField->IsMotion(
				Faction::GLActorID( CROW_PC, m_dwChaDBNum ),
				m_pDungeon->MOTION_CERTIFY ) )
			{
				m_pInstanceField->UI_Auth( m_dwChaDBNum, true, dwCertifyTime );
			}

			// ������ �����ϸ� �����Ǿ� �ִ� ��ųȿ���� ���� ��ų�� �����Ѵ�;
			for ( int i=0; i<m_pDungeon->VEC_REMOVE_SKILLEFFECT_BY_CERTIFYING.size(); ++i )
			{
				const DWORD dwSkillEffect = m_pDungeon->VEC_REMOVE_SKILLEFFECT_BY_CERTIFYING.at( i );

				m_pInstanceField->RemoveSkillFact(
					Faction::GLActorID( CROW_PC, m_dwChaDBNum ),
					static_cast< EMSPEC_ADDON >( dwSkillEffect ) );
			}

			// ������ �����ϸ� �����Ǿ� �ִ� ������ �����Ѵ�;
			for ( int i=0; i<m_pDungeon->VEC_REMOVE_BUFF_BY_CERTIFYING.size(); ++i )
			{
				const SNATIVEID sBuff = m_pDungeon->VEC_REMOVE_BUFF_BY_CERTIFYING.at( i );

				m_pInstanceField->RemoveSkillFact(
					Faction::GLActorID( CROW_PC, m_dwChaDBNum ),
					sBuff );
			}

			return true;
		}

		return false;
	}


	void CCertificationCharacter::EndCertification ( void )
	{
		if ( NULL == m_pInstanceField )
			return;

		if ( NULL == m_pDungeon )
			return;

		if ( false == m_bValidation )
			return;

		// ���� ����� ����� ��� �������� ������ ������ �ʱ�ȭ�Ѵ�;
		m_dwCertifyingMachine = MAXDWORD;

		// ������ �����ϵ��� �ϴ� ���±����� ����� ���� �д�;
		m_pInstanceField->RegistTimer(
			m_pDungeon->TIME_CERTIFY_WAITTIME,
			CertificationMachineMode::EMTIMER_ID_TRY_CERTIFY_WAITTIME,
			m_dwChaDBNum );

		// ���� UI �� ����;
		m_pInstanceField->UI_Auth( m_dwChaDBNum, false, 0 );
	}


	void CCertificationCharacter::SuccessCertification (
		const DWORD _dwMachineID )
	{
		if ( false == m_bValidation )
			return;

		// �������� ������ �����ߴµ� ������ �ӽ��� ���� �������̶�� ����Ѵ�;
		if ( _dwMachineID == m_dwCertifyingMachine )
		{
			m_pInstanceField->ResetMotion( Faction::GLActorID( CROW_PC, m_dwChaDBNum ) );
		}
	}


	void CCertificationCharacter::ActivateAcceleratorBuff ( void )
	{
		if ( NULL == m_pDungeon )
			return;

		if ( false == m_bValidation )
			return;

		for ( int i=0; i<m_pDungeon->VEC_ACCELERATOR_BUFF.size(); ++i )
		{
			const SNATIVEID& sBuff = m_pDungeon->VEC_ACCELERATOR_BUFF.at( i );

			if ( m_pInstanceField->IsHaveSkillFact(
				Faction::GLActorID( CROW_PC, m_dwChaDBNum ),
				sBuff ) )
			{
				InactivateAceeleratorBuff();
			}
			
			m_pInstanceField->ApplySkill(
				Faction::GLActorID( CROW_PC, m_dwChaDBNum ),
				sBuff,
				0,
				m_pDungeon->TIME_ACCELERATOR_BUFFTIME );
			
		}
	}


	void CCertificationCharacter::InactivateAceeleratorBuff ( void )
	{
		if ( NULL == m_pDungeon )
			return;

		if ( false == m_bValidation )
			return;

		for ( int i=0; i<m_pDungeon->VEC_ACCELERATOR_BUFF.size(); ++i )
		{
			const SNATIVEID& sBuff = m_pDungeon->VEC_ACCELERATOR_BUFF.at( i );

			m_pInstanceField->RemoveSkillFact(
				Faction::GLActorID( CROW_PC, m_dwChaDBNum ),
				sBuff );
		}
	}


	void CCertificationCharacter::ActivateJoinBuff (
		const DWORD _dwTime )
	{
		if ( NULL == m_pDungeon )
			return;

		if ( false == m_bValidation )
			return;

		for ( int i=0; i<m_pDungeon->VEC_JOIN_BUFF.size(); ++i )
		{
			const SNATIVEID& sBuff = m_pDungeon->VEC_JOIN_BUFF.at( i );

			if ( false == m_pInstanceField->IsHaveSkillFact(
				Faction::GLActorID( CROW_PC, m_dwChaDBNum ),
				sBuff ) )
			{
				m_pInstanceField->ApplySkill(
					Faction::GLActorID( CROW_PC, m_dwChaDBNum ),
					sBuff,
					0,
					_dwTime,
					m_pDungeon->TIME_JOIN_BUFFTIME );
			}
		}
	}


	void CCertificationCharacter::InactivateJoinBuff ( void )
	{
		if ( NULL == m_pDungeon )
			return;

		if ( false == m_bValidation )
			return;

		for ( int i=0; i<m_pDungeon->VEC_JOIN_BUFF.size(); ++i )
		{
			const SNATIVEID& sBuff = m_pDungeon->VEC_JOIN_BUFF.at( i );

			m_pInstanceField->RemoveSkillFact(
				Faction::GLActorID( CROW_PC, m_dwChaDBNum ),
				sBuff );
		}
	}


	void CCertificationCharacter::ActivateJoinDebuff (
		const DWORD _dwTime )
	{
		if ( NULL == m_pDungeon )
			return;

		if ( false == m_bValidation )
			return;

		for ( int i=0; i<m_pDungeon->VEC_JOIN_DEBUFF.size(); ++i )
		{
			const SNATIVEID& sDebuff = m_pDungeon->VEC_JOIN_DEBUFF.at( i );

			if ( false == m_pInstanceField->IsHaveSkillFact(
				Faction::GLActorID( CROW_PC, m_dwChaDBNum ),
				sDebuff ) )
			{
				m_pInstanceField->ApplySkill(
					Faction::GLActorID( CROW_PC, m_dwChaDBNum ),
					sDebuff,
					0,
					_dwTime,
					m_pDungeon->TIME_JOIN_DEBUFFTIME );
			}
		}
	}


	void CCertificationCharacter::InactivateJoinDebuff ( void )
	{
		if ( NULL == m_pDungeon )
			return;

		if ( false == m_bValidation )
			return;

		for ( int i=0; i<m_pDungeon->VEC_JOIN_DEBUFF.size(); ++i )
		{
			const SNATIVEID& sBuff = m_pDungeon->VEC_JOIN_DEBUFF.at( i );

			m_pInstanceField->RemoveSkillFact(
				Faction::GLActorID( CROW_PC, m_dwChaDBNum ),
				sBuff );
		}
	}


	void CCertificationCharacter::GameEnd ( void )
	{
		if ( NULL == m_pDungeon )
			return;

		if ( NULL == m_pInstanceField )
			return;

		if ( false == m_bValidation )
			return;

		// ������ ����Ǹ� ����� ����Ѵ�;
		if ( m_pInstanceField->IsMotion(
			Faction::GLActorID( CROW_PC, m_dwChaDBNum ),
			m_pDungeon->MOTION_CERTIFY ) )
		{
			m_pInstanceField->ResetMotion( Faction::GLActorID( CROW_PC, m_dwChaDBNum ) );
		}
	}


	const bool CCertificationCharacter::IsCertifying ( void )
	{
		if ( NULL == m_pDungeon )
			return false;

		if ( NULL == m_pInstanceField )
			return false;

		if ( false == m_bValidation )
			return false;

		// ������ ����Ǹ� ����� ����Ѵ�;
		if ( m_pInstanceField->IsMotion(
			Faction::GLActorID( CROW_PC, m_dwChaDBNum ),
			m_pDungeon->MOTION_CERTIFY ) )
		{
			return true;
		}

		return false;
	}


	const bool CCertificationCharacter::IsDie ( void )
	{
		if ( NULL == m_pDungeon )
			return true;

		if ( NULL == m_pInstanceField )
			return true;

		if ( false == m_bValidation )
			return true;

		GLChar* pChar = m_pInstanceField->GetChar( m_dwChaDBNum );
		if ( NULL == pChar )
			return true;

		return ( false == pChar->IsAlive() );
	}




	void CCertificationCharacterManager::Join (
		const DWORD _dwChaDBNum,
		const DWORD _dwFactionID,
		const bool _bClub )
	{
		if ( NULL == m_pDungeon )
			return;

		MAP_CERTIFICATION_CHARACTER_ITER iterUser = m_mapCertificationCharacter.find ( _dwChaDBNum );
		if ( iterUser !=  m_mapCertificationCharacter.end() )
		{
			CCertificationCharacter& sTempUser = iterUser->second;
			sTempUser.Join( _dwFactionID );
		}
		else
		{
			// ���� �ѹ��� �������� ���� ĳ������ ��쿡�� ĳ���������� ���� �����Ѵ�;
			CCertificationCharacter sTempUser( m_pInstanceField, m_pDungeon, _dwChaDBNum, _dwFactionID );
			sTempUser.Join( _dwFactionID );

			m_mapCertificationCharacter.insert( MAP_CERTIFICATION_CHARACTER_VALUE( _dwChaDBNum, sTempUser ) );
		}

		// ĳ���Ͱ� �Ҽӵ� ������ ���ο� ĳ���͸� �����Ų��;
		m_sFactionManager.Join( _dwFactionID, _bClub );
	}


	void CCertificationCharacterManager::Out (
		const DWORD _dwChaDBNum )
	{
		MAP_CERTIFICATION_CHARACTER_ITER iterUser = m_mapCertificationCharacter.find ( _dwChaDBNum );
		if ( iterUser !=  m_mapCertificationCharacter.end() )
		{
			CCertificationCharacter& sTempUser = iterUser->second;
			sTempUser.Out();

			sTempUser.InactivateJoinBuff();
			sTempUser.InactivateJoinDebuff();

			// ĳ������ ������ Ȯ���Ѵ�;
			const DWORD dwTempFactionID = sTempUser.GetFactionID();

			// �������� ĳ���͸� �Ѹ� �����Ų��;
			m_sFactionManager.Out( dwTempFactionID );
		}
	}


	void CCertificationCharacterManager::EnterMap (
		const DWORD _dwChaDBNum,
		const SNATIVEID _sEnterMapID )
	{
		if ( NULL == m_pInstanceField )
			return;

		MAP_CERTIFICATION_CHARACTER_ITER iterUser = m_mapCertificationCharacter.find ( _dwChaDBNum );
		if ( iterUser !=  m_mapCertificationCharacter.end() )
		{
			CCertificationCharacter& sTempUser = iterUser->second;
			sTempUser.EnterMap( _sEnterMapID, m_sFactionManager.IsAddPointBuff( sTempUser.GetFactionID() ) );
		}
	}


	const bool CCertificationCharacterManager::StartCertification (
		const DWORD _dwChaDBNum,
		const DWORD _dwMachineID,
		const DWORD _dwCapturedFactionID )
	{
		if ( NULL == m_pInstanceField )
			return false;

		MAP_CERTIFICATION_CHARACTER_ITER iterUser = m_mapCertificationCharacter.find ( _dwChaDBNum );
		if ( iterUser !=  m_mapCertificationCharacter.end() )
		{
			CCertificationCharacter& sTempUser = iterUser->second;

			// �̹� �������̸� �����Ѵ�;
			if ( sTempUser.IsCertifying() || sTempUser.IsDie() )
			{
				return false;
			}

			// ������ �� �� ���� �����̰ų� ĳ���Ͱ� ��ȿ���� ������ �����Ѵ�;
			if ( false == sTempUser.CanCertification() ||
				false == sTempUser.IsValidation() )
			{
				m_pInstanceField->RegistTimer(
					0,
					CertificationMachineMode::EMTIMER_ID_CERTIFY_FAIL_NOTIFY,
					_dwChaDBNum );

				return false;
			}

			// ĳ������ ������ Ȯ���Ѵ�;
			const DWORD dwTempFactionID = sTempUser.GetFactionID();

			// ���� �����̰ų� �ڱ� ������ �̹� �������� �������� �����Ѵ�;
			if ( false == m_sFactionManager.StartCertification( dwTempFactionID, _dwMachineID, _dwCapturedFactionID ) )
			{
				m_pInstanceField->RegistTimer(
					0,
					CertificationMachineMode::EMTIMER_ID_CERTIFY_CAPTURED_FAIL_NOTIFY,
					_dwChaDBNum,
					_dwCapturedFactionID );

				return false;
			}

			// ���� ������ ��� �����Ѵ�;
			if ( m_pDungeon && m_pDungeon->IsClubFaction() )
			{
				if ( m_pInstanceField->IsAllianceClub( dwTempFactionID, _dwCapturedFactionID ) )
				{
					m_pInstanceField->RegistTimer(
						0,
						CertificationMachineMode::EMTIMER_ID_CERTIFY_DUPLICATE_FAIL_NOTIFY,
						_dwChaDBNum );

					return false;
				}
			}

			const DWORD dwCertifyTime = m_sFactionManager.GetCertifyTime( dwTempFactionID );

			// ������ �����Ѵ�;
			if ( false == sTempUser.StartCertification( _dwMachineID, dwCertifyTime ) )
			{
				m_pInstanceField->RegistTimer(
					0,
					CertificationMachineMode::EMTIMER_ID_CERTIFY_FAIL_NOTIFY,
					_dwChaDBNum );

				return false;
			}

			// ������ ����� ��� ���� �õ� ���� Ÿ�̸Ӹ� ����Ѵ�;
			m_pInstanceField->RegistTimer(
				0,
				CertificationMachineMode::EMTIMER_ID_TRY_CERTIFY_NOTIFY,
				_dwChaDBNum,
				_dwMachineID );

			return true;
		}

		return false;
	}


	void CCertificationCharacterManager::EndCertification (
		const DWORD _dwChaDBNum )
	{
		MAP_CERTIFICATION_CHARACTER_ITER iterUser = m_mapCertificationCharacter.find ( _dwChaDBNum );
		if ( iterUser !=  m_mapCertificationCharacter.end() )
		{
			CCertificationCharacter& sTempUser = iterUser->second;
			sTempUser.EndCertification();
		}
	}


	void CCertificationCharacterManager::SuccessCertification (
		const DWORD _dwChaDBNum )
	{
		if ( NULL == m_pDungeon )
			return;

		MAP_CERTIFICATION_CHARACTER_ITER iterUser = m_mapCertificationCharacter.find ( _dwChaDBNum );
		if ( iterUser !=  m_mapCertificationCharacter.end() )
		{
			// �������� ĳ������ ���������� �����´�;
			const DWORD dwFactionID = GetCertifyingFactionID( _dwChaDBNum );
			if ( MAXDWORD == dwFactionID )
				return;

			// ĳ���Ͱ� �������� ������������ �����´�;
			const DWORD dwMachineID = GetCertifyingMachine( _dwChaDBNum );
			if ( MAXDWORD == dwMachineID )
				return;

			// ������ ������Ų��;
			m_sFactionManager.SuccessCertification( dwFactionID, dwMachineID );

			// ĳ���Ͱ� ���� ���� ȹ�� ������ �����ϰ� ������� ������ AddPoint ���·� �����Ѵ�;
			if ( m_pInstanceField->IsHaveSkillFact(
				Faction::GLActorID( CROW_PC, _dwChaDBNum ),
				m_pDungeon->SKILL_CERTIFY_ADD_POINT_BUFF ) )
			{
				m_sFactionManager.ActivateAddPoint( dwFactionID );
			}

			// ��� ĳ���Ϳ��� Ư�� �������� ������ �����Ͽ��ٰ� �˸���;
			MAP_CERTIFICATION_CHARACTER_ITER iterUser = m_mapCertificationCharacter.begin();
			MAP_CERTIFICATION_CHARACTER_ITER endIter = m_mapCertificationCharacter.end();
			for ( ; iterUser != endIter; ++iterUser )
			{
				CCertificationCharacter& sTempUser = iterUser->second;
				sTempUser.SuccessCertification( dwMachineID );
			}
		}
	}


	void CCertificationCharacterManager::ReleaseSafeFaction (
		const DWORD _dwChaDBNum )
	{
		MAP_CERTIFICATION_CHARACTER_ITER iterUser = m_mapCertificationCharacter.find ( _dwChaDBNum );
		if ( iterUser !=  m_mapCertificationCharacter.end() )
		{
			CCertificationCharacter& sTempUser = iterUser->second;
			sTempUser.ReleaseSafeFaction();
		}
	}


	void CCertificationCharacterManager::UnlockCertification (
		const DWORD _dwChaDBNum )
	{
		MAP_CERTIFICATION_CHARACTER_ITER iterUser = m_mapCertificationCharacter.find ( _dwChaDBNum );
		if ( iterUser !=  m_mapCertificationCharacter.end() )
		{
			CCertificationCharacter& sTempUser = iterUser->second;
			sTempUser.UnlockCertification();
		}
	}


	void CCertificationCharacterManager::CaptureAcceleratorBuff (
		const DWORD _dwChaDBNum )
	{
		if ( NULL == m_pInstanceField )
			return;

		if ( NULL == m_pDungeon )
			return;

		// Faction ID �� ã�´�;
		DWORD dwFactionID = 0;
		{
			MAP_CERTIFICATION_CHARACTER_ITER iterUser = m_mapCertificationCharacter.find ( _dwChaDBNum );
			if ( iterUser !=  m_mapCertificationCharacter.end() )
			{
				const CCertificationCharacter& sTempUser = iterUser->second;
				dwFactionID = sTempUser.GetFactionID();
			}
			else
			{
				return;
			}
		}

		// �̹� ���ӱ� �������� Ÿ�̸Ӱ� ��ϵǾ��ִٸ� �������ش� ( �ʱ�ȭ�� �����̴� );
		if ( m_pInstanceField->IsRegistTimer( m_hTimerAcceleratorBuff ) )
		{
			m_pInstanceField->RemoveTimer( m_hTimerAcceleratorBuff );
		}

		// ������ AddPointBuff �ο� ���¸� Ȱ��ȭ��Ų��;
		m_sFactionManager.ActivateAddPointBuff( dwFactionID );

		// ������ ���� ��������� AddPoint ���¸� Ȱ��ȭ��Ų��;
		m_sFactionManager.ActivateAddPoint_CertificationMachine( dwFactionID );

		// ������ ��� ĳ���͵��� ���ӱ� ������ Ȱ��ȭ��Ų��;
		MAP_CERTIFICATION_CHARACTER_ITER iterUser = m_mapCertificationCharacter.begin();
		MAP_CERTIFICATION_CHARACTER_ITER endIter = m_mapCertificationCharacter.end();
		for ( ; iterUser != endIter; ++iterUser )
		{
			CCertificationCharacter& sTempUser = iterUser->second;
			if ( sTempUser.GetFactionID() == dwFactionID )
			{
				sTempUser.ActivateAcceleratorBuff();
			}
		}

		// ���ӱ� ���� ���� Ÿ�̸Ӹ� ����Ѵ�;
		m_hTimerAcceleratorBuff = m_pInstanceField->RegistTimer(
			m_pDungeon->TIME_ACCELERATOR_BUFFTIME,
			CertificationMachineMode::EMTIMER_ID_ACCELERATOR_BUFFTIME,
			dwFactionID );
	}


	void CCertificationCharacterManager::ExitAcceleratorBuff (
		const DWORD _dwFactionID )
	{
		// ������ AddPointBuff �ο� ���¸� ��Ȱ��ȭ��Ų��;
		m_sFactionManager.InactivateAddPointBuff( _dwFactionID );

		// ������ ��� ĳ���͵��� ���ӱ� ������ �����Ѵ�;
		MAP_CERTIFICATION_CHARACTER_ITER iterUser = m_mapCertificationCharacter.begin();
		MAP_CERTIFICATION_CHARACTER_ITER endIter = m_mapCertificationCharacter.end();
		for ( ; iterUser != endIter; ++iterUser )
		{
			CCertificationCharacter& sTempUser = iterUser->second;
			if ( sTempUser.GetFactionID() == _dwFactionID )
			{
				sTempUser.InactivateAceeleratorBuff();
			}
		}
	}


	const bool CCertificationCharacterManager::UpdatePoint (
		const CertificationMachineMode::MAP_FACTION_POINT& _mapFactionPoint )
	{
		return m_sFactionManager.UpdatePoint( _mapFactionPoint );
	}


	const bool CCertificationCharacterManager::UpdatePointDirect (
		const DWORD _dwFactionID,
		const DWORD _dwPoint )
	{
		if ( _dwPoint <= 0 )
			return false;

		m_sFactionManager.UpdatePointDirect( _dwFactionID, _dwPoint );

		return true;
	}


	void CCertificationCharacterManager::GameEnd ( void )
	{
		// ������ ��� ĳ���͵��� ���ӱ� ������ �����Ѵ�;
		MAP_CERTIFICATION_CHARACTER_ITER iterUser = m_mapCertificationCharacter.begin();
		MAP_CERTIFICATION_CHARACTER_ITER endIter = m_mapCertificationCharacter.end();
		for ( ; iterUser != endIter; ++iterUser )
		{
			CCertificationCharacter& sTempUser = iterUser->second;
			sTempUser.GameEnd();
		}

		m_sFactionManager.GameEnd();
	}


	const bool CCertificationCharacterManager::GameReward ( void )
	{
		return m_sFactionManager.GameReward();
	}


	void CCertificationCharacterManager::SyncronizeFactionInfo (
		const DWORD _dwFactionID,
		const DWORD _dwPoint,
		const DWORD _dwCertifyCount )
	{
		m_sFactionManager.Syncronize( _dwFactionID, _dwPoint, _dwCertifyCount );
	}


	void CCertificationCharacterManager::SyncronizeCertificationMachine (
		const DWORD _dwMachineID,
		const DWORD _dwFactionID )
	{
		m_sFactionManager.SyncronizeCertificationMachine( _dwMachineID, _dwFactionID );
	}


	void CCertificationCharacterManager::SendSyncronizeMessage ( void )
	{
		m_sFactionManager.SendSyncronizeMessage();
	}


	void CCertificationCharacterManager::CertificationMachineNeutralization (
		const DWORD _dwFactionID,
		const DWORD _dwMachineID )
	{
		m_sFactionManager.CertificationMachineNeutralization( _dwFactionID, _dwMachineID );
	}


	const DWORD	CCertificationCharacterManager::GetCertifyingMachine (
		const DWORD _dwChaDBNum )
	{
		MAP_CERTIFICATION_CHARACTER_ITER iterUser = m_mapCertificationCharacter.find ( _dwChaDBNum );
		if ( iterUser !=  m_mapCertificationCharacter.end() )
		{
			const CCertificationCharacter& sTempUser = iterUser->second;
			return sTempUser.GetCertifyingMachine();
		}

		return MAXDWORD;
	}


	const DWORD CCertificationCharacterManager::GetCertifyingFactionID (
		const DWORD _dwChaDBNum )
	{
		MAP_CERTIFICATION_CHARACTER_ITER iterUser = m_mapCertificationCharacter.find ( _dwChaDBNum );
		if ( iterUser !=  m_mapCertificationCharacter.end() )
		{
			const CCertificationCharacter& sTempUser = iterUser->second;
			return sTempUser.GetFactionID();
		}

		return MAXDWORD;
	}


	const bool CCertificationCharacterManager::IsAddPointFaction (
		const DWORD _dwChaDBNum )
	{
		DWORD dwFactionID = 0;
		MAP_CERTIFICATION_CHARACTER_ITER iterUser = m_mapCertificationCharacter.find ( _dwChaDBNum );
		if ( iterUser !=  m_mapCertificationCharacter.end() )
		{
			const CCertificationCharacter& sTempUser = iterUser->second;
			dwFactionID = sTempUser.GetFactionID();
		}
		else
		{
			return false;
		}

		return m_sFactionManager.IsAddPoint( dwFactionID );
	}


	const DWORD	CCertificationCharacterManager::GetPointFaction (
		const DWORD _dwFactionID )
	{
		return m_sFactionManager.GetPoint( _dwFactionID );
	}


	const bool CCertificationCharacterManager::IsEmpty ( void )
	{
		return m_mapCertificationCharacter.empty();
	}

}