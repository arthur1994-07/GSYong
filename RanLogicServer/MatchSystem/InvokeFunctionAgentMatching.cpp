#include "pch.h"

#include "../FieldServer/GLGaeaServer.h"
#include "../PVE/PVESystem.h"
#include "./MatchingManager.h"

#include "./InvokeFunctionAgentMatching.h"

namespace MatchingSystem
{
	void registFunction_AgentMatching(sc::LuaInstance& _luaInstance)
	{
		_luaInstance.RegistFunction("IsJoinGameofALL", MatchingSystem::IsJoinGameofALL);
		_luaInstance.RegistFunction("IsJoinOtherContents", MatchingSystem::IsJoinOtherContents);
	}

	void registFunction_AgentMatching(InstanceSystem::ScriptManager* const _pScriptManager)
	{
		if ( _pScriptManager == NULL )
			return;

		_pScriptManager->registFunctionCommon( registFunction_AgentMatching );
	}





	int IsJoinGameofALL(lua_State* pState)
	{
		int currentStack(1);

		const DWORD _dwCharDbNum(lua_tointeger(pState, currentStack++));

		const bool _bResult(MatchingCore::getinstance()->IsAreadyJoin(_dwCharDbNum));

		lua_pushboolean(pState, _bResult);
		return 1;
	}

	int IsJoinOtherContents(lua_State* pState)
	{
		int currentStack(1);

		const DWORD _dwCharDbNum(lua_tointeger(pState, currentStack++));

		GLCharAG* pCharAg =  gpAgentServer->GetCharByDbNum(_dwCharDbNum);
		if ( pCharAg == NULL )
		{
			return 0;
		}
		
		// PVE�δ��� �������ΰ�?;
		if ( PVESystem::FindInstantDungeon(pCharAg->ChaDbNum()) )
		{
			lua_pushboolean(pState, true);
			return 1;
		}

		// CTF�� ��û���̰ų� �÷������ΰ�?;
		if ( gpAgentServer->IsRegisterCTFMember(_dwCharDbNum)  || pCharAg->IsCtfPlayer() || pCharAg->IsInInstance() )
		{
			lua_pushboolean(pState, true);
			return 1;
		}

		lua_pushboolean(pState, false);
		return 1;
	}
}
