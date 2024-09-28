#include "pch.h"

#include "../../RanLogic/Msg/JsonMsgDefine.h"
#include "../../RanLogic/Msg/GLContrlClubMsg.h"
#include "../../RanLogic/Msg/ClubMsg.h"

#include "../Client/NetAgentClientMan.h"
#include "AgentServer.h"


#include "../../SigmaCore/DebugInclude.h"

void CAgentServer::MsgCreateClub(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    GLMSG::SNET_CLUB_NEW_2AGT* pPacket = (GLMSG::SNET_CLUB_NEW_2AGT*) nmg;
    wb::EM_COUNTRY UserCountry = m_pClientManager->GetCountry(dwClient);
    DWORD GaeaID = m_pClientManager->GetGaeaID(dwClient);
    m_pGLAgentServer->MsgClubNew2Agt(dwClient, pPacket->dwChaNum, UserCountry, pPacket->szClubName, std::string(""), false);
}

void CAgentServer::MsgClubLogCA(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    // NET_MSG_CLUB_LOG_CA
    NET_MSG_PACK* pPacket = (NET_MSG_PACK*) nmg;
    NET_CLUB_LOG_CA RecvData;
    if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
        return;

    GLCharAG* pChar = m_pGLAgentServer->GetChar(dwGaeaID);
    if (!pChar)
        return;

    if (pChar->GetClubDbNum() == CLUB_NULL)
        return;

    NET_CLUB_LOG_AH SendData;
    SendData.m_LogType   = RecvData.m_LogType;
    SendData.m_Page      = RecvData.m_Page;
    SendData.m_PageSize  = RecvData.m_PageSize;
    SendData.m_ClubDbNum = pChar->GetClubDbNum();
    SendData.m_ChaDbNum  = pChar->ChaDbNum();

    msgpack::sbuffer SendBuffer;
    msgpack::pack(SendBuffer, SendData);    
    SendCache(NET_MSG_CLUB_LOG_AH, SendBuffer);
}

void CAgentServer::MsgClubLogHA(DWORD ClientSlot, DWORD GaeaID, NET_MSG_GENERIC* nmg)
{
    NET_MSG_PACK* pPacket = (NET_MSG_PACK*) nmg;    
    NET_CLUB_LOG_HA RecvData;
    if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
        return;

    GLCharAG* pChar = m_pGLAgentServer->GetCharByDbNum(RecvData.m_ChaDbNum);
    if (!pChar)
        return;

    // Send to client
    club::LOG_DATA SendData(RecvData.m_Log);
    msgpack::sbuffer SendBuffer;
    msgpack::pack(SendBuffer, SendData);    
    SendClient(pChar->ClientSlot(), NET_MSG_CLUB_LOG_AC, SendBuffer);
}

void CAgentServer::MsgClubLogHA_End(DWORD ClientSlot, DWORD GaeaID, NET_MSG_GENERIC* nmg)
{
    NET_MSG_PACK* pPacket = (NET_MSG_PACK*) nmg;    
    NET_CLUB_LOG_HA_END RecvData;
    if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
        return;
    
    GLCharAG* pChar = m_pGLAgentServer->GetCharByDbNum(RecvData.m_ChaDbNum);
    if (!pChar)
        return;

    NET_CLUB_LOG_AC_END SendData;
    SendData.m_ResultSize = RecvData.m_ResultSize;

    msgpack::sbuffer SendBuffer;
    msgpack::pack(SendBuffer, SendData);

    SendClient(pChar->ClientSlot(), NET_MSG_CLUB_LOG_AC_END, SendBuffer);
}

void CAgentServer::MsgClubMemberGradeChangeCA(DWORD ClientSlot, DWORD GaeaID, NET_MSG_GENERIC* pMsg)
{
    GLCharAG* pChar = m_pGLAgentServer->GetChar(GaeaID);
    if (!pChar)
        return;

    NET_MSG_PACK* pPacket = (NET_MSG_PACK*) pMsg;
    NET_CLUB_MEMBER_GRADE_CHANGE_CA RecvData;
    if (msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))    
        m_pGLAgentServer->ClubMemberGradeChange(pChar, RecvData.m_MemberDbNum, RecvData.m_Index);
}

void CAgentServer::MsgClubMemberCdCertifyCA(DWORD ClientSlot, DWORD GaeaID, const json_spirit::mObject& Obj)
{
    GLCharAG* pChar = m_pGLAgentServer->GetChar(GaeaID);
    if (!pChar)
        return;

    DWORD MemberDbNum = 0;
    sc::Json::find_value(Obj, "C", MemberDbNum);
    bool bCd = false;
    sc::Json::find_value(Obj, "D", bCd);

    m_pGLAgentServer->ClubCdCertifyChange(pChar, MemberDbNum, bCd);
}

void CAgentServer::MsgClubMemberLobbyCA(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    GLMSG::NET_CLUB_MENBER_LOBBY_CA* pPacket = (GLMSG::NET_CLUB_MENBER_LOBBY_CA*) nmg;
    if (!m_pGLAgentServer || pPacket->m_Guid != msg_guid::GUID1)
        return;        
    
    m_pGLAgentServer->MsgClubMemberAddLobby(pPacket->m_ClubDbNum, dwClient, pPacket->m_ChaDbNum, pPacket->m_ChaName, pPacket->m_ChaClass);
}

void CAgentServer::MsgClubNewLobbyCA(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    GLMSG::NET_CLUB_NEW_LOBBY_CA* pPacket = (GLMSG::NET_CLUB_NEW_LOBBY_CA*) nmg;
    if (pPacket->m_Guid != msg_guid::GUID2)
        return;

    if (dwGaeaID != GAEAID_NULL)
        return;
    
    m_pGLAgentServer->MsgClubNew2Agt(
        dwClient,
        pPacket->m_ChaDbNum,
        m_pClientManager->GetCountry(dwClient),
        pPacket->m_ClubName,
        std::string(pPacket->m_ChaName),
        true);
}

void CAgentServer::MsgClubGradeNameChangeCA(DWORD ClientSlot, DWORD GaeaID, NET_MSG_GENERIC* pMsg)
{
    GLCharAG* pChar = m_pGLAgentServer->GetChar(GaeaID);
    if (!pChar)
        return;

    NET_MSG_PACK* pPacket = (NET_MSG_PACK*) pMsg;
    NET_CLUB_GRADE_NAME_CHANGE_CA RecvData;
    if (msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))    
        m_pGLAgentServer->ClubGradeNameChange(pChar, RecvData.m_Index, RecvData.m_GradeName);
}