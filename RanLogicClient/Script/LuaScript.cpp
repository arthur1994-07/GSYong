#include "pch.h"

#include "../../SigmaCore/Lua/MinLua.h"
#include "../../SigmaCore/Lua/lua_tinker.h"
#include "../../SigmaCore/String/StringFormat.h"

#include "../../EngineLib/GUInterface/UITextControl.h"

#include "../GLGaeaClient.h"

#include "./ScriptManager.h"
#include "./LuaScript.h"

#include "../../SigmaCore/DebugInclude.h"

ClientLuaScript::ClientLuaScript(
    ClientScriptManager* pMacroManager,
    GLGaeaClient* pGaeaClient)
    : m_pMacroManager(pMacroManager)
    , m_pGaeaClient(pGaeaClient)
{
    m_time          = 0;
    m_LoadSuccess   = false;	
    m_State         = LSS_NOTLOADED;
    m_ThreadState   = lua_newthread(m_pMacroManager->m_pMasterState);

    // save a pointer to the thread manager object in the global table
    // using the new thread's vm pointer as a key
    lua_pushlightuserdata(m_pMacroManager->m_pMasterState, m_ThreadState);
    lua_pushlightuserdata(m_pMacroManager->m_pMasterState, this);
    lua_settable(m_pMacroManager->m_pMasterState, LUA_GLOBALSINDEX);
}

ClientLuaScript::~ClientLuaScript()
{	
}

void ClientLuaScript::SetWaitTime(const float WaitTime)
{
    m_waitTimestamp = WaitTime;
}

bool ClientLuaScript::Run(const std::wstring& MacroFile)
{
    std::string FileName = sc::string::unicodeToAnsi(MacroFile.c_str(), (int) MacroFile.length());
    return Run(FileName);
}

bool ClientLuaScript::Run(const std::string& MacroFile)
{
    int Return;
    std::string Msg;
    
    Return = luaL_loadfile(m_ThreadState, MacroFile.c_str());
    if (0 == Return)
    {
        Msg = sc::string::format("%1% start", MacroFile);
        m_LoadSuccess = true;
    }
    else
    {
        Msg = sc::string::format("%1% load failed", MacroFile);
        m_LoadSuccess = false;		
    }

    m_pGaeaClient->PrintMsgTextDlg(NS_UITEXTCOLOR::YELLOW, Msg);

    if (!m_LoadSuccess)
        return false;

    ResumeScript(0.0f);

    return true;
}

void ClientLuaScript::ResumeScript(float param)
{	
    // we're about to Run/resume the thread, so set the global
    int status;
    m_State = LSS_RUNNING;

    // param is treated as a return value from the function that yielded
    lua_pushnumber(m_ThreadState, param);
    status = lua_resume(m_ThreadState, 1);

    if (status)
    {
        PrintError();		
    }
}

void ClientLuaScript::Update(float ElspTime)
{
    m_time += ElspTime;

    switch (m_State)
    {
    case LSS_WAITTIME:
        if (m_time >= m_waitTimestamp)
            ResumeScript(0.0f);
        break;	
    case LSS_NOTLOADED:
        break;
    default :
        break;
    }
}

void ClientLuaScript::PrintError()
{
    const char* msg;
    msg = sc::GetStringToLuaState(m_ThreadState, -1);
    if (msg == NULL)
    {
        msg = "error with no message";
        return;
    }

    char ErrorString[256] = {0};
    lua_pop(m_ThreadState, 1);
    StringCchCopyA(ErrorString, 256, msg);
    m_LastErrorMsg = ErrorString;
    m_pGaeaClient->PrintMsgTextDlg(NS_UITEXTCOLOR::RED, m_LastErrorMsg);
}