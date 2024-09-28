#include "pch.h"
#include "./GLMatchingScriptManager.h"
#include "../MatchSystem/MatchingDefine.h"
#include "../../EngineLib/G-Logic/GLOGIC.h"
#include "../../SigmaCore/Lua/MinLua.h"
#include "../../SigmaCore/Util/SystemInfo.h"
#include "../../SigmaCore/Log/LogMan.h"
#include "../../SigmaCore/Util/MinMessageBox.h"

#define MATCHING_SCRIPT_PATH "scripts\\Matching\\"


namespace MatchingSystem
{

	const int ScriptManager::getScriptCOPY(const DWORD& _MatchingID,sc::LuaInstance* _scriptReference) const
	{
// 		TournamentScriptMapCIter _iteratorScript(this->TournametScriptMap.find(_MatchingID));
// 		if ( _iteratorScript == this->TournametScriptMap.end() )
// 			return -1;

		if(instanceScriptVector.size() < _MatchingID)
			return -1;

		const MatchingCoreScript& _instanceScript = instanceScriptVector[_MatchingID];

		if ( _scriptReference->DoBuffer(_instanceScript.pBuffer, _instanceScript.nSize, _instanceScript.fileName.c_str()) == -1 )
			return -1;
		return 0;
	}
	ScriptManager::ScriptManager()
	{
	}

	ScriptManager::~ScriptManager(void)
	{
		/*
		for ( TournamentScriptMapCIter _iterator(this->TournametScriptMap.begin()); _iterator != this->TournametScriptMap.end(); ++_iterator )
		{
			// lua 객체가 복사가 되지 않는 이유로,
			// 객체 소멸자가 아닌 총 관리 소멸자인 이곳에서 직접 destroy() 함수를 호출하여 제거 해 준다;
			// 물론 매니저 클래스는 서버가 꺼질때 사라지므로, 굳이 안해주더라도 릭이 발생하는 정도는 아니다;
			const MatchingCoreScript& _instanceScript = _iterator->second;
			_instanceScript._destroy();
		}*/
		TournametScriptMap.clear();
	}
	const bool ScriptManager::doReset()
	{
		TournametScriptMap.clear();
		instanceScriptVector.clear();
		return true;
	}

	const bool ScriptManager::doFile(const std::string& _fileName,const std::string& _paramName)
	{
		
		std::string FullPath(GLOGIC::GetServerPath());
		FullPath.append(MATCHING_SCRIPT_PATH);
		FullPath.append(_fileName);

		sc::lua_init();
		if ( sc::lua_doFile(FullPath) == false )
		{
			std::string ErrMsg(
				sc::string::format(
				"%1% file missing. (error 5512)",
				FullPath));
			sc::ErrorMessageBox(ErrMsg, false);
			return false;
		}

		LuaPlus::LuaObject _instanceTable(sc::lua_getGlobalFromName(_paramName.c_str()));
		if ( _instanceTable.IsNil() == true )
		{
			sc::writeLogInfo(sc::string::format("MatchingSystem : ScriptManager Script param dose not Exsist : %s",_paramName));
			return false;
		}

		if(_instanceTable.IsTable() == false)
		{
			sc::writeLogInfo(sc::string::format("MatchingSystem : Table Not Exsist : %s",_paramName));
			return false;
		}

		for ( LuaPlus::LuaTableIterator _iterator(_instanceTable); _iterator; _iterator.Next() )
		{
			const std::wstring _instanceWFileName((const wchar_t*)  _iterator.GetValue().GetWString());
			const std::string _instanceFileName(sc::string::unicodeToAnsi(_instanceWFileName));	

			MatchingCoreScript _scriptInstance;			
			if ( _scriptInstance.doFile(_instanceFileName) == -1 )
			{
				return false;
			}

			const int _nInstanceID(this->instanceScriptVector.size());
			this->instanceScriptVector.push_back(_scriptInstance);
		}
		sc::writeLogInfo(sc::string::format("MatchingSystem : Table successfully Load : %s",_paramName));
	
		return true;
	}


	const int ScriptManager::doFileDirect(const std::string& _fileName)
	{
		sc::lua_init();


		MatchingCoreScript _scriptInstance;			
		if ( _scriptInstance.doFile(_fileName) == -1 )
		{
			return -1;
		}

		const int _nInstanceID(this->instanceScriptVector.size());
		this->instanceScriptVector.push_back(_scriptInstance);

		return _nInstanceID;
	}


	const bool ScriptManager::_destroy(const unsigned int& _instanceMapID)
	{
		TournamentScriptMapCIter _iteratorInstance(this->TournametScriptMap.find(_instanceMapID));
		if ( _iteratorInstance == this->TournametScriptMap.end() )
			return false;

		const MatchingCoreScript& _instanceField = _iteratorInstance->second;
		// lua를 파괴하고 맵에서 삭제 시킨다;
		// lua객체의 경우 깊은복사가 안되서 소멸자를 이용할 수 없다;
		// 직접 해줘야 함;
		this->TournametScriptMap.erase(_iteratorInstance);
		return true;
	}







	MatchingCoreScript::MatchingCoreScript()
		: fileName("")
		, nSize(0)
		, pBuffer(0)		
	{		
	}

	MatchingCoreScript::MatchingCoreScript(const MatchingCoreScript& _rhs)
		: fileName(_rhs.fileName)
		, nSize(_rhs.nSize)
		, pBuffer(new char[_rhs.nSize])
	{
		::memcpy(this->pBuffer, _rhs.pBuffer, _rhs.nSize);
	}

	MatchingCoreScript::~MatchingCoreScript(void)
	{
		if ( this->pBuffer != 0 )
		{
			delete[] this->pBuffer;
		}
	}

	const MatchingCoreScript& MatchingCoreScript::operator =(const MatchingCoreScript& _rhs)
	{
		if ( this->pBuffer != 0 )
			delete[] this->pBuffer;

		this->nSize = _rhs.nSize;
		this->fileName = _rhs.fileName;
		this->pBuffer = new char[_rhs.nSize];
		::memcpy(this->pBuffer, _rhs.pBuffer, _rhs.nSize);

		return *this;
	}

	const int MatchingCoreScript::doFile(const std::string& _fileName)
	{
		this->fileName = _fileName;
		std::string _fullPath(GLOGIC::GetServerPath());
		_fullPath.append(MATCHING_SCRIPT_PATH);
		_fullPath.append(_fileName);

		const HANDLE _hFile(::CreateFile(_fullPath.c_str(), GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0));
		if ( _hFile == INVALID_HANDLE_VALUE )
			return -1;

		unsigned int _nFileSizeHigh;
		this->nSize = ::GetFileSize(_hFile, (LPDWORD)&_nFileSizeHigh);

		this->pBuffer = new char[this->nSize];
		if ( this->pBuffer == 0 )
			goto doFile_error;

		unsigned int _nReadSize;
		const BOOL _bResult(::ReadFile(_hFile, this->pBuffer, this->nSize, (LPDWORD)&_nReadSize, 0));
		if ( _bResult == FALSE )
			goto doFile_error;

		::CloseHandle(_hFile);
		return 0;


doFile_error:
		if ( this->pBuffer != 0 )
		{
			delete[] this->pBuffer;
			this->pBuffer = 0;
		}
		::CloseHandle(_hFile);
		return -1;
	}

}



