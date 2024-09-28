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

    // 에이전트 서버 공용 함수;
    int DisableOldGuidance(lua_State* pState);
    int SetGuidanceClub(lua_State* pState);

    // 필드 서버 공용 함수;
    // -- 없음;

    // 필드 서버 인스턴스용 인보크;
    int UI_Guidance_Auth(lua_State* pState);  // 선도전 인증 UI;
    int UI_Guidance_Progress(lua_State* pState);  // 선도전 진행 UI;
    int UI_Guidance_Progress_All(lua_State* pState);  // 선도전 진행 UI;
    int UI_Guidance_Rank(lua_State* pState);  // 선도전 순위 UI;
    int UI_Guidance_Rank_MyInfo(lua_State* pState);  // 선도전 순위 UI 내 정보 패킷;
    int UI_Guidance_MiniMap(lua_State* pState);  // 선도전 순위 UI 내 정보 패킷;
    int UI_Guidance_MiniMap_All(lua_State* pState);  // 선도전 순위 UI 내 정보 패킷;

    int UI_CompetitionResult_Guidance_Begin(lua_State* pState);
    int UI_CompetitionResult_Guidance_Info(lua_State* pState);
    int UI_CompetitionResult_Guidance_End(lua_State* pState);

    // Log
    int Log_Guidance_In(lua_State* pState);
    int Log_Guidance_Out(lua_State* pState);
    int Log_Guidance_Result(lua_State* pState);
    // 참고 : GLGuidanceAgentMan::ChangeGuidClub_RenewalGuidance 함수에서 Guidance 인던로그를 사용중;

}  