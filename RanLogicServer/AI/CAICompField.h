#pragma once

#include "../../RanLogic/ComponentSystem/IComponentBase.h"

#include "../ServerActor.h"

namespace AISystem
{

	// FSM을 이용한 AI이다;
	// Script를 사용하도록 종속되어있다;
	class CAICompField : public ComponentSystem::IComponentBase
	{
	public:
		CAICompField ( ServerActor* pMe );
		virtual ~CAICompField ();

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
		// 상태 변환;
		void ChangeState ( const std::string& strChangeTableName );

	public:
		void Attack ();
		void Attacked ( ServerActor* const pAttacker );
		void OwnerAttack ();
		void OwnerAttacked ( ServerActor* const pAttacker );

	public:
		const DWORD GetTypeMaxCount () const;

	public:
		inline const DWORD GetType () const { return m_dwType; }
		inline void SetAIType ( DWORD dwType ) { m_dwType = dwType; }

	protected:
		// AI Type;
		DWORD m_dwType;

		// 현재의 상태 TableName;
		std::string m_strCurTableName;

		// AI를 가지는 Actor정보;
		ServerActor* m_pMe;
	};

}