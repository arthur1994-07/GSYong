#pragma once

#include "../../SigmaCore/Lua/MinLua.h"

namespace InstanceSystem
{
    void initializeAgentPublicInvokeFunction(void);
    void initializeAgentInstanceInvokeFunction(sc::LuaInstance& _luaInstance);

    void registFunctionAgent(InstanceSystem::ScriptManager* const _pScriptManager);        
    void registFunctionAgent(sc::LuaInstance& _luaInstance);

    void registFunctionAgentInstance(InstanceSystem::ScriptManager* const _pScriptManager);        
    void registFunctionAgentInstance(sc::LuaInstance& _luaInstance);

    namespace AgentScriptFunction
    {
        //////////////////////////////////////////////////////////////////////////////////////////////
        //public funtion : Agent 서버 공용 Invoke함수;
        //////////////////////////////////////////////////////////////////////////////////////////////

        // isXXX;
        int isParty(lua_State* pState);
        int isExpedition(lua_State* pState);
        int isPartyMaster(lua_State* pState);

        // getXXX;
        int GetLevel(lua_State* pState); // 액터 Level 확인 / param1 : DWORD 해당 액터ID (DBNum or GaeaID), return : int Level;
        int GetName(lua_State* pState);  // 유저 Name 확인 / param1 : DWROD 해당 캐릭터 DBNum, return : char* Name;
        int GetSchool(lua_State* pState);  // 유저의 소속학원 확인 / param : DWORD 해당 캐릭터 DBNum, return : DWORD SchoolNum;
        int GetClubID(lua_State* pState);  // 유저의 클럽 DBNum 확인 / param : DWORD 해당 캐릭터 DBNum, return : DWORD ClubDBNum;
        int GetClubName(lua_State* pState);  // 클럽 이름 확인 / param : DWORD 해당 ClubDBNum, return : char* ClubName;
        int GetClubMasterName(lua_State* pState); // 클럽 마스터의 이름 확인 / param : DWORD 해당 ClubDBNum, return : char* ClubMasterName;
        int GetClubRank(lua_State* pState); // 클럽 랭크;

        int GetParty(lua_State* pState);
        int GetPartyID(lua_State* pState);
        int GetPartyMember(lua_State* pState); // return : 파티원 DbNum;
        int GetPartyMemberNum(lua_State* pState);
        int GetPartyMaster(lua_State* pState);  // return : 파티장 DbNum;

        int GetBaseMapID(lua_State* pState);

		int GetPosFromMapPos(lua_State* pState);
		int GetMapPosFromPos(lua_State* pState);
		int GetCountry(lua_State* pState);
		int GetCountryName(lua_State* pState);

        // setXXX;
        int setPublicJoinLock(lua_State* pState);

        // doXXX;
        int doCreateInstance(lua_State* pState);
        int doDestroyInstance(lua_State* pState);

        // party;
        int JoinParty(lua_State* pState);  // 파티 생성 or 파티원 추가;
        int OutParty(lua_State* pState);  // 파티 탈퇴;
        int ClearParty(lua_State* pState);  // 파티 해산;

        // SendXXX;
        int SendMessage(lua_State* pState);

        // registXXX;
        int RegistTimer(lua_State* pState);
        int RegistAbsTimer(lua_State* pState);
        int IsRegistTimer(lua_State* pState);
        int RemoveTimer(lua_State* pState);
        

        // UI;
        int PrintChatMsg(lua_State* pState);
        int PrintChatXmlMsg(lua_State* pState);
        int PrintChatMsg_Broad(lua_State* pState);
        int PrintChatXmlMsg_Broad(lua_State* pState);

        int PrintSimpleMsg(lua_State* pState);
        int PrintSimpleXmlMsg(lua_State* pState);
        int PrintSimpleMsg_Broad(lua_State* pState);
        int PrintSimpleXmlMsg_Broad(lua_State* pState);

        int UI_TimerMessageBox(lua_State* pState);
        int UI_TimerMessageBox_Broad(lua_State* pState);
        int UI_XmlTimerMessageBox(lua_State* pState);
        int UI_XmlTimerMessageBox_Broad(lua_State* pState);


		int SendSystemMail(lua_State* pState);

		// 토너먼트 루아 스크립트에서 남기는 로그 (DB에 저장되는 로그)
		int tournamentStartLog(lua_State* pState);
		//int tournamentMemberLog(lua_State* pState);
		//int tournamentRoundStartLog(lua_State* pState);
		int tournamentRoundEndLog(lua_State* pState);
		//int tournamentEndLog(lua_State* pState);
		//int tournamentSystemMailLog(lua_State* pState);

        //////////////////////////////////////////////////////////////////////////////////////////////
        //Instance funtion : Agent 서버 인던 Instance Script용 Invoke함수 (인던내에서만 사용가능);
        ///////////////////////////////////////////////////////////////////////////////////////////////
		int DoJoin(lua_State* pState);
		int DoOut(lua_State* pState);
    }
}
