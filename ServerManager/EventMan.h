#pragma once

#pragma once

#include "../SigmaCore/Thread/EventThread.h"
#include "ServerManagerDlg.h"

class EventMan : public sc::EventThread
{
public:
    EventMan(CServerManagerDlg* pDlg, DWORD WaitTime);
    ~EventMan();

protected:
    bool m_bLoad; //! Lua 파일에 제대로 load 되었는가?
    int m_LastSec;
    CServerManagerDlg* m_pDlg;

protected:
    void bindLuaCmd();

    //! 전체 메시지 전송
    void cmdMsg(int ServerGroup, const char* szNoticeUtf8);
    void cmdExpUp(int ServerGroup, float Scale);
    void cmdExpUpEnd(int ServerGroup);
    void cmdItemUp(int ServerGroup, float Scale);
    void cmdMoneyUp(int ServerGroup, float Scale);
    void cmdEnchantUp(int ServerGroup, float Rate, float Discount);
    void cmdEnchantRestore(int ServerGroup);
    void cmdLevelNew(int ServerGroup, int MapMid, int MapSid, const char* szLevelFile);
    void cmdLevelDel(int ServerGroup, int MapMid, int MapSid);
    void cmdMobGenAll(int ServerGroup, int MapMid, int MapSid, int CrowMid, int CrowSid, int PosX, int PosY);
    void cmdMobDel(int ServerGroup);
    void cmdPkMode(int ServerGroup, int OnOff);
    bool FrameMoveLua(int WeekDay, int Year, int Month, int Day, int Hour, int Min, int Sec);

public:
    virtual void start() override;
    virtual void end() override;
    virtual void process() override;
    virtual void endHook() override;
    bool load();
};
