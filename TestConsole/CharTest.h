#pragma once

#include <iostream>
#include <string>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/key_extractors.hpp>
#include <boost/tuple/tuple.hpp>


class CHARACTER 
{
public:
    CHARACTER(const int nID, const int nLevel, const int nExp) 
        : m_nID(nID), m_nLevel(nLevel), m_nExp(nExp)
    {
    }

    int m_nLevel;
    int m_nExp;
    int m_nID;

    const int Level() const { return m_nLevel; }
    const int Exp() const { return m_nExp; }    
};

// 컨테이너의 키를 선언
typedef boost::multi_index::const_mem_fun<CHARACTER, const int, &CHARACTER::Level>	IDX_LEVEL;
typedef boost::multi_index::member<CHARACTER, int, &CHARACTER::m_nExp>				IDX_EXP;
typedef boost::multi_index::composite_key<CHARACTER, IDX_LEVEL, IDX_EXP>			IDX_LEVEL_EXP;
typedef boost::multi_index::composite_key<CHARACTER, IDX_EXP, IDX_LEVEL>			IDX_EXP_LEVEL;
typedef boost::multi_index::member<CHARACTER, int, &CHARACTER::m_nID>				IDX_CHARID;

// 인덱싱 타입을 선언
typedef struct indices : public boost::multi_index::indexed_by
<
    boost::multi_index::ordered_non_unique<IDX_LEVEL_EXP>,
    boost::multi_index::hashed_non_unique<IDX_EXP_LEVEL>,
    boost::multi_index::hashed_unique<IDX_CHARID>,
    boost::multi_index::hashed_non_unique<IDX_LEVEL>
>
{
    enum INDEX
    {
        IDX_NON_UNIQUE_LEVEL_EXP = 0				
        , IDX_NON_UNIQUE_EXP_LEVEL 
        , IDX_UNIQUE_CHARID
        , IDX_NON_UNIQUE_LEVEL 
        , IDX_END		
    };
} INDICES;

typedef boost::multi_index_container<CHARACTER*, indices> CharContainer;
