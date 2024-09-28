#include "stdafx.h"

#include "./Define.h"
#include "./MinLua.h"

namespace lua
{
    LuaPlus::LuaState* g_pLua = NULL;
	std::string lua_strError = "";

	void lua_init()
	{
		if (!g_pLua)
		{
			g_pLua = LuaPlus::LuaState::Create();
			lua_State* L = g_pLua->GetCState();
			// Lua 기본 함수들을 로드한다.			
			luaopen_base(L);
			luaopen_table(L);
			//luaopen_io(L);
			luaopen_os(L);
			luaopen_string(L);
			luaopen_math(L);
			luaopen_debug(L);
			//luaopen_package(L);
			//luaL_openlibs(L);
		}
	}

	LuaPlus::LuaState* lua_getState()
	{
		if (!g_pLua)
			lua_init();
		return g_pLua;
	}

	LuaPlus::LuaState* lua_getThreadState()
	{		
		LuaPlus::LuaObject LO = LuaPlus::LuaState::CreateThread( lua_getState() );
		return LO.GetState();
	}

	void lua_destroyState()
	{
		if (g_pLua)
		{
			LuaPlus::LuaState::Destroy(g_pLua);
			g_pLua = NULL;
		}
	}

	// 가비지 컬렉션으로 자동 처리된다
	/*
	void lua_destroyThreadState( LuaPlus::LuaState*& pState )
	{
		if ( NULL != pState )
		{
			LuaPlus::LuaState::Destroy( pState );
			pState = NULL;
		}
	}
	*/

	LuaPlus::LuaObject lua_getGlobalFromName( const char* szName )
	{
		return g_pLua->GetGlobals()[szName];
	}

    bool lua_doFile(const std::string& filename, LuaPlus::LuaState* pState)
    {
        return lua_doFile(filename.c_str(), pState);
    }

	bool lua_doFile(const char *filename, LuaPlus::LuaState* pState)
	{
		if (!pState)
			pState = g_pLua;

		if (0 != pState->DoFile(filename))
		{
			lua_strError = LuaPlus::LuaStackObject(pState, -1).GetString();
			return false;
		}
		else
		{
			return true;
		}
	}

	bool lua_doBuffer(const char* buff, size_t size, const char* name, LuaPlus::LuaState* pState)
	{
		if (!pState)
			pState = g_pLua;

		if (0 != pState->DoBuffer(buff, size, name))
		{
			lua_strError = LuaPlus::LuaStackObject(pState, -1).GetString();
			return false;
		}
		else
		{
			return true;
		}
	}

	std::string lua_getError()
	{
		return lua_strError;
	}
}