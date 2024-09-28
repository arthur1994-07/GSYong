#ifndef _DB_ACTION_GAME_POINT_SHOP_H_
#define _DB_ACTION_GAME_POINT_SHOP_H_

#pragma once

#include "./DbAction.h"

namespace db
{

class CharacterPointSet : public DbAction
{
public:
    CharacterPointSet(
        DWORD ChaDbNum,
        __int64 PointRefundable,
        __int64 PointNotRefundable);
    virtual ~CharacterPointSet();
    virtual int Execute(NetServer* pServer) override;

protected:
    DWORD m_ChaDbNum;
    __int64 m_PointRefundable; //! 환불가능 포인트
    __int64 m_PointNotRefundable; //! 환불불가 포인트    
};

} // namespace db

#endif // _DB_ACTION_GAME_POINT_SHOP_H_