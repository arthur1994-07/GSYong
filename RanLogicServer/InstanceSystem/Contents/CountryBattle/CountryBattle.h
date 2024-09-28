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

	// 에이전트 서버 공용 함수;
	int ResetOldCTICountryAgent ( lua_State* pState );
	int SetCTICountryAgent ( lua_State* pState );
	int GetCTIVCountryAgent ( lua_State* pState );

	// 필드 서버 공용 함수;
	int ResetOldCTICountryField ( lua_State* pState );
	int SetCTICountryField ( lua_State* pState );
	int GetCTIVCountryField ( lua_State* pState );

	// 필드 서버 인스턴스용 인보크;
	int UI_CTI_Auth ( lua_State* pState );  // 선도전 인증 UI;
	int UI_CTI_Progress ( lua_State* pState );  // 선도전 진행 UI;
	int UI_CTI_Progress_All ( lua_State* pState );  // 선도전 진행 UI;
	int UI_CTI_Rank ( lua_State* pState );  // 선도전 순위 UI;
	int UI_CTI_Rank_MyInfo ( lua_State* pState );  // 선도전 순위 UI 내 정보 패킷;
	int UI_CTI_MiniMap ( lua_State* pState );  // 선도전 순위 UI 내 정보 패킷;
	int UI_CTI_MiniMap_All ( lua_State* pState );  // 선도전 순위 UI 내 정보 패킷;

	int UI_CompetitionResult_CTI_Begin ( lua_State* pState );
	int UI_CompetitionResult_CTI_Info ( lua_State* pState );
	int UI_CompetitionResult_CTI_End ( lua_State* pState );

	// Log
	int Log_CTI_In ( lua_State* pState );
	int Log_CTI_Out (lua_State* pState );
	int Log_CTI_Result (lua_State* pState) ;
	// 참고 : GLGuidanceAgentMan::ChangeGuidClub_RenewalGuidance 함수에서 Guidance 인던로그를 사용중;

}  