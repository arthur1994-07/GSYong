#include "pch.h"
#include "./FriendGroup.h"

#include "../../SigmaCore/DebugInclude.h"

namespace friendlogic
{

FriendGroup::FriendGroup()
{
}

FriendGroup::~FriendGroup()
{
}

void FriendGroup::Clear(void)
{
	m_Data.clear();
}

bool FriendGroup::Add(const FRIEND_GROUP& Info)
{
    if (IsExist(Info.m_GroupNum))
        return false;
    if (IsExist(Info.m_GroupName))
        return false;
    m_Data.insert(FG_DATA_VALUE(Info.m_GroupNum, Info));
    return true;
}

bool FriendGroup::Del(WORD GroupNum)
{
    FG_DATA_ITER iter = m_Data.find(GroupNum);
    if (iter != m_Data.end())
    {
        m_Data.erase(iter);
        return true;
    }
    else
    {
        return false;   
    }
}

bool FriendGroup::Rename(WORD GroupNum, const std::string& NewName)
{
    FG_DATA_ITER iter = m_Data.find(GroupNum);
    if (iter == m_Data.end())
        return false;
    FRIEND_GROUP& Group = iter->second;
    Group.SetName(NewName);
    return true;
}

bool FriendGroup::IsExist(WORD GroupNum) const
{
    FG_DATA_CITER iter = m_Data.find(GroupNum);
    if (iter != m_Data.end())
        return true;
    else
        return false;
}

bool FriendGroup::IsExist(const std::string& GroupName) const
{
    for (FG_DATA_CITER iter=m_Data.begin(); iter!=m_Data.end(); ++iter)
    {
        const FRIEND_GROUP& Group = iter->second;
        if (GroupName.compare(Group.m_GroupName) == 0)
            return true;
    }
    return false;
}

WORD FriendGroup::GetFreeNum() const
{
    for (WORD i=1; i<INVALID_FRIEND_GROUP_NUMBER; ++i)
    {
        if (!IsExist(i))
            return i;
    }
    return INVALID_FRIEND_GROUP_NUMBER;
}

std::string FriendGroup::GetName(WORD GroupNum) const
{
    FG_DATA_CITER iter = m_Data.find(GroupNum);
    if (iter != m_Data.end())
    {
        const FRIEND_GROUP& Group = iter->second;
        return std::string(Group.m_GroupName);
    }
    else
    {
        return std::string("");
    }
}

void FriendGroup::GetData(std::vector<FRIEND_GROUP>& GroupInfo) const
{
    for (FG_DATA_CITER iter=m_Data.begin(); iter!=m_Data.end(); ++iter)
    {
        GroupInfo.push_back(iter->second);
    }
}

} // namespace friendlogic