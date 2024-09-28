#include "pch.h"

#include "../../FieldServer/GLGaeaServer.h"
#include "InstanceSystemAgent.h"
#include "InvokeFunctionAgent.h"

// Matching system
#include "../../MatchSystem/InvokeFunctionAgentMatching.h"

namespace InstanceSystem
{
	void initializeAgentInstanceInvokeFunction(sc::LuaInstance& _luaInstance)
	{
		registFunctionCommon(_luaInstance);

		registFunctionAgent(_luaInstance);
		registFunctionAgentInstance(_luaInstance);

		// 매칭 시스템;
		MatchingSystem::registFunction_AgentMatching(_luaInstance);
	}

	void registFunctionAgentInstance(InstanceSystem::ScriptManager* const _pScriptManager)
	{
		if ( NULL == _pScriptManager )
			return;

		_pScriptManager->registFunctionCommon( registFunctionAgentInstance );
	}

	void registFunctionAgentInstance(sc::LuaInstance& _luaInstance)
	{
		_luaInstance.RegistFunction("DoJoin", InstanceSystem::AgentScriptFunction::DoJoin);
		_luaInstance.RegistFunction("DoOut", InstanceSystem::AgentScriptFunction::DoOut);
	}


	namespace AgentScriptFunction
	{
		void ServerLog_InstanceScriptSystem_InstanceFunction ( lua_State* pState, const std::string& strServerLog )
		{
#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			lua_getglobal(pState, "instanceMapID");
			const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));
			lua_pop(pState, 1);

			lua_getglobal(pState, "keyMapID");
			SNATIVEID _keyMapID((DWORD)lua_tonumber(pState, -1));
			lua_pop(pState, 1);

			sc::writeLogInfo( strServerLog );
			sc::writeLogInfo( sc::string::format( "[ Instance Log ] -> [ Instance ID : %1% ]", _instanceMapID ) );
			sc::writeLogInfo( sc::string::format( "[ Instance Log ] -> [ Map ID : %1% / %2% ]", _keyMapID.Mid(), _keyMapID.Sid() ) );
#endif
		}

		int DoJoin(lua_State* pState)
		{
			int currentStack(1);

			lua_getglobal(pState, "instanceMapID");
			const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));
			lua_pop(pState, 1);

			const DWORD _CharDbNum(lua_tointeger(pState, currentStack++));
			LuaPlus::LuaObject _firstReturnValue(LuaPlus::LuaState::CastState(pState), currentStack++);
			LuaPlus::LuaObject _secondReturnValue(LuaPlus::LuaState::CastState(pState), currentStack++);

			if ( _firstReturnValue.IsNumber() == false )
			{
				// 리턴값이 없다면 초기 위치 변경 안함;
				const InformationPosition _informationPosition(
					Faction::GLActorID(CROW_PC, _CharDbNum),
					SNATIVEID(_instanceMapID, 0),
					Faction::GLActorID(), 
					0.0f, 0.0f, InstanceSystem::EMMOVE_FIELD2INSTANCE);

				GLMSG::NET_INSTANCE_REQUEST_JOIN _joinMessage(_informationPosition, InstanceSystem::EMAUTHORITY_NORMAL_FORCED);
				gpAgentServer->SENDTOMYSELF(&_joinMessage);
			}
			else if ( _secondReturnValue.IsNumber() == false )
			{
				// 리턴 값이 1개일 때는 게이트 번호를 뜻한다.;
				const int _nGateID( _firstReturnValue.GetInteger() );

				const InformationPosition _informationPosition(
					Faction::GLActorID(CROW_PC, _CharDbNum),
					SNATIVEID(_instanceMapID, 0),
					Faction::GLActorID(CROW_GATE, _nGateID), 
					0.0f, 0.0f, InstanceSystem::EMMOVE_FIELD2INSTANCE);

				GLMSG::NET_INSTANCE_REQUEST_JOIN _joinMessage(_informationPosition, InstanceSystem::EMAUTHORITY_NORMAL_FORCED);
				gpAgentServer->SENDTOMYSELF(&_joinMessage);
			}
			else
			{
				// 리턴 값이 2개일 때는 월드 좌표를 뜻한다.;
				const float _fPositionX( _firstReturnValue.GetFloat() );
				const float _fPositionZ( _secondReturnValue.GetFloat() );

				const InformationPosition _informationPosition(
					Faction::GLActorID(CROW_PC, _CharDbNum),
					SNATIVEID(_instanceMapID, 0),
					Faction::GLActorID(), 
					_fPositionX, _fPositionZ, InstanceSystem::EMMOVE_FIELD2INSTANCE);

				GLMSG::NET_INSTANCE_REQUEST_JOIN _joinMessage(_informationPosition, InstanceSystem::EMAUTHORITY_NORMAL_FORCED);
				gpAgentServer->SENDTOMYSELF(&_joinMessage);
			}

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - DoJoin ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

			return 0;
		}

		int DoOut(lua_State* pState)
		{
			int currentStack(1);

			const DWORD _CharDbNum(lua_tointeger(pState, currentStack++));

			lua_getglobal(pState, "instanceMapID");
			const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));
			lua_pop(pState, 1);

			GLMSG::NET_INSTANCE_REQUEST_OUT _outMessage(_instanceMapID, _CharDbNum);
			gpAgentServer->SENDTOMYSELF(&_outMessage);

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - DoOut ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

			return 0;
		}
	}
}