#ifndef _CACHE_USER_LOBBY_H_
#define _CACHE_USER_LOBBY_H_

#pragma once

#include <boost/tr1/memory.hpp>
#include <boost/tr1/unordered_map.hpp>

struct SCHARINFO_LOBBY;

//! Lobby User/Character
class CacheUserLobby
{
public:
    CacheUserLobby(
        int UserDbNum,
        DWORD CacheSlot,
        DWORD AgentSlot,
        const char* szIp,
        USHORT Port);
    ~CacheUserLobby();

private:
    int m_UserDbNum;
    DWORD m_CacheSlot;
    DWORD m_AgentClientSlot;
    std::string m_IpAddress;
    USHORT m_Port;
    
    std::vector<SCHARINFO_LOBBY> m_vCharInfo; //! 积己茄 某腐磐

public:
    void AddChar(const SCHARINFO_LOBBY& CharData);
    int GetCharDbNum(size_t Index);
    bool GetChar(int ChaDbNum, SCHARINFO_LOBBY& ChaData);

    inline int UserDbNum() const { return m_UserDbNum; }
    inline DWORD CacheSlot() const { return m_CacheSlot; }
    inline DWORD AgentClientSlot() const { return m_AgentClientSlot; }
    inline std::string IpAddress() const { return m_IpAddress; }
    inline USHORT Port() const { return m_Port; }
    inline size_t TotalChaNum() const { return m_vCharInfo.size(); }
};
typedef std::tr1::shared_ptr<CacheUserLobby> spChacheUserLobby;

//! Lobbby User/Character 包府
class CacheUserLobbyMan
{
public:
    typedef std::tr1::unordered_map<int, spChacheUserLobby> CACHE_LOBBY_USER;
    typedef CACHE_LOBBY_USER::iterator                      CACHE_LOBBY_USER_ITER;
    typedef CACHE_LOBBY_USER::const_iterator                CACHE_LOBBY_USER_CITER;
    typedef CACHE_LOBBY_USER::value_type                    CACHE_LOBBY_USER_VALUE;    

public:
    CacheUserLobbyMan();
    ~CacheUserLobbyMan();

private:
    CACHE_LOBBY_USER m_UserData;

public:
    bool RemoveUser(int UserDbNum);
    void AddUser(int UserDbNum, DWORD CacheSlot, DWORD AgentSlot, const char* szIp, USHORT Port);
    bool AddChar(int UserDbNum, const SCHARINFO_LOBBY& CharData);
    const spChacheUserLobby GetUser(int UserDbNum);
};

#endif // _CACHE_USER_LOBBY_H_