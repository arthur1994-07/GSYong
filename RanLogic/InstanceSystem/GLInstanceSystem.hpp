#pragma once

namespace InstanceSystem
{
//---------------------------------- Instance Script ----------------------------------
    __forceinline void InstanceScript::setState(const unsigned int _nState, const bool _bOn)
    {
        if ( _bOn == true )
            this->nState += _nState;
        else
            this->nState -= _nState;
    }
    __forceinline const bool InstanceScript::isState(const unsigned int _nState) const
    {
        return this->nState[_nState];
    }
	__forceinline const unsigned int InstanceScript::getNLevelAgentScript(void) const
	{
		return this->levelAgentScriptHashVector.size();
	}
	__forceinline const unsigned int InstanceScript::getNLevelFieldScript(void) const
	{
		return this->levelFieldScriptHashVector.size();
	}	
	__forceinline const unsigned int InstanceScript::getLevelAgentKey(const unsigned int _nIndex) const
	{
		if ( _nIndex >= this->levelAgentScriptHashVector.size() )
			return -1;

		return this->levelAgentScriptHashVector[_nIndex];
	}
	__forceinline const unsigned int InstanceScript::getLevelFieldKey(const unsigned int _nIndex) const
	{
		if ( _nIndex >= this->levelFieldScriptHashVector.size() )
			return -1;

		return this->levelFieldScriptHashVector[_nIndex];
	}
	__forceinline const unsigned int InstanceScript::getMaxPlayer(void) const
	{
		return this->nMaxPlayer;
	}
	__forceinline const unsigned int InstanceScript::getMaxCreate(void) const
	{
		return this->nMaxCreate;
	}	
    __forceinline const SNATIVEID& InstanceScript::getKeyMapID(void) const
    {
        return this->keyMapID;
    }
	__forceinline sc::LuaInstance& InstanceScript::getScript(void)
	{
		return this->scriptInstance;
	}
    __forceinline const std::string& InstanceScript::getIsntanceName(void) const
    {
        return this->instanceName;
    }
//------------------------------------------------------------------------------------

//---------------------------------- Instance Information Client ----------------------------------
	__forceinline const InstanceInformationClient& InstanceInformationClient::operator =(const InstanceScript& _instanceScript)
	{
		this->keyMapID = _instanceScript.keyMapID;
		this->bOpen = !(_instanceScript.isState(EMINSTANCE_SCRIPT_JOIN_LOCK));
		::StringCchCopy(this->instanceName, InstanceSystem::EMINSTANCE_NAME_LENGTH, _instanceScript.instanceName.c_str());
		return *this;
	}
	__forceinline const InstanceInformationClient& InstanceInformationClient::operator =(const InstanceInformationClient& _rhs)
	{
		this->keyMapID = _rhs.keyMapID;
		this->bOpen = _rhs.bOpen;
		::StringCchCopy(this->instanceName, InstanceSystem::EMINSTANCE_NAME_LENGTH, _rhs.instanceName);
		return *this;
	}
//------------------------------------------------------------------------------------


//---------------------------------- Script Manager ----------------------------------    
	__forceinline void ScriptManager::setState(const unsigned int _nState, const bool _bOn)
	{
		_bOn == true ? (this->nState += _nState) : (this->nState -= _nState);
	}
	__forceinline const bool ScriptManager::setState(const SNATIVEID& _keyMapID, const unsigned int _nState, const bool _bOn)
	{
		InstanceScriptMapIter _iteratorScript(this->instanceScriptMap.find(_keyMapID));
		if ( _iteratorScript == this->instanceScriptMap.end() )
			return false;

		InstanceScript& _instanceScript = _iteratorScript->second;
		_instanceScript.setState(_nState, _bOn);
		return true;
	}
	__forceinline const bool ScriptManager::isState(const unsigned int _nState) const
	{
		return this->nState[_nState];
	}
    __forceinline const bool ScriptManager::isState(const SNATIVEID& _keyMapID, const unsigned int _nState) const
    {
		InstanceScriptMapCIter _iteratorScript(this->instanceScriptMap.find(_keyMapID));
		if ( _iteratorScript == this->instanceScriptMap.end() )
			return false;

		const InstanceScript& _instanceScript = _iteratorScript->second;
        return _instanceScript.isState(_nState);
    }
	__forceinline const bool ScriptManager::isExist(const SNATIVEID& _keyMapID) const
	{
		InstanceScriptMapCIter _iteratorScript(this->instanceScriptMap.find(_keyMapID));
		if ( _iteratorScript == this->instanceScriptMap.end() )
			return false;

		return true;
	}
    
	__forceinline const unsigned int ScriptManager::getNInstance(void) const
	{
		return this->instanceScriptMap.size();
	}

	__forceinline sc::LuaInstance* const ScriptManager::getScript(const SNATIVEID& _keyMapID)
	{
		InstanceScriptMapIter _iteratorScript(this->instanceScriptMap.find(_keyMapID));
		if ( _iteratorScript == this->instanceScriptMap.end() )
			return 0;

		InstanceScript& _instanceScript = _iteratorScript->second;
		return &_instanceScript.getScript();
	}

	__forceinline const unsigned int ScriptManager::getMaxPlayer(const SNATIVEID& _keyMapID) const
	{
		InstanceScriptMapCIter _iteratorScript(this->instanceScriptMap.find(_keyMapID));
		if ( _iteratorScript == this->instanceScriptMap.end() )
			return 0;

		const InstanceScript& _instanceScript = _iteratorScript->second;
		return _instanceScript.getMaxPlayer();
	}
	__forceinline const unsigned int ScriptManager::getMaxCreate(const SNATIVEID& _keyMapID) const
	{
		InstanceScriptMapCIter _iteratorScript(this->instanceScriptMap.find(_keyMapID));
		if ( _iteratorScript == this->instanceScriptMap.end() )
			return 0;

		const InstanceScript& _instanceScript = _iteratorScript->second;
		return _instanceScript.getMaxCreate();
	}

	__forceinline const int ScriptManager::getInstanceInformation(const SNATIVEID& _keyMapID, InstanceInformationClient& _instanceInformation) const
	{
		InstanceScriptMapCIter _iteratorScript(this->instanceScriptMap.find(_keyMapID));
		if ( _iteratorScript == this->instanceScriptMap.end() )
			return -1;

		const InstanceScript& _instanceScript = _iteratorScript->second;
		if ( (_instanceScript.isState(InstanceSystem::EMINSTANCE_SCRIPT_VISIBLE) == false) ||
			(_instanceScript.isState(InstanceSystem::EMINSTANCE_SCRIPT_USE) == false) )
			return -1;

		_instanceInformation = _instanceScript;
		return 0;
	}

    __forceinline const std::string* ScriptManager::getInstanceName(const SNATIVEID& _keyMapID) const
    {
        InstanceScriptMapCIter _iteratorScript(this->instanceScriptMap.find(_keyMapID));
        if ( _iteratorScript == this->instanceScriptMap.end() )
            return NULL;

        const InstanceScript& _instanceScript = _iteratorScript->second;
        return &_instanceScript.getIsntanceName();
    }

	__forceinline const int ScriptManager::getLevelAgentScript(const InstanceMapID& _instanceMapID, const SNATIVEID& _keyMapID, sc::LuaInstance& _scriptReference) const
	{
		InstanceScriptMapCIter iterInstanceScript = instanceScriptMap.find( _keyMapID );
		if ( iterInstanceScript == instanceScriptMap.end() )
			return -1;

		const InstanceScript& sInstanceScript = iterInstanceScript->second;

		const unsigned int nSize = sInstanceScript.getNLevelAgentScript();
		for ( unsigned int i=0; i<nSize; ++i )
		{
			const unsigned int nHashKey = sInstanceScript.getLevelAgentKey( i );
			LevelScriptMapCIter iterLevelScript = levelScriptMap.find( nHashKey );
			if ( iterLevelScript == levelScriptMap.end() )
			{
				return -1;
			}

			const LevelScript& sLevelScript = iterLevelScript->second;
			if ( _scriptReference.DoBuffer(
				sLevelScript.pBuffer,
				sLevelScript.nSize,
				sLevelScript.fileName.c_str() ) == -1 )
			{
				return -1;
			}
		}

		// 스크립트 안에 고유키를 심어준다;
		const std::string strInstanceMapID( sc::string::format( "instanceMapID = %d", _instanceMapID ) );
		_scriptReference.DoString( strInstanceMapID.c_str() );

		const std::string strKeyMapID( sc::string::format( "keyMapID = %d;", _keyMapID.Id() ) );
		_scriptReference.DoString( strKeyMapID.c_str() );

		return 0;
	}

	__forceinline const int ScriptManager::getLevelFieldScript(const InstanceMapID& _instanceMapID, const SNATIVEID& _keyMapID, sc::LuaInstance& _scriptReference) const
	{
		InstanceScriptMapCIter iterInstanceScript = instanceScriptMap.find( _keyMapID );
		if ( iterInstanceScript == instanceScriptMap.end() )
			return -1;

		const InstanceScript& sInstanceScript = iterInstanceScript->second;

		const unsigned int nSize = sInstanceScript.getNLevelFieldScript();
		for ( unsigned int i=0; i<nSize; ++i )
		{
			const unsigned int nHashKey = sInstanceScript.getLevelFieldKey( i );
			LevelScriptMapCIter iterLevelScript = levelScriptMap.find( nHashKey );
			if ( iterLevelScript == this->levelScriptMap.end() )
			{
				return -1;
			}

			const LevelScript& sLevelScript = iterLevelScript->second;
			if ( _scriptReference.DoBuffer(
				sLevelScript.pBuffer,
				sLevelScript.nSize,
				sLevelScript.fileName.c_str() ) == -1 )
			{
				return -1;
			}
		}
	
		// 스크립트 안에 고유키를 심어준다;
        const std::string strInstanceMapID( sc::string::format( "instanceMapID = %d;", _instanceMapID ) );
        _scriptReference.DoString( strInstanceMapID.c_str() );

		const std::string strKeyMapID( sc::string::format( "keyMapID = %d;", _keyMapID.Id() ) );
		_scriptReference.DoString( strKeyMapID.c_str() );

		return 0;
	}	

	__forceinline const MapIDVector& ScriptManager::getKeyMapIDVector(void) const
	{
		return this->keyMapIDVector;
	}

	__forceinline void ScriptManager::registFunction(const char* const _name, lua_CFunction _function)
	{
		for ( InstanceScriptMapCIter _iteratorScript(this->instanceScriptMap.begin()); _iteratorScript != this->instanceScriptMap.end(); ++_iteratorScript )		
		{
			const SNATIVEID& _keyMapID(_iteratorScript->first);
			sc::LuaInstance* const _pScript(ScriptManager::getScript(_keyMapID));
			GASSERT(_pScript);
			_pScript->RegistFunction(_name, _function);
		}

	}

    __forceinline void ScriptManager::pushCallFunction(const char* const _name)
    {
        for ( InstanceScriptMapCIter _iteratorScript(this->instanceScriptMap.begin()); _iteratorScript != this->instanceScriptMap.end(); ++_iteratorScript )		
        {
            const SNATIVEID& _keyMapID(_iteratorScript->first);
            sc::LuaInstance* const _pScript(ScriptManager::getScript(_keyMapID));
            GASSERT(_pScript);
            if ( _pScript->PushFunctionEx(_name) == true )
            {
                _pScript->CallFunction(0,0);
            }
        }
    }

	__forceinline void ScriptManager::registFunctionCommon ( FUNC_SCRIPT_BASE _function )
	{
		for ( InstanceScriptMapCIter iter( instanceScriptMap.begin() );
			iter != instanceScriptMap.end(); ++iter )		
		{
			const SNATIVEID& skeyMapID( iter->first );
			sc::LuaInstance* const pScript( getScript( skeyMapID ) );
			if ( NULL == pScript )
				return;

			_function( *pScript );
		}
	}

//------------------------------------------------------------------------------------

//---------------------------------- Information SetPosition ----------------------------------	
//------------------------------------------------------------------------------------



//---------------------------------- Instance Log ----------------------------------
    __forceinline const InstanceLog& InstanceLog::operator =(const InstanceLog& _rhs)
    {
        this->keyMapID = _rhs.keyMapID;
        this->nUsedCount = _rhs.nUsedCount;
        this->nRunningCount = _rhs.nRunningCount;
		this->nMaxCreate = _rhs.nMaxCreate;

        return *this;
    }

    __forceinline const InstanceList& InstanceList::operator =(const InstanceList& _rhs)
    {
        this->keyMapID = _rhs.keyMapID;
        this->instanceMapID = _rhs.instanceMapID;

        return *this;
    }
//------------------------------------------------------------------------------------

//---------------------------------- Instance Timer ----------------------------------
    __forceinline const InstanceTimer& InstanceTimer::operator =(const InstanceTimer& _rhs)
    {
		hTimerHandle;

        tStartTime;  // 생성된 시간;
        fElapsedTime;  // 경과 시간;
        bPause;

        m_dwParam0;
        m_dwParam1;
		m_dwParam2;
		m_dwParam3;

        ::memcpy(this, &_rhs, sizeof(InstanceTimer));
        return *this;
    }

    __forceinline const bool InstanceTimer::updateTimer(const float _fElapsedTime)
    {
        if ( this->bPause == true )
            return false;

        this->fElapsedTime -= _fElapsedTime;
        if ( 0.0f < this->fElapsedTime )
            return false;

        return true;  // 타이머 이벤트 발생;
    }

    __forceinline const bool InstanceTimer::isPause(void) const
    {
        return this->bPause;
    }

    __forceinline void InstanceTimer::suspend(void)
    {
        this->bPause = true;
    }

    __forceinline void InstanceTimer::resume(void)
    {
        this->bPause = false;
    }
//------------------------------------------------------------------------------------


//---------------------------------- Instance Absolute Timer -------------------------
    __forceinline const InstanceAbsTimer& InstanceAbsTimer::operator =(const InstanceAbsTimer& _rhs)
    {
        hTimerHandle;
        wWeek;
        wDay;
        wHour;
        wMinute;
        wSecond;
        m_dwParam0;
        m_dwParam1;
        m_dwParam2;
        m_dwParam3;

        ::memcpy(this, &_rhs, sizeof(InstanceAbsTimer));
        return *this;
    }

    __forceinline const bool InstanceAbsTimer::checkTimer(const CTime& _cCurrentTime)
    {
        if ( this->wWeek != 0 )
        {
            //  1: sunday  7: saturday;
            const int _today = _cCurrentTime.GetDayOfWeek();
            if ( this->wWeek != _today )
            {
                bUsed = false;
                return false;
            }
        }
        if ( this->wDay != 0 )
        {
            const int _today = _cCurrentTime.GetDay();
            if ( this->wDay != _today )
            {
                bUsed = false;
                return false;
            }
        }

        if ( ( this->wHour != _cCurrentTime.GetHour() ) || ( this->wMinute != _cCurrentTime.GetMinute() ) || ( this->wSecond != _cCurrentTime.GetSecond() ) )
        {
            bUsed = false;
            return false;
        }

        if ( bUsed == false )
            return true;

        // (bUsed == true) => loop타이머때문에 이전 프레임에 타이머가 재호출되는 경우에 발생;
        return false;

    }
//------------------------------------------------------------------------------------


	__forceinline const InstanceSystem::PacketCounterElement& PacketCounterElement::operator =(const InstanceSystem::PacketCounterElement& _rhs)
	{
		::memcpy(this, &_rhs, sizeof(InstanceSystem::PacketCounterElement));
		return *this;
	}	
	__forceinline const unsigned int PacketCounterElement::increase(void)
	{
		return ++this->countPacket;
	}
	__forceinline void PacketCounterElement::clear(void)
	{
		this->countPacketPrevious = max(this->countPacket, this->countPacketPrevious);
		this->countPacket = 0;
	}


	__forceinline const InstanceSystem::PacketCounter& PacketCounter::operator =(const InstanceSystem::PacketCounter& _rhs)
	{
		::memcpy(this, &_rhs, sizeof(this->timerReset));
		this->sizeMessageNotify = _rhs.sizeMessageNotify;
		this->countNotify = _rhs.countNotify;
		this->timeCumulative = _rhs.timeCumulative;
		this->timeGap = _rhs.timeGap;
		this->vectorPacketCounterElement = _rhs.vectorPacketCounterElement;
		return *this;
	}
	__forceinline const unsigned int PacketCounter::operator [](const unsigned int _indexPacket) const
	{
		return this->vectorPacketCounterElement[_indexPacket].countPacketPrevious;
	}

	__forceinline const unsigned int PacketCounter::size(void) const
	{
		return this->vectorPacketCounterElement.size();
	}
};
