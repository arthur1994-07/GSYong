#pragma once

#include "../../RanLogic/ComponentSystem/IComponentBase.h"

#include "../ServerActor.h"

namespace ComponentSystem
{

	class ICrowCompField : public IComponentBase
	{
	public:
		ICrowCompField ( GLActor* pOwner ) : IComponentBase( pOwner )
		{
			m_pServerOwner = static_cast< ServerActor* >( GetOwner() );
		}
		virtual ~ICrowCompField () { }

	public:
		virtual const ComponentSystem::EMCOMP_ID GetComponentID () const = 0;
		virtual const ComponentSystem::EMFAMILY_ID GetFamilyID () const = 0;

		virtual void Initialize () { }
		virtual void Destroy () { }
		virtual void Reset () { }

	protected:
		virtual void BeginUpdate () { }
		virtual void Update ( float fElapsedTime ) { }
		virtual void EndUpdate () { }

	public:
		virtual void SetCrowData ( const SCROWDATA& sCrowData ) = 0;

		virtual void IdleProcess ( float fElapsedTime ) = 0;
		virtual void MoveProcess ( float fElapsedTime ) = 0;

	public:
		virtual DWORD GETCLIENTID () = 0;
		virtual WORD GetSchool () const = 0;
		virtual SCONFTING* GETCONFTING () = 0;

	protected:
		ServerActor* m_pServerOwner;
	};
}