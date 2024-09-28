#pragma once

#include "../../SigmaCore/Lua/MinLua.h"

//#	define _USE_LUA_SCRIPT_FILE_

class ServerActor;

namespace AISystem
{
	namespace LuaFSMString
	{
		static const char* g_strFunc_StateBegin = "Begin";
		static const char* g_strFunc_StateUpdate = "Update";
		static const char* g_strFunc_StateEnd = "End";

		static const char* g_strFunc_StateAttack = "Attack";
		static const char* g_strFunc_StateAttacked = "Attacked";
		static const char* g_strFunc_StateOwnerAttack = "OwnerAttack";
		static const char* g_strFunc_StateOwnerAttacked = "OwnerAttacked";

		static const char* g_strTable_Default = "State_Normal";
		static const char* g_strTable_Move = "State_Move";
		static const char* g_strTable_Attack = "State_Attack";

		static const char* g_strGlobal_MaxCount = "Type_Max_Count";
		static const char* g_strGlobal_Distance = "Type_Distance";
	}

	namespace FSMAIIndex
	{
		enum FSM_AI
		{
			FSM_AI_BASIC = 0,
			FSM_AI_CHANNELING,
			FSM_AI_DELAYED_INVOKE,
			FSM_AI_TRAP
		};
	}

	class GLScriptedFSM : public sc::IScriptBase
	{
	public:
		GLScriptedFSM ();
		virtual ~GLScriptedFSM ();

		virtual bool Init ( const char* szFile, int nIndex );
		virtual bool Reload ();

		virtual void Destroy ();

	public:
		bool IsValid ();

		void RegisterScriptedFSMCallback ();

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

	public:
		inline const DWORD GetTypeMaxCount () const { return m_dwTypeMaxCount; }

	protected:
		std::string m_strFileName;		// Script FileName;
		int m_nIndex;					// AI Index;

		bool m_bValid;

		bool m_bReload;

		// Type 관련 전역 데이터;
		DWORD m_dwTypeMaxCount;			// 최대 소환 개수;
	};

}