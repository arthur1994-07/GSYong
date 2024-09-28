#include "pch.h"
#include "../../../SigmaCore/Database/DbDefine.h"
#include "../../../RanLogic/Msg/ClubMsg.h"
#include "../../Club/ClubLogMan.h"
#include "../../Server/CacheServer.h"
#include "./DbActionGameClubLog.h"


// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


namespace db
{

ClubLogInsert::ClubLogInsert(
    DWORD ClientSlot,
    DWORD ClubDbNum,
    club::EM_LOG_TYPE LogType,
    const std::string& LogMsg)
    : DbAction(CLUB_LOG_DB, ClientSlot)
    , m_ClubDbNum(ClubDbNum)
    , m_LogType(LogType)
    , m_LogMsg(LogMsg)
{
}

ClubLogInsert::~ClubLogInsert()
{
}

int ClubLogInsert::Execute(NetServer* pServer)
{
    if (!pServer)
        return sc::db::DB_ERROR;

    CacheServer* pCacheServer = reinterpret_cast<CacheServer*> (pServer);
    if (!pCacheServer)
        return sc::db::DB_ERROR;

    club::LogManServer* pClubLogMan = pCacheServer->GetClibLogMan();
    if (!pClubLogMan)
        return sc::db::DB_ERROR;

    pClubLogMan->Insert(m_ClubDbNum, m_LogType, m_LogMsg);
    return sc::db::DB_OK;
}

ClubLogDelete::ClubLogDelete(const __time64_t& DeleteTime)
    : DbAction(CLUB_LOG_DB)
    , m_DeleteTime(DeleteTime)
{
}

ClubLogDelete::~ClubLogDelete()
{
}

int ClubLogDelete::Execute(NetServer* pServer)
{
    if (!pServer)
        return sc::db::DB_ERROR;

    CacheServer* pCacheServer = reinterpret_cast<CacheServer*> (pServer);
    if (!pCacheServer)
        return sc::db::DB_ERROR;

    club::LogManServer* pClubLogMan = pCacheServer->GetClibLogMan();
    if (!pClubLogMan)
        return sc::db::DB_ERROR;

    pClubLogMan->DeleteOneDayData(m_DeleteTime);
    return sc::db::DB_OK;
}

ClubLogSearch::ClubLogSearch(
    DWORD ClientSlot,
    DWORD ChaDbNum,
    DWORD ClubDbNum,
    WORD Page,
    WORD PageSize,
    club::EM_LOG_TYPE LogType)
    : DbAction(CLUB_LOG_DB, ClientSlot)
    , m_ChaDbNum(ChaDbNum)
    , m_ClubDbNum(ClubDbNum)
    , m_Page(Page)
    , m_PageSize(PageSize)
    , m_LogType(LogType)
{
}

ClubLogSearch::~ClubLogSearch()
{
}

int ClubLogSearch::Execute(NetServer* pServer)
{
    if (!pServer)
        return sc::db::DB_ERROR;

    CacheServer* pCacheServer = reinterpret_cast<CacheServer*> (pServer);
    if (!pCacheServer)
        return sc::db::DB_ERROR;

    club::LogManServer* pClubLogMan = pCacheServer->GetClibLogMan();
    if (!pClubLogMan)
        return sc::db::DB_ERROR;

    std::vector<club::LOG_DATA> Result;
    pClubLogMan->Search(
        m_ClubDbNum,
        Result,
        m_Page,
        m_PageSize,
        m_LogType);
    for (size_t i=0; i<Result.size(); ++i)
    {
        NET_CLUB_LOG_DH SendData(m_ClientSlot, m_ChaDbNum, Result[i]);
        
        msgpack::sbuffer SendBuffer;
        msgpack::pack(SendBuffer, SendData);
    
        NET_MSG_PACK MsgPack;
        MsgPack.SetData(NET_MSG_CLUB_LOG_DH, SendBuffer);

        pCacheServer->InsertMsg(0, &MsgPack);
    }

    NET_CLUB_LOG_DH_END SendDataEnd(m_ClientSlot, m_ChaDbNum, Result.size());

    msgpack::sbuffer SendBufferEnd;
    msgpack::pack(SendBufferEnd, SendDataEnd);

    NET_MSG_PACK MsgPackEnd;
    MsgPackEnd.SetData(NET_MSG_CLUB_LOG_DH_END, SendBufferEnd);

    pCacheServer->InsertMsg(0, &MsgPackEnd);

    return sc::db::DB_OK;
}


} // namespace db