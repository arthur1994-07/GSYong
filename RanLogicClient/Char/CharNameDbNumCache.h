#pragma once

#include <string>
#include <set>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/key_extractors.hpp>
#include <boost/tuple/tuple.hpp>

class GLGaeaClient;

//! 2012-08-10 jgkim
//! Character name and db number cache
class CharNameDbNumCache
{
public:
    struct CACHE_DATA
    {
        std::string m_ChaName;
        DWORD       m_ChaDbNum;

        CACHE_DATA()
            : m_ChaDbNum(0)
        {
        }

        CACHE_DATA(DWORD ChaDbNum, const std::string& ChaName)
            : m_ChaName(ChaName)
            , m_ChaDbNum(ChaDbNum)
        {
        }
    };

public:
    CharNameDbNumCache(GLGaeaClient* pGaeaClient);
    ~CharNameDbNumCache();

protected:
    typedef boost::multi_index_container<
        CACHE_DATA,
        boost::multi_index::indexed_by<
            boost::multi_index::ordered_unique<BOOST_MULTI_INDEX_MEMBER(CACHE_DATA, DWORD, m_ChaDbNum)>,
            boost::multi_index::ordered_unique<BOOST_MULTI_INDEX_MEMBER(CACHE_DATA, std::string, m_ChaName)> 
        >
    > CACHE_DATA_MULTI_INDEX;

    typedef CACHE_DATA_MULTI_INDEX::nth_index<0>::type  CACHE_DATA_MULTI_INDEX_CHA_DB_NUM;
    typedef CACHE_DATA_MULTI_INDEX::nth_index<1>::type  CACHE_DATA_MULTI_INDEX_CHA_NAME;

    CACHE_DATA_MULTI_INDEX m_CacheData;

    GLGaeaClient* m_pGaeaClient;    
    
    typedef std::map<std::string, DWORD> REQ_LIST;
    typedef REQ_LIST::iterator           REQ_LIST_ITER;
    typedef REQ_LIST::const_iterator     REQ_LIST_CITER;
    typedef REQ_LIST::value_type         REQ_LIST_VALUE;

    REQ_LIST m_ReqList; //! 요청한 리스트

public:
    void Add(const std::string& ChaName, DWORD ChaDbNum);

    //! DB 번호로 캐릭터 이름을 찾음
    std::string FindByDbNum(DWORD ChaDbNum);

    //! 캐릭터 이름으로 DB 번호를 찾음
    DWORD FindByChaName(const std::string& ChaName);

    //! 모두 지음
    void Clear();

    //! 출력
    void Display();

    //! 요청 리스트에 등록
    void AddReqList(const std::string& ChaName, DWORD ReqNum);

    //! 요청 번호를 가져온다
    DWORD GetReqNum(const std::string& ChaName);
};