/*
 * redmine : #1161 Ȯ������ �ý��� ó��
 * desc : Randombox ����Ʈ ����
 * created : sckim, 2015.12.15
 * modified : 
 */

#ifndef _CACHE_RANDOMBOX_CHANCE_H_
#define _CACHE_RANDOMBOX_CHANCE_H_

#pragma once

#include "../../EngineLib/G-Logic/TypeDefine.h"

#include <string>
#include <boost/unordered_map.hpp>
#include <boost/tr1/memory.hpp>

struct RANDOMBOX_CHANCE_CACHE_DATA
{
    SNATIVEID       sBoxID;       /* RandomBox ID */
    SNATIVEID       sItemID;      /* Item ID */
    INT32           nReqCount;    /* ��÷���� �ҿ�Ǵ� Randombox ���� �� */
    DWORD           dwChaNum;     /* ��÷ Target ChaNum, GM Command���� ChaNum�� �Ѿ���� ������ 0���� Setting */
    DWORD           dwGMNum;      /* ��û GM ChaNum */
    INT32           nOpenCount;   /* Randombox Open�� */
    INT32           nUniqueIndex; /* Index(Unique Key) */
    BOOL            bEnd;         /* ���� �Ϸ� : �κ��丮�� �� �־����� �̳��� üũ�Ѵ�. */

    RANDOMBOX_CHANCE_CACHE_DATA()
        : sBoxID(false)
        , sItemID(false)
        , nReqCount(0)
        , dwChaNum(0)
        , dwGMNum(0)
        , nOpenCount(0)
        , nUniqueIndex(0)
        , bEnd(false)
    {
    }

    RANDOMBOX_CHANCE_CACHE_DATA(const SNATIVEID& BoxId, const SNATIVEID& ItemId, INT32 ReqCount, DWORD ChaNum, DWORD GMNum, INT32 UniqueIndex)
        : sBoxID(BoxId)
        , sItemID(ItemId)
        , nReqCount(ReqCount)
        , dwChaNum(ChaNum)
        , dwGMNum(GMNum)
        , nOpenCount(0)
        , nUniqueIndex(UniqueIndex)
        , bEnd(false)
    {
    }

    inline SNATIVEID BoxId() const { return sBoxID; }
    inline SNATIVEID ItemId() const { return sItemID; }
    inline INT32 ReqCount() const { return nReqCount; }
    inline DWORD ChaNum() const { return dwChaNum; }
    inline DWORD GMNum() const { return dwGMNum; }
    inline INT32 OpenCount() const { return nOpenCount; }
    inline INT32 UniqueIndex() const { return nUniqueIndex; }
    inline BOOL isEnd() const { return bEnd; };

    inline INT32 PlusOpenCount() { return ++nOpenCount; }
    inline BOOL SetEnd(BOOL bend)
    { 
        bEnd = bend;
        return bEnd;
    }
};

class CacheRandomboxChance
{
public:
    CacheRandomboxChance();
    ~CacheRandomboxChance();

protected:
    INT32 m_nIndex;

    //! Index + Randombox Ȯ������ ������
    typedef boost::unordered_map<INT32, std::tr1::shared_ptr<RANDOMBOX_CHANCE_CACHE_DATA>> INDEX_RANDOMBOXCHANCE;
    typedef INDEX_RANDOMBOXCHANCE::iterator       INDEX_RANDOMBOXCHANCE_ITER;
    typedef INDEX_RANDOMBOXCHANCE::const_iterator INDEX_RANDOMBOXCHANCE_CITER;
    typedef INDEX_RANDOMBOXCHANCE::value_type     INDEX_RANDOMBOXCHANCE_VALUE;

    INDEX_RANDOMBOXCHANCE m_mapRandomboxChanceInfo;

    CRITICAL_SECTION m_Lock;

private:
    INT32 FindIndex(const SNATIVEID& BoxId, const SNATIVEID& ItemId, INT32 ReqCount, DWORD ChaNum);
    std::tr1::shared_ptr<RANDOMBOX_CHANCE_CACHE_DATA> FindData(INT32 nindex);

public:
    BOOL Add(const SNATIVEID& BoxId, const SNATIVEID& ItemId, INT32 ReqCount, DWORD ChaNum, DWORD GMNum);
    BOOL Delete(INT32 nindex);
    BOOL List(std::vector<RANDOMBOX_CHANCE_CACHE_DATA>& vRCCD);
    BOOL CheckRandomboxOpenChance(const SNATIVEID& BoxId, DWORD ChaNum, RANDOMBOX_CHANCE_CACHE_DATA& pRCCD, INT32& IndexAll, INT32& IndexCha);
    void EndRandomboxOpenChanceDelete(BOOL bComplete, INT32 IndexAll, INT32 IndexCha);
};

#endif // _CACHE_RANDOMBOX_CHANCE_H_
