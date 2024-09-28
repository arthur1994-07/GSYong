#include "pch.h"

#include "../../../RanLogic/Msg/GLContrlInstanceMsg.h"
#include "../../../RanLogic/Msg/GLContrlFactionMsg.h"
#include "../../../RanLogic/InstanceSystem/GLInstanceSystem.h"
#include "../../AgentServer/GLAgentServer.h"
#include "../InstanceSystem.h"
#include "InstanceSystemAgent.h"

namespace InstanceSystem
{
	const bool ManagerAgent::requestFaction(const DBNUM _dbNum,const DWORD _dwFaction, const InstanceMapID _instanceMapID,Faction::EMFACTION_TYPE factionstype)
	{
		GLCharAG* const _pRequester(gpAgentServer->GetCharByDbNum(_dbNum));
		if ( _pRequester == 0 )
			return false;
		InstanceAgentMapIter _iteratorInstance(this->instanceMap.find(_instanceMapID));
		if ( _iteratorInstance == this->instanceMap.end() )
			return false;

		InstanceAgent& _instanceAgent = _iteratorInstance->second;
		GLMSG::NET_FACTION_RELATION_SET_FACTOIN _SetFactionMessage(_dbNum,_dwFaction,_instanceMapID,factionstype);
		gpAgentServer->SENDTOFIELDSVR(
			_instanceAgent.getFieldChannel(), _instanceAgent.getFieldServer(), &_SetFactionMessage);

        return true;
	}


    const bool ManagerAgent::requestEntry(const DBNUM _dbNum, const SNATIVEID& _keyMapID, const unsigned int _nGateID, const unsigned int _nToGateID)
	{
		sc::LuaInstance* const _pLuaInstance(InstanceSystem::getScript(_keyMapID));
		if ( _pLuaInstance == 0 )
			return false; // �߸��� �δ� ID;
		
		{ // ���� Ȱ��ȭ �Ǿ� �ִ� �δ��鿡 �߰� ������ �������� �����;
			for ( InstanceAgentMapIter _iteratorInstance(this->instanceMap.begin()); _iteratorInstance != this->instanceMap.end(); ++_iteratorInstance )
			{				
				InstanceAgent& _instanceAgent(_iteratorInstance->second);
				const InstanceMapID _instanceMapID(_instanceAgent.getInstanceMapID());
				if ( _instanceAgent.getKeyMapID() != _keyMapID )
					continue;

				// �߰� ���� ������ �������� �����;
				const bool _bRequestByGate(_nGateID != GAEAID_NULL);
				if ( _instanceAgent.requestEntry(_dbNum, _bRequestByGate) == false )
					continue;

				const InformationPosition _informationPosition(
					Faction::GLActorID(CROW_PC, _dbNum),
					SNATIVEID(_instanceMapID, 0),
					Faction::GLActorID(CROW_GATE, _nToGateID),
					0.0f, 0.0f, InstanceSystem::EMMOVE_FIELD2INSTANCE);

				if ( ManagerAgent::reserveMember(_dbNum, _informationPosition) == true )
					return true;
			}
		}

		return false;
	}

	const bool ManagerAgent::DoOut(const DBNUM _dbNum, const DWORD _instanceMapID )
	{
		InstanceAgentMapIter _iteratorInstance(this->instanceMap.find(_instanceMapID));
		if ( _iteratorInstance == this->instanceMap.end() )
			return false;

		InstanceAgent& _instanceAgent = _iteratorInstance->second;
		return _instanceAgent.removeMember(_dbNum);
	}

	const bool ManagerAgent::DoJoin(const DBNUM _dbNum, const InformationPosition& _informationPosition, const EMAUTHORITY _emAuthority,MatchingSystem::JoinTrigger* _trigger)
	{
		m_pMatchingCallBackTriggerManager->m_JoinTriggerList.push_back( _trigger );

		const bool _bResult( 
			ManagerAgent::reserveMember(
			_dbNum, _informationPosition, _emAuthority));
		// ���� �����ϰ�

		MatchingSystem::JoinTrigger* jointrigger;
		MatchingSystem::JOINTRIGGER_ITER biter = m_pMatchingCallBackTriggerManager->m_JoinTriggerList.begin();
		MatchingSystem::JOINTRIGGER_ITER eiter = m_pMatchingCallBackTriggerManager->m_JoinTriggerList.end();
		//�ݺ��� �����
		for (;biter!= eiter; ++biter )
		{
			jointrigger = *biter;

			// �̹� ������ �̹� ���� �Ͽ���. �ֳĸ� �̹� Join������. �׷��ϱ� True�� �����Ͽ� �ϴܼ����̶� �˸�.
			if ( jointrigger->_ClientDBID == _trigger->_ClientDBID &&
				jointrigger->_instanceMapID == _trigger->_instanceMapID )
				return true;
		}

		return _bResult;
	}

	const bool ManagerAgent::requestCreate(const DBNUM _dbNum, const SNATIVEID& _keyMapID, const bool _bRequestByGate, OUT DWORD& _param1, OUT DWORD& _param2, OUT DWORD& _param3, OUT DWORD& _param4)
	{
		GLCharAG* const _pRequester(gpAgentServer->GetCharByDbNum(_dbNum));
		if ( _pRequester == 0 )
			return false;

		InstanceLogMapCIter _iteratorLog(this->instanceLogMap.find(_keyMapID));
		if ( _iteratorLog == this->instanceLogMap.end() )
			return false;

		const InstanceLog& _instanceLog = _iteratorLog->second;
		if ( _instanceLog.nRunningCount >= _instanceLog.nMaxCreate )
			return false;

		sc::LuaInstance* const _pLuaInstance(InstanceSystem::getScript(_keyMapID));
		if ( _pLuaInstance == 0 )
			return false; // �߸��� �δ� ID;

		// ���� Ȱ��ȭ �Ǿ� �ִ� �δ��� �ִµ� �ѹ��� ���������� ��쿡�� �������� �ʴ´�;
		{
			for ( InstanceAgentMapIter _iteratorInstance( this->instanceMap.begin() );
				_iteratorInstance != this->instanceMap.end(); ++_iteratorInstance )
			{				
				// ���� ������ �δ����� Ȯ���Ѵ�;
				const InstanceAgent& _instanceAgent( _iteratorInstance->second );
				const InstanceMapID _instanceMapID( _instanceAgent.getInstanceMapID() );
				if ( _instanceAgent.getKeyMapID() != _keyMapID )
					continue;

				// �ѹ��� ������������ Ȯ���Ѵ�;
				if ( _pLuaInstance->PushFunctionEx("luaAgentRequestDuplicateCreate") == true )
				{
					_pLuaInstance->CallFunction( 0, 1 );

					bool bResult = true;
					bResult = _pLuaInstance->PopBoolean();

					if ( false == bResult )
						return false;
				}

				// �Լ��� �������� ������ �⺻������ �ߺ� ���� �����ϵ��� �Ѵ�;
			}
		}

		if ( _pLuaInstance->PushFunctionEx("luaAgentRequestCreate") == true )
		{
			_pLuaInstance->PushInteger(_dbNum);
			_pLuaInstance->PushInteger(_bRequestByGate);
			_pLuaInstance->CallFunction(2, 5);

			// Stack �� ���̱� ������ Lua ���� �Ѱ��ִ°Ͱ� �ݴ��� ������ �޴´�;
			_param4 = _pLuaInstance->PopInteger();
			_param3 = _pLuaInstance->PopInteger();
			_param2 = _pLuaInstance->PopInteger();
			_param1 = _pLuaInstance->PopInteger();

			const bool bResult = _pLuaInstance->PopBoolean();

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			sc::writeLogInfo( "[ Instance Log ] [ Callback - luaAgentRequestCreate ]" );
#endif

			return bResult;
		}
		
		return true;
	}

	const SNATIVEID ManagerAgent::requestMapID(const SNATIVEID& _keyMapID)
	{
		sc::LuaInstance* const _pLuaInstance(InstanceSystem::getScript(_keyMapID));
		if ( _pLuaInstance == 0 )
			return SNATIVEID(false); // �߸��� �δ� ID;

		SNATIVEID _baseMapID(_keyMapID);
		if ( _pLuaInstance->PushFunctionEx("luaAgentRequestMapID") == true )
		{
            _pLuaInstance->CallFunction(0, 2);

			const unsigned int _nSubMapID = _pLuaInstance->PopInteger();
			const unsigned int _nMainMapID = _pLuaInstance->PopInteger();
			_baseMapID = SNATIVEID( WORD(_nMainMapID), WORD(_nSubMapID) );

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			sc::writeLogInfo( "[ Instance Log ] [ Callback - luaAgentRequestMapID ]" );
#endif
		}

		if ( gpAgentServer->GetLand(_baseMapID) == 0 )
			return SNATIVEID(false); // ���� ���� �ʴ� ��;

		return _baseMapID;
	}

    const TimerHandle ManagerAgent::registTimer(const SNATIVEID& _keyMapID, const float _fTime, const DWORD _dwParam0, const DWORD _dwParam1, const DWORD _dwParam2, const DWORD _dwParam3, const bool _bLoop)
    {
        InstanceBaseAgentMapIter _iter = instanceBaseMap.find(_keyMapID);
        if ( _iter == instanceBaseMap.end() )
            return -1;

        InstanceBaseAgent& _instanceAgent(_iter->second);
        const TimerHandle _timerHandel = _instanceAgent.registTimer(_fTime, _dwParam0, _dwParam1, _dwParam2, _dwParam3, _bLoop);
        return _timerHandel;
    }

    const TimerHandle ManagerAgent::registAbsTimer(const SNATIVEID& _keyMapID, const WORD wWeek, const WORD wDay, const WORD wHour, const WORD wMinute, const WORD wSecond,
                                                   const DWORD _dwParam0, const DWORD _dwParam1, const DWORD _dwParam2, const DWORD _dwParam3, const bool _bLoop)
    {
        InstanceBaseAgentMapIter _iter = instanceBaseMap.find(_keyMapID);
        if ( _iter == instanceBaseMap.end() )
            return -1;

        InstanceBaseAgent& _instanceAgent(_iter->second);
        const TimerHandle _timerHandel = _instanceAgent.registAbsTimer(wWeek, wDay, wHour, wMinute, wSecond, _dwParam0, _dwParam1, _dwParam2, _dwParam3, _bLoop);
        return _timerHandel;
    }

    const bool ManagerAgent::isRegistTimer(const SNATIVEID& _keyMapID, const TimerHandle _hTimer)
    {
        InstanceBaseAgentMapIter _iter = instanceBaseMap.find(_keyMapID);
        if ( _iter == instanceBaseMap.end() )
            return false;

        InstanceBaseAgent& _instanceAgent(_iter->second);
        return _instanceAgent.isRegistTimer(_hTimer);
    }

    const bool ManagerAgent::removeTimer(const SNATIVEID& _keyMapID, const TimerHandle _hTimer)
    {
        InstanceBaseAgentMapIter _iter = instanceBaseMap.find(_keyMapID);
        if ( _iter == instanceBaseMap.end() )
            return false;

        InstanceBaseAgent& _instanceAgent(_iter->second);
        return _instanceAgent.removeTimer(_hTimer);
    }

	const bool InstanceAgent::requestJoin(const DBNUM _dbNum, const EMAUTHORITY _emAuthority)
	{
		if ( InstanceAgent::isState(InstanceSystem::EMINSTANCE_RESERVE_DESTROY) == true )
			return false; // ���� �����̸� �ź�;

		if ( InstanceAgent::isState(InstanceSystem::EMINSTANCE_JOIN_LOCK) == true )
			return false; // ���� �Ұ� ������ ��� �ź�;

		if ( this->luaInstance.PushFunctionEx("luaAgentRequestJoin") == true )
		{
			this->luaInstance.PushInteger(_dbNum);
			this->luaInstance.PushInteger(_emAuthority);
			this->luaInstance.CallFunction(2, 1);

			bool _bResult = luaInstance.PopBoolean();

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			sc::writeLogInfo( "[ Instance Log ] [ Callback - luaAgentRequestJoin ]" );
			sc::writeLogInfo( sc::string::format( "[ Instance Log ] -> [ Instance ID : %1% ]", instanceMapID ) );
			sc::writeLogInfo( sc::string::format( "[ Instance Log ] -> [ Map ID : %1% / %2% ]", keyMapID.Mid(), keyMapID.Sid() ) );
#endif

			return _bResult;
		}

		// ��ũ��Ʈ �ʿ� �Լ��� ���� �Ǿ� ���� ���� ��� ���� �˻� ���� ����;
		return true;
	}

	const bool InstanceAgent::requestEntry(const DBNUM _dbNum, const bool _bRequestByGate)
	{		
		if ( InstanceAgent::isState(InstanceSystem::EMINSTANCE_RESERVE_DESTROY) == true )
			return false; // ���� �����̸� �ź�;

		if ( InstanceAgent::isState(InstanceSystem::EMINSTANCE_JOIN_LOCK) == true )
			return false; // ���� �Ұ� ������ ��� �ź�;

		if ( this->nCurrentPlayer >= this->nMaxPlayer )
			return false; // ��� �ο��� �� á���� �ź�;

		if ( this->luaInstance.PushFunctionEx("luaAgentRequestEntry") == true )
		{
			this->luaInstance.PushInteger(_dbNum);
			this->luaInstance.PushInteger(_bRequestByGate);
			this->luaInstance.CallFunction(2, 1);

			bool _bResult = luaInstance.PopBoolean();
			
#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			sc::writeLogInfo( "[ Instance Log ] [ Callback - luaAgentRequestEntry ]" );
			sc::writeLogInfo( sc::string::format( "[ Instance Log ] -> [ Instance ID : %1% ]", instanceMapID ) );
			sc::writeLogInfo( sc::string::format( "[ Instance Log ] -> [ Map ID : %1% / %2% ]", keyMapID.Mid(), keyMapID.Sid() ) );
#endif

			return _bResult;
		}

		// ��ũ��Ʈ �ʿ� �Լ��� ���� �Ǿ� ���� ���� ��� ���� �˻� ���� ����;
		return true;
	}

	void InstanceAgent::eventCreate(const InstanceMapID _instanceMapID, const DWORD _dwParam1, const DWORD _dwParam2, const DWORD _dwParam3, const DWORD _dwParam4)
	{
		if ( this->luaInstance.PushFunctionEx("luaAgentEventCreate") == true )
		{
#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			sc::writeLogInfo( "[ Instance Log ] [ Callback - luaAgentEventCreate ]" );
			sc::writeLogInfo( sc::string::format( "[ Instance Log ] -> [ Instance ID : %1% ]", instanceMapID ) );
			sc::writeLogInfo( sc::string::format( "[ Instance Log ] -> [ Map ID : %1% / %2% ]", keyMapID.Mid(), keyMapID.Sid() ) );
#endif

			this->luaInstance.PushInteger(_instanceMapID);
			this->luaInstance.PushInteger(_dwParam1);
			this->luaInstance.PushInteger(_dwParam2);
			this->luaInstance.PushInteger(_dwParam3);
			this->luaInstance.PushInteger(_dwParam4);
			this->luaInstance.CallFunction(5, 0);
		}
	}
	
	const InformationPosition InstanceAgent::eventJoin(const DBNUM _dbNum, const InformationPosition& _informationPosition, const EMAUTHORITY _emAuthority)
	{
		if ( this->luaInstance.PushFunctionEx("luaAgentEventJoin") == true )
		{
			this->luaInstance.PushInteger(_dbNum);
			this->luaInstance.PushInteger(_emAuthority);
			this->luaInstance.CallFunction(2, 2);

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			sc::writeLogInfo( "[ Instance Log ] [ Callback - luaAgentEventJoin ]" );
			sc::writeLogInfo( sc::string::format( "[ Instance Log ] -> [ Instance ID : %1% ]", instanceMapID ) );
			sc::writeLogInfo( sc::string::format( "[ Instance Log ] -> [ Map ID : %1% / %2% ]", keyMapID.Mid(), keyMapID.Sid() ) );
#endif
			float fResultValue2 = luaInstance.PopNumber();
			float fResultValue1 = luaInstance.PopNumber();

			// ���ϰ��� ���ٸ� �ʱ� ��ġ ���� ����;
			if ( sc::LuaInstance::INVAID_POP_FLOAT == fResultValue1 &&
				sc::LuaInstance::INVAID_POP_FLOAT == fResultValue2 )
			{
				return _informationPosition;
			}

			if ( sc::LuaInstance::INVAID_POP_FLOAT == fResultValue2 )
			{
				// ���� ���� 1���� ���� ����Ʈ ��ȣ�� ���Ѵ�.;
				const int _nGateID = static_cast< const int >( fResultValue1 );

				return InformationPosition(
					Faction::GLActorID(CROW_PC, _dbNum), 
					SNATIVEID(this->instanceMapID, 0),
					Faction::GLActorID(CROW_GATE, _nGateID), 
					0.0f, 0.0f, InstanceSystem::EMMOVE_FIELD2INSTANCE);
			}
			else
			{
				// ���� ���� 2���� ���� ���� ��ǥ�� ���Ѵ�.;
				const float _fPositionX = fResultValue1;
				const float _fPositionZ = fResultValue2;

				return InformationPosition(
					Faction::GLActorID(CROW_PC, _dbNum),
					SNATIVEID(this->instanceMapID, 0),
					Faction::GLActorID(), 
					_fPositionX, _fPositionZ, InstanceSystem::EMMOVE_FIELD2INSTANCE);
			}
		}

		// �Լ��� ���ٸ� �ʱ� ��ġ ���� ����;
		return _informationPosition;
	}

    const bool InstanceAgent::eventPartyJoin(const DBNUM _masterDbNum, const DBNUM _memberDbNum)
    {
        if ( this->luaInstance.PushFunctionEx("luaAgentEventPartyJoin") == true )
        {
            this->luaInstance.PushInteger(_masterDbNum);
            this->luaInstance.PushInteger(_memberDbNum);
			this->luaInstance.CallFunction(2, 1);

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			sc::writeLogInfo( "[ Instance Log ] [ Callback - luaAgentEventPartyJoin ]" );
			sc::writeLogInfo( sc::string::format( "[ Instance Log ] -> [ Instance ID : %1% ]", instanceMapID ) );
			sc::writeLogInfo( sc::string::format( "[ Instance Log ] -> [ Map ID : %1% / %2% ]", keyMapID.Mid(), keyMapID.Sid() ) );
#endif

			return luaInstance.PopBoolean();
        }

        return true;
    }

    const bool InstanceAgent::eventPartyOut(const DBNUM _dbNum)
    {
        if ( this->luaInstance.PushFunctionEx("luaAgentEventPartryOut") == true )
        {
            this->luaInstance.PushInteger(_dbNum);
            this->luaInstance.CallFunction(1, 1);

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			sc::writeLogInfo( "[ Instance Log ] [ Callback - luaAgentEventPartryOut ]" );
			sc::writeLogInfo( sc::string::format( "[ Instance Log ] -> [ Instance ID : %1% ]", instanceMapID ) );
			sc::writeLogInfo( sc::string::format( "[ Instance Log ] -> [ Map ID : %1% / %2% ]", keyMapID.Mid(), keyMapID.Sid() ) );
#endif

			return luaInstance.PopBoolean();
        }

        return true;
    }

    const bool InstanceAgent::eventPartyDissolution(const DBNUM _dbNum)
    {
        if ( this->luaInstance.PushFunctionEx("luaAgentEventPartryDissolution") == true )
        {
            this->luaInstance.PushInteger(_dbNum);
            this->luaInstance.CallFunction(1, 1);

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			sc::writeLogInfo( "[ Instance Log ] [ Callback - luaAgentEventPartryDissolution ]" );
			sc::writeLogInfo( sc::string::format( "[ Instance Log ] -> [ Instance ID : %1% ]", instanceMapID ) );
			sc::writeLogInfo( sc::string::format( "[ Instance Log ] -> [ Map ID : %1% / %2% ]", keyMapID.Mid(), keyMapID.Sid() ) );
#endif

            return luaInstance.PopBoolean();
        }
        return true;
    }

	void InstanceAgent::eventClubOut(const DWORD _clubDbNum, const DWORD _dbNum, const DWORD _actionActor)
	{
		if ( this->luaInstance.PushFunctionEx("luaAgentEventClubOut") == true )
		{
#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			sc::writeLogInfo( "[ Instance Log ] [ Callback - luaAgentEventClubOut ]" );
			sc::writeLogInfo( sc::string::format( "[ Instance Log ] -> [ Instance ID : %1% ]", instanceMapID ) );
			sc::writeLogInfo( sc::string::format( "[ Instance Log ] -> [ Map ID : %1% / %2% ]", keyMapID.Mid(), keyMapID.Sid() ) );
#endif

			this->luaInstance.PushInteger(_clubDbNum);
			this->luaInstance.PushInteger(_dbNum);
			this->luaInstance.PushInteger(_actionActor);
			this->luaInstance.CallFunction(3, 0);
		}
	}

    void InstanceAgent::eventCustomMessage(const EM_CUSTOM_MESSAGE_TYPE _emSrcType, const DWORD _dwSrcID, const DWORD _Param1, const DWORD _Param2, const DWORD _Param3, const DWORD _Param4)
    {
        if ( this->luaInstance.PushFunctionEx("luaAgentEventCustomMessage") == true )
        {
#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			sc::writeLogInfo( "[ Instance Log ] [ Callback - luaAgentEventCustomMessage ]" );
			sc::writeLogInfo( sc::string::format( "[ Instance Log ] -> [ Instance ID : %1% ]", instanceMapID ) );
			sc::writeLogInfo( sc::string::format( "[ Instance Log ] -> [ Map ID : %1% / %2% ]", keyMapID.Mid(), keyMapID.Sid() ) );
#endif

            this->luaInstance.PushFunction( "luaAgentEventCustomMessage" );
            this->luaInstance.PushInteger((int)_emSrcType);
            this->luaInstance.PushInteger(_dwSrcID);
            this->luaInstance.PushInteger(_Param1);
            this->luaInstance.PushInteger(_Param2);
            this->luaInstance.PushInteger(_Param3);
            this->luaInstance.PushInteger(_Param4);
            this->luaInstance.CallFunction(6, 0);
        }
    }
}
