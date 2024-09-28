#pragma once

#include "../../SigmaCore/Lua/MinLua.h"


namespace MatchingSystem
{
	void registFunction_AgentMatching(sc::LuaInstance& _luaInstance);
	void registFunction_AgentMatching(InstanceSystem::ScriptManager* const _pScriptManager);

	int IsJoinGameofALL(lua_State* pState);	// 게임이 진입을 한적이 있는지 물어봄 (모든 메칭에 한하여);
	int IsJoinOtherContents(lua_State* pState);	// 다른 컨텐츠를 진행중이거나 신청중인지 물어봄 ( CTF, PVE인던 );
}