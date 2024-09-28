#pragma once

#include "../../../SigmaCore/Lua/MinLua.h"

namespace InstanceSystem
{
	class ScriptManager;

	void RegistInvokeFunction_DBLog_Instance ( sc::LuaInstance& _sLua );
	void RegistInvokeFunction_DBLog_Agent ( sc::LuaInstance& _sLua );
	void RegistInvokeFunction_DBLog_Agent ( InstanceSystem::ScriptManager* const _pScriptManager );
	void RegistInvokeFunction_DBLog_Field ( sc::LuaInstance& _sLua );
	void RegistInvokeFunction_DBLog_Field ( InstanceSystem::ScriptManager* const _pScriptManager );

	/************************************************************************/
	/* DBAction Instance Dungeon - Invoke Function List                                   */
	/************************************************************************/

	// Show Authorization UI;
	int InvokeFunction_DBActionEnterUser ( lua_State* _pState );
	int InvokeFunction_DBActionReenterUser ( lua_State* _pState );
	int InvokeFunction_DBActionOutUser ( lua_State* _pState );
	int InvokeFunction_DBActionNextStage ( lua_State* _pState );
	int InvokeFunction_DBActionRetry ( lua_State* _pState );

	// Show Result UI;
	int InvokeFunction_DBActionResultAccumulate ( lua_State* _pState );

	// Show Timer UI;
	int InvokeFunction_DBActionReward ( lua_State* _pState );

}  