#include "pch.h"

#include "../../../FieldServer/GLGaeaServer.h"
#include "../../../Club/GLClubManField.h"
#include "../../../Club/GLClubField.h"
#include "../../../Club/GLClubManAgent.h"
#include "../../../Club/GLClubAgent.h"
#include "../../../../RanLogic/Msg/GLContrlInstanceContentsMsg.h"
#include "../../InstanceSystem.h"
#include "../../LogicField/InstanceSystemField.h"

#include "../../../Database/DBAction/DbActionLogInstantDungeon.h"

#include "../../../../EngineLib/GUInterface/GameTextControl.h"


#include "./RnClubDeathMatch.h"

namespace InstanceSystem
{
    PreviousCDM_Info gPreviousCDM_Info;

    void registFunction_CDM(sc::LuaInstance& _luaInstance)
    {
        _luaInstance.RegistFunction("HaveCDM_Authority", InstanceSystem::HaveCDM_Authority);
        _luaInstance.RegistFunction("Save_CDM_History", InstanceSystem::Save_CDM_History);
    }

    void registFunction_CDM(InstanceSystem::ScriptManager* const _pScriptManager)
    {
        if ( _pScriptManager == NULL )
            return;

		_pScriptManager->registFunctionCommon( registFunction_CDM );
    }

    void registFunction_CDM_Instance(sc::LuaInstance& _luaInstance)
    {
        _luaInstance.RegistFunction("UI_CDM_Progress", InstanceSystem::UI_CDM_Progress);
        _luaInstance.RegistFunction("UI_CDM_ProgressBroad", InstanceSystem::UI_CDM_ProgressBroad);
        _luaInstance.RegistFunction("UI_CDM_ProgressOff", InstanceSystem::UI_CDM_ProgressOff);
        _luaInstance.RegistFunction("UI_CDM_ProgressOffBroad", InstanceSystem::UI_CDM_ProgressOffBroad);
        _luaInstance.RegistFunction("UI_CDM_BattleStart", InstanceSystem::UI_CDM_BattleStart);
        _luaInstance.RegistFunction("UI_CDM_BattleEnd", InstanceSystem::UI_CDM_BattleEnd);
        _luaInstance.RegistFunction("UI_CDM_DoublePoint", InstanceSystem::UI_CDM_DoublePoint);
        _luaInstance.RegistFunction("UI_CDM_DoublePointBroad", InstanceSystem::UI_CDM_DoublePointBroad);

        _luaInstance.RegistFunction("UI_CompetitionResult_CDM_Begin", InstanceSystem::UI_CompetitionResult_CDM_Begin);
        _luaInstance.RegistFunction("UI_CompetitionResult_CDM_Info", InstanceSystem::UI_CompetitionResult_CDM_Info);
        _luaInstance.RegistFunction("UI_CompetitionResult_CDM_End", InstanceSystem::UI_CompetitionResult_CDM_End);

        _luaInstance.RegistFunction("Log_CDM_In", InstanceSystem::Log_CDM_In);
        _luaInstance.RegistFunction("Log_CDM_Out", InstanceSystem::Log_CDM_Out);
        _luaInstance.RegistFunction("Log_CDM_Result", InstanceSystem::Log_CDM_Result);
    }

    void registFunction_CDM_Instance(InstanceSystem::ScriptManager* const _pScriptManager)
    {
        if ( _pScriptManager == NULL )
            return;

		_pScriptManager->registFunctionCommon( registFunction_CDM_Instance );
    }

    void registFunction_CDM_Agent(sc::LuaInstance& _luaInstance)
    {
        _luaInstance.RegistFunction("HaveCDM_Authority", InstanceSystem::HaveCDM_Authority_Agent);
        _luaInstance.RegistFunction("Send_CDM_History", InstanceSystem::Send_CDM_History_Agent);
    }

    void registFunction_CDM_Agent(InstanceSystem::ScriptManager* const _pScriptManager)
    {
        if ( _pScriptManager == NULL )
            return;

		_pScriptManager->registFunctionCommon( registFunction_CDM_Agent );
    }







    int UI_CDM_Progress(lua_State* pState)
    {
        int currentStack(1);
        GLMSG::NET_INSTANCE_RNCDM_PROGRESS_UPDATE_FC NetMsg;

        lua_getglobal(pState, "instanceMapID");
        const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));
        lua_pop(pState, 1);
        GLLandManager* const pLandMan = gpGaeaServer->GetLandManager(_instanceMapID);

        const lua_Integer _nDbNum(lua_tointeger(pState, currentStack++));
        GLChar* const _pChar = gpGaeaServer->GetCharByDbNum(_nDbNum);
        if ( _pChar == NULL )
            return 0;

        int _ProcessingTime((int)lua_tointeger(pState, currentStack++));
        const int nHour = _ProcessingTime / 3600;
        _ProcessingTime -= nHour * 3600;
        const int nMin = _ProcessingTime / 60;
        _ProcessingTime -= nMin * 60;
        const int nSec = _ProcessingTime;

        NetMsg.nHour = nHour;
        NetMsg.nMin = nMin;
        NetMsg.nSecond = nSec;

        LuaPlus::LuaObject _ClubTable(LuaPlus::LuaState::CastState(pState), currentStack++);
        if ( _ClubTable.IsTable() == false )
            return false;

        for ( LuaPlus::LuaTableIterator _iter(_ClubTable); _iter; _iter.Next() )
        {
            LuaPlus::LuaObject _keyObj = _iter.GetKey();
            if ( _keyObj.IsInteger() == false )
                continue;
            DWORD _dwClubDbNum = _keyObj.GetInteger();

            LuaPlus::LuaObject _valueObj = _iter.GetValue();
            if ( _valueObj.IsInteger() == false )
                continue;
            DWORD _dwClubPoint = _valueObj.GetInteger();

            std::tr1::shared_ptr<GLClubField> spClub = gpGaeaServer->GetClub(_dwClubDbNum);
            if ( spClub == NULL )
                continue;
            
            if ( NetMsg.addClub( spClub->GetMarkVersion(), _dwClubDbNum, _dwClubPoint, spClub->Name() ) == false )
            {
                gpGaeaServer->SENDTOCLIENT(_pChar->GETCLIENTID(), &NetMsg);
                NetMsg.reset();
            }
        }
        if ( NetMsg.isValid() )
        {
            gpGaeaServer->SENDTOCLIENT(_pChar->GETCLIENTID(), &NetMsg);
            NetMsg.reset();
        }

        return 0;
    }

    int UI_CDM_ProgressBroad(lua_State* pState)
    {
        int currentStack(1);
        GLMSG::NET_INSTANCE_RNCDM_PROGRESS_UPDATE_FC NetMsg;

        lua_getglobal(pState, "instanceMapID");
        const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));
        lua_pop(pState, 1);
        GLLandManager* const pLandMan = gpGaeaServer->GetLandManager(_instanceMapID);

        int _ProcessingTime((int)lua_tointeger(pState, currentStack++));
        const int nHour = _ProcessingTime / 3600;
        _ProcessingTime -= nHour * 3600;
        const int nMin = _ProcessingTime / 60;
        _ProcessingTime -= nMin * 60;
        const int nSec = _ProcessingTime;

        NetMsg.nHour = nHour;
        NetMsg.nMin = nMin;
        NetMsg.nSecond = nSec;

        LuaPlus::LuaObject _ClubTable(LuaPlus::LuaState::CastState(pState), currentStack++);
        if ( _ClubTable.IsTable() == false )
            return false;

        for ( LuaPlus::LuaTableIterator _iter(_ClubTable); _iter; _iter.Next() )
        {
            LuaPlus::LuaObject _keyObj = _iter.GetKey();
            if ( _keyObj.IsInteger() == false )
                continue;
            DWORD _dwClubDbNum = _keyObj.GetInteger();

            LuaPlus::LuaObject _valueObj = _iter.GetValue();
            if ( _valueObj.IsInteger() == false )
                continue;
            DWORD _dwClubPoint = _valueObj.GetInteger();

            std::tr1::shared_ptr<GLClubField> spClub = gpGaeaServer->GetClub(_dwClubDbNum);
            if ( spClub == NULL )
                continue;

            if ( NetMsg.addClub( spClub->GetMarkVersion(), _dwClubDbNum, _dwClubPoint, spClub->Name() ) == false )
            {
                pLandMan->sendMsgPC(&NetMsg);
                NetMsg.reset();
            }
        }
        if ( NetMsg.isValid() )
        {
            pLandMan->sendMsgPC(&NetMsg);
            NetMsg.reset();
        }

        return 0;
    }

    int UI_CDM_ProgressOff(lua_State* pState)
    {
        int currentStack(1);

        GLMSG::NET_INSTANCE_RNCDM_PROGRESS_OFF_FC NetMsg;

        const lua_Integer _nDbNum(lua_tointeger(pState, currentStack++));

        GLChar* const _pChar = gpGaeaServer->GetCharByDbNum(_nDbNum);
        if ( _pChar == NULL )
            return 0;

        GLMSG::NET_INSTANCE_RNCDM_PROGRESS_OFF_FC pNetMsg;
        gpGaeaServer->SENDTOCLIENT(_pChar->GETCLIENTID(), &pNetMsg);

        return 0;
    }

    int UI_CDM_ProgressOffBroad(lua_State* pState)
    {
        GLMSG::NET_INSTANCE_RNCDM_PROGRESS_OFF_FC NetMsg;

        lua_getglobal(pState, "instanceMapID");
        const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));
        lua_pop(pState, 1);
        GLLandManager* const pLandMan = gpGaeaServer->GetLandManager(_instanceMapID);

        if ( pLandMan == NULL )
            return 0;

        pLandMan->sendMsgPC(&NetMsg);

        return 0;
    }

    int UI_CDM_BattleStart(lua_State* pState) // CDM 전투종료 이미지 (인던내 모든 플레이어에게 전송);
    {
        int currentStack(1);

        GLMSG::NET_INSTANCE_RNCDM_START_LOGO_SHOW_FC NetMsg;
        lua_getglobal(pState, "instanceMapID");

        const InstanceMapID _instanceMapID((DWORD)lua_tointeger(pState, currentStack++));

        GLLandManager* const pLandMan = gpGaeaServer->GetLandManager(_instanceMapID);

        if ( pLandMan )
            pLandMan->sendMsgPC(&NetMsg);

        return 0;
    }

    int UI_CDM_BattleEnd(lua_State* pState)
    {
        int currentStack(1);

        GLMSG::NET_INSTANCE_RNCDM_END_LOGO_SHOW_FC NetMsg;
        lua_getglobal(pState, "instanceMapID");

        const InstanceMapID _instanceMapID((DWORD)lua_tointeger(pState, currentStack++));

        GLLandManager* const pLandMan = gpGaeaServer->GetLandManager(_instanceMapID);

        if ( pLandMan )
            pLandMan->sendMsgPC(&NetMsg);

        return 0;
    }

    int UI_CDM_DoublePoint(lua_State* pState) // 더블포인트 UI;
    {
        int currentStack(1);

        lua_getglobal(pState, "instanceMapID");
        const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));
        lua_pop(pState, 1);

        const DWORD _dwActorID = (DWORD)lua_tointeger(pState, currentStack++);
        const BOOL _bVisible = lua_toboolean(pState, currentStack++);

        GLChar* const _pChar = gpGaeaServer->GetCharByDbNum(_dwActorID);
        if ( _pChar == NULL )
            return 0;

        GLMSG::NET_INSTANCE_RNCDM_DOUBLE_POINT_FC NetMsg;
        NetMsg.bVisible = _bVisible;
        msgpack::sbuffer buf;
        msgpack::pack(buf, NetMsg);

        gpGaeaServer->SENDTOCLIENT(_pChar->GETCLIENTID(), NET_MSG_GCTRL_INSTANCE_CONTENTS_MSGPACK, buf);        
        return 0;
    }

    int UI_CDM_DoublePointBroad(lua_State* pState) // 더블포인트 UI;
    {
        int currentStack(1);

        lua_getglobal(pState, "instanceMapID");
        const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));
        lua_pop(pState, 1);

        const BOOL _bVisible = lua_toboolean(pState, currentStack++);

        GLMSG::NET_INSTANCE_RNCDM_DOUBLE_POINT_FC NetMsg;
        NetMsg.bVisible = _bVisible;

        GLLandManager* const pLandMan = gpGaeaServer->GetLandManager(_instanceMapID);

        if ( pLandMan )
        {
            msgpack::sbuffer buf;
            msgpack::pack(buf, NetMsg);

            pLandMan->sendMsgPC(NET_MSG_GCTRL_INSTANCE_CONTENTS_MSGPACK, buf);
        }

        return 0;
    }

    int HaveCDM_Authority(lua_State* pState)
    {
        int currentStack(1);

        const DWORD _nMemeberDbNum = lua_tointeger(pState, currentStack++);

        GLChar* const pChar =gpGaeaServer->GetCharByDbNum(_nMemeberDbNum);
        if ( pChar == NULL )
        {
            lua_pushboolean(pState, false);
            return 1;
        }

        std::tr1::shared_ptr<GLClubField> spClub = gpGaeaServer->GetClub(pChar->GetClubDbNum());
        if ( spClub == NULL )
        {
            lua_pushboolean(pState, false);
            return 1;
        }

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
		sc::writeLogInfo( "[ Instance Log ] [ Invoke - HaveCDM_Authority ]" );
#endif

        const bool bAuthority = spClub->HaveDeathMatchAuthority(_nMemeberDbNum);
        lua_pushboolean(pState, bAuthority);
        return 1;
    }

    int HaveCDM_Authority_Agent(lua_State* pState)
    {
        int currentStack(1);

        const DWORD _nMemeberDbNum = lua_tointeger(pState, currentStack++);

        GLCharAG* const pAgChar(gpAgentServer->GetCharByDbNum(_nMemeberDbNum));
        if ( pAgChar == NULL )
        {
            lua_pushboolean(pState, false);
            return 1;
        }

        std::tr1::shared_ptr<GLClubAgent> spClub = gpAgentServer->GetClub(pAgChar->GetClubDbNum());
        if ( spClub == NULL )
        {
            lua_pushboolean(pState, false);
            return 1;
        }

        const bool bAuthority = spClub->HaveDeathMatchAuthority(_nMemeberDbNum);
        lua_pushboolean(pState, bAuthority);
        return 1;
    }

    int Save_CDM_History(lua_State* pState)  // 이전CDM 결과 기록 (Field -> Agent);
    {
        int currentStack(1);
        GLMSG::NET_INSTANCE_RNCDM_PREVIOUS_INFO_FAC NetMsg;   

        const __time64_t _tStartTime(lua_tonumber(pState, currentStack++));
        const __time64_t _tEndTime(lua_tonumber(pState, currentStack++));

        NetMsg.tStartTime = _tStartTime;
        NetMsg.tEndTime = _tEndTime;

        LuaPlus::LuaObject _RankTable(LuaPlus::LuaState::CastState(pState), currentStack++);
        if ( _RankTable.IsTable() )
        {
            for ( LuaPlus::LuaTableIterator _iter(_RankTable); _iter; _iter.Next() )
            {
				LuaPlus::LuaObject _key = _iter.GetKey();
				if ( _key.IsInteger() == false )
					continue;

				LuaPlus::LuaObject _obj = _iter.GetValue();
				if ( _obj.IsInteger() == false )
					continue;

				NetMsg.vecClubDbNum.push_back(_key.GetInteger());                
				NetMsg.vecClubRank.push_back(_obj.GetInteger());
            }
        }
        
        msgpack::sbuffer buf;
        msgpack::pack(buf, NetMsg);
        gpGaeaServer->SENDTOAGENT(NET_MSG_GCTRL_INSTANCE_CONTENTS_MSGPACK, buf);

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
		sc::writeLogInfo( "[ Instance Log ] [ Invoke - Save_CDM_History ]" );
#endif

        return 0;
    }

    int Send_CDM_History_Agent(lua_State* pState)  // 이전CDM 결과 전송 (Agent -> Client);
    {
        int currentStack(1);
        GLMSG::NET_INSTANCE_RNCDM_PREVIOUS_INFO_FAC NetMsg;

        const DWORD _charDbNum = lua_tointeger(pState,currentStack++);

        GLCharAG* pCharAG = gpAgentServer->GetCharByDbNum(_charDbNum);
        if ( pCharAG == NULL )
            return 0;

        NetMsg.tStartTime = gPreviousCDM_Info.previous_StartTime;
        NetMsg.tEndTime = gPreviousCDM_Info.previous_EndTime;
        NetMsg.vecClubRank = gPreviousCDM_Info.vecPrevious_Rank;
        NetMsg.vecClubDbNum = gPreviousCDM_Info.vecPrevious_ClubDbNum;

        msgpack::sbuffer buf;
        msgpack::pack(buf, NetMsg);
        gpAgentServer->SENDTOCLIENT(pCharAG->ClientSlot(), NET_MSG_GCTRL_INSTANCE_CONTENTS_MSGPACK, buf);
        return 0;
    }

    int UI_CompetitionResult_CDM_Begin(lua_State* pState)
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
        _obj1.strTitleName = ID2SERVERTEXT("RN_COMPETITION_RESULT_TITLE",0); //"순위";

        InstanceSystem::RnCompetitionResultTitleObj _obj2;
        _obj2.emItemType = InstanceSystem::RnCompetitionResultTitleObj::EM_CONTENT_ICON_CLUBMARK;
        _obj2.nBoxSize = 34;
        _obj2.strTitleName = ID2SERVERTEXT("RN_COMPETITION_RESULT_TITLE",15); //"클럽마크";

        InstanceSystem::RnCompetitionResultTitleObj _obj3;
        _obj3.emItemType = InstanceSystem::RnCompetitionResultTitleObj::EM_CONTENT_STRING;
        _obj3.nBoxSize = 110;
        _obj3.strTitleName = ID2SERVERTEXT("RN_COMPETITION_RESULT_TITLE",4); // "클럽명";

        InstanceSystem::RnCompetitionResultTitleObj _obj4;
        _obj4.emItemType = InstanceSystem::RnCompetitionResultTitleObj::EM_CONTENT_STRING;
        _obj4.nBoxSize = 110;
        _obj4.strTitleName = ID2SERVERTEXT("RN_COMPETITION_RESULT_TITLE",14); // "클럽 마스터명";

        InstanceSystem::RnCompetitionResultTitleObj _obj5;
        _obj5.emItemType = InstanceSystem::RnCompetitionResultTitleObj::EM_CONTENT_INT;
        _obj5.nBoxSize = 64;
        _obj5.strTitleName = ID2SERVERTEXT("RN_COMPETITION_RESULT_TITLE",5); // "획득 pt";

        InstanceSystem::RnCompetitionResultTitleObj _obj6;
        _obj6.emItemType = InstanceSystem::RnCompetitionResultTitleObj::EM_CONTENT_INT;
        _obj6.nBoxSize = 64;
        _obj6.strTitleName = ID2SERVERTEXT("RN_COMPETITION_RESULT_TITLE",8); // "사살";

        InstanceSystem::RnCompetitionResultTitleObj _obj7;
        _obj7.emItemType = InstanceSystem::RnCompetitionResultTitleObj::EM_CONTENT_INT;
        _obj7.nBoxSize = 64;
        _obj7.strTitleName = ID2SERVERTEXT("RN_COMPETITION_RESULT_TITLE",9); // "죽음";

        InstanceSystem::RnCompetitionResultTitleObj _obj8;
        _obj8.emItemType = InstanceSystem::RnCompetitionResultTitleObj::EM_CONTENT_INT;
        _obj8.nBoxSize = 64;
        _obj8.strTitleName = ID2SERVERTEXT("RN_COMPETITION_RESULT_TITLE",10); // "부활";

        InstanceSystem::RnCompetitionResultTitleObj _obj9;
        _obj9.emItemType = (InstanceSystem::RnCompetitionResultTitleObj::EM_CONTENT_REWARD) | (InstanceSystem::RnCompetitionResultTitleObj::EM_CONTENT_REWARD_ITEM);
        _obj9.nBoxSize = 50;
        _obj9.strTitleName = ID2SERVERTEXT("RN_COMPETITION_RESULT_TITLE",11); // "보상";

        GLMSG::NET_INSTANCE_COMPETITION_RESULT_TAB_FC netMsg;
		netMsg.typeCompetition = InstanceSystem::COMPETITION_CDM;

        netMsg.sTabInfo.strTabName = ID2SERVERTEXT("RN_COMPETITION_RESULT_TAB",0); // "전투 결과";
        netMsg.sTabInfo.emSortType = InstanceSystem::RnCompetitionResultTabInfo::EM_RESULT_SOR_ASC;
        netMsg.sTabInfo.wSortTitleNum = 0;
        netMsg.sTabInfo.emMyResultType = InstanceSystem::RnCompetitionResultTabInfo::EM_MY_RESULT_CLUB;

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

    int UI_CompetitionResult_CDM_Info(lua_State* pState)
    {
        int currentStack(1);

        lua_getglobal(pState, "instanceMapID");
        const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));
        lua_pop(pState, 1);

        GLLandManager* const _pLandMan = gpGaeaServer->GetLandManager(_instanceMapID);
        if ( _pLandMan == NULL )
            return 0;

        GLMSG::NET_INSTANCE_COMPETITION_RESULT_INFO_FC netMsg;
		netMsg.typeCompetition = InstanceSystem::COMPETITION_CDM;
        netMsg.nTabNumber = 0;

        const int _keyID((DWORD)lua_tointeger(pState, currentStack++));  // ClubID;
        netMsg.sContents.dwKeyID = _keyID;

        const int _data1((DWORD)lua_tointeger(pState, currentStack++));  // 순위;
        InstanceSystem::RnCompetitionResultContentObj _objData1;
        _objData1.ContentItemInt = _data1;

        const int _data2((DWORD)lua_tointeger(pState, currentStack++));  // 학원;
        InstanceSystem::RnCompetitionResultContentObj _objData2;
        _objData2.ContentItemInt = _data2;

        const char* _data3(sc::GetStringToLuaState(pState, currentStack++));  // 클럽 명;
        InstanceSystem::RnCompetitionResultContentObj _objData3;
        _objData3.ContentItemString = _data3;

        const char* _data4(sc::GetStringToLuaState(pState, currentStack++));  // 클럽 마스터 명;
        InstanceSystem::RnCompetitionResultContentObj _objData4;
        _objData4.ContentItemString = _data4;

        const int _data5((DWORD)lua_tointeger(pState, currentStack++));  // 획득 포인트;
        InstanceSystem::RnCompetitionResultContentObj _objData5;
        _objData5.ContentItemInt = _data5;

        const int _data6((DWORD)lua_tointeger(pState, currentStack++));  // 사살 수;
        InstanceSystem::RnCompetitionResultContentObj _objData6;
        _objData6.ContentItemInt = _data6;

        const int _data7((DWORD)lua_tointeger(pState, currentStack++));  // 사망 수;
        InstanceSystem::RnCompetitionResultContentObj _objData7;
        _objData7.ContentItemInt = _data7;

        const int _data8((DWORD)lua_tointeger(pState, currentStack++));  // 부활 수;
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

                    if ( _IDiterator.Next() == false )
                        continue;
                    
                    LuaPlus::LuaObject _SIDObj = _IDiterator.GetValue();
                    if ( _SIDObj.IsInteger() )
                        _rewardItem.itemID.wSubID = _SIDObj.GetInteger();

                    _rewardItem.wAmount = 1;
					if ( _IDiterator.Next() != false )
					{
						if ( _IDiterator.IsValid() )
						{
							LuaPlus::LuaObject _AmountObj = _IDiterator.GetValue();
							if ( _AmountObj.IsInteger() )
								_rewardItem.wAmount = _AmountObj.GetInteger();
						}
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

    int UI_CompetitionResult_CDM_End(lua_State* pState)
    {
        int currentStack(1);
        const float _OutTime((float)lua_tonumber(pState, currentStack++));  // 부활 수;
        
        lua_getglobal(pState, "instanceMapID");
        const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));
        lua_pop(pState, 1);

        GLLandManager* const _pLandMan = gpGaeaServer->GetLandManager(_instanceMapID);
        if ( _pLandMan == NULL )
            return 0;
        GLMSG::NET_INSTANCE_COMPETITION_RESULT_END_INFO_FC netMsg;
		netMsg.typeCompetition = InstanceSystem::COMPETITION_CDM;
        netMsg.fOutTime = _OutTime;
        netMsg.nCloseButtonParam = 1;

        msgpack::sbuffer buf;
        msgpack::pack(buf, netMsg);

        _pLandMan->sendMsgPC(NET_MSG_GCTRL_INSTANCE_CONTENTS_MSGPACK, buf);
        return 0;
    }


    /////////////////////////    Log    ////////////////////////////
    int Log_CDM_In(lua_State* pState)
    {
        int currentStack(1);
        const DWORD _charDbNum((DWORD)lua_tointeger(pState, currentStack++));  // 케릭터 DbNum;
        const DWORD _clubDbNum((DWORD)lua_tointeger(pState, currentStack++));  // 클럽 DbNum;

        lua_getglobal(pState, "instanceMapID");
        const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));
        lua_pop(pState, 1);

        GLLandManager* const _pLandMan = gpGaeaServer->GetLandManager(_instanceMapID);
        if ( _pLandMan == NULL )
            return 0;

        db::LogInstantCDM* pDbAction = new db::LogInstantCDM(
            db::LogInstantCDM::EM_IN,
            _pLandMan->getGUID());

        pDbAction->dwCharDbNum = _charDbNum;
        pDbAction->dwClubDbNum = _clubDbNum;

        gpGaeaServer->AddLogAdoJob( db::DbActionPtr(pDbAction) );

        return 0;
    }

    int Log_CDM_Out(lua_State* pState)
    {
        int currentStack(1);

        const DWORD _charDbNum((DWORD)lua_tointeger(pState, currentStack++));  // 케릭터 DbNum;
        const DWORD _clubDbNum((DWORD)lua_tointeger(pState, currentStack++));  // 클럽 DbNum;

        lua_getglobal(pState, "instanceMapID");
        const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));
        lua_pop(pState, 1);

        GLLandManager* const _pLandMan = gpGaeaServer->GetLandManager(_instanceMapID);
        if ( _pLandMan == NULL )
            return 0;

        db::LogInstantCDM* pDbAction = new db::LogInstantCDM(
            db::LogInstantCDM::EM_OUT,
            _pLandMan->getGUID());

        pDbAction->dwCharDbNum = _charDbNum;
        pDbAction->dwClubDbNum = _clubDbNum;

        gpGaeaServer->AddLogAdoJob( db::DbActionPtr(pDbAction) );
        return 0;
    }

    int Log_CDM_Result(lua_State* pState)
    {
        int currentStack(1);

        const DWORD _clubDbNum((DWORD)lua_tointeger(pState, currentStack++));  // 클럽 DbNum;
        const DWORD _clubRank((DWORD)lua_tointeger(pState, currentStack++));  // 클럽 DbNum;
        const DWORD _clubPoint((DWORD)lua_tointeger(pState, currentStack++));  // 클럽 DbNum;
        const DWORD _clubKill((DWORD)lua_tointeger(pState, currentStack++));  // 클럽 DbNum;
        const DWORD _clubDeath((DWORD)lua_tointeger(pState, currentStack++));  // 클럽 DbNum;        

        lua_getglobal(pState, "instanceMapID");
        const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));
        lua_pop(pState, 1);

        GLLandManager* const _pLandMan = gpGaeaServer->GetLandManager(_instanceMapID);
        if ( _pLandMan == NULL )
            return 0;

        db::LogInstantCDM* pDbAction = new db::LogInstantCDM(
            db::LogInstantCDM::EM_RESULT,
            _pLandMan->getGUID());

        pDbAction->dwClubDbNum = _clubDbNum;
        pDbAction->nClubRank = _clubRank;
        pDbAction->nClubPoint = _clubPoint;
        pDbAction->nClubKill = _clubKill;
        pDbAction->nClubDeath = _clubDeath;

        gpGaeaServer->AddLogAdoJob( db::DbActionPtr(pDbAction) );
        return 0;
    }

}