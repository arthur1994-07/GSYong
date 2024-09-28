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
        //public funtion : Agent ���� ���� Invoke�Լ�;
        //////////////////////////////////////////////////////////////////////////////////////////////

        // isXXX;
        int isParty(lua_State* pState);
        int isExpedition(lua_State* pState);
        int isPartyMaster(lua_State* pState);

        // getXXX;
        int GetLevel(lua_State* pState); // ���� Level Ȯ�� / param1 : DWORD �ش� ����ID (DBNum or GaeaID), return : int Level;
        int GetName(lua_State* pState);  // ���� Name Ȯ�� / param1 : DWROD �ش� ĳ���� DBNum, return : char* Name;
        int GetSchool(lua_State* pState);  // ������ �Ҽ��п� Ȯ�� / param : DWORD �ش� ĳ���� DBNum, return : DWORD SchoolNum;
        int GetClubID(lua_State* pState);  // ������ Ŭ�� DBNum Ȯ�� / param : DWORD �ش� ĳ���� DBNum, return : DWORD ClubDBNum;
        int GetClubName(lua_State* pState);  // Ŭ�� �̸� Ȯ�� / param : DWORD �ش� ClubDBNum, return : char* ClubName;
        int GetClubMasterName(lua_State* pState); // Ŭ�� �������� �̸� Ȯ�� / param : DWORD �ش� ClubDBNum, return : char* ClubMasterName;
        int GetClubRank(lua_State* pState); // Ŭ�� ��ũ;

        int GetParty(lua_State* pState);
        int GetPartyID(lua_State* pState);
        int GetPartyMember(lua_State* pState); // return : ��Ƽ�� DbNum;
        int GetPartyMemberNum(lua_State* pState);
        int GetPartyMaster(lua_State* pState);  // return : ��Ƽ�� DbNum;

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
        int JoinParty(lua_State* pState);  // ��Ƽ ���� or ��Ƽ�� �߰�;
        int OutParty(lua_State* pState);  // ��Ƽ Ż��;
        int ClearParty(lua_State* pState);  // ��Ƽ �ػ�;

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

		// ��ʸ�Ʈ ��� ��ũ��Ʈ���� ����� �α� (DB�� ����Ǵ� �α�)
		int tournamentStartLog(lua_State* pState);
		//int tournamentMemberLog(lua_State* pState);
		//int tournamentRoundStartLog(lua_State* pState);
		int tournamentRoundEndLog(lua_State* pState);
		//int tournamentEndLog(lua_State* pState);
		//int tournamentSystemMailLog(lua_State* pState);

        //////////////////////////////////////////////////////////////////////////////////////////////
        //Instance funtion : Agent ���� �δ� Instance Script�� Invoke�Լ� (�δ��������� ��밡��);
        ///////////////////////////////////////////////////////////////////////////////////////////////
		int DoJoin(lua_State* pState);
		int DoOut(lua_State* pState);
    }
}
