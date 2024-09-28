#ifndef _LOGIC_SERVER_LOG_ITEM_MAN_H_
#define _LOGIC_SERVER_LOG_ITEM_MAN_H_

#pragma once

#include <set>

struct SNATIVEID;

namespace gamelog
{

//! 로그를 남길 아이템을 관리한다.
class LogItemMan
{
    typedef std::set<DWORD>               LOG_ITEM_DATA;
    typedef LOG_ITEM_DATA::iterator       LOG_ITEM_DATA_ITER;
    typedef LOG_ITEM_DATA::const_iterator LOG_ITEM_DATA_CITER;

public:
    LogItemMan();
    ~LogItemMan();

protected:
    LOG_ITEM_DATA m_Data;

public:
    void Reset();

    void AddItem(WORD ItemMid, WORD ItemSid);
    void AddItem(DWORD ItemID);
    void AddItem(const SNATIVEID& ItemID);
    
    bool IsLogItem(WORD ItemMid, WORD ItemSid) const;
    bool IsLogItem(DWORD ItemID) const;
    bool IsLogItem(const SNATIVEID& ItemID) const;
};

} // namespace gamelog

#endif // _LOGIC_SERVER_LOG_ITEM_MAN_H_