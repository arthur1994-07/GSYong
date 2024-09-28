#ifndef _RAN_LOGIC_CLIENT_SCRIPT_MANAGER_H_
#define _RAN_LOGIC_CLIENT_SCRIPT_MANAGER_H_

#pragma once

#include <string>

typedef struct lua_State lua_State;
class ClientLuaScript;
class GLGaeaClient;

class ClientScriptManager
{
public:
    lua_State* m_pMasterState;
    ClientLuaScript* m_pGLMacro;
    static GLGaeaClient* m_pGaeaClient;

    ClientScriptManager(GLGaeaClient* pGaeaClient);
    ~ClientScriptManager(void);

    void Bind();
    bool Run(const std::string& FileName);
    bool Run(const std::wstring& FileName);
    void UnlinkScript();
    void Update(float elapsedSec);
    int NumScripts();

public:
    static ClientLuaScript* GetScriptObject(lua_State* l);

    static int print(lua_State* l);
    static int chat(lua_State* l);
    static int move(lua_State* l);
    static int quickSlot(lua_State* l);
    static int announce(lua_State* l);
    static int sleep(lua_State* l);

    static int eventItemBegin(lua_State* l);
    static int eventItemEnd(lua_State* l);

    static int eventMoneyBegin(lua_State* l);
    static int eventMoneyEnd(lua_State* l);

    static int eventExpBegin(lua_State* l);
    static int eventExpEnd(lua_State* l);

    static int freePkOff(lua_State* l);
    static int freePkOn(lua_State* l);

    static int levDel(lua_State* l);
    static int levNew(lua_State* l);

    static int mobGen(lua_State* l);
    static int mobDel(lua_State* l);

    static int moveToGate(lua_State* l);	
    static int moveToPos(lua_State* l);

    static int visibleOn(lua_State* l);
    static int visibleOff(lua_State* l);
    static int visibleNone(lua_State* l);

    static int enchant(lua_State* l);
    static int enchantRestore(lua_State* l);
};

#endif // _RAN_LOGIC_CLIENT_SCRIPT_MANAGER_H_