#pragma once

namespace InstanceSystem
{
	__forceinline const ReloadState& ReloadState::operator =(const ReloadState& _rhs)
	{
		::memcpy(this, &_rhs, sizeof(ReloadState));
		return *this;
	}	


	__forceinline const MemberAgent& MemberAgent::operator =(const MemberAgent& _rhs)
	{
		this->dbNum = _rhs.dbNum;
		this->emAuthority = _rhs.emAuthority;
		this->nState = _rhs.nState;
		this->previousMapInformation = _rhs.previousMapInformation;
		this->queueInformationPosition = _rhs.queueInformationPosition;

		return *this;
	}
	__forceinline void MemberAgent::setPosition(const InformationPosition& _informationPosition)
	{
		if ( this->nState[InstanceSystem::EMMEMBER_SET_TO_REMOVE] == true )
			return;

		this->queueInformationPosition.push(_informationPosition);
	}	
	__forceinline void MemberAgent::moveDone(void)
	{
		MemberAgent::setState(InstanceSystem::EMMEMBER_IN_GUARD, false);
		if ( this->queueInformationPosition.size() != 0 )
			this->queueInformationPosition.pop();
	}
	__forceinline void MemberAgent::offline(void)
	{
		while ( this->queueInformationPosition.size() )
			this->queueInformationPosition.pop();
		MemberAgent::setState(InstanceSystem::EMMEMBER_SET_TO_REMOVE);	
		MemberAgent::setState(InstanceSystem::EMMEMBER_IN_GUARD, false);
	}

	__forceinline const DBNUM MemberAgent::getDbNum(void) const
	{
		return this->dbNum;
	}
	__forceinline const EMAUTHORITY MemberAgent::getAuthority(void) const
	{
		return this->emAuthority;
	}
	__forceinline const MapInformation& MemberAgent::getPreviousMapInformation(void) const
	{
		return this->previousMapInformation;
	}
	__forceinline MapInformation& MemberAgent::getPreviousMapInformation(void)
	{
		return this->previousMapInformation;
	}
	__forceinline const bool MemberAgent::isState(const InstanceSystem::EMMEMBER_STATE _emState) const
	{
		return this->nState[unsigned int(_emState)];
	}
	__forceinline const bool MemberAgent::isValidToDestroy(void) const
	{
		return this->queueInformationPosition.size() == 0;
	}
	__forceinline void MemberAgent::setState(const InstanceSystem::EMMEMBER_STATE _emState, const bool _bOn)
	{
		if ( _bOn == true )
			this->nState += unsigned int(_emState);
		else
			this->nState -= unsigned int(_emState);
	}
	__forceinline void MemberAgent::setPreviousMapInformation(const MapInformation& _previousMapInformation)
	{
		this->previousMapInformation = _previousMapInformation;
	}



    
	__forceinline const bool InstanceAgent::reserveMember(const DBNUM _dbNum, const InformationPosition& _informationPosition, const EMAUTHORITY _emAuthority)
	{
		if ( InstanceAgent::isState(InstanceSystem::EMINSTANCE_JOIN_LOCK) == true )
			return false;

		if ( InstanceAgent::isState(InstanceSystem::EMINSTANCE_RESERVE_DESTROY) == true )
			return false;

		MemberAgentMapIter _iteratorMember(this->memberMap.find(_dbNum));
		if ( _iteratorMember != this->memberMap.end() )
			return false;

		if ( InstanceAgent::requestJoin(_dbNum, _emAuthority) == false )
			return false;

		const InformationPosition _resultInformationPosition(InstanceAgent::eventJoin(_dbNum, _informationPosition, _emAuthority));
		if ( _resultInformationPosition.targetActorID == Faction::GLActorID(CROW_GATE, unsigned int(-1)) )
			return false;

		MemberAgent _memberAgent(_dbNum, _emAuthority);
		_memberAgent.setPosition(_resultInformationPosition);
		_memberAgent.setState(InstanceSystem::EMMEMBER_NOT_INITIALIZE);

		this->memberMap.insert(MemberAgentValue(_dbNum, _memberAgent));
		this->nCurrentPlayer++;

		sc::writeLogInfo( sc::string::format(
			"[ Move Map ] [ Reserve Member ( Instance ) : %1% / %2% ]",
			_dbNum,
			instanceMapID ) );

		return true;
	}

	__forceinline const bool InstanceAgent::removeMember(const DBNUM _dbNum, const InformationPosition& _informationPosition)
	{
		MemberAgentMapIter _iteratorMember(this->memberMap.find(_dbNum));
		if ( _iteratorMember == this->memberMap.end() )
			return false;

		MemberAgent& _member(_iteratorMember->second);		
		_member.setPosition(_informationPosition);

		// 반드시 원래 위치로 돌려 보낸다는 예약 후 삭제 대기 상태로 돌려야 한다;
		_member.setState(InstanceSystem::EMMEMBER_SET_TO_REMOVE);
	}
	__forceinline const bool InstanceAgent::removeMember(const DBNUM _dbNum)
	{
		MemberAgentMapIter _iteratorMember(this->memberMap.find(_dbNum));
		if ( _iteratorMember == this->memberMap.end() )
			return false;

		MemberAgent& _member(_iteratorMember->second);
		const MapInformation& _mapInformationPrevious(_member.getPreviousMapInformation());
		const Faction::GLActorID _actorID(CROW_PC, _dbNum);
		const SNATIVEID& _targetGaeaMapID(_mapInformationPrevious.targetMapID);
		const Faction::GLActorID& _targetActorID(_mapInformationPrevious.targetActorID);
		const float _fPositionX(_mapInformationPrevious.targetPosition.x);
		const float _fPositionZ(_mapInformationPrevious.targetPosition.z);
		const InformationPosition _informationPosition(_actorID, _targetGaeaMapID, _targetActorID, _fPositionX, _fPositionZ, InstanceSystem::EMMOVE_INSTANCE2FIELD);

		_member.setPosition(_informationPosition);

		// 반드시 원래 위치로 돌려 보낸다는 예약 후 삭제 대기 상태로 돌려야 한다;
		_member.setState(InstanceSystem::EMMEMBER_SET_TO_REMOVE);
        return true;
	}
	__forceinline const bool InstanceAgent::moveDone(const DBNUM _dbNum)
	{
		MemberAgentMapIter _iteratorMember(this->memberMap.find(_dbNum));
		if ( _iteratorMember == this->memberMap.end() )
			return false;

		MemberAgent& _member(_iteratorMember->second);
		_member.moveDone();	

		sc::writeLogInfo( sc::string::format(
			"[ Move Map ] [ OnCompleteMove ( Instance ) : %1% / %2% ]",
			_dbNum,
			instanceMapID ) );

        return true;
	}
	__forceinline const MapInformation* const InstanceAgent::getPreviousMapPosition(const DBNUM _dbNum)
	{
		MemberAgentMapIter _iteratorMember(this->memberMap.find(_dbNum));
		if ( _iteratorMember == this->memberMap.end() )
			return 0;

		const MemberAgent& _member(_iteratorMember->second);
		return &_member.getPreviousMapInformation();
	}
	__forceinline const bool InstanceAgent::setPreviousMapPosition(const DBNUM _dbNum, const MapInformation& _previousMapInformation)
	{
		MemberAgentMapIter _iteratorMember(this->memberMap.find(_dbNum));
		if ( _iteratorMember == this->memberMap.end() )
			return false;

		MemberAgent& _member(_iteratorMember->second);
		_member.setPreviousMapInformation(_previousMapInformation);
        return true;
	}
	__forceinline const bool InstanceAgent::setPositionMember(const DBNUM _dbNum, const InformationPosition& _informationPosition)
	{
		MemberAgentMapIter _iteratorMember(this->memberMap.find(_dbNum));
		if ( _iteratorMember == this->memberMap.end() )
			return false;

		MemberAgent& _member(_iteratorMember->second);
		_member.setPosition(_informationPosition);
        return true;
	}
	
	__forceinline void InstanceAgent::setState(const InstanceSystem::EMINSTANCE_STATE _emState, const bool _bSet)
	{		
		if ( _bSet == true )
			this->nState += unsigned int(_emState);
		else
			this->nState -= unsigned int(_emState);
	}
	__forceinline void InstanceAgent::setStateMember(const DBNUM _dbNum, const InstanceSystem::EMMEMBER_STATE _emState, const bool _bSet)
	{
		MemberAgentMapIter _iteratorMember(this->memberMap.find(_dbNum));
		if ( _iteratorMember == this->memberMap.end() )
			return;

		MemberAgent& _memberAgent = _iteratorMember->second;
		_memberAgent.setState(_emState, _bSet);
	}
	
    __forceinline const bool InstanceAgent::addChildInstance(const SNATIVEID& _baseMapID)
    {
        MapIDVectorCIter _iteratorInstanceMapID(std::find(this->childInstanceIDVector.begin(), this->childInstanceIDVector.end(), _baseMapID));
        if ( _iteratorInstanceMapID != this->childInstanceIDVector.end() )
            return false; // 이미 생성 되어 있으면 그냥 빠져 나간다;

        // this->nCreateChildInstanceCount 의 경우 생성 완료 메세지가 오면 1씩 감소 한다;
        this->childInstanceIDVector.push_back(_baseMapID);
        ++this->countCreatingChildLand;
        return true;
    }
	
	__forceinline const bool InstanceAgent::isMember(const DBNUM _dbNum) const
	{
		return this->memberMap.find(_dbNum) != this->memberMap.end();
	}
	__forceinline const bool InstanceAgent::isState(const InstanceSystem::EMINSTANCE_STATE _emState) const
	{
		return this->nState[unsigned int(_emState)];
	}	
	__forceinline const bool InstanceAgent::isState(const DBNUM _dbNum, const InstanceSystem::EMMEMBER_STATE _emState) const
	{
		MemberAgentMapCIter _iteratorMember(this->memberMap.find(_dbNum));
		if ( _iteratorMember == this->memberMap.end() )
			return false;

		const MemberAgent& _memberAgent = _iteratorMember->second;
		return _memberAgent.isState(_emState);
	}
	__forceinline const bool InstanceAgent::isValidToDestroy(const float _fTimeElapsed)
	{
		if ( InstanceAgent::isState(EMINSTANCE_RESERVE_DESTROY) == false )
			return false;

		if ( InstanceAgent::isState(EMINSTANCE_CREATE_COMPLETE) == false )
			return false;

		for ( MemberAgentMapCIter _iteratorMember(this->memberMap.begin()); _iteratorMember != this->memberMap.end(); ++_iteratorMember )
		{
			const MemberAgent& _member(_iteratorMember->second);
			if ( _member.isValidToDestroy() == false )
				return false;
		}

		if ( (this->fDestroyTimer += _fTimeElapsed) < DEFINSTANCE_TIME_TO_DESTROY )
			return false;

		return true;
	}
	__forceinline const bool InstanceAgent::isValidToMove(void) const
	{
		return this->countCreatingChildLand == 0;
	}
	
    __forceinline const SNATIVEID InstanceAgent::getLandMapID(const SNATIVEID& _baseMapID) const
    {
        const unsigned int _sizeChildInstance(this->childInstanceIDVector.size());
        for ( unsigned int _i(_sizeChildInstance); _i--; )
        {			
            const SNATIVEID& _childBaseMapID(this->childInstanceIDVector[_i]);
            if ( _childBaseMapID == _baseMapID )
                return SNATIVEID(this->instanceMapID, _i);
        }

        return SNATIVEID(false);
    }
    __forceinline const SNATIVEID InstanceAgent::getChildMapID(const unsigned int _indexStage) const
    {
        if ( _indexStage >= this->childInstanceIDVector.size() )
            return SNATIVEID(false);

        const SNATIVEID& _childMapID(this->childInstanceIDVector[_indexStage]);
        return _childMapID;
    }    
	__forceinline const InstanceSystem::InstanceMapID InstanceAgent::getInstanceMapID(void) const
	{
		return this->instanceMapID;
	}
    __forceinline const unsigned int InstanceAgent::getNChildInstance(void) const
    {
        const unsigned int _sizeChildInstance(this->childInstanceIDVector.size());
        return _sizeChildInstance;
    }
	__forceinline const SNATIVEID& InstanceAgent::getKeyMapID(void) const
	{
		return this->keyMapID;
	}
	__forceinline const InstanceSystem::EMINSTANCE_TYPE InstanceAgent::getInstanceType(void) const
	{
		return this->emInstanceType;
	}
	__forceinline const InstanceSystem::EMAUTHORITY InstanceAgent::getAuthority(const InstanceSystem::DBNUM _dbNum) const
	{
		MemberAgentMapCIter _iteratorMember(this->memberMap.find(_dbNum));
		if ( _iteratorMember == this->memberMap.end() )
			return InstanceSystem::EMAUTHORITY_NORMAL;

		const InstanceSystem::MemberAgent& _member(_iteratorMember->second);
		return _member.getAuthority();
	}

	__forceinline const unsigned int InstanceAgent::getFieldServer(void) const
	{
		return this->nFieldServer;
	}
	__forceinline const unsigned int InstanceAgent::getFieldChannel(void) const
	{
		return this->nFieldChannel;
	}
	

	__forceinline const MemberAgentMap& InstanceAgent::getMemberMap(void) const
	{
		return this->memberMap;
	}	

    __forceinline const InstanceMapID ManagerAgent::getInstanceMapID(void) const
    {
        if ( this->instanceMapIDQueue.size() == 0 )
            return GAEAID_NULL;

        const InstanceMapID _instanceMapID(this->instanceMapIDQueue.front());
        return _instanceMapID;
    }
    __forceinline void ManagerAgent::popInstanceMapID(void)
    {
        this->instanceMapIDQueue.pop();
    }
    __forceinline void ManagerAgent::pushInstanceMapID(const InstanceMapID& _instanceMapID)
    {
        this->instanceMapIDQueue.push(_instanceMapID);
    }
}
