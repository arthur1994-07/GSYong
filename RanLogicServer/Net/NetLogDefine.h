#ifndef _RLS_NET_LOG_DEFINE_H_
#define _RLS_NET_LOG_DEFINE_H_

#pragma once

#include "../../SigmaCore/gassert.h"
#include "../../RanLogic/s_NetGlobal.h"
#include "../Database/DbDefineLog.h"

//! 아이템 거래로그
struct NET_LOG_ITEM_EXCHANGE : public NET_MSG_GENERIC
{
    LOG_ITEM_EXCHANGE Data;

    NET_LOG_ITEM_EXCHANGE(const LOG_ITEM_EXCHANGE& _Data)
        : Data(_Data)
    {
        dwSize = sizeof(NET_LOG_ITEM_EXCHANGE);
        nType = NET_MSG_LOG_ITEM_EXCHANGE;
        MIN_STATIC_ASSERT(sizeof(NET_LOG_ITEM_EXCHANGE)<=NET_DATA_BUFSIZE);
    }
};

//! 게임머니 거래로그
struct NET_LOG_MONEY_EXCHANGE : public NET_MSG_GENERIC
{
    LOG_MONEY_EXCHANGE Data;
    NET_LOG_MONEY_EXCHANGE(const LOG_MONEY_EXCHANGE& _Data)
        : Data(_Data)
    {
        dwSize = sizeof(NET_LOG_MONEY_EXCHANGE);
        nType = NET_MSG_LOG_MONEY_EXCHANGE;
        MIN_STATIC_ASSERT(sizeof(NET_LOG_MONEY_EXCHANGE)<=NET_DATA_BUFSIZE);
    }
};

//! 아이템 인첸트로그
struct NET_LOG_ITEM_CONVERSION : public NET_MSG_GENERIC
{
    LOG_ITEM_CONVERSION Data;
    NET_LOG_ITEM_CONVERSION(const LOG_ITEM_CONVERSION& _Data)
        : Data(_Data)
    {
        dwSize = sizeof(NET_LOG_ITEM_CONVERSION);
        nType = NET_MSG_LOG_ITEM_CONVERSION;
        MIN_STATIC_ASSERT(sizeof(NET_LOG_ITEM_CONVERSION)<=NET_DATA_BUFSIZE);
    }
};

//! Action 로그
struct NET_LOG_ACTION : public NET_MSG_GENERIC
{
    LOG_ACTION Data;
    NET_LOG_ACTION(const LOG_ACTION& _Data)
        : Data(_Data)
    {
        dwSize = sizeof(NET_LOG_ACTION);
        nType = NET_MSG_LOG_ACTION;
        MIN_STATIC_ASSERT(sizeof(NET_LOG_ACTION)<=NET_DATA_BUFSIZE);
    }

};

//! Item random option log
struct NET_LOG_RANDOM_OPTION : public NET_MSG_GENERIC
{
    LOG_RANDOM_OPTION Data;
    NET_LOG_RANDOM_OPTION(const LOG_RANDOM_OPTION& _Data)
        : Data(_Data)
    {
        dwSize = sizeof(NET_LOG_RANDOM_OPTION);
        nType = NET_MSG_LOG_RANDOM_OPTION;
        MIN_STATIC_ASSERT(sizeof(NET_LOG_RANDOM_OPTION)<=NET_DATA_BUFSIZE);
    }
};

struct NET_LOG_ADDON_OPTION : public NET_MSG_GENERIC
{
	LOG_ADDON_OPTION Data;
	NET_LOG_ADDON_OPTION( const LOG_ADDON_OPTION& _Data )
		: Data( _Data )
	{
		dwSize = sizeof( NET_LOG_ADDON_OPTION );
		nType = NET_MSG_LOG_ADDON_OPTION;
		MIN_STATIC_ASSERT( sizeof( NET_LOG_ADDON_OPTION ) <= NET_DATA_BUFSIZE );
	}
};

struct NET_LOG_LINKSKILL_OPTION : public NET_MSG_GENERIC
{
	LOG_LINKSKILL_OPTION Data;
	NET_LOG_LINKSKILL_OPTION( const LOG_LINKSKILL_OPTION& _Data )
		: Data( _Data )
	{
		dwSize = sizeof( NET_LOG_LINKSKILL_OPTION );
		nType = NET_MSG_LOG_LINKSKILL_OPTION;
		MIN_STATIC_ASSERT( sizeof( NET_LOG_LINKSKILL_OPTION ) <= NET_DATA_BUFSIZE );
	}
};

struct NET_LOG_BASICSTAT_OPTION : public NET_MSG_GENERIC
{
	LOG_BASICSTAT_OPTION Data;
	NET_LOG_BASICSTAT_OPTION( const LOG_BASICSTAT_OPTION& _Data )
		: Data( _Data )
	{
		dwSize = sizeof( NET_LOG_BASICSTAT_OPTION );
		nType = NET_MSG_LOG_BASICSTAT_OPTION;
		MIN_STATIC_ASSERT( sizeof( NET_LOG_BASICSTAT_OPTION ) <= NET_DATA_BUFSIZE );
	}
};

//! Item costumestat log
struct NET_LOG_COSTUME_STATS : public NET_MSG_GENERIC
{
	LOG_COSTUME_STATS Data;
	NET_LOG_COSTUME_STATS(const LOG_COSTUME_STATS& _Data)
		: Data(_Data)
	{
		dwSize = sizeof(NET_LOG_COSTUME_STATS);
		nType = NET_MSG_LOG_INVEN_COSTUME_STATS;
		MIN_STATIC_ASSERT(sizeof(NET_LOG_COSTUME_STATS)<=NET_DATA_BUFSIZE);
	}
};

//! Pet action log
struct NET_LOG_PET_ACTION : public NET_MSG_GENERIC
{
    LOG_PET_ACTION Data;
    NET_LOG_PET_ACTION(const LOG_PET_ACTION& _Data)
        : Data(_Data)
    {
        dwSize = sizeof(NET_LOG_PET_ACTION);
        nType = NET_MSG_LOG_PET_ACTION;
        MIN_STATIC_ASSERT(sizeof(NET_LOG_PET_ACTION)<=NET_DATA_BUFSIZE);
    }

};

//! Vehicle action log
struct NET_LOG_VEHICLE_ACTION : public NET_MSG_GENERIC
{
    LOG_VEHICLE_ACTION Data;
    NET_LOG_VEHICLE_ACTION(const LOG_VEHICLE_ACTION& _Data)
        : Data(_Data)
    {
        dwSize = sizeof(NET_LOG_VEHICLE_ACTION);
        nType = NET_MSG_LOG_VEHICLE_ACTION;
        MIN_STATIC_ASSERT(sizeof(NET_LOG_VEHICLE_ACTION)<=NET_DATA_BUFSIZE);
    }
};

struct NET_LOG_ATTENDANCE
{
    int m_ChaNum;
    int m_Type;
    std::string m_LoadType;
    int m_DayN;
    int m_Status;

    MSGPACK_DEFINE(m_ChaNum, m_Type, m_LoadType, m_DayN, m_Status);

    NET_LOG_ATTENDANCE()
        : m_ChaNum(0)
        , m_Type(0)
        , m_DayN(0)
        , m_Status(0)
    {
    }

    NET_LOG_ATTENDANCE(
        int ChaNum,
        int Type,
        const std::string& LoadType,
        int DayN,
        int Status)
        : m_ChaNum(ChaNum)
        , m_Type(Type)
        , m_LoadType(LoadType)
        , m_DayN(DayN)
        , m_Status(Status)
    {
    }
};

struct NET_LOG_ATTENDANCE_TASK
{
    int m_ChaNum;
	LONGLONG m_ChaCurExp;
    int m_TaskN;
    int m_Type;
    int m_Status;
    int m_Value;
    int m_ValueBase;
    int m_RewardPoint;
    int m_RewardExp;
    int m_RewardMoney;
    int m_RewardItems;
    int m_RewardItem1;
    int m_RewardItem2;
    int m_RewardItem3;
    int m_TookRewards;

    MSGPACK_DEFINE(m_ChaNum, m_ChaCurExp, m_TaskN, m_Type, m_Status, m_Value, m_ValueBase, m_RewardPoint, m_RewardExp, m_RewardMoney, m_RewardItems, m_RewardItem1, m_RewardItem2, m_RewardItem3, m_TookRewards);

    NET_LOG_ATTENDANCE_TASK()
        : m_ChaNum(0), m_ChaCurExp(0), m_TaskN(0), m_Type(0), m_Status(0), m_Value(0)
        , m_ValueBase(0), m_RewardPoint(0), m_RewardExp(0), m_RewardMoney(0), m_RewardItems(0)
        , m_RewardItem1(0), m_RewardItem2(0), m_RewardItem3(0), m_TookRewards(0)
    {
    }

    NET_LOG_ATTENDANCE_TASK(
        int ChaNum, LONGLONG ChaCurExp, int TaskN, int Type, int Status, int Value,
        int ValueBase, int RewardPoint, int RewardExp, int RewardMoney, int RewardItems,
        int RewardItem1, int RewardItem2, int RewardItem3, int TookRewards)
        : m_ChaNum(ChaNum), m_ChaCurExp(ChaCurExp), m_TaskN(TaskN), m_Type(Type), m_Status(Status), m_Value(Value)
        , m_ValueBase(ValueBase), m_RewardPoint(RewardPoint), m_RewardExp(RewardExp), m_RewardMoney(RewardMoney), m_RewardItems(RewardItems)
        , m_RewardItem1(RewardItem1), m_RewardItem2(RewardItem2), m_RewardItem3(RewardItem3), m_TookRewards(TookRewards)
    {
    }
};

#endif // _RLS_NET_LOG_DEFINE_H_