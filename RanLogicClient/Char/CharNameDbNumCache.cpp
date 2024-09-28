#include "pch.h"
#include "../../SigmaCore/String/StringFormat.h"
#include "../../Enginelib/GUInterface/UITextControl.h"

#include "../GLGaeaClient.h"
#include "./CharNameDbNumCache.h"

#include "../../SigmaCore/DebugInclude.h"

CharNameDbNumCache::CharNameDbNumCache(GLGaeaClient* pGaeaClient)
    : m_pGaeaClient(pGaeaClient)
{
}

CharNameDbNumCache::~CharNameDbNumCache()
{
}

void CharNameDbNumCache::Add(const std::string& ChaName, DWORD ChaDbNum)
{
    CACHE_DATA InsertData(ChaDbNum, ChaName);
    
    CACHE_DATA_MULTI_INDEX_CHA_DB_NUM* IndexDbNum = &m_CacheData.get<0>();
    CACHE_DATA_MULTI_INDEX_CHA_DB_NUM::iterator it = IndexDbNum->find(ChaDbNum);
    if (it != IndexDbNum->end())
        IndexDbNum->replace(it, InsertData);
    else
        IndexDbNum->insert(InsertData);

    // 요청 리스트에서 삭제
    REQ_LIST_ITER iter = m_ReqList.find(ChaName);
    if (iter != m_ReqList.end())
        m_ReqList.erase(iter);
}

std::string CharNameDbNumCache::FindByDbNum(DWORD ChaDbNum)
{
    CACHE_DATA_MULTI_INDEX_CHA_DB_NUM* IndexDbNum = &m_CacheData.get<0>();
    CACHE_DATA_MULTI_INDEX_CHA_DB_NUM::iterator it = IndexDbNum->find(ChaDbNum);
    if (it != IndexDbNum->end())
        return it->m_ChaName;
    else
        return std::string("");
}
    
DWORD CharNameDbNumCache::FindByChaName(const std::string& ChaName)
{
    CACHE_DATA_MULTI_INDEX_CHA_NAME* IndexChaName = &m_CacheData.get<1>();
    CACHE_DATA_MULTI_INDEX_CHA_NAME::iterator it = IndexChaName->find(ChaName);
    if (it != IndexChaName->end())
        return it->m_ChaDbNum;
    else
        return 0;
}

void CharNameDbNumCache::Clear()
{
    m_CacheData.clear();
    m_ReqList.clear();
}

void CharNameDbNumCache::Display()
{
    if (!m_pGaeaClient)
        return;
    CACHE_DATA_MULTI_INDEX_CHA_NAME* IndexChaName = &m_CacheData.get<1>();
    for (CACHE_DATA_MULTI_INDEX_CHA_NAME::iterator it = IndexChaName->begin(); it!=IndexChaName->end(); ++it)
    {
        std::string ChaInfo(
            sc::string::format(
                "%1% DbNum:%2%",
                it->m_ChaName,
                it->m_ChaDbNum));
        m_pGaeaClient->PrintMsgText(NS_UITEXTCOLOR::DEFAULT, ChaInfo);
    }
}

void CharNameDbNumCache::AddReqList(const std::string& ChaName, DWORD ReqNum)
{
    m_ReqList.insert(REQ_LIST_VALUE(ChaName, ReqNum));
}

DWORD CharNameDbNumCache::GetReqNum(const std::string& ChaName)
{
    REQ_LIST_CITER iter = m_ReqList.find(ChaName);
    if (iter != m_ReqList.end())
    {
        return iter->second;
    }
    else
    {
        return 0;
    }
}