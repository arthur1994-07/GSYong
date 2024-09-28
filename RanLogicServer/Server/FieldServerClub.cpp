#include "pch.h"
#include "../../RanLogic/Msg/ClubMsg.h"

#include "../FieldServer/GLGaeaServer.h"
#include "s_CFieldServer.h"


#include "../../SigmaCore/DebugInclude.h"


void CFieldServer::ClubMemberGradeChange(DWORD ClientSlot, NET_MSG_GENERIC* pMsg)
{
    if (m_dwAgentSlot != ClientSlot)
        return;

    NET_MSG_PACK* pPacket = (NET_MSG_PACK*) pMsg;
    NET_CLUB_MEMBER_GRADE_CHANGE_AF RecvData;
    if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
        return;
    
    if (m_pGaeaServer)
        m_pGaeaServer->ClubMemberGradeChange(RecvData.m_ClubDbNum, RecvData.m_MemberDbNum, RecvData.m_Index);
}

void CFieldServer::ClubGradeNameChangeAF(DWORD ClientSlot, NET_MSG_GENERIC* pMsg)
{
    if (m_dwAgentSlot != ClientSlot)
        return;

    NET_MSG_PACK* pPacket = (NET_MSG_PACK*) pMsg;
    NET_CLUB_GRADE_NAME_CHANGE_AF RecvData;
    if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
        return;
    
    if (m_pGaeaServer)
        m_pGaeaServer->ClubGradeNameChange(RecvData.m_ClubDbNum, RecvData.m_Index, RecvData.m_GradeName);
}