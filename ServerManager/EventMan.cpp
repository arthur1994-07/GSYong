#include "StdAfx.h"

#include "../SigmaCore/String/StringFormat.h"
#include "../SigmaCore/String/StringUtil.h"
#include "../SigmaCore/Util/SystemInfo.h"
#include "../SigmaCore/Lua/MinLua.h"
#include "../SigmaCore/Lua/lua_tinker.h"
#include "../SigmaCore/Log/LogMan.h"
#include "ServerManagerDlg.h"
#include "EventMan.h"

#include "../SigmaCore/DebugInclude.h"

EventMan::EventMan(CServerManagerDlg* pDlg, DWORD WaitTime)
    : sc::EventThread(WaitTime)
    , m_bLoad(false)
    , m_LastSec(0)
    , m_pDlg(pDlg)
{
    stop();
    bindLuaCmd();
}

EventMan::~EventMan(void)
{
    end();
    sc::lua_destroyState();
}

//자식에서 명시적인 구현이 필요하다.
void EventMan::start()
{
    startThread();
}

//자식에서 명시적인 구현이 필요하다.
void EventMan::end()
{
    endThread();
}

void EventMan::endHook()
{

}

bool EventMan::load()
{
    sc::lua_destroyState();
    lua_State* pLua = sc::lua_getState()->GetCState();
    if (!pLua)
    {
        WRITE_LOG_ERROR("Lua init error");
        return false;
    }

    bindLuaCmd();

    std::string FullPath(sc::getAppPath());
    FullPath += "\\Event.lua";

    if (false == sc::lua_doFile(FullPath.c_str(), NULL))
    {
        WRITE_LOG_INFO("Event.lua load failed. check file or systax error! ex>luaplusc -o event.luc event.lua");
        return false;
    }
    else
    {
        m_bLoad = true;
        startThread();
        WRITE_LOG_INFO("Event.lua load success.");
        return true;
    }
}

void EventMan::bindLuaCmd()
{
    lua_State* L = sc::lua_getState()->GetCState();

    //////////////////////////////////////////////////////////////////////////////
    // AutoLevelMan 클래스를 Lua 에 추가한다.
    lua_tinker::class_add<EventMan>(L, "Event");

    // AutoLevel 의 함수를 등록한다.
    lua_tinker::class_def<EventMan>(L, "msg",            &EventMan::cmdMsg);
    lua_tinker::class_def<EventMan>(L, "expup",          &EventMan::cmdExpUp);
    lua_tinker::class_def<EventMan>(L, "expupend",       &EventMan::cmdExpUpEnd);
    lua_tinker::class_def<EventMan>(L, "itemup",         &EventMan::cmdItemUp);
    lua_tinker::class_def<EventMan>(L, "moneyup",        &EventMan::cmdMoneyUp);
    lua_tinker::class_def<EventMan>(L, "enchant",        &EventMan::cmdEnchantUp);
    lua_tinker::class_def<EventMan>(L, "enchantRestore", &EventMan::cmdEnchantRestore);
    lua_tinker::class_def<EventMan>(L, "levNew",         &EventMan::cmdLevelNew);
    lua_tinker::class_def<EventMan>(L, "levDel",         &EventMan::cmdLevelDel);
    lua_tinker::class_def<EventMan>(L, "mobGenAll",      &EventMan::cmdMobGenAll);
    lua_tinker::class_def<EventMan>(L, "mobDel",         &EventMan::cmdMobDel);
    lua_tinker::class_def<EventMan>(L, "pkMode",         &EventMan::cmdPkMode);

    // Lua 전역 변수로 AutoLevel 의 포인터를 등록한다.
    lua_tinker::set(L, "g_pEvent", this);
}

void EventMan::process()
{
    CTime CurTime(CTime::GetCurrentTime());
    int WeekDay = CurTime.GetDayOfWeek();
    int Year    = CurTime.GetYear();
    int Month   = CurTime.GetMonth();
    int Day     = CurTime.GetDay();
    int Hour    = CurTime.GetHour();
    int Min     = CurTime.GetMinute();
    int Sec     = CurTime.GetSecond();

    if ((m_LastSec != Sec) && (Sec % 10) == 0)
    {
        m_LastSec = Sec;
        WRITE_LOG_DEBUG(
            sc::string::format(
                "Event run %1% %2% %3% %4% %5% %6% %7%",
                WeekDay, Year, Month, Day, Hour, Min, Sec));
        FrameMoveLua(WeekDay, Year, Month, Day, Hour, Min, Sec);
    }
}

bool EventMan::FrameMoveLua( int WeekDay, int Year, int Month, int Day, int Hour, int Min, int Sec )
{
    if (m_bLoad)
    {
        lua_State* L = sc::lua_getState()->GetCState();

        // AutoLevel.luc 의 AutoLevelMain 함수를 실행한다.
        lua_tinker::call<int,int,int,int,int,int,int>(
            L,
            "EventMain",
            WeekDay,
            Year,
            Month,
            Day,
            Hour,
            Min,
            Sec );
        return true;
    }
    else
    {
        return false;
    }
}

//! 전체 메시지 전송
void EventMan::cmdMsg(int ServerGroup, const char* szNoticeUtf8)
{
    if (NULL == szNoticeUtf8)
    {
        WRITE_LOG_ERROR("Msg format was wrong. g_pEvent:msg(ServerGroup, Message)");
    }
    else
    {
        std::string NoticeMsg(szNoticeUtf8);
        std::wstring NoticeMsgW = sc::string::utf8ToUnicode(NoticeMsg);        
        std::string Notice = sc::string::unicodeToAnsi(NoticeMsgW);
        m_pDlg->Notice(ServerGroup, Notice.c_str());
    }
}

void EventMan::cmdExpUp(int ServerGroup, float Scale)
{
    if (Scale < 1.0f || Scale > 4.0f)
    {
        WRITE_LOG_ERROR("expup Scale error. scale must be 1 to 4.");
        return;
    }

    m_pDlg->ExpUp(ServerGroup, Scale);
}

void EventMan::cmdExpUpEnd(int ServerGroup)
{    
    m_pDlg->ExpUpEnd(ServerGroup);
}

void EventMan::cmdItemUp(int ServerGroup, float Scale)
{
    if (Scale < 0.0f || Scale > 10.0f)
    {
        WRITE_LOG_ERROR("itempup Scale error. scale must be 0 to 10");
        return;
    }

    //m_pDlg->ItemUp( ServerGroup, Scale );
}

void EventMan::cmdMoneyUp(int ServerGroup, float Scale)
{
    if (Scale < 0.0f || Scale > 10.0f)
    {
        WRITE_LOG_ERROR("moneyup Scale error. scale must be 0 to 10");
        return;
    }

    //m_pDlg->MoneyUp( ServerGroup, Scale );
}

void EventMan::cmdEnchantUp(int ServerGroup, float Rate, float Discount)
{
    if (Rate < 1.0f || Rate > 90.0f)
    {
        WRITE_LOG_ERROR("Rate must 1 to 90");
        return;
    }

    if (Discount < 0.09f || Discount > 0.99f)
    {
        WRITE_LOG_ERROR("Rate Discount must 0.09 to 0.99");
        return;
    }
    
    //m_pDlg->EnchantUp( ServerGroup, Rate, Discount );
}

void EventMan::cmdEnchantRestore( int ServerGroup )
{
    //m_pDlg->EnchantRestore(ServerGroup);
}

void EventMan::cmdLevelNew(int ServerGroup, int MapMid, int MapSid, const char* szLevelFile)
{
    if (!szLevelFile)
    {
        WRITE_LOG_ERROR("levelfile name null");
    }
    else
    {
        //std::string LevelFile( szLevelFile );
        //std::wstring LevelFileW = CORE_COMMON::StringUtil::utf8ToUnicode( LevelFile );
        //CORE_COMMON::StringUtil::trim( LevelFileW );
        //m_pDlg->LevelNew( ServerGroup, MapMid, MapSid, LevelFileW );
    }
}

void EventMan::cmdLevelDel( int ServerGroup, int MapMid, int MapSid )
{
    //m_pDlg->LevelDel( ServerGroup, MapMid, MapSid );
}

void EventMan::cmdMobGenAll(
    int ServerGroup,
    int MapMid,
    int MapSid,
    int CrowMid,
    int CrowSid,
    int PosX,
    int PosY)
{
    //m_pDlg->MobGenAll( ServerGroup, MapMid, MapSid, CrowMid, CrowSid, PosX, PosY );
}

void EventMan::cmdMobDel(int ServerGroup)
{
    //m_pDlg->MobDel( ServerGroup );
}

void EventMan::cmdPkMode(int ServerGroup, int OnOff)
{
    //m_pDlg->PkMode( ServerGroup, OnOff );
}