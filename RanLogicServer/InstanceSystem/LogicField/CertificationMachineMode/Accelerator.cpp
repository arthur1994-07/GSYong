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

			// 이펙트를 위한 스킬을 걸어준다;
			m_pInstanceField->ApplySkill(
				Faction::GLActorID( CROW_MOB, m_dwCrowGaeaID ),
				m_sInfo.ID_EFFECT,
				1 );
		}
		else
		{
			// 진영을 유저와 적으로 변경해서 공격할 수 있도록 한다;
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

		// 진영을 유저와 적으로 변경해서 공격할 수 있도록 한다;
		m_pInstanceField->SetFaction(
			Faction::GLActorID( CROW_MOB, m_dwCrowGaeaID ),
			Faction::FactionID( Faction::EMFACTION_BASIC, CertificationMachineMode::EMFACTION_BASIC_ENEMY ) );

		// 이펙트 효과를 제거한다;
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

		// 진영을 유저와 아군으로 변경해서 공격할 수 없도록 한다;
		m_pInstanceField->SetFaction(
			Faction::GLActorID( CROW_MOB, m_dwCrowGaeaID ),
			Faction::FactionID( Faction::EMFACTION_BASIC, CertificationMachineMode::EMFACTION_BASIC_ALLY ) );

		// 이펙트 효과를 준다;
		m_pInstanceField->DoEffectPositionAll(
			m_sInfo.STR_EFFECT1,
			m_sInfo.ID_MAP,
			m_pInstanceField->GetActorPosition( Faction::GLActorID( CROW_MOB, m_dwCrowGaeaID ) ) );
		m_pInstanceField->DoEffectPositionAll(
			m_sInfo.STR_EFFECT2,
			m_sInfo.ID_MAP,
			m_pInstanceField->GetActorPosition( Faction::GLActorID( CROW_MOB, m_dwCrowGaeaID ) ) );

		// 이펙트를 위한 스킬을 걸어준다;
		m_pInstanceField->ApplySkill(
			Faction::GLActorID( CROW_MOB, m_dwCrowGaeaID ),
			m_sInfo.ID_EFFECT,
			1 );

		// 가속기의 HP를 전부 회복시킨다;
		const DWORD dwMaxHP = m_pInstanceField->GetMaxHP( Faction::GLActorID( CROW_MOB, m_dwCrowGaeaID ) );
		m_pInstanceField->SetHP( Faction::GLActorID( CROW_MOB, m_dwCrowGaeaID ), dwMaxHP );
	}




	void CAcceleratorManager::AddAccelerator (
		const SACCELERATOR_INFO& _sMachineInfo )
	{
		// 새로운 Machine ID 를 가져온다;
		DWORD dwID = m_vecAccelerator.size();

		// 가속기를 생성한다;
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
				// 점령되었으므로 비활성화시킨다;
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