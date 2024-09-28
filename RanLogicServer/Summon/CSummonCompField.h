#pragma once

#include "../Crow/ICrowCompField.h"

#include "../ServerActor.h"

namespace SummonSystem
{

	class CSummonCompField : public ComponentSystem::ICrowCompField
	{
	public:
		CSummonCompField ( GLActor* pOwner );
		virtual ~CSummonCompField ();

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
		// �� ������ ����;
		void SetSummonData ( const GLSUMMON& sSummonData );
		void SetSummoner ( const Faction::GLActorID& sSummonID );

		virtual void SetCrowData ( const SCROWDATA& sCrowData );

		virtual void IdleProcess ( float fElapsedTime );
		virtual void MoveProcess ( float fElapsedTime );

	public:
		virtual DWORD GETCLIENTID ();
		virtual WORD GetSchool () const;
		virtual SCONFTING* GETCONFTING ();

	public:
		inline const Faction::GLActorID& GetSummonerID() const { return m_sSummmonerID; }

	private:
		Faction::GLActorID m_sSummmonerID;	// ��ȯ�� ID;
		GLSUMMON m_sSummonData;				// Summon Data;

		DWORD m_dwAtypeAfterTracing;		// ���� �� ����� AType;
		WORD m_wRangeTracing;				// ������ ����� Range;

		DWORD m_dwReactCount;				// �ൿ Ƚ��;

		float m_fCurSpeed;					// ��ȯ Object�� �ӵ�;
	};

}