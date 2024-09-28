#include "../pch.h"
#include "../../EngineLib/Common/profile.h"
#include "../../RanLogic/Msg/TraceMsg.h"
#include "../../RanLogic/Msg/ServerMsg.h"
#include "../../RanLogic/s_NetGlobal.h"

#include "../Client/NetSessionClientMan.h"
#include "./SessionServer.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void CSessionServer::initPacketFunc()
{	
	m_MsgFunc[NET_MSG_HEARTBEAT_SERVER_ANS].Msgfunc			= &CSessionServer::MsgHeartbeatServerAns;
	m_MsgFunc[NET_MSG_HEARTBEAT_SERVER_REQ].Msgfunc			= &CSessionServer::MsgServerHeartbeatReq;
	m_MsgFunc[NET_MSG_SND_FULL_SVR_INFO].Msgfunc			= &CSessionServer::MsgServerInfo;
	m_MsgFunc[NET_MSG_REQ_ALL_SVR_INFO].Msgfunc				= &CSessionServer::MsgSndAllSvrInfo;
	m_MsgFunc[NET_MSG_REQ_SVR_INFO].Msgfunc					= &CSessionServer::MsgSndCurSvrInfo;
	m_MsgFunc[NET_MSG_SND_CUR_STATE].Msgfunc				= &CSessionServer::MsgServerCurInfo;
	m_MsgFunc[NET_MSG_SND_CHANNEL_STATE].Msgfunc			= &CSessionServer::MsgServerChannelInfo;
	m_MsgFunc[NET_MSG_GAME_JOIN_OK].Msgfunc					= &CSessionServer::MsgGameJoinOK;
	m_MsgFunc[NET_MSG_CHAT_CTRL].Msgfunc					= &CSessionServer::MsgChatProcess;
	m_MsgFunc[NET_MSG_CHAT_CTRL_SVRMANAGER].Msgfunc			= &CSessionServer::MsgChatProcess;
	m_MsgFunc[NET_MSG_SVR_CMD].Msgfunc						= &CSessionServer::MsgSvrCmdProcess;
	m_MsgFunc[NET_MSG_SND_ENCRYPT_KEY].Msgfunc				= &CSessionServer::MsgEncryptKey;
	m_MsgFunc[NET_MSG_LOGIN_MAINTENANCE_SESSION].Msgfunc	= &CSessionServer::MsgLoginMaintenance;
    m_MsgFunc[NET_MSG_PS_RELOAD_AS].Msgfunc	                = &CSessionServer::MsgPsReloadAS;
    m_MsgFunc[NET_MSG_CLOSE_CLIENT_SESSION].Msgfunc	        = &CSessionServer::MsgCloseClient;
    m_MsgFunc[NET_MSG_VERSION_LS].Msgfunc	                = &CSessionServer::MsgVersionLS;
    m_MsgFunc[NET_MSG_GM_LOG_ITEM_RELOAD_AS].Msgfunc	    = &CSessionServer::MsgGmLogItemReloadAS;
    m_MsgFunc[NET_MSG_GM_GAME_DATA_UPDATE_AS].Msgfunc       = &CSessionServer::MsgGmGameDataUpdateAS;
    m_MsgFunc[NET_MSG_EVENT_EXP_MS].Msgfunc                 = &CSessionServer::MsgEventExpMS;
    m_MsgFunc[NET_MSG_EVENT_EXP_END_MS].Msgfunc             = &CSessionServer::MsgEventExpEndMS;
	m_MsgFunc[NET_MSG_AUTH_CERTIFICATION_REQUEST].Msgfunc	= &CSessionServer::MsgAuthCertificationRequest;
	m_MsgFunc[NET_MSG_AUTH_EVENT_INFO_ANS].Msgfunc          = &CSessionServer::MsgAuthEventInfoResponse;
	m_MsgFunc[NET_MSG_REQ_AUTH_SVR_INFO].Msgfunc			= &CSessionServer::MsgSendAuthServerInfo;

    m_MsgFunc[NET_MSG_INSPECTION_REWARD_SET_REQ].Msgfunc	= &CSessionServer::MsgServerMansgerMsg;
	m_MsgFunc[NET_MSG_INSPECTION_REWARD_SET_ACK].Msgfunc	= &CSessionServer::MsgServerMansgerMsg;

    
	
//#if defined(_RELEASED) || defined(TW_PARAM) || defined(TH_PARAM) || defined(HK_PARAM) 	// ***Tracing Log print
	//if (m_ServiceProvider == SP_OFFICE_TEST || m_ServiceProvider == SP_TAIWAN || m_ServiceProvider == SP_HONGKONG || m_ServiceProvider == SP_THAILAND)
	//{
		m_MsgFunc[NET_MSG_UPDATE_TRACING_ALL].Msgfunc		= &CSessionServer::MsgTracingCharAll;
		m_MsgFunc[NET_MSG_UPDATE_TRACING_CHARACTER].Msgfunc	= &CSessionServer::MsgTracingChar;
	//}
//#endif
}

int CSessionServer::MsgProcess(MSG_LIST* pMsg)
{	
    if (!pMsg)
        return NET_ERROR;

	NET_MSG_GENERIC* pPacket = reinterpret_cast<NET_MSG_GENERIC*> (pMsg->Buffer);
	DWORD dwClient = pMsg->dwClient;

	//	MsgProfile
	PROFILE_MSG_COUNT_RECV( pPacket->nType, pPacket->dwSize );

	if (dwClient == m_nAuthServerSlot)
	{
		MsgProcessForAuth(pMsg);
	}
	else
	{
		EMNET_MSG emType = pPacket->nType;
		if (emType < 0 || emType >= NET_MSG_GCTRL_MAX || NULL == m_MsgFunc[emType].Msgfunc)
		{
			sc::writeLogError( sc::string::format( "CSessionServer::MsgProcess Out of range or illegal message:%d", emType ) );
		}
		else
		{
			(this->*m_MsgFunc[emType].Msgfunc )( dwClient, pMsg);
		}
	}

	return 0;
}
