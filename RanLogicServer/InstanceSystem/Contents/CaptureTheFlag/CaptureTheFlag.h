#pragma once

#include "../../../../SigmaCore/Lua/MinLua.h"
#include <map>

namespace InstanceSystem
{
    class ScriptManager;
}

namespace InstanceSystem
{
	class CaptureTheFlagMan
	{
		static CaptureTheFlagMan& getInstance()
		{
			static CaptureTheFlagMan Instance;
			return Instance;
		}
	public:
		std::map<DWORD, DWORD> RatingMap;
	};

    void registFunction_CaptureTheFlag(sc::LuaInstance& _luaInstance);
    void registFunction_CaptureTheFlag(InstanceSystem::ScriptManager* const _pScriptManager);
    void registFunction_CaptureTheFlag_Instance(sc::LuaInstance& _luaInstance);
    void registFunction_CaptureTheFlag_Instance(InstanceSystem::ScriptManager* const _pScriptManager);
    void registFunction_CaptureTheFlag_Agent(sc::LuaInstance& _luaInstance);
    void registFunction_CaptureTheFlag_Agent(InstanceSystem::ScriptManager* const _pScriptManager);

	int UI_Navagation_On ( lua_State* pState );
	int UI_Navagation_Off ( lua_State* pState );

    int UI_BlueFlagGuideArrow_Target(lua_State* pState);
    int UI_RedFlagGuideArrow_Target(lua_State* pState);

    int UI_BlueFlagGuideArrow_Off(lua_State* pState);
    int UI_RedFlagGuideArrow_Off(lua_State* pState);

    int UI_FlagGuideArrow_Off(lua_State* pState);
    int UI_FlagGuideArrow_Off_User(lua_State* pState);

	int UI_CompetitionResult_CaptureTheFlag_Begin(lua_State* pState);
	int UI_CompetitionResult_CaptureTheFlag_Info(lua_State* pState);
	int UI_CompetitionResult_CaptureTheFlag_End(lua_State* pState);

    int Log_CaptureTheFlag_In(lua_State* pState);
    int Log_CaptureTheFlag_Out(lua_State* pState);
    int Log_CaptureTheFlag_Result(lua_State* pState);
    int Log_CaptureTheFlag_Reward(lua_State* pState);
    int Log_CaptureTheFlag_Rating_Agent(lua_State* pState);
}
