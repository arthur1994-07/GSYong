#include "pch.h"
#include "../../RanLogic/Msg/GLContrlBaseMsg.h"
#include "./CacheUserLobby.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
CacheUserLobby::CacheUserLobby(
    int UserDbNum,
    DWORD CacheSlot,
    DWORD AgentSlot,
    const char* szIp,
    USHORT Port)
    : m_UserDbNum(UserDbNum)
    , m_CacheSlot(CacheSlot)
    , m_AgentClientSlot(AgentSlot)
    , m_IpAddress(szIp)
    , m_Port(Port)
{
}

CacheUserLobby::~CacheUserLobby()
{
}

void CacheUserLobby::AddChar(const SCHARINFO_LOBBY& CharData)
{
    m_vCharInfo.push_back(CharData);
}

int CacheUserLobby::GetCharDbNum(size_t Index)
{
    if (Index < 0 || Index >= m_vCharInfo.size())
        return 0;
    else
        return m_vCharInfo[Index].ChaDbNum();
}

bool CacheUserLobby::GetChar(int ChaDbNum, SCHARINFO_LOBBY& ChaData)
{
    for (size_t i=0; i<m_vCharInfo.size(); ++i)
    {
        if (m_vCharInfo[i].ChaDbNum() == ChaDbNum)
        {
            ChaData = m_vCharInfo[i];
            return true;
        }
    }
    return false;
}

// ----------------------------------------------------------------------------
CacheUserLobbyMan::CacheUserLobbyMan()
{
}

CacheUserLobbyMan::~CacheUserLobbyMan()
{
}

bool CacheUserLobbyMan::RemoveUser(int UserDbNum)
{
    CACHE_LOBBY_USER_ITER iter = m_UserData.find(UserDbNum);
    if (iter != m_UserData.end())
    {
        m_UserData.erase(iter);
        return true;
    }
    else
    {
        return false;
    }
}

void CacheUserLobbyMan::AddUser(int UserDbNum, DWORD CacheSlot, DWORD AgentSlot, const char* szIp, USHORT Port)
{
    spChacheUserLobby spUser(new CacheUserLobby(UserDbNum, CacheSlot, AgentSlot, szIp, Port));
    m_UserData.insert(CACHE_LOBBY_USER_VALUE(UserDbNum, spUser));
}

bool CacheUserLobbyMan::AddChar(int UserDbNum, const SCHARINFO_LOBBY& CharData)
{
    CACHE_LOBBY_USER_ITER iter = m_UserData.find(UserDbNum);
    if (iter == m_UserData.end())
        return false;

    spChacheUserLobby spUser = iter->second;
    spUser->AddChar(CharData);
    return true;
}

const spChacheUserLobby CacheUserLobbyMan::GetUser(int UserDbNum)
{
    CACHE_LOBBY_USER_CITER iter = m_UserData.find(UserDbNum);
    if (iter != m_UserData.end())
    {
        return iter->second;
    }
    else
    {
        static spChacheUserLobby NullLobbyUser;
        return NullLobbyUser;
    }
}