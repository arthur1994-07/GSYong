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

		// 맵 입장 최초에는 안전진영으로 세팅된다;
		m_pInstanceField->SetFaction(
			Faction::GLActorID( CROW_PC, m_dwChaDBNum ),
			Faction::FactionID( Faction::EMFACTION_SAFE_TEAM, m_dwFactionID ) );

		const DWORD dwAllTime = m_pDungeon->GetAllTime();
		const DWORD dwDurationTime = m_pDungeon->GetDurationTime();
		DWORD dwPastTime = dwAllTime - dwDurationTime;

		// 입장 버프를 활성화시킨다;
		if ( m_pDungeon->IsJoinBuff() &&
			dwPastTime < m_pDungeon->TIME_JOIN_BUFFTIME )
		{
			const DWORD dwTime = m_pDungeon->TIME_JOIN_BUFFTIME - dwPastTime;
			ActivateJoinBuff( dwTime );
		}

		// 입장 디버프를 활성화시킨다;
		if ( m_pDungeon->IsJoinDebuff() &&
			dwPastTime < m_pDungeon->TIME_JOIN_DEBUFFTIME )
		{
			const DWORD dwTime = m_pDungeon->TIME_JOIN_DEBUFFTIME - dwPastTime;
			ActivateJoinDebuff( dwTime );
		}

		// 전장맵으로 입장할 경우에는 다른처리를 한다;
		if ( _sEnterMapID == m_pDungeon->MAP_BATTLE )
		{
			// 안전 시간이 지난 후 진영이 바뀌도록 타이머를 등록한다;
			m_pInstanceField->RegistTimer(
				m_pDungeon->TIME_SAFE_TIME,
				CertificationMachineMode::EMTIMER_ID_SAFETIME,
				m_dwChaDBNum );

			// 현재 진영이 AddPoint 상태이면 가속기 버프를 활성화시킨다;
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

		// 안전진영상태를 해제한다;
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

		// 현재 인증중인 인증기가 없고 인증이 가능한 상태이며 진영이 존재하면 처리한다;
		if ( ( MAXDWORD == m_dwCertifyingMachine ) &&
			m_bCanCertification &&
			( 0 != m_dwFactionID ) )
		{
			m_dwCertifyingMachine = _dwMachineID;
			m_bCanCertification = false;

			// 인증시간 감소 버프가 있다면 인증시간을 감소시킨다;
			DWORD dwCertifyTime = _dwCertifyTime;
			if ( m_pInstanceField->IsHaveSkillFact(
				Faction::GLActorID( CROW_PC, m_dwChaDBNum ),
				m_pDungeon->SKILL_CERTIFY_TIME_REDUCE_BUFF ) )
			{
				dwCertifyTime /= 2;
			}

			// 인증을 모션을 부여한다;
			m_pInstanceField->SetMotion(
				Faction::GLActorID( CROW_PC, m_dwChaDBNum ),
				m_pDungeon->MOTION_CERTIFY,
				dwCertifyTime );

			// 인증 모션이 부여되었다면 인증 UI를 보여준다;
			if ( m_pInstanceField->IsMotion(
				Faction::GLActorID( CROW_PC, m_dwChaDBNum ),
				m_pDungeon->MOTION_CERTIFY ) )
			{
				m_pInstanceField->UI_Auth( m_dwChaDBNum, true, dwCertifyTime );
			}

			// 인증을 시작하면 설정되어 있는 스킬효과를 가진 스킬을 삭제한다;
			for ( int i=0; i<m_pDungeon->VEC_REMOVE_SKILLEFFECT_BY_CERTIFYING.size(); ++i )
			{
				const DWORD dwSkillEffect = m_pDungeon->VEC_REMOVE_SKILLEFFECT_BY_CERTIFYING.at( i );

				m_pInstanceField->RemoveSkillFact(
					Faction::GLActorID( CROW_PC, m_dwChaDBNum ),
					static_cast< EMSPEC_ADDON >( dwSkillEffect ) );
			}

			// 인증을 시작하면 설정되어 있는 버프를 삭제한다;
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

		// 인증 모션이 종료될 경우 인증중인 인증기 정보를 초기화한다;
		m_dwCertifyingMachine = MAXDWORD;

		// 인증이 가능하도록 하는 상태까지는 잠시의 텀을 둔다;
		m_pInstanceField->RegistTimer(
			m_pDungeon->TIME_CERTIFY_WAITTIME,
			CertificationMachineMode::EMTIMER_ID_TRY_CERTIFY_WAITTIME,
			m_dwChaDBNum );

		// 인증 UI 를 끈다;
		m_pInstanceField->UI_Auth( m_dwChaDBNum, false, 0 );
	}


	void CCertificationCharacter::SuccessCertification (
		const DWORD _dwMachineID )
	{
		if ( false == m_bValidation )
			return;

		// 인증기의 인증이 성공했는데 인증된 머신을 현재 인증중이라면 취소한다;
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

		// 게임이 종료되면 모션을 취소한다;
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

		// 게임이 종료되면 모션을 취소한다;
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
			// 아직 한번도 입장하지 않은 캐릭터의 경우에는 캐릭터정보를 새로 생성한다;
			CCertificationCharacter sTempUser( m_pInstanceField, m_pDungeon, _dwChaDBNum, _dwFactionID );
			sTempUser.Join( _dwFactionID );

			m_mapCertificationCharacter.insert( MAP_CERTIFICATION_CHARACTER_VALUE( _dwChaDBNum, sTempUser ) );
		}

		// 캐릭터가 소속된 진영에 새로운 캐릭터를 입장시킨다;
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

			// 캐릭터의 진영을 확인한다;
			const DWORD dwTempFactionID = sTempUser.GetFactionID();

			// 진영에서 캐릭터를 한명 퇴장시킨다;
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

			// 이미 인증중이면 실패한다;
			if ( sTempUser.IsCertifying() || sTempUser.IsDie() )
			{
				return false;
			}

			// 인증을 할 수 없는 상태이거나 캐릭터가 유효하지 않으면 실패한다;
			if ( false == sTempUser.CanCertification() ||
				false == sTempUser.IsValidation() )
			{
				m_pInstanceField->RegistTimer(
					0,
					CertificationMachineMode::EMTIMER_ID_CERTIFY_FAIL_NOTIFY,
					_dwChaDBNum );

				return false;
			}

			// 캐릭터의 진영을 확인한다;
			const DWORD dwTempFactionID = sTempUser.GetFactionID();

			// 같은 진영이거나 자기 진영이 이미 인증중인 인증기라면 실패한다;
			if ( false == m_sFactionManager.StartCertification( dwTempFactionID, _dwMachineID, _dwCapturedFactionID ) )
			{
				m_pInstanceField->RegistTimer(
					0,
					CertificationMachineMode::EMTIMER_ID_CERTIFY_CAPTURED_FAIL_NOTIFY,
					_dwChaDBNum,
					_dwCapturedFactionID );

				return false;
			}

			// 동맹 진영일 경우 실패한다;
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

			// 인증을 시작한다;
			if ( false == sTempUser.StartCertification( _dwMachineID, dwCertifyTime ) )
			{
				m_pInstanceField->RegistTimer(
					0,
					CertificationMachineMode::EMTIMER_ID_CERTIFY_FAIL_NOTIFY,
					_dwChaDBNum );

				return false;
			}

			// 인증이 진행될 경우 인증 시도 공지 타이머를 등록한다;
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
			// 인증중인 캐릭터의 진영정보를 가져온다;
			const DWORD dwFactionID = GetCertifyingFactionID( _dwChaDBNum );
			if ( MAXDWORD == dwFactionID )
				return;

			// 캐릭터가 인증중인 인증기정보를 가져온다;
			const DWORD dwMachineID = GetCertifyingMachine( _dwChaDBNum );
			if ( MAXDWORD == dwMachineID )
				return;

			// 인증을 성공시킨다;
			m_sFactionManager.SuccessCertification( dwFactionID, dwMachineID );

			// 캐릭터가 인증 점수 획득 버프를 보유하고 있을경우 진영을 AddPoint 상태로 변경한다;
			if ( m_pInstanceField->IsHaveSkillFact(
				Faction::GLActorID( CROW_PC, _dwChaDBNum ),
				m_pDungeon->SKILL_CERTIFY_ADD_POINT_BUFF ) )
			{
				m_sFactionManager.ActivateAddPoint( dwFactionID );
			}

			// 모든 캐릭터에게 특정 인증기의 인증이 성공하였다고 알린다;
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

		// Faction ID 를 찾는다;
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

		// 이미 가속기 버프해제 타이머가 등록되어있다면 제거해준다 ( 초기화를 위함이다 );
		if ( m_pInstanceField->IsRegistTimer( m_hTimerAcceleratorBuff ) )
		{
			m_pInstanceField->RemoveTimer( m_hTimerAcceleratorBuff );
		}

		// 진영의 AddPointBuff 부여 상태를 활성화시킨다;
		m_sFactionManager.ActivateAddPointBuff( dwFactionID );

		// 진영에 속한 인증기들의 AddPoint 상태를 활성화시킨다;
		m_sFactionManager.ActivateAddPoint_CertificationMachine( dwFactionID );

		// 진영의 모든 캐릭터들의 가속기 버프를 활성화시킨다;
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

		// 가속기 버프 해제 타이머를 등록한다;
		m_hTimerAcceleratorBuff = m_pInstanceField->RegistTimer(
			m_pDungeon->TIME_ACCELERATOR_BUFFTIME,
			CertificationMachineMode::EMTIMER_ID_ACCELERATOR_BUFFTIME,
			dwFactionID );
	}


	void CCertificationCharacterManager::ExitAcceleratorBuff (
		const DWORD _dwFactionID )
	{
		// 진영의 AddPointBuff 부여 상태를 비활성화시킨다;
		m_sFactionManager.InactivateAddPointBuff( _dwFactionID );

		// 진영의 모든 캐릭터들의 가속기 버프를 해제한다;
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
		// 진영의 모든 캐릭터들의 가속기 버프를 해제한다;
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