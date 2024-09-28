#include "pch.h"

#include "../InstanceSystem/LogicAgent/InstanceSystemAgent.h"
#include "GLAgentServer.h"

//추가된 인던 컨텐츠 Include;
#include "../InstanceSystem/Contents/ClubDeathMatch/RnClubDeathMatch.h"

BOOL GLAgentServer::MessageInstanceSystemProc ( NET_MSG_GENERIC* pMsg, DWORD dwClientID, DWORD dwGaeaID )
{
	if ( NULL == m_pInstanceManager )
		return FALSE;

	return m_pInstanceManager->OnMessageProc(
		static_cast< GLMSG::NET_INSTANCE_MESSAGE_HEADER* >( pMsg ),
		dwGaeaID ) ? TRUE : FALSE;
}

BOOL GLAgentServer::MessageInstanceSystem(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
	return m_pInstanceManager->messageProcedure(nmg, dwGaeaID) ? TRUE : FALSE;
}

BOOL GLAgentServer::MessageInstanceSystemMsgPack(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
    return m_pInstanceManager->messageProcedureMsgPack(nmg, dwGaeaID) ? TRUE : FALSE;
}

//추가된 인던 컨텐츠의 메시지 처리;
BOOL GLAgentServer::MessageInstanceSystemContents(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
    return FALSE;
}
BOOL GLAgentServer::MessageInstanceSystemContentsMsgPack(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
    GLMSG::NET_INSTANCE_CONTENTS_MSGPACK_MESSAGE headerPacket;
    if ( CastNetMsg(nmg, headerPacket) == false )
        return FALSE;

    switch ( headerPacket.emType )
    {
    case InstanceSystem::EMMSG_RNCDM_PREVIOUS_INFO_FAC:
        {
            GLMSG::NET_INSTANCE_RNCDM_PREVIOUS_INFO_FAC netMsg;

            if ( CastNetMsg(nmg, netMsg) == false )
                return FALSE;

            InstanceSystem::gPreviousCDM_Info.previous_StartTime = netMsg.tStartTime;
            InstanceSystem::gPreviousCDM_Info.previous_EndTime = netMsg.tEndTime;
            InstanceSystem::gPreviousCDM_Info.vecPrevious_ClubDbNum = netMsg.vecClubDbNum;
            InstanceSystem::gPreviousCDM_Info.vecPrevious_Rank = netMsg.vecClubRank;

            return TRUE;
        }
        break;
    }

    return FALSE;
}