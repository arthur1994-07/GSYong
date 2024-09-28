/*
 * redmine : #1161 확률제어 시스템 처리
 * desc : Randombox 리스트 관리
 * created : sckim, 2015.12.15
 * modified : 
 */

#include "pch.h"
#include "../../SigmaCore/Util/Lock.h"
#include "./CacheRandomboxChance.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------

CacheRandomboxChance::CacheRandomboxChance()
{
    m_nIndex = 0;

    InitializeCriticalSectionAndSpinCount(&m_Lock, sc::CRITICAL_SECTION_SPIN_COUNT);
}

CacheRandomboxChance::~CacheRandomboxChance()
{
    DeleteCriticalSection(&m_Lock);
}

INT32 CacheRandomboxChance::FindIndex(const SNATIVEID& BoxId, const SNATIVEID& ItemId, INT32 ReqCount, DWORD ChaNum)
{
    sc::CriticalSectionOwnerLock OwnerLock(m_Lock);

    INT32 nRtnValue = 0;

    // 해당 데이터가 존재하는지 체크
	for (INDEX_RANDOMBOXCHANCE_CITER iter = m_mapRandomboxChanceInfo.begin(); iter != m_mapRandomboxChanceInfo.end(); ++iter)
	{
        std::tr1::shared_ptr<RANDOMBOX_CHANCE_CACHE_DATA> spData = iter->second;
        if (spData->BoxId() == BoxId &&
            spData->ItemId() == ItemId &&
            spData->ReqCount() == ReqCount &&
            spData->ChaNum() == ChaNum)
        {
            nRtnValue = spData->UniqueIndex();
            break;
        }
	}

    return nRtnValue;
}

std::tr1::shared_ptr<RANDOMBOX_CHANCE_CACHE_DATA> CacheRandomboxChance::FindData(INT32 nindex)
{
    sc::CriticalSectionOwnerLock OwnerLock(m_Lock);

    INDEX_RANDOMBOXCHANCE_CITER iter = m_mapRandomboxChanceInfo.find(nindex);
    if (iter != m_mapRandomboxChanceInfo.end())
    {
        return iter->second;
    }
    else
    {
        std::tr1::shared_ptr<RANDOMBOX_CHANCE_CACHE_DATA> rccd((RANDOMBOX_CHANCE_CACHE_DATA*) 0);
        return rccd;
    }
}

BOOL CacheRandomboxChance::Add(const SNATIVEID& BoxId, const SNATIVEID& ItemId, INT32 ReqCount, DWORD ChaNum, DWORD GMNum)
{
    // 데이터가 존재하면 오류
    INT32 nUIndex = FindIndex(BoxId, ItemId, ReqCount, ChaNum);
    if (nUIndex > 0)
    {
        return false;
    }

    sc::CriticalSectionOwnerLock OwnerLock(m_Lock);

    INT32 tmpIndex = ++m_nIndex;

    std::tr1::shared_ptr<RANDOMBOX_CHANCE_CACHE_DATA> rccd(new RANDOMBOX_CHANCE_CACHE_DATA(BoxId, ItemId, ReqCount, ChaNum, GMNum, tmpIndex));
    m_mapRandomboxChanceInfo.insert(INDEX_RANDOMBOXCHANCE_VALUE(tmpIndex, rccd));

    return true;
}

BOOL CacheRandomboxChance::Delete(INT32 nindex)
{
    sc::CriticalSectionOwnerLock OwnerLock(m_Lock);

    if (m_mapRandomboxChanceInfo.size() == 0)
    {
        return false;
    }

    if (nindex > 0)
    {
        INDEX_RANDOMBOXCHANCE_ITER iterA = m_mapRandomboxChanceInfo.find(nindex);
        if (iterA != m_mapRandomboxChanceInfo.end())
        {
            m_mapRandomboxChanceInfo.erase(iterA);

            return true;
        }
    }

    return false;
}

BOOL CacheRandomboxChance::List(std::vector<RANDOMBOX_CHANCE_CACHE_DATA>& vRCCD)
{
    sc::CriticalSectionOwnerLock OwnerLock(m_Lock);

    if (m_mapRandomboxChanceInfo.size() == 0)
    {
        return false;
    }

	for (INDEX_RANDOMBOXCHANCE_CITER iter = m_mapRandomboxChanceInfo.begin(); iter != m_mapRandomboxChanceInfo.end(); ++iter)
	{
        std::tr1::shared_ptr<RANDOMBOX_CHANCE_CACHE_DATA> spData = iter->second;
        if (spData->isEnd() == false)
        {
            RANDOMBOX_CHANCE_CACHE_DATA rccd;
            rccd.sBoxID = spData->BoxId();
            rccd.sItemID = spData->ItemId();
            rccd.nReqCount = spData->ReqCount();
            rccd.dwChaNum = spData->ChaNum();
            rccd.dwGMNum = spData->GMNum();
            rccd.nOpenCount = spData->OpenCount();
            rccd.nUniqueIndex = spData->UniqueIndex();
            rccd.bEnd = spData->isEnd();

            vRCCD.push_back(rccd);
        }
	}

    if (vRCCD.size() > 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

BOOL CacheRandomboxChance::CheckRandomboxOpenChance(const SNATIVEID& BoxId, DWORD ChaNum, RANDOMBOX_CHANCE_CACHE_DATA& rccd, INT32& IndexAll, INT32& IndexCha)
{
    sc::CriticalSectionOwnerLock OwnerLock(m_Lock);

    if (m_mapRandomboxChanceInfo.size() == 0)
    {
        return false;
    }

    BOOL bFindData = false;

    // ChaNum이 정의되지 않은 데이터가 있는지 확인
	for (INDEX_RANDOMBOXCHANCE_CITER iter = m_mapRandomboxChanceInfo.begin(); iter != m_mapRandomboxChanceInfo.end(); ++iter)
	{
        std::tr1::shared_ptr<RANDOMBOX_CHANCE_CACHE_DATA> spData = iter->second;
        if (spData->BoxId() == BoxId &&
            spData->ChaNum() == 0 &&
            spData->isEnd() == false)
        {
            INT32 opencnt = spData->PlusOpenCount();
            if (opencnt >= spData->ReqCount())
            {
                IndexAll = spData->UniqueIndex();
                rccd.sItemID = spData->ItemId();
                rccd.nReqCount = spData->ReqCount();
                rccd.dwChaNum = spData->ChaNum();
                rccd.dwGMNum = spData->GMNum();
                rccd.nOpenCount = spData->OpenCount();

                bFindData = spData->SetEnd(true);
            }

            break;
        }
	}

    // ChaNum이 정의된 데이터가 있는지 확인
	for (INDEX_RANDOMBOXCHANCE_CITER iter = m_mapRandomboxChanceInfo.begin(); iter != m_mapRandomboxChanceInfo.end(); ++iter)
	{
        std::tr1::shared_ptr<RANDOMBOX_CHANCE_CACHE_DATA> spData = iter->second;
        if (spData->BoxId() == BoxId &&
            spData->ChaNum() == ChaNum &&
            spData->isEnd() == false)
        {
            INT32 opencnt = spData->PlusOpenCount();
            //if ((opencnt >= spData->ReqCount()) || bFindData)
            if (opencnt >= spData->ReqCount())
            {
                IndexCha = spData->UniqueIndex();
                if (!bFindData)
                {
                    rccd.sItemID = spData->ItemId();
                    rccd.nReqCount = spData->ReqCount();
                    rccd.dwChaNum = spData->ChaNum();
                    rccd.dwGMNum = spData->GMNum();
                    rccd.nOpenCount = spData->OpenCount();
                }
                bFindData = spData->SetEnd(true);
            }

            break;
        }
	}

    return bFindData;
}

void CacheRandomboxChance::EndRandomboxOpenChanceDelete(BOOL bComplete, INT32 IndexAll, INT32 IndexCha)
{
    sc::CriticalSectionOwnerLock OwnerLock(m_Lock);

    if (m_mapRandomboxChanceInfo.size() == 0)
    {
        return;
    }

    if (IndexAll > 0)
    {
        INDEX_RANDOMBOXCHANCE_ITER iterA = m_mapRandomboxChanceInfo.find(IndexAll);
        if (iterA != m_mapRandomboxChanceInfo.end() )
        {
            if (bComplete)
            {
	            m_mapRandomboxChanceInfo.erase(iterA);
            }
            else
            {
                std::tr1::shared_ptr<RANDOMBOX_CHANCE_CACHE_DATA> spData = iterA->second;
                spData->SetEnd(false);
            }
        }
    }

    if (IndexCha > 0)
    {
        INDEX_RANDOMBOXCHANCE_ITER iterC = m_mapRandomboxChanceInfo.find(IndexCha);
        if (iterC != m_mapRandomboxChanceInfo.end() )
        {
            if (bComplete)
            {
	            m_mapRandomboxChanceInfo.erase(iterC);
            }
            else
            {
                std::tr1::shared_ptr<RANDOMBOX_CHANCE_CACHE_DATA> spData = iterC->second;
                spData->SetEnd(false);
            }
        }
    }
}
