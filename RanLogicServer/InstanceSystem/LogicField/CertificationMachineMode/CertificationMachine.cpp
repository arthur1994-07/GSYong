#include "pch.h"

#include "../IInstanceFieldMode.h"

#include "./InstanceDungeon_CertificationMachine.h"

#include "./CertificationMachine.h"

namespace InstanceSystem
{

	void CCertificationMachine::CreateMachine ( void )
	{
		if ( NULL == m_pInstanceField )
			return;

		m_dwCrowGaeaID = m_pInstanceField->DropCrow(
			m_sInfo.ID_MAP,
			m_sInfo.ID_CROW,
			D3DXVECTOR3( m_sInfo.POSITION.x, 0.f, m_sInfo.POSITION.z ),
			m_sInfo.ROTATE );

		if ( GAEAID_NULL == m_dwCrowGaeaID )
			return;

		m_dwMaxHP = m_pInstanceField->GetMaxHP( Faction::GLActorID( CROW_NPC, m_dwCrowGaeaID ) );

		if ( MAXDWORD != m_dwFactionID )
		{
			// �������� Effect �� ǥ���Ѵ�;
			m_pInstanceField->DoEffectPositionAll(
				m_sInfo.STR_CERTIFICATION_EFFECT,
				m_sInfo.ID_MAP,
				m_pInstanceField->GetActorPosition( Faction::GLActorID( CROW_NPC, m_dwCrowGaeaID ) ) );

			// �������� Effect Buff �� �����Ѵ�;
			m_pInstanceField->ApplySkill(
				Faction::GLActorID( CROW_NPC, m_dwCrowGaeaID ),
				m_sInfo.ID_CERTIFY_EFFECT_BUFF,
				1 );

			// �������� ������ ������ �������� �����Ѵ�;
			m_pInstanceField->SetFaction(
				Faction::GLActorID( CROW_NPC, m_dwCrowGaeaID ),
				Faction::FactionID( Faction::EMFACTION_TEAM, m_dwFactionID ) );

			// Max Point Count �� �����Ѵ�;
			m_dwMaxPointCount = m_sInfo.NUM_POINT_COUNT;
		}
		else
		{
			// �������� ������ �߸����� �����Ѵ�;
			m_pInstanceField->SetFaction(
				Faction::GLActorID( CROW_NPC, m_dwCrowGaeaID ),
				Faction::FactionID( Faction::EMFACTION_BASIC, CertificationMachineMode::EMFACTION_BASIC_NEUTRAL ) );
		}
	}


	void CCertificationMachine::CertificationMachine (
		const DWORD _dwFactionID,
		const DWORD _dwChaDBNum,
		const bool _bAddPoint )
	{
		if ( NULL == m_pInstanceField )
			return;

		if ( NULL == m_pDungeon )
			return;

		if ( GAEAID_NULL == m_dwCrowGaeaID )
			return;

		// �������� Effect �� ǥ���Ѵ�;
		m_pInstanceField->DoEffectPositionAll(
			m_sInfo.STR_CERTIFICATION_EFFECT,
			m_sInfo.ID_MAP,
			m_pInstanceField->GetActorPosition( Faction::GLActorID( CROW_NPC, m_dwCrowGaeaID ) ) );

		// �������� Effect Buff �� �����Ѵ�;
		m_pInstanceField->ApplySkill(
			Faction::GLActorID( CROW_NPC, m_dwCrowGaeaID ),
			m_sInfo.ID_CERTIFY_EFFECT_BUFF,
			1 );

		// ���������� �����Ѵ�;
		SetFactionID( _dwFactionID );

		// �������� ������ ������ �������� �����Ѵ�;
		m_pInstanceField->SetFaction(
			Faction::GLActorID( CROW_NPC, m_dwCrowGaeaID ),
			Faction::FactionID( Faction::EMFACTION_TEAM, _dwFactionID ) );

		// �����⸦ �ʱ�ȭ�Ѵ�;
		m_dwMaxPointCount = m_sInfo.NUM_POINT_COUNT;
		m_dwCurPointCount = 0;

		// �������� HP�� ȸ����Ų��;
		const DWORD dwMaxHP = m_pInstanceField->GetMaxHP( Faction::GLActorID( CROW_NPC, m_dwCrowGaeaID ) );
		m_pInstanceField->SetHP( Faction::GLActorID( CROW_NPC, m_dwCrowGaeaID ), dwMaxHP );

		// Add Point ���뿩�θ� Ȯ���ϰ� �����Ѵ�;
		m_bAddPoint = _bAddPoint;
		if ( m_bAddPoint )
			m_dwMaxPointCount = m_sInfo.NUM_POINT_COUNT + m_sInfo.NUM_ADD_POINT_COUNT;
		else
			m_dwMaxPointCount = m_sInfo.NUM_POINT_COUNT;

		// �����⸦ ���� �Ұ����� ���·� �����Ѵ�;
		InactivateMachine();

		// ���� ������ �˸���;
		m_pInstanceField->RegistTimer(
			0,
			CertificationMachineMode::EMTIMER_ID_CERTIFY_SUCCESS_NOTIFY,
			_dwChaDBNum,
			m_dwCrowGaeaID,
			static_cast< DWORD >( m_bAddPoint ) );

		// ���� ������ ���·� �����ϴ� Ÿ�̸Ӹ� ����Ѵ�;
		m_pInstanceField->RegistTimer(
			m_pDungeon->TIME_CERTIFICATION_MACHINE_LOCKTIME,
			CertificationMachineMode::EMTIMER_ID_CERTIFICATION_MACHINE_LOCKTIME,
			m_dwID );
	}


	const DWORD CCertificationMachine::GeneratePoint ( void )
	{
		if ( NULL == m_pInstanceField )
			return 0;

		if ( GAEAID_NULL == m_dwCrowGaeaID )
			return 0;

		int nPointRate = 1;
		if ( m_bAddPoint )
			nPointRate = 2;

		if ( 0 <= m_dwCurPointCount && m_dwCurPointCount < m_dwMaxPointCount )
		{
			// Point �� �����Ѵ�;
			DWORD dwPoint = m_sInfo.NUM_POINT * nPointRate;

			// �������� HP �� ���ҽ�Ų��;
			DWORD dwDecreaseHP = m_dwMaxHP / m_dwMaxPointCount;
			DWORD dwCurrentHP = m_dwMaxHP - dwDecreaseHP * m_dwCurPointCount;
			
			m_pInstanceField->SetHP( Faction::GLActorID( CROW_NPC, m_dwCrowGaeaID ), dwCurrentHP );

			m_dwCurPointCount++;

			if ( m_dwCurPointCount == 24 )
			{
				// �޽��� Ÿ�̸� ���;
				m_pInstanceField->RegistTimer(
					0,
					CertificationMachineMode::EMTIMER_ID_CERTIFICATION_MACHINE_NEUTRALIZATION_WAIT_NOTIFY,
					m_dwFactionID,
					m_dwCrowGaeaID,
					60 );
			}
			else if ( m_dwCurPointCount == 30 )
			{
				// �޽��� Ÿ�̸� ���;
				m_pInstanceField->RegistTimer(
					0,
					CertificationMachineMode::EMTIMER_ID_CERTIFICATION_MACHINE_NEUTRALIZATION_WAIT_NOTIFY,
					m_dwFactionID,
					m_dwCrowGaeaID,
					30 );
			}

			// �����⸦ �߸�ȭ�Ѵ�;
			if ( m_dwMaxPointCount <= m_dwCurPointCount )
			{
				m_pInstanceField->RegistTimer(
					0,
					CertificationMachineMode::EMTIMER_ID_CERTIFICATION_MACHINE_NEUTRALIZATION,
					m_dwFactionID,
					m_dwID );

				_Neutralization();
			}

			return dwPoint;
		}

		return 0;
	}


	void CCertificationMachine::Syncronize (
		const DWORD _dwFactionID,
		const DWORD _dwCurPointCount )
	{
		if ( MAXDWORD == _dwFactionID )
			return;

		m_dwFactionID = _dwFactionID;
		m_dwCurPointCount = _dwCurPointCount;
	}


	void CCertificationMachine::SendSyncronizeMessage ( void ) const
	{
		if ( NULL == m_pInstanceField )
			return;

		m_pInstanceField->SendCustomMessage(
			EM_MESSAGE_AGENT_BASE,
			m_pInstanceField->GetKeyMapID().Mid(),
			CInstanceDungeon_CertificationMachine::EMCUSTOM_MSG_CERTIFICATION_MODE_UPDATE_CERTIFYMACHINE,
			m_dwID,
			m_dwFactionID,
			m_dwCurPointCount );
	}


	void CCertificationMachine::_Neutralization ( void )
	{
		if ( GAEAID_NULL == m_dwCrowGaeaID )
			return;

		DWORD dwPreFactionID = m_dwFactionID;

		m_dwFactionID = MAXDWORD;
		m_dwCurPointCount = 0;
		m_dwMaxPointCount = 0;
		m_bAddPoint = false;

		// �������� HP�� ȸ����Ų��;
		const DWORD dwMaxHP = m_pInstanceField->GetMaxHP( Faction::GLActorID( CROW_NPC, m_dwCrowGaeaID ) );
		m_pInstanceField->SetHP( Faction::GLActorID( CROW_NPC, m_dwCrowGaeaID ), dwMaxHP );

		// �߸�ȭ Effect �� ǥ���Ѵ�;
		m_pInstanceField->DoEffectPositionAll(
			m_sInfo.STR_UNCERTIFICATION_EFFECT,
			m_sInfo.ID_MAP,
			m_pInstanceField->GetActorPosition( Faction::GLActorID( CROW_NPC, m_dwCrowGaeaID ) ) );

		// �������� Effect Buff �� �����Ѵ�;
		m_pInstanceField->RemoveSkillFact(
			Faction::GLActorID( CROW_NPC, m_dwCrowGaeaID ),
			m_sInfo.ID_CERTIFY_EFFECT_BUFF );

		// �������� ������ �߸����� �����Ѵ�;
		m_pInstanceField->SetFaction(
			Faction::GLActorID( CROW_NPC, m_dwCrowGaeaID ),
			Faction::FactionID( Faction::EMFACTION_BASIC, CertificationMachineMode::EMFACTION_BASIC_NEUTRAL ) );

		// �޽��� Ÿ�̸� ���;
		m_pInstanceField->RegistTimer(
			0,
			CertificationMachineMode::EMTIMER_ID_CERTIFICATION_MACHINE_NEUTRALIZATION_NOTIFY,
			dwPreFactionID,
			m_dwCrowGaeaID );
	}




	void CCertificationMachineManager::AddCertificationMachine (
		const SCERTIFICATION_MACHINE_INFO& _sMachineInfo )
	{
		DWORD dwID = m_vecCertificationMachine.size();

		CCertificationMachine sMachine(
			m_pInstanceField,
			m_pDungeon,
			dwID,
			_sMachineInfo );

		m_vecCertificationMachine.push_back( sMachine );
	}


	void CCertificationMachineManager::CreateAllCertificationMachine ()
	{
		for ( int i=0; i<m_vecCertificationMachine.size(); ++i )
		{
			CCertificationMachine& sMachine = m_vecCertificationMachine.at( i );
			sMachine.CreateMachine();
		}

		m_pInstanceField->SendCustomMessage(
			EM_MESSAGE_AGENT_BASE,
			m_pInstanceField->GetKeyMapID().Mid(),
			m_pDungeon->EMCUSTOM_MSG_CERTIFICATION_MODE_UPDATE_CERTIFYMACHINE );
	}


	const DWORD CCertificationMachineManager::GetCertificationMachineID (
		const DWORD _dwGaeaID )
	{
		for ( int i=0; i<m_vecCertificationMachine.size(); ++i )
		{
			const CCertificationMachine& sMachine = m_vecCertificationMachine.at( i );
			if ( sMachine.GetGaeaID() == _dwGaeaID )
				return sMachine.GetID();
		}

		return MAXDWORD;
	}


	const DWORD CCertificationMachineManager::GetCertificationMachineGaeaID (
		const DWORD _dwMachineID )
	{
		for ( int i=0; i<m_vecCertificationMachine.size(); ++i )
		{
			const CCertificationMachine& sMachine = m_vecCertificationMachine.at( i );
			if ( sMachine.GetID() == _dwMachineID )
				return sMachine.GetGaeaID();
		}

		return MAXDWORD;
	}


	const DWORD CCertificationMachineManager::GetCapturedFaction (
		const DWORD _dwMachineID )
	{
		if ( _dwMachineID < 0 || m_vecCertificationMachine.size() <= _dwMachineID )
			return MAXDWORD;

		const CCertificationMachine& sTempMachine = m_vecCertificationMachine.at( _dwMachineID );
		return sTempMachine.GetFactionID();
	}


	void CCertificationMachineManager::CertificationMachine (
		const DWORD _dwMachineID,
		const DWORD _dwFactionID,
		const DWORD _dwChaDBNum,
		const bool _bAddPoint )
	{
		if ( _dwMachineID < 0 || m_vecCertificationMachine.size() <= _dwMachineID )
			return;

		CCertificationMachine& sTempMachine = m_vecCertificationMachine.at( _dwMachineID );
		sTempMachine.CertificationMachine( _dwFactionID, _dwChaDBNum, _bAddPoint );
	}


	void CCertificationMachineManager::GenerateFactionPoint (
		CertificationMachineMode::MAP_FACTION_POINT& _refMapFactionPoint )
	{
		for ( int i=0; i<m_vecCertificationMachine.size(); ++i )
		{
			CCertificationMachine& sMachine = m_vecCertificationMachine.at( i );

			const DWORD dwFactionID = sMachine.GetFactionID();
			if ( MAXDWORD == dwFactionID )
				continue;
			
			const DWORD dwAddPoint = sMachine.GeneratePoint();
			CertificationMachineMode::MAP_FACTION_POINT_ITER findIter = _refMapFactionPoint.find( dwFactionID );
			if ( findIter != _refMapFactionPoint.end() )
			{
				DWORD& dwPoint = findIter->second;
				dwPoint += dwAddPoint;
			}
			else
			{
				_refMapFactionPoint.insert( CertificationMachineMode::MAP_FACTION_POINT_VALUE( dwFactionID, dwAddPoint ) );
			}
		}

		// �� Faction Point Map;
		CertificationMachineMode::MAP_FACTION_POINT_ITER pointIter = _refMapFactionPoint.begin();
		CertificationMachineMode::MAP_FACTION_POINT_ITER eIter = _refMapFactionPoint.end();
		for ( ; pointIter != eIter; ++pointIter )
		{
			DWORD& dwPoint = pointIter->second;
			if ( NUM_ONCE_LIMIT_POINT < dwPoint )
				dwPoint = NUM_ONCE_LIMIT_POINT;
		}
	}


	void CCertificationMachineManager::Syncronize (
		const DWORD _dwMachineID,
		const DWORD _dwFactionID,
		const DWORD _dwCurPointCount )
	{
		if ( _dwMachineID < 0 || m_vecCertificationMachine.size() <= _dwMachineID )
			return;

		CCertificationMachine& sTempMachine = m_vecCertificationMachine.at( _dwMachineID );
		sTempMachine.Syncronize( _dwFactionID, _dwCurPointCount );
	}


	void CCertificationMachineManager::SendSyncronizeMessage ( void )
	{
		for ( int i=0; i<m_vecCertificationMachine.size(); ++i )
		{
			const CCertificationMachine& sMachine = m_vecCertificationMachine.at( i );
			sMachine.SendSyncronizeMessage();
		}
	}


	void CCertificationMachineManager::SetMinimapInfo (
		const SNATIVEID& _sMapID,
		GLMSG::NET_INSTANCE_GUIDANCE_UI_MINIMAP_FC* _pNetMsg )
	{
		if ( NULL == _pNetMsg )
			return;

		for ( int i=0; i<m_vecCertificationMachine.size(); ++i )
		{
			const CCertificationMachine& sMachine = m_vecCertificationMachine.at( i );

			if ( _sMapID != sMachine.GetMapID() )
				continue;

			GLMSG::NET_INSTANCE_GUIDANCE_UI_MINIMAP_FC::AuthClubInfo sCertificationMachineInfo;
			if ( MAXDWORD != sMachine.GetFactionID() )
				sCertificationMachineInfo.clubID = sMachine.GetFactionID();
			else
				sCertificationMachineInfo.clubID = 0;
			sCertificationMachineInfo.fXpos = sMachine.GetPosition().x;
			sCertificationMachineInfo.fZpos = sMachine.GetPosition().z;

			_pNetMsg->vecAuthClub.push_back( sCertificationMachineInfo );
		}
	}


	const bool CCertificationMachineManager::IsValidation ( const DWORD _dwMachineID )
	{
		if ( _dwMachineID < 0 || m_vecCertificationMachine.size() <= _dwMachineID )
			return false;

		const CCertificationMachine& sTempMachine = m_vecCertificationMachine.at( _dwMachineID );
		return sTempMachine.IsValidation();
	}


	void CCertificationMachineManager::ActivateMachine ( const DWORD _dwMachineID )
	{
		if ( _dwMachineID < 0 || m_vecCertificationMachine.size() <= _dwMachineID )
			return;

		CCertificationMachine& sTempMachine = m_vecCertificationMachine.at( _dwMachineID );
		sTempMachine.ActivateMachine();
	}


	void CCertificationMachineManager::InctivateMachine ( const DWORD _dwMachineID )
	{
		if ( _dwMachineID < 0 || m_vecCertificationMachine.size() <= _dwMachineID )
			return;

		CCertificationMachine& sTempMachine = m_vecCertificationMachine.at( _dwMachineID );
		sTempMachine.InactivateMachine();
	}


	void CCertificationMachineManager::ActivateAddPoint ( const DWORD _dwMachineID )
	{
		if ( _dwMachineID < 0 || m_vecCertificationMachine.size() <= _dwMachineID )
			return;

		CCertificationMachine& sTempMachine = m_vecCertificationMachine.at( _dwMachineID );
		sTempMachine.ActivateAddPoint();
	}


	void CCertificationMachineManager::InactivateAddPoint ( const DWORD _dwMachineID )
	{
		if ( _dwMachineID < 0 || m_vecCertificationMachine.size() <= _dwMachineID )
			return;

		CCertificationMachine& sTempMachine = m_vecCertificationMachine.at( _dwMachineID );
		sTempMachine.InactivateAddPoint();
	}


	void CCertificationMachineManager::ConfirmCertification ( void )
	{
		if ( NULL == m_pInstanceField )
			return;

		for ( int i=0; i<m_vecCertificationMachine.size(); ++i )
		{
			const CCertificationMachine& sMachine = m_vecCertificationMachine.at( i );
			const DWORD dwMachineGaeaID = sMachine.GetGaeaID();
			const DWORD dwFactionID = sMachine.GetFactionID();

			m_pInstanceField->RegistTimer(
				0,
				CertificationMachineMode::EMTIMER_ID_CONFIRM_CERTIFICATION_NOTIFY,
				dwMachineGaeaID,
				dwFactionID );
		}
	}

}