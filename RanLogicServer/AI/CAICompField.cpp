#include "pch.h"

#include "./GLFSMContainer.h"
#include "./GLScriptedFSM.h"

#include "./CAICompField.h"

#include "../SigmaCore/DebugInclude.h"

namespace AISystem
{
	CAICompField::CAICompField (
		ServerActor* pMe )
		: IComponentBase( pMe )
		, m_pMe( pMe )
		, m_dwType( 0 )
		, m_strCurTableName( AISystem::LuaFSMString::g_strTable_Default )
	{
	}

	CAICompField::~CAICompField () { Destroy(); }

	const ComponentSystem::EMCOMP_ID CAICompField::GetComponentID () const
	{
		return ComponentSystem::EMCOMP_AI;
	}

	const ComponentSystem::EMFAMILY_ID CAICompField::GetFamilyID () const
	{
		return ComponentSystem::EMFAMILY_AI;
	}

	void CAICompField::Initialize ()
	{
		if ( !GLFSMContainer::Instance()->GetFSM( m_dwType )->IsValid() )
			return;

		if ( !m_pMe )
			return;

		GLFSMContainer::Instance()->GetFSM( m_dwType )->Begin(
			m_strCurTableName,
			m_pMe );
	}

	void CAICompField::Destroy ()
	{
		/*if ( !GLFSMContainer::Instance()->GetFSM( m_dwType )->IsValid() )
			return;

		GLFSMContainer::Instance()->GetFSM( m_dwType )->End(
			m_strCurTableName,
			m_pMe );*/
	}

	void CAICompField::Reset ()
	{

	}

	void CAICompField::BeginUpdate ()
	{
	}

	void CAICompField::Update ( float fElapsedTime )
	{
		if ( !GLFSMContainer::Instance()->GetFSM( m_dwType )->IsValid() )
			return;

		if ( !m_pMe )
			return;

		GLFSMContainer::Instance()->GetFSM( m_dwType )->Update(
			m_strCurTableName,
			m_pMe,
			fElapsedTime );
	}

	void CAICompField::EndUpdate ()
	{

	}

	// 상태 변환;
	void CAICompField::ChangeState ( const std::string& strChangeTableName )
	{
		if ( !GLFSMContainer::Instance()->GetFSM( m_dwType )->IsValid() )
			return;

		if ( !m_pMe )
			return;

		GLFSMContainer::Instance()->GetFSM( m_dwType )->End(
			m_strCurTableName,
			m_pMe );

		m_strCurTableName = strChangeTableName;

		GLFSMContainer::Instance()->GetFSM( m_dwType )->Begin(
			m_strCurTableName,
			m_pMe );
	}

	void CAICompField::Attack ()
	{
		if ( !GLFSMContainer::Instance()->GetFSM( m_dwType )->IsValid() )
			return;

		if ( !m_pMe )
			return;

		GLFSMContainer::Instance()->GetFSM( m_dwType )->Attack(
			m_strCurTableName,
			m_pMe );
	}

	void CAICompField::Attacked ( ServerActor* const pAttacker )
	{
		if ( !GLFSMContainer::Instance()->GetFSM( m_dwType )->IsValid() )
			return;

		if ( !m_pMe )
			return;

		GLFSMContainer::Instance()->GetFSM( m_dwType )->Attacked(
			m_strCurTableName,
			m_pMe,
			pAttacker );

		/*if ( !m_pMe )
		{
			return false;
		}

		STARGETID sTargetID = STARGETID ( sAID.actorType, sAID.actorID_Num );
		if( !m_pMe->isAction( GLAT_ATTACK, GLAT_SKILL ) )
		{
			m_pMe->BasicSkillAction( sTargetID );
		}*/
	}

	void CAICompField::OwnerAttack ()
	{
		if ( !GLFSMContainer::Instance()->GetFSM( m_dwType )->IsValid() )
			return;

		if ( !m_pMe )
			return;

		GLFSMContainer::Instance()->GetFSM( m_dwType )->OwnerAttack(
			m_strCurTableName,
			m_pMe );
	}

	void CAICompField::OwnerAttacked ( ServerActor* const pAttacker )
	{
		if ( !GLFSMContainer::Instance()->GetFSM( m_dwType )->IsValid() )
			return;

		if ( !m_pMe )
			return;

		GLFSMContainer::Instance()->GetFSM( m_dwType )->OwnerAttacked(
			m_strCurTableName,
			m_pMe,
			pAttacker );
	}

	const DWORD CAICompField::GetTypeMaxCount () const
	{
		if ( !GLFSMContainer::Instance()->GetFSM( m_dwType )->IsValid() )
			return 0;

		return GLFSMContainer::Instance()->GetFSM( m_dwType )->GetTypeMaxCount();
	}
}