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
	{ // field에 요청한 land가 정상적으로 생성 된 경우 호출 됨;

		unsigned int& _nCreateCount = this->countCreatingChildLand;

		if ( this->countCreatingChildLand == 0 || this->countCreatingChildLand > MAX_LAND_SID )
		{ // 자식 인던 생성 완료 시점에서 _nCreateCount는 반드시 0에서 MAX_LAND_SID 사이여야 한다;
			// _nCreateCount 값은 생성 요청이 올 때마다 1씩 올라감;
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
				// EMMEMBER_NOT_INITIALIZE 플래그가 켜져 있다면,
				// 입장 신청만 하고 아직 아무런 조치가 취해지지 않은 상태이다;
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
        // EventTimer를 eventTimerVector를 사용하여 호출 하는 이유는
        // instanceTimerMap for문안에서 EventTimer호출 시 스크립트에서
        // RemoveTimer를 하였을때 Map의 특성상 데이터 정렬이 일어나고 따라서 Iterater가 변경이 되는데
        // 스크립트에서 호출된 함수라서 erase후의 변경된 Iter를 돌려받을 수가 없기 때문;
        // 따라서 regist,remove를 Queueing을 하던지 EventTimer를 Queueing을 하던지 해야했기 때문;

        // ID로 관리가 되는 Timer Map의 타이머들의 시간이 경과 되었는지 검사;
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

            // 만약 LoopTimer의 경우는 계속 재사용되므로 지우지 않는다.;
            if ( _timer.bLoop )
            {
                _timer.bUsed = true; // 사용되었음 체크. (다음 프레임에서 다시 checkTimer이벤트가 발생하는것을 방지);
				++_iter;
            }
            else
            {   
                //타이머는 한번 사용되고 나면 버린다;
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

            // 만약 LoopTimer의 경우는 계속 재사용되므로 지우지 않는다.( 0초짜리 인스턴스 타이머는 Loop사용금지 (삭제처리) );
            if ( _timer.bLoop && _timer.fLifeTime != 0.0f)
            {
                _timer.fElapsedTime = _timer.fLifeTime;
				++_iter;
            }
            else
            {
                //타이머는 한번 사용되고 나면 버린다;
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

    // GLAgentServer::Create()에서 mapslist를 다 읽은 후 아래 ManagerAgent의 생성자가 호출 됨;
    // _bUsed에는 mapslist에 기록되어 사용 중인 mapID에 대한 정보가 들어 있음;
    // 사용 중인 mapID 외의 ID는 전부 instance 생성에 사용 됨;
    // 기존에 sID를 instance ID로 사용한데 반해, MID를 통합하여 사용;
    // sID 는 stage ID로 전환 함으로써, 일반 field map도 1 Land n Stage 구성이 가능해짐;
    ManagerAgent::ManagerAgent(GLAgentServer* const _pAgentServer, const bool* const _bUsed /* size = MAX_LAND_MID */)
		: pScriptManager(InstanceSystem::ScriptManager::getInstance())
        , fUpdateTimer(0.0f)
		, packetCounter(InstanceSystem::EMMESSAGE_NSIZE)
	{
		gpAgentServer =_pAgentServer;

        for ( DWORD _i(MAX_LAND_MID); _i--; )
        {
            if ( _bUsed[_i] == true )
                continue; // 사용 중인 mapID면 넘어감;

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
	 * 현재 매칭 시스템이 없기에, 임시로 이곳에서 매칭 시스템 대리 역할을 함;
	*/
	HRESULT ManagerAgent::frameMove(const float _fTime, const float _fElapsedTime, const CTime& _currentTime)
	{
		if ( GLUseFeatures::GetInstance().IsUsingNewInstance() == false )
			return S_OK;

		if ( this->pScriptManager->isState(InstanceSystem::EMINSTANCE_SCRIPT_RELOAD) == true )
			return S_OK; // reloading 중이면 처리하지 않는다;

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
				//등록된 콜벡에 찾아가자!.
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
		{ // reloading 중이면 처리하지 않는다;
			GLMSG::NET_INSTANCE_ERROR _errorMessage(InstanceSystem::EMERROR_RELOAD_SCRIPT);
			ManagerAgent::_sendMessageToClient(_dbNum, &_errorMessage);
			return false;
		}

		if ( this->pScriptManager->isState(_keyMapID, InstanceSystem::EMINSTANCE_SCRIPT_USE) == false )
		{ // 인던이 생성 가능 상태가 아니면 실패;
			GLMSG::NET_INSTANCE_ERROR _errorMessage(InstanceSystem::EMERROR_CANNOT_CREATE_INSTANCE, _keyMapID);
			ManagerAgent::_sendMessageToClient(_dbNum, &_errorMessage);
			return false;
		}

		// 인던 입장 제한이 걸려있을 경우 실패한다;
		if ( this->pScriptManager->isState(_keyMapID, InstanceSystem::EMINSTANCE_SCRIPT_JOIN_LOCK) == true )
		{
			GLMSG::NET_INSTANCE_ERROR _errorMessage(InstanceSystem::EMERROR_REJECT_JOIN, _keyMapID);
			ManagerAgent::_sendMessageToClient(_dbNum, &_errorMessage);
			return false;
		}

		// 이미 Matching 에 들어가 있는 경우 실패한다;
		if ( MatchingSystem::MatchingCore::getinstance()->IsAreadyJoin( _dbNum ) )
		{
			GLMSG::NET_INSTANCE_ERROR _errorMessage(InstanceSystem::EMERROR_ALREADY_IN_INSTANCE, _keyMapID);
			ManagerAgent::_sendMessageToClient(_dbNum, &_errorMessage);
			return false;
		}

		GLCharAG* const _pRequester(gpAgentServer->GetCharByDbNum(_dbNum));
		if ( _pRequester == 0 )
			return false;

		// 이미 점령전 참가 중이면 실패한다;
		if ( _pRequester->IsCtfPlayer() )
		{
			GLMSG::NET_INSTANCE_ERROR _errorMessage(InstanceSystem::EMERROR_ALREADY_IN_INSTANCE, _keyMapID);
			ManagerAgent::_sendMessageToClient(_dbNum, &_errorMessage);
			return false;
		}

		if ( this->memberInstanceMap.find(_dbNum) != this->memberInstanceMap.end() )
		{ // 인던 안에서 다른 인던으로 이동 할 수 없다;
			GLMSG::NET_INSTANCE_ERROR _errorMessage(InstanceSystem::EMERROR_CANNOT_CREATE_INSTANCE, _keyMapID);
			ManagerAgent::_sendMessageToClient(_dbNum, &_errorMessage);
			return false;
		}

		if ( ManagerAgent::requestEntry(_dbNum, _keyMapID, _nGateID, _nToGateID) == true )
			return true; // 중간 난입 or 재입장 가능한 인던이 있는가?;

		// requestCreate/createInstance : 한 loop 안에서 호출되어야 한다;
		// 생성 되기 전에 requestJoin이 또 호출 되는 경우 또 생성 하려 듦;
		const bool _bRequestByGate(_nGateID != GAEAID_NULL);
		DWORD _createParam1(0);
		DWORD _createParam2(0);
		DWORD _createParam3(0);
		DWORD _createParam4(0);
		if ( ManagerAgent::requestCreate(_dbNum, _keyMapID, _bRequestByGate, _createParam1, _createParam2, _createParam3, _createParam4) == false )
		{ // 생성 가능한 입장인가?;
			GLMSG::NET_INSTANCE_ERROR _errorMessage(InstanceSystem::EMERROR_CANNOT_CREATE_INSTANCE, _keyMapID);
			ManagerAgent::_sendMessageToClient(_dbNum, &_errorMessage);
			return false;
		}

		const InstanceMapID _instanceMapID(ManagerAgent::createInstance(_keyMapID, _createParam1, _createParam2, _createParam3, _createParam4));
		if ( _instanceMapID == InstanceMapID(-1) )
		{ // 생성에 성공 하였는가?;
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

			// 생성 까지 다 했는데 입장불가일 경우 인던을 파괴한다;
			// 실제로 이곳에는 들어오지 않는것이 좋다;
			// 이럴거면 그냥 생성하지 못하도록 막아라;
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
            return InstanceMapID(-1); // 사용 불가 상태면 실패;
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

		// 생성이 되었다고 해서 바로 입장 할 수 있는건 아니다;
		// field server 측에도 생성 요청을 보내고 완료 메세지가 올때까지 생성 대기 상태;
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

			// 인던 생성시 EventCreate 함수 호출;
			_instanceAgent.eventCreate(_instanceMapID, _dwParam1, _dwParam2, _dwParam3, _dwParam4);
		}

        GLMSG::NET_INSTANCE_CREATE _createMessage(_keyMapID, _instanceMapID, _baseMapID, InstanceSystem::EMINSTANCE_NORMAL);
        gpAgentServer->SENDTOFIELDSVR(sFieldInfo.nChannelNum, sFieldInfo.nFieldNum, &_createMessage);

        return _instanceMapID;
	}

	// 생성 되었을때 트리거로 인던에 알림.
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

		/// 인던에 있을경우 이동할 수 없다 ( Field 에서 Instance 이동일 경우 사용된다 );
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
			return false; // 해당 인던이 존재 하지 않음;
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
			return false; // 유효한 keyMapID가 아니라면 실패;

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
	 * 아래 함수는 안전하게 인던 밖으로 이동하게 되면 호출 된다;
	 * 멤버 삭제를 목적으로 하는 경우에는,
	 * _instanceAgent.removeMember() 함수를 통해서 밖으로 내보내야 한다;
	*/
	const bool CALLBACK ManagerAgent::_removeMember(const DBNUM _dbNum)
	{
		MemberInstanceAgentMapCIter _iteratorMember(this->memberInstanceMap.find(_dbNum));
		if ( _iteratorMember != this->memberInstanceMap.end() )
		{ // 해당 인던이 삭제 되기 전까지 이전 인던에 대한 정보가 필요 하므로 삭제가 아닌, 값 갱신만 해줌;
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
					// 갯수 만큼인지 체크!
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


		// 갯수대로 만들어지면 리턴!
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
				//등록된 콜벡에 찾아가자!.
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

