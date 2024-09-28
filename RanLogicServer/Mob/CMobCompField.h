#pragma once

#include "../Crow/ICrowCompField.h"

#include "../ServerActor.h"

namespace MobSystem
{

	class CMobCompField : public ComponentSystem::ICrowCompField
	{
	public:
		CMobCompField ( GLActor* pOwner );
		virtual ~CMobCompField ();

	public:
		virtual const ComponentSystem::EMCOMP_ID GetComponentID () const;
		virtual const ComponentSystem::EMFAMILY_ID GetFamilyID () const;

		virtual void Initialize ();
		virtual void Destroy ();
		virtual void Reset ();

	protected:
		virtual void BeginUpdate ();
		virtual void Update ( float fElapsedTime );
		virtual void EndUpdate ();

	public:
		virtual void SetCrowData ( const SCROWDATA& sCrowData );

		virtual void IdleProcess ( float fElapsedTime );
		virtual void MoveProcess ( float fElapsedTime );	

	public:
		virtual DWORD GETCLIENTID ();
		virtual WORD GetSchool () const;
		virtual SCONFTING* GETCONFTING ();
	};

}