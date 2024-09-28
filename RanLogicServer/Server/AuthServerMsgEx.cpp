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

// ��缭���鿡�� ��û�� ���� ó��
// ���� ���� �۾��� DB ���ν������� ó���ϰ� DB���� �۾��� ����� ��û�� ������ ����
void CAuthServer::MsgAuthCertificationRequest(DWORD dwClient, MSG_LIST* pMsg)
{
	if ( !pMsg )
		return;

	NET_AUTH_CERTIFICATION_REQUEST* authInfo = reinterpret_cast <NET_AUTH_CERTIFICATION_REQUEST*> (pMsg->Buffer);

	// ���� ���� ����
	SetSvrInfo(dwClient, &(authInfo->gsi));

	ResponseAuthCertification(dwClient, pMsg);
}

// ���ŵ� ������ �̿��Ͽ� ���� �۾� ó�� 
void CAuthServer::ResponseAuthCertification(DWORD dwClient, MSG_LIST* pMsg)
{
	if ( !pMsg )
		return;

	NET_AUTH_CERTIFICATION_REQUEST* authInfo = reinterpret_cast <NET_AUTH_CERTIFICATION_REQUEST*> (pMsg->Buffer);

	m_pAuthManager->CertificateAuthData(std::string(authInfo->gsi.szAuthData));

	CertificateAuthDataFromDB(dwClient);
}

// ������� ��� ������ ����
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
		// ���ŵ� ���� ���� ���� ( �䱸������ ����Ǿ DB�� ���� �ʿ䰡 ��� �۾� �ߴ���)
		m_pAdoManager->AddAuthJob(db::DbActionPtr(new db::LogAuthState(serverStateInfo->gci.nCounrtyType, serverStateInfo->gci.ServerType, serverStateInfo->gci.szServerIP, serverStateInfo->gci.nServicePort, serverStateInfo->gci.nServerCurrentUser, serverStateInfo->gci.nServerMaxLoginedUser)));
		sc::writeLogInfo(sc::string::format("���� ���Ǽ��� ������ ���� SvrID[%d] counrty[%d] type[%d] ip[%s] port[%s] curuser[%d] maxuser[%d]",  dwClient, serverStateInfo->gci.nCounrtyType, serverStateInfo->gci.ServerType, serverStateInfo->gci.szServerIP, serverStateInfo->gci.nServicePort, serverStateInfo->gci.nServerCurrentUser, serverStateInfo->gci.nServerMaxLoginedUser));
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

	sc::writeLogInfo( std::string( "���Ǽ����� ���� ���� ���� ��û �Ϸ�" ) );
}

void CAuthServer::MsgAuthServerStateResponse(DWORD dwClient, MSG_LIST* pMsg)
{
	if ( !pMsg )
		return;

	NET_AUTH_SERVER_STATE_ANS* serverStateInfo = reinterpret_cast <NET_AUTH_SERVER_STATE_ANS*> (pMsg->Buffer);

	if ( m_pAdoManager )
    {
		// ���ŵ� ���� ���� ���� ( �䱸������ ����Ǿ DB�� ���� �ʿ䰡 ��� �۾� �ߴ���)
		m_pAdoManager->AddAuthJob(db::DbActionPtr(new db::LogAuthServerState(serverStateInfo->gci.nSessionSvrID, serverStateInfo->gci.nCounrtyType, serverStateInfo->gci.ServerType, serverStateInfo->gci.szServerIP, serverStateInfo->gci.nServicePort)));
		sc::writeLogInfo(sc::string::format("���� �������� ������Ʈ SvrID[%d] counrty[%d] type[%d] ip[%s] port[%s]",  dwClient, serverStateInfo->gci.nCounrtyType, serverStateInfo->gci.ServerType, serverStateInfo->gci.szServerIP, serverStateInfo->gci.nServicePort));
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
