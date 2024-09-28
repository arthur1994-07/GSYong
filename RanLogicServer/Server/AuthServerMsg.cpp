#include "pch.h"
#include "../../EngineLib/Common/profile.h"
#include "../../RanLogic/Msg/TraceMsg.h"
#include "../../RanLogic/Msg/ServerMsg.h"
#include "../../RanLogic/s_NetGlobal.h"
#include "../Client/NetAuthClientMan.h"
#include "./AuthServer.h"
#include "../../SigmaCore/DebugInclude.h"

void CAuthServer::initPacketFunc()
{	
	m_MsgFunc[NET_MSG_AUTH_CERTIFICATION_REQUEST].Msgfunc = &CAuthServer::MsgAuthCertificationRequest;
	m_MsgFunc[NET_MSG_CLOSE_CLIENT_AUTH].Msgfunc = &CAuthServer::MsgCloseServer;
	m_MsgFunc[NET_MSG_AUTH_STATE_ANS].Msgfunc = &CAuthServer::MsgAuthSessionStateResponse;
	m_MsgFunc[NET_MSG_AUTH_EVENT_INFO_ANS].Msgfunc = &CAuthServer::MsgAuthEventInfoResponse;
	m_MsgFunc[NET_MSG_AUTH_SERVER_STATE_ANS].Msgfunc = &CAuthServer::MsgAuthServerStateResponse;
}

int CAuthServer::MsgProcess(MSG_LIST* pMsg)
{	
    if ( !pMsg )
        return NET_ERROR;

	NET_MSG_GENERIC* pPacket = reinterpret_cast<NET_MSG_GENERIC*> (pMsg->Buffer);
	DWORD dwClient = pMsg->dwClient;

	//	MsgProfile
	PROFILE_MSG_COUNT_RECV(pPacket->nType, pPacket->dwSize);
	EMNET_MSG emType = pPacket->nType;

	if ( emType < 0 || emType >= NET_MSG_GCTRL_MAX || NULL == m_MsgFunc[emType].Msgfunc )
	{
		sc::writeLogError( sc::string::format( "MSG Protocol is not defined :%d", emType ) );
	}
	else
	{
		(this->*m_MsgFunc[emType].Msgfunc )( dwClient, pMsg);
	}

	return 0;
}
