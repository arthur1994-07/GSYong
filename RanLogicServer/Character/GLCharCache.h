#ifndef _GL_CHAR_CACHE_H_
#define _GL_CHAR_CACHE_H_

#pragma once

#include <boost/tr1/memory.hpp>
#include "../../RanLogic/Character/GLCharDefine.h"

//! Cache 서버용 캐릭터 정보
class GLCharCache
{
public:
    GLCharCache(DWORD UserDbNum, DWORD ChaDbNum);
    virtual ~GLCharCache();

private:
    DWORD m_UserDbNum; //! 유저 DB 번호
    DWORD m_ChaDbNum; //! 캐릭터 DB 번호

    CHARACTER_POINT m_Point; //! PointShop Point

public:
    //! 유저 DB 번호
    inline DWORD UserDbNum() const { return m_UserDbNum; }

    //! 캐릭터 DB 번호
    inline DWORD ChaDbNum() const { return m_ChaDbNum; }

    //! PointShop Point
    inline void PointSet(const CHARACTER_POINT& Point) { m_Point=Point; }
    inline CHARACTER_POINT Point() const { return m_Point; }

};

typedef std::tr1::shared_ptr<GLCharCache> spCharCache;

#endif // _GL_CHAR_CACHE_H_