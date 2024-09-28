#ifndef _RAN_LOGIC_FRIEND_H_
#define _RAN_LOGIC_FRIEND_H_

#pragma once

#include <boost/tr1/unordered_map.hpp>
#include <map>
#include <string>
#include "../MSG/GLContrlMsg.h"
#include "./FriendDefine.h"

namespace friendlogic
{

typedef std::map<std::string, std::tr1::shared_ptr<SFRIEND> > FRIENDMAP;
typedef FRIENDMAP::iterator            FRIENDMAP_ITER;
typedef FRIENDMAP::const_iterator      FRIENDMAP_CITER;
typedef FRIENDMAP::value_type          FRIENDMAP_VALUE;

typedef std::map<DWORD, std::tr1::shared_ptr<SFRIEND> > FRIEND_ID;
typedef FRIEND_ID::iterator            FRIEND_ID_ITER;
typedef FRIEND_ID::const_iterator      FRIEND_ID_CITER;
typedef FRIEND_ID::value_type          FRIEND_ID_VALUE;

class FriendGroup;

class Friend
{
public:
    Friend();
    virtual ~Friend();
    Friend& operator = (const Friend& rhs);

protected:
    FRIENDMAP m_mapFriend; //! 이름/pointer
    FRIEND_ID m_FriendId; //! DB번호/pointer
    FriendGroup* m_pGroup;

public:
    virtual void Clear();
    bool Del(const char* szName);
    bool Add(const SFRIEND& Info);
    bool Add(const CHA_FRIEND& Info);
    bool Add(std::tr1::shared_ptr<SFRIEND> spFriend);

    const FRIENDMAP& GetList() { return m_mapFriend; }
    int GetFriendListCount();
    int GetBlockListCount();
    
    std::tr1::shared_ptr<SFRIEND> GetData(const char* szName);
    std::tr1::shared_ptr<SFRIEND> GetData(DWORD FriendDbNum);

    void SetFriendLevel(DWORD FriendDbNum, WORD FriendLevel);
    void SetFriendMap(DWORD FriendDbNum, const MapID& FriendMap);
    void SetFriendMapPosition(DWORD FriendDbNum, int PosX, int PosY);
    void SetFriendState(DWORD FriendDbNum, BYTE Channel, const MapID& CurrentMap, int PosX, int PosY);
    void SetFriendOffline(DWORD FriendDbNum);
    void SetFriendParty(DWORD ChaDbNum, const GLPartyID& PartyID);

    size_t Size(EMFRIEND_FLAGS emFLAGS);
    size_t Size() const { return m_mapFriend.size(); }
    bool IsExist(const std::string& ChaName) const;
    bool IsExist(DWORD FriendDbNum) const;
    EMFRIEND_FLAGS GetState(const char* szFriend);
    EMFRIEND_FLAGS GetState(DWORD FriendDbNum);

    //! Friend Group
    bool GroupAdd(const FRIEND_GROUP& Info);
    virtual bool GroupDel(WORD GroupNum);
    bool GroupRename(WORD GroupNum, const std::string& NewName);
    bool GroupIsExist(WORD GroupNum) const;
    bool GroupIsExist(const std::string& GroupName) const;
    WORD GroupGetFreeNum() const;
    std::string GroupGetName(WORD GroupNum) const;
    void GetGroup(std::vector<FRIEND_GROUP>& GroupInfo) const;  
    bool GroupSetFriend(DWORD FriendDbNum, WORD GroupNum, WORD& wBeforeGroupNum);
};

} // namespace friendlogic

#endif