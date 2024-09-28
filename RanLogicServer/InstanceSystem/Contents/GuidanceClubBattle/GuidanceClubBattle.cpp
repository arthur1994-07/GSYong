#include "pch.h"

#include "../../../FieldServer/GLGaeaServer.h"
#include "../../../Guidance/GuidanceAgent.h"
#include "../../../Club/GLClubManField.h"
#include "../../../Club/GLClubField.h"
#include "../../../Club/GLClubManAgent.h"
#include "../../../Club/GLClubAgent.h"
#include "../../../../RanLogic/Msg/GLContrlInstanceContentsMsg.h"
#include "../../InstanceSystem.h"
#include "../../LogicField/InstanceSystemField.h"

#include "../../../Database/DBAction/DbActionLogInstantDungeon.h"

#include "../../../../EngineLib/GUInterface/GameTextControl.h"

#include "GuidanceClubBattle.h"

namespace InstanceSystem
{
    void registFunction_Guidance(sc::LuaInstance& _luaInstance)
    {
    }

    void registFunction_Guidance(InstanceSystem::ScriptManager* const _pScriptManager)
    {
        if ( _pScriptManager == NULL )
            return;

		_pScriptManager->registFunctionCommon( registFunction_Guidance );
    }

    void registFunction_Guidance_Instance(sc::LuaInstance& _luaInstance)
    {
        _luaInstance.RegistFunction("UI_Guidance_Auth", InstanceSystem::UI_Guidance_Auth);
        _luaInstance.RegistFunction("UI_Guidance_Progress", InstanceSystem::UI_Guidance_Progress);
        _luaInstance.RegistFunction("UI_Guidance_Progress_All", InstanceSystem::UI_Guidance_Progress_All);
        _luaInstance.RegistFunction("UI_Guidance_Rank", InstanceSystem::UI_Guidance_Rank);
        _luaInstance.RegistFunction("UI_Guidance_Rank_MyInfo", InstanceSystem::UI_Guidance_Rank_MyInfo);
        _luaInstance.RegistFunction("UI_Guidance_MiniMap", InstanceSystem::UI_Guidance_MiniMap);
        _luaInstance.RegistFunction("UI_Guidance_MiniMap_All", InstanceSystem::UI_Guidance_MiniMap_All);
        _luaInstance.RegistFunction("UI_CompetitionResult_Guidance_Begin", InstanceSystem::UI_CompetitionResult_Guidance_Begin);
        _luaInstance.RegistFunction("UI_CompetitionResult_Guidance_Info", InstanceSystem::UI_CompetitionResult_Guidance_Info);
        _luaInstance.RegistFunction("UI_CompetitionResult_Guidance_End", InstanceSystem::UI_CompetitionResult_Guidance_End);
        _luaInstance.RegistFunction("Log_Guidance_In", InstanceSystem::Log_Guidance_In);
        _luaInstance.RegistFunction("Log_Guidance_Out", InstanceSystem::Log_Guidance_Out);
        _luaInstance.RegistFunction("Log_Guidance_Result", InstanceSystem::Log_Guidance_Result);
    }

    void registFunction_Guidance_Instance(InstanceSystem::ScriptManager* const _pScriptManager)
    {
        if ( _pScriptManager == NULL )
            return;

		_pScriptManager->registFunctionCommon( registFunction_Guidance_Instance );
    }

    void registFunction_Guidance_Agent(sc::LuaInstance& _luaInstance)
    {
        _luaInstance.RegistFunction("DisableOldGuidance", InstanceSystem::DisableOldGuidance);
        _luaInstance.RegistFunction("SetGuidanceClub", InstanceSystem::SetGuidanceClub);
    }

    void registFunction_Guidance_Agent(InstanceSystem::ScriptManager* const _pScriptManager)
    {
        if ( _pScriptManager == NULL )
            return;

		_pScriptManager->registFunctionCommon( registFunction_Guidance_Agent );
    }

    int DisableOldGuidance(lua_State* pState)
    {
        int currentStack(1);
        bool _bDisable(true);
        if ( lua_isboolean(pState, currentStack) )
            _bDisable = lua_toboolean(pState, currentStack++);

        GLGuidanceAgentMan* _pGuidanceMan = gpAgentServer->Guidance();
        if ( _pGuidanceMan )
            _pGuidanceMan->DisableOldGuidance(_bDisable);

        return 0;
    }

    int SetGuidanceClub(lua_State* pState)
    {
        int currentStack(1);
        const DWORD _dwGuidanceID((DWORD)lua_tonumber(pState, currentStack++));
        const DWORD _dwClubID((DWORD)lua_tonumber(pState, currentStack++));

		bool bReturn = true;
        GLGuidanceAgentMan* _pGuidanceMan = gpAgentServer->Guidance();
        if ( _pGuidanceMan )
			bReturn = _pGuidanceMan->ChangeGuidClub_RenewalGuidance(_dwGuidanceID, _dwClubID);

		lua_pushboolean(pState, bReturn);

        return 1;
    }

    int UI_Guidance_Auth(lua_State* pState)
    {
        int currentStack(1);
        const DWORD _CharDbNum((DWORD)lua_tonumber(pState, currentStack++));
        const bool _bVisible(lua_toboolean(pState, currentStack++));
        const float _fAuthTime((float)lua_tonumber(pState, currentStack++));
        //const int _iResult(lua_tointeger(pState, currentStack++));

        GLChar* _pChar = gpGaeaServer->GetCharByDbNum(_CharDbNum);
        if ( _pChar == NULL )
            return 0;

        GLMSG::NET_INSTANCE_GUIDANCE_UI_AUTH_FC netMsg;
        netMsg.bVisible = _bVisible;
        netMsg.fAuthTime = _fAuthTime;

        gpGaeaServer->SENDTOCLIENT(_pChar->ClientSlot(), &netMsg);

        
        return 0;
    }

    int UI_Guidance_Progress(lua_State* pState)  // 선도전 진행 UI;
    {
        int currentStack(1);
        const DWORD _CharDbNum((DWORD)lua_tonumber(pState, currentStack++));
        const int _iNameIndex(lua_tointeger(pState, currentStack++));
        const int _iProgressIndex(lua_tointeger(pState, currentStack++));
        const int _iPoint(lua_tointeger(pState, currentStack++));
        const float _fRemainTime((float)lua_tonumber(pState, currentStack++));

        GLChar* _pChar = gpGaeaServer->GetCharByDbNum(_CharDbNum);
        if ( _pChar == NULL )
            return 0;

        GLMSG::NET_INSTANCE_GUIDANCE_UI_PROGRESS_FC netMsg; 
        netMsg.iNameIndex = _iNameIndex;
        netMsg.iProgressIndex = _iProgressIndex;
        netMsg.iPoint = _iPoint;
        netMsg.fRemainTime = _fRemainTime;

        gpGaeaServer->SENDTOCLIENT(_pChar->ClientSlot(), &netMsg);

        return 0;
    }

    int UI_Guidance_Progress_All(lua_State* pState)  // 선도전 진행 UI;
    {
        int currentStack(1);
        lua_getglobal(pState, "instanceMapID");
        const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));
        lua_pop(pState, 1);

        GLLandManager* const _pLandMan = gpGaeaServer->GetLandManager(_instanceMapID);
        if ( _pLandMan == NULL )
            return 0;

        const int _iNameIndex(lua_tointeger(pState, currentStack++));
        const int _iProgressIndex(lua_tointeger(pState, currentStack++));
        const int _iPoint(lua_tointeger(pState, currentStack++));
        const float _fRemainTime((float)lua_tonumber(pState, currentStack++));

        GLMSG::NET_INSTANCE_GUIDANCE_UI_PROGRESS_FC netMsg; 
        netMsg.iNameIndex = _iNameIndex;
        netMsg.iProgressIndex = _iProgressIndex;
        netMsg.iPoint = _iPoint;
        netMsg.fRemainTime = _fRemainTime;

        _pLandMan->sendMsgPC(&netMsg);
        return 0;
    }

    int UI_Guidance_Rank(lua_State* pState)  // 선도전 순위 UI;
    {
        int currentStack(1);

        lua_getglobal(pState, "instanceMapID");
        const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));
        lua_pop(pState, 1);

        GLLandManager* const _pLandMan = gpGaeaServer->GetLandManager(_instanceMapID);
        if ( _pLandMan == NULL )
            return 0;

        LuaPlus::LuaObject _ClubTable(LuaPlus::LuaState::CastState(pState), currentStack++);
        if ( _ClubTable.IsTable() == false )
            return false;

        GLMSG::NET_INSTANCE_GUIDANCE_UI_RANK_FC netMsg;

        const int MAX_SHOW_RANK = 5; // 최대 표시 순위 (현재 5위까지 );
        int i = 0;
        for ( LuaPlus::LuaTableIterator _iter(_ClubTable); _iter && i < MAX_SHOW_RANK; _iter.Next(), i++ )
        {
            LuaPlus::LuaObject _valueObj = _iter.GetValue();
            if ( _valueObj.IsTable() == false )
                return 0;

            LuaPlus::LuaTableIterator _dataIter(_valueObj);
            LuaPlus::LuaObject _clubObj = _dataIter.GetValue();
            if ( _clubObj.IsInteger() == false )
                return 0;
            int _dwClubDbNum = (DWORD)_clubObj.GetNumber();

            if ( _dataIter.Next() == false )
                return 0;

            LuaPlus::LuaObject _pointObj = _dataIter.GetValue();
            if ( _pointObj.IsInteger() == false )
                return 0;
            int _iClubPoint = _pointObj.GetInteger();

            if ( _dataIter.Next() == false )
                return 0;

            LuaPlus::LuaObject _rankObj = _dataIter.GetValue();
            if ( _rankObj.IsInteger() == false )
                return 0;
            int _iClubRank = _rankObj.GetInteger();

            std::tr1::shared_ptr<GLClubField> spClub = gpGaeaServer->GetClub(_dwClubDbNum);
            if ( spClub == NULL )
                continue;

            GLMSG::NET_INSTANCE_GUIDANCE_UI_RANK_FC::RankInfo _rankinfo;
            _rankinfo.clubName = spClub->Name();
            _rankinfo.iPoint = _iClubPoint;
            _rankinfo.iRank = _iClubRank;

            netMsg.rankVector.push_back(_rankinfo);
        }
        msgpack::sbuffer buf;
        msgpack::pack(buf, netMsg);
        _pLandMan->sendMsgPC(NET_MSG_GCTRL_INSTANCE_CONTENTS_MSGPACK, buf);
        return 0;
    }



    int UI_Guidance_Rank_MyInfo(lua_State* pState)  // 선도전 순위 UI;
    {
        int currentStack(1);

        lua_getglobal(pState, "instanceMapID");
        const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));
        lua_pop(pState, 1);

        GLLandManager* const _pLandMan = gpGaeaServer->GetLandManager(_instanceMapID);
        if ( _pLandMan == NULL )
            return 0;

        const int _factionType((DWORD)lua_tonumber(pState, currentStack++));
        const int _factionID((DWORD)lua_tonumber(pState, currentStack++));
        const int _iPoint(lua_tointeger(pState, currentStack++));
        const int _iRank(lua_tointeger(pState, currentStack++));

        GLMSG::NET_INSTANCE_GUIDANCE_UI_RANK_MY_INFO_FC netMsg;
        netMsg.iMyClubPoint = _iPoint;
        netMsg.iMyClubRank = _iRank;

        _pLandMan->sendMsgFaction(Faction::FactionID(_factionType, _factionID), &netMsg);
        return 0;
    }

    const bool Make_Guidance_MiniMap_Message(lua_State* pState, int& _currentStack, msgpack::sbuffer& buf)  // 중복 코드 방지용;
    {
        // Lua쪽으로부터 정의된 형태의 인증기정보Table과 가속기정보Table을 받는것을 전제로 하고 있다.;
        // 인증기 정보 테이블 : { 인증클럽ID, 인증기 X좌표, 인증기 Y좌표 };
        // 가속기 정보 테이블 : { 가속기활성화 여부, 가속기 X좌표, 가속기 X좌표 };

        const WORD _wMapMID((WORD)lua_tonumber(pState, _currentStack++));
        const WORD _wMapSID((WORD)lua_tonumber(pState, _currentStack++));

        GLMSG::NET_INSTANCE_GUIDANCE_UI_MINIMAP_FC netMsg;
        netMsg.dwGuidanceMapID = SNATIVEID(_wMapMID, _wMapSID).dwID;

        LuaPlus::LuaObject AuthInfoTable(LuaPlus::LuaState::CastState(pState), _currentStack++);
        if ( AuthInfoTable.IsTable() )
        {
            for ( LuaPlus::LuaTableIterator _iter(AuthInfoTable); _iter; _iter.Next())
            {
                LuaPlus::LuaObject _valueObj = _iter.GetValue();
                if ( _valueObj.IsTable() == false )
                    return false;

                LuaPlus::LuaTableIterator _dataIter(_valueObj);
                LuaPlus::LuaObject _clubObj = _dataIter.GetValue();
                if ( _clubObj.IsNumber() == false )
                    return false;
                DWORD _dwClubDbNum = (DWORD)_clubObj.GetNumber();

                if ( _dataIter.Next() == false )
                    return false;

                LuaPlus::LuaObject _xPosObj = _dataIter.GetValue();
                if ( _xPosObj.IsNumber() == false )
                    return false;
                float _iXpos = (float)_xPosObj.GetNumber();

                if ( _dataIter.Next() == false )
                    return false;

                LuaPlus::LuaObject _yPosObj = _dataIter.GetValue();
                if ( _yPosObj.IsNumber() == false )
                    return false;
                float _iYpos = (float)_yPosObj.GetNumber();

                GLMSG::NET_INSTANCE_GUIDANCE_UI_MINIMAP_FC::AuthClubInfo _authClubInfo;
                _authClubInfo.clubID = _dwClubDbNum;
                _authClubInfo.fXpos = _iXpos;
                _authClubInfo.fZpos = _iYpos;

                netMsg.vecAuthClub.push_back(_authClubInfo);
            }
        }
        
        // 인증 가속기 현황표시;
        LuaPlus::LuaObject AcceleratorInfoTable(LuaPlus::LuaState::CastState(pState), _currentStack++);
        if ( AcceleratorInfoTable.IsTable() )
        {
            for ( LuaPlus::LuaTableIterator _iter(AcceleratorInfoTable); _iter; _iter.Next())
            {
                LuaPlus::LuaObject _valueObj = _iter.GetValue();
                if ( _valueObj.IsTable() == false )
                    return false;

                LuaPlus::LuaTableIterator _dataIter(_valueObj);
                LuaPlus::LuaObject _clubObj = _dataIter.GetValue();
                if ( _clubObj.IsBoolean() == false )
                    return false;

                const bool _bAcivateAccelerator = _clubObj.GetBoolean();
                if ( _bAcivateAccelerator == false )
                    continue;

                if ( _dataIter.Next() == false )
                    return false;

                LuaPlus::LuaObject _xPosObj = _dataIter.GetValue();
                if ( _xPosObj.IsNumber() == false )
                    return false;
                float _iXpos = (float)_xPosObj.GetNumber();

                if ( _dataIter.Next() == false )
                    return false;

                LuaPlus::LuaObject _yPosObj = _dataIter.GetValue();
                if ( _yPosObj.IsNumber() == false )
                    return false;
                float _iYpos = (float)_yPosObj.GetNumber();

                GLMSG::NET_INSTANCE_GUIDANCE_UI_MINIMAP_FC::AcceleratorInfo _AcceleratorInfo;
                _AcceleratorInfo.fXpos = _iXpos;
                _AcceleratorInfo.fZpos = _iYpos;

                netMsg.vecAccelerator.push_back(_AcceleratorInfo);
            }
        }

		const bool bAllianceClub = lua_toboolean( pState, _currentStack++ );

		netMsg.bAllianceClub = bAllianceClub;

        msgpack::pack(buf, netMsg);
        return true;
    }

    int UI_Guidance_MiniMap(lua_State* pState)  // 선도전 순위 UI 내 정보 패킷;
    {
        int currentStack(1);
        msgpack::sbuffer buf;

        const DWORD _CharDbNum((DWORD)lua_tonumber(pState, currentStack++));
        GLChar* _pChar = gpGaeaServer->GetCharByDbNum(_CharDbNum);
        if ( _pChar == NULL )
            return 0;

        if ( Make_Guidance_MiniMap_Message(pState, currentStack, buf) == false )
            return 0;
        gpGaeaServer->SENDTOCLIENT(_pChar->ClientSlot(), NET_MSG_GCTRL_INSTANCE_CONTENTS_MSGPACK, buf);
        return 0;
    }

    int UI_Guidance_MiniMap_All(lua_State* pState)  // 선도전 순위 UI 내 정보 패킷;
    {
        int currentStack(1);

        lua_getglobal(pState, "instanceMapID");
        const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));
        lua_pop(pState, 1);

        GLLandManager* const _pLandMan = gpGaeaServer->GetLandManager(_instanceMapID);
        if ( _pLandMan == NULL )
            return 0;

        msgpack::sbuffer buf;
        if ( Make_Guidance_MiniMap_Message(pState, currentStack, buf) == false )
            return 0;

        _pLandMan->sendMsgPC(NET_MSG_GCTRL_INSTANCE_CONTENTS_MSGPACK, buf);
        return 0;
    }












    int UI_CompetitionResult_Guidance_Begin(lua_State* pState)
    {
        lua_getglobal(pState, "instanceMapID");
        const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));
        lua_pop(pState, 1);

        GLLandManager* const _pLandMan = gpGaeaServer->GetLandManager(_instanceMapID);
        if ( _pLandMan == NULL )
            return 0;

        GLMSG::NET_INSTANCE_COMPETITION_RESULT_TAB_FC netMsg;
		netMsg.typeCompetition = COMPETITION::COMPETITION_GUIDANCE;

        netMsg.sTabInfo.strTabName = ID2SERVERTEXT("RN_COMPETITION_RESULT_TAB",0); // "전투 결과";
        netMsg.sTabInfo.emSortType = RnCompetitionResultTabInfo::EM_RESULT_SOR_ASC;
        netMsg.sTabInfo.wSortTitleNum = 0;
        netMsg.sTabInfo.emMyResultType = RnCompetitionResultTabInfo::EM_MY_RESULT_CLUB;

        netMsg.sTabInfo.sTitle.AddObj(RnCompetitionResultTitleObj::EM_CONTENT_INT, 34, ID2SERVERTEXT("RN_COMPETITION_RESULT_TITLE",0));
        netMsg.sTabInfo.sTitle.AddObj(RnCompetitionResultTitleObj::EM_CONTENT_ICON_CLUBMARK, 34, ID2SERVERTEXT("RN_COMPETITION_RESULT_TITLE",15));
        netMsg.sTabInfo.sTitle.AddObj(RnCompetitionResultTitleObj::EM_CONTENT_STRING, 110, ID2SERVERTEXT("RN_COMPETITION_RESULT_TITLE",4));
        netMsg.sTabInfo.sTitle.AddObj(RnCompetitionResultTitleObj::EM_CONTENT_STRING, 110, ID2SERVERTEXT("RN_COMPETITION_RESULT_TITLE",14));
        netMsg.sTabInfo.sTitle.AddObj(RnCompetitionResultTitleObj::EM_CONTENT_INT, 64, ID2SERVERTEXT("RN_COMPETITION_RESULT_TITLE",16));
        netMsg.sTabInfo.sTitle.AddObj(RnCompetitionResultTitleObj::EM_CONTENT_INT, 64, ID2SERVERTEXT("RN_COMPETITION_RESULT_TITLE",17));
        netMsg.sTabInfo.sTitle.AddObj(RnCompetitionResultTitleObj::EM_CONTENT_INT, 64, ID2SERVERTEXT("RN_COMPETITION_RESULT_TITLE",18));

        msgpack::sbuffer buf;
        msgpack::pack(buf, netMsg);

        _pLandMan->sendMsgPC(NET_MSG_GCTRL_INSTANCE_CONTENTS_MSGPACK, buf);
        return 0;
    }

    int UI_CompetitionResult_Guidance_Info(lua_State* pState)
    {
        int currentStack(1);

        lua_getglobal(pState, "instanceMapID");
        const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));
        lua_pop(pState, 1);

        GLLandManager* const _pLandMan = gpGaeaServer->GetLandManager(_instanceMapID);
        if ( _pLandMan == NULL )
            return 0;

        GLMSG::NET_INSTANCE_COMPETITION_RESULT_INFO_FC netMsg;
		netMsg.typeCompetition = COMPETITION::COMPETITION_GUIDANCE;
        netMsg.nTabNumber = 0;

        const int _keyID((DWORD)lua_tonumber(pState, currentStack++));
        netMsg.sContents.dwKeyID = _keyID;

        netMsg.sContents.AddContent(lua_tointeger(pState, currentStack++));
        netMsg.sContents.AddContent(lua_tointeger(pState, currentStack++));
        netMsg.sContents.AddContent(sc::GetStringToLuaState(pState, currentStack++));
        netMsg.sContents.AddContent(sc::GetStringToLuaState(pState, currentStack++));
        netMsg.sContents.AddContent(lua_tointeger(pState, currentStack++));
        netMsg.sContents.AddContent(lua_tointeger(pState, currentStack++));
        netMsg.sContents.AddContent(lua_tointeger(pState, currentStack++));       

        msgpack::sbuffer buf;
        msgpack::pack(buf, netMsg);

        _pLandMan->sendMsgPC(NET_MSG_GCTRL_INSTANCE_CONTENTS_MSGPACK, buf);
        return 0;
    }

    int UI_CompetitionResult_Guidance_End(lua_State* pState)
    {
        int currentStack(1);
        const float _OutTime((float)lua_tonumber(pState, currentStack++));  // 강제 나가기 시간 (실제 강제 나가기는 따로 구현해야함. 단순 시간표시용);
        const DWORD _CloseButtonParam((DWORD)lua_tonumber(pState, currentStack++));  // 나가기 버튼 클릭시 발생하는 커스텀 메시지 Param값;

        lua_getglobal(pState, "instanceMapID");
        const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));
        lua_pop(pState, 1);

        GLLandManager* const _pLandMan = gpGaeaServer->GetLandManager(_instanceMapID);
        if ( _pLandMan == NULL )
            return 0;

        GLMSG::NET_INSTANCE_COMPETITION_RESULT_END_INFO_FC netMsg;
		netMsg.typeCompetition = COMPETITION::COMPETITION_GUIDANCE;
        netMsg.fOutTime = _OutTime;
        netMsg.nCloseButtonParam = _CloseButtonParam;

        msgpack::sbuffer buf;
        msgpack::pack(buf, netMsg);

        _pLandMan->sendMsgPC(NET_MSG_GCTRL_INSTANCE_CONTENTS_MSGPACK, buf);
        return 0;
    }


    // reward
    int Log_Guidance_In(lua_State* pState)
    {
        int currentStack(1);
        const char* _strGuidanceName(NULL);
		_strGuidanceName = sc::GetStringToLuaState(pState, currentStack++);  // 클럽 DbNum;

        const DWORD _charDbNum((DWORD)lua_tonumber(pState, currentStack++));  // 케릭터 DbNum;
        const DWORD _clubDbNum((DWORD)lua_tonumber(pState, currentStack++));  // 클럽 DbNum;
		const int nPoint(lua_tointeger(pState, currentStack++));  // 포인트

        lua_getglobal(pState, "instanceMapID");
        const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));
        lua_pop(pState, 1);

        GLLandManager* const _pLandMan = gpGaeaServer->GetLandManager(_instanceMapID);
        if ( _pLandMan == NULL )
            return 0;

        db::LogInstantGuidance* pDbAction = new db::LogInstantGuidance(
            db::LogInstantGuidance::EM_IN,
            _pLandMan->getGUID());

        pDbAction->strGuidanceName = _strGuidanceName;

        pDbAction->dwCharDbNum = _charDbNum;
        pDbAction->dwClubDbNum = _clubDbNum;
		pDbAction->nClubPoint = nPoint;

        gpGaeaServer->AddLogAdoJob( db::DbActionPtr(pDbAction) );

        return 0;
    }

    int Log_Guidance_Out(lua_State* pState)
    {
        int currentStack(1);
        const char* _strGuidanceName(NULL);
		_strGuidanceName = sc::GetStringToLuaState(pState, currentStack++);  // 클럽 DbNum;

        const DWORD _charDbNum((DWORD)lua_tonumber(pState, currentStack++));  // 케릭터 DbNum;
        const DWORD _clubDbNum((DWORD)lua_tonumber(pState, currentStack++));  // 클럽 DbNum;
		const int nPoint(lua_tointeger(pState, currentStack++));  // 포인트

        lua_getglobal(pState, "instanceMapID");
        const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));
        lua_pop(pState, 1);

        GLLandManager* const _pLandMan = gpGaeaServer->GetLandManager(_instanceMapID);
        if ( _pLandMan == NULL )
            return 0;

        db::LogInstantGuidance* pDbAction = new db::LogInstantGuidance(
            db::LogInstantGuidance::EM_OUT,
            _pLandMan->getGUID());

        pDbAction->strGuidanceName = _strGuidanceName;

        pDbAction->dwCharDbNum = _charDbNum;
        pDbAction->dwClubDbNum = _clubDbNum;
		pDbAction->nClubPoint = nPoint;

        gpGaeaServer->AddLogAdoJob( db::DbActionPtr(pDbAction) );

        return 0;
    }

    int Log_Guidance_Result(lua_State* pState)
    {
        int currentStack(1);

        const char* _strGuidanceName(NULL);
		_strGuidanceName = sc::GetStringToLuaState(pState, currentStack++);  // 클럽 DbNum;

        const DWORD _clubDbNum((DWORD)lua_tonumber(pState, currentStack++));  // 클럽 DbNum;
        const int _clubPoint(lua_tointeger(pState, currentStack++));  // 클럽 DbNum;
        const int _clubHaveAuthNum(lua_tointeger(pState, currentStack++));  // 클럽 DbNum;
        const int _clubAuthCount(lua_tointeger(pState, currentStack++));  // 클럽 DbNum;

        const DWORD _secondClubDbNum((DWORD)lua_tonumber(pState, currentStack++));  // 클럽 DbNum;
        const int _secondClubPoint(lua_tointeger(pState, currentStack++));  // 클럽 DbNum;
        const int _secondClubHaveAuthNum(lua_tointeger(pState, currentStack++));  // 클럽 DbNum;
        const int _secondClubAuthCount(lua_tointeger(pState, currentStack++));  // 클럽 DbNum;

        const DWORD nThirdClubDbNum((DWORD)lua_tonumber(pState, currentStack++));  // 클럽 DbNum;
        const int nThirdClubPoint(lua_tointeger(pState, currentStack++));  // 클럽 DbNum;
        const int nThirdClubHaveAuthNum(lua_tointeger(pState, currentStack++));  // 클럽 DbNum;
        const int nThirdClubAuthCount(lua_tointeger(pState, currentStack++));  // 클럽 DbNum;

        lua_getglobal(pState, "instanceMapID");
        const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));
        lua_pop(pState, 1);

        GLLandManager* const _pLandMan = gpGaeaServer->GetLandManager(_instanceMapID);
        if ( _pLandMan == NULL )
            return 0;

        db::LogInstantGuidance* pDbAction = new db::LogInstantGuidance(
            db::LogInstantGuidance::EM_RESULT,
            _pLandMan->getGUID());

        pDbAction->strGuidanceName = _strGuidanceName;

        pDbAction->dwClubDbNum = _clubDbNum;
        pDbAction->nClubPoint = _clubPoint;
        pDbAction->nClubHaveAuthNum = _clubHaveAuthNum;
        pDbAction->nClubAuthCount = _clubAuthCount;

        pDbAction->dwSecondClubDbNum = _secondClubDbNum;
        pDbAction->nSecondClubPoint = _secondClubPoint;
        pDbAction->nSecondClubHaveAuthNum = _secondClubHaveAuthNum;
        pDbAction->nSecondClubAuthCount = _secondClubAuthCount;

        pDbAction->dwThirdClubDbNum = nThirdClubDbNum;
        pDbAction->nThirdClubPoint = nThirdClubPoint;
        pDbAction->nThirdClubHaveAuthNum = nThirdClubHaveAuthNum;
        pDbAction->nThirdClubAuthCount = nThirdClubAuthCount;

        gpGaeaServer->AddLogAdoJob( db::DbActionPtr(pDbAction) );

        return 0;
    }

}