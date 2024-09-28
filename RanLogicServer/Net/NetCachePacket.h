#ifndef _NET_CACHE_PACKET_H_
#define _NET_CACHE_PACKET_H_

#pragma once

#include "../../RanLogic/s_NetGlobal.h"

//! Cache Packet
namespace cp
{

//! Agent->Cache 게임 접속했음
struct NET_CHAR_JOIN_AH : public NET_MSG_GENERIC
{
    DWORD m_UserDbNum; //! 유저 DB 번호
    DWORD m_ChaDbNum; //! 캐릭터 DB 번호

    NET_CHAR_JOIN_AH(DWORD UserDbNum, DWORD ChaDbNum)
        : m_UserDbNum(UserDbNum)
        , m_ChaDbNum(ChaDbNum)
    {
        dwSize = sizeof(NET_CHAR_JOIN_AH);
        nType = NET_MSG_GAME_JOIN_AH;
        MIN_STATIC_ASSERT(sizeof(NET_CHAR_JOIN_AH)<=NET_DATA_BUFSIZE);
    }

    //! 유저 DB 번호
    inline DWORD UserDbNum() const { return m_UserDbNum; }
    //! 캐릭터 DB 번호
    inline DWORD ChaDbNum() const { return m_ChaDbNum; }
};

//! Agent->Cache 게임 접속종료
struct NET_CHAR_CLOSE_AH : public NET_MSG_GENERIC
{
    DWORD m_UserDbNum; //! 유저 DB 번호
    DWORD m_ChaDbNum; //! 캐릭터 DB 번호

    NET_CHAR_CLOSE_AH(DWORD UserDbNum, DWORD ChaDbNum)
        : m_UserDbNum(UserDbNum)
        , m_ChaDbNum(ChaDbNum)
    {
        dwSize = sizeof(NET_CHAR_CLOSE_AH);
        nType = NET_MSG_CHAR_CLOSE_AH;
        MIN_STATIC_ASSERT(sizeof(NET_CHAR_CLOSE_AH)<=NET_DATA_BUFSIZE);
    }

    //! 유저 DB 번호
    inline DWORD UserDbNum() const { return m_UserDbNum; }
    //! 캐릭터 DB 번호
    inline DWORD ChaDbNum() const { return m_ChaDbNum; }
};

//! Field->Cache:캐릭터 Point 정보 요청
struct NET_CHAR_REQ_POINT_FH : public NET_MSG_GENERIC
{
    DWORD m_ChaDbNum;
    NET_CHAR_REQ_POINT_FH(DWORD ChaDbNum)
        : m_ChaDbNum(ChaDbNum)
    {
        dwSize = sizeof(NET_CHAR_REQ_POINT_FH);
        nType = NET_MSG_CHAR_REQ_POINT_FH;
        MIN_STATIC_ASSERT(sizeof(NET_CHAR_REQ_POINT_FH)<=NET_DATA_BUFSIZE);
    }

    inline DWORD ChaDbNum() const { return m_ChaDbNum; }
};

}

#endif