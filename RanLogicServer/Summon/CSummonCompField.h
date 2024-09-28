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
		// 각 데이터 세팅;
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
		Faction::GLActorID m_sSummmonerID;	// 소환사 ID;
		GLSUMMON m_sSummonData;				// Summon Data;

		DWORD m_dwAtypeAfterTracing;		// 추적 후 사용할 AType;
		WORD m_wRangeTracing;				// 추적에 사용할 Range;

		DWORD m_dwReactCount;				// 행동 횟수;

		float m_fCurSpeed;					// 소환 Object의 속도;
	};

}