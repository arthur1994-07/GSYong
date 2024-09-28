#ifndef _RN_MATCHING_MATCH_H_
#define _RN_MATCHING_MATCH_H_

#include "../../SigmaCore/Lua/MinLua.h"

namespace InstanceSystem
{


	void registFunction_FieldMatching(sc::LuaInstance& _luaInstance);
	void registFunction_FieldMatching(InstanceSystem::ScriptManager* const _pScriptManager);

	int Matching_Resualt_Send_Start(lua_State* pState);				// 메칭 정보 전송 시작알림.
	int MATCHING_RESULT_SCORE(lua_State* pState);					// Matching 스코어
	int Matching_Faction_Resualt(lua_State* pState);				// Matching 팀 결과
    int Matching_Result_CustomMSG(lua_State* pState);				// Matching 팀 결과
	int Matching_Resualt_Send_End(lua_State* pState);				// 메칭 정보 전송 완료.

	int Matching_Error_Send(lua_State* pState);
	int Matching_Tournament_OUT(lua_State* pState);

	
	int Matching_Tournament_OUT_ALL(lua_State* pState);

	int Matching_Tournament_GamInfo_Role(lua_State* pState);
	int SendGameTeamInfo(lua_State* pState);
	int Matching_Tournament_GamInfo_Win(lua_State* pState);
	int Matching_Tournament_GamInfo_Lose(lua_State* pState);
	int Matching_Tournament_FightCounting(lua_State* pState);

	int Matching_Tournament_GameTimeLimit(lua_State* pState);

	int Matching_GetLandSize(lua_State* pState);

	int SetMatchingUserOn(lua_State* pState);
	//int SetSendFactionInfo(lua_State* pState);

	int SendClientsInstancePopUp(lua_State* pState);

	int SetFcationUI(lua_State* pState);

    int UI_Indicator(lua_State* pState);
    int UI_Indicator_All(lua_State* pState);

	int UI_CompetitionResult_Tour_Begin(lua_State* pState);
	int UI_CompetitionResult_Tour_Info(lua_State* pState);
	int UI_CompetitionResult_Tour_End(lua_State* pState);

    int Report_CaptureTheFlag_Result(lua_State* pState);
}

#endif