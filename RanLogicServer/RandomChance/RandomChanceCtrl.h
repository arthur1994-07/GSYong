#ifndef _LOGIC_SERVER_RANDOMCHANCE_CTRL_H_
#define _LOGIC_SERVER_RANDOMCHANCE_CTRL_H_

#pragma once

#include <set>

struct SNATIVEID;

//! 확률/아이템 리스트 제어 기능 : sckim, Redmine #4544
namespace randomchance
{

//! 확률/아이템 리스트 제어 기능 : RandomChanceCtrlSys.lua
class RandomChanceCtrl
{
    typedef std::set<DWORD>                        RANDOMCHANCE_ITEM_DATA;
    typedef RANDOMCHANCE_ITEM_DATA::iterator       RANDOMCHANCE_ITEM_DATA_ITER;
    typedef RANDOMCHANCE_ITEM_DATA::const_iterator RANDOMCHANCE_ITEM_DATA_CITER;

public:
    RandomChanceCtrl();
    ~RandomChanceCtrl();

protected:
    bool m_bActive;

    //! 시스템 메세지 출력용 확률(해당 확률 이하의 값이 시스템 메시지 출럭)
    float m_fGlobalRC;

    //! 시스템 메시지 출력용 ItemList
    RANDOMCHANCE_ITEM_DATA m_RCData;

public:
    //! lua file loading
    bool LoadFile(const std::string& strName);

    //! 시스템 메시지 출력용 아이템 셋팅
    void Reset();

    void AddItem(WORD ItemMid, WORD ItemSid);
    void AddItem(DWORD ItemID);
    void AddItem(const SNATIVEID& ItemID);
    
    float GetGlobalRC() const;

    bool IsGlobalRC(float fRate) const;
    bool IsRCItem(WORD ItemMid, WORD ItemSid) const;
    bool IsRCItem(DWORD ItemID) const;
    bool IsRCItem(const SNATIVEID& ItemID) const;
};

} // namespace Randomchance

#endif // _LOGIC_SERVER_RANDOMCHANCE_CTRL_H_