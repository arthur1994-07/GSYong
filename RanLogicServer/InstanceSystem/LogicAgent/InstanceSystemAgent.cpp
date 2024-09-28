#include "pch.h"

#include "../../../SigmaCore/String/StringUtil.h"
#include "../../../SigmaCore/Log/LogMan.h"
#include "../../../SigmaCore/Util/SystemInfo.h"
#include "../../../SigmaCore/Lua/MinLua.h"
#include "../../../enginelib/DxLand/DxLandDef.h"
#include "../../../RanLogic/GLUseFeatures.h"
#include "../../../RanLogic/Msg/GLContrlInstanceMsg.h"
#include "../../../RanLogic/MatchSystem/MatchModel.h"
#include "../../InstantMap/InstantMapTypes.h"
#include "../../Character/GLCharAG.h"
#include "../../AgentServer/GLAgentServer.h"
#include "../../AgentServer/StateFieldServer/StateFieldServer.h"
#include "../../MatchSystem/MatchingManager.h"
#include "InvokeFunctionAgent.h"
#include "InstanceSystemAgent.h"

namespace InstanceSystem
{
	GLAgentServer* gpAgentServer = 0;
	volatile long nSimpleUniqueKey = 0;
	__forceinline const unsigned int generateSimpleUniqueKey(void)
	{
		::InterlockedIncrement(&nSimpleUniqueKey);
		return unsigned int(nSimpleUniqueKey);
	}
	/*void doMessageFieldServerOut(GLCharAG* _pPlayer, const SNATIVEID& _baseMapID, const SNATIVEID& _targetMapID, const DWORD _dwTargetGateID, const D3DXVECTOR3& _vTargetPosition, const unsigned int _nTargetFieldServer, const unsigned int _nTargetFieldChannel)
	{
		const unsigned int _nPlayerFieldChannel(_pPlayer->m_nChannel);
		const unsigned int _nPlayerFieldServer(_pPlayer->m_dwCurFieldSvr);

		GLMSG::SNETPC_FIELDSVR_OUT NetMsg;
		NetMsg.dwChaNum = _pPlayer->ChaDbNum();
		NetMsg.dwSvrGroupNum = gpAgentServer->GetServerGroup();	
		NetMsg.targetMapID = _targetMapID;
		NetMsg.bSameFieldServer = (_nPlayerFieldChannel == _nTargetFieldChannel) && (_nPlayerFieldServer == _nTargetFieldServer);
		gpAgentServer->SENDTOFIELD(_pPlayer->ClientSlot(), &NetMsg);

		_pPlayer->SetNextFieldSvr(MapID(_baseMapID, _targetMapID), _dwTargetGateID, _vTargetPosition, _nTargetFieldServer);
	}*/


	ReloadState::ReloadState(const unsigned int _nFieldServer, const unsigned int _nFieldChannel, const bool _bSuccess)
		: nFieldServer(_nFieldServer)
		, nFieldChannel(_nFieldChannel)
		, bSuccess(_bSuccess)
	{
	}
	ReloadState::~ReloadState(void)
	{
	}


	MemberAgent::MemberAgent(const DBNUM _dbNum, const EMAUTHORITY _emAuthority)
		: dbNum(_dbNum)
		, emAuthority(_emAuthority)
	{
	}
	MemberAgent::MemberAgent(const MemberAgent& _rhs)
		: dbNum(_rhs.dbNum)
		, emAuthority(_rhs.emAuthority)
		, nState(_rhs.nState)
		, previousMapInformation(_rhs.previousMapInformation)
		, queueInformationPosition(_rhs.queueInformationPosition)
	{
	}
	MemberAgent::~MemberAgent(void)
	{
	}
	






	InstanceAgent::InstanceAgent(const InstanceMapID& _instanceMapID, const SNATIVEID& _keyMapID, const EMINSTANCE_TYPE _emInstanceType, const unsigned int _nFieldServer, const unsigned int _nChannel)
		: instanceMapID(_instanceMapID)
		, keyMapID(_keyMapID)
		, emInstanceType(_emInstanceType)
		, instanceUniqueKey(InstanceSystem::generateSimpleUniqueKey())
		, nFieldServer(_nFieldServer)
		, nFieldChannel(_nChannel)		
        , countCreatingChildLand(0)
		, nCurrentPlayer(0)
		, nMaxPlayer(0)
		, fDestroyTimer(0.0f)
	{	
	}
	InstanceAgent::InstanceAgent(const InstanceAgent& _rhs)
		: instanceMapID(_rhs.instanceMapID)
		, keyMapID(_rhs.keyMapID)
		, emInstanceType(_rhs.emInstanceType)
		, instanceUniqueKey(_rhs.instanceUniqueKey)
		, nFieldServer(_rhs.nFieldServer)
		, nFieldChannel(_rhs.nFieldChannel)
		, luaInstance(_rhs.luaInstance)
		, memberMap(_rhs.memberMap)
        , childInstanceIDVector(_rhs.childInstanceIDVector)
		, nState(_rhs.nState)
        , countCreatingChildLand(_rhs.countCreatingChildLand)
		, nCurrentPlayer(_rhs.nCurrentPlayer)
		, nMaxPlayer(_rhs.nMaxPlayer)
		, fDestroyTimer(0.0f)
	{
		
	}
	InstanceAgent::~InstanceAgent(void)
	{
	
	}
	const bool InstanceAgent::initialize(void)
	{
		InstanceSystem::ScriptManager* const _pScriptManager(InstanceSystem::ScriptManager::getInstance());
		if ( _pScriptManager->getLevelAgentScript(this->instanceMapID, this->keyMapID, this->luaInstance) == -1 )
			return false;

		InstanceSystem::initializeAgentInstanceInvokeFunction(this->luaInstance);
		this->nMaxPlayer = _pScriptManager->getMaxPlayer(this->keyMapID);
		return true;
	}

	



	void InstanceAgent::destroy(void)
	{
		this->luaInstance.OnDestroy();

		GLMSG::NET_INSTANCE_DESTROY _destroyMessage(this->instanceMapID);
		gpAgentServer->SENDTOFIELDSVR(this->nFieldChannel, this->nFieldServer, &_destroyMessage);
	}
	void InstanceAgent::reserveDestroy(void)
	{
		for ( MemberAgentMapIter _iteratorMember(this->memberMap.begin()); _iteratorMember != this->memberMap.end(); ++_iteratorMember )
		{
			MemberAgent& _member(_iteratorMember->second);
			const MapInformation& _mapInformationPrevious(_member.getPreviousMapInformation());

			const Faction::GLActorID _actorID(CROW_PC, _member.getDbNum());
			const SNATIVEID& _targetGaeaMapID(_mapInformationPrevious.targetMapID);
			const Faction::GLActorID& _targetActorID(_mapInformationPrevious.targetActorID);
			const float _fPositionX(_mapInformationPrevious.targetPosition.x);
			const float _fPositionZ(_mapInformationPrevious.targetPosition.z);
			const InformationPosition _informationPosition(_actorID, _targetGaeaMapID, _targetActorID, _fPositionX, _fPositionZ, InstanceSystem::EMMOVE_INSTANCE2FIELD);

			_member.setPosition(_informationPosition);
			_member.setState(InstanceSystem::EMMEMBER_SET_TO_REMOVE);
		}
		InstanceAgent::setState(InstanceSystem::EMINSTANCE_RESERVE_DESTROY);
	}


	void CALLBACK InstanceAgent::createComplete(void)
	{ // field�� ��û�� land�� ���������� ���� �� ��� ȣ�� ��;

		unsigned int& _nCreateCount = this->countCreatingChildLand;

		if ( this->countCreatingChildLand == 0 || this->countCreatingChildLand > MAX_LAND_SID )
		{ // �ڽ� �δ� ���� �Ϸ� �������� _nCreateCount�� �ݵ�� 0���� MAX_LAND_SID ���̿��� �Ѵ�;
			// _nCreateCount ���� ���� ��û�� �� ������ 1�� �ö�;
			gpAgentServer->SENDTOMYSELF(&GLMSG::NET_INSTANCE_DESTROY(this->instanceMapID));
		}
		else
			this->countCreatingChildLand -= 1;

		InstanceAgent::setState(InstanceSystem::EMINSTANCE_CREATE_COMPLETE);
	}
	
	void InstanceAgent::offline(const DBNUM _dbNum)
	{
		MemberAgentMapIter _iteratorMember(this->memberMap.find(_dbNum));
		if ( _iteratorMember == this->memberMap.end() )
			return;

		MemberAgent& _member(_iteratorMember->second);
		_member.offline();		
	}
	
    const bool InstanceAgent::_clearRequestMember(void)
    {
        for ( MemberAgentMapIter _iteratorMember(this->memberMap.begin()); _iteratorMember != this->memberMap.end(); )
        {			
            MemberAgent& _member = _iteratorMember->second;
            if ( _member.isState(InstanceSystem::EMMEMBER_NOT_INITIALIZE) == true )
            { 
				// EMMEMBER_NOT_INITIALIZE �÷��װ� ���� �ִٸ�,
				// ���� ��û�� �ϰ� ���� �ƹ��� ��ġ�� �������� ���� �����̴�;
                _iteratorMember = this->memberMap.erase(_iteratorMember);                

				nCurrentPlayer--;
            }
			else
				++_iteratorMember;
        }

        return true;
    } 
    InstanceBaseAgent::InstanceBaseAgent(const SNATIVEID& _keyMapID)
        : keyMapID(_keyMapID)
        , maxTimer(0)
    {
    }

    InstanceBaseAgent::~InstanceBaseAgent()
    {
    }

    HRESULT InstanceBaseAgent::frameMove(const float fTime, const float fElapsedTime, const CTime& CurrentTime)
    {
        // EventTimer�� eventTimerVector�� ����Ͽ� ȣ�� �ϴ� ������
        // instanceTimerMap for���ȿ��� EventTimerȣ�� �� ��ũ��Ʈ����
        // RemoveTimer�� �Ͽ����� Map�� Ư���� ������ ������ �Ͼ�� ���� Iterater�� ������ �Ǵµ�
        // ��ũ��Ʈ���� ȣ��� �Լ��� erase���� ����� Iter�� �������� ���� ���� ����;
        // ���� regist,remove�� Queueing�� �ϴ��� EventTimer�� Queueing�� �ϴ��� �ؾ��߱� ����;

        // ID�� ������ �Ǵ� Timer Map�� Ÿ�̸ӵ��� �ð��� ��� �Ǿ����� �˻�;
        for ( InstanceAbsTimerMapIter _iter(this->instanceAbsTimerMap.begin()); _iter != this->instanceAbsTimerMap.end(); )
        {
            InstanceAbsTimer& _timer = _iter->second;
            if ( _timer.checkTimer(CurrentTime) == false )
            {
                ++_iter;
                continue;
            }

            const TimerHandle _hTimer(_iter->first);
            eventAbsTimerVector.push_back( _timer );

            // ���� LoopTimer�� ���� ��� ����ǹǷ� ������ �ʴ´�.;
            if ( _timer.bLoop )
            {
                _timer.bUsed = true; // ���Ǿ��� üũ. (���� �����ӿ��� �ٽ� checkTimer�̺�Ʈ�� �߻��ϴ°��� ����);
				++_iter;
            }
            else
            {   
                //Ÿ�̸Ӵ� �ѹ� ���ǰ� ���� ������;
                this->timerHandleQueue.push(_hTimer);
                _iter = this->instanceAbsTimerMap.erase(_iter);
            }
        }

        for ( InstanceTimerMapIter _iter(this->instanceTimerMap.begin()); _iter != this->instanceTimerMap.end(); )
        {
            InstanceTimer& _timer = _iter->second;
            if ( _timer.updateTimer(fElapsedTime) == false )
            {
                ++_iter;
                continue;
            }

            const TimerHandle _hTimer(_iter->first);
            eventTimerVector.push_back( _timer );

            // ���� LoopTimer�� ���� ��� ����ǹǷ� ������ �ʴ´�.( 0��¥�� �ν��Ͻ� Ÿ�̸Ӵ� Loop������ (����ó��) );
            if ( _timer.bLoop && _timer.fLifeTime != 0.0f)
            {
                _timer.fElapsedTime = _timer.fLifeTime;
				++_iter;
            }
            else
            {
                //Ÿ�̸Ӵ� �ѹ� ���ǰ� ���� ������;
                this->timerHandleQueue.push(_hTimer);
                _iter = this->instanceTimerMap.erase(_iter);
            }
        }

        if ( eventTimerVector.empty() == false )
        {
            sc::LuaInstance* const luaInstance = ScriptManager::getInstance()->getScript(keyMapID);
            for( std::vector< InstanceTimer >::iterator iter = eventTimerVector.begin(); iter != eventTimerVector.end(); iter++ )
            {   
                if ( luaInstance->PushFunctionEx("luaAgentEventTimer") == true )
                {
                    InstanceTimer& _timer = *iter;
                    luaInstance->PushInteger(_timer.hTimerHandle);
                    luaInstance->PushInteger(_timer.m_dwParam0);
                    luaInstance->PushInteger(_timer.m_dwParam1);
                    luaInstance->PushInteger(_timer.m_dwParam2);
                    luaInstance->PushInteger(_timer.m_dwParam3);
                    luaInstance->CallFunction(5,0);
                }
            }
            eventTimerVector.clear();
        }

        if ( eventAbsTimerVector.empty() == false )
        {
            sc::LuaInstance* const luaInstance = ScriptManager::getInstance()->getScript(keyMapID);
            for( std::vector< InstanceAbsTimer >::iterator iter = eventAbsTimerVector.begin(); iter != eventAbsTimerVector.end(); iter++ )
            {   
                if ( luaInstance->PushFunctionEx("luaAgentEventAbsTimer") == true )
                {
                    InstanceAbsTimer& _timer = *iter;
                    luaInstance->PushInteger(_timer.hTimerHandle);
                    luaInstance->PushInteger(_timer.m_dwParam0);
                    luaInstance->PushInteger(_timer.m_dwParam1);
                    luaInstance->PushInteger(_timer.m_dwParam2);
                    luaInstance->PushInteger(_timer.m_dwParam3);
                    luaInstance->CallFunction(5,0);
                }
            }
            eventAbsTimerVector.clear();
        }
        return S_OK;
    }

    const TimerHandle InstanceBaseAgent::registAbsTimer(const WORD wWeek, const WORD wDay, const WORD wHour, const WORD wMinute, const WORD wSecond,
                                                        const DWORD _dwParam0, const DWORD _dwParam1, const DWORD _dwParam2, const DWORD _dwParam3, const bool _bLoop)
    {
        TimerHandle _hTimer;
        if ( this->timerHandleQueue.size() == 0 )
        {
            _hTimer = maxTimer++;
        }
        else
        {
            _hTimer = this->timerHandleQueue.front();
            this->timerHandleQueue.pop();
        }

        InstanceSystem::InstanceAbsTimer _timer(_hTimer, wWeek, wDay, wHour, wMinute, wSecond, _dwParam0, _dwParam1, _dwParam2, _dwParam3, _bLoop);
        InstanceSystem::InstanceAbsTimerValue _pair(_hTimer,_timer);
        this->instanceAbsTimerMap.insert(_pair);

        return _hTimer;
    }

    const TimerHandle InstanceBaseAgent::registTimer(const float _fTime, const DWORD _dwParam0, const DWORD _dwParam1, const DWORD _dwParam2, const DWORD _dwParam3, const bool _bLoop)
    {
        TimerHandle _hTimer;
        if ( this->timerHandleQueue.size() == 0 )
        {
            _hTimer = maxTimer++;
        }
        else
        {
            _hTimer = this->timerHandleQueue.front();
            this->timerHandleQueue.pop();
        }

        InstanceSystem::InstanceTimer _timer(_hTimer, _fTime, _dwParam0, _dwParam1, _dwParam2, _dwParam3, _bLoop);
        InstanceSystem::InstanceTimerValue _pair(_hTimer,_timer);
        this->instanceTimerMap.insert(_pair);

        return _hTimer;
    }

    const bool InstanceBaseAgent::isRegistTimer(const TimerHandle _hTimer) const
    {
        InstanceTimerMapCIter _iteratorTimer(this->instanceTimerMap.find(_hTimer));
        if ( _iteratorTimer != this->instanceTimerMap.end() )
            return true;

        InstanceAbsTimerMapCIter _iteratorAbsTimer(this->instanceAbsTimerMap.find(_hTimer));
        if ( _iteratorAbsTimer != this->instanceAbsTimerMap.end() )
            return true;

        return false;
    }

    const bool InstanceBaseAgent::removeTimer(const TimerHandle _hTimer)
    {
        InstanceTimerMapIter _iteratorTimer(this->instanceTimerMap.find(_hTimer));
        if ( _iteratorTimer != this->instanceTimerMap.end() )
        {
            this->timerHandleQueue.push(_hTimer);
            this->instanceTimerMap.erase(_iteratorTimer);
            return true;
        }

        InstanceAbsTimerMapIter _iteratorAbsTimer(this->instanceAbsTimerMap.find(_hTimer));
        if ( _iteratorAbsTimer != this->instanceAbsTimerMap.end() )
        {
            this->timerHandleQueue.push(_hTimer);
            this->instanceAbsTimerMap.erase(_iteratorAbsTimer);
            return true;
        }
        
        return false;
    }

    // GLAgentServer::Create()���� mapslist�� �� ���� �� �Ʒ� ManagerAgent�� �����ڰ� ȣ�� ��;
    // _bUsed���� mapslist�� ��ϵǾ� ��� ���� mapID�� ���� ������ ��� ����;
    // ��� ���� mapID ���� ID�� ���� instance ������ ��� ��;
    // ������ sID�� instance ID�� ����ѵ� ����, MID�� �����Ͽ� ���;
    // sID �� stage ID�� ��ȯ �����ν�, �Ϲ� field map�� 1 Land n Stage ������ ��������;
    ManagerAgent::ManagerAgent(GLAgentServer* const _pAgentServer, const bool* const _bUsed /* size = MAX_LAND_MID */)
		: pScriptManager(InstanceSystem::ScriptManager::getInstance())
        , fUpdateTimer(0.0f)
		, packetCounter(InstanceSystem::EMMESSAGE_NSIZE)
	{
		gpAgentServer =_pAgentServer;

        for ( DWORD _i(MAX_LAND_MID); _i--; )
        {
            if ( _bUsed[_i] == true )
                continue; // ��� ���� mapID�� �Ѿ;

            this->instanceMapIDQueue.push(_i);		
        }

		const InstanceSystem::MapIDVector& _keyMapIDVector = this->pScriptManager->getKeyMapIDVector();
		for ( InstanceSystem::MapIDVectorCIter _iteratorKeyMapID(_keyMapIDVector.begin()); _iteratorKeyMapID != _keyMapIDVector.end(); ++_iteratorKeyMapID )
		{
			const SNATIVEID& _keyMapID(*_iteratorKeyMapID);
			const unsigned int _nMaxCreate(this->pScriptManager->getMaxCreate(_keyMapID));
			const InstanceLog _instanceLog(_keyMapID, 0, 0, _nMaxCreate);			

			this->instanceLogMap.insert(InstanceLogValue(_keyMapID, _instanceLog));
		}		
		m_pMatchingCallBackTriggerManager = MatchingSystem::MatchingCallbackTriggerManager::GetInstance();
	}
	ManagerAgent::~ManagerAgent(void)
	{
	}
	
    void ManagerAgent::initialize()
    {
        instanceBaseMap.clear();
        const MapIDVector& _mapIDvector = this->pScriptManager->getKeyMapIDVector();        
        for ( MapIDVectorCIter _iter = _mapIDvector.begin(); _iter != _mapIDvector.end(); _iter++ )
        {
            sc::LuaInstance* const _pLuaInstance = this->pScriptManager->getScript(*_iter);
            if ( _pLuaInstance )
            {
                instanceBaseMap.insert(InstanceBaseAgentValue(*_iter, InstanceBaseAgent(*_iter)));
            }
        }

        InstanceSystem::initializeAgentPublicInvokeFunction();
    }

	/*
	 * ���� ��Ī �ý����� ���⿡, �ӽ÷� �̰����� ��Ī �ý��� �븮 ������ ��;
	*/
	HRESULT ManagerAgent::frameMove(const float _fTime, const float _fElapsedTime, const CTime& _currentTime)
	{
		if ( GLUseFeatures::GetInstance().IsUsingNewInstance() == false )
			return S_OK;

		if ( this->pScriptManager->isState(InstanceSystem::EMINSTANCE_SCRIPT_RELOAD) == true )
			return S_OK; // reloading ���̸� ó������ �ʴ´�;

        //this->pScriptManager->pushCallFunction("luaAgentEventUpdate");

        for ( InstanceBaseAgentMapIter _iteratorBase(this->instanceBaseMap.begin()); _iteratorBase != this->instanceBaseMap.end(); ++_iteratorBase )
        {			
            InstanceBaseAgent& _baseAgent = _iteratorBase->second;
            _baseAgent.frameMove(_fTime, _fElapsedTime, _currentTime);
        }

		for ( InstanceAgentMapIter _iteratorInstance(this->instanceMap.begin()); _iteratorInstance != this->instanceMap.end(); ++_iteratorInstance )
		{			
			InstanceAgent& _instanceAgent = _iteratorInstance->second;
            _instanceAgent.frameMove(_fTime, _fElapsedTime, _currentTime);
		}			

		MatchingSystem::RoomMultyTriggerVectorIter _biter = m_pMatchingCallBackTriggerManager->m_RoomMultyTriggerList.begin();
		for(;_biter != m_pMatchingCallBackTriggerManager->m_RoomMultyTriggerList.end() ;)
		{
			MatchingSystem::RoomMultyTrigger* m_roomMultyTrigger = *_biter;
			if(m_roomMultyTrigger->m_frameMove + _fElapsedTime > 120)
			{
				sc::writeLogError("MatchingInstanceBase : OnMapMultyCreate TIMEOUT");

				MatchingSystem::RoomMultyReceipt receipt;
				for( unsigned int i = 0 ; i < m_roomMultyTrigger->_bCreateComplete.size(); i ++)
				{
					receipt._bCreateComplete.push_back(m_roomMultyTrigger->_bCreateComplete[i]);
					receipt._instanceMapID.push_back(m_roomMultyTrigger->_vecInstanceMapID[i]);
				}

				_biter = m_pMatchingCallBackTriggerManager->m_RoomMultyTriggerList.erase(_biter);
				//��ϵ� �ݺ��� ã�ư���!.
				m_roomMultyTrigger->Call(&receipt);

				SAFE_DELETE(m_roomMultyTrigger);
				continue;
			}
			else
			{
				m_roomMultyTrigger->m_frameMove += _fElapsedTime;
			}
			++_biter;

		}

		return S_OK;
	}

	const bool ManagerAgent::doScript(const DBNUM _dbNum, const SNATIVEID& _keyMapID, const unsigned int _nGateID, const unsigned int _nToGateID)
	{
		if ( this->pScriptManager->isExist(_keyMapID) == false )
			return false;
		
		if ( this->pScriptManager->isState(InstanceSystem::EMINSTANCE_SCRIPT_RELOAD) == true )
		{ // reloading ���̸� ó������ �ʴ´�;
			GLMSG::NET_INSTANCE_ERROR _errorMessage(InstanceSystem::EMERROR_RELOAD_SCRIPT);
			ManagerAgent::_sendMessageToClient(_dbNum, &_errorMessage);
			return false;
		}

		if ( this->pScriptManager->isState(_keyMapID, InstanceSystem::EMINSTANCE_SCRIPT_USE) == false )
		{ // �δ��� ���� ���� ���°� �ƴϸ� ����;
			GLMSG::NET_INSTANCE_ERROR _errorMessage(InstanceSystem::EMERROR_CANNOT_CREATE_INSTANCE, _keyMapID);
			ManagerAgent::_sendMessageToClient(_dbNum, &_errorMessage);
			return false;
		}

		// �δ� ���� ������ �ɷ����� ��� �����Ѵ�;
		if ( this->pScriptManager->isState(_keyMapID, InstanceSystem::EMINSTANCE_SCRIPT_JOIN_LOCK) == true )
		{
			GLMSG::NET_INSTANCE_ERROR _errorMessage(InstanceSystem::EMERROR_REJECT_JOIN, _keyMapID);
			ManagerAgent::_sendMessageToClient(_dbNum, &_errorMessage);
			return false;
		}

		// �̹� Matching �� �� �ִ� ��� �����Ѵ�;
		if ( MatchingSystem::MatchingCore::getinstance()->IsAreadyJoin( _dbNum ) )
		{
			GLMSG::NET_INSTANCE_ERROR _errorMessage(InstanceSystem::EMERROR_ALREADY_IN_INSTANCE, _keyMapID);
			ManagerAgent::_sendMessageToClient(_dbNum, &_errorMessage);
			return false;
		}

		GLCharAG* const _pRequester(gpAgentServer->GetCharByDbNum(_dbNum));
		if ( _pRequester == 0 )
			return false;

		// �̹� ������ ���� ���̸� �����Ѵ�;
		if ( _pRequester->IsCtfPlayer() )
		{
			GLMSG::NET_INSTANCE_ERROR _errorMessage(InstanceSystem::EMERROR_ALREADY_IN_INSTANCE, _keyMapID);
			ManagerAgent::_sendMessageToClient(_dbNum, &_errorMessage);
			return false;
		}

		if ( this->memberInstanceMap.find(_dbNum) != this->memberInstanceMap.end() )
		{ // �δ� �ȿ��� �ٸ� �δ����� �̵� �� �� ����;
			GLMSG::NET_INSTANCE_ERROR _errorMessage(InstanceSystem::EMERROR_CANNOT_CREATE_INSTANCE, _keyMapID);
			ManagerAgent::_sendMessageToClient(_dbNum, &_errorMessage);
			return false;
		}

		if ( ManagerAgent::requestEntry(_dbNum, _keyMapID, _nGateID, _nToGateID) == true )
			return true; // �߰� ���� or ������ ������ �δ��� �ִ°�?;

		// requestCreate/createInstance : �� loop �ȿ��� ȣ��Ǿ�� �Ѵ�;
		// ���� �Ǳ� ���� requestJoin�� �� ȣ�� �Ǵ� ��� �� ���� �Ϸ� ��;
		const bool _bRequestByGate(_nGateID != GAEAID_NULL);
		DWORD _createParam1(0);
		DWORD _createParam2(0);
		DWORD _createParam3(0);
		DWORD _createParam4(0);
		if ( ManagerAgent::requestCreate(_dbNum, _keyMapID, _bRequestByGate, _createParam1, _createParam2, _createParam3, _createParam4) == false )
		{ // ���� ������ �����ΰ�?;
			GLMSG::NET_INSTANCE_ERROR _errorMessage(InstanceSystem::EMERROR_CANNOT_CREATE_INSTANCE, _keyMapID);
			ManagerAgent::_sendMessageToClient(_dbNum, &_errorMessage);
			return false;
		}

		const InstanceMapID _instanceMapID(ManagerAgent::createInstance(_keyMapID, _createParam1, _createParam2, _createParam3, _createParam4));
		if ( _instanceMapID == InstanceMapID(-1) )
		{ // ������ ���� �Ͽ��°�?;
			GLMSG::NET_INSTANCE_ERROR _errorMessage(InstanceSystem::EMERROR_FAILED_TO_CREATE_INSTANCE, _keyMapID);
			ManagerAgent::_sendMessageToClient(_dbNum, &_errorMessage);
			return false;
		}

		InstanceAgentMapIter _iteratorInstance( this->instanceMap.find( _instanceMapID ) );
		if ( _iteratorInstance == this->instanceMap.end() )
		{
			return false;
		}

		InstanceAgent& _instanceAgent = _iteratorInstance->second;

		const InformationPosition _informationPosition(
			Faction::GLActorID(CROW_PC, _dbNum),
			SNATIVEID(_instanceMapID, 0),
			Faction::GLActorID(CROW_GATE, _nToGateID),
			0.0f, 0.0f, InstanceSystem::EMMOVE_FIELD2INSTANCE);

		if ( ManagerAgent::reserveMember(_dbNum, _informationPosition, InstanceSystem::EMAUTHORITY_NORMAL) == false )
		{
			sc::writeLogError( "[ Instance Log ] [ Don't recommend logic. This logic will do better to limited create. ]" );

			// ���� ���� �� �ߴµ� ����Ұ��� ��� �δ��� �ı��Ѵ�;
			// ������ �̰����� ������ �ʴ°��� ����;
			// �̷��Ÿ� �׳� �������� ���ϵ��� ���ƶ�;
			_instanceAgent.reserveDestroy();
			//gpAgentServer->DeleteInstantMap(_instanceMapID);

			GLMSG::NET_INSTANCE_ERROR _errorMessage(InstanceSystem::EMERROR_REJECT_JOIN, _keyMapID);
			ManagerAgent::_sendMessageToClient(_dbNum, &_errorMessage);

			return false;
		}

		return true;
	}	

	const InstanceMapID ManagerAgent::createInstance(const SNATIVEID& _keyMapID, const DWORD _dwParam1, const DWORD _dwParam2, const DWORD _dwParam3, const DWORD _dwParam4)
	{
        const SNATIVEID _baseMapID(ManagerAgent::requestMapID(_keyMapID));
        if ( _baseMapID == SNATIVEID(false) )
		{
			CString strTemp;
			strTemp.Format("MapCreate Fail :: MAP ID INVAILD (%d/%d)",
				_keyMapID.wMainID, _keyMapID.wSubID );
			sc::writeLogError(strTemp.GetString());
            return InstanceMapID(-1);	
		}

        if ( this->instanceMapIDQueue.size() == 0 )
		{
			CString strTemp;
			strTemp.Format("MapCreate Fail :: MAP IS NOT LOADED (%d/%d)",
				_keyMapID.wMainID, _keyMapID.wSubID );
			sc::writeLogError(strTemp.GetString());
            return InstanceMapID(-1);
		}

        if ( this->pScriptManager->isState(_keyMapID, EMINSTANCE_SCRIPT_USE) == false )
		{
			CString strTemp;
			strTemp.Format("MapCreate Fail :: CAN NOT USE INSTANCE_SCRIPT (%d/%d)",
				_keyMapID.wMainID, _keyMapID.wSubID );
			sc::writeLogError(strTemp.GetString());
            return InstanceMapID(-1); // ��� �Ұ� ���¸� ����;
		}

		const SFIELDSERVER_INFO sFieldInfo = gpAgentServer->getStateFieldServer()->RequestFreeServerInfo();
		if ( false == sFieldInfo.IsValid() )
		{
			CString strTemp;
			strTemp.Format( "MapCreate Fail :: Field Server Not enough FreeServerID (%d/%d)",
				_keyMapID.wMainID, _keyMapID.wSubID );
			sc::writeLogError( strTemp.GetString() );

			return InstanceMapID(-1);
		}

		// ������ �Ǿ��ٰ� �ؼ� �ٷ� ���� �� �� �ִ°� �ƴϴ�;
		// field server ������ ���� ��û�� ������ �Ϸ� �޼����� �ö����� ���� ��� ����;
        const InstanceMapID _instanceMapID(gpAgentServer->CreateInstantMap(_baseMapID, sFieldInfo.nChannelNum, sFieldInfo.nFieldNum));
        if ( _instanceMapID >= MAX_LAND_MID )
		{
			CString strTemp;
			strTemp.Format("MapCreate Fail :: CreateInstantMap Fail (%d/%d)",
				_keyMapID.wMainID, _keyMapID.wSubID );
			sc::writeLogError(strTemp.GetString());
            return InstanceMapID(-1);
		}

		GLAGLandManager* const _pLandManager(gpAgentServer->GetLandManager(_instanceMapID));
		if ( _pLandManager == 0 )
		{
			CString strTemp;
			strTemp.Format("MapCreate Fail :: Land Does Not Exist(%d/%d)",
				_keyMapID.wMainID, _keyMapID.wSubID );
			sc::writeLogError(strTemp.GetString());
			return InstanceMapID(-1);
		}

		{
			InstanceAgent _instanceAgent(_instanceMapID, _keyMapID, InstanceSystem::EMINSTANCE_NORMAL, sFieldInfo.nFieldNum, sFieldInfo.nChannelNum);
			const bool _bSuccess(_instanceAgent.initialize());
			if ( _bSuccess == false )
			{
				_instanceAgent.reserveDestroy();
				//gpAgentServer->DeleteInstantMap(_instanceMapID);

				CString strTemp;
				strTemp.Format("MapCreate Fail :: Instance Script Initialize Fail (%d/%d)",
					_keyMapID.wMainID, _keyMapID.wSubID );
				sc::writeLogError(strTemp.GetString());
				return InstanceMapID(-1);
			}

			_instanceAgent.addChildInstance(_baseMapID);
			this->instanceMap.insert(InstanceAgentValue(_instanceMapID, _instanceAgent));
		}

		{
			InstanceAgentMapIter _iteratorInstance(this->instanceMap.find(_instanceMapID));
			InstanceAgent& _instanceAgent = _iteratorInstance->second;
			_pLandManager->setScriptInstance(&_instanceAgent);

			// �δ� ������ EventCreate �Լ� ȣ��;
			_instanceAgent.eventCreate(_instanceMapID, _dwParam1, _dwParam2, _dwParam3, _dwParam4);
		}

        GLMSG::NET_INSTANCE_CREATE _createMessage(_keyMapID, _instanceMapID, _baseMapID, InstanceSystem::EMINSTANCE_NORMAL);
        gpAgentServer->SENDTOFIELDSVR(sFieldInfo.nChannelNum, sFieldInfo.nFieldNum, &_createMessage);

        return _instanceMapID;
	}

	// ���� �Ǿ����� Ʈ���ŷ� �δ��� �˸�.
	const bool ManagerAgent::createInstanceByTrigger(MatchingSystem::RoomTrigger* _trigger)
	{
		if ( _trigger == NULL )
			return false;

		const InstanceMapID _instanceMapID= createInstance(_trigger->_instanceID);
		if ( _instanceMapID == InstanceMapID(-1) )
		{
			return false;
		}
		
		InstanceAgentMapIter _iteratorInstance(this->instanceMap.find(_instanceMapID));
		if ( _iteratorInstance == this->instanceMap.end() )
		{
			return false;
		}

		MatchingSystem::MatchingCallbackTriggerManager* m_pMatchingCallBackTriggerManager = MatchingSystem::MatchingCallbackTriggerManager::GetInstance();

		if(m_pMatchingCallBackTriggerManager == NULL)
			return true;
		m_pMatchingCallBackTriggerManager->m_RoomTriggerList.push_back(_trigger);

		return true;
	}

	const bool ManagerAgent::createInstanceByTrigger(MatchingSystem::RoomMultyTrigger* _trigger)
	{
		if ( _trigger == NULL )
			return false;

		const InstanceMapID _instanceMapID= createInstance(_trigger->_instanceID);
		if ( _instanceMapID == InstanceMapID(-1) )
		{
			return false;
		}

		InstanceAgentMapIter _iteratorInstance(this->instanceMap.find(_instanceMapID));
		if ( _iteratorInstance == this->instanceMap.end() )
		{
			return false;
		}

		MatchingSystem::MatchingCallbackTriggerManager* m_pMatchingCallBackTriggerManager = MatchingSystem::MatchingCallbackTriggerManager::GetInstance();

		if(m_pMatchingCallBackTriggerManager == NULL)
			return true;

		MatchingSystem::RoomMultyTrigger* m_roomMultyTrigger = NULL;

		MatchingSystem::RoomMultyTriggerVectorIter _biter = m_pMatchingCallBackTriggerManager->m_RoomMultyTriggerList.begin();

		for(;_biter != m_pMatchingCallBackTriggerManager->m_RoomMultyTriggerList.end(); ++_biter)
		{
			m_roomMultyTrigger = *_biter;
			if(m_roomMultyTrigger == _trigger)
			{
				return true;
			}
		}

		m_pMatchingCallBackTriggerManager->m_RoomMultyTriggerList.push_back(_trigger);

		return true;
	}

	void ManagerAgent::_sendMessageToClient(const DBNUM _dbNum, NET_MSG_GENERIC* _pMessage)
	{
		GLCharAG* const _pPlayer(gpAgentServer->GetCharByDbNum(_dbNum));
		if ( _pPlayer == 0 )
			return;

		gpAgentServer->SENDTOCLIENT(_pPlayer->ClientSlot(), _pMessage);
	}

	const bool ManagerAgent::reserveMember(const DBNUM _dbNum, const InformationPosition& _informationPosition, const EMAUTHORITY _emAuthority /* = EMAUTHORITY_NORMAL */)
	{
		GLCharAG* const _pRequester(gpAgentServer->GetCharByDbNum(_dbNum));
		if ( _pRequester == 0 )
			return false;

		const InstanceMapID _instanceMapID(_informationPosition.targetMapID.Mid());

		/// �δ��� ������� �̵��� �� ���� ( Field ���� Instance �̵��� ��� ���ȴ� );
		if ( false == _pRequester->GetCurrentMap().isSameID() )
		{
			GLMSG::NET_INSTANCE_ERROR _errorMessage(
				InstanceSystem::EMERROR_CANNOT_JOIN_INSTANCE,
				SNATIVEID(false),
				_instanceMapID );
			ManagerAgent::_sendMessageToClient( _dbNum, &_errorMessage );

			return false;
		}

		InstanceAgentMapIter _iteratorInstance(this->instanceMap.find(_instanceMapID));
		if ( _iteratorInstance == this->instanceMap.end() )
		{
			CString strTemp;
			strTemp.Format("ManagerAgent Fail :: EMERROR_NOT_EXIST_INSTANCE %d(user:%d)", _instanceMapID,_dbNum);
			sc::writeLogError(strTemp.GetString());
			GLMSG::NET_INSTANCE_ERROR _errorMessage(InstanceSystem::EMERROR_NOT_EXIST_INSTANCE, SNATIVEID(false), _instanceMapID);
			ManagerAgent::_sendMessageToClient(_dbNum, &_errorMessage);
			return false; // �ش� �δ��� ���� ���� ����;
		}		

		InstanceAgent& _instanceAgent(_iteratorInstance->second);
		
		const bool _bResult(_instanceAgent.reserveMember(_dbNum, _informationPosition, _emAuthority));
        if ( _bResult == false )
            return false;

		return true;
	}

	const InstanceSystem::EMAUTHORITY ManagerAgent::getAuthority(const DBNUM _dbNum) const
	{
		MemberInstanceAgentMapCIter _iteratorMember(this->memberInstanceMap.find(_dbNum));
		if ( _iteratorMember == this->memberInstanceMap.end() )
			return InstanceSystem::EMAUTHORITY_NORMAL;

		const InstanceSystem::InstanceIDSet& _instanceIDSet(_iteratorMember->second);
		const InstanceSystem::InstanceMapID _instanceMapID(_instanceIDSet.instanceMapID);

		InstanceAgentMapCIter _iteratorInstance(this->instanceMap.find(_instanceMapID));
		if ( _iteratorInstance == this->instanceMap.end() )
			return InstanceSystem::EMAUTHORITY_NORMAL;

		const InstanceSystem::InstanceAgent& _instance(_iteratorInstance->second);
		return _instance.getAuthority(_dbNum);
	}

    const bool ManagerAgent::offline(const DBNUM _dbNum)
    {
        _removeMember(_dbNum);
        for ( InstanceAgentMapIter _iteratorInstance(this->instanceMap.begin()); _iteratorInstance != this->instanceMap.end(); ++_iteratorInstance )
        {
            InstanceAgent& _instanceAgent = _iteratorInstance->second;
            _instanceAgent.offline(_dbNum);
        }
        return true;
    }
	
    const bool ManagerAgent::_joinLock(const InstanceMapID& _instanceMapID, const bool _bLock)
    {
        InstanceAgentMapIter& _iteratorInstance(this->instanceMap.find(_instanceMapID));
        if ( _iteratorInstance == this->instanceMap.end() )
            return false;

        InstanceAgent& _instanceAgent(_iteratorInstance->second);
		_instanceAgent.setState(InstanceSystem::EMINSTANCE_JOIN_LOCK, _bLock);

        return true;
    }

	const bool ManagerAgent::_joinLock(const SNATIVEID& _keyMapID, const bool _bLock)
	{
		if ( this->pScriptManager->isExist(_keyMapID) == false )
			return false; // ��ȿ�� keyMapID�� �ƴ϶�� ����;

		for ( InstanceAgentMapIter _iteratorInstance(this->instanceMap.begin()); _iteratorInstance != this->instanceMap.end(); ++_iteratorInstance )
		{
			InstanceAgent& _instanceAgent(_iteratorInstance->second);
			if ( _instanceAgent.getKeyMapID() != _keyMapID )
				continue;

			_instanceAgent.setState(InstanceSystem::EMINSTANCE_JOIN_LOCK, _bLock);
		}

		this->pScriptManager->setState(_keyMapID, InstanceSystem::EMINSTANCE_SCRIPT_JOIN_LOCK, _bLock);

		return true;
	}

	const bool ManagerAgent::_joinSuspend(const InstanceMapID& _instanceMapID, const bool _bSuspend)
	{
		InstanceAgentMapIter& _iteratorInstance(this->instanceMap.find(_instanceMapID));
		if ( _iteratorInstance == this->instanceMap.end() )
			return false;

		InstanceAgent& _instanceAgent = _iteratorInstance->second;
		_instanceAgent.setState(InstanceSystem::EMINSTANCE_JOIN_SUSPEND, _bSuspend);		

		return true;
	}	

	/*
	 * �Ʒ� �Լ��� �����ϰ� �δ� ������ �̵��ϰ� �Ǹ� ȣ�� �ȴ�;
	 * ��� ������ �������� �ϴ� ��쿡��,
	 * _instanceAgent.removeMember() �Լ��� ���ؼ� ������ �������� �Ѵ�;
	*/
	const bool CALLBACK ManagerAgent::_removeMember(const DBNUM _dbNum)
	{
		MemberInstanceAgentMapCIter _iteratorMember(this->memberInstanceMap.find(_dbNum));
		if ( _iteratorMember != this->memberInstanceMap.end() )
		{ // �ش� �δ��� ���� �Ǳ� ������ ���� �δ��� ���� ������ �ʿ� �ϹǷ� ������ �ƴ�, �� ���Ÿ� ����;
			const InstanceIDSet& _instanceIDSet(_iteratorMember->second);
			InstanceAgentMapIter _iteratorInstance(this->instanceMap.find(_instanceIDSet.instanceMapID));
			if ( _iteratorInstance != this->instanceMap.end() )
			{
				InstanceAgent& _instanceAgent(_iteratorInstance->second);
				_instanceAgent.setStateMember(_dbNum, InstanceSystem::EMMEMBER_SET_TO_REMOVE);
			}
			this->memberInstanceMap.erase(_iteratorMember);
		}
		return true;
	}	

    const bool ManagerAgent::reload(const DWORD _dwGaeaID)
    {
		if ( this->pScriptManager->isState(InstanceSystem::EMINSTANCE_SCRIPT_RELOAD) == true )
			return false;
		

		this->pScriptManager->setState(InstanceSystem::EMINSTANCE_SCRIPT_RELOAD);
        if ( this->pScriptManager->reload() == -1 )
			return false;

        ManagerAgent::initialize();

        GLMSG::NET_INSTANCE_RELOAD_AF _reloadMessage(_dwGaeaID);			
		this->nReloadServerGroup = gpAgentServer->SENDTOALLCHANNEL(&_reloadMessage);
		this->reloadStateVector.clear();

        return true;
    }



	void ManagerAgent::CallTrigger(const SNATIVEID& _MapID,const InstanceMapID& instanceMapID,	const bool bSuccess)
	{

		MatchingSystem::MatchingCallbackTriggerManager* m_pMatchingCallBackTriggerManager = MatchingSystem::MatchingCallbackTriggerManager::GetInstance();

		if(m_pMatchingCallBackTriggerManager == NULL)
			return;

		if(m_pMatchingCallBackTriggerManager->m_RoomTriggerList.size() != 0)
		{
			MatchingSystem::RoomTriggerVectorIter biter = m_pMatchingCallBackTriggerManager->m_RoomTriggerList.begin();

			for(;biter != m_pMatchingCallBackTriggerManager->m_RoomTriggerList.end(); )
			{
				MatchingSystem::RoomTrigger* m_roomTrigger = *biter;
				if(m_roomTrigger->_instanceID == _MapID)
				{
					biter = m_pMatchingCallBackTriggerManager->m_RoomTriggerList.erase(biter);
					MatchingSystem::RoomReceipt receipt;
					receipt._bCreateComplete = bSuccess;
					receipt._instanceMapID = instanceMapID;

					m_roomTrigger->Call(&receipt);
					SAFE_DELETE(m_roomTrigger);

					return;
				}
				else
				{
					++biter;
				}
			}
		}


		MatchingSystem::RoomMultyTrigger* m_roomMultyTrigger = NULL;

		if(m_pMatchingCallBackTriggerManager->m_RoomMultyTriggerList.size() == 0)
			return;

		MatchingSystem::RoomMultyTriggerVectorIter _biter = m_pMatchingCallBackTriggerManager->m_RoomMultyTriggerList.begin();
		{			
			for(;_biter != m_pMatchingCallBackTriggerManager->m_RoomMultyTriggerList.end(); )
			{
				m_roomMultyTrigger = *_biter;
				if(m_roomMultyTrigger->_instanceID == _MapID)
				{
					// ���� ��ŭ���� üũ!
					m_roomMultyTrigger->_bCreateComplete.push_back(bSuccess);
					m_roomMultyTrigger->_vecInstanceMapID.push_back(instanceMapID);
					break;
				}
				else
				{
					++_biter;
				}
			}
		}


		// ������� ��������� ����!
		if(m_roomMultyTrigger != NULL && _biter != m_pMatchingCallBackTriggerManager->m_RoomMultyTriggerList.end() )
		{
			if(m_roomMultyTrigger->_MapNeedSize <= m_roomMultyTrigger->_bCreateComplete.size())
			{
				MatchingSystem::RoomMultyReceipt receipt;
				for( unsigned int i = 0 ; i < m_roomMultyTrigger->_bCreateComplete.size(); i ++)
				{
					receipt._bCreateComplete.push_back(m_roomMultyTrigger->_bCreateComplete[i]);
					receipt._instanceMapID.push_back(m_roomMultyTrigger->_vecInstanceMapID[i]);
				}

				_biter = m_pMatchingCallBackTriggerManager->m_RoomMultyTriggerList.erase(_biter);
				//��ϵ� �ݺ��� ã�ư���!.
				m_roomMultyTrigger->Call(&receipt);
				SAFE_DELETE(m_roomMultyTrigger);
			}
			else
			{
				m_roomMultyTrigger->m_frameMove = 0;
				_biter = m_pMatchingCallBackTriggerManager->m_RoomMultyTriggerList.erase(_biter);
				InstanceSystem::ManagerAgent* pmanager = gpAgentServer->getInstanceManager();
				pmanager->createInstanceByTrigger(m_roomMultyTrigger);
			}
		}
	}

	

}

