#pragma once
//
//mjeon.CaptureTheField
//

#include "./ScriptMan.h"

namespace LuaPlus
{
	class LuaState;
}

class LuaMan: public IScriptMan
{
public:
	LuaMan();
	virtual ~LuaMan();

	void LoadScript(const char *szFilePath);
	virtual void ToDo() = 0;

	virtual BOOL IsLoaded() {return m_bLoaded;}

protected:

	LuaPlus::LuaState*  m_pLuaState;
};
