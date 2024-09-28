#pragma once

#include "../../../../SigmaCore/Lua/MinLua.h"

namespace InstanceSystem
{
    class ScriptManager;
}

namespace InstanceSystem
{
    class PreviousCDM_Info
    {
    public:
        __time64_t previous_StartTime;
        __time64_t previous_EndTime;
        std::vector<DWORD> vecPrevious_ClubDbNum;
        std::vector<DWORD> vecPrevious_Rank;
    };

    extern PreviousCDM_Info gPreviousCDM_Info;

    void registFunction_CDM(sc::LuaInstance& _luaInstance);
    void registFunction_CDM(InstanceSystem::ScriptManager* const _pScriptManager);
    void registFunction_CDM_Instance(sc::LuaInstance& _luaInstance);
    void registFunction_CDM_Instance(InstanceSystem::ScriptManager* const _pScriptManager);
    void registFunction_CDM_Agent(sc::LuaInstance& _luaInstance);
    void registFunction_CDM_Agent(InstanceSystem::ScriptManager* const _pScriptManager);

    int UI_CDM_Progress(lua_State* pState);  // CDM 진행상황 UI;
    int UI_CDM_ProgressBroad(lua_State* pState);  // CDM 진행상황 UI (인던내 모든 플레이어에게 전송);

    int UI_CDM_ProgressOff(lua_State* pState);  // CDM 진행상황 UI 끔;
    int UI_CDM_ProgressOffBroad(lua_State* pState);  // CDM 진행상황 UI 끔 (인던내 모든 플레이어에게 전송);
    int UI_CDM_BattleStart(lua_State* pState); // CDM 전투종료 이미지 (인던내 모든 플레이어에게 전송);
    int UI_CDM_BattleEnd(lua_State* pState); // CDM 전투종료 이미지 (인던내 모든 플레이어에게 전송);

    int UI_CDM_DoublePoint(lua_State* pState); // 더블포인트 UI;
    int UI_CDM_DoublePointBroad(lua_State* pState); // 더블포인트 UI (인던내 모든 플레이어에게 전송);

    int HaveCDM_Authority(lua_State* pState); // 클럽 멤버가 CDM 참가 권한을 가진 멤버인가?;
    int HaveCDM_Authority_Agent(lua_State* pState); // 클럽 멤버가 CDM 참가 권한을 가진 멤버인가?;

    int Save_CDM_History(lua_State* pState);  // 이전CDM 결과 기록 (Field -> Agent);
    int Send_CDM_History_Agent(lua_State* pState);  // 이전CDM 결과 전송 (Agent -> Client);

    int UI_CompetitionResult_CDM_Begin(lua_State* pState);
    int UI_CompetitionResult_CDM_Info(lua_State* pState);
    int UI_CompetitionResult_CDM_End(lua_State* pState);


    // Log
    int Log_CDM_In(lua_State* pState);
    int Log_CDM_Out(lua_State* pState);
    int Log_CDM_Result(lua_State* pState);

    //int UI_CDM_Result(lua_State* pState); // CDM 결과 창 (인던내 모든 플레이어에게 전송);
    //int UI_CDM_ResultShow(lua_State* pState); // CDM 결과 창 (인던내 모든 플레이어에게 전송);
}