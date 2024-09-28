#ifndef _RAN_LOGIC_CLIENT_LUA_SCRIPT_H_
#define _RAN_LOGIC_CLIENT_LUA_SCRIPT_H_

#pragma once

class GLGaeaClient;
class GLCharacter;

namespace LuaPlus {
    class LuaState;
};

enum LUASCRIPTSTATE
{
    LSS_WAITFRAME,
    LSS_WAITTIME,
    LSS_RUNNING,
    LSS_NOTLOADED,
    LSS_DONE
};

class ClientScriptManager;
class GLGaeaClient;

//! class ClientLuaScript
//! 2009-03-20 Jgkim
//! 게임 매크로 시스템
class ClientLuaScript
{
public:
    ClientLuaScript(ClientScriptManager* pMacroManager, GLGaeaClient* pGaeaClient);
    virtual ~ClientLuaScript();

protected:
    ClientScriptManager*  m_pMacroManager;
    LUASCRIPTSTATE m_State;
    lua_State*     m_ThreadState;
    bool m_LoadSuccess;
    float m_waitTimestamp;      //! time to wake up
    float m_time;               //! current time
    std::string m_LastErrorMsg;
    GLGaeaClient* m_pGaeaClient;

protected:
    void ResumeScript(float param);
    void PrintError();

public:
    bool Run(const std::wstring& MacroFile);
    bool Run(const std::string& MacroFile);
    void Update(float ElspTime);

    void  SetWaitTime(const float WaitTime);
    float GetCurTime() const { return m_time; }
    void  SetState(LUASCRIPTSTATE State) { m_State=State; }

};

#endif // _RAN_LOGIC_CLIENT_LUA_SCRIPT_H_