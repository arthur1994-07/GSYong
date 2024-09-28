#include "pch.h"
#include "FriendGroup.h"
#include "Friend.h"

#include "../../SigmaCore/DebugInclude.h"

namespace friendlogic
{

Friend::Friend()
{
    m_pGroup = new FriendGroup;
}

Friend::~Friend()
{
    delete m_pGroup;
}

Friend& Friend::operator = (const Friend& rhs)
{
    if (this == &rhs)
        return *this;
    
    m_mapFriend = rhs.m_mapFriend;
    m_FriendId = rhs.m_FriendId;

    delete m_pGroup;
    m_pGroup = new FriendGroup;
    *m_pGroup = *rhs.m_pGroup;

    return *this;
}

void Friend::Clear()
{
    m_mapFriend.clear();
    m_FriendId.clear();
	if ( m_pGroup )
		m_pGroup->Clear();
}

std::tr1::shared_ptr<SFRIEND> Friend::GetData(const char* szName)
{
    if (!szName)
        return std::tr1::shared_ptr<SFRIEND> ((SFRIEND*) 0);

    FRIENDMAP_ITER iter = m_mapFriend.find(szName);
    if (iter != m_mapFriend.end())
        return iter->second;
    else
        return std::tr1::shared_ptr<SFRIEND> ((SFRIEND*) 0);
}

std::tr1::shared_ptr<SFRIEND> Friend::GetData(DWORD FriendDbNum)
{
    FRIEND_ID_ITER iter = m_FriendId.find(FriendDbNum);
    if (iter != m_FriendId.end())
        return iter->second;
    else
        return std::tr1::shared_ptr<SFRIEND> ((SFRIEND*) 0);
}

size_t Friend::Size(EMFRIEND_FLAGS emFLAGS)
{
    size_t nCnt(0);

    switch (emFLAGS)
    {
    case EMFRIEND_BLOCK:
        {
            FRIENDMAP_ITER iter = m_mapFriend.begin();
            for ( ; iter != m_mapFriend.end(); ++iter)
            {
                std::tr1::shared_ptr<SFRIEND> spFriend = iter->second;
                if (spFriend->IsBLOCK())
                    ++nCnt;
            }
        }
        break;

    case EMFRIEND_VALID:
        {
            FRIENDMAP_ITER iter = m_mapFriend.begin();
            for ( ; iter != m_mapFriend.end(); ++iter)
            {
                std::tr1::shared_ptr<SFRIEND> spFriend = iter->second;
                if (spFriend->IsVALID())
                    ++nCnt;
            }
        }
        break;

    case EMFRIEND_OFF:
        {
            FRIENDMAP_ITER iter = m_mapFriend.begin();
            for ( ; iter != m_mapFriend.end(); ++iter)
            {
                std::tr1::shared_ptr<SFRIEND> spFriend = iter->second;
                if (spFriend->IsOFF())
                    ++nCnt;
            }
        }
        break;

    case EMFRIEND_BETRAY:
        {
            FRIENDMAP_ITER iter = m_mapFriend.begin();
            for ( ; iter != m_mapFriend.end(); ++iter)
            {
                std::tr1::shared_ptr<SFRIEND> spFriend = iter->second;
                if (spFriend->IsBETRAY())
                    ++nCnt;
            }
        }
        break;

    default:
        GASSERT(0 && "Friend::Size() : EMFRIEND_FLAGS Error");
        break;
    }
    return nCnt;
}

bool Friend::Del(const char* szName)
{
    if (!szName)
        return false;

    FRIENDMAP_ITER iter = m_mapFriend.find(szName);
    if (iter != m_mapFriend.end())
    {
        std::tr1::shared_ptr<SFRIEND> spFriend = iter->second;
        m_mapFriend.erase(iter);
        m_FriendId.erase(spFriend->GetChaDbNum());
        return true;
    }
    else
    {
        return false;
    }
}

bool Friend::IsExist(const std::string& ChaName) const
{
    FRIENDMAP_CITER iter = m_mapFriend.find(ChaName);
    if (iter != m_mapFriend.end())
        return true;
    else
        return false;
}

bool Friend::IsExist(DWORD FriendDbNum) const
{
    FRIEND_ID_CITER iter = m_FriendId.find(FriendDbNum);
    if (iter != m_FriendId.end())
        return true;
    else
        return false;
}

bool Friend::Add(const SFRIEND& Info)
{
    FRIENDMAP_CITER iter = m_mapFriend.find(Info.m_ChaName);
    if (iter != m_mapFriend.end())
        return false;
    
    std::tr1::shared_ptr<SFRIEND> spFriend(new SFRIEND(Info));
    m_mapFriend.insert(FRIENDMAP_VALUE(Info.m_ChaName, spFriend));
    m_FriendId.insert(FRIEND_ID_VALUE(Info.GetChaDbNum(), spFriend));
    return true;
}

bool Friend::Add(const CHA_FRIEND& Info)
{
    FRIENDMAP_CITER iter = m_mapFriend.find(Info.m_ChaName);
    if (iter != m_mapFriend.end())
        return false;

    std::tr1::shared_ptr<SFRIEND> spFriend(new SFRIEND(Info.m_Group));
    spFriend->m_ChaDbNum = Info.nChaS;
    spFriend->m_Flag = Info.nChaFlag;
    spFriend->SetFriendName(Info.m_ChaName);
    spFriend->m_ChaClass = Info.m_ChaClass;
    spFriend->m_ChaLevel = Info.m_ChaLevel;
    spFriend->m_ChaMap   = Info.m_ChaMap;

    m_mapFriend.insert(FRIENDMAP_VALUE(spFriend->m_ChaName, spFriend));
    m_FriendId.insert(FRIEND_ID_VALUE(Info.nChaS, spFriend));

    return true;
}

bool Friend::Add(std::tr1::shared_ptr<SFRIEND> spFriend)
{
    FRIEND_ID_ITER iter = m_FriendId.find(spFriend->m_ChaDbNum);
    if (iter != m_FriendId.end())
        return false;

    m_mapFriend.insert(FRIENDMAP_VALUE(spFriend->m_ChaName, spFriend));
    m_FriendId.insert(FRIEND_ID_VALUE(spFriend->m_ChaDbNum, spFriend));
    return true;
}

int Friend::GetFriendListCount()
{
    const friendlogic::FRIENDMAP& friendMap = GetList();

    unsigned int nFriendCound = 0;
    BOOST_FOREACH( const friendlogic::FRIENDMAP_VALUE& value, friendMap )
    {
        const std::tr1::shared_ptr<SFRIEND> spFriend  = value.second;

        if( spFriend && spFriend->IsBLOCK() == false )
            nFriendCound++;
    }

    return nFriendCound;
}

int Friend::GetBlockListCount()
{
    const friendlogic::FRIENDMAP& friendMap = GetList();

    unsigned int nFriendCound = 0;
    BOOST_FOREACH( const friendlogic::FRIENDMAP_VALUE& value, friendMap )
    {
        const std::tr1::shared_ptr<SFRIEND> spFriend  = value.second;

        if( spFriend && spFriend->IsBLOCK() )
            nFriendCound++;
    }

    return nFriendCound;
}

EMFRIEND_FLAGS Friend::GetState(const char* szFriend)
{
    // 상대 찾음
    if (!szFriend)
        return EMFRIEND_OFF;

    std::tr1::shared_ptr<SFRIEND> spFriend = GetData(szFriend);
    if (spFriend)
        return static_cast<EMFRIEND_FLAGS> (spFriend->m_Flag);
    else
        return EMFRIEND_OFF;
}

EMFRIEND_FLAGS Friend::GetState(DWORD FriendDbNum)
{
    std::tr1::shared_ptr<SFRIEND> spFriend = GetData(FriendDbNum);
    if (spFriend)
        return static_cast<EMFRIEND_FLAGS> (spFriend->m_Flag);
    else
        return EMFRIEND_OFF;
}

bool Friend::GroupAdd(const FRIEND_GROUP& Info)
{
    return m_pGroup->Add(Info);
}

bool Friend::GroupDel(WORD GroupNum)
{
    return m_pGroup->Del(GroupNum);
}

bool Friend::GroupRename(WORD GroupNum, const std::string& NewName)
{
    return m_pGroup->Rename(GroupNum, NewName);
}

bool Friend::GroupIsExist(WORD GroupNum) const
{
    return m_pGroup->IsExist(GroupNum);
}

bool Friend::GroupIsExist(const std::string& GroupName) const
{
    return m_pGroup->IsExist(GroupName);
}

WORD Friend::GroupGetFreeNum() const
{
    return m_pGroup->GetFreeNum();
}

std::string Friend::GroupGetName(WORD GroupNum) const
{
    return m_pGroup->GetName(GroupNum);
}

void Friend::GetGroup(std::vector<FRIEND_GROUP>& GroupInfo) const
{
    return m_pGroup->GetData(GroupInfo);
}

bool Friend::GroupSetFriend(DWORD FriendDbNum, WORD GroupNum, WORD& wBeforeGroupNum)
{
    if (GroupNum != 0 && !m_pGroup->IsExist(GroupNum))
        return false;
    
    FRIEND_ID_ITER iter = m_FriendId.find(FriendDbNum);
    if (iter == m_FriendId.end())
        return false;

    std::tr1::shared_ptr<SFRIEND> spFriend = iter->second;
    if (!spFriend)
        return false;

	wBeforeGroupNum = spFriend->GetGroup();
    spFriend->SetGroup(GroupNum);
    return true;
}

void Friend::SetFriendLevel(DWORD FriendDbNum, WORD FriendLevel)
{
    FRIEND_ID_ITER iter = m_FriendId.find(FriendDbNum);
    if (iter == m_FriendId.end())
        return;
    std::tr1::shared_ptr<SFRIEND> spFriend = iter->second;
    if (spFriend)
        spFriend->m_ChaLevel = FriendLevel;
}

void Friend::SetFriendMap(DWORD FriendDbNum, const MapID& FriendMap)
{
    FRIEND_ID_ITER iter = m_FriendId.find(FriendDbNum);
    if (iter == m_FriendId.end())
        return;
    std::tr1::shared_ptr<SFRIEND> spFriend = iter->second;
    if (spFriend)
        spFriend->m_ChaMap = FriendMap;
}

void Friend::SetFriendMapPosition(DWORD FriendDbNum, int PosX, int PosY)
{
    FRIEND_ID_ITER iter = m_FriendId.find(FriendDbNum);
    if (iter == m_FriendId.end())
        return;
    std::tr1::shared_ptr<SFRIEND> spFriend = iter->second;
    if (spFriend)
    {
        spFriend->m_PosX = PosX;
        spFriend->m_PosY = PosY;
    }
}

void Friend::SetFriendState(DWORD FriendDbNum, BYTE Channel, const MapID& CurrentMap, int PosX, int PosY)
{
    FRIEND_ID_ITER iter = m_FriendId.find(FriendDbNum);
    if (iter == m_FriendId.end())
        return;
    std::tr1::shared_ptr<SFRIEND> spFriend = iter->second;
    if (!spFriend)
        return;
    spFriend->m_Online  = true;
    spFriend->m_Channel = Channel;
    spFriend->m_ChaMap  = CurrentMap;
    
}

void Friend::SetFriendOffline(DWORD FriendDbNum)
{
    FRIEND_ID_ITER iter = m_FriendId.find(FriendDbNum);
    if (iter == m_FriendId.end())
        return;
    std::tr1::shared_ptr<SFRIEND> spFriend = iter->second;
    if (!spFriend)
        return;
    spFriend->m_Online  = false;
}

void Friend::SetFriendParty(DWORD ChaDbNum, const GLPartyID& PartyID)
{
    FRIEND_ID_ITER iter = m_FriendId.find(ChaDbNum);
    if (iter == m_FriendId.end())
        return;
    std::tr1::shared_ptr<SFRIEND> spFriend = iter->second;
    if ( spFriend )
        spFriend->m_PartyID = PartyID;
}

} // namespace friendlogic