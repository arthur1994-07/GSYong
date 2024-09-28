#ifndef _GL_CHAR_CACHE_H_
#define _GL_CHAR_CACHE_H_

#pragma once

#include <boost/tr1/memory.hpp>
#include "../../RanLogic/Character/GLCharDefine.h"

//! Cache ������ ĳ���� ����
class GLCharCache
{
public:
    GLCharCache(DWORD UserDbNum, DWORD ChaDbNum);
    virtual ~GLCharCache();

private:
    DWORD m_UserDbNum; //! ���� DB ��ȣ
    DWORD m_ChaDbNum; //! ĳ���� DB ��ȣ

    CHARACTER_POINT m_Point; //! PointShop Point

public:
    //! ���� DB ��ȣ
    inline DWORD UserDbNum() const { return m_UserDbNum; }

    //! ĳ���� DB ��ȣ
    inline DWORD ChaDbNum() const { return m_ChaDbNum; }

    //! PointShop Point
    inline void PointSet(const CHARACTER_POINT& Point) { m_Point=Point; }
    inline CHARACTER_POINT Point() const { return m_Point; }

};

typedef std::tr1::shared_ptr<GLCharCache> spCharCache;

#endif // _GL_CHAR_CACHE_H_