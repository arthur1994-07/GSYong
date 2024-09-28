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

    REQ_LIST m_ReqList; //! ��û�� ����Ʈ

public:
    void Add(const std::string& ChaName, DWORD ChaDbNum);

    //! DB ��ȣ�� ĳ���� �̸��� ã��
    std::string FindByDbNum(DWORD ChaDbNum);

    //! ĳ���� �̸����� DB ��ȣ�� ã��
    DWORD FindByChaName(const std::string& ChaName);

    //! ��� ����
    void Clear();

    //! ���
    void Display();

    //! ��û ����Ʈ�� ���
    void AddReqList(const std::string& ChaName, DWORD ReqNum);

    //! ��û ��ȣ�� �����´�
    DWORD GetReqNum(const std::string& ChaName);
};