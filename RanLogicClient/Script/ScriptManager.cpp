#include "pch.h"
#include "../../SigmaCore/Lua/MinLua.h"
#include "../../SigmaCore/Lua/lua_tinker.h"

#include "../GLGaeaClient.h"
#include "../Char/GLCharacter.h"
#include "./LuaScript.h"
#include "./ScriptManager.h"

#include "../../SigmaCore/DebugInclude.h"

GLGaeaClient* ClientScriptManager::m_pGaeaClient = NULL;

ClientScriptManager::ClientScriptManager(GLGaeaClient* pGaeaClient)
    : m_pGLMacro(NULL)	
{
    m_pGaeaClient = pGaeaClient;
    m_pMasterState = sc::lua_getState()->GetCState();
    Bind();
    //LuaOpenScriptLib(m_pMasterState);
}

ClientScriptManager::~ClientScriptManager()
{
    SAFE_DELETE(m_pGLMacro);
}

bool ClientScriptManager::Run(const std::wstring& FileName)
{
    std::string ScriptFileName = sc::string::unicodeToAnsi(FileName);
    return Run(ScriptFileName);
}

bool ClientScriptManager::Run(const std::string& FileName)
{
    bool bReturn = true;
    SAFE_DELETE(m_pGLMacro);
    m_pGLMacro = new ClientLuaScript(this, m_pGaeaClient);
    if (false == m_pGLMacro->Run(FileName))
    {
        SAFE_DELETE(m_pGLMacro);
        bReturn = false;
    }
    return bReturn;
}

void ClientScriptManager::UnlinkScript()
{
    SAFE_DELETE(m_pGLMacro);
}

void ClientScriptManager::Update(float elapsedSec)
{
    if (m_pGLMacro)
        m_pGLMacro->Update(elapsedSec);	
}

static const luaL_reg scriptLib[] =
{
    {"Announce",        &ClientScriptManager::announce},
    {"Chat",            &ClientScriptManager::chat},	
    {"Enchant",         &ClientScriptManager::enchant},
    {"EnchantRestore",  &ClientScriptManager::enchantRestore},
    {"EventItemBegin",  &ClientScriptManager::eventItemBegin},
    {"EventItemEnd",    &ClientScriptManager::eventItemEnd},
    {"EventMoneyBegin", &ClientScriptManager::eventMoneyBegin},
    {"EventMoneyEnd",   &ClientScriptManager::eventMoneyEnd},
    {"EventExpBegin",   &ClientScriptManager::eventExpBegin},
    {"EventExpEnd",     &ClientScriptManager::eventExpEnd},
    {"FreePkOff",       &ClientScriptManager::freePkOff},
    {"FreePkOn",        &ClientScriptManager::freePkOn},
    {"LevDel",          &ClientScriptManager::levDel},
    {"LevNew",          &ClientScriptManager::levNew},
    {"MobGen",          &ClientScriptManager::mobGen},
    {"MobDel",          &ClientScriptManager::mobDel},
    {"MoveToGate",      &ClientScriptManager::moveToGate},
    {"M2G",             &ClientScriptManager::moveToGate},
    {"MoveToPos",       &ClientScriptManager::moveToPos},
    {"M2P",             &ClientScriptManager::moveToPos},
    {"Print",           &ClientScriptManager::print},    
    {"Sleep",           &ClientScriptManager::sleep},
    {"VisibleOn",       &ClientScriptManager::visibleOn},
    {"VisibleOff",      &ClientScriptManager::visibleOff},
    {"VisibleNone",     &ClientScriptManager::visibleNone},
    {"Move",            &ClientScriptManager::move},
    {"QuickSlot",       &ClientScriptManager::quickSlot},
    {NULL, NULL}
};

void ClientScriptManager::Bind()
{
    luaL_openlib(m_pMasterState, "Macro", scriptLib, 0);
}

ClientLuaScript* ClientScriptManager::GetScriptObject(lua_State* l)
{
    lua_pushlightuserdata(l, l);
    lua_gettable(l, LUA_GLOBALSINDEX);
    return ((ClientLuaScript*) lua_touserdata(l, -1));
}

int ClientScriptManager::print(lua_State* l)
{
    /*
    ClientLuaScript* s = GetScriptObject(l);
    size_t MsgSize=0;
    const char* Msg = luaL_checklstring(l, 1, &MsgSize);    
    m_pGaeaClient->PrintMsgTextDlg(NS_UITEXTCOLOR::DEFAULT, std::string(Msg));
    */
    return 0;
}

int ClientScriptManager::chat(lua_State* l)
{
    /*
    ClientLuaScript* s = GetScriptObject(l);
    size_t MsgSize=0;
    const char* Msg = luaL_checklstring(l, 1, &MsgSize);    
    std::string strMsg = sc::string::utf8_to_ansi(Msg, (int) strlen(Msg));
    m_pGaeaClient->ChatMacro(strMsg);
    */
    return 0;
}

int ClientScriptManager::move(lua_State* l)
{	
    /*
    ClientLuaScript* s = GetScriptObject(l);
    GLLandClient* pLandClient = m_pGaeaClient->GetActiveMap();
    if ( pLandClient )
    {
        D3DXVECTOR3 Pos;
        pLandClient->Map2WorldPos(
            (int) luaL_checknumber(l, 1),
            (int) luaL_checknumber(l, 2),
            Pos );
        GLCharacter* pCharacter = m_pGaeaClient->GetMyCharacter();
        pCharacter->ActionMoveTo( Pos, FALSE, FALSE );
    }
    */
    return 0;
}

//! Quick Slot 사용 요청
//! EM_COMMON_SLOT_MAX 이하
int ClientScriptManager::quickSlot(lua_State* l)
{
    /*
    ClientLuaScript* s = GetScriptObject(l);
    m_pGaeaClient->ReqCommonRunSet( (WORD) luaL_checknumber(l, 1) );
    */
    return 0;
}

int ClientScriptManager::announce(lua_State* l)
{
    /*
    ClientLuaScript* s = GetScriptObject(l);
    size_t MsgSize=0;
    const char* Msg = luaL_checklstring( l, 1, &MsgSize );
    std::wstring strMsg = StringUtil::utf8ToUnicode( Msg, (int) strlen(Msg) );
    m_pGaeaClient->ChatGlobal( strMsg );
    */
    return 0;
}

int ClientScriptManager::sleep(lua_State* l)
{
    /*
    ClientLuaScript* s = GetScriptObject(l);
    s->SetWaitTime( s->GetCurTime() + (float) luaL_checknumber(l, 1) );
    s->SetState( LSS_WAITTIME );
    */
    return lua_yield(l, 1);
}

int ClientScriptManager::eventItemBegin(lua_State* l)
{
    /*
    ClientLuaScript* s = GetScriptObject(l);
    float Scale = luaL_checknumber(l, 1);
    m_pGaeaClient->GmEventItemBegin( Scale );
    */
    return 0;
}

int ClientScriptManager::eventItemEnd(lua_State* l)
{
    //m_pGaeaClient->GmEventItemEnd();
    return 0;
}

int ClientScriptManager::eventMoneyBegin(lua_State* l)
{
    /*
    ClientLuaScript* s = GetScriptObject(l);
    float Scale = luaL_checknumber(l, 1);
    m_pGaeaClient->GmEventMoneyBegin( Scale );
    */
    return 0;
}

int ClientScriptManager::eventMoneyEnd(lua_State* l)
{
    //m_pGaeaClient->GmEventMoneyEnd();
    return 0;
}

int ClientScriptManager::eventExpBegin(lua_State* l)
{
    /*
    ClientLuaScript* s = GetScriptObject(l);
    float Scale = luaL_checknumber(l, 1);
    m_pGaeaClient->GmEventExpBegin( Scale );
    */
    return 0;
}

int ClientScriptManager::eventExpEnd(lua_State* l)
{
    //m_pGaeaClient->GmEventExpEnd();
    return 0;
}

int ClientScriptManager::freePkOff(lua_State* l)
{
    //m_pGaeaClient->GmFreePk(0);
    return 0;
}

int ClientScriptManager::freePkOn(lua_State* l)
{
    /*
    ClientLuaScript* s = GetScriptObject(l);
    DWORD Min = (DWORD) luaL_checknumber(l, 1);
    m_pGaeaClient->GmFreePk(Min);
    */
    return 0;
}

int ClientScriptManager::levDel(lua_State* l)
{
    //m_pGaeaClient->GmLevelDel();
    return 0;
}

int ClientScriptManager::levNew(lua_State* l)
{
    /*
    ClientLuaScript* s = GetScriptObject(l);
    size_t MsgSize=0;
    const char* Msg = luaL_checklstring( l, 1, &MsgSize );
    std::wstring LevelFile = StringUtil::utf8ToUnicode( Msg, (int) strlen(Msg) );
    m_pGaeaClient->GmLevelNew( LevelFile );
    */
    return 0;
}

int ClientScriptManager::mobGen(lua_State* l)
{
    /*
    ClientLuaScript* s = GetScriptObject(l);
    WORD MobMid = (WORD) luaL_checknumber(l, 1);
    WORD MobSid = (WORD) luaL_checknumber(l, 2);
    WORD PosX   = (WORD) luaL_checknumber(l, 3);
    WORD PosY   = (WORD) luaL_checknumber(l, 4);
    m_pGaeaClient->GmMobGen( MobMid, MobSid, PosX, PosY );
    */
    return 0;
}

int ClientScriptManager::mobDel(lua_State* l)
{
    //m_pGaeaClient->GmMobDel(0);
    return 0;
}

int ClientScriptManager::moveToGate(lua_State* l)
{
    /*
    ClientLuaScript* s = GetScriptObject(l);
    DWORD GateID = (DWORD) luaL_checknumber(l, 1);
    m_pGaeaClient->GmMoveToGate( GateID );
    */
    return 0;
}

int ClientScriptManager::moveToPos(lua_State* l)
{
    /*
    ClientLuaScript* s = GetScriptObject(l);
    WORD  MapMid = (WORD)  luaL_checknumber(l, 1);
    WORD  MapSid = (WORD)  luaL_checknumber(l, 2);
    DWORD PosX   = (DWORD) luaL_checknumber(l, 3);
    DWORD PosY   = (DWORD) luaL_checknumber(l, 4);
    m_pGaeaClient->GmMoveToPos( MapMid, MapSid, PosX, PosY );
    */
    return 0;
}

int ClientScriptManager::visibleOn(lua_State* l)
{
    //m_pGaeaClient->ReqVisibleOn();
    return 0;
}

int ClientScriptManager::visibleOff(lua_State* l)
{
    //m_pGaeaClient->ReqVisibleOff();
    return 0;
}

int ClientScriptManager::visibleNone(lua_State* l)
{
    //m_pGaeaClient->ReqVisibleNone();
    return 0;
}

int ClientScriptManager::enchant(lua_State* l)
{
    /*
    ClientLuaScript* s = GetScriptObject(l);
    float Rate     = (float)  luaL_checknumber(l, 1);
    float Discount = (float)  luaL_checknumber(l, 2);
    m_pGaeaClient->GmEnchantChange( Rate, Discount );
    */
    return 0;
}

int ClientScriptManager::enchantRestore(lua_State* l)
{
    //m_pGaeaClient->GmEnchantRestore();
    return 0;
}

