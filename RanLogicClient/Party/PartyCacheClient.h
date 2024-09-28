#pragma once

#include <set>
#include <map>
#include <boost/tr1/memory.hpp>
#include "../../RanLogic/Msg/PartyMsg.h"

struct PARTY_CACHE_CLIENT : public party::NET_PARTY_INFO_REQ_AC
{
    DWORD m_LastUpdateTime;

    PARTY_CACHE_CLIENT();
    DWORD GetLastUpdateTime() const { return m_LastUpdateTime; }
};

class PartyClientCache
{
    enum { CACHE_DELETE_TIME = 60000, };

public:
    PartyClientCache();
    virtual ~PartyClientCache();

protected:
    typedef std::map<GLPartyID, std::tr1::shared_ptr<PARTY_CACHE_CLIENT> > PARTY_CACHE;
    typedef PARTY_CACHE::iterator       PARTY_CACHE_ITER;
    typedef PARTY_CACHE::const_iterator PARTY_CACHE_CITER;
    typedef PARTY_CACHE::value_type     PARTY_CACHE_VALUE;

    PARTY_CACHE m_Data;

    typedef std::set<GLPartyID>          REQ_LIST;
    typedef REQ_LIST::iterator       REQ_LIST_ITER;
    typedef REQ_LIST::const_iterator REQ_LIST_CITER;

    REQ_LIST m_ReqList;

public:
    void Add(const party::NET_PARTY_INFO_REQ_AC& PartyData);
    void Reset();
    std::tr1::shared_ptr<PARTY_CACHE_CLIENT> GetData(const GLPartyID& PartyID);
    //! 요청 리스트에 등록
    void AddReqList(const GLPartyID& PartyID);
    //! 요청 리스트에 있는가?
    bool HaveReqList(const GLPartyID& PartyID);
};