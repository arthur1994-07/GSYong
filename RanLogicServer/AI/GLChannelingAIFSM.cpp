#include "pch.h"

#include "../ServerActor.h"

#include "./GLChannelingAIFSM.h"

#include "../SigmaCore/DebugInclude.h"

namespace AISystem
{

	GLChannelingAIFSM::GLChannelingAIFSM ()
		: GLScriptedFSM()
	{

	}

	GLChannelingAIFSM::~GLChannelingAIFSM ()
	{

	}

	void GLChannelingAIFSM::Begin (
		const std::string& strCurState,
		ServerActor* const pActor )
	{
		GLScriptedFSM::Begin( strCurState, pActor );

#ifdef	_USE_LUA_SCRIPT_FILE_
#else
#endif
	}

	void GLChannelingAIFSM::Update (
		const std::string& strCurState,
		ServerActor* const pActor,
		float fElapsedTime )
	{
		GLScriptedFSM::Update( strCurState, pActor, fElapsedTime );

		if ( !pActor )
			return;

#ifdef	_USE_LUA_SCRIPT_FILE_
#else

		pActor->AutoSearchTarget();

		pActor->StartTargetAction();

#endif
	}

	void GLChannelingAIFSM::End (
		const std::string& strCurState,
		ServerActor* const pActor )
	{
		GLScriptedFSM::End( strCurState, pActor );

#ifdef	_USE_LUA_SCRIPT_FILE_
#else
#endif
	}

	void GLChannelingAIFSM::Attack (
		const std::string& strCurState,
		ServerActor* const pActor )
	{
		GLScriptedFSM::Attack( strCurState, pActor );

#ifdef	_USE_LUA_SCRIPT_FILE_
#else
#endif
	}

	bool GLChannelingAIFSM::Attacked (
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

	void GLChannelingAIFSM::OwnerAttack (
		const std::string& strCurState,
		ServerActor* const pActor )
	{
		GLScriptedFSM::OwnerAttack( strCurState, pActor );

#ifdef	_USE_LUA_SCRIPT_FILE_
#else
#endif
	}

	bool GLChannelingAIFSM::OwnerAttacked (
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