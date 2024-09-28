#include "pch.h"
#include "../../SigmaCore/Json/MinJson.h"
#include "../../RanLogic/Msg/JsonMsgDefine.h"
#include "../../RanLogic/Msg/ClubMsg.h"

#include "../Database/DBAction/DbActionGameClubLog.h"
#include "../Database/DbManager.h"
#include "../Club/ClubLogMan.h"
#include "./CacheServer.h"

#include "../../RanLogic/Msg/GLContrlMsg.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void CacheServer::MsgClubLog(DWORD ClientSlot, NET_MSG_GENERIC* pMsg)
{
    NET_MSG_PACK* pPacket = (NET_MSG_PACK*) pMsg;
    NET_CLUB_LOG RecvData;
    if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
        return;

    //m_pClubLog->Insert(RecvData.m_ClubDbNum, static_cast<club::EM_LOG_TYPE> (RecvData.m_Type), RecvData.m_LogMsg);
    m_pDbManager->AddClubLogDbJob(
        db::DbActionPtr(
            new db::ClubLogInsert(
                ClientSlot,
                RecvData.m_ClubDbNum,
                static_cast<club::EM_LOG_TYPE> (RecvData.m_Type), 
                RecvData.m_LogMsg)));
}

//! Agent->Cache Club log request
void CacheServer::MsgClubLogAH(DWORD ClientSlot, NET_MSG_GENERIC* pMsg)
{
    // NET_MSG_CLUB_LOG_AH
    NET_MSG_PACK* pPacket = (NET_MSG_PACK*) pMsg;
    NET_CLUB_LOG_AH RecvData;
    if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
        return;

    m_pDbManager->AddClubLogDbJob(
        db::DbActionPtr(
            new db::ClubLogSearch(
                ClientSlot,
                RecvData.m_ChaDbNum,
                RecvData.m_ClubDbNum,
                RecvData.m_Page,
                RecvData.m_PageSize,
                static_cast<club::EM_LOG_TYPE> (RecvData.m_LogType))));
}

void CacheServer::ClubLogCheck()
{
    //m_pClubLog->DeleteOneDayData(club::ClubLogDeleteTime);
    m_pDbManager->AddClubLogDbJob(
        db::DbActionPtr(
            new db::ClubLogDelete(0)));
}

void CacheServer::MsgClubLogDh(DWORD ClientSlot, NET_MSG_GENERIC* pMsg)
{
    NET_MSG_PACK* pPacket = (NET_MSG_PACK*) pMsg;
    NET_CLUB_LOG_DH RecvData;
    if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
        return;

    NET_CLUB_LOG_HA SendData;
    SendData.m_Log      = RecvData.m_Log;
    SendData.m_ChaDbNum = RecvData.m_ChaDbNum;

    msgpack::sbuffer SendBuffer;
    msgpack::pack(SendBuffer, SendData);

    SendClient(RecvData.m_ClientSlot, NET_MSG_CLUB_LOG_HA, SendBuffer);
}

void CacheServer::MsgClubLogDhEnd(DWORD ClientSlot, NET_MSG_GENERIC* pMsg)
{
    NET_MSG_PACK* pPacket = (NET_MSG_PACK*) pMsg;
    NET_CLUB_LOG_DH_END RecvData;
    if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
        return;

    NET_CLUB_LOG_HA_END SendDataEnd;
    SendDataEnd.m_ChaDbNum   = RecvData.m_ChaDbNum;
    SendDataEnd.m_ResultSize = RecvData.m_ResultSize;

    msgpack::sbuffer SendBufferEnd;
    msgpack::pack(SendBufferEnd, SendDataEnd);

    SendClient(RecvData.m_ClientSlot, NET_MSG_CLUB_LOG_HA_END, SendBufferEnd);
}