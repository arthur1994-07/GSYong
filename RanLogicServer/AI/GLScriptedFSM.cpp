#include "pch.h"

#include "../ServerActor.h"

#include "./GLScriptedFSM.h"

#include "../SigmaCore/DebugInclude.h"

namespace AISystem
{

	GLScriptedFSM::GLScriptedFSM ()
		: m_bValid( false )
		, m_bReload( false )
		, m_dwTypeMaxCount( 1 )
		, m_nIndex( 0 )
	{

	}

	GLScriptedFSM::~GLScriptedFSM ()
	{

	}

	bool GLScriptedFSM::Init ( const char* szFile, int nIndex )
	{
		m_strFileName = szFile;
		m_nIndex = nIndex;

		if ( !DefaultInit( szFile ) )
			return false;

		RegisterScriptedFSMCallback();

		m_dwTypeMaxCount = GetGlobal< DWORD >( LuaFSMString::g_strGlobal_MaxCount );

		m_bValid = true;

		return true;
	}

	bool GLScriptedFSM::Reload ()
	{
		m_bReload = true;

		return true;
	}

	void GLScriptedFSM::Destroy ()
	{

	}

	bool GLScriptedFSM::IsValid ()
	{
		return m_bValid;
	}

	void GLScriptedFSM::RegisterScriptedFSMCallback ()
	{
		RegClass< ServerActor >( "Actor" );

		RegClassFunc< ServerActor >( "ApplyDelay", &ServerActor::ApplyDelay );
		RegClassFunc< ServerActor >( "GetDelayState", &ServerActor::GetDelayState );

		RegClassFunc< ServerActor >( "GetAction", &ServerActor::GetAction );
		RegClassFunc< ServerActor >( "GetOwner", &ServerActor::GetOwner );

		RegClassFunc< ServerActor >( "IsTarget", &ServerActor::IsTarget );
		RegClassFunc< ServerActor >( "IsTargetExcludeOwner", &ServerActor::IsTargetExcludeOwner );
		RegClassFunc< ServerActor >( "IsValidOwnerDistance", &ServerActor::IsValidOwnerDistance );
		RegClassFunc< ServerActor >( "AutoSearchTarget", &ServerActor::AutoSearchTarget );
		RegClassFunc< ServerActor >( "SetOwnerTarget", &ServerActor::SetOwnerTarget );

		RegClassFunc< ServerActor >( "StartTargetAction", &ServerActor::StartTargetAction );
		RegClassFunc< ServerActor >( "DieAction", &ServerActor::DieAction );
		RegClassFunc< ServerActor >( "UseEventSkill", &ServerActor::UseEventSkill );

		RegClassFunc< ServerActor >( "RunAwayAction", &ServerActor::RunAwayAction );
		RegClassFunc< ServerActor >( "NormalAction", &ServerActor::NormalAction );
		RegClassFunc< ServerActor >( "TracingOwner", &ServerActor::TracingOwner );
		RegClassFunc< ServerActor >( "TracingTarget", &ServerActor::TracingTarget );
		RegClassFunc< ServerActor >( "SyncPositionTarget", &ServerActor::SyncPositionTarget );

		RegClassFunc< ServerActor >( "LogInfo", &ServerActor::LogInfo );
		RegClassFunc< ServerActor >( "ChangeAIState", &ServerActor::ChangeAIState );
	}

	void GLScriptedFSM::Begin (
		const std::string& strCurState,
		ServerActor* const pActor )
	{
		try
		{
			if ( !pActor )
				return;

#ifdef	_USE_LUA_SCRIPT_FILE_

			CallLuaTableMember< void >(
				strCurState.c_str(),
				LuaFSMString::g_strFunc_StateBegin,
				pActor );

#endif
		}
		catch ( ... )
		{
			sc::writeLogError( "Failed Lua FSM 'Begin' Function Call!" );
		}
	}

	void GLScriptedFSM::Update (
		const std::string& strCurState,
		ServerActor* const pActor,
		float fElapsedTime )
	{
		try
		{
			if ( !pActor )
				return;

#ifdef	_USE_LUA_SCRIPT_FILE_

			CallLuaTableMember< void >(
				strCurState.c_str(),
				LuaFSMString::g_strFunc_StateUpdate,
				pActor,
				fElapsedTime );

			if ( m_bReload )
			{
				DefaultDestroy();

				Init ( m_strFileName.c_str() );

				m_bReload = false;
			}
#else

			if ( m_bReload )
				m_bReload = false;

#endif
		}
		catch ( ... )
		{
			sc::writeLogError( "Failed Lua FSM 'Update' Function Call!" );
		}
	}

	void GLScriptedFSM::End (
		const std::string& strCurState,
		ServerActor* const pActor )
	{
		try
		{
			if ( !pActor )
				return;

#ifdef	_USE_LUA_SCRIPT_FILE_

			CallLuaTableMember< void >(
				strCurState.c_str(),
				LuaFSMString::g_strFunc_StateEnd,
				pActor );

#endif
		}
		catch ( ... )
		{
			sc::writeLogError( "Failed Lua FSM 'End' Function Call!" );
		}
	}

	void GLScriptedFSM::Attack (
		const std::string& strCurState,
		ServerActor* const pActor )
	{
		try
		{
			if ( !pActor )
				return;

#ifdef	_USE_LUA_SCRIPT_FILE_

			CallLuaTableMember< void >(
				strCurState.c_str(),
				LuaFSMString::g_strFunc_StateAttack,
				pActor );

#endif
		}
		catch ( ... )
		{
			sc::writeLogError( "Failed Lua FSM 'Attack' Function Call!" );
		}
	}

	bool GLScriptedFSM::Attacked (
		const std::string& strCurState,
		ServerActor* const pActor,
		ServerActor* const pAttacker )
	{
		try
		{
			if ( !pActor || !pAttacker )
				return true;

#ifdef	_USE_LUA_SCRIPT_FILE_

			if ( !CallLuaTableMember< bool >(
				strCurState.c_str(),
				LuaFSMString::g_strFunc_StateAttacked,
				pActor,
				pAttacker ) )
				return false;

			return true;

#endif	
		}
		catch ( ... )
		{
			sc::writeLogError( "Failed Lua FSM 'Attacked' Function Call!" );
		}

		return true;
	}

	void GLScriptedFSM::OwnerAttack (
		const std::string& strCurState,
		ServerActor* const pActor )
	{
		try
		{
			if ( !pActor )
				return;

#ifdef	_USE_LUA_SCRIPT_FILE_

			CallLuaTableMember< void >(
				strCurState.c_str(),
				LuaFSMString::g_strFunc_StateOwnerAttack,
				pActor );

#endif
		}
		catch ( ... )
		{
			sc::writeLogError( "Failed Lua FSM 'Owner Attack' Function Call!" );
		}
	}

	bool GLScriptedFSM::OwnerAttacked (
		const std::string& strCurState,
		ServerActor* const pActor,
		ServerActor* const pAttacker )
	{
		try
		{
			if ( !pActor || !pAttacker )
				return true;

#ifdef	_USE_LUA_SCRIPT_FILE_

			if ( !CallLuaTableMember< bool >(
				strCurState.c_str(),
				LuaFSMString::g_strFunc_StateOwnerAttacked,
				pActor,
				pAttacker ) )
				return false;

			return true;

#endif
		}
		catch ( ... )
		{
			sc::writeLogError( "Failed Lua FSM 'Owner Attacked' Function Call!" );
		}

		return true;
	}

}