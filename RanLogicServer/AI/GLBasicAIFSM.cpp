#include "pch.h"

#include "../ServerActor.h"

#include "./GLBasicAIFSM.h"

#include "../SigmaCore/DebugInclude.h"

namespace AISystem
{

	GLBasicAIFSM::GLBasicAIFSM ()
		: GLScriptedFSM()
	{

	}

	GLBasicAIFSM::~GLBasicAIFSM ()
	{

	}

	void GLBasicAIFSM::Begin (
		const std::string& strCurState,
		ServerActor* const pActor )
	{
		GLScriptedFSM::Begin( strCurState, pActor );

		if ( !pActor )
			return;

#ifdef	_USE_LUA_SCRIPT_FILE_
#else

		if ( strCurState == LuaFSMString::g_strTable_Default )
		{

		}
		else if ( strCurState == LuaFSMString::g_strTable_Move )
		{

		}
		else if ( strCurState == LuaFSMString::g_strTable_Attack )
		{

		}
		else
		{
			sc::writeLogError( "Invalid strCurState String - AI System!!" );
		}

#endif
	}

	void GLBasicAIFSM::Update (
		const std::string& strCurState,
		ServerActor* const pActor,
		float fElapsedTime )
	{
		GLScriptedFSM::Update( strCurState, pActor, fElapsedTime );

		if ( !pActor )
			return;

#ifdef	_USE_LUA_SCRIPT_FILE_
#else

		if ( strCurState == LuaFSMString::g_strTable_Default )
		{
			ChangeState( pActor, pActor->GetOwner() );

			if ( !pActor->IsValidOwnerDistance() )
				pActor->RunAwayAction();

			pActor->StartTargetAction();

			pActor->TracingOwner();
		}
		else if ( strCurState == LuaFSMString::g_strTable_Move )
		{
			ChangeState( pActor, pActor->GetOwner() );

			if ( !pActor->IsValidOwnerDistance() )
				pActor->RunAwayAction();

			pActor->StartTargetAction();

			pActor->TracingOwner();
		}
		else if ( strCurState == LuaFSMString::g_strTable_Attack )
		{
			ChangeState( pActor, pActor->GetOwner() );

			if ( !pActor->IsValidOwnerDistance() )
				pActor->RunAwayAction();

			pActor->StartTargetAction();

			pActor->TracingOwner();
		}
		else
		{
			sc::writeLogError( "Invalid strCurState String - AI System!!" );
		}

#endif
	}

	void GLBasicAIFSM::End (
		const std::string& strCurState,
		ServerActor* const pActor )
	{
		GLScriptedFSM::End( strCurState, pActor );

		if ( !pActor )
			return;

#ifdef	_USE_LUA_SCRIPT_FILE_
#else

		if ( strCurState == LuaFSMString::g_strTable_Default )
		{

		}
		else if ( strCurState == LuaFSMString::g_strTable_Move )
		{

		}
		else if ( strCurState == LuaFSMString::g_strTable_Attack )
		{

		}
		else
		{
			sc::writeLogError( "Invalid strCurState String - AI System!!" );
		}

#endif
	}

	void GLBasicAIFSM::Attack (
		const std::string& strCurState,
		ServerActor* const pActor )
	{
		GLScriptedFSM::Attack( strCurState, pActor );

		if ( !pActor )
			return;

#ifdef	_USE_LUA_SCRIPT_FILE_
#else

		if ( strCurState == LuaFSMString::g_strTable_Default )
		{

		}
		else if ( strCurState == LuaFSMString::g_strTable_Move )
		{

		}
		else if ( strCurState == LuaFSMString::g_strTable_Attack )
		{

		}
		else
		{
			sc::writeLogError( "Invalid strCurState String - AI System!!" );
		}

#endif
	}

	bool GLBasicAIFSM::Attacked (
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

		if ( strCurState == LuaFSMString::g_strTable_Default )
		{
			if ( !pActor->IsTargetExcludeOwner() &&
				!pActor->IsRunAway() )
			{
				STARGETID sTargetID;
				sTargetID.emCrow = pAttacker->GetCrow();
				sTargetID.GaeaId = pAttacker->GetGaeaID();
				pActor->SetTargetID( sTargetID );
			}
		}
		else if ( strCurState == LuaFSMString::g_strTable_Move )
		{
			if ( !pActor->IsTargetExcludeOwner() &&
				!pActor->IsRunAway() )
			{
				STARGETID sTargetID;
				sTargetID.emCrow = pAttacker->GetCrow();
				sTargetID.GaeaId = pAttacker->GetGaeaID();
				pActor->SetTargetID( sTargetID );
			}
		}
		else if ( strCurState == LuaFSMString::g_strTable_Attack )
		{

		}
		else
		{
			sc::writeLogError( "Invalid strCurState String - AI System!!" );
		}

		return true;

#endif
	}

	void GLBasicAIFSM::OwnerAttack (
		const std::string& strCurState,
		ServerActor* const pActor )
	{
		GLScriptedFSM::OwnerAttack( strCurState, pActor );

		if ( !pActor )
			return;

#ifdef	_USE_LUA_SCRIPT_FILE_
#else

		if ( strCurState == LuaFSMString::g_strTable_Default )
		{
			if ( !pActor->IsTargetExcludeOwner() )
				pActor->SetOwnerTarget();
		}
		else if ( strCurState == LuaFSMString::g_strTable_Move )
		{
			if ( !pActor->IsTargetExcludeOwner() )
				pActor->SetOwnerTarget();
		}
		else if ( strCurState == LuaFSMString::g_strTable_Attack )
		{
			if ( !pActor->IsTargetExcludeOwner() )
				pActor->SetOwnerTarget();
		}
		else
		{
			sc::writeLogError( "Invalid strCurState String - AI System!!" );
		}

#endif
	}

	bool GLBasicAIFSM::OwnerAttacked (
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

		if ( strCurState == LuaFSMString::g_strTable_Default )
		{
			if ( !pActor->IsTargetExcludeOwner() &&
				!pActor->IsRunAway() )
			{
				STARGETID sTargetID;
				sTargetID.emCrow = pAttacker->GetCrow();
				sTargetID.GaeaId = pAttacker->GetGaeaID();
				pActor->SetTargetID( sTargetID );
			}
		}
		else if ( strCurState == LuaFSMString::g_strTable_Move )
		{
			if ( !pActor->IsTargetExcludeOwner() &&
				!pActor->IsRunAway() )
			{
				STARGETID sTargetID;
				sTargetID.emCrow = pAttacker->GetCrow();
				sTargetID.GaeaId = pAttacker->GetGaeaID();
				pActor->SetTargetID( sTargetID );
			}
		}
		else if ( strCurState == LuaFSMString::g_strTable_Attack )
		{

		}
		else
		{
			sc::writeLogError( "Invalid strCurState String - AI System!!" );
		}

		return true;

#endif
	}

	void GLBasicAIFSM::ChangeState (
		ServerActor* const pActor,
		ServerActor* const pOwner )
	{
		if ( !pActor || !pOwner )
			return;

		if ( pOwner->GetAction() == pActor->GetAction() )
			return;

		if ( pOwner->GetAction() == GLAT_IDLE )
			pActor->ChangeAIState( LuaFSMString::g_strTable_Default );
		else if ( pOwner->GetAction() == GLAT_MOVE )
			pActor->ChangeAIState( LuaFSMString::g_strTable_Move );
		else if ( pOwner->GetAction() == GLAT_SKILL || pOwner->GetAction() == GLAT_ATTACK )
			pActor->ChangeAIState( LuaFSMString::g_strTable_Attack );
	}

}