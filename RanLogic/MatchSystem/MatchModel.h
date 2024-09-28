#pragma once

#include "GLCallbackTrigger.h"
#include "../../SigmaCore/Lua/MinLua.h"

using namespace CallbackUtility;

namespace MatchingSystem
{

	class CopyScript
	{
	public:
		sc::LuaInstance* Resualt;
	};

	class RoomReceipt
	{
	public:
		bool _bCreateComplete;
		DWORD _instanceMapID;
		DWORD _MatchingInstanceID;
		DWORD dwMatchingBaseID;
	};

	class RoomMultyReceipt
	{
	public:
		std::vector<DWORD> _instanceMapID;
		std::vector<bool> _bCreateComplete;
		lua_State* pState;
	};

	class DestroyReceipt
	{
	public:
		bool _bDestroyComplete;
		DWORD _instanceMapID;
	};

	class JoinReceipt
	{
	public:
		DWORD _instanceMapID;
		DWORD ClientDB;
		bool isFail;
	};

	class ReadyReceipt
	{
	public:
		DWORD ClietDBID;
		bool isOK;
	};

	class InstancePopUpReceipt
	{
	public:
		const char* TimerName;
		DWORD dwTime;
		bool isRevers;
		bool isShow;
	};
}