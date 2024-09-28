#include "pch.h"

#include "../ServerActor.h"

#include "./GLDelayedInvokeAIFSM.h"

#include "../SigmaCore/DebugInclude.h"

namespace AISystem
{

	GLDelayedInvokeAIFSM::GLDelayedInvokeAIFSM ()
		: GLScriptedFSM()
	{

	}

	GLDelayedInvokeAIFSM::~GLDelayedInvokeAIFSM ()
	{

	}

	void GLDelayedInvokeAIFSM::Begin (
		const std::string& strCurState,
		ServerActor* const pActor )
	{
		GLScriptedFSM::Begin( strCurState, pActor );

		if ( !pActor )
			return;

#ifdef	_USE_LUA_SCRIPT_FILE_
#else

		pActor->ApplyDelay( 3 );

#endif
	}

	void GLDelayedInvokeAIFSM::Update (
		const std::string& strCurState,
		ServerActor* const pActor,
		float fElapsedTime )
	{
		GLScriptedFSM::Update( strCurState, pActor, fElapsedTime );

		if ( !pActor )
			return;

#ifdef	_USE_LUA_SCRIPT_FILE_
#else

		if ( !pActor->GetDelayState() )
		{
			pActor->SyncPositionTarget();
			pActor->AutoSearchTarget();

			if ( !pActor->IsTarget() )
				pActor->DieAction();

			pActor->StartTargetAction( 1 );
		}
		else
		{
			pActor->SyncPositionTarget();

			if ( !pActor->IsTarget() )
				pActor->DieAction();
		}

#endif
	}

	void GLDelayedInvokeAIFSM::End (
		const std::string& strCurState,
		ServerActor* const pActor )
	{
		GLScriptedFSM::End( strCurState, pActor );

#ifdef	_USE_LUA_SCRIPT_FILE_
#else
#endif
	}

	void GLDelayedInvokeAIFSM::Attack (
		const std::string& strCurState,
		ServerActor* const pActor )
	{
		GLScriptedFSM::Attack( strCurState, pActor );

#ifdef	_USE_LUA_SCRIPT_FILE_
#else
#endif
	}

	bool GLDelayedInvokeAIFSM::Attacked (
		const std::string& strCurState,
		ServerActor* const pActor,
		ServerActor* const pAttacker )
	{
		if ( !GLScriptedFSM::Attacked(
			strCurState,
			pActor,
			pAttacker ) )
			return true;

		if ( !pActor || !pAttacker )
			return true;

#ifdef	_USE_LUA_SCRIPT_FILE_

		if ( !pActor->IsTargetExcludeOwner() &&
			!pActor->IsRunAway() )
		{
			STARGETID sTargetID;
			sTargetID.emCrow = pAttacker->GetCrow();
			sTargetID.GaeaId = pAttacker->GetGaeaID();
			pActor->SetTargetID( sTargetID );
		}

#else

		return true;

#endif
	}

	void GLDelayedInvokeAIFSM::OwnerAttack (
		const std::string& strCurState,
		ServerActor* const pActor )
	{
		GLScriptedFSM::OwnerAttack( strCurState, pActor );

#ifdef	_USE_LUA_SCRIPT_FILE_
#else
#endif
	}

	bool GLDelayedInvokeAIFSM::OwnerAttacked (
		const std::string& strCurState,
		ServerActor* const pActor,
		ServerActor* const pAttacker )
	{
		if ( !GLScriptedFSM::OwnerAttacked(
			strCurState,
			pActor,
			pAttacker ) )
			return true;

		if ( !pActor || !pAttacker )
			return true;

#ifdef	_USE_LUA_SCRIPT_FILE_

		if ( !pActor->IsTargetExcludeOwner() &&
			!pActor->IsRunAway() )
		{
			STARGETID sTargetID;
			sTargetID.emCrow = pAttacker->GetCrow();
			sTargetID.GaeaId = pAttacker->GetGaeaID();
			pActor->SetTargetID( sTargetID );
		}

#else

		return true;

#endif
	}

}