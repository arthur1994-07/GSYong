#include "pch.h"

#include "../../EngineLib/G-Logic/GLOGIC.h"
#include "../../SigmaCore/String/StringUtil.h"

#include "../Character/GLCharDefine.h"

#include "GLInstanceSystem.h"

#define KEYWORD_INSTANCE_TABLE "instanceTable"
#define KEYWORD_INCLUDE "Include"
#define KEYWORD_PUBLIC_AGENT_SCRIPT "publicAgentScript"
#define KEYWORD_PUBLIC_FIELD_SCRIPT "publicFieldScript"
#define KEYWORD_LEVEL_AGENT_SCRIPT "levelAgentScript"
#define KEYWORD_LEVEL_FIELD_SCRIPT "levelFieldScript"
#define KEYWORD_NAME "name"
#define KEYWORD_KEY_MAP_MAIN_ID "keyMapMainID"
#define KEYWORD_KEY_MAP_SUB_ID "keyMapSubID"
#define KEYWORD_MAX_PLAYER "maxPlayer"
#define KEYWORD_MAX_CREATE "maxCreate"
#define KEYWORD_IS_VISIBLE "isVisible"
#define KEYWORD_IS_USE "isUse"

namespace InstanceSystem
{
	HRESULT LoadFile(const std::string& _fileName, const bool _bPastLoad)
	{
		if ( _bPastLoad == false )
			return S_OK;

		std::string _fullPath(GLOGIC::GetServerPath());
		_fullPath.append("scripts\\instance\\");
		_fullPath.append(_fileName);

		ScriptManager* const _pScriptManager(ScriptManager::getInstance());
		_pScriptManager->doFile(_fullPath);

		return S_OK;
	}

    __forceinline void errorMessage(const std::string& _errorMessage)
    {
        ::AfxMessageBox(_errorMessage.c_str(), MB_OK|MB_ICONSTOP);
    }




	LevelScript::LevelScript(const std::string& _fileName)
		: fileName(_fileName)
		, nSize(0)
		, pBuffer(0)		
	{		
	}
	LevelScript::LevelScript(const LevelScript& _rhs)
		: fileName(_rhs.fileName)
		, nSize(_rhs.nSize)
		, pBuffer(new char[_rhs.nSize])
	{
		::memcpy(this->pBuffer, _rhs.pBuffer, _rhs.nSize);
	}

	LevelScript::~LevelScript(void)
	{
		SAFE_DELETE_ARRAY(this->pBuffer);
	}

	const LevelScript& LevelScript::operator =(const LevelScript& _rhs)
	{
		if ( this->pBuffer != 0 )
			delete[] this->pBuffer;

		this->nSize = _rhs.nSize;
		this->fileName = _rhs.fileName;
		this->pBuffer = new char[_rhs.nSize];
		::memcpy(this->pBuffer, _rhs.pBuffer, _rhs.nSize);

		return *this;
	}

	const int LevelScript::doFile( sc::LuaInstance& _refDefaultLuaInstance )
	{
		std::string _fullPath(GLOGIC::GetServerPath());
		_fullPath.append("scripts\\instance\\");
		_fullPath.append(this->fileName);

		const HANDLE _hFile(::CreateFile(_fullPath.c_str(), GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0));
		if ( _hFile == INVALID_HANDLE_VALUE )
        {
            errorMessage(sc::string::format("%1% open failed", this->fileName));
            return -1;
        }

		unsigned int _nFileSizeHigh;
		this->nSize = ::GetFileSize(_hFile, (LPDWORD)&_nFileSizeHigh);

		this->pBuffer = new char[this->nSize];
		if ( this->pBuffer == 0 )
        {
            SAFE_DELETE_ARRAY(this->pBuffer);
            ::CloseHandle(_hFile);

            errorMessage(sc::string::format("%1% load failed", this->fileName));
            return -1;
        }

		unsigned int _nReadSize;
		const BOOL _bResult(::ReadFile(_hFile, this->pBuffer, this->nSize, (LPDWORD)&_nReadSize, 0));
		if ( _bResult == FALSE )
        {
            SAFE_DELETE_ARRAY(this->pBuffer);
            ::CloseHandle(_hFile);

            errorMessage(sc::string::format("%1% load failed", this->fileName));
            return -1;
        }

		::CloseHandle(_hFile);

		// level Script의 경우 인던이 생성 될 때 컴파일 되므로,
		// 임시로 스크립트 객체를 만들어서 컴파일을 한번 해본다;
		if ( _refDefaultLuaInstance.DoBuffer(this->pBuffer, this->nSize, this->fileName.c_str()) == -1 )
		{
			errorMessage(sc::string::format("%1% compile failed", this->fileName));
			return -1;
		}

        return 0;
	}



	
	
	InstanceScript::InstanceScript(void)
	{
	}
	InstanceScript::~InstanceScript(void)
	{
	}
	void InstanceScript::_destroy(void)
	{
		this->scriptInstance.OnDestroy();
	}	

    // 실패 하더라도 다른 기능들엔 영향이 가지 않도로 한다;
	const int InstanceScript::doFile(const std::string& _fileName)
	{
		ScriptManager* const _pScriptManager(ScriptManager::getInstance());

		std::string _fullPath(GLOGIC::GetServerPath());
		_fullPath.append("scripts\\instance\\");
		_fullPath.append(_fileName);

		if ( sc::lua_doFile(_fullPath) == false )
        {
            errorMessage(sc::string::format("%1% load failed", _fullPath));
            return -1;
        }

		bool _bSuccess(true);
		// Instance Name;
        LuaPlus::LuaObject _nameObject(sc::lua_getGlobalFromName(KEYWORD_NAME));
        if ( _nameObject.IsNil() == true )
        {
            errorMessage(sc::string::format("[%1%] not defined in %2%", KEYWORD_NAME, _fullPath));
            _bSuccess = false;
        }
		else
		{
			const std::wstring _WName((const wchar_t*)_nameObject.GetWString());

			const std::string _name(sc::string::unicodeToAnsi(_WName));
			this->instanceName = _name;
			this->nInstanceNameKey = InstanceSystem::generateHashKey((const void* const)_name.c_str(), _name.size());
		}
		//



		// default mapID;
		unsigned int _nMainMapID;
		unsigned int _nSubMapID;
        LuaPlus::LuaObject _keyMapMainIDObject(sc::lua_getGlobalFromName(KEYWORD_KEY_MAP_MAIN_ID));
        if ( _keyMapMainIDObject.IsNil() == true )
        {
            errorMessage(sc::string::format("[%1%] not defined in %2%", KEYWORD_KEY_MAP_MAIN_ID, _fullPath));
            _bSuccess = false;
        }
		else
			_nMainMapID = _keyMapMainIDObject.GetInteger();

        LuaPlus::LuaObject _keyMapSubIDObject(sc::lua_getGlobalFromName(KEYWORD_KEY_MAP_SUB_ID));
        if ( _keyMapSubIDObject.IsNil() == true )
        {
            errorMessage(sc::string::format("[%1%] not defined in %2%", KEYWORD_KEY_MAP_SUB_ID, _fullPath));
            _bSuccess = false;
        }
		else
			_nSubMapID = _keyMapSubIDObject.GetInteger();

		if ( _bSuccess == true )
			this->keyMapID = SNATIVEID(_nMainMapID, _nSubMapID); // 첫번째 mapID를 대표 mapID로 함;
        //


		// maxPlayer;
		LuaPlus::LuaObject _maxPlayerObject(sc::lua_getGlobalFromName(KEYWORD_MAX_PLAYER));
		if ( _maxPlayerObject.IsNil() == true )
		{
			errorMessage(sc::string::format("[%1%] not defined in %2%", KEYWORD_MAX_PLAYER, _fullPath));
			_bSuccess = false;
		}
		else
			this->nMaxPlayer = _maxPlayerObject.GetInteger();
		//


		// maxCreate;
		LuaPlus::LuaObject _maxCreateObject(sc::lua_getGlobalFromName(KEYWORD_MAX_CREATE));
		if ( _maxCreateObject.IsNil() == true )
		{
			errorMessage(sc::string::format("[%1%] not defined in %2%", KEYWORD_MAX_CREATE, _fullPath));
			_bSuccess = false;
		}
		else
			this->nMaxCreate = _maxCreateObject.GetInteger();
		//



        // isVisible 
        LuaPlus::LuaObject _isVisibleObject(sc::lua_getGlobalFromName(KEYWORD_IS_VISIBLE));
        if ( _isVisibleObject.IsNil() == true )
		{
			errorMessage(sc::string::format("[%1%] not defined in %2%", KEYWORD_IS_VISIBLE, _fullPath));
			_bSuccess = false;
		}
		else
        {
			const bool _isVisible(_isVisibleObject.GetBoolean());
			if ( _isVisible == true )
				this->nState += EMINSTANCE_SCRIPT_VISIBLE;
        }
        //



        // isUse;
        LuaPlus::LuaObject _isUseObject(sc::lua_getGlobalFromName(KEYWORD_IS_USE));
        if ( _isUseObject.IsNil() == true )
        {
            errorMessage(sc::string::format("[%1%] not defined in %2%", KEYWORD_IS_USE, _fullPath));
            _bSuccess = false;
        }
		else
		{
			const bool _isUse(_isUseObject.GetBoolean());
			if ( _isUse == true )
				this->nState += EMINSTANCE_SCRIPT_USE;
		}
        //



		// Include
		LuaPlus::LuaObject objInclude( sc::lua_getGlobalFromName( KEYWORD_INCLUDE ) );
		if ( false == objInclude.IsNil() )
		{
			for ( LuaPlus::LuaTableIterator iter(objInclude); iter; iter.Next() )
			{
				const std::wstring strScriptWFileName( (const wchar_t*)iter.GetValue().GetWString() );

				const std::string strScriptFileName( sc::string::unicodeToAnsi( strScriptWFileName ) );
				std::string strScriptFileFullPath( GLOGIC::GetServerPath() );
				strScriptFileFullPath.append( "scripts\\instance\\" );
				strScriptFileFullPath.append( strScriptFileName );
				if ( this->scriptInstance.DoFile( strScriptFileFullPath ) == -1 )
				{
					errorMessage( sc::string::format( "%1% compile(load) failed in %2%", strScriptFileFullPath, _fullPath ) );
					return -1;
				}
			}
		}
		//



        // public instance Agent Script;
        LuaPlus::LuaObject _publicAgentScript(sc::lua_getGlobalFromName(KEYWORD_PUBLIC_AGENT_SCRIPT));
        if ( _publicAgentScript.IsNil() == true )
        {
            errorMessage(sc::string::format("[%1%] not defined in %2%", KEYWORD_PUBLIC_AGENT_SCRIPT, _fullPath));
            _bSuccess = false;
        }

		if ( _bSuccess == false )
			return -1;

        for ( LuaPlus::LuaTableIterator _iteratorScript(_publicAgentScript); _iteratorScript; _iteratorScript.Next() )
        {
            const std::wstring _scriptWFileName((const wchar_t*)_iteratorScript.GetValue().GetWString());

            const std::string _scriptFileName(sc::string::unicodeToAnsi(_scriptWFileName));
            std::string _scriptFileFullPath(GLOGIC::GetServerPath());
            _scriptFileFullPath.append("scripts\\instance\\");
            _scriptFileFullPath.append(_scriptFileName);
            if ( this->scriptInstance.DoFile(_scriptFileFullPath) == -1 )
            {
                errorMessage(sc::string::format("%1% compile(load) failed in %2%", _scriptFileFullPath, _fullPath));
                return -1;
            }
			
            const std::string _strKeyMapID(sc::string::format("keyMapID = %d;", this->keyMapID.Id()));
            this->scriptInstance.DoString(_strKeyMapID.c_str());
        }
        //


        // public instance Field Script;
        LuaPlus::LuaObject _publicFieldScript(sc::lua_getGlobalFromName(KEYWORD_PUBLIC_FIELD_SCRIPT));
        if ( _publicFieldScript.IsNil() == true )
            return -1;
        for ( LuaPlus::LuaTableIterator _iteratorScript(_publicFieldScript); _iteratorScript; _iteratorScript.Next() )
        {
            const std::wstring _scriptWFileName((const wchar_t*)_iteratorScript.GetValue().GetWString());

            const std::string _scriptFileName(sc::string::unicodeToAnsi(_scriptWFileName));
            std::string _scriptFileFullPath(GLOGIC::GetServerPath());
            _scriptFileFullPath.append("scripts\\instance\\");
            _scriptFileFullPath.append(_scriptFileName);
            if ( this->scriptInstance.DoFile(_scriptFileFullPath) == -1 )
            {
                errorMessage(sc::string::format("%1% compile(load) failed in %2%", _scriptFileFullPath, _fullPath));
                return -1;
            }

            const std::string _strKeyMapID(sc::string::format("keyMapID = %d;", this->keyMapID.Id()));
            this->scriptInstance.DoString(_strKeyMapID.c_str());
        }
        //

		// level Agent Script : agent server instance에서 사용할 스크립트;
		{
			LuaPlus::LuaObject _levelAgentScript(sc::lua_getGlobalFromName(KEYWORD_LEVEL_AGENT_SCRIPT));
			if ( _levelAgentScript.IsNil() == true )
				return -1;

			sc::LuaInstance sLuaInstance;
			for ( LuaPlus::LuaTableIterator _iteratorLevel(_levelAgentScript); _iteratorLevel; _iteratorLevel.Next() )
			{
				const std::wstring _levelWFileName((const wchar_t*)_iteratorLevel.GetValue().GetWString());

				const std::string _levelFileName(sc::string::unicodeToAnsi(_levelWFileName));
				const unsigned int _nHashKey(InstanceSystem::generateHashKey((const void* const)_levelFileName.c_str(), _levelFileName.size()));

				if ( _pScriptManager->_pushScriptLevel(_levelFileName, _nHashKey, sLuaInstance) == -1 )
					return -1;

				this->levelAgentScriptHashVector.push_back(_nHashKey);			
			}
			sLuaInstance.OnDestroy();
		}
		//

		// level Field Script : field server instance에서 사용할 스크립트;
		{
			LuaPlus::LuaObject _levelFieldScript(sc::lua_getGlobalFromName(KEYWORD_LEVEL_FIELD_SCRIPT));
			if ( _levelFieldScript.IsNil() == true )
				return -1;

			sc::LuaInstance sLuaInstance;
			for ( LuaPlus::LuaTableIterator _iteratorLevel(_levelFieldScript); _iteratorLevel; _iteratorLevel.Next() )
			{
				const std::wstring _levelWFileName((const wchar_t*)_iteratorLevel.GetValue().GetWString());

				const std::string _levelFileName(sc::string::unicodeToAnsi(_levelWFileName));
				const unsigned int _nHashKey(InstanceSystem::generateHashKey((const void* const)_levelFileName.c_str(), _levelFileName.size()));

				if ( _pScriptManager->_pushScriptLevel(_levelFileName, _nHashKey, sLuaInstance) == -1 )
					return -1;

				this->levelFieldScriptHashVector.push_back(_nHashKey);			
			}
			sLuaInstance.OnDestroy();
		}
		//

		return 0;
	}





	InstanceInformationClient::InstanceInformationClient(void)
		: keyMapID(SNATIVEID(false))
	{
	}


	ScriptManager* const ScriptManager::getInstance(void)
	{
		static ScriptManager _instance;
		return &_instance;
	}
	ScriptManager::~ScriptManager(void)
	{
		for ( InstanceScriptMapIter _iterator(this->instanceScriptMap.begin()); _iterator != this->instanceScriptMap.end(); ++_iterator )
		{
			// lua 객체가 복사가 되지 않는 이유로,
			// 객체 소멸자가 아닌 총 관리 소멸자인 이곳에서 직접 destroy() 함수를 호출하여 제거 해 준다;
			// 물론 매니저 클래스는 서버가 꺼질때 사라지므로, 굳이 안해주더라도 릭이 발생하는 정도는 아니다;
			InstanceScript& _instanceScript = _iterator->second;
			_instanceScript._destroy();
		}
	}
	ScriptManager::ScriptManager(void)
	{
	}

	void ScriptManager::_reset(void)
	{
        for ( InstanceScriptMapIter _iterator(this->instanceScriptMap.begin()); _iterator != this->instanceScriptMap.end(); ++_iterator )
        {
            InstanceScript& _instanceScript = _iterator->second;
            _instanceScript._destroy();
        }
		this->instanceScriptMap.clear();
		this->levelScriptMap.clear();
		this->keyMapIDVector.clear();
	}

    // 란 특성상 정규 데이타를 처음에 모두 읽고 에러 발생 시 중단 시키는데;
    // 인스턴스 시스템에 한해서 실패 시 에러 메세지 출력 정도로 그치고;
    // 성공 한 부분에 대해서는 처리 할 수 있도록 한다;
    const int ScriptManager::reload(void)
    {
        return ScriptManager::doFile(this->fileName);
    }

	const int ScriptManager::doFile(const std::string& _fileName)
	{
		sc::lua_init();

        ScriptManager::_reset();

		if ( sc::lua_doFile(_fileName) == false )
        {
            errorMessage(sc::string::format("%1% load failed", _fileName));
            return -1;
        }

        this->fileName = _fileName;

		LuaPlus::LuaObject _instanceTable(sc::lua_getGlobalFromName(KEYWORD_INSTANCE_TABLE));
		if ( _instanceTable.IsNil() == true )
        {			
            errorMessage(sc::string::format("[%1%] not defined in %2%", KEYWORD_INSTANCE_TABLE, _fileName));
            return -1;
        }
	
		for ( LuaPlus::LuaTableIterator _iterator(_instanceTable); _iterator; _iterator.Next() )
		{
			const std::wstring _instanceWFileName((const wchar_t*)_iterator.GetValue().GetWString());
			const std::string _instanceFileName(sc::string::unicodeToAnsi(_instanceWFileName));			

			InstanceScript _scriptInstance;			
			if ( _scriptInstance.doFile(_instanceFileName) == -1 )
			{
                errorMessage(sc::string::format("%1% load failed in %2%", _instanceFileName, _fileName));
                continue;
			}

			const SNATIVEID& _keyMapID(_scriptInstance.keyMapID);
			if ( this->instanceScriptMap.find(_keyMapID) != this->instanceScriptMap.end() )
			{
				errorMessage(sc::string::format("duplicated baseMapID at %1% in %2%", _instanceFileName, _fileName));
				continue;
			}

			this->instanceScriptMap.insert(InstanceScriptValue(_keyMapID, _scriptInstance));
			this->keyMapIDVector.push_back(_keyMapID);
		}
		
		return 0;
	}

	const int ScriptManager::_pushScriptLevel(const std::string& _fileName, const unsigned int _nHashKey, sc::LuaInstance& _refDefaultLuaInstance)
	{
		LevelScriptMapCIter _iterator(this->levelScriptMap.find(_nHashKey));
		if ( _iterator != this->levelScriptMap.end() )
			return 0; // 이미 로딩 되어 있다면 다시 로딩 하지 않음;

		LevelScript _scriptLevel(_fileName);
		if ( _scriptLevel.doFile(_refDefaultLuaInstance) == -1 )
			return -1;

		this->levelScriptMap.insert(std::pair<HashKey, LevelScript>(_nHashKey, _scriptLevel));

		return 0;
	}


    const MapInformation& MapInformation::operator =(const MapInformation& _rhs)
    {
        ::memcpy(this, &_rhs, sizeof(MapInformation));
        return *this;
    }
    MapInformation::MapInformation(void)
        : targetMapID(false)
        , targetPosition(D3DXVECTOR3(0.0f, 0.0f, 0.0f)) 
        , targetActorID(Faction::GLActorID())
    {
    }
    MapInformation::MapInformation(const SNATIVEID& _targetMapID, const D3DXVECTOR3& _targetPosition, const Faction::GLActorID& _targetActorID)
        : targetMapID(_targetMapID)
        , targetPosition(_targetPosition)
        , targetActorID(_targetActorID)
    {
    }
    MapInformation::MapInformation(const MapInformation& _rhs)
        : targetMapID(_rhs.targetMapID)
        , targetPosition(_rhs.targetPosition)
        , targetActorID(_rhs.targetActorID)
    {
    }




    InstanceIDSet::InstanceIDSet(const SNATIVEID& _keyMapID, const InstanceMapID& _instanceMapID, const unsigned int _nStage)
        : keyMapID(_keyMapID)
        , instanceMapID(_instanceMapID)
        , nStage(_nStage)
    {
    }
	InstanceIDSet::InstanceIDSet(const SNATIVEID& _keyMapID, const SNATIVEID& _gaeaMapID)
		: keyMapID(_keyMapID)
		, instanceMapID(_gaeaMapID.Mid())
		, nStage(_gaeaMapID.Sid())
	{
	}
    InstanceIDSet::InstanceIDSet(const InstanceIDSet& _rhs)
        : keyMapID(_rhs.keyMapID)
        , instanceMapID(_rhs.instanceMapID)		
        , nStage(_rhs.nStage)
    {
    }


    const InformationPosition& InformationPosition::operator =(const InformationPosition& _rhs)
    {
        ::memcpy(this, &_rhs, sizeof(InformationPosition));
        return *this;
    }
    InformationPosition::InformationPosition(
		const Faction::GLActorID& _actorID,
		const SNATIVEID& _targetMapID, const Faction::GLActorID& _targetActorID,
        const float _fPositionX, const float _fPositionZ,
		const InstanceSystem::EMMOVE_TYPE _emMoveType)
        : emMoveType(_emMoveType)
		, actorID(_actorID)
		, targetActorID(_targetActorID)
		, targetMapID(_targetMapID)        
        , fPositionX(_fPositionX)
        , fPositionZ(_fPositionZ)
		, bResurrect(false)
    {
    }
    InformationPosition::InformationPosition(const InformationPosition& _rhs)
		: emMoveType(_rhs.emMoveType)
		, actorID(_rhs.actorID)
		, targetMapID(_rhs.targetMapID)
        , targetActorID(_rhs.targetActorID)
        , fPositionX(_rhs.fPositionX)
        , fPositionZ(_rhs.fPositionZ)
		, bResurrect(_rhs.bResurrect)
    {
    }	
    InformationPosition::InformationPosition(void)
        : emMoveType(InstanceSystem::EMMOVE_NSIZE)
		, actorID(Faction::GLActorID())
		, targetActorID(Faction::GLActorID())
		, targetMapID(false)        
        , fPositionX(0.0f)
        , fPositionZ(0.0f)
		, bResurrect(false)
    {
    }

    InstanceLog::InstanceLog(void)
        : keyMapID(false)
        , nUsedCount(0)
        , nRunningCount(0)
		, nMaxCreate(0)
    {
    }
    InstanceLog::InstanceLog(const SNATIVEID& _keyMapID, const unsigned int _nUsedCount, const unsigned int _nRunningCount, const unsigned int _nMaxCreate)
        : keyMapID(_keyMapID)
        , nUsedCount(_nUsedCount)
        , nRunningCount(_nRunningCount)
		, nMaxCreate(_nMaxCreate)
    {
    }

    InstanceList::InstanceList(void)
        : keyMapID(false)
    {
    }
    InstanceList::InstanceList(const SNATIVEID& _keyMapID, const InstanceMapID _instanceMapID)
        : keyMapID(_keyMapID)
        , instanceMapID(_instanceMapID)
    {
    }


    InstanceTimer::InstanceTimer(TimerHandle _hTimerHandle, const float _fLifeTime, const DWORD _dwParam0, const DWORD _dwParam1, const DWORD _dwParam2, const DWORD _dwParam3, const bool _bLoop)
        : hTimerHandle(_hTimerHandle)
        , tStartTime(CTime::GetCurrentTime().GetTime())
        , fLifeTime(_fLifeTime)
        , fElapsedTime(_fLifeTime)
        , bPause(false)
        , m_dwParam0(_dwParam0)
        , m_dwParam1(_dwParam1)
        , m_dwParam2(_dwParam2)
        , m_dwParam3(_dwParam3)
        , bLoop(_bLoop)
    {

    }

    InstanceTimer::~InstanceTimer()
    {
    }

    InstanceAbsTimer::InstanceAbsTimer(TimerHandle _hTimerHandle, WORD _wWeek, WORD _wDay, WORD _wHour, WORD _wMinute, WORD _wSecond,
                                       const DWORD _dwParam0, const DWORD _dwParam1, const DWORD _dwParam2, const DWORD _dwParam3, const bool _bLoop)
        : hTimerHandle(_hTimerHandle)
        , wWeek(_wWeek)
        , wDay(_wDay)
        , wHour(_wHour)
        , wMinute(_wMinute)
        , wSecond(_wSecond)
        , m_dwParam0(_dwParam0)
        , m_dwParam1(_dwParam1)
        , m_dwParam2(_dwParam2)
        , m_dwParam3(_dwParam3)
        , bLoop(_bLoop)
    {
    }

    InstanceAbsTimer::~InstanceAbsTimer()
    {
    }




	PacketCounterElement::PacketCounterElement(const unsigned int _indexPacket)
		: indexPacket(_indexPacket)
		, countPacket(0)
		, countPacketPrevious(0)
	{
	}




	PacketCounter::PacketCounter(const unsigned int _sizePacket, const unsigned int _timerReset)
		: timerReset(_timerReset)
		, sizeMessageNotify(5)
		, timeCumulative(0)
		, timeGap(0)
	{
		InstanceSystem::PacketCounterElementVector& _vectorPacketCounterElement(this->vectorPacketCounterElement);
		for ( unsigned int _index(0); _index < _sizePacket; ++_index )
			_vectorPacketCounterElement.push_back(InstanceSystem::PacketCounterElement(_index));
	}
	void PacketCounter::_clear(void)
	{
		InstanceSystem::PacketCounterElementVector& _vectorPacketCounterElement(this->vectorPacketCounterElement);
		const unsigned int _sizePacket(_vectorPacketCounterElement.size());
		for ( unsigned int _index(_sizePacket); _index--; )
		{
			InstanceSystem::PacketCounterElement& _elementPacketCounter(_vectorPacketCounterElement[_index]);
			_elementPacketCounter.clear();
		}
		this->countNotify = 0;
	}
	const unsigned int PacketCounter::increase(const unsigned int _indexPacket)
	{
		InstanceSystem::PacketCounterElementVector& _vectorPacketCounterElement(this->vectorPacketCounterElement);
		const unsigned int _sizePacketElement(_vectorPacketCounterElement.size());
		if ( _indexPacket >= _sizePacketElement )
			return 0; // 잘못 된 패킷 번호라면 취소;

		const unsigned int _timeCurrent(::clock());
		const unsigned int _timeGap(_timeCurrent - this->timeCumulative);		
		this->timeGap = _timeGap;

		InstanceSystem::PacketCounterElement& _elementPacketCounter(_vectorPacketCounterElement[_indexPacket]);
		const unsigned int _sizePacket(_elementPacketCounter.increase());
		if ( _sizePacket >= this->sizeMessageNotify )
		{ // 단위 시간 당 패킷 수신량이 지정된 회수 이상이면;
			if ( ++this->countNotify > 2 )
			{ // 너무 많다 싶으면 지정된 회수를 5개씩 늘려간다;				
				this->countNotify = 0;
				this->sizeMessageNotify += 5;
				_elementPacketCounter.clear();
			}
			
			return _sizePacket;
		}

		if ( _timeGap < this->timerReset )
			return 0;

		PacketCounter::_clear();
		this->timeCumulative = _timeCurrent;

		return false;
	}
}