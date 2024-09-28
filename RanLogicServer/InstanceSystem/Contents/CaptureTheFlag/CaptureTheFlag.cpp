#include "pch.h"

#include "../../../FieldServer/GLGaeaServer.h"
#include "../../InstanceSystem.h"
#include "../../LogicField/InstanceSystemField.h"
#include "../../../Database/DBAction/DbActionLogInstantDungeon.h"
#include "../../../../EngineLib/GUInterface/GameTextControl.h"

#include "CaptureTheFlag.h"

namespace InstanceSystem
{
    void registFunction_CaptureTheFlag(sc::LuaInstance& _luaInstance)
    {
    }

    void registFunction_CaptureTheFlag(InstanceSystem::ScriptManager* const _pScriptManager)
    {
        if ( _pScriptManager == NULL )
            return;

		_pScriptManager->registFunctionCommon( registFunction_CaptureTheFlag );
    }

    void registFunction_CaptureTheFlag_Instance(sc::LuaInstance& _luaInstance)
    {
		_luaInstance.RegistFunction("UI_Navagation_On", InstanceSystem::UI_Navagation_On);
		_luaInstance.RegistFunction("UI_Navagation_Off", InstanceSystem::UI_Navagation_Off);
        _luaInstance.RegistFunction("UI_BlueFlagGuideArrow_Target", InstanceSystem::UI_BlueFlagGuideArrow_Target);
        _luaInstance.RegistFunction("UI_RedFlagGuideArrow_Target", InstanceSystem::UI_RedFlagGuideArrow_Target);
        _luaInstance.RegistFunction("UI_BlueFlagGuideArrow_Off", InstanceSystem::UI_BlueFlagGuideArrow_Off);
        _luaInstance.RegistFunction("UI_RedFlagGuideArrow_Off", InstanceSystem::UI_RedFlagGuideArrow_Off);
        _luaInstance.RegistFunction("UI_FlagGuideArrow_Off", InstanceSystem::UI_FlagGuideArrow_Off);
		_luaInstance.RegistFunction("UI_FlagGuideArrow_Off_User", InstanceSystem::UI_FlagGuideArrow_Off_User);
        _luaInstance.RegistFunction("UI_CompetitionResult_CaptureTheFlag_Begin", InstanceSystem::UI_CompetitionResult_CaptureTheFlag_Begin);
        _luaInstance.RegistFunction("UI_CompetitionResult_CaptureTheFlag_Info", InstanceSystem::UI_CompetitionResult_CaptureTheFlag_Info);
        _luaInstance.RegistFunction("UI_CompetitionResult_CaptureTheFlag_End", InstanceSystem::UI_CompetitionResult_CaptureTheFlag_End);

        // competition log
        _luaInstance.RegistFunction("Log_CaptureTheFlag_In", InstanceSystem::Log_CaptureTheFlag_In);
        _luaInstance.RegistFunction("Log_CaptureTheFlag_Out", InstanceSystem::Log_CaptureTheFlag_Out);
        _luaInstance.RegistFunction("Log_CaptureTheFlag_Result", InstanceSystem::Log_CaptureTheFlag_Result);
        _luaInstance.RegistFunction("Log_CaptureTheFlag_Reward", InstanceSystem::Log_CaptureTheFlag_Reward);        
    }

    void registFunction_CaptureTheFlag_Instance(InstanceSystem::ScriptManager* const _pScriptManager)
    {
        if ( _pScriptManager == NULL )
            return;

		_pScriptManager->registFunctionCommon( registFunction_CaptureTheFlag_Instance );
    }

    void registFunction_CaptureTheFlag_Agent(sc::LuaInstance& _luaInstance)
    {
        // competition log
        _luaInstance.RegistFunction("Log_CaptureTheFlag_Rating", InstanceSystem::Log_CaptureTheFlag_Rating_Agent);
    }

    void registFunction_CaptureTheFlag_Agent(InstanceSystem::ScriptManager* const _pScriptManager)
    {
        if ( _pScriptManager == NULL )
            return;

        _pScriptManager->registFunctionCommon( registFunction_CaptureTheFlag_Agent );
    }












	int UI_Navagation_On ( lua_State* pState )
	{
		int nCurrentStack = 1;

		const DWORD dwChaDBNum = static_cast< DWORD >( lua_tonumber( pState, nCurrentStack++ ) );

		GLChar* pChar = gpGaeaServer->GetCharByDbNum( dwChaDBNum );
		if ( NULL == pChar )
			return 0;

		GLMSG::NET_INSTANCE_CTF_FLAG_UPDATE_FC sNetMsg;
		sNetMsg.nFlagType = GLMSG::NET_INSTANCE_CTF_FLAG_UPDATE_FC::EM_NAVIGATION_ON;

		gpGaeaServer->SENDTOCLIENT( pChar->ClientSlot(), &sNetMsg );

		return 0;
	}

	int UI_Navagation_Off ( lua_State* pState )
	{
		int nCurrentStack = 1;

		const DWORD dwChaDBNum = static_cast< DWORD >( lua_tonumber( pState, nCurrentStack++ ) );

		GLChar* pChar = gpGaeaServer->GetCharByDbNum( dwChaDBNum );
		if ( NULL == pChar )
			return 0;

		GLMSG::NET_INSTANCE_CTF_FLAG_UPDATE_FC sNetMsg;
		sNetMsg.nFlagType = GLMSG::NET_INSTANCE_CTF_FLAG_UPDATE_FC::EM_NAVIGATION_OFF;

		gpGaeaServer->SENDTOCLIENT( pChar->ClientSlot(), &sNetMsg );

		return 0;
	}

    int UI_BlueFlagGuideArrow_Target(lua_State* pState)
    {
        int currentStack(1);
        GLMSG::NET_INSTANCE_CTF_FLAG_UPDATE_FC netMsg;

        lua_getglobal(pState, "instanceMapID");
        const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));
        lua_pop(pState, 1);
        GLLandManager* const pLandMan = gpGaeaServer->GetLandManager(_instanceMapID);

        const DWORD dwFactionType = lua_tointeger(pState, currentStack++);
        const DWORD dwFactionID = lua_tointeger(pState, currentStack++);
        const DWORD dwTargetType = lua_tointeger(pState, currentStack++);
        const DWORD dwTargetID = lua_tointeger(pState, currentStack++);
        const bool bBlink = lua_toboolean(pState, currentStack++);

        Faction::GLActorID targetActor(dwTargetType, dwTargetID);
        ServerActor* pActor = pLandMan->getActor(targetActor);
        if (pActor == NULL)
            return 0;

        netMsg.sTargetID.emCrow = (EMCROW)dwTargetType;
        netMsg.sTargetID.GaeaId = pActor->GetGaeaID();
        netMsg.sTargetID.vPos = pActor->GetPosition();
        netMsg.nFlagType = GLMSG::NET_INSTANCE_CTF_FLAG_UPDATE_FC::EM_BLUE_FLAG_VISIBLE_ON;
        netMsg.bBlink = bBlink;

        pLandMan->sendMsgFaction(Faction::FactionID(dwFactionType, dwFactionID), &netMsg);

        return 0;
    }
    int UI_RedFlagGuideArrow_Target(lua_State* pState)
    {
        int currentStack(1);
        GLMSG::NET_INSTANCE_CTF_FLAG_UPDATE_FC netMsg;

        lua_getglobal(pState, "instanceMapID");
        const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));
        lua_pop(pState, 1);
        GLLandManager* const pLandMan = gpGaeaServer->GetLandManager(_instanceMapID);

        const DWORD dwFactionType = lua_tointeger(pState, currentStack++);
        const DWORD dwFactionID = lua_tointeger(pState, currentStack++);
        const DWORD dwTargetType = lua_tointeger(pState, currentStack++);
        const DWORD dwTargetID = lua_tointeger(pState, currentStack++);
        const bool bBlink = lua_toboolean(pState, currentStack++);

        Faction::GLActorID targetActor(dwTargetType, dwTargetID);
        ServerActor* pActor = pLandMan->getActor(targetActor);
        if (pActor == NULL)
            return 0;

        netMsg.sTargetID.emCrow = (EMCROW)dwTargetType;
        netMsg.sTargetID.GaeaId = pActor->GetGaeaID();
		netMsg.sTargetID.vPos = pActor->GetPosition();
        netMsg.nFlagType = GLMSG::NET_INSTANCE_CTF_FLAG_UPDATE_FC::EM_RED_FLAG_VISIBLE_ON;
        netMsg.bBlink = bBlink;

        pLandMan->sendMsgFaction(Faction::FactionID(dwFactionType, dwFactionID), &netMsg);
        
        return 0;
    }

    int UI_BlueFlagGuideArrow_Off(lua_State* pState)
    {
        int currentStack(1);

        GLMSG::NET_INSTANCE_CTF_FLAG_UPDATE_FC netMsg;
        lua_getglobal(pState, "instanceMapID");
        const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));
        lua_pop(pState, 1);
        GLLandManager* const pLandMan = gpGaeaServer->GetLandManager(_instanceMapID);

        const DWORD dwFactionType = lua_tointeger(pState, currentStack++);
        const DWORD dwFactionID = lua_tointeger(pState, currentStack++);

        netMsg.nFlagType = GLMSG::NET_INSTANCE_CTF_FLAG_UPDATE_FC::EM_BLUE_FLAG_VISIBLE_OFF;
        
        pLandMan->sendMsgFaction(Faction::FactionID(dwFactionType, dwFactionID), &netMsg);

        return 0;
    }

    int UI_RedFlagGuideArrow_Off(lua_State* pState)
    {
        int currentStack(1);

        GLMSG::NET_INSTANCE_CTF_FLAG_UPDATE_FC netMsg;
        lua_getglobal(pState, "instanceMapID");
        const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));
        lua_pop(pState, 1);
        GLLandManager* const pLandMan = gpGaeaServer->GetLandManager(_instanceMapID);

        const DWORD dwFactionType = lua_tointeger(pState, currentStack++);
        const DWORD dwFactionID = lua_tointeger(pState, currentStack++);

        netMsg.nFlagType = GLMSG::NET_INSTANCE_CTF_FLAG_UPDATE_FC::EM_RED_FLAG_VISIBLE_OFF;

        pLandMan->sendMsgFaction(Faction::FactionID(dwFactionType, dwFactionID), &netMsg);

        return 0;
    }

    int UI_FlagGuideArrow_Off(lua_State* pState)
    {
        GLMSG::NET_INSTANCE_CTF_FLAG_UPDATE_FC netMsg;
        lua_getglobal(pState, "instanceMapID");
        const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));
        lua_pop(pState, 1);
        GLLandManager* const pLandMan = gpGaeaServer->GetLandManager(_instanceMapID);

        netMsg.nFlagType = GLMSG::NET_INSTANCE_CTF_FLAG_UPDATE_FC::EM_ALL_FLAG_VISIBLE_OFF;

        pLandMan->sendMsgPC(&netMsg);

        return 0;
    }

	int UI_FlagGuideArrow_Off_User ( lua_State* pState )
	{
		int nCurrentStack = 1;

		const DWORD dwChaDBNum = static_cast< DWORD >( lua_tonumber( pState, nCurrentStack++ ) );

		GLChar* pChar = gpGaeaServer->GetCharByDbNum( dwChaDBNum );
		if ( NULL == pChar )
			return 0;

		GLMSG::NET_INSTANCE_CTF_FLAG_UPDATE_FC sNetMsg;
		sNetMsg.nFlagType = GLMSG::NET_INSTANCE_CTF_FLAG_UPDATE_FC::EM_ALL_FLAG_VISIBLE_OFF;

		gpGaeaServer->SENDTOCLIENT( pChar->ClientSlot(), &sNetMsg );

		return 0;
	}

	int UI_CompetitionResult_CaptureTheFlag_Begin(lua_State* pState)
	{
		lua_getglobal(pState, "instanceMapID");
		const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));
		lua_pop(pState, 1);

		GLLandManager* const _pLandMan = gpGaeaServer->GetLandManager(_instanceMapID);
		if ( _pLandMan == NULL )
			return 0;

		GLMSG::NET_INSTANCE_COMPETITION_RESULT_TAB_FC netMsg;
		netMsg.typeCompetition = InstanceSystem::COMPETITION_CTFLAG;

		netMsg.sTabInfo.strTabName = ID2SERVERTEXT("RN_COMPETITION_RESULT_TAB",0); // "전투 결과";
		netMsg.sTabInfo.emSortType = InstanceSystem::RnCompetitionResultTabInfo::EM_RESULT_SOR_ASC;
		netMsg.sTabInfo.wSortTitleNum = 0;
		netMsg.sTabInfo.emMyResultType = InstanceSystem::RnCompetitionResultTabInfo::EM_MY_RESULT_CHAR_DBNUM;

		netMsg.sTabInfo.sTitle.vecTitleObj.push_back(
			InstanceSystem::RnCompetitionResultTitleObj(InstanceSystem::RnCompetitionResultTitleObj::EM_CONTENT_ICON_WINLOSE, // 순위;
			34,
			ID2SERVERTEXT("RN_COMPETITION_RESULT_TITLE",0)));

		netMsg.sTabInfo.sTitle.vecTitleObj.push_back(
			InstanceSystem::RnCompetitionResultTitleObj(InstanceSystem::RnCompetitionResultTitleObj::EM_CONTENT_ICON_SCHOOL, // 학원;
			48,
			ID2SERVERTEXT("RN_COMPETITION_RESULT_TITLE",1)));

		netMsg.sTabInfo.sTitle.vecTitleObj.push_back(
			InstanceSystem::RnCompetitionResultTitleObj(InstanceSystem::RnCompetitionResultTitleObj::EM_CONTENT_ICON_CLASS, // 클래스;
			64,
			ID2SERVERTEXT("RN_COMPETITION_RESULT_TITLE",2)));

		netMsg.sTabInfo.sTitle.vecTitleObj.push_back(
			InstanceSystem::RnCompetitionResultTitleObj(InstanceSystem::RnCompetitionResultTitleObj::EM_CONTENT_STRING, // 케릭터 명;
			110,
			ID2SERVERTEXT("RN_COMPETITION_RESULT_TITLE",3)));

		netMsg.sTabInfo.sTitle.vecTitleObj.push_back(
			InstanceSystem::RnCompetitionResultTitleObj(InstanceSystem::RnCompetitionResultTitleObj::EM_CONTENT_INT, // 깃발 탈취수;
			64,
			ID2SERVERTEXT("RN_COMPETITION_RESULT_TITLE",5)));

		netMsg.sTabInfo.sTitle.vecTitleObj.push_back(
			InstanceSystem::RnCompetitionResultTitleObj(InstanceSystem::RnCompetitionResultTitleObj::EM_CONTENT_INT, // 사살 수;
			64,
			ID2SERVERTEXT("RN_COMPETITION_RESULT_TITLE",8)));

		netMsg.sTabInfo.sTitle.vecTitleObj.push_back(
			InstanceSystem::RnCompetitionResultTitleObj(InstanceSystem::RnCompetitionResultTitleObj::EM_CONTENT_INT, // 사망 수;
			64,
			ID2SERVERTEXT("RN_COMPETITION_RESULT_TITLE",9)));

        netMsg.sTabInfo.sTitle.vecTitleObj.push_back(
            InstanceSystem::RnCompetitionResultTitleObj((int)(InstanceSystem::RnCompetitionResultTitleObj::EM_CONTENT_REWARD |
                                                            InstanceSystem::RnCompetitionResultTitleObj::EM_CONTENT_REWARD_ITEM), // 아이템;
            64,
            ID2SERVERTEXT("RN_COMPETITION_RESULT_TITLE",11)));

		msgpack::sbuffer buf;
		msgpack::pack(buf, netMsg);

		_pLandMan->sendMsgPC(NET_MSG_GCTRL_INSTANCE_CONTENTS_MSGPACK, buf);
		return 0;
	}

	int UI_CompetitionResult_CaptureTheFlag_Info(lua_State* pState)
	{
		int currentStack(1);

		lua_getglobal(pState, "instanceMapID");
		const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));
		lua_pop(pState, 1);

		GLLandManager* const _pLandMan = gpGaeaServer->GetLandManager(_instanceMapID);
		if ( _pLandMan == NULL )
			return 0;

		GLMSG::NET_INSTANCE_COMPETITION_RESULT_INFO_FC netMsg;
		netMsg.typeCompetition = InstanceSystem::COMPETITION_CTFLAG;
		netMsg.nTabNumber = 0;

		netMsg.sContents.dwKeyID = (DWORD)lua_tonumber(pState, currentStack++);  // CharDbNum;
		netMsg.sContents.vecResultObj.push_back( InstanceSystem::RnCompetitionResultContentObj(lua_tointeger(pState, currentStack++)) );	// Win,Lose;
		netMsg.sContents.vecResultObj.push_back( InstanceSystem::RnCompetitionResultContentObj(lua_tointeger(pState, currentStack++)) );	// 학원;
		netMsg.sContents.vecResultObj.push_back( InstanceSystem::RnCompetitionResultContentObj(lua_tointeger(pState, currentStack++)) );	// 클래스;
		netMsg.sContents.vecResultObj.push_back( InstanceSystem::RnCompetitionResultContentObj(sc::GetStringToLuaState(pState, currentStack++)) );  // 케릭터 명;
		netMsg.sContents.vecResultObj.push_back( InstanceSystem::RnCompetitionResultContentObj(lua_tointeger(pState, currentStack++)) );	// 깃발 탈취수;
		netMsg.sContents.vecResultObj.push_back( InstanceSystem::RnCompetitionResultContentObj(lua_tointeger(pState, currentStack++)) );	// 사살 수;
		netMsg.sContents.vecResultObj.push_back( InstanceSystem::RnCompetitionResultContentObj(lua_tointeger(pState, currentStack++)) );	// 사망 수;

		// 보상 아이템;
		LuaPlus::LuaObject rewardData(
			LuaPlus::LuaState::CastState(pState),
			currentStack++ );
		InstanceSystem::RnCompetitionResultContentRewardObj rewardObj;

		if ( rewardData.IsTable() )
		{
			for ( LuaPlus::LuaTableIterator _ItemIterator( rewardData );
				_ItemIterator; _ItemIterator.Next() )
			{
				LuaPlus::LuaObject _ItemIDData = _ItemIterator.GetValue();
				if ( _ItemIDData.IsTable() )
				{
					InstanceSystem::RnCompetitionResultContentRewardObj::RewardItem _rewardItem;

					LuaPlus::LuaTableIterator _IDiterator( _ItemIDData );
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

					rewardObj.vecItemReward.push_back( _rewardItem );
				}
			}
		}

        //InstanceSystem::RnCompetitionResultContentRewardObj rewardObj;
        //rewardObj.strExpReward = (const char*)lua_tostring(pState, currentStack++);
        netMsg.sContents.vecRewardObj.push_back( InstanceSystem::RnCompetitionResultContentRewardObj(rewardObj) );	// 보상 경험치;

		msgpack::sbuffer buf;
		msgpack::pack(buf, netMsg);

		_pLandMan->sendMsgPC(NET_MSG_GCTRL_INSTANCE_CONTENTS_MSGPACK, buf);
		return 0;
	}

	int UI_CompetitionResult_CaptureTheFlag_End(lua_State* pState)
	{
		int currentStack(1);
		const float _OutTime((float)lua_tonumber(pState, currentStack++));  // 나가기 시간;

		lua_getglobal(pState, "instanceMapID");
		const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));
		lua_pop(pState, 1);

		GLLandManager* const _pLandMan = gpGaeaServer->GetLandManager(_instanceMapID);
		if ( _pLandMan == NULL )
			return 0;
		GLMSG::NET_INSTANCE_COMPETITION_RESULT_END_INFO_FC netMsg;
		netMsg.typeCompetition = InstanceSystem::COMPETITION_CTFLAG;
		netMsg.fOutTime = _OutTime;
		netMsg.nCloseButtonParam = 1;

		msgpack::sbuffer buf;
		msgpack::pack(buf, netMsg);

		_pLandMan->sendMsgPC(NET_MSG_GCTRL_INSTANCE_CONTENTS_MSGPACK, buf);
		return 0;
	}

    int Log_CaptureTheFlag_In(lua_State* pState)
    {
        int currentStack(1);

        lua_getglobal(pState, "instanceMapID");
        const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));
        lua_pop(pState, 1);

        GLLandManager* const _pLandMan = gpGaeaServer->GetLandManager(_instanceMapID);
        if ( _pLandMan == NULL )
            return 0;

        const DWORD _dwCharDbNum = (DWORD)lua_tonumber(pState, currentStack++);
        const int _nTeam = lua_tointeger(pState, currentStack++);
        const int _nRating = lua_tointeger(pState, currentStack++);

        db::LogInstantCaptureTheFlag* pDbAction = new db::LogInstantCaptureTheFlag(
            db::LogInstantCaptureTheFlag::EM_IN,
            _pLandMan->getGUID());

        pDbAction->dwCharDbNum = _dwCharDbNum;
        pDbAction->nTeam = _nTeam;
        pDbAction->nRating = _nRating;

        gpGaeaServer->AddLogAdoJob( db::DbActionPtr(pDbAction) );

        return 0;
    }

    int Log_CaptureTheFlag_Out(lua_State* pState)
    {
        int currentStack(1);

        lua_getglobal(pState, "instanceMapID");
        const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));
        lua_pop(pState, 1);

        GLLandManager* const _pLandMan = gpGaeaServer->GetLandManager(_instanceMapID);
        if ( _pLandMan == NULL )
            return 0;

        const DWORD _dwCharDbNum = (DWORD)lua_tonumber(pState, currentStack++);
        const int _nTeam = lua_tointeger(pState, currentStack++);
        const int _nRating = lua_tointeger(pState, currentStack++);

        db::LogInstantCaptureTheFlag* pDbAction = new db::LogInstantCaptureTheFlag(
            db::LogInstantCaptureTheFlag::EM_OUT,
            _pLandMan->getGUID());

        pDbAction->dwCharDbNum = _dwCharDbNum;
        pDbAction->nTeam = _nTeam;
        pDbAction->nRating = _nRating;

        gpGaeaServer->AddLogAdoJob( db::DbActionPtr(pDbAction) );

        return 0;
    }

    int Log_CaptureTheFlag_Result(lua_State* pState)
    {
        int currentStack(1);

        lua_getglobal(pState, "instanceMapID");
        const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));
        lua_pop(pState, 1);

        GLLandManager* const _pLandMan = gpGaeaServer->GetLandManager(_instanceMapID);
        if ( _pLandMan == NULL )
            return 0;

        const int _nTeam = lua_tointeger(pState, currentStack++);
        const bool _bWin = lua_toboolean(pState, currentStack++);
        const int _nScore = lua_tointeger(pState, currentStack++);
        const int _nKill = lua_tointeger(pState, currentStack++);
        const int _nDeath = lua_tointeger(pState, currentStack++);

        db::LogInstantCaptureTheFlag* pDbAction = new db::LogInstantCaptureTheFlag(
            db::LogInstantCaptureTheFlag::EM_RESULT,
            _pLandMan->getGUID());

        pDbAction->nTeam = _nTeam;
        pDbAction->bResult = _bWin;
        pDbAction->nFlagScore = _nScore;
        pDbAction->nKill = _nKill;
        pDbAction->nDeath = _nDeath;

        gpGaeaServer->AddLogAdoJob( db::DbActionPtr(pDbAction) );

        return 0;
    }

    int Log_CaptureTheFlag_Reward(lua_State* pState)
    {
        int currentStack(1);

        lua_getglobal(pState, "instanceMapID");
        const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));
        lua_pop(pState, 1);

        GLLandManager* const _pLandMan = gpGaeaServer->GetLandManager(_instanceMapID);
        if ( _pLandMan == NULL )
            return 0;

        const DWORD _dwCharDbNum = (DWORD)lua_tonumber(pState, currentStack++);
        const int _nReward = lua_tointeger(pState, currentStack++);

        db::LogInstantCaptureTheFlag* pDbAction = new db::LogInstantCaptureTheFlag(
            db::LogInstantCaptureTheFlag::EM_REWARD,
            _pLandMan->getGUID());

        pDbAction->dwCharDbNum = _dwCharDbNum;
        pDbAction->nReward = _nReward;

        gpGaeaServer->AddLogAdoJob( db::DbActionPtr(pDbAction) );

        return 0;
    }

    int Log_CaptureTheFlag_Rating_Agent(lua_State* pState)
    {
        int currentStack(1);

        const DWORD _dwCharDbNum = (DWORD)lua_tonumber(pState, currentStack++);
        const int _nRating = lua_tointeger(pState, currentStack++);
        const int _nChangedRating = lua_tointeger(pState, currentStack++);
        const int _nWinningStreak = lua_tointeger(pState, currentStack++);

        db::LogInstantCaptureTheFlag* pDbAction = new db::LogInstantCaptureTheFlag(
            db::LogInstantCaptureTheFlag::EM_RATING_CHANGE,
            sc::MGUID());

        pDbAction->dwCharDbNum = _dwCharDbNum;
        pDbAction->nRating = _nRating;
        pDbAction->nChangedRating = _nChangedRating;
        pDbAction->nWinningStreak = _nWinningStreak;

        gpAgentServer->AddLogAdoJob( db::DbActionPtr(pDbAction) );

        return 0;
    }
}