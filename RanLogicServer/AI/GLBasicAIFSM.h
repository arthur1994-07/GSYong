#pragma once

#include "GLScriptedFSM.h"

class ServerActor;

namespace AISystem
{

	class GLBasicAIFSM : public GLScriptedFSM
	{
	public:
		GLBasicAIFSM ();
		virtual ~GLBasicAIFSM ();

	public:
		virtual void Begin (
			const std::string& strCurState,
			ServerActor* const pActor );

		virtual void Update (
			const std::string& strCurState,
			ServerActor* const pActor,
			float fElapsedTime );

		virtual void End (
			const std::string& strCurState,
			ServerActor* const pActor );

	public:
		virtual void Attack (
			const std::string& strCurState,
			ServerActor* const pActor );

		virtual bool Attacked (
			const std::string& strCurState,
			ServerActor* const pActor,
			ServerActor* const pAttacker );

		virtual void OwnerAttack (
			const std::string& strCurState,
			ServerActor* const pActor );

		virtual bool OwnerAttacked (
			const std::string& strCurState,
			ServerActor* const pActor,
			ServerActor* const pAttacker );

	private:
		void ChangeState (
			ServerActor* const pActor,
			ServerActor* const pOwner );
	};

}