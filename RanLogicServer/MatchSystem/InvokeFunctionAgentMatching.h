#pragma once

#include "../../SigmaCore/Lua/MinLua.h"


namespace MatchingSystem
{
	void registFunction_AgentMatching(sc::LuaInstance& _luaInstance);
	void registFunction_AgentMatching(InstanceSystem::ScriptManager* const _pScriptManager);

	int IsJoinGameofALL(lua_State* pState);	// ������ ������ ������ �ִ��� ��� (��� ��Ī�� ���Ͽ�);
	int IsJoinOtherContents(lua_State* pState);	// �ٸ� �������� �������̰ų� ��û������ ��� ( CTF, PVE�δ� );
}