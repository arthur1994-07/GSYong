#include "pch.h"

#include "./CMobCompField.h"

#include "../FieldServer/GLGaeaServer.h"

#include "../SigmaCore/DebugInclude.h"

namespace MobSystem
{
	CMobCompField::CMobCompField (
		GLActor* pOwner )
		: ICrowCompField( pOwner )
	{
		m_pServerOwner = static_cast< ServerActor* >( GetOwner() );
	}

	CMobCompField::~CMobCompField () { }

	const ComponentSystem::EMCOMP_ID CMobCompField::GetComponentID () const
	{
		return ComponentSystem::EMCOMP_MOB;
	}

	const ComponentSystem::EMFAMILY_ID CMobCompField::GetFamilyID () const
	{
		return ComponentSystem::EMFAMILY_CROW;
	}

	void CMobCompField::Initialize ()
	{
	}

	void CMobCompField::Destroy ()
	{

	}

	void CMobCompField::Reset ()
	{
	}

	void CMobCompField::BeginUpdate ()
	{

	}

	void CMobCompField::Update ( float fElapsedTime )
	{

	}

	void CMobCompField::EndUpdate ()
	{

	}

	void CMobCompField::SetCrowData ( const SCROWDATA& sCrowData )
	{
	}

	void CMobCompField::IdleProcess ( float fElapsedTime )
	{

	}

	void CMobCompField::MoveProcess ( float fElapsedTime )
	{
	}

	DWORD CMobCompField::GETCLIENTID ()
	{
		return 0;
	}

	WORD CMobCompField::GetSchool () const
	{
		return 0;
	}

	SCONFTING* CMobCompField::GETCONFTING ()
	{
		return NULL;
	}
}