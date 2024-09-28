#pragma once

#include "../../../../SigmaCore/Lua/MinLua.h"

namespace InstanceSystem
{
    class ScriptManager;
}

namespace InstanceSystem
{
    void registFunction_Guidance(sc::LuaInstance& _luaInstance);
    void registFunction_Guidance(InstanceSystem::ScriptManager* const _pScriptManager);
    void registFunction_Guidance_Instance(sc::LuaInstance& _luaInstance);
    void registFunction_Guidance_Instance(InstanceSystem::ScriptManager* const _pScriptManager);
    void registFunction_Guidance_Agent(sc::LuaInstance& _luaInstance);
    void registFunction_Guidance_Agent(InstanceSystem::ScriptManager* const _pScriptManager);

    // ������Ʈ ���� ���� �Լ�;
    int DisableOldGuidance(lua_State* pState);
    int SetGuidanceClub(lua_State* pState);

    // �ʵ� ���� ���� �Լ�;
    // -- ����;

    // �ʵ� ���� �ν��Ͻ��� �κ�ũ;
    int UI_Guidance_Auth(lua_State* pState);  // ������ ���� UI;
    int UI_Guidance_Progress(lua_State* pState);  // ������ ���� UI;
    int UI_Guidance_Progress_All(lua_State* pState);  // ������ ���� UI;
    int UI_Guidance_Rank(lua_State* pState);  // ������ ���� UI;
    int UI_Guidance_Rank_MyInfo(lua_State* pState);  // ������ ���� UI �� ���� ��Ŷ;
    int UI_Guidance_MiniMap(lua_State* pState);  // ������ ���� UI �� ���� ��Ŷ;
    int UI_Guidance_MiniMap_All(lua_State* pState);  // ������ ���� UI �� ���� ��Ŷ;

    int UI_CompetitionResult_Guidance_Begin(lua_State* pState);
    int UI_CompetitionResult_Guidance_Info(lua_State* pState);
    int UI_CompetitionResult_Guidance_End(lua_State* pState);

    // Log
    int Log_Guidance_In(lua_State* pState);
    int Log_Guidance_Out(lua_State* pState);
    int Log_Guidance_Result(lua_State* pState);
    // ���� : GLGuidanceAgentMan::ChangeGuidClub_RenewalGuidance �Լ����� Guidance �δ��α׸� �����;

}  