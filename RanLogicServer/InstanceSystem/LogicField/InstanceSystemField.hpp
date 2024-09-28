#pragma once

namespace InstanceSystem
{
	__forceinline const bool MemberField::isState(const InstanceSystem::EMMEMBER_STATE _emState) const
	{
		return this->nState[unsigned int(_emState)];
	}
	__forceinline void MemberField::setState(const InstanceSystem::EMMEMBER_STATE _emState, const bool _bOn)
	{
		if ( _bOn == true )
			this->nState += unsigned int(_emState);
		else
			this->nState -= unsigned int(_emState);
	}

	__forceinline const InstanceMapID InstanceField::getInstanceMapID(void) const
	{
		return this->instanceMapID;
	}
	__forceinline const SNATIVEID& InstanceField::getKeyMapID(void) const
	{
		return this->keyMapID;
	}
    __forceinline const SNATIVEID InstanceField::getGaeaMapID(const SNATIVEID& _baseMapID) const
    {
        for ( unsigned int _index(MAX_LAND_SID); _index--; )
        {			
            const SNATIVEID& _childBaseMapID(this->childInstanceIDVector[_index]);
            if ( _childBaseMapID == _baseMapID )
                return SNATIVEID(this->instanceMapID, _index);
        }

        return SNATIVEID(false);
    }
    __forceinline const SNATIVEID InstanceField::getBaseMapID(const SNATIVEID& _gaeaMapID) const
    {
        const unsigned int _indexChild(_gaeaMapID.wSubID);
        if ( _indexChild >= this->childInstanceIDVector.size() )
            return SNATIVEID(false);

        const SNATIVEID& _baseMapID(this->childInstanceIDVector[_indexChild]);
        return _baseMapID;
    }

	__forceinline const bool InstanceField::isState(const DBNUM _dbNum, const InstanceSystem::EMMEMBER_STATE _emState) const
	{
		MemberFieldMapCIter _iteratorMember(this->memberMap.find(_dbNum));
		if ( _iteratorMember == this->memberMap.end() )
			return false;

		const MemberField& _memberField = _iteratorMember->second;
		return _memberField.isState(_emState);
	}
    __forceinline const bool InstanceField::isState(const InstanceSystem::EMINSTANCE_STATE _emState) const
    {
        return this->nState[unsigned int(_emState)];
    }

	__forceinline void InstanceField::setState(const DBNUM _dbNum, const InstanceSystem::EMMEMBER_STATE _emState, const bool _bSet)
	{
		MemberFieldMapIter _iteratorMember(this->memberMap.find(_dbNum));
		if ( _iteratorMember == this->memberMap.end() )
			return;

		MemberField& _memberField(_iteratorMember->second);
		_memberField.setState(_emState, _bSet);
	}		
    __forceinline void InstanceField::setState(const InstanceSystem::EMINSTANCE_STATE _emState, const bool _bSet)
    {
        if ( _bSet == true )
            this->nState += unsigned int(_emState);
        else
            this->nState -= unsigned int(_emState);
    }

    __forceinline void InstanceField::addChildInstance(const unsigned int _index, const SNATIVEID& _baseMapID)
    {
		if ( _index >= MAX_LAND_SID )
			return;

        MapIDVectorCIter _iteratorBaseMapID(std::find(this->childInstanceIDVector.begin(), this->childInstanceIDVector.end(), _baseMapID));
        if ( _iteratorBaseMapID != this->childInstanceIDVector.end() )
            return;		

		this->childInstanceIDVector[_index] = _baseMapID;
    }
    __forceinline void InstanceField::addMember(const DBNUM _dbNum)
    {
        MemberFieldMapIter _iteratorMember(this->memberMap.find(_dbNum));
        if ( _iteratorMember != this->memberMap.end() )
			return;

		MemberField _member(_dbNum);
		this->memberMap.insert(MemberFieldValue(_dbNum, _member));
    }
    __forceinline void InstanceField::removeMember(const DBNUM _dbNum)
    {
		this->memberMap.erase(_dbNum);        
    }

	__forceinline const bool InstanceField::reserveRegistActorTrigger(const Faction::GLActorID& _actorID, const DWORD _triggerType, const float _fRadius)
	{
		InstanceActorTriggerReserveInfo _reserveInfo;
		_reserveInfo.emReserveAction = InstanceActorTriggerReserveInfo::EM_RESERVE_REGIST_NOMAL;

		_reserveInfo._actorID = _actorID;
		_reserveInfo._triggerType = _triggerType;
		_reserveInfo._fRadius = _fRadius;

		ActorTriggerReserveQueue.push(_reserveInfo);
		return true;
	}

	__forceinline const bool InstanceField::reserveRegistActorTrigger(const Faction::GLActorID& _actorID, const DWORD _triggerType, const float _fZoneX, const float _fZoneZ, const float _fZoneY)
	{
		InstanceActorTriggerReserveInfo _reserveInfo;
		_reserveInfo.emReserveAction = InstanceActorTriggerReserveInfo::EM_RESERVE_REGIST_ZONE;

		_reserveInfo._actorID = _actorID;
		_reserveInfo._triggerType = _triggerType;
		_reserveInfo._fZoneX = _fZoneX;
		_reserveInfo._fZoneZ = _fZoneZ;
		_reserveInfo._fZoneY = _fZoneY;

		ActorTriggerReserveQueue.push(_reserveInfo);
		return true;
	}

	__forceinline const bool InstanceField::reserveRemoveActorTrigger(const Faction::GLActorID& _actorID)
	{
		InstanceActorTriggerReserveInfo _reserveInfo;
		_reserveInfo.emReserveAction = InstanceActorTriggerReserveInfo::EM_RESERVE_UNREGIST;

		_reserveInfo._actorID = _actorID;

		ActorTriggerReserveQueue.push(_reserveInfo);
		return true;
	}

    __forceinline const TimerHandle InstanceField::registTimer(const float _nTimeSec, const DWORD _dwParam0, const DWORD _dwParam1, const DWORD _dwParam2, const DWORD _dwParam3, const bool _bLoop)
	{
		// Handle 값을 구한다;
		int nCheckCounter = 0;
		while ( isRegistTimer( maxTimer++ ) )
		{
			nCheckCounter++;

			/// 등록될 때마다 더해지기 때문에 오버플로우 나지 않으면 중복되지 않는다;
			/// Timer가 오버플로우 될 확률이 거의 없기 때문에 들어오지 않는게 좋다;
			/// Timer가 오버플로우 될 경우 성능상 문제가 없는지 체크해보자;
			sc::writeLogWarn( "[ Instance Log ] [ Duplicated Handle of Timer! ( InstanceField::registTimer ) ]" );

			/// 혹시나 무한루프가 생길 경우를 대비해 CheckCounter를 설치한다;
			/// 이 경우가 발생하는 경우에는 스크립트를 다시 작성하기를 권한다;
			if ( nCheckCounter >= 100 )
			{
				sc::writeLogError( "[ Instance Log ] [ Overflow CheckCount!!! ( InstanceField::registTimer ) ]" );

				GASSERT( 0 && "Overflow CheckCount!!! ( InstanceField::registTimer )" );

				// GASSERT 를 넘어갈 경우 임시로 돌아가도록 존재하는 Timer를 제거한다;
				// 무조건 버그가 발생한다;
				this->instanceTimerMap.erase(maxTimer-1);

				break;
			}
		}

		TimerHandle _hTimer = maxTimer-1;

		InstanceSystem::InstanceTimer _timer(_hTimer, _nTimeSec, _dwParam0, _dwParam1, _dwParam2, _dwParam3, _bLoop);		
		InstanceSystem::InstanceTimerValue _pair(_hTimer,_timer);
		this->instanceTimerMap.insert(_pair);

		// 현재 등록된 Tiemr List를 보여준다;
#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
		//sc::writeLogInfo( "[ Instance Log ] [ ----- Current Timer List ----- ]" );
#endif
		for ( InstanceTimerMapIter _iteratorTimer(this->instanceTimerMap.begin());
			_iteratorTimer != this->instanceTimerMap.end(); ++_iteratorTimer )
		{
			TimerHandle _hTemp = _iteratorTimer->first;
			InstanceSystem::InstanceTimer& _instanceTimer = _iteratorTimer->second;
			
#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			//sc::writeLogInfo( "[ Instance Log ] [ Timer Info ]" );
			//sc::writeLogInfo( sc::string::format( "[ Instance Log ] -> [ TimerHandle : %1% ]", _hTemp ) );
			//sc::writeLogInfo( sc::string::format( "[ Instance Log ] -> [ LifeTime : %1% ]", _instanceTimer.fLifeTime ) );
			//sc::writeLogInfo( sc::string::format( "[ Instance Log ] -> [ Parameter1 : %1% ]", _instanceTimer.fParam0 ) );
			//sc::writeLogInfo( sc::string::format( "[ Instance Log ] -> [ Parameter2 : %1% ]", _instanceTimer.fParam1 ) );
			//sc::writeLogInfo( sc::string::format( "[ Instance Log ] -> [ Parameter3 : %1% ]", _instanceTimer.fParam2 ) );
			//sc::writeLogInfo( sc::string::format( "[ Instance Log ] -> [ Parameter4 : %1% ]", _instanceTimer.fParam3 ) );
#endif
		}

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
		//sc::writeLogInfo( "[ Instance Log ] [ ------------------------------ ]" );
#endif

		return _hTimer;
	}
	__forceinline const int InstanceField::isRegistTimer(const TimerHandle _hTimer) const
	{
		InstanceTimerMapCIter _iteratorTimer(this->instanceTimerMap.find(_hTimer));
		return ( _iteratorTimer != this->instanceTimerMap.end() );
	}
	__forceinline const int InstanceField::getLeftTime(const TimerHandle _hTimer) const
	{
		InstanceTimerMapCIter _iteratorTimer = this->instanceTimerMap.find(_hTimer);
		if ( _iteratorTimer == this->instanceTimerMap.end() )
			return -1;
		
		const InstanceTimer& _timer = _iteratorTimer->second;
		return int(_timer.fElapsedTime);
	}
	__forceinline const int InstanceField::suspendTimer(const TimerHandle _hTimer)
	{
		InstanceTimerMapIter _iteratorTimer(this->instanceTimerMap.find(_hTimer));
		if ( _iteratorTimer == this->instanceTimerMap.end() )
			return -1;

		InstanceTimer& _timer = _iteratorTimer->second;
		_timer.suspend();
        return 0;
	}
	__forceinline const int InstanceField::resumeTimer(const TimerHandle _hTimer)
	{
		InstanceTimerMapIter _iteratorTimer(this->instanceTimerMap.find(_hTimer));
		if ( _iteratorTimer == this->instanceTimerMap.end() )
			return -1;

		InstanceTimer& _timer = _iteratorTimer->second;
		_timer.resume();
        return 0;
	}
	__forceinline const int InstanceField::removeTimer(const TimerHandle _hTimer)
	{
		InstanceTimerMapIter _iteratorTimer(this->instanceTimerMap.find(_hTimer));
		if ( _iteratorTimer == this->instanceTimerMap.end() )
			return -1;

		//this->timerHandleQueue.push(_hTimer);
		this->instanceTimerMap.erase(_iteratorTimer);

		// 삭제된 Timer의 정보를 보여준다;
#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
		sc::writeLogInfo( "[ Instance Log ] [ Invoke - RemoveTimer ]" );
		sc::writeLogInfo( sc::string::format( "[ Instance Log ] -> [ TimerHandle : %1% ]", _hTimer ) );
#endif

        return 0;
	}
	__forceinline const bool InstanceField::removeAllTimer(void)
	{
		this->instanceTimerMap.clear();

		return true;
	}



/////////////////////////////////////////////////////////////          private Function       ///////////////////////////////////////////////////////////////////////////////////////////////

	__forceinline const bool InstanceField::registActorTrigger(const Faction::GLActorID& _actorID, const DWORD _triggerType, const float _fRadius)
	{
		InstanceActorTrigger _actorTrigger(instanceMapID, _actorID, _triggerType, _fRadius);

		for ( InstanceActorTriggerVectorIter iter = instanceActorTriggerVector.begin() ; iter != instanceActorTriggerVector.end(); )
		{
			InstanceActorTrigger& _actorTrigger = *iter;
			if ( _actorTrigger.actorID == _actorID )
			{
				iter = instanceActorTriggerVector.erase(iter);
			}
			else
			{
				iter++;
			}
		}

		instanceActorTriggerVector.push_back(_actorTrigger);
		return true;
	}

	__forceinline const bool InstanceField::registActorTrigger(const Faction::GLActorID& _actorID, const DWORD _triggerType, const float _fZoneX, const float _fZoneZ, const float _fZoneY)
	{
		InstanceActorTrigger _actorTrigger(instanceMapID, _actorID, _triggerType, _fZoneX, _fZoneZ, _fZoneY);

		for ( InstanceActorTriggerVectorIter iter = instanceActorTriggerVector.begin() ; iter != instanceActorTriggerVector.end(); )
		{
			InstanceActorTrigger& _actorTrigger = *iter;
			if ( _actorTrigger.actorID == _actorID )
			{
				iter = instanceActorTriggerVector.erase(iter);
			}
			else
			{
				iter++;
			}
		}

		instanceActorTriggerVector.push_back(_actorTrigger);
		return true;
	}

	__forceinline const bool InstanceField::removeActorTrigger(const Faction::GLActorID& _actorID)
	{
		for ( InstanceActorTriggerVectorIter iter = instanceActorTriggerVector.begin() ; iter != instanceActorTriggerVector.end(); )
		{
			InstanceActorTrigger& _actorTrigger = *iter;
			if ( _actorTrigger.actorID == _actorID )
			{
				iter = instanceActorTriggerVector.erase(iter);
				return true;
			}
			else
			{
				iter++;
			}
		}
		return false;
	}
}

