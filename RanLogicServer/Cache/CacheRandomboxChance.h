/*
 * redmine : #1161 확률제어 시스템 처리
 * desc : Randombox 리스트 관리
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
    INT32           nReqCount;    /* 당첨까지 소요되는 Randombox 오픈 수 */
    DWORD           dwChaNum;     /* 당첨 Target ChaNum, GM Command에서 ChaNum이 넘어오지 않으면 0으로 Setting */
    DWORD           dwGMNum;      /* 요청 GM ChaNum */
    INT32           nOpenCount;   /* Randombox Open수 */
    INT32           nUniqueIndex; /* Index(Unique Key) */
    BOOL            bEnd;         /* 지급 완료 : 인벤토리에 못 넣었을때 이넘을 체크한다. */

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

    //! Index + Randombox 확률제어 데이터
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
