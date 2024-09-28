#include "pch.h"
#include "../../EngineLib/Common/profile.h"
#include "../../RanLogic/Msg/TraceMsg.h"
#include "../../RanLogic/Msg/ServerMsg.h"
#include "../../RanLogic/Msg/GLContrlServerMsg.h"
#include "../../RanLogic/s_NetGlobal.h"
#include "../Database/DbAction/DbActionAuth.h"
#include "../Database/DbManager.h"
#include "../../RanLogic/Util/GlobalAuthManager.h"
#include "../Client/NetAuthClientMan.h"
#include "./AuthServerConfigXml.h"
#include "./AuthServer.h"
#include "../../SigmaCore/DebugInclude.h"

// 모든서버들에서 요청한 인증 처리
// 실제 인증 작업은 DB 프로시져에서 처리하고 DB에서 작업한 결과를 요청한 서버로 전송
void CAuthServer::MsgAuthCertificationRequest(DWORD dwClient, MSG_LIST* pMsg)
{
	if ( !pMsg )
		return;

	NET_AUTH_CERTIFICATION_REQUEST* authInfo = reinterpret_cast <NET_AUTH_CERTIFICATION_REQUEST*> (pMsg->Buffer);

	// 서버 정보 설정
	SetSvrInfo(dwClient, &(authInfo->gsi));

	ResponseAuthCertification(dwClient, pMsg);
}

// 수신된 정보를 이용하여 인증 작업 처리 
void CAuthServer::ResponseAuthCertification(DWORD dwClient, MSG_LIST* pMsg)
{
	if ( !pMsg )
		return;

	NET_AUTH_CERTIFICATION_REQUEST* authInfo = reinterpret_cast <NET_AUTH_CERTIFICATION_REQUEST*> (pMsg->Buffer);

	m_pAuthManager->CertificateAuthData(std::string(authInfo->gsi.szAuthData));

	CertificateAuthDataFromDB(dwClient);
}

// 인증결과 대상 서버로 전송
int CAuthServer::SendAuthCertificationToSessionServer(DWORD dwClient, G_AUTH_INFO &authInfo)
{
    NET_AUTH_CERTIFICATION_ANS Msg(authInfo);

	if ( SendClient(dwClient, &Msg) == NET_ERROR )
		return NET_ERROR;
		
	return NET_OK;
}

void CAuthServer::MsgCloseServer(DWORD dwClient, MSG_LIST* pMsg)
{
	if ( !pMsg )
		return;

	if ( m_pClientManager->CloseClient( dwClient ) == NET_ERROR )
	{
		sc::writeLogError(sc::string::format("Client Close ERROR : dwClinet[%1%]", dwClient));
	}
	
	return;
}

void CAuthServer::RequestSessionStateInfo()
{
	NET_AUTH_STATE_REQ svrStateMsg;

    SendAllClient(&svrStateMsg);
}

void CAuthServer::MsgAuthSessionStateResponse(DWORD dwClient, MSG_LIST* pMsg)
{
	if ( !pMsg )
		return;

	m_pClientManager->SetHeartBeat(dwClient);

	NET_AUTH_STATE_ANS* serverStateInfo = reinterpret_cast <NET_AUTH_STATE_ANS*> (pMsg->Buffer);

	if ( m_pAdoManager )
    {
		// 수신된 서버 정보 설정 ( 요구사항이 변경되어서 DB에 남길 필요가 없어서 작업 중단함)
		m_pAdoManager->AddAuthJob(db::DbActionPtr(new db::LogAuthState(serverStateInfo->gci.nCounrtyType, serverStateInfo->gci.ServerType, serverStateInfo->gci.szServerIP, serverStateInfo->gci.nServicePort, serverStateInfo->gci.nServerCurrentUser, serverStateInfo->gci.nServerMaxLoginedUser)));
		sc::writeLogInfo(sc::string::format("현재 세션서버 접속자 정보 SvrID[%d] counrty[%d] type[%d] ip[%s] port[%s] curuser[%d] maxuser[%d]",  dwClient, serverStateInfo->gci.nCounrtyType, serverStateInfo->gci.ServerType, serverStateInfo->gci.szServerIP, serverStateInfo->gci.nServicePort, serverStateInfo->gci.nServerCurrentUser, serverStateInfo->gci.nServerMaxLoginedUser));
    }
}

BOOL CAuthServer::CertificateAuthDataFromDB(DWORD dwClient)
{
	if ( m_pAdoManager )
    {
        m_pAdoManager->AddAuthJob(db::DbActionPtr( new db::CertificationAuthData( dwClient, m_pAuthManager->GetAuthData() )));
	}

	return TRUE;
}

void CAuthServer::RequestAuthEventInfo()
{
	NET_AUTH_EVENT_INFO_REQ eventInfo;

    SendAllClient(&eventInfo);
}


void CAuthServer::MsgAuthEventInfoResponse(DWORD dwClient, MSG_LIST* pMsg)
{
	if ( !pMsg )
		return;

	NET_EVENT_INFO_AUTH_ANS* eventInfo = reinterpret_cast <NET_EVENT_INFO_AUTH_ANS*> (pMsg->Buffer);

	G_AUTH_EVENT_INFO authEventInfo(eventInfo->gei);

	if ( m_pAdoManager )
    {
        m_pAdoManager->AddAuthJob(db::DbActionPtr( new db::AuthEventInfo( authEventInfo )));
	}
}

void CAuthServer::RequestServerStateInfo()
{
	if ( m_pAdoManager )
    {
        m_pAdoManager->AddAuthJob(db::DbActionPtr( new db::LogAuthServerStateInit()));
	}
}

void CAuthServer::SendRequestServerStateInfo()
{
	NET_AUTH_SERVER_STATE_REQ svrStateMsg;

	SendAllClient(&svrStateMsg);

	sc::writeLogInfo( std::string( "세션서버에 서버 접속 정보 요청 완료" ) );
}

void CAuthServer::MsgAuthServerStateResponse(DWORD dwClient, MSG_LIST* pMsg)
{
	if ( !pMsg )
		return;

	NET_AUTH_SERVER_STATE_ANS* serverStateInfo = reinterpret_cast <NET_AUTH_SERVER_STATE_ANS*> (pMsg->Buffer);

	if ( m_pAdoManager )
    {
		// 수신된 서버 정보 설정 ( 요구사항이 변경되어서 DB에 남길 필요가 없어서 작업 중단함)
		m_pAdoManager->AddAuthJob(db::DbActionPtr(new db::LogAuthServerState(serverStateInfo->gci.nSessionSvrID, serverStateInfo->gci.nCounrtyType, serverStateInfo->gci.ServerType, serverStateInfo->gci.szServerIP, serverStateInfo->gci.nServicePort)));
		sc::writeLogInfo(sc::string::format("서버 연결정보 업데이트 SvrID[%d] counrty[%d] type[%d] ip[%s] port[%s]",  dwClient, serverStateInfo->gci.nCounrtyType, serverStateInfo->gci.ServerType, serverStateInfo->gci.szServerIP, serverStateInfo->gci.nServicePort));
    }
}

int CAuthServer::SendCountryAccessApprove( DWORD dwClient, const std::vector<int>& vecCountryList )
{
	if( vecCountryList.size() == 0 )
		return NET_OK;

	sc::writeLogInfo(sc::string::format("SendCountryAccessApprove size [%d]",  vecCountryList.size()));

	NET_COUNTRY_ACCESS_APPROVE SendData;
	SendData.vecContryList = vecCountryList;

	msgpack::sbuffer PackBuffer;
	msgpack::pack( PackBuffer, SendData );

	if ( SendClient(dwClient, NET_MSG_COUNTRY_ACCESS_APPROVE, PackBuffer ) == NET_ERROR )
		return NET_ERROR;

	return NET_OK;
}
