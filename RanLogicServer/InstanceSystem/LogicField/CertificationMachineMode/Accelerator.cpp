#include "pch.h"

#include "../IInstanceFieldMode.h"

#include "./InstanceDungeon_CertificationMachine.h"

#include "./Accelerator.h"

namespace InstanceSystem
{

	void CAccelerator::CreateMachine ( void )
	{
		if ( NULL == m_pInstanceField )
			return;

		m_dwCrowGaeaID = m_pInstanceField->DropCrow(
			m_sInfo.ID_MAP,
			m_sInfo.ID_CROW,
			D3DXVECTOR3( m_sInfo.POSITION.x, 0.f , m_sInfo.POSITION.z ),
			m_sInfo.ROTATE );

		if ( GAEAID_NULL == m_dwCrowGaeaID )
			return;

		if ( false == m_bActivate )
		{
			m_pInstanceField->SetFaction(
				Faction::GLActorID( CROW_MOB, m_dwCrowGaeaID ),
				Faction::FactionID( Faction::EMFACTION_BASIC, CertificationMachineMode::EMFACTION_BASIC_ALLY ) );

			// ����Ʈ�� ���� ��ų�� �ɾ��ش�;
			m_pInstanceField->ApplySkill(
				Faction::GLActorID( CROW_MOB, m_dwCrowGaeaID ),
				m_sInfo.ID_EFFECT,
				1 );
		}
		else
		{
			// ������ ������ ������ �����ؼ� ������ �� �ֵ��� �Ѵ�;
			m_pInstanceField->SetFaction(
				Faction::GLActorID( CROW_MOB, m_dwCrowGaeaID ),
				Faction::FactionID( Faction::EMFACTION_BASIC, CertificationMachineMode::EMFACTION_BASIC_ENEMY ) );
		}
	}


	void CAccelerator::Activate ( void )
	{
		m_bActivate = true;

		if ( GAEAID_NULL == m_dwCrowGaeaID )
			return;

		// ������ ������ ������ �����ؼ� ������ �� �ֵ��� �Ѵ�;
		m_pInstanceField->SetFaction(
			Faction::GLActorID( CROW_MOB, m_dwCrowGaeaID ),
			Faction::FactionID( Faction::EMFACTION_BASIC, CertificationMachineMode::EMFACTION_BASIC_ENEMY ) );

		// ����Ʈ ȿ���� �����Ѵ�;
		m_pInstanceField->RemoveSkillFact(
			Faction::GLActorID( CROW_MOB, m_dwCrowGaeaID ),
			m_sInfo.ID_EFFECT );
	}


	void CAccelerator::Inactivate ( void )
	{
		if ( NULL == m_pInstanceField )
			return;

		m_bActivate = false;

		if ( GAEAID_NULL == m_dwCrowGaeaID )
			return;

		// ������ ������ �Ʊ����� �����ؼ� ������ �� ������ �Ѵ�;
		m_pInstanceField->SetFaction(
			Faction::GLActorID( CROW_MOB, m_dwCrowGaeaID ),
			Faction::FactionID( Faction::EMFACTION_BASIC, CertificationMachineMode::EMFACTION_BASIC_ALLY ) );

		// ����Ʈ ȿ���� �ش�;
		m_pInstanceField->DoEffectPositionAll(
			m_sInfo.STR_EFFECT1,
			m_sInfo.ID_MAP,
			m_pInstanceField->GetActorPosition( Faction::GLActorID( CROW_MOB, m_dwCrowGaeaID ) ) );
		m_pInstanceField->DoEffectPositionAll(
			m_sInfo.STR_EFFECT2,
			m_sInfo.ID_MAP,
			m_pInstanceField->GetActorPosition( Faction::GLActorID( CROW_MOB, m_dwCrowGaeaID ) ) );

		// ����Ʈ�� ���� ��ų�� �ɾ��ش�;
		m_pInstanceField->ApplySkill(
			Faction::GLActorID( CROW_MOB, m_dwCrowGaeaID ),
			m_sInfo.ID_EFFECT,
			1 );

		// ���ӱ��� HP�� ���� ȸ����Ų��;
		const DWORD dwMaxHP = m_pInstanceField->GetMaxHP( Faction::GLActorID( CROW_MOB, m_dwCrowGaeaID ) );
		m_pInstanceField->SetHP( Faction::GLActorID( CROW_MOB, m_dwCrowGaeaID ), dwMaxHP );
	}




	void CAcceleratorManager::AddAccelerator (
		const SACCELERATOR_INFO& _sMachineInfo )
	{
		// ���ο� Machine ID �� �����´�;
		DWORD dwID = m_vecAccelerator.size();

		// ���ӱ⸦ �����Ѵ�;
		CAccelerator sMachine(
			m_pInstanceField,
			m_pDungeon,
			dwID,
			_sMachineInfo );

		m_vecAccelerator.push_back( sMachine );
	}


	void CAcceleratorManager::CreateAllAccelerator ( void )
	{
		for ( int i=0; i<m_vecAccelerator.size(); ++i )
		{
			CAccelerator& sMachine = m_vecAccelerator.at( i );
			sMachine.CreateMachine();
		}
	}


	void CAcceleratorManager::ActivateAllAccelerator ( void )
	{
		m_bActivate = true;

		for ( int i=0; i<m_vecAccelerator.size(); ++i )
		{
			CAccelerator& sMachine = m_vecAccelerator.at( i );
			sMachine.Activate();
		}
	}


	const bool CAcceleratorManager::CaptureTheAccelerator (
		const DWORD _dwAcceleratorGaeaID )
	{
		for ( int i=0; i<m_vecAccelerator.size(); ++i )
		{
			CAccelerator& sMachine = m_vecAccelerator.at( i );
			if ( sMachine.GetGaeaID() == _dwAcceleratorGaeaID )
			{
				// ���ɵǾ����Ƿ� ��Ȱ��ȭ��Ų��;
				sMachine.Inactivate();

				return true;
			}
		}

		return false;
	}


	const DWORD CAcceleratorManager::GetPointAccelerator (
		const DWORD _dwAcceleratorGaeaID )
	{
		for ( int i=0; i<m_vecAccelerator.size(); ++i )
		{
			const CAccelerator& sMachine = m_vecAccelerator.at( i );
			if ( sMachine.GetGaeaID() == _dwAcceleratorGaeaID )
			{
				return sMachine.GetPoint();
			}
		}

		return 0;
	}


	void CAcceleratorManager::SetMinimapInfo (
		const SNATIVEID& _sMapID, 
		GLMSG::NET_INSTANCE_GUIDANCE_UI_MINIMAP_FC* _pNetMsg )
	{
		if ( NULL == _pNetMsg )
			return;

		for ( int i=0; i<m_vecAccelerator.size(); ++i )
		{
			const CAccelerator& sMachine = m_vecAccelerator.at( i );
			if ( false == sMachine.GetActivate() )
				continue;

			if ( _sMapID != sMachine.GetMapID() )
				continue;

			GLMSG::NET_INSTANCE_GUIDANCE_UI_MINIMAP_FC::AcceleratorInfo sAcceleratorInfo;
			sAcceleratorInfo.fXpos = sMachine.GetPosition().x;
			sAcceleratorInfo.fZpos = sMachine.GetPosition().z;

			_pNetMsg->vecAccelerator.push_back( sAcceleratorInfo );
		}
	}

}