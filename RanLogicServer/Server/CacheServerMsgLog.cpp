#include "../pch.h"
#include "../Net/NetLogDefine.h"
#include "../Database/DBAction/DbActionLog.h"
#include "../Database/DBAction/DbActionGameAttendance.h"
#include "../Database/DbManager.h"
#include "../Net/NetCachePacket.h"
#include "./NetCacheClientMan.h"
#include "./CacheServer.h"

#include "../../RanLogic/Msg/GLContrlMsg.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void CacheServer::MsgHeartbeatReq(DWORD dwClient, NET_MSG_GENERIC* pMsg)
{
#ifdef HEARTBEAT_LOG
	sc::writeLogInfo(std::string("[HEARTBEAT] Recv Heartbeat Request from Session Svr"));
#endif
    //NET_HEARTBEAT_SERVER_REQ
    NET_HEARTBEAT_SERVER_ANS Msg;
    SendClient(dwClient, &Msg);
#ifdef HEARTBEAT_LOG
	sc::writeLogInfo(std::string("[HEARTBEAT] Send Heartbeat Response to Session Svr"));
#endif
}

void CacheServer::MsgHeartbeatAns(DWORD dwClient, NET_MSG_GENERIC* pMsg)
{
    if (m_pClientManager)
	{
#ifdef HEARTBEAT_LOG
		sc::writeLogInfo(sc::string::format("[HEARTBEAT] Recv Heartbeat Response from ( %1% ) Svr", dwClient));
#endif
        m_pClientManager->SetHeartBeat(dwClient);
	}
}

void CacheServer::MsgLogItemExchange(DWORD dwClient, NET_MSG_GENERIC* pMsg)
{
    NET_LOG_ITEM_EXCHANGE* pPacket = (NET_LOG_ITEM_EXCHANGE*) pMsg;    
    m_pDbManager->AddJob(
        db::DbActionPtr(
            new db::LogItemExchange(pPacket->Data)));

// #ifndef _RELEASE
//     sc::writeLogInfo(std::string("NET_LOG_ITEM_EXCHANGE LogItemExchange"));
// #endif
}

void CacheServer::MsgLogMoneyExchange(DWORD dwClient, NET_MSG_GENERIC* pMsg)
{
    NET_LOG_MONEY_EXCHANGE* pPacket = (NET_LOG_MONEY_EXCHANGE*) pMsg;
    m_pDbManager->AddLogJob(
        db::DbActionPtr(
            new db::LogMoneyExchange(pPacket->Data)));

// #ifndef _RELEASE
//     sc::writeLogInfo(std::string("NET_LOG_SLOGMONEYEXCHANGE LogMoneyExchange"));
// #endif
}

void CacheServer::MsgLogItemConversion(DWORD dwClient, NET_MSG_GENERIC* pMsg)
{
    NET_LOG_ITEM_CONVERSION* pPacket = (NET_LOG_ITEM_CONVERSION*) pMsg;
    m_pDbManager->AddLogJob(
        db::DbActionPtr(
            new db::LogItemConversion(pPacket->Data)));

// #ifndef _RELEASE
//     sc::writeLogInfo(std::string("NET_LOG_ITEM_CONVERSION LogItemConversion"));
// #endif
}

void CacheServer::MsgLogAction(DWORD dwClient, NET_MSG_GENERIC* pMsg)
{
    NET_LOG_ACTION* pPacket = (NET_LOG_ACTION*) pMsg;
    m_pDbManager->AddLogJob(
        db::DbActionPtr(
            new db::LogAction(pPacket->Data)));

// #ifndef _RELEASE
//     sc::writeLogInfo(std::string("NET_LOG_ACTION MsgLogAction"));
// #endif
}

void CacheServer::MsgLogItemRandomOption(DWORD dwClient, NET_MSG_GENERIC* pMsg)
{
    NET_LOG_RANDOM_OPTION* pPacket = (NET_LOG_RANDOM_OPTION*) pMsg;
    m_pDbManager->AddLogJob(
        db::DbActionPtr(
            new db::LogRandomItem(pPacket->Data)));

// #ifndef _RELEASE
//     sc::writeLogInfo(std::string("NET_LOG_RANDOM_OPTION MsgLogItemRandomOption"));
// #endif
}

void CacheServer::MsgLogItemAddonOption( DWORD dwClient, NET_MSG_GENERIC* pMsg )
{
	NET_LOG_ADDON_OPTION* pPacket = ( NET_LOG_ADDON_OPTION* ) pMsg;
	m_pDbManager->AddLogJob( 
		db::DbActionPtr( 
		new db::LogAddonItem( pPacket->Data ) ) );
}

void CacheServer::MsgLogItemLinkSkillOption( DWORD dwClient, NET_MSG_GENERIC* pMsg )
{
	NET_LOG_LINKSKILL_OPTION* pPacket = ( NET_LOG_LINKSKILL_OPTION* ) pMsg;
	m_pDbManager->AddLogJob( 
		db::DbActionPtr( 
		new db::LogLinkSkillItem( pPacket->Data ) ) );
}

void CacheServer::MsgLogItemBasicStatOption( DWORD dwClient, NET_MSG_GENERIC* pMsg )
{
	NET_LOG_BASICSTAT_OPTION* pPacket = ( NET_LOG_BASICSTAT_OPTION* ) pMsg;
	m_pDbManager->AddLogJob( 
		db::DbActionPtr( 
		new db::LogBasicStatItem( pPacket->Data ) ) );
}

void CacheServer::MsgLogCostumeStat(DWORD dwClient, NET_MSG_GENERIC* pMsg)
{
	NET_LOG_COSTUME_STATS* pPacket = (NET_LOG_COSTUME_STATS*) pMsg;
	m_pDbManager->AddLogJob(
		db::DbActionPtr(
		new db::LogCostumeStatItem(pPacket->Data)));

	// #ifndef _RELEASE
	//     sc::writeLogInfo(std::string("NET_LOG_RANDOM_OPTION MsgLogItemRandomOption"));
	// #endif
}

void CacheServer::MsgLogPetAction(DWORD dwClient, NET_MSG_GENERIC* pMsg)
{
    NET_LOG_PET_ACTION* pPacket = (NET_LOG_PET_ACTION*) pMsg;
    m_pDbManager->AddLogJob(
        db::DbActionPtr(
            new db::LogPetAction(pPacket->Data)));

// #ifndef _RELEASE
//     sc::writeLogInfo(std::string("NET_LOG_PET_ACTION MsgLogPetAction"));
// #endif
}

void CacheServer::MsgLogVehicleAction(DWORD dwClient, NET_MSG_GENERIC* pMsg)
{
    NET_LOG_VEHICLE_ACTION* pPacket = (NET_LOG_VEHICLE_ACTION*) pMsg;
    m_pDbManager->AddLogJob(
        db::DbActionPtr(
            new db::LogVehicleAction(pPacket->Data)));

// #ifndef _RELEASE
//     sc::writeLogInfo(std::string("NET_LOG_PET_ACTION MsgLogVehicleAction"));
// #endif
}

//
//mjeon.CaptureTheField
//
void CacheServer::MsgCTFStatLogAH(DWORD dwClient, NET_MSG_GENERIC* pMsg)
{
	GLMSG::SNET_CTF_STAT_LOG_AH *pPacket = (GLMSG::SNET_CTF_STAT_LOG_AH *) pMsg;

	 m_pDbManager->AddLogJob(
        db::DbActionPtr(
            new db::CTFStatLog(pPacket->nPlayers, pPacket->state)));
}


void CacheServer::MsgCTFResultLogAH(DWORD dwClient, NET_MSG_GENERIC* pMsg)
{
	GLMSG::SNET_CTF_REWARD_INFO *pPacket = (GLMSG::SNET_CTF_REWARD_INFO *) pMsg;

	 m_pDbManager->AddLogJob(
        db::DbActionPtr(
            new db::CTFResultLog(pPacket->wSchoolAuthed, pPacket->arrBuffs, pPacket->nHowMany)));
}

void CacheServer::MsgOddEvenLogFH(DWORD ClientSlot, NET_MSG_GENERIC* pMsg)
{
    GLMSG::NET_ODD_EVEN_LOG_FH* pPacket = (GLMSG::NET_ODD_EVEN_LOG_FH*) pMsg;
    m_pDbManager->AddLogJob(
        db::DbActionPtr(
            new db::LogMiniGameOddEvenInsert(
                pPacket->m_UserDbNum,
                pPacket->m_CharDbNum,
                pPacket->m_CurrentRound,
                pPacket->m_WinLost,
                pPacket->m_BattingMoney,
                pPacket->m_DividendMoney,
                pPacket->m_ChaMoney)));
}

void CacheServer::MsgLogAttendanceSH(DWORD ClientSlot, NET_MSG_GENERIC* pMsg)
{
    NET_MSG_PACK* pPacket = (NET_MSG_PACK*) pMsg;
    NET_LOG_ATTENDANCE Data;
    if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), Data))
    {
        sc::writeLogError("ERROR CacheServer::MsgLogAttendanceSH");
        return;
    }

    m_pDbManager->AddLogJob(
        db::DbActionPtr(
            new AttendanceLogInsert(
                Data.m_ChaNum,
                Data.m_Type,
                Data.m_LoadType,
                Data.m_DayN,
                Data.m_Status)));
    //sc::writeLogDebug("CacheServer::MsgLogAttendanceSH");
}

void CacheServer::MsgLogAttendanceTaskSH(DWORD ClientSlot, NET_MSG_GENERIC* pMsg)
{
    NET_MSG_PACK* pPacket = (NET_MSG_PACK*) pMsg;
    NET_LOG_ATTENDANCE_TASK Data;
    if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), Data))
    {
        sc::writeLogError("ERROR CacheServer::MsgLogAttendanceTaskSH");
        return;
    }

    m_pDbManager->AddLogJob(
        db::DbActionPtr(
            new AttendanceTaskLogInsert(
                Data.m_ChaNum,
				Data.m_ChaCurExp,
                Data.m_TaskN,
                Data.m_Type,
                Data.m_Status,
                Data.m_Value,
                Data.m_ValueBase,
                Data.m_RewardPoint,
                Data.m_RewardExp,
                Data.m_RewardMoney,
                Data.m_RewardItems,
                Data.m_RewardItem1,
                Data.m_RewardItem2,
                Data.m_RewardItem3,
                Data.m_TookRewards)));
    //sc::writeLogDebug("CacheServer::MsgLogAttendanceTaskSH");
}