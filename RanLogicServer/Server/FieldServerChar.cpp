#include "../pch.h"
#include "../../RanLogic/Msg/GLContrlMsg.h"
#include "../Net/NetLogDefine.h"
#include "../FieldServer/GLGaeaServer.h"
#include "../Character/GLChar.h"
#include "../Client/NetFieldClientMan.h"
#include "./s_CFieldServer.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void CFieldServer::MsgChaExtraInfoHF(NET_MSG_GENERIC* nmg)
{
	GASSERT(GetServerType() == net::SERVER_FIELD);
	
	GLMSG::SNET_GET_CHA_EXTRA_INFO_FH_FB *msg = (GLMSG::SNET_GET_CHA_EXTRA_INFO_FH_FB *)nmg;
	
	GLChar* pChar = m_pGaeaServer->GetCharByDbNum(msg->dwChaNum);

	if (pChar)
	{
		pChar->MsgChaExtraInfoHF(msg);
	}
	else
	{
		sc::writeLogError(sc::string::format("CFieldServer::MsgChaExtraInfoHF() - The character(%1%) does not exist.", msg->dwChaNum));
	}
}

void CFieldServer::LogAttendanceDS(DWORD ClientSlot, NET_MSG_GENERIC* pMsg)
{
    NET_MSG_PACK* pPacket = (NET_MSG_PACK*) pMsg;
    NET_LOG_ATTENDANCE DataDS;
    if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), DataDS))
    {
        sc::writeLogDebug("ERROR CAgentServer::MsgLogAttendanceDS");
        return;
    }

    msgpack::sbuffer BufferAH;
    msgpack::pack(BufferAH, DataDS);

    SendCache(NET_LOG_ATTENDANCE_SH, BufferAH);
}

void CFieldServer::LogAttendanceTaskDS(DWORD ClientSlot, NET_MSG_GENERIC* pMsg)
{
    NET_MSG_PACK* pPacket = (NET_MSG_PACK*) pMsg;
    NET_LOG_ATTENDANCE_TASK DataDS;
    if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), DataDS))
    {
        sc::writeLogDebug("ERROR CAgentServer::MsgLogAttendanceDS");
        return;
    }

    msgpack::sbuffer BufferAH;
    msgpack::pack(BufferAH, DataDS);

    SendCache(NET_LOG_ATTENDANCE_TASK_SH, BufferAH);
}