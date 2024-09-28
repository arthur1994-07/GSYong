#pragma once

#include "../../../SigmaCore/Lua/MinLua.h"

namespace InstanceSystem
{
	class ScriptManager;

	void RegistInvokeFunction_UI_Instance ( sc::LuaInstance& _sLua );
	void RegistInvokeFunction_UI_Agent ( sc::LuaInstance& _sLua );
	void RegistInvokeFunction_UI_Agent ( InstanceSystem::ScriptManager* const _pScriptManager );
	void RegistInvokeFunction_UI_Field ( sc::LuaInstance& _sLua );
	void RegistInvokeFunction_UI_Field ( InstanceSystem::ScriptManager* const _pScriptManager );

	/************************************************************************/
	/* UI Instance - Invoke Function List                                   */
	/************************************************************************/

	// Show Authorization UI;
	int InvokeFunction_ShowAuthUI ( lua_State* pState );

	// Show Result UI;
	int InvokeFunction_ShowResultUI ( lua_State* pState );

	// Show Timer UI;
	int InvokeFunction_ShowTimeUI ( lua_State* pState );

	// Show Entrance State UI;
	int InvokeFunction_ShowEntranceStateUI ( lua_State* pState );

}  