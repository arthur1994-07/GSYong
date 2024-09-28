#include "pch.h"
#include "../../SigmaCore/Util/Lock.h"
#include "./CharNameCache.h"

#include "../../SigmaCore/DebugInclude.h"

CharNameCache::CharNameCache()
{
    InitializeCriticalSectionAndSpinCount(&m_Lock, sc::CRITICAL_SECTION_SPIN_COUNT);
}

CharNameCache::~CharNameCache()
{
    DeleteCriticalSection(&m_Lock);
}

void CharNameCache::Add(const std::string& ChaName, DWORD ChaDbNum, DWORD UserDbNum, EMUSERTYPE UserType)
{
    sc::CriticalSectionOwnerLock OwnerLock(m_Lock);

    NAME_DBNUM_ITER iterA = m_NameDb.find(ChaName);
    if (iterA != m_NameDb.end())
        m_NameDb.erase(iterA);

    DBNUM_NAME_ITER iterB = m_DbName.find(ChaDbNum);
    if (iterB != m_DbName.end())
        m_DbName.erase(iterB);

    std::tr1::shared_ptr<CHAR_BASE_CACHE_DATA> spData(new CHAR_BASE_CACHE_DATA(ChaDbNum, UserDbNum, static_cast<BYTE> (UserType), ChaName));
    m_NameDb.insert(NAME_DBNUM_VALUE(ChaName, spData));
    m_DbName.insert(DBNUM_NAME_VALUE(ChaDbNum, spData));
}

void CharNameCache::Add(CHAR_BASE_CACHE_DATA& sData)
{
	sc::CriticalSectionOwnerLock OwnerLock(m_Lock);

	NAME_DBNUM_ITER iterA = m_NameDb.find(sData.ChaName());
	if (iterA != m_NameDb.end())
		m_NameDb.erase(iterA);

	DBNUM_NAME_ITER iterB = m_DbName.find(sData.ChaDbNum());
	if (iterB != m_DbName.end())
		m_DbName.erase(iterB);

	std::tr1::shared_ptr<CHAR_BASE_CACHE_DATA> spData(new CHAR_BASE_CACHE_DATA(sData.ChaDbNum(), sData.m_UserNum, static_cast<BYTE> (sData.UserType()), sData.ChaName(), sData.GetClass(), sData.GetLv()));
	m_NameDb.insert(NAME_DBNUM_VALUE(sData.ChaName(), spData));
	m_DbName.insert(DBNUM_NAME_VALUE(sData.ChaDbNum(), spData));
}

void CharNameCache::Del(const std::string& ChaName)
{
    sc::CriticalSectionOwnerLock OwnerLock(m_Lock);

    NAME_DBNUM_ITER iterName = m_NameDb.find(ChaName);
    if (iterName != m_NameDb.end())
    {
        std::tr1::shared_ptr<CHAR_BASE_CACHE_DATA> spData = iterName->second;        
        m_NameDb.erase(iterName);
        m_DbName.erase(spData->ChaDbNum());
    }
}

void CharNameCache::Del(const DWORD ChaDbNum)
{
    sc::CriticalSectionOwnerLock OwnerLock(m_Lock);

    DBNUM_NAME_ITER iterDbNum = m_DbName.find(ChaDbNum);
    if (iterDbNum != m_DbName.end())
    {
        std::tr1::shared_ptr<CHAR_BASE_CACHE_DATA> spData = iterDbNum->second;        
        m_DbName.erase(iterDbNum);
        m_NameDb.erase(spData->ChaName());
    }
}

DWORD CharNameCache::FindChaDbNum(const std::string& ChaName)
{
    sc::CriticalSectionOwnerLock OwnerLock(m_Lock);

    NAME_DBNUM_CITER iterName = m_NameDb.find(ChaName);
    if (iterName != m_NameDb.end())
    {
        std::tr1::shared_ptr<CHAR_BASE_CACHE_DATA> spData = iterName->second;
        return spData->ChaDbNum();
    }
    else
    {
        return 0;
    }
}

std::string CharNameCache::FindChaName(DWORD ChaDbNum)
{
    sc::CriticalSectionOwnerLock OwnerLock(m_Lock);

    DBNUM_NAME_CITER iterDbNum = m_DbName.find(ChaDbNum);
    if (iterDbNum != m_DbName.end())
    {
        std::tr1::shared_ptr<CHAR_BASE_CACHE_DATA> spData = iterDbNum->second;
        return spData->ChaName();
    }
    else
    {
        return std::string("");
    }
}

std::tr1::shared_ptr<CHAR_BASE_CACHE_DATA> CharNameCache::Find(const std::string& ChaName)
{
    sc::CriticalSectionOwnerLock OwnerLock(m_Lock);

    NAME_DBNUM_CITER iterName = m_NameDb.find(ChaName);
    if (iterName != m_NameDb.end())
    {
        return iterName->second;
    }
    else
    {
        std::tr1::shared_ptr<CHAR_BASE_CACHE_DATA> spData((CHAR_BASE_CACHE_DATA*) 0);
        return spData;
    }
}

std::tr1::shared_ptr<CHAR_BASE_CACHE_DATA> CharNameCache::Find(DWORD ChaDbNum)
{
    sc::CriticalSectionOwnerLock OwnerLock(m_Lock);

    DBNUM_NAME_CITER iterDbNum = m_DbName.find(ChaDbNum);
    if (iterDbNum != m_DbName.end())
    {
        return iterDbNum->second;
    }
    else
    {
        std::tr1::shared_ptr<CHAR_BASE_CACHE_DATA> spData((CHAR_BASE_CACHE_DATA*) 0);
        return spData;
    }
}