#include "pch.h"

#include "../../../RanLogic/InstanceSystem/GLInstanceSystem.h"
#include "../../../enginelib/GUInterface/GameTextControl.h"
#include "../../../SigmaCore/String/StringUtil.h"
#include "../../../RanLogic/Msg/GLContrlInstanceMsg.h"
#include "../../AgentServer/GLAgentServer.h"
#include "../../Party/GLPartyManagerAgent.h"
#include "../../Club/GLClubAgent.h"
#include "../../Club/GLClubManAgent.h"
#include "../../Character/GLCharAG.h"
#include "../InstanceSystem.h"
#include "InstanceSystemAgent.h"
#include "InvokeFunctionAgent.h"

// matching system
#include "../../MatchSystem/InvokeFunctionAgentMatching.h"

// added Contents
#include "../Contents/ClubDeathMatch/RnClubDeathMatch.h"
#include "../Contents/CaptureTheFlag/CaptureTheFlag.h"
#include "../Contents/GuidanceClubBattle/GuidanceClubBattle.h"
#include "../Contents/CountryBattle/CountryBattle.h"

#include "../InvokeUI/InvokeFunctionUI.h"
#include "../InvokeDBLog/InvokeFunctionDBLog.h"

#include "../../Database/DBAction/DbActionLogInstantDungeon.h"

namespace InstanceSystem
{
	void initializeAgentPublicInvokeFunction(void)
	{
		InstanceSystem::ScriptManager* const _pScriptManager(InstanceSystem::ScriptManager::getInstance());

		registFunctionCommon(_pScriptManager);

		registFunctionAgent(_pScriptManager);

		// 매칭 시스템;
		MatchingSystem::registFunction_AgentMatching(_pScriptManager);

        // 초기화 함수 호출;
        _pScriptManager->pushCallFunction("luaAgentEventInitialize");
	}	

    void registFunctionAgent(InstanceSystem::ScriptManager* const _pScriptManager)
    {
		if ( NULL == _pScriptManager )
			return;

		_pScriptManager->registFunctionCommon( registFunctionAgent );
    }

    void registFunctionAgent(sc::LuaInstance& _luaInstance)
    {
        {  // regist fucnction for added contents;
            registFunction_CDM_Agent(_luaInstance);
            registFunction_CaptureTheFlag_Agent(_luaInstance);
            registFunction_Guidance_Agent(_luaInstance);
			registFunction_CTI_Agent( _luaInstance );

			RegistInvokeFunction_UI_Agent( _luaInstance );
			RegistInvokeFunction_DBLog_Agent( _luaInstance );
        }

        { // isXXX;
            _luaInstance.RegistFunction("IsParty", InstanceSystem::AgentScriptFunction::isParty);
            _luaInstance.RegistFunction("IsExpedition", InstanceSystem::AgentScriptFunction::isExpedition);
            _luaInstance.RegistFunction("IsPartyMaster", InstanceSystem::AgentScriptFunction::isPartyMaster);
        }

        { // getXXX;
            _luaInstance.RegistFunction("GetLevel", InstanceSystem::AgentScriptFunction::GetLevel);
            _luaInstance.RegistFunction("GetName", InstanceSystem::AgentScriptFunction::GetName);
            _luaInstance.RegistFunction("GetSchool", InstanceSystem::AgentScriptFunction::GetSchool);
            _luaInstance.RegistFunction("GetClubID", InstanceSystem::AgentScriptFunction::GetClubID);
            _luaInstance.RegistFunction("GetClubName", InstanceSystem::AgentScriptFunction::GetClubName);
            _luaInstance.RegistFunction("GetClubMasterName", InstanceSystem::AgentScriptFunction::GetClubMasterName);
            _luaInstance.RegistFunction("GetClubRank", InstanceSystem::AgentScriptFunction::GetClubRank);
            _luaInstance.RegistFunction("GetParty", InstanceSystem::AgentScriptFunction::GetParty);
            _luaInstance.RegistFunction("GetPartyID", InstanceSystem::AgentScriptFunction::GetPartyID);
            _luaInstance.RegistFunction("GetPartyMember", InstanceSystem::AgentScriptFunction::GetPartyMember);
            _luaInstance.RegistFunction("GetPartyMemberNum", InstanceSystem::AgentScriptFunction::GetPartyMemberNum);
            _luaInstance.RegistFunction("GetPartyMaster", InstanceSystem::AgentScriptFunction::GetPartyMaster);
			_luaInstance.RegistFunction("GetBaseMapID", InstanceSystem::AgentScriptFunction::GetBaseMapID);
			_luaInstance.RegistFunction("GetPosFromMapPos", InstanceSystem::AgentScriptFunction::GetPosFromMapPos);
			_luaInstance.RegistFunction("GetMapPosFromPos", InstanceSystem::AgentScriptFunction::GetMapPosFromPos);
			_luaInstance.RegistFunction("GetCountry", InstanceSystem::AgentScriptFunction::GetCountry);
			_luaInstance.RegistFunction("GetCountryName", InstanceSystem::AgentScriptFunction::GetCountryName);
        }

        { // setXXX;            
            _luaInstance.RegistFunction("SetPublicJoinLock", InstanceSystem::AgentScriptFunction::setPublicJoinLock);
        }

        { // doXXX;
            _luaInstance.RegistFunction("DoCreateInstance", InstanceSystem::AgentScriptFunction::doCreateInstance);
            _luaInstance.RegistFunction("DoDestroyInstance", InstanceSystem::AgentScriptFunction::doDestroyInstance);
        }
        { // party
            _luaInstance.RegistFunction("JoinParty", InstanceSystem::AgentScriptFunction::JoinParty);
            _luaInstance.RegistFunction("OutParty", InstanceSystem::AgentScriptFunction::OutParty);
            _luaInstance.RegistFunction("ClearParty", InstanceSystem::AgentScriptFunction::ClearParty);
        }
        { // sendXXX;
            _luaInstance.RegistFunction("SendMessage", InstanceSystem::AgentScriptFunction::SendMessage);
			_luaInstance.RegistFunction("SendSystemMail",InstanceSystem::AgentScriptFunction::SendSystemMail);
        }
        { // RegistXXX;
            _luaInstance.RegistFunction("RegistTimer", InstanceSystem::AgentScriptFunction::RegistTimer);
            _luaInstance.RegistFunction("RegistAbsTimer", InstanceSystem::AgentScriptFunction::RegistAbsTimer);
            _luaInstance.RegistFunction("IsRegistTimer", InstanceSystem::AgentScriptFunction::IsRegistTimer);
            _luaInstance.RegistFunction("RemoveTimer", InstanceSystem::AgentScriptFunction::RemoveTimer);
        }

        { // UI;
            _luaInstance.RegistFunction("PrintChatMsg", InstanceSystem::AgentScriptFunction::PrintChatMsg);
            _luaInstance.RegistFunction("PrintChatXmlMsg", InstanceSystem::AgentScriptFunction::PrintChatXmlMsg);
            _luaInstance.RegistFunction("PrintChatMsg_Broad", InstanceSystem::AgentScriptFunction::PrintChatMsg_Broad);
            _luaInstance.RegistFunction("PrintChatXmlMsg_Broad", InstanceSystem::AgentScriptFunction::PrintChatXmlMsg_Broad);

            _luaInstance.RegistFunction("PrintSimpleMsg", InstanceSystem::AgentScriptFunction::PrintSimpleMsg);
            _luaInstance.RegistFunction("PrintSimpleXmlMsg", InstanceSystem::AgentScriptFunction::PrintSimpleXmlMsg);
            _luaInstance.RegistFunction("PrintSimpleMsg_Broad", InstanceSystem::AgentScriptFunction::PrintSimpleMsg_Broad);
            _luaInstance.RegistFunction("PrintSimpleXmlMsg_Broad", InstanceSystem::AgentScriptFunction::PrintSimpleXmlMsg_Broad);   

            _luaInstance.RegistFunction("UI_TimerMessageBox", InstanceSystem::AgentScriptFunction::UI_TimerMessageBox);            
            _luaInstance.RegistFunction("UI_TimerMessageBox_Broad", InstanceSystem::AgentScriptFunction::UI_TimerMessageBox_Broad);
            _luaInstance.RegistFunction("UI_XmlTimerMessageBox", InstanceSystem::AgentScriptFunction::UI_XmlTimerMessageBox);            
            _luaInstance.RegistFunction("UI_XmlTimerMessageBox_Broad", InstanceSystem::AgentScriptFunction::UI_XmlTimerMessageBox_Broad);
        }

		// 토너먼트 루아 스크립트에서 남기는 로그 (DB에 저장되는 로그)
		_luaInstance.RegistFunction("TournamentStartLog", InstanceSystem::AgentScriptFunction::tournamentStartLog);
		//_luaInstance.registFunction("TournamentMemberLog", InstanceSystem::AgentScriptFunction::tournamentMemberLog);
		//_luaInstance.registFunction("TournamentRoundStartLog", InstanceSystem::AgentScriptFunction::tournamentRoundStartLog);
		_luaInstance.RegistFunction("TournamentRoundEndLog", InstanceSystem::AgentScriptFunction::tournamentRoundEndLog);
		//_luaInstance.registFunction("TournamentEndLog", InstanceSystem::AgentScriptFunction::tournamentEndLog);
    }

    namespace AgentScriptFunction
    {
        // isXXX;
        int isParty(lua_State* pState)
        {
            int currentStack(1);

            const DBNUM _dbNum(lua_tointeger(pState, currentStack++));

            GLCharAG* const _pCharacter(gpAgentServer->GetCharByDbNum(_dbNum));
            const bool _bParty(_pCharacter == 0 ? false : _pCharacter->GetPartyID().isValidParty());
            lua_pushboolean(pState, _bParty);

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - isParty ]";
			sc::writeLogInfo( tempLogString );
#endif

            return 1;
        }
        int isExpedition(lua_State* pState)
        {
            int currentStack(1);

            const DBNUM _dbNum(lua_tointeger(pState, currentStack++));

            GLCharAG* const _pCharacter(gpAgentServer->GetCharByDbNum(_dbNum));
            const bool _bParty(_pCharacter == 0 ? false : _pCharacter->GetPartyID().isValidExpedition());
            lua_pushboolean(pState, _bParty);

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - isExpedition ]";
			sc::writeLogInfo( tempLogString );
#endif

            return 1;
        }
        int isPartyMaster(lua_State* pState)
        {
            int currentStack(1);

            const DBNUM _dbNum(lua_tointeger(pState, currentStack++));

            GLCharAG* const _pCharacter(gpAgentServer->GetCharByDbNum(_dbNum));
            if ( _pCharacter )
            {
                GLPartyID _id = _pCharacter->GetPartyID();
                const GLPartyAgent* agent= gpAgentServer->GetParty(_id);
                return agent->getMaster() == _pCharacter->m_dwGaeaID;
            }
            lua_pushboolean(pState, false);

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - isPartyMaster ]";
			sc::writeLogInfo( tempLogString );
#endif

            return 1;
        }

        // getXXX;
        int GetLevel(lua_State* pState)
        {
            int currentStack(1);

            const DBNUM _dbNum(lua_tointeger(pState, currentStack++));

            GLCharAG* const pAgChar(gpAgentServer->GetCharByDbNum(_dbNum));
            if ( pAgChar == NULL )
            {
                return 0;    
            }
            WORD _wLevel = pAgChar->GetChaLevel();
            lua_pushinteger(pState, _wLevel);

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - GetLevel ]";
			sc::writeLogInfo( tempLogString );
#endif

            return 1;
        }

        int GetName(lua_State* pState)
        {
            int currentStack(1);

            const DBNUM _dbNum(lua_tointeger(pState, currentStack++));

            GLCharAG* const pAgChar(gpAgentServer->GetCharByDbNum(_dbNum));
            if ( pAgChar == NULL )
            {
                return 0;
            }
            const char* _strName = pAgChar->GetChaName();
            lua_pushstring(pState, _strName);

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - GetName ]";
			sc::writeLogInfo( tempLogString );
#endif

            return 1;
        }

        int GetSchool(lua_State* pState)
        {
            int currentStack(1);

            const DBNUM _dbNum(lua_tointeger(pState, currentStack++));

            GLCharAG* const pAgChar(gpAgentServer->GetCharByDbNum(_dbNum));
            if ( pAgChar == NULL )
            {
                return 0;
            }
            WORD _wSchool = pAgChar->GetSchool();
            lua_pushinteger(pState, _wSchool);

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - GetSchool ]";
			sc::writeLogInfo( tempLogString );
#endif

            return 1;
        }

        int GetClubID(lua_State* pState)
        {
            int currentStack(1);

            const DBNUM _dbNum(lua_tointeger(pState, currentStack++));

            GLCharAG* const pAgChar(gpAgentServer->GetCharByDbNum(_dbNum));
            if ( pAgChar )
            {
                DWORD _dwClubID = pAgChar->GetClubDbNum();
                if ( _dwClubID != CLUB_NULL )
                {
                    lua_pushinteger(pState, _dwClubID);

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
					std::string tempLogString = "[ Instance Log ] [ Invoke - GetClubID ]";
					sc::writeLogInfo( tempLogString );
#endif

                    return 1;
                }
            }

            return 0;
        }

        int GetClubName(lua_State* pState)
        {
            int currentStack(1);

            const DBNUM _ClubdbNum(lua_tointeger(pState, currentStack++));

            GLClubManAgent* _pClubMan = gpAgentServer->GetClubMan();
            if ( _pClubMan )
            {
                std::tr1::shared_ptr<GLClubAgent> _spClub = _pClubMan->GetClub(_ClubdbNum);
                if ( _spClub )
                {
                    lua_pushlstring(pState, _spClub->Name(), CLUB_NAME_LENGTH);

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
					std::string tempLogString = "[ Instance Log ] [ Invoke - GetClubName ]";
					sc::writeLogInfo( tempLogString );
#endif

                    return 1;
                }
            }

            return 0;
        }

        int GetClubMasterName(lua_State* pState)
        {
            int currentStack(1);

            const DBNUM _ClubdbNum(lua_tointeger(pState, currentStack++));

            GLClubManAgent* _pClubMan = gpAgentServer->GetClubMan();
            if ( _pClubMan )
            {
                std::tr1::shared_ptr<GLClubAgent> _spClub = _pClubMan->GetClub(_ClubdbNum);
                if ( _spClub )
                {
                    lua_pushlstring(pState, _spClub->GetMasterName(), CHAR_SZNAME);

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
					std::string tempLogString = "[ Instance Log ] [ Invoke - GetClubMasterName ]";
					sc::writeLogInfo( tempLogString );
#endif
                    return 1;
                }
            }

            return 0;
        }

        int GetClubRank(lua_State* pState)
        {
            int currentStack(1);

            const DBNUM _ClubdbNum(lua_tointeger(pState, currentStack++));

            GLClubManAgent* _pClubMan = gpAgentServer->GetClubMan();
            if ( _pClubMan )
            {
                std::tr1::shared_ptr<GLClubAgent> _spClub = _pClubMan->GetClub(_ClubdbNum);
                if ( _spClub )
                {
                    const DWORD dwRank = _spClub->GetRank();
                    lua_pushinteger(pState, dwRank);

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
					std::string tempLogString = "[ Instance Log ] [ Invoke - GetClubRank ]";
					sc::writeLogInfo( tempLogString );
#endif
                    return 1;
                }
            }

            return 0;
        }

        int GetParty(lua_State* pState)
        {
            int currentStack(1);

            const DWORD _charDbNum(lua_tointeger(pState, currentStack++));

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - GetParty ]";
			sc::writeLogInfo( tempLogString );
#endif

            GLCharAG* const pAgChar(gpAgentServer->GetCharByDbNum(_charDbNum));
            if ( pAgChar )
            {
                GLPartyID _partyID = pAgChar->GetPartyID();
                const GLPartyAgent* pParty = gpAgentServer->GetParty(_partyID);
                if ( pParty )
                {
                    if ( pParty->m_Member.size() == 0 )
                        return 0;

                    lua_newtable(pState);
                    int top = lua_gettop(pState);

                    GLPartyAgent::MEMBER_CITER _memberIter = pParty->m_Member.begin();
                    GLPartyAgent::MEMBER_CITER _endIter = pParty->m_Member.end();
                    for ( int i = 1; _memberIter != _endIter; _memberIter++, i++ ) 
                    {
                        GLCharAG* const pAgCharMember(gpAgentServer->GetCharByGaeaID(*_memberIter));
                        if ( pAgCharMember )
                        {
                            lua_pushinteger(pState, pAgCharMember->ChaDbNum());
                            lua_rawseti(pState, top, i);
                        }
                    }

                    return 1;
                }
            }

            return 0;
        }

        int GetPartyID(lua_State* pState)
        {
            int currentStack(1);

            const DBNUM _ClubdbNum(lua_tointeger(pState, currentStack++));

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - GetPartyID ]";
			sc::writeLogInfo( tempLogString );
#endif

            GLCharAG* const pAgChar(gpAgentServer->GetCharByDbNum(_ClubdbNum));
            if ( pAgChar )
            {
                const GLPartyID _sPartyID = pAgChar->GetPartyID();
                if ( _sPartyID.isValidParty() )
                {
                    lua_pushinteger(pState, _sPartyID.GetID());            

                    return 1;
                }
            }

            return 0;
        }

        int GetPartyMemberNum(lua_State* pState)
        {
            int currentStack(1);

            const DBNUM _dwPartyNum(lua_tointeger(pState, currentStack++));

            const GLPartyID sPartyID(_dwPartyNum);

            const GLPartyAgent* const pParty = gpAgentServer->GetParty(sPartyID);
            if ( pParty == NULL )
            {
                lua_pushinteger(pState, 0);

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
				std::string tempLogString = "[ Instance Log ] [ Invoke - GetPartyMemberNum ]";
				sc::writeLogInfo( tempLogString );
#endif

                return 1;
            }

            const int nPartyMember = pParty->getNPartyMember();
            lua_pushinteger(pState, nPartyMember);
            return 1;
        }

        int GetPartyMaster(lua_State* pState)
        {
            int currentStack(1);

            const DBNUM _dwPartyNum(lua_tointeger(pState, currentStack++));

            const GLPartyID sPartyID(_dwPartyNum);

            const GLPartyAgent* const pParty = gpAgentServer->GetParty(sPartyID);
            if ( pParty )
            {
                GLCharAG* const pAgChar(gpAgentServer->GetCharByGaeaID(pParty->getMaster()));
                if ( pAgChar )
                {
                    lua_pushinteger(pState, pAgChar->ChaDbNum());

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
					std::string tempLogString = "[ Instance Log ] [ Invoke - GetPartyMaster ]";
					sc::writeLogInfo( tempLogString );
#endif

                    return 1;
                }
            }
            return 0;
        }

        int GetPartyMember(lua_State* pState)
        {
            int currentStack(1);

            const DBNUM _dwPartyNum(lua_tointeger(pState, currentStack++));
            const DBNUM _dwMemberNum(lua_tointeger(pState, currentStack++));
            const GLPartyID sPartyID(_dwPartyNum);

            const GLPartyAgent* const pParty = gpAgentServer->GetParty(sPartyID);
            if ( pParty && (pParty->getNPartyMember() > _dwMemberNum) )
            {
                GLCharAG* const pAgChar(gpAgentServer->GetCharByGaeaID(pParty->m_Member[_dwMemberNum]));
                if ( pAgChar )
                {
                    lua_pushinteger(pState, pAgChar->ChaDbNum());

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
					std::string tempLogString = "[ Instance Log ] [ Invoke - GetPartyMember ]";
					sc::writeLogInfo( tempLogString );
#endif

                    return 1;
                }
            }
            return 0;
        }

        int GetBaseMapID(lua_State* pState)
        {
            int currentStack(1);
            const DWORD _nDbNum(lua_tointeger(pState, currentStack++));

            const GLCharAG* pCharAG = gpAgentServer->GetCharByDbNum(_nDbNum);
            if ( pCharAG == NULL )
                return 0;

            const SNATIVEID& _baseMapID(pCharAG->GetCurrentMap().getBaseMapID());
            lua_pushinteger(pState, _baseMapID.wMainID);
            lua_pushinteger(pState, _baseMapID.wSubID);

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - GetBaseMapID ]";
			sc::writeLogInfo( tempLogString );
#endif

            return 2;
        }

		int GetPosFromMapPos(lua_State* pState)
		{
			int currentStack(1);

			const int _mapMid(lua_tointeger(pState, currentStack++ ));
			const int _mapSid(lua_tointeger(pState, currentStack++ ));
			const int _mapXPos(lua_tointeger(pState, currentStack++ ));
			const int _mapZPos(lua_tointeger(pState, currentStack++ ));

			D3DXVECTOR3 _worldPos;
			gpAgentServer->ConvertMapToWorld(MapID(SNATIVEID(_mapMid, _mapSid)), _mapXPos, _mapZPos, _worldPos);

			lua_pushnumber(pState, _worldPos.x);
			lua_pushnumber(pState, _worldPos.z);

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - GetPosFromMapPos ]";
			sc::writeLogInfo( tempLogString );
#endif

			return 2;
		}

		int GetMapPosFromPos(lua_State* pState)
		{
			int currentStack(1);

			const int _mapMid(lua_tointeger(pState, currentStack++ ));
			const int _mapSid(lua_tointeger(pState, currentStack++ ));
			const float _worldXPos((float)lua_tonumber(pState, currentStack++ ));
			const float _worldZPos((float)lua_tonumber(pState, currentStack++ ));

			int _mapXPos;
			int _mapZPos;
			gpAgentServer->ConvertWorldToMap(MapID(SNATIVEID(_mapMid, _mapSid)), D3DXVECTOR3(_worldXPos,0.0f,_worldZPos), _mapXPos, _mapZPos);

			lua_pushinteger(pState, _mapXPos);
			lua_pushinteger(pState, _mapZPos);

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - GetMapPosFromPos ]";
			sc::writeLogInfo( tempLogString );
#endif

			return 2;
		}

		int GetCountry(lua_State* pState)
		{
			int currentStack = 1;

			const DBNUM dwDbNum = 
				static_cast< DBNUM >( lua_tonumber( pState, currentStack++ ) );

			const GLCharAG* pCharAG = gpAgentServer->GetCharByDbNum(dwDbNum);
			if ( pCharAG == NULL )
				return 0;

			const Country::SCOUNTRY_INFO& sCountryInfo =
				pCharAG->GetCountryInfo();

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			sc::writeLogInfo( "[ Instance Log ] [ Invoke - GetCountry ]" );
#endif

			lua_pushinteger( pState, sCountryInfo.dwID );

			return 1;
		}

		int GetCountryName(lua_State* pState)
		{
			int currentStack = 1;

			const DWORD dwCountryID = 
				static_cast< DBNUM >( lua_tonumber( pState, currentStack++ ) );

			Country::GLCountryManServer* pCountryMan =
				Country::GLCountryManServer::Instance();
			if ( NULL == pCountryMan )
				return 0;

			Country::GLCountry* pCountry =
				pCountryMan->GetCountryUsingID( dwCountryID );
			if ( NULL == pCountry )
				return 0;

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			sc::writeLogInfo( "[ Instance Log ] [ Invoke - GetCountryName ]" );
#endif

			lua_pushlstring( pState, pCountry->GetCountryName().c_str(), CHAR_SZNAME );

			return 1;
		}

        // setXXX;
        int setPublicJoinLock(lua_State* pState) // 특정 종류에 속하는 모든 인던에 대해 접속을 제한 함;
        {
            int currentStack(1);

            const WORD _nKeyMapMainID((WORD)lua_tointeger(pState, currentStack++));
            const WORD _nKeyMapSubID((WORD)lua_tointeger(pState, currentStack++));
            const bool _bLock(lua_toboolean(pState, currentStack++));

            const SNATIVEID _keyMapID(_nKeyMapMainID, _nKeyMapSubID);
            GLMSG::NET_INSTANCE_JOIN_LOCK_AF _joinLockMessage(GAEAID_NULL, _keyMapID, _bLock);
            gpAgentServer->SENDTOMYSELF(&_joinLockMessage);

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - setPublicJoinLock ]";
			sc::writeLogInfo( tempLogString );
#endif

            return 0;
        }

        // doXXX;
        int doCreateInstance(lua_State* pState)
        {
            int currentStack(1);

            const DWORD _nFieldChannelNum((EMINSTANCE_TYPE)lua_tointeger(pState, currentStack++));
            const WORD _nKeyMapMainID((WORD)lua_tointeger(pState, currentStack++));
            const WORD _nKeyMapSubID((WORD)lua_tointeger(pState, currentStack++));

            const SNATIVEID _keyMapID(_nKeyMapMainID, _nKeyMapSubID);
            InstanceSystem::ManagerAgent* const _pManager(gpAgentServer->getInstanceManager());
            const InstanceMapID _instanceMapID(_pManager->createInstance(_keyMapID));
            lua_pushinteger(pState, _instanceMapID);

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - doCreateInstance ]";
			sc::writeLogInfo( tempLogString );
#endif

            return 1;
        }
        int doDestroyInstance(lua_State* pState)
        {
            int currentStack(1);
            const DWORD _instanceMapID(lua_tointeger(pState, currentStack++));

            gpAgentServer->SENDTOMYSELF(&GLMSG::NET_INSTANCE_DESTROY(InstanceMapID(_instanceMapID)));

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - doDestroyInstance ]";
			sc::writeLogInfo( tempLogString );
#endif

            return 0;
        }	

        // party;
        int JoinParty(lua_State* pState)  // 파티 생성 or 파티원 추가;
        {
            int currentStack(1);

            const DWORD _nMasterDbNum((DWORD)lua_tonumber(pState, currentStack++));
            GLCharAG* const _pMasterChar = gpAgentServer->GetCharByDbNum(_nMasterDbNum);
            if ( _pMasterChar == NULL )
                return 0;

            const DWORD _nMemberDbNum((DWORD)lua_tonumber(pState, currentStack++));
            GLCharAG* const _pMemberChar = gpAgentServer->GetCharByDbNum(_nMemberDbNum);
            if ( _pMemberChar == NULL )
                return 0;

            gpAgentServer->JoinParty(_pMasterChar->GaeaId(), _pMemberChar->GaeaId(), SPartyOption(), false);

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - JoinParty ]";
			sc::writeLogInfo( tempLogString );
#endif

            return 0;
        }

        int OutParty(lua_State* pState)  // 파티 탈퇴;
        {
            int currentStack(1);

            const DWORD _nCharDbNum((DWORD)lua_tonumber(pState, currentStack++));
            GLCharAG* const _pChar = gpAgentServer->GetCharByDbNum(_nCharDbNum);
            if ( _pChar == NULL )
                return 0;

            gpAgentServer->QuitParty(_pChar, false);

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - OutParty ]";
			sc::writeLogInfo( tempLogString );
#endif

            return 0;
        }

        int ClearParty(lua_State* pState)  // 파티 해산;
        {
            int currentStack(1);

            const DWORD _partyID((DWORD)lua_tonumber(pState, currentStack++));

            gpAgentServer->DeleteParty(GLPartyID(_partyID));

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - ClearParty ]";
			sc::writeLogInfo( tempLogString );
#endif

            return 0;
        }

        // sendXXX;
        int SendMessage(lua_State* pState)
        {
            int currentStack(1);
            const DWORD _DestType((DWORD)lua_tonumber(pState, currentStack++));
            const DWORD _DestID((DWORD)lua_tonumber(pState, currentStack++));
            const DWORD _Param1((DWORD)lua_tonumber(pState, currentStack++));
            const DWORD _Param2((DWORD)lua_tonumber(pState, currentStack++));
            const DWORD _Param3((DWORD)lua_tonumber(pState, currentStack++));
            const DWORD _Param4((DWORD)lua_tonumber(pState, currentStack++));

            switch ( _DestType )
            {
                case InstanceSystem::EM_MESSAGE_MY_INSTANCE:    // 이 경우는 메시지를 보낼 수 없음;
                case InstanceSystem::EM_MESSAGE_FIELD_BASE:
                    return 0;
            }

            lua_getglobal(pState, "keyMapID");
            const DWORD _keyMapID((DWORD)lua_tointeger(pState, -1));
            lua_pop(pState, 1);

            GLMSG::NET_INSTANCE_NEW_CUSTOM_MESSAGE netMsg(_Param1, _Param2, _Param3, _Param4);
            netMsg.emSrcType = EM_MESSAGE_AGENT_BASE;
            netMsg.dwSrcID = _keyMapID;
            netMsg.emDestType = _DestType;
            netMsg.dwDestID = _DestID;
            
            gpAgentServer->SENDTOMYSELF(&netMsg);

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - SendMessage ]";
			sc::writeLogInfo( tempLogString );
#endif

            return 0;
        }

        // registXXX;    
        int RegistTimer(lua_State* pState)
        {
            int currentStack(1);
            lua_getglobal(pState, "keyMapID");
            const DWORD _keyMapID((DWORD)lua_tointeger(pState, -1));
            lua_pop(pState, 1);

            const float _fTime((float)lua_tonumber(pState, currentStack++));
            const DWORD _dwParam1((DWORD)lua_tonumber(pState, currentStack++));
            const DWORD _dwParam2((DWORD)lua_tonumber(pState, currentStack++));
            const DWORD _dwParam3((DWORD)lua_tonumber(pState, currentStack++));
            const DWORD _dwParam4((DWORD)lua_tonumber(pState, currentStack++));
            const bool _nLoop(lua_toboolean(pState, currentStack++));

            InstanceSystem::ManagerAgent* const _pManager(gpAgentServer->getInstanceManager());
            if ( _pManager == NULL )
                return  0;

            const DWORD _timerHandle = (DWORD)_pManager->registTimer(_keyMapID, _fTime, _dwParam1, _dwParam2, _dwParam3, _dwParam4, _nLoop);
            if ( _timerHandle == -1 )
                return 0;

            lua_pushnumber(pState, (lua_Number)_timerHandle);

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - RegistTimer ]";
			sc::writeLogInfo( tempLogString );
#endif

            return 1;
        }

        int RegistAbsTimer(lua_State* pState)
        {
            int currentStack(1);
            lua_getglobal(pState, "keyMapID");
            const DWORD _keyMapID((DWORD)lua_tointeger(pState, -1));
            lua_pop(pState, 1);

            const WORD _wWeek((WORD)lua_tointeger(pState, currentStack++));
            const WORD _wDay((WORD)lua_tointeger(pState, currentStack++));
            const WORD _wHour((WORD)lua_tointeger(pState, currentStack++));
            const WORD _wMinute((WORD)lua_tointeger(pState, currentStack++));
            const WORD _wSecond((WORD)lua_tointeger(pState, currentStack++));

            const DWORD _dwParam1((DWORD)lua_tonumber(pState, currentStack++));
            const DWORD _dwParam2((DWORD)lua_tonumber(pState, currentStack++));
            const DWORD _dwParam3((DWORD)lua_tonumber(pState, currentStack++));
            const DWORD _dwParam4((DWORD)lua_tonumber(pState, currentStack++));
            const bool _nLoop(lua_toboolean(pState, currentStack++));

            InstanceSystem::ManagerAgent* const _pManager(gpAgentServer->getInstanceManager());
            if ( _pManager == NULL )
                return  0;

            const DWORD _timerHandle = (DWORD)_pManager->registAbsTimer(_keyMapID, _wWeek, _wDay, _wHour, _wMinute, _wSecond, _dwParam1, _dwParam2, _dwParam3, _dwParam4, _nLoop);
            if ( _timerHandle == -1 )
                return 0;

            lua_pushnumber(pState, (lua_Number)_timerHandle);

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - RegistAbsTimer ]";
			sc::writeLogInfo( tempLogString );
#endif

            return 1;
        }

        int IsRegistTimer(lua_State* pState)
        {
            int currentStack(1);
            lua_getglobal(pState, "keyMapID");
            const DWORD _keyMapID((DWORD)lua_tointeger(pState, -1));
            lua_pop(pState, 1);

            const DWORD _timerHandle((DWORD)lua_tonumber(pState, currentStack++));

            InstanceSystem::ManagerAgent* const _pManager(gpAgentServer->getInstanceManager());
            if ( _pManager == NULL )
                return  0;

            const bool bResult = _pManager->isRegistTimer(_keyMapID, _timerHandle);
            lua_pushboolean(pState, bResult);

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - IsRegistTimer ]";
			sc::writeLogInfo( tempLogString );
#endif

            return 1;
        }

        int RemoveTimer(lua_State* pState)
        {
            int currentStack(1);
            lua_getglobal(pState, "keyMapID");
            const DWORD _keyMapID((DWORD)lua_tointeger(pState, -1));
            lua_pop(pState, 1);

            const DWORD _timerHandle((DWORD)lua_tonumber(pState, currentStack++));

            InstanceSystem::ManagerAgent* const _pManager(gpAgentServer->getInstanceManager());
            if ( _pManager == NULL )
                return  0;

            const bool bResult = _pManager->removeTimer(_keyMapID, _timerHandle);
            lua_pushboolean(pState, bResult);

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - RemoveTimer ]";
			sc::writeLogInfo( tempLogString );
#endif

            return 1;
        }

        // UI;
        int PrintChatMsg(lua_State* pState)
        {
            int currentStack(1);

            const lua_Integer _nDbNum(lua_tointeger(pState, currentStack++));
            GLCharAG* const _pChar = gpAgentServer->GetCharByDbNum(_nDbNum);
            if ( _pChar == NULL )
                return 0;

            GLMSG::NET_INSTANCE_UI_CHATMSG NetMsg;
            if ( _makeChatMsgPacket(pState, currentStack, NetMsg) == false )
                return 0;

            msgpack::sbuffer SendBuffer;
            msgpack::pack(SendBuffer, NetMsg);
            gpAgentServer->SENDTOCLIENT(_pChar->ClientSlot(), NET_MSG_GCTRL_INSTANCE_SYSTEM_MSGPACK, SendBuffer);

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - PrintChatMsg ]";
			sc::writeLogInfo( tempLogString );
#endif

            return 0;
        }

        int PrintChatXmlMsg(lua_State* pState)
        {
            int currentStack(1);

            const lua_Integer _nDbNum(lua_tointeger(pState, currentStack++));
            GLCharAG* const _pChar = gpAgentServer->GetCharByDbNum(_nDbNum);
            if ( _pChar == NULL )
                return 0;

            GLMSG::NET_INSTANCE_UI_CHATXMLMSG NetMsg;
            if ( _makeChatXmlMsgPacket(pState, currentStack, NetMsg) == false )
                return 0;

            msgpack::sbuffer SendBuffer;
            msgpack::pack(SendBuffer, NetMsg);
            gpAgentServer->SENDTOCLIENT(_pChar->ClientSlot(), NET_MSG_GCTRL_INSTANCE_SYSTEM_MSGPACK, SendBuffer);

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - PrintChatXmlMsg ]";
			sc::writeLogInfo( tempLogString );
#endif

            return 0;
        }


        int PrintChatMsg_Broad(lua_State* pState)
        {
            int currentStack(1);

            GLMSG::NET_INSTANCE_UI_CHATMSG NetMsg;
            if ( _makeChatMsgPacket(pState, currentStack, NetMsg) == false )
                return 0;

            msgpack::sbuffer SendBuffer;
            msgpack::pack(SendBuffer, NetMsg);

            gpAgentServer->SENDTOALLCLIENT(NET_MSG_GCTRL_INSTANCE_SYSTEM_MSGPACK, SendBuffer);

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - PrintChatMsg_Broad ]";
			sc::writeLogInfo( tempLogString );
#endif

            return 0;
        }

        int PrintChatXmlMsg_Broad(lua_State* pState)
        {
            int currentStack(1);

            GLMSG::NET_INSTANCE_UI_CHATXMLMSG NetMsg;
            if ( _makeChatXmlMsgPacket(pState, currentStack, NetMsg) == false )
                return 0;

            msgpack::sbuffer SendBuffer;
            msgpack::pack(SendBuffer, NetMsg);

            gpAgentServer->SENDTOALLCLIENT(NET_MSG_GCTRL_INSTANCE_SYSTEM_MSGPACK, SendBuffer);

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - PrintChatXmlMsg_Broad ]";
			sc::writeLogInfo( tempLogString );
#endif

            return 0;
        }

        int PrintSimpleMsg(lua_State* pState)
        {
            int currentStack(1);

            const lua_Integer _nDbNum(lua_tointeger(pState, currentStack++));
            GLCharAG* const _pChar = gpAgentServer->GetCharByDbNum(_nDbNum);
            if ( _pChar == NULL )
                return 0;

            GLMSG::NET_INSTANCE_UI_SIMPLE_MSG NetMsg;
            if ( _makeSimpleMsgPacket(pState, currentStack, NetMsg) == false )
                return 0;

            msgpack::sbuffer SendBuffer;
            msgpack::pack(SendBuffer, NetMsg);
            gpAgentServer->SENDTOCLIENT(_pChar->ClientSlot(), NET_MSG_GCTRL_INSTANCE_SYSTEM_MSGPACK, SendBuffer);

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - PrintSimpleMsg ]";
			sc::writeLogInfo( tempLogString );
#endif

            return 0;
        }

        int PrintSimpleXmlMsg(lua_State* pState)
        {
            int currentStack(1);

            const lua_Integer _nDbNum(lua_tointeger(pState, currentStack++));
            GLCharAG* const _pChar = gpAgentServer->GetCharByDbNum(_nDbNum);
            if ( _pChar == NULL )
                return 0;

            GLMSG::NET_INSTANCE_UI_SIMPLE_XMLMSG NetMsg;
            if ( _makeSimpleXmlMsgPacket(pState, currentStack, NetMsg) == false )
                return 0;

            msgpack::sbuffer SendBuffer;
            msgpack::pack(SendBuffer, NetMsg);
            gpAgentServer->SENDTOCLIENT(_pChar->ClientSlot(), NET_MSG_GCTRL_INSTANCE_SYSTEM_MSGPACK, SendBuffer);

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - PrintSimpleXmlMsg ]";
			sc::writeLogInfo( tempLogString );
#endif

            return 0;
        }

        int PrintSimpleMsg_Broad(lua_State* pState)
        {
            int currentStack(1);

            GLMSG::NET_INSTANCE_UI_SIMPLE_MSG NetMsg;
            if ( _makeSimpleMsgPacket(pState, currentStack, NetMsg) == false )
                return 0;

            msgpack::sbuffer SendBuffer;
            msgpack::pack(SendBuffer, NetMsg);
            gpAgentServer->SENDTOALLCLIENT(NET_MSG_GCTRL_INSTANCE_SYSTEM_MSGPACK, SendBuffer);

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - PrintSimpleMsg_Broad ]";
			sc::writeLogInfo( tempLogString );
#endif

            return 0;
        }

        int PrintSimpleXmlMsg_Broad(lua_State* pState)
        {
            int currentStack(1);

            GLMSG::NET_INSTANCE_UI_SIMPLE_XMLMSG NetMsg;
            if ( _makeSimpleXmlMsgPacket(pState, currentStack, NetMsg) == false )
                return 0;

            msgpack::sbuffer SendBuffer;
            msgpack::pack(SendBuffer, NetMsg);
            gpAgentServer->SENDTOALLCLIENT(NET_MSG_GCTRL_INSTANCE_SYSTEM_MSGPACK, SendBuffer);

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - PrintSimpleXmlMsg_Broad ]";
			sc::writeLogInfo( tempLogString );
#endif

            return 0;
        }

        int UI_TimerMessageBox(lua_State* pState)
        {
            int currentStack(1);

            lua_getglobal(pState, "keyMapID");
            const SNATIVEID _keyMapID((DWORD)lua_tointeger(pState, -1));
            lua_pop(pState, 1);

            const DWORD _nDbNum(lua_tointeger(pState, currentStack++));
            GLCharAG* const _pChar = gpAgentServer->GetCharByDbNum(_nDbNum);
            if ( _pChar == NULL )
                return 0;

            GLMSG::NET_INSTANCE_UI_TIMER_MSGBOX NetMsg;

            _makeTimerMsgBoxPacket(pState, currentStack, NetMsg);

            NetMsg.emResponseType = EM_MESSAGE_AGENT_BASE;
            NetMsg.dwInstanceID = _keyMapID.dwID;

            msgpack::sbuffer SendBuffer;
            msgpack::pack(SendBuffer, NetMsg);
            gpAgentServer->SENDTOCLIENT(_pChar->ClientSlot(), NET_MSG_GCTRL_INSTANCE_SYSTEM_MSGPACK, SendBuffer);

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - UI_TimerMessageBox ]";
			sc::writeLogInfo( tempLogString );
#endif

            return 0;
        }

        int UI_TimerMessageBox_Broad(lua_State* pState)
        {
            int currentStack(1);

            lua_getglobal(pState, "keyMapID");
            const SNATIVEID _keyMapID((DWORD)lua_tointeger(pState, -1));
            lua_pop(pState, 1);

            GLMSG::NET_INSTANCE_UI_TIMER_MSGBOX NetMsg;

            _makeTimerMsgBoxPacket(pState, currentStack, NetMsg);

            NetMsg.emResponseType = EM_MESSAGE_AGENT_BASE;
            NetMsg.dwInstanceID = _keyMapID.dwID;

            msgpack::sbuffer SendBuffer;
            msgpack::pack(SendBuffer, NetMsg);
            gpAgentServer->SENDTOALLCLIENT(NET_MSG_GCTRL_INSTANCE_SYSTEM_MSGPACK, SendBuffer);

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - UI_TimerMessageBox_Broad ]";
			sc::writeLogInfo( tempLogString );
#endif

            return 0;
        }

        int UI_XmlTimerMessageBox(lua_State* pState)
        {
            int currentStack(1);

            lua_getglobal(pState, "keyMapID");
            const SNATIVEID _keyMapID((DWORD)lua_tointeger(pState, -1));
            lua_pop(pState, 1);

            const DWORD _nDbNum(lua_tointeger(pState, currentStack++));
            GLCharAG* const _pChar = gpAgentServer->GetCharByDbNum(_nDbNum);
            if ( _pChar == NULL )
                return 0;

            GLMSG::NET_INSTANCE_UI_TIMER_MSGBOX NetMsg;

            _makeXmlTimerMsgBoxPacket(pState, currentStack, NetMsg);

            NetMsg.emResponseType = EM_MESSAGE_AGENT_BASE;
            NetMsg.dwInstanceID = _keyMapID.dwID;

            msgpack::sbuffer SendBuffer;
            msgpack::pack(SendBuffer, NetMsg);
            gpAgentServer->SENDTOCLIENT(_pChar->ClientSlot(), NET_MSG_GCTRL_INSTANCE_SYSTEM_MSGPACK, SendBuffer);

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - UI_XmlTimerMessageBox ]";
			sc::writeLogInfo( tempLogString );
#endif

            return 0;
        }

        int UI_XmlTimerMessageBox_Broad(lua_State* pState)
        {
            int currentStack(1);

            lua_getglobal(pState, "keyMapID");
            const SNATIVEID _keyMapID((DWORD)lua_tointeger(pState, -1));
            lua_pop(pState, 1);

            GLMSG::NET_INSTANCE_UI_TIMER_MSGBOX NetMsg;

            _makeXmlTimerMsgBoxPacket(pState, currentStack, NetMsg);

            NetMsg.emResponseType = EM_MESSAGE_AGENT_BASE;
            NetMsg.dwInstanceID = _keyMapID.dwID;

            msgpack::sbuffer SendBuffer;
            msgpack::pack(SendBuffer, NetMsg);
            gpAgentServer->SENDTOALLCLIENT(NET_MSG_GCTRL_INSTANCE_SYSTEM_MSGPACK, SendBuffer);

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - UI_XmlTimerMessageBox_Broad ]";
			sc::writeLogInfo( tempLogString );
#endif

            return 0;
        }

		// SendXXX;
		int SendSystemMail(lua_State* pState)
		{
			int currentStack(1);

			const DWORD _dbNum(lua_tointeger(pState, currentStack++));
			const GLCharAG* const pChar(gpAgentServer->GetCharByDbNum(_dbNum));
			if ( pChar == NULL )
				return 0;

			std::string _strPostTitle;
			if ( _luaTableToString(pState, currentStack, _strPostTitle) == false )
				return 0;

			std::string _strPostContent;
			if ( _luaTableToString(pState, currentStack, _strPostContent) == false )
				return 0;

			const DWORD _dwAttachedMoney(lua_tointeger(pState, currentStack++)); 

			const DWORD dwContentTextIndex = 
				static_cast< DWORD >( lua_tonumber( pState, currentStack++ ) );

			DWORD _dwItemMainID(0);
			DWORD _dwItemSubID(0);
			DWORD _dwItemNum(0);
			bool bResult(false);
			LuaPlus::LuaObject _AttachedItem(LuaPlus::LuaState::CastState(pState), currentStack++);

			if (_AttachedItem.IsNil() == false)
			{
				if ( _AttachedItem.IsTable() )
				{
					LuaPlus::LuaObject _Obj;
					LuaPlus::LuaTableIterator _ItemIterator(_AttachedItem);
					{
						_Obj = _ItemIterator.GetValue();
                        if ( _Obj.IsNumber() )
						    _dwItemMainID = _Obj.GetInteger();
					}
					if ( _ItemIterator.Next() )
					{
						_Obj = _ItemIterator.GetValue();
                        if ( _Obj.IsNumber() )
						    _dwItemSubID = _Obj.GetInteger();
					}
					if ( _ItemIterator.Next() )
					{
						_Obj = _ItemIterator.GetValue();
                        if ( _Obj.IsNumber() )
						    _dwItemNum = _Obj.GetInteger();
					}
				}

				const SNATIVEID receiveItemID((WORD)_dwItemMainID, (WORD)_dwItemSubID);        


				GLMSG::NET_INSTANCE_MAIL Message;
				
				Message.Mail.CharID = pChar->ChaDbNum();
				Message.Mail.szName = std::string(pChar->GetChaName());
				Message.Mail.NewItem = receiveItemID;
				Message.Mail.IsUseItem = true;
				Message.Mail.AttachedMoney = _dwAttachedMoney;
				Message.Mail.Sender = ID2SERVERTEXT("SYSTEM_POST_SENDER_NAME");
				Message.Mail.PostTitle = _strPostTitle;
				Message.Mail.PostContent = _strPostContent;
				Message.Mail.ItemNum = _dwItemNum;
				Message.Mail.dwContentTextIndex = dwContentTextIndex;
				msgpack::sbuffer SendBuffer;
				msgpack::pack(SendBuffer, Message);
				gpAgentServer->SENDTOFIELD(pChar->GetChannel(),pChar->GetCurrentFieldServer(),NET_MSG_GCTRL_INSTANCE_CONTENTS_MSGPACK, SendBuffer);

				// 토너먼트 로그
				db::LogTournament* pDbAction = new db::LogTournament(9006);

				pDbAction->dwCharDbNum =  pChar->ChaDbNum();   
				pDbAction->strChaName = pChar->GetChaName();  
				pDbAction->nMoney = _dwAttachedMoney;   
				pDbAction->dwItemCount = _dwItemNum;

				gpAgentServer->AddLogAdoJob( db::DbActionPtr(pDbAction) );
			}
			else
			{

				GLMSG::NET_INSTANCE_MAIL Message;

				Message.Mail.CharID = pChar->ChaDbNum();
				Message.Mail.szName = std::string(pChar->GetChaName());
				Message.Mail.IsUseItem = false;
				Message.Mail.AttachedMoney = _dwAttachedMoney;
				Message.Mail.Sender = ID2SERVERTEXT("SYSTEM_POST_SENDER_NAME");
				Message.Mail.PostTitle = _strPostTitle;
				Message.Mail.PostContent = _strPostContent;
				Message.Mail.dwContentTextIndex = dwContentTextIndex;

				msgpack::sbuffer SendBuffer;
				msgpack::pack(SendBuffer, Message);
				gpAgentServer->SENDTOFIELD(pChar->GetChannel(),pChar->GetCurrentFieldServer(),NET_MSG_GCTRL_INSTANCE_CONTENTS_MSGPACK, SendBuffer);

				// 토너먼트 로그
				db::LogTournament* pDbAction = new db::LogTournament(9006);

				pDbAction->dwCharDbNum =  pChar->ChaDbNum();   
				pDbAction->strChaName = pChar->GetChaName();
				pDbAction->nMoney = _dwAttachedMoney;   

				gpAgentServer->AddLogAdoJob( db::DbActionPtr(pDbAction) );
			}

			if ( bResult == false )
				return 0; // 우편 보내기 실패;

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - SendSystemMail ]";
			sc::writeLogInfo( tempLogString );
#endif

			return 0;
		}

		int tournamentStartLog(lua_State* pState)
		{
			int currentStack(1);

			const DWORD dwTournamentMemberCount(lua_tointeger(pState, currentStack++));
			const DWORD dwTournamentRoundCount(lua_tointeger(pState, currentStack++));
			const DWORD dwTournamentNodeCount(lua_tointeger(pState, currentStack++));

			db::LogTournament* pDbAction = new db::LogTournament(9001);

			pDbAction->dwTotalMemberCount = dwTournamentMemberCount;
			pDbAction->dwTournamentSize = dwTournamentRoundCount;
			pDbAction->dwTournamentNodeCount = dwTournamentNodeCount;
			
			gpAgentServer->AddLogAdoJob( db::DbActionPtr(pDbAction) );

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - tournamentStartLog ]";
			sc::writeLogInfo( tempLogString );
#endif

			return 0;
		}

		//int tournamentMemberLog(lua_State* pState)
		//{

		//	return 0;
		//}

		//int tournamentRoundStartLog(lua_State* pState)
		//{

		//	return 0;
		//}

		int tournamentRoundEndLog(lua_State* pState)
		{
			int currentStack(1);

			const DWORD dwChaNum(lua_tointeger(pState, currentStack++));
			const DWORD dwGropuID(lua_tointeger(pState, currentStack++));
			const DWORD dwForceWin(lua_tointeger(pState, currentStack++));

			const GLCharAG* pChar(gpAgentServer->GetCharByDbNum(dwChaNum));
			if( pChar == NULL )
				return false;

			db::LogTournament* pDbAction = new db::LogTournament(9003);

			pDbAction->dwCharDbNum = dwChaNum;
			pDbAction->strChaName = pChar->GetChaName();
			pDbAction->dwFactionID = dwGropuID;
			pDbAction->dwForceWin = dwForceWin;

			gpAgentServer->AddLogAdoJob( db::DbActionPtr(pDbAction) );

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - tournamentRoundEndLog ]";
			sc::writeLogInfo( tempLogString );
#endif

			return 0;
		}

		//int tournamentEndLog(lua_State* pState)
		//{

		//	return 0;
		//}

    }
}