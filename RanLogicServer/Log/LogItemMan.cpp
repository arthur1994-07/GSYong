#include "pch.h"
#include "../../EngineLib/G-Logic/TypeDefine.h"
#include "./LogItemMan.h"

#include "../../SigmaCore/DebugInclude.h"

namespace gamelog
{

LogItemMan::LogItemMan()
{   
}

LogItemMan::~LogItemMan()
{
}

void LogItemMan::Reset()
{
    m_Data.clear();
}

void LogItemMan::AddItem(WORD ItemMid, WORD ItemSid)
{
    SNATIVEID ItemID(ItemMid, ItemSid);
    AddItem(ItemID);    
}

void LogItemMan::AddItem(const SNATIVEID& ItemID)
{
    AddItem(ItemID.Id());
}

void LogItemMan::AddItem(DWORD ItemID)
{
    m_Data.insert(ItemID);
}

bool LogItemMan::IsLogItem(WORD ItemMid, WORD ItemSid) const
{
    SNATIVEID ItemID(ItemMid, ItemSid);
    return IsLogItem(ItemID);
}

bool LogItemMan::IsLogItem(const SNATIVEID& ItemID) const
{
    return IsLogItem(ItemID.Id());
}

bool LogItemMan::IsLogItem(DWORD ItemID) const
{
    LOG_ITEM_DATA_CITER iter = m_Data.find(ItemID);
    if (iter != m_Data.end())
        return true;
    else
        return false;
}

} // namespace gamelog

