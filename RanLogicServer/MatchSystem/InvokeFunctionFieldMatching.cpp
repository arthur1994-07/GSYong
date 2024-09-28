#include "pch.h"

#include "../FieldServer/GLGaeaServer.h"
#include "../Club/GLClubManField.h"
#include "../Club/GLClubField.h"
#include "../../RanLogic/Msg/GLContrlInstanceContentsMsg.h"
#include "../../RanLogic/Msg/GLContrlMatching.h"
#include "../../RanLogic/Land/FactionManager/FactionManagerDefine.h"
#include "../InstanceSystem/InstanceSystem.h"
#include "../InstanceSystem/LogicField/InstanceSystemField.h"
#include "InvokeFunctionFieldMatching.h"


#include "../../EngineLib/GUInterface/GameTextControl.h"

namespace InstanceSystem
{
	void registFunction_FieldMatching(sc::LuaInstance& _luaInstance)
	{

		_luaInstance.RegistFunction("Matching_Resualt_Send_Start", InstanceSystem::Matching_Resualt_Send_Start);
		_luaInstance.RegistFunction("MATCHING_RESULT_SCORE", InstanceSystem::MATCHING_RESULT_SCORE);
		_luaInstance.RegistFunction("Matching_Faction_Resualt", InstanceSystem::Matching_Faction_Resualt);
        _luaInstance.RegistFunction("Matching_Result_CustomMSG", InstanceSystem::Matching_Result_CustomMSG);
		_luaInstance.RegistFunction("Matching_Resualt_Send_End", InstanceSystem::Matching_Resualt_Send_End);

		_luaInstance.RegistFunction("Matching_Error_Send", InstanceSystem::Matching_Error_Send);

		_luaInstance.RegistFunction("Matching_Tournament_OUT", InstanceSystem::Matching_Tournament_OUT);
		_luaInstance.RegistFunction("SetMatchingUserOn", InstanceSystem::SetMatchingUserOn);
		_luaInstance.RegistFunction("Matching_Tournament_OUT_ALL", InstanceSystem::Matching_Tournament_OUT_ALL);
		_luaInstance.RegistFunction("Matching_Tournament_GamInfo_Role", InstanceSystem::Matching_Tournament_GamInfo_Role);
		_luaInstance.RegistFunction("SendGameTeamInfo", InstanceSystem::SendGameTeamInfo);

		_luaInstance.RegistFunction("Matching_Tournament_GamInfo_Win", InstanceSystem::Matching_Tournament_GamInfo_Win);
		_luaInstance.RegistFunction("Matching_Tournament_GamInfo_Lose", InstanceSystem::Matching_Tournament_GamInfo_Lose);
		_luaInstance.RegistFunction("Matching_Tournament_GameTimeLimit", InstanceSystem::Matching_Tournament_GameTimeLimit);
		_luaInstance.RegistFunction("ShowCountingUI", InstanceSystem::Matching_Tournament_FightCounting);

		_luaInstance.RegistFunction("Matching_GetLandSize", InstanceSystem::Matching_GetLandSize);
		_luaInstance.RegistFunction("SetFcationUI",InstanceSystem::SetFcationUI);
		_luaInstance.RegistFunction("SendClientsInstancePopUp",InstanceSystem::SendClientsInstancePopUp);

        _luaInstance.RegistFunction("UI_Indicator",InstanceSystem::UI_Indicator);
        _luaInstance.RegistFunction("UI_Indicator_All",InstanceSystem::UI_Indicator_All);

		_luaInstance.RegistFunction("UI_CompetitionResult_Tour_Begin",InstanceSystem::UI_CompetitionResult_Tour_Begin);
		_luaInstance.RegistFunction("UI_CompetitionResult_Tour_Info",InstanceSystem::UI_CompetitionResult_Tour_Info);
		_luaInstance.RegistFunction("UI_CompetitionResult_Tour_End",InstanceSystem::UI_CompetitionResult_Tour_End);
		//_luaInstance.registFunction("SetSendFactionInfo", InstanceSystem::SetSendFactionInfo);

        _luaInstance.RegistFunction("Report_CaptureTheFlag_Result", InstanceSystem::Report_CaptureTheFlag_Result);
		//
	}
	void registFunction_FieldMatching(InstanceSystem::ScriptManager* const _pScriptManager)
	{
		if ( _pScriptManager == NULL )
			return;

		_pScriptManager->registFunctionCommon( registFunction_FieldMatching );
	}

	int UI_CompetitionResult_Tour_Begin(lua_State* pState)
	{
		lua_getglobal(pState, "instanceMapID");
		const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));
		lua_pop(pState, 1);

		GLLandManager* const _pLandMan = gpGaeaServer->GetLandManager(_instanceMapID);
		if ( _pLandMan == NULL )
			return 0;

		InstanceSystem::RnCompetitionResultTitleObj _obj1;
		_obj1.emItemType = InstanceSystem::RnCompetitionResultTitleObj::EM_CONTENT_INT;
		_obj1.nBoxSize = 34;
		_obj1.strTitleName = ID2SERVERTEXT("RN_COMPETITION_RESULT_TITLE_TOUR",0); //"순위";

		InstanceSystem::RnCompetitionResultTitleObj _obj2;
		_obj2.emItemType = InstanceSystem::RnCompetitionResultTitleObj::EM_CONTENT_ICON_SCHOOL;
		_obj2.nBoxSize = 34;
		_obj2.strTitleName = ID2SERVERTEXT("RN_COMPETITION_RESULT_TITLE_TOUR",1); //"학원";

		InstanceSystem::RnCompetitionResultTitleObj _obj3;
		_obj3.emItemType = InstanceSystem::RnCompetitionResultTitleObj::EM_CONTENT_ICON_CLASS;
		_obj3.nBoxSize = 34;
		_obj3.strTitleName = ID2SERVERTEXT("RN_COMPETITION_RESULT_TITLE_TOUR",2); // "클레스";

		InstanceSystem::RnCompetitionResultTitleObj _obj4;
		_obj4.emItemType = InstanceSystem::RnCompetitionResultTitleObj::EM_CONTENT_INT;
		_obj4.nBoxSize = 34;
		_obj4.strTitleName = ID2SERVERTEXT("RN_COMPETITION_RESULT_TITLE_TOUR",3); // "레벨";

		InstanceSystem::RnCompetitionResultTitleObj _obj5;
		_obj5.emItemType = InstanceSystem::RnCompetitionResultTitleObj::EM_CONTENT_STRING;
		_obj5.nBoxSize = 114;
		_obj5.strTitleName = ID2SERVERTEXT("RN_COMPETITION_RESULT_TITLE_TOUR",4); // "캐릭터명";

		InstanceSystem::RnCompetitionResultTitleObj _obj6;
		_obj6.emItemType = InstanceSystem::RnCompetitionResultTitleObj::EM_CONTENT_INT;
		_obj6.nBoxSize = 34;
		_obj6.strTitleName = ID2SERVERTEXT("RN_COMPETITION_RESULT_TITLE_TOUR",5); // "승리수";

		InstanceSystem::RnCompetitionResultTitleObj _obj7;
		_obj7.emItemType = InstanceSystem::RnCompetitionResultTitleObj::EM_CONTENT_INT;
		_obj7.nBoxSize = 114;
		_obj7.strTitleName = ID2SERVERTEXT("RN_COMPETITION_RESULT_TITLE_TOUR",6); // "딜량";

		InstanceSystem::RnCompetitionResultTitleObj _obj8;
		_obj8.emItemType = InstanceSystem::RnCompetitionResultTitleObj::EM_CONTENT_INT;
		_obj8.nBoxSize = 114;
		_obj8.strTitleName = ID2SERVERTEXT("RN_COMPETITION_RESULT_TITLE_TOUR",7); // "치유량";

		InstanceSystem::RnCompetitionResultTitleObj _obj9;
		_obj9.emItemType = (InstanceSystem::RnCompetitionResultTitleObj::EM_CONTENT_REWARD) | (InstanceSystem::RnCompetitionResultTitleObj::EM_CONTENT_REWARD_ITEM);
		_obj9.nBoxSize = 50;
		_obj9.strTitleName = ID2SERVERTEXT("RN_COMPETITION_RESULT_TITLE_TOUR",8); // "보상";

		GLMSG::NET_INSTANCE_COMPETITION_RESULT_TAB_FC netMsg;

		netMsg.sTabInfo.strTabName = ID2SERVERTEXT("RN_COMPETITION_RESULT_TAB",0); // "전투 결과";
		netMsg.sTabInfo.emSortType = InstanceSystem::RnCompetitionResultTabInfo::EM_RESULT_SOR_ASC;
		netMsg.sTabInfo.wSortTitleNum = 0;
		netMsg.sTabInfo.emMyResultType = InstanceSystem::RnCompetitionResultTabInfo::EM_MY_RESULT_CHAR_DBNUM;

		netMsg.sTabInfo.sTitle.vecTitleObj.push_back(_obj1);
		netMsg.sTabInfo.sTitle.vecTitleObj.push_back(_obj2);
		netMsg.sTabInfo.sTitle.vecTitleObj.push_back(_obj3);
		netMsg.sTabInfo.sTitle.vecTitleObj.push_back(_obj4);
		netMsg.sTabInfo.sTitle.vecTitleObj.push_back(_obj5);
		netMsg.sTabInfo.sTitle.vecTitleObj.push_back(_obj6);
		netMsg.sTabInfo.sTitle.vecTitleObj.push_back(_obj7);
		netMsg.sTabInfo.sTitle.vecTitleObj.push_back(_obj8);
		netMsg.sTabInfo.sTitle.vecTitleObj.push_back(_obj9);

		msgpack::sbuffer buf;
		msgpack::pack(buf, netMsg);

		_pLandMan->sendMsgPC(NET_MSG_GCTRL_INSTANCE_CONTENTS_MSGPACK, buf);
		return 0;
	}

	int UI_CompetitionResult_Tour_Info(lua_State* pState)
	{
		lua_getglobal(pState, "instanceMapID");
		const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));
		lua_pop(pState, 1);

		GLLandManager* const _pLandMan = gpGaeaServer->GetLandManager(_instanceMapID);
		if ( _pLandMan == NULL )
			return 0;

		GLMSG::NET_INSTANCE_COMPETITION_RESULT_INFO_FC netMsg;
		netMsg.nTabNumber = 0;

		int currentStack(1);

		const int _keyID((DWORD)lua_tointeger(pState, currentStack++));  // 유저 DBID;
		netMsg.sContents.dwKeyID = _keyID;

		const int _data1((DWORD)lua_tointeger(pState, currentStack++));  // 순위;
		InstanceSystem::RnCompetitionResultContentObj _objData1;
		_objData1.ContentItemInt = _data1;

		const int _data2((DWORD)lua_tointeger(pState, currentStack++));  // 학원;
		InstanceSystem::RnCompetitionResultContentObj _objData2;
		_objData2.ContentItemInt = _data2;

		const int _data3((DWORD)lua_tointeger(pState, currentStack++));  // 클레스;
		InstanceSystem::RnCompetitionResultContentObj _objData3;
		_objData3.ContentItemInt = _data3;

		const int _data4((DWORD)lua_tointeger(pState, currentStack++));  // 레벨;
		InstanceSystem::RnCompetitionResultContentObj _objData4;
		_objData4.ContentItemInt = _data4;

		const char* _data5(sc::GetStringToLuaState(pState, currentStack++));  //캐릭터 명;
		InstanceSystem::RnCompetitionResultContentObj _objData5;
		_objData5.ContentItemString = _data5;

		const int _data6((DWORD)lua_tointeger(pState, currentStack++));  // 승리수;
		InstanceSystem::RnCompetitionResultContentObj _objData6;
		_objData6.ContentItemInt = _data6;

		const int _data7((DWORD)lua_tointeger(pState, currentStack++));  // 딜량;
		InstanceSystem::RnCompetitionResultContentObj _objData7;
		_objData7.ContentItemInt = _data7;

		const int _data8((DWORD)lua_tointeger(pState, currentStack++));  // 치유량;
		InstanceSystem::RnCompetitionResultContentObj _objData8;
		_objData8.ContentItemInt = _data8;

		//
		LuaPlus::LuaObject _data9(LuaPlus::LuaState::CastState(pState), currentStack++);
		InstanceSystem::RnCompetitionResultContentRewardObj _objData9;

		if ( _data9.IsTable() )
		{
			for ( LuaPlus::LuaTableIterator _ItemIterator(_data9); _ItemIterator; _ItemIterator.Next() )
			{
				LuaPlus::LuaObject _ItemIDData = _ItemIterator.GetValue();
				if ( _ItemIDData.IsTable() )
				{
					InstanceSystem::RnCompetitionResultContentRewardObj::RewardItem _rewardItem;

					LuaPlus::LuaTableIterator _IDiterator(_ItemIDData);
					if ( _IDiterator.IsValid() == false )
						continue;

					LuaPlus::LuaObject _MIDObj = _IDiterator.GetValue();
					if ( _MIDObj.IsInteger() )
						_rewardItem.itemID.wMainID = _MIDObj.GetInteger();
					_IDiterator.Next();

					if ( _IDiterator.IsValid() == false )
						continue;

					LuaPlus::LuaObject _SIDObj = _IDiterator.GetValue();
					if ( _SIDObj.IsInteger() )
						_rewardItem.itemID.wSubID = _SIDObj.GetInteger();

					_rewardItem.wAmount = 1;
					if ( _IDiterator.IsValid() )
					{
						LuaPlus::LuaObject _AmountObj = _IDiterator.GetValue();
						if ( _AmountObj.IsInteger() )
							_rewardItem.wAmount = _AmountObj.GetInteger();
					}

					_objData9.vecItemReward.push_back(_rewardItem);
				}
			}
		}

		netMsg.sContents.vecResultObj.push_back(_objData1);
		netMsg.sContents.vecResultObj.push_back(_objData2);
		netMsg.sContents.vecResultObj.push_back(_objData3);
		netMsg.sContents.vecResultObj.push_back(_objData4);
		netMsg.sContents.vecResultObj.push_back(_objData5);
		netMsg.sContents.vecResultObj.push_back(_objData6);
		netMsg.sContents.vecResultObj.push_back(_objData7);
		netMsg.sContents.vecResultObj.push_back(_objData8);
		netMsg.sContents.vecRewardObj.push_back(_objData9);

		msgpack::sbuffer buf;
		msgpack::pack(buf, netMsg);

		_pLandMan->sendMsgPC(NET_MSG_GCTRL_INSTANCE_CONTENTS_MSGPACK, buf);
		return 0;
	}

	int UI_CompetitionResult_Tour_End(lua_State* pState)
	{
		lua_getglobal(pState, "instanceMapID");
		const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));
		lua_pop(pState, 1);

		const float _OutTime((float)lua_tonumber(pState, -1));  // 부활 수;
		lua_pop(pState, 1);

		GLLandManager* const _pLandMan = gpGaeaServer->GetLandManager(_instanceMapID);
		if ( _pLandMan == NULL )
			return 0;
		GLMSG::NET_INSTANCE_COMPETITION_RESULT_END_INFO_FC netMsg;
		netMsg.fOutTime = _OutTime;
		netMsg.nCloseButtonParam = 1;

		msgpack::sbuffer buf;
		msgpack::pack(buf, netMsg);

		_pLandMan->sendMsgPC(NET_MSG_GCTRL_INSTANCE_CONTENTS_MSGPACK, buf);
		return 0;
	}

	
	int Matching_Resualt_Send_Start(lua_State* pState)
	{
		lua_getglobal(pState, "instanceMapID");
		const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));
		lua_pop(pState, 1);
		GLMSG::NET_MATCHING_RESUALT_SEND_START_FA sendserverinfo(_instanceMapID);
		gpGaeaServer->SENDTOAGENT(&sendserverinfo);

		return 0;
	}

	int Matching_Resualt_Send_End(lua_State* pState)
	{
		lua_getglobal(pState, "instanceMapID");
		const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));
		lua_pop(pState, 1);
		GLMSG::NET_MATCHING_RESUALT_SEND_END_FA sendserverinfo(_instanceMapID);
		gpGaeaServer->SENDTOAGENT(&sendserverinfo);

		return 0;
	}

	int MATCHING_RESULT_SCORE(lua_State* pState)
	{
		lua_getglobal(pState, "instanceMapID");
		const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));
		lua_pop(pState, 1);

		// 루아 이름
		std::string _strLuavalueName(sc::GetStringToLuaState(pState, -1));
		lua_pop(pState, 1);

		int luatype = lua_type(pState, -1);

		if(luatype == LUA_TNUMBER)
		{
			const lua_Integer _nItemMainID(lua_tonumber(pState, -1));
			lua_pop(pState, 1);
			GLMSG::NET_MATCHING_RESUALT_SCORE_FA sendserverinfo(_instanceMapID,_strLuavalueName.c_str(),(double)_nItemMainID);
			gpGaeaServer->SENDTOAGENT(&sendserverinfo);
		}
		else if(luatype == LUA_TSTRING)
		{
			std::string _strLuavalue(sc::GetStringToLuaState(pState, -1));
			lua_pop(pState, 1);

			GLMSG::NET_MATCHING_RESUALT_SCORE_FA sendserverinfo(_instanceMapID,_strLuavalueName.c_str(),_strLuavalue.c_str());
			gpGaeaServer->SENDTOAGENT(&sendserverinfo);
		}
		else if(luatype == LUA_TBOOLEAN)
		{
			bool _strLuavalue(lua_toboolean(pState, -1));
			lua_pop(pState, 1);
			GLMSG::NET_MATCHING_RESUALT_SCORE_FA sendserverinfo(_instanceMapID,_strLuavalueName.c_str(),_strLuavalue);
			gpGaeaServer->SENDTOAGENT(&sendserverinfo);
		}
		return 0;
	}
	int Matching_Faction_Resualt(lua_State* pState)
	{
		lua_getglobal(pState, "instanceMapID");
		const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));
		lua_pop(pState, 1);

		const DWORD _factinID((DWORD)lua_tointeger(pState, -1));
		lua_pop(pState, 1);

		const bool _iswin(lua_toboolean(pState, -1));
		lua_pop(pState, 1);

		GLMSG::NET_MATCHING_RESUALT_FACTION_RESUALT_FA sendserverinfo(_instanceMapID,_factinID,_iswin);
		gpGaeaServer->SENDTOAGENT(&sendserverinfo);

		return 0;
	}

    int Matching_Result_CustomMSG(lua_State* pState)
    {
        int currentStack(1);

        lua_getglobal(pState, "instanceMapID");
        const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));
        lua_pop(pState, 1);

        const double _param1(lua_tonumber(pState, currentStack++));
        const double _param2(lua_tonumber(pState, currentStack++));
        const double _param3(lua_tonumber(pState, currentStack++));
        const double _param4(lua_tonumber(pState, currentStack++));

        GLMSG::NET_MATCHING_RESULT_CUSTOM_MSG_FA sendserverinfo(_instanceMapID, _param1, _param2, _param3, _param4);
        gpGaeaServer->SENDTOAGENT(&sendserverinfo);

        return 0;
    }


	int Matching_Error_Send(lua_State* pState)
	{
		lua_getglobal(pState, "instanceMapID");
		const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));
		lua_pop(pState, 1);

		const DWORD _ErrorInfo((DWORD)lua_tointeger(pState, -1));
		lua_pop(pState, 1);


		GLMSG::NET_MATCHING_RESUALT_ERROR_FA sendserverinfo(_instanceMapID,_ErrorInfo);
		gpGaeaServer->SENDTOAGENT(&sendserverinfo);

		return 0;
	}

	int SetMatchingUserOn(lua_State* pState)
	{
		

		const DWORD ClientID(lua_tointeger(pState, -1));  // 주의! : PC의 경우 DbNum, 몹의 경우 GlobalID, 파티의 경우 PartyID, 클럽의 경우 ClubID;
		lua_pop(pState, 1);

		const DWORD ClientType(lua_tointeger(pState, -1));  // 주의! : PC의 경우 DbNum, 몹의 경우 GlobalID, 파티의 경우 PartyID, 클럽의 경우 ClubID;
		lua_pop(pState, 1);

		lua_getglobal(pState, "instanceMapID");
		const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));
		lua_pop(pState, 1);

		GLChar* const _pChar = gpGaeaServer->GetCharByDbNum(ClientID);
		if(_pChar == NULL)
			return 0;

		//Faction::FactionID faction(CROW_PC,ClientType);

// 		GLMSG::NET_TOURNAMENT_IN_TOURNAMENT_FC sendserverinfo(ClientType,_instanceMapID);
// 		gpGaeaServer->SENDTOCLIENT(_pChar->GETCLIENTID(),&sendserverinfo);
		return 0;
	}


	int SendClientsInstancePopUp(lua_State* pState)
	{
		lua_getglobal(pState, "instanceMapID");
		const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));
		lua_pop(pState, 1);

		const lua_Integer _nDbNum(lua_tointeger(pState, -1));
		lua_pop(pState, 1);

		size_t _strlen;
		const char* pStr = lua_tolstring(pState, -1, &_strlen);
		lua_pop(pState, 1);

		const lua_Integer _nGameTimeOut(lua_tointeger(pState, -1));
		lua_pop(pState, 1);

		const bool _bIsRevers(lua_toboolean(pState, -1));
		lua_pop(pState, 1);

		const bool _bIsShow(lua_toboolean(pState, -1));
		lua_pop(pState, 1);

		//_nDbNum

		GLChar* const _pChar = gpGaeaServer->GetCharByDbNum(_nDbNum);

		if(_pChar == NULL)
			return 0;

// 		GLMSG::NET_TOURNAMENT_GAME_POPUP_FC sendserverinfo(pStr,_strlen,_nGameTimeOut,_bIsRevers,_bIsShow);
// 		gpGaeaServer->SENDTOCLIENT(_pChar->GETCLIENTID(),&sendserverinfo);

		return 0;
	}



	int Matching_Tournament_OUT(lua_State* pState)
	{
		lua_getglobal(pState, "instanceMapID");
		const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));
		lua_pop(pState, 1);

		const lua_Integer _nDbNum(lua_tointeger(pState, -1));
		lua_pop(pState, 1);

		GLChar* const _pChar = gpGaeaServer->GetCharByDbNum(_nDbNum);

		if(_pChar == NULL)
			return 0;

		GLMSG::NET_TOURNAMENT_OUT_TOURNAMENT_FC sendserverinfo(_nDbNum,_instanceMapID);
		gpGaeaServer->SENDTOCLIENT(_pChar->GETCLIENTID(),&sendserverinfo);
		return 0;
	}

	int Matching_Tournament_OUT_ALL(lua_State* pState)
	{
		lua_getglobal(pState, "instanceMapID");
		const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));
		lua_pop(pState, 1);

		const lua_Integer _nDbNum(lua_tointeger(pState, -1));
		lua_pop(pState, 1);

		GLLandManager* const _pLandMan = gpGaeaServer->GetLandManager(_instanceMapID);
		if ( _pLandMan == NULL )
			return 0;

		GLMSG::NET_TOURNAMENT_OUT_TOURNAMENT_FC sendserverinfo(_nDbNum,_instanceMapID);
		_pLandMan->sendMsgPC(&sendserverinfo);
		return 0;
	}

	int Matching_Tournament_GamInfo_Role(lua_State* pState)
	{
		lua_getglobal(pState, "instanceMapID");
		const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));
		lua_pop(pState, 1);

		const lua_Integer leftTime(lua_tointeger(pState, -1));
		lua_pop(pState, 1);

		const lua_Integer GameWinConditionScore(lua_tointeger(pState, -1));
		lua_pop(pState, 1);

		GLLandManager* const _pLandMan = gpGaeaServer->GetLandManager(_instanceMapID);
		if ( _pLandMan == NULL )
			return 0;

		GLMSG::NET_TOURNAMENT_GAMEINFORMATION_ROLE_FC sendserverinfo(
			GameWinConditionScore,leftTime,_instanceMapID);
		_pLandMan->sendMsgPC(&sendserverinfo);
		return 0;
	}

	int SendGameTeamInfo(lua_State* pState)
	{

		lua_getglobal(pState, "instanceMapID");
		const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));
		lua_pop(pState, 1);

		const lua_Integer TeamFactionID(lua_tointeger(pState, -1));
		lua_pop(pState, 1);

		const lua_Integer TeamScore(lua_tointeger(pState, -1));
		lua_pop(pState, 1);

		GLLandManager* const _pLandMan = gpGaeaServer->GetLandManager(_instanceMapID);
		if ( _pLandMan == NULL )
			return 0;

		GLMSG::NET_TOURNAMENT_GAMEINFORMATION_TEAMINFO_FC sendserverinfo(TeamScore,TeamFactionID,_instanceMapID);
		_pLandMan->sendMsgPC(&sendserverinfo);
		return 0;
	}


	int SetFcationUI(lua_State* pState)
	{
		lua_getglobal(pState, "instanceMapID");
		const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));
		lua_pop(pState, 1);

		const lua_Integer _ViewType(lua_tointeger(pState, -1));
		lua_pop(pState, 1);

		const lua_Integer _Team2FactionID(lua_tointeger(pState, -1));
		lua_pop(pState, 1);

		const lua_Integer _Team1FactionID(lua_tointeger(pState, -1));
		lua_pop(pState, 1);

		const lua_Integer _nDbNum(lua_tointeger(pState, -1));
		lua_pop(pState, 1);

		GLChar* const _pChar = gpGaeaServer->GetCharByDbNum(_nDbNum);

		if(_pChar == NULL)
			return 0;

// 		GLLandManager* const _pLandMan = gpGaeaServer->GetLandManager(_instanceMapID);
// 		if ( _pLandMan == NULL )
// 			return 0;

		GLMSG::NET_TOURNAMENT_GAME_FACTION_INFO_FC sendserverinfo(_pChar->GetGaeaID(),_Team1FactionID,_Team2FactionID,_ViewType,_instanceMapID);
		gpGaeaServer->SENDTOCLIENT(_pChar->GETCLIENTID(),&sendserverinfo);
		return 0;
	}

    int UI_Indicator(lua_State* pState)
    {
        int currentStack(1);

        lua_getglobal(pState, "instanceMapID");
        const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));
        lua_pop(pState, 1);

        const DWORD _dwCharDbNum((DWORD)lua_tonumber(pState, currentStack++));
        GLChar* const _pChar = gpGaeaServer->GetCharByDbNum(_dwCharDbNum);
        if(_pChar == NULL)
            return 0;

        GLMSG::NET_TOURNAMENT_INDICATOR_UI_INFO_FC sendserverinfo(_instanceMapID);

        const int _emInfoType(lua_tointeger(pState, currentStack++));
        sendserverinfo.emInfoType = _emInfoType;

        LuaPlus::LuaObject _InfoData(LuaPlus::LuaState::CastState(pState), currentStack++);
        if ( _InfoData.IsString() )
        {
            sendserverinfo.strText = _InfoData.GetString();
        }
        else if ( _InfoData.IsNumber() )
        {
            sendserverinfo.fFloatData = (float)_InfoData.GetNumber();
        }
        else if ( _InfoData.IsBoolean() )
        {
            sendserverinfo.fFloatData = _InfoData.GetBoolean() ? 1.0f : 0.0f;
        }
        else
        {
            return 0;
        }

        WORD _wColorRed(0xFF);
        WORD _wColorGreen(0xFF);
        WORD _wColorBlue(0xFF);
        if ( ( _emInfoType == GLMSG::NET_TOURNAMENT_INDICATOR_UI_INFO_FC::LeftImage ) || ( _emInfoType == GLMSG::NET_TOURNAMENT_INDICATOR_UI_INFO_FC::RightImage ) )
        {
            const bool _bBlink(lua_toboolean(pState, currentStack++));
            sendserverinfo.bBlink = _bBlink;
        }
        else
        {
            LuaPlus::LuaObject _colorTable(LuaPlus::LuaState::CastState(pState), currentStack++);
            if ( _colorTable.IsTable() )
            {
                LuaPlus::LuaTableIterator _colorIterator(_colorTable);
                if ( _colorIterator.IsValid() )
                {
                    LuaPlus::LuaObject _colorR = _colorIterator.GetValue();
                    if ( _colorR.IsInteger() )
                        _wColorRed = _colorR.GetInteger();
                }
                if ( _colorIterator.Next() )
                {
                    LuaPlus::LuaObject _colorG = _colorIterator.GetValue();
                    if ( _colorG.IsInteger() )
                        _wColorGreen = _colorG.GetInteger();
                }
                if ( _colorIterator.Next() )
                {
                    LuaPlus::LuaObject _colorB = _colorIterator.GetValue();
                    if ( _colorB.IsInteger() )
                        _wColorBlue = _colorB.GetInteger();            
                }
            }
        }
        sendserverinfo.dwFontColor = (DWORD)D3DCOLOR_XRGB(_wColorRed, _wColorGreen, _wColorBlue);

        msgpack::sbuffer SendBuffer;
        msgpack::pack(SendBuffer, sendserverinfo);
        gpGaeaServer->SENDTOCLIENT(_pChar->GETCLIENTID(), NET_MSG_GCTRL_MATCHING_MSGPACK_SYSTEM, SendBuffer);
        return 0;
    }

    int UI_Indicator_All(lua_State* pState)
    {
        int currentStack(1);

        lua_getglobal(pState, "instanceMapID");
        const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));
        lua_pop(pState, 1);

        GLLandManager* const _pLandMan = gpGaeaServer->GetLandManager(_instanceMapID);
        if ( _pLandMan == NULL )
            return 0;

        GLMSG::NET_TOURNAMENT_INDICATOR_UI_INFO_FC sendserverinfo(_instanceMapID);

        const int _emInfoType(lua_tointeger(pState, currentStack++));
        sendserverinfo.emInfoType = _emInfoType;

        LuaPlus::LuaObject _InfoData(LuaPlus::LuaState::CastState(pState), currentStack++);
        if ( _InfoData.IsString() )
        {
            sendserverinfo.strText = _InfoData.GetString();
        }
        else if ( _InfoData.IsNumber() )
        {
            sendserverinfo.fFloatData = (float)_InfoData.GetNumber();
        }
        else if ( _InfoData.IsBoolean() )
        {
            sendserverinfo.fFloatData = _InfoData.GetBoolean() ? 1.0f : 0.0f;
        }
        else
        {
            return 0;
        }

        WORD _wColorRed(0xFF);
        WORD _wColorGreen(0xFF);
        WORD _wColorBlue(0xFF);
        if ( ( _emInfoType == GLMSG::NET_TOURNAMENT_INDICATOR_UI_INFO_FC::LeftImage ) || ( _emInfoType == GLMSG::NET_TOURNAMENT_INDICATOR_UI_INFO_FC::RightImage ) )
        {
            const bool _bBlink(lua_toboolean(pState, currentStack++));
            sendserverinfo.bBlink = _bBlink;
        }
        else
        {
            LuaPlus::LuaObject _colorTable(LuaPlus::LuaState::CastState(pState), currentStack++);
            if ( _colorTable.IsTable() )
            {
                LuaPlus::LuaTableIterator _colorIterator(_colorTable);
                if ( _colorIterator.IsValid() )
                {
                    LuaPlus::LuaObject _colorR = _colorIterator.GetValue();
                    if ( _colorR.IsInteger() )
                        _wColorRed = _colorR.GetInteger();

                    _colorIterator.Next();
                }
                if ( _colorIterator.IsValid() )
                {
                    LuaPlus::LuaObject _colorG = _colorIterator.GetValue();
                    if ( _colorG.IsInteger() )
                        _wColorGreen = _colorG.GetInteger();

                    _colorIterator.Next();
                }
                if ( _colorIterator.IsValid() )
                {
                    LuaPlus::LuaObject _colorB = _colorIterator.GetValue();
                    if ( _colorB.IsInteger() )
                        _wColorBlue = _colorB.GetInteger();            
                }
            }
        }
        sendserverinfo.dwFontColor = (DWORD)D3DCOLOR_XRGB(_wColorRed, _wColorGreen, _wColorBlue);

        msgpack::sbuffer SendBuffer;
        msgpack::pack(SendBuffer, sendserverinfo);
        _pLandMan->sendMsgPC(NET_MSG_GCTRL_MATCHING_MSGPACK_SYSTEM, SendBuffer);
        return 0;
    }

	int Matching_Tournament_FightCounting(lua_State* pState)
	{
		lua_getglobal(pState, "instanceMapID");
		const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));

		lua_pop(pState, 1);

		const lua_Integer _dwWinLose(lua_tointeger(pState, -1));
		lua_pop(pState, 1);

		//5이상은 안된다 ㅠㅠ
		if(_dwWinLose > 5)
			return 0;

		//0은 Fight 메시지. 만약 그것보다 작다면.. 이상한거니깐 
		// 보내지 않는다.
		if(_dwWinLose < 0)
			return 0;


		GLMSG::NET_TOURNAMENT_FIGHT_COUNTING_FC sendserverinfo((DWORD)_dwWinLose);

		GLLandManager* const _pLandMan = gpGaeaServer->GetLandManager(_instanceMapID);
		if ( _pLandMan == NULL )
			return 0;
		_pLandMan->sendMsgPC(&sendserverinfo);
		return 0;
		
	}

	int Matching_Tournament_GamInfo_Win(lua_State* pState)
	{

		lua_getglobal(pState, "instanceMapID");
		const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));
		lua_pop(pState, 1);

		const lua_Integer dwClientType(lua_tointeger(pState, -1));
		lua_pop(pState, 1);

		const lua_Integer _nDbNum(lua_tointeger(pState, -1));
		lua_pop(pState, 1);

		GLChar* const _pChar = gpGaeaServer->GetCharByDbNum(_nDbNum);

		if(_pChar == NULL)
			return 0;

		GLMSG::NET_TOURNAMENT_GAMEINFORMATION_WIN_FC sendserverinfo(_nDbNum,dwClientType,_instanceMapID);
		gpGaeaServer->SENDTOCLIENT(_pChar->GETCLIENTID(),&sendserverinfo);
		return 0;
	}

	int Matching_Tournament_GamInfo_Lose(lua_State* pState)
	{
		lua_getglobal(pState, "instanceMapID");
		const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));
		lua_pop(pState, 1);

		const lua_Integer dwClientType(lua_tointeger(pState, -1));
		lua_pop(pState, 1);

		const lua_Integer _nDbNum(lua_tointeger(pState, -1));
		lua_pop(pState, 1);

		GLChar* const _pChar = gpGaeaServer->GetCharByDbNum(_nDbNum);

		if(_pChar == NULL)
			return 0;

		GLMSG::NET_TOURNAMENT_GAMEINFORMATION_LOSE_FC sendserverinfo(_nDbNum,dwClientType,_instanceMapID);
		gpGaeaServer->SENDTOCLIENT(_pChar->GETCLIENTID(),&sendserverinfo);
		return 0;
	}

	int Matching_Tournament_GameTimeLimit(lua_State* pState)
	{
		lua_getglobal(pState, "instanceMapID");
		const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));
		lua_pop(pState, 1);

		const bool bShow(lua_toboolean(pState, -1));
		lua_pop(pState, 1);

		const bool bBlank(lua_toboolean(pState, -1));
		lua_pop(pState, 1);

		const lua_Integer nSec(lua_tointeger(pState, -1));
		lua_pop(pState, 1);

		GLLandManager* const _pLandMan = gpGaeaServer->GetLandManager(_instanceMapID);
		if ( _pLandMan == NULL )
			return 0;

		GLMSG::NET_TOURNAMENT_GAME_TIMELIMIT_FC sendserverinfo(bShow,bBlank,nSec);
		_pLandMan->sendMsgPC(&sendserverinfo);
		return 0;
	}

	int Matching_GetLandSize(lua_State* pState)
	{

		lua_getglobal(pState, "instanceMapID");
		const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));
		lua_pop(pState, 1);

		const WORD _crowSubID((WORD)lua_tointeger(pState, -1));
		lua_pop(pState, 1);

		const WORD _crowMainID((WORD)lua_tointeger(pState, -1 ));
		lua_pop(pState, 1);

		const SNATIVEID _MapID(_crowMainID, _crowSubID);

		GLLandManager* const _pLandMan = gpGaeaServer->GetLandManager(_instanceMapID);
		if ( _pLandMan == NULL )
			return 0;

		int maxSizeX = _pLandMan->getLand(_MapID)->GetLandTree()->GetSizeX();
		int maxSizeY = _pLandMan->getLand(_MapID)->GetLandTree()->GetSizeX();

		if(maxSizeX < maxSizeY)
			lua_pushinteger(pState, maxSizeY);
		else
			lua_pushinteger(pState, maxSizeX);
		return 1;
	}

    int Report_CaptureTheFlag_Result(lua_State* pState)
    {
        int currentStack(1);
    
        lua_getglobal(pState, "instanceMapID");
        const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));
        lua_pop(pState, 1);

        LuaPlus::LuaObject _redTeamList(LuaPlus::LuaState::CastState(pState), currentStack++);
        LuaPlus::LuaObject _blueTeamList(LuaPlus::LuaState::CastState(pState), currentStack++);
        const int _winnerTeam = lua_tointeger(pState, currentStack++);

        GLMSG::NET_MATCHING_CAPTURE_THE_FLAG_RESULT_FA netMsg;
        netMsg.InstanceMapID = _instanceMapID;
        netMsg.dwResult = _winnerTeam;

        if ( _redTeamList.IsTable() )
        {
            LuaPlus::LuaTableIterator _GroupIterator(_redTeamList);
            while ( _GroupIterator.IsValid() )
            {
                LuaPlus::LuaObject _keyObj = _GroupIterator.GetKey();
                LuaPlus::LuaObject _valueObj = _GroupIterator.GetValue();

                if ( _keyObj.IsNumber() && _valueObj.IsBoolean() ) 
                {
                    const DWORD _userDbNum = _keyObj.GetInteger();
                    const bool _userExist = _valueObj.GetBoolean();
                    if ( _userExist == true )
                    {
                        netMsg.RedExistPlayer.push_back(_userDbNum);
                    }
                    else
                    {
                        netMsg.RedRunawayPlayer.push_back(_userDbNum);
                    }
                }
                _GroupIterator.Next();
            }
        }
        if ( _blueTeamList.IsTable() )
        {
            LuaPlus::LuaTableIterator _GroupIterator(_blueTeamList);
            while ( _GroupIterator.IsValid() )
            {
                LuaPlus::LuaObject _keyObj = _GroupIterator.GetKey();
                LuaPlus::LuaObject _valueObj = _GroupIterator.GetValue();

                if ( _keyObj.IsNumber() && _valueObj.IsBoolean() ) 
                {
                    const DWORD _userDbNum = _keyObj.GetInteger();
                    const bool _userExist = _valueObj.GetBoolean();
                    if ( _userExist == true )
                    {
                        netMsg.BlueExistPlayer.push_back(_userDbNum);
                    }
                    else
                    {
                        netMsg.BlueRunawayPlayer.push_back(_userDbNum);
                    }
                }
                _GroupIterator.Next();
            }
        }

        msgpack::sbuffer buf;
        msgpack::pack(buf, netMsg);

        gpGaeaServer->SENDTOAGENT(NET_MSG_GCTRL_MATCHING_MSGPACK_SYSTEM, buf);

        return 0;
    }
}
