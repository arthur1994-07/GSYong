#pragma once

#include <string>
#include "../../=LuaPlus/src/LuaPlus/LuaPlus.h"

extern "C"
{
	#include "../../=LuaPlus/src/LuaPlus/src/lualib.h"
}

namespace lua
{
	extern LuaPlus::LuaState* g_pLua;

	extern std::string lua_strError;

	void lua_init();

	LuaPlus::LuaState* lua_getState();
	LuaPlus::LuaState* lua_getThreadState();
	void lua_destroyState();

	LuaPlus::LuaObject lua_getGlobalFromName(const char* szName);

	bool lua_doFile(const std::string& filename, LuaPlus::LuaState* pState=NULL);
	bool lua_doFile(const char* filename, LuaPlus::LuaState* pState=NULL);

	bool lua_doBuffer(const char* buff, size_t size, const char* name, LuaPlus::LuaState* pState=NULL);
	std::string lua_getError();
}
