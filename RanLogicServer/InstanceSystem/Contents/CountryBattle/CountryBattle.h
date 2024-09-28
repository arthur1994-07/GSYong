#pragma once

#include "../../../../SigmaCore/Lua/MinLua.h"

namespace InstanceSystem
{
	class ScriptManager;
}

namespace InstanceSystem
{
	void registFunction_CTI_Instance ( sc::LuaInstance& _luaInstance );
	void registFunction_CTI_Instance ( InstanceSystem::ScriptManager* const _pScriptManager );
	void registFunction_CTI_Agent ( sc::LuaInstance& _luaInstance );
	void registFunction_CTI_Agent ( InstanceSystem::ScriptManager* const _pScriptManager );
	void registFunction_CTI_Field ( sc::LuaInstance& _luaInstance );
	void registFunction_CTI_Field ( InstanceSystem::ScriptManager* const _pScriptManager );

	// ������Ʈ ���� ���� �Լ�;
	int ResetOldCTICountryAgent ( lua_State* pState );
	int SetCTICountryAgent ( lua_State* pState );
	int GetCTIVCountryAgent ( lua_State* pState );

	// �ʵ� ���� ���� �Լ�;
	int ResetOldCTICountryField ( lua_State* pState );
	int SetCTICountryField ( lua_State* pState );
	int GetCTIVCountryField ( lua_State* pState );

	// �ʵ� ���� �ν��Ͻ��� �κ�ũ;
	int UI_CTI_Auth ( lua_State* pState );  // ������ ���� UI;
	int UI_CTI_Progress ( lua_State* pState );  // ������ ���� UI;
	int UI_CTI_Progress_All ( lua_State* pState );  // ������ ���� UI;
	int UI_CTI_Rank ( lua_State* pState );  // ������ ���� UI;
	int UI_CTI_Rank_MyInfo ( lua_State* pState );  // ������ ���� UI �� ���� ��Ŷ;
	int UI_CTI_MiniMap ( lua_State* pState );  // ������ ���� UI �� ���� ��Ŷ;
	int UI_CTI_MiniMap_All ( lua_State* pState );  // ������ ���� UI �� ���� ��Ŷ;

	int UI_CompetitionResult_CTI_Begin ( lua_State* pState );
	int UI_CompetitionResult_CTI_Info ( lua_State* pState );
	int UI_CompetitionResult_CTI_End ( lua_State* pState );

	// Log
	int Log_CTI_In ( lua_State* pState );
	int Log_CTI_Out (lua_State* pState );
	int Log_CTI_Result (lua_State* pState) ;
	// ���� : GLGuidanceAgentMan::ChangeGuidClub_RenewalGuidance �Լ����� Guidance �δ��α׸� �����;

}  