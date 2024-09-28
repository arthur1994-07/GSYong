#include "pch.h"
#include "./PartyCacheClient.h"

#include "../../SigmaCore/DebugInclude.h"

PARTY_CACHE_CLIENT::PARTY_CACHE_CLIENT()
{
    m_LastUpdateTime = GetTickCount();
}

PartyClientCache::PartyClientCache()
{
}

PartyClientCache::~PartyClientCache()
{
}

void PartyClientCache::Reset()
{
    m_Data.clear();
    m_ReqList.clear();
}

void PartyClientCache::Add(const party::NET_PARTY_INFO_REQ_AC& PartyData)
{
    PARTY_CACHE_ITER iter = m_Data.find(PartyData.PartyID.GetPartyID());
    if (iter != m_Data.end())
        m_Data.erase(iter);

    std::tr1::shared_ptr<PARTY_CACHE_CLIENT> spData(new PARTY_CACHE_CLIENT);
    spData->PartyID = PartyData.PartyID;
    spData->m_Member  = PartyData.m_Member;

    m_Data.insert(PARTY_CACHE_VALUE(spData->PartyID.GetPartyID(), spData));

    // 요청리스트 삭제
    REQ_LIST_ITER iterReq = m_ReqList.find(PartyData.PartyID.GetPartyID());
    if (iterReq != m_ReqList.end())
        m_ReqList.erase(iterReq);
}

std::tr1::shared_ptr<PARTY_CACHE_CLIENT> PartyClientCache::GetData(const GLPartyID& PartyID)
{
    PARTY_CACHE_ITER iter = m_Data.find(PartyID);
    if (iter != m_Data.end())
    {
        DWORD CurrentTime = GetTickCount();
        std::tr1::shared_ptr<PARTY_CACHE_CLIENT> spData = iter->second;
        if (CurrentTime - spData->GetLastUpdateTime() > CACHE_DELETE_TIME)
        {            
            return std::tr1::shared_ptr<PARTY_CACHE_CLIENT> ((PARTY_CACHE_CLIENT*) 0);
        }
        else
        {
            return spData;
        }
    }
    else
    {
        return std::tr1::shared_ptr<PARTY_CACHE_CLIENT> ((PARTY_CACHE_CLIENT*) 0);
    }
}

void PartyClientCache::AddReqList(const GLPartyID& PartyID)
{
    m_ReqList.insert(PartyID);
}

bool PartyClientCache::HaveReqList(const GLPartyID& PartyID)
{
    REQ_LIST_CITER iterReq = m_ReqList.find(PartyID);
    if (iterReq != m_ReqList.end())
        return true;
    else
        return false;
}