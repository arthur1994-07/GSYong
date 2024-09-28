#ifndef _GLOGIC_FRIEND_GROUP_H_
#define _GLOGIC_FRIEND_GROUP_H_

#pragma  once

#include <string>
#include <map>
#include "FriendDefine.h"

namespace friendlogic
{

//! 模备 弊缝 包府
class FriendGroup
{
public:
    FriendGroup();
    virtual ~FriendGroup();
    
    typedef std::map<WORD, FRIEND_GROUP> FG_DATA;
    typedef FG_DATA::iterator            FG_DATA_ITER;
    typedef FG_DATA::const_iterator      FG_DATA_CITER;
    typedef FG_DATA::value_type          FG_DATA_VALUE;

protected:
    FG_DATA m_Data;    

public:
	void Clear(void);
    bool Add(const FRIEND_GROUP& Info);
    bool Del(WORD GroupNum);
    bool Rename(WORD GroupNum, const std::string& NewName);
    bool IsExist(WORD GroupNum) const;
    bool IsExist(const std::string& GroupName) const;
    WORD GetFreeNum() const;
    std::string GetName(WORD GroupNum) const;
    void GetData(std::vector<FRIEND_GROUP>& GroupInfo) const;
};

} // namespace friendlogic
#endif