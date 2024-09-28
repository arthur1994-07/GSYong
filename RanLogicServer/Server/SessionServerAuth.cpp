#include "pch.h"

#include "../../EngineLib/Common/profile.h"
#include "../../RanLogic/Msg/ServerMsg.h"
#include "../Client/NetSessionClientMan.h"
#include "./NetServerHelper.h"
#include "./SessionServerConfigXml.h"
#include "./SessionServer.h"
#include "../../RanLogic/Util/GlobalAuthManager.h"


// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

// 인증서버 ->  세션서버로 전송되는 메세지
void CSessionServer::MsgProcessForAuth(MSG_LIST* pMsg)
{	
    if ( !pMsg )
        return;

    NET_MSG_GENERIC* pPacket = reinterpret_cast<NET_MSG_GENERIC*> (pMsg->Buffer);

    switch (pPacket->nType)
    {
	case NET_MSG_AUTH_CERTIFICATION_ANS:
		MsgAuthCertificationResponse(pPacket);
		break;
	case NET_MSG_AUTH_STATE_REQ:
		MsgAuthSessionStateRequest();
		break;
	case NET_MSG_AUTH_EVENT_INFO_REQ:
		MsgAuthEventInfoRequest(pPacket);
		break;
	case NET_MSG_AUTH_SERVER_STATE_REQ:
		MsgAuthServerStateRequest();
		break;
	case NET_MSG_COUNTRY_ACCESS_APPROVE:
		CountryAccessApprove( pPacket );
		break;
    default:
        sc::writeLogInfo(sc::string::format("Unknown Msg Type from Auth Server %1%", pPacket->nType));
        break;
    }
}

int CSessionServer::ConnectAuthServer()
{
	return ConnectAuthServer(m_pConfig->GetAuthServerIP(), m_pConfig->GetAuthServerPort());
}

void CSessionServer::CheckAuthServerSocket()
{
	sc::writeLogInfo( sc::string::format("AUTH STATE : Reconnect %1%, slot %2%, stat %3%", m_bReconnect, m_nAuthServerSlot, m_pClientManager->GetSocket(m_nAuthServerSlot)) );

	/*if ( m_nAuthServerSlot == net::INVALID_SLOT || INVALID_SOCKET == m_pClientManager->GetSocket(m_nAuthServerSlot) )
    {	
		sc::writeLogError( sc::string::format("Auth Server Socket ERROR : reconnect %1%, slot %2%, stat %3%", m_bReconnect, m_nAuthServerSlot, m_pClientManager->GetSocket(m_nAuthServerSlot)) );
    }*/
}

bool CSessionServer::CertificationInternalAuth()
{
	return m_pAuthManager->CertificationInternalAuth();
}

int	CSessionServer::ConnectAuthServer(const char* szServerIP, int nPort)
{
    if ( m_nAuthServerSlot != net::INVALID_SLOT && INVALID_SOCKET != m_pClientManager->GetSocket(m_nAuthServerSlot) )
    {	
        sc::writeLogInfo( std::string( "AuthServer is online" ) );
        return NET_OK;
    }

    int nAuthServerSlot = m_pClientManager->GetFreeClientID( true );

	if ( nAuthServerSlot == NET_ERROR )
	{
		sc::writeLogError( std::string( "GetFreeClientID ERROR ( Don't have free network slot" ) );
		return NET_ERROR;
	}

    net::CONNECT_RESULT Connect = net::ConnectAndReceive( szServerIP, nPort, m_hIOServer, nAuthServerSlot );

    int Result = Connect.get<0> ();

    SOCKET ConnectSocket = Connect.get<1> ();

    if ( Result == NET_ERROR )
    {
        m_pClientManager->ReleaseClientID( nAuthServerSlot );
        sc::writeLogError( std::string( "Auth Server Connection Failed" ) );
        return NET_ERROR;
    }  

	if ( NET_ERROR == m_pClientManager->SetAcceptedClient( nAuthServerSlot, ConnectSocket, net::SLOT_AUTH ) )
	{
		::closesocket( ConnectSocket );
		m_pClientManager->ReleaseClientID( nAuthServerSlot );
		sc::writeLogError( std::string( "ConnectAuthServer SetAcceptedClient failed" ) );
		return NET_ERROR;
	}

    m_nAuthServerSlot = nAuthServerSlot;

    m_pClientManager->SetSlotType( m_nAuthServerSlot, net::SLOT_AUTH );

	PER_IO_OPERATION_DATA* pIOData = m_pClientManager->GetFreeOverIO( NET_RECV_POSTED );
	if ( NULL == pIOData )
	{
		CloseClient( nAuthServerSlot );
		sc::writeLogError( std::string( "ConnectAuthServer GetFreeOverIO failed" ) );
		return NET_ERROR;
	}

	m_pClientManager->AddReference( nAuthServerSlot );

	DWORD Flags = 0;
	DWORD dwRcvBytes = sizeof( NET_MSG_GENERIC );
	int nRetCode = WSARecv( ConnectSocket, &( pIOData->DataBuf ), 1, &dwRcvBytes, &Flags, &( pIOData->Overlapped ), NULL );
	if ( nRetCode == SOCKET_ERROR && ( WSAGetLastError() != ERROR_IO_PENDING ) )
	{
		m_pClientManager->ReleaseReference( nAuthServerSlot );
		m_pClientManager->ReleaseOperationData( pIOData );
		CloseClient( nAuthServerSlot );
		sc::writeLogError( sc::string::format( "ConnectAuthServer WSARecv %1%", WSAGetLastError() ) );
		return NET_ERROR;
	}

	m_bConnectedAuthServer = true;

    sc::writeLogInfo( sc::string::format( "Auth Server Connection OK. slot %1%", m_nAuthServerSlot ) );

    return NET_OK;
}

int CSessionServer::SendAuthInfoToAuthServer()
{
	if( !m_bConnectedAuthServer ) return NET_ERROR; 

    G_AUTH_INFO authInfo;

	authInfo.nCounrtyType = m_pConfig->GetServiceProvider();
    authInfo.ServerType = m_pConfig->GetServerType();
    authInfo.nServicePort = m_pConfig->GetServicePort();	
    authInfo.SetServerIp(m_szAddress);
	authInfo.nSessionSvrID = 0;

	m_pAuthManager->SetServerPort(m_pConfig->GetServicePort());
	m_pAuthManager->SetServerType(m_pConfig->GetServerType());
	m_pAuthManager->SetServiceProvider(m_pConfig->GetServiceProvider());
	m_pAuthManager->SetServerIP(std::string(m_szAddress));

	authInfo.SetAuthData(m_pAuthManager->GetEncryptedAuthData().c_str());

    NET_AUTH_CERTIFICATION_REQUEST Msg(authInfo);
	
	if (SendAuthServer(&Msg, Msg.Size()) == NET_ERROR)
	{
		sc::writeLogError(std::string("SendAuthInfoToAuthServer NET_ERROR"));		
		return NET_ERROR;
	}

	return NET_OK;
}

// 인증서버에서 세션서버로 전달된 인증 결과
// 만약 세션서버에서 요청한 인증결과라면 결과에 따른 처리를 수행하고
// 다른 서버에서 요청한 인증결과 라면 릴레이 시킨다
void CSessionServer::MsgAuthCertificationResponse(NET_MSG_GENERIC* nmg)
{   
    if ( nmg == NULL ) 
		return;

	NET_AUTH_CERTIFICATION_ANS* nsm = reinterpret_cast<NET_AUTH_CERTIFICATION_ANS*> (nmg);

	// 인증을 요청한 서버가 세션서버가 아니라면 해당 서버를 찾아서 릴레이
	if ( nsm->gsi.ServerType != net::SERVER_SESSION )
	{
		//sc::writeLogInfo(sc::string::format("기타서버로 인증 결과 릴레이 대상 정보 SetSvrInfo IP[%s] Port[%d] Country[%d] Key[%s] SvrType[%d]", nsm->gsi.szServerIP, nsm->gsi.nServicePort, nsm->gsi.nCounrtyType, nsm->gsi.szAuthData, nsm->gsi.ServerType)); 

		int nServerClientSlot = FindServer(CString(nsm->gsi.szServerIP), nsm->gsi.nServicePort );

		if ( nServerClientSlot != NET_ERROR )
		{
			SendClient(nServerClientSlot, nmg);
		}
		else
		{
			sc::writeLogInfo(std::string("Certification relay ERROR to other server"));
		}		
	}
	else
	{
		// 인증서버에서 요청한 결과라면 결과 확인후 처리
		m_pAuthManager->ConfirmAuthData(std::string(nsm->gsi.szAuthData));
		//sc::writeLogInfo(sc::string::format("인증 결과 확인 RET[%d] IP[%s] Port[%d] Country[%d] SvrType[%d] Key[%s] ", m_pAuthManager->GetAuthData()->nCertification, m_pAuthManager->GetAuthData()->strIP, m_pAuthManager->GetAuthData()->nPort, m_pAuthManager->GetAuthData()->nServiceProvider, m_pAuthManager->GetAuthData()->nServerType, m_pAuthManager->GetAuthData()->strUniqKey));

		if( m_pAuthManager->GetAuthData()->nCertification <= 0 )
		{
			m_nSessionSvrID = 0;

			sc::writeLogError( std::string( "AUTH FAIL. Server STOP" ) );
			m_bUseEventThread = FALSE;
			::SendMessage(m_hWnd, WM_USER + 504, (WPARAM) 0, (LPARAM) 1 );
		}
		else
		{
			m_nSessionSvrID = nsm->gsi.nSessionSvrID;

			sc::writeLogInfo( std::string( "AUTH SUCCESS." ) );
		}
	}
}

int CSessionServer::SendAuthServer( NET_MSG_GENERIC* pBuffer, DWORD SendSize )
{
	if( !m_bConnectedAuthServer ) return NET_ERROR; 

    if ( pBuffer && m_nAuthServerSlot != net::INVALID_SLOT )
    {
        PROFILE_MSG_COUNT_SEND( pBuffer );
        return m_pClientManager->SendClient2( m_nAuthServerSlot, pBuffer );
    }
    else 
    {
        sc::writeLogError( std::string( "SendSession buffer is null or session slot is invalid." ) );
        return NET_ERROR;        
    }
}

// 로그인,필드,캐쉬,에이젼트 서버에서 전달받은 인증정보를 인증서버로 릴레이
// 로그인, 필드, 캐쉬, 에이젼트 서버는 내부망으로 구성되어 외부에서 접근이 불가능할 수 있으므로 ( 현재 필리핀 캐쉬 서버 ) 반드시 세션 서버를 통해 릴레이한다
void CSessionServer::MsgAuthCertificationRequest(DWORD dwClient, MSG_LIST* pMsg)
{
    if ( !pMsg ) 
		return;

	if( !m_bConnectedAuthServer ) return; 

	NET_AUTH_CERTIFICATION_REQUEST* authInfo = reinterpret_cast <NET_AUTH_CERTIFICATION_REQUEST*> (pMsg->Buffer);

	if ( m_nSessionSvrID == 0 )
	{
		sc::writeLogError(std::string("SessionSvrID is not exist. Relay ERROR to the auth server"));		
		return;
	}

	authInfo->gsi.nSessionSvrID = m_nSessionSvrID;

	if ( SendAuthServer(authInfo, authInfo->Size()) == NET_ERROR )
	{
		sc::writeLogError(std::string("Relay ERROR to the auth server"));		
	}
}

void CSessionServer::MsgAuthSessionStateRequest()
{   
	ResponseAuthSessionState();
}

void CSessionServer::ResponseAuthSessionState()
{   
	if( !m_bConnectedAuthServer ) return; 

    G_AUTH_CUR_INFO curServerInfo;

	curServerInfo.nCounrtyType = m_pConfig->GetServiceProvider();
    curServerInfo.ServerType = m_pConfig->GetServerType();
    curServerInfo.nServicePort = m_pConfig->GetServicePort();	
    curServerInfo.SetServerIp(m_szAddress);
	
	for (DWORD dwSvr=0; dwSvr<(DWORD) m_pClientManager->GetMaxClient(); ++dwSvr)
	{
		if (m_pClientManager->IsOnline(dwSvr) == true &&  m_pClientManager->GetSvrInfo(dwSvr).ServerType == net::SERVER_AGENT)
		{
			curServerInfo.nServerCurrentUser += m_pClientManager->GetSvrInfo(dwSvr).nServerCurrentClient;
			curServerInfo.nServerMaxLoginedUser += m_pClientManager->GetSvrInfo(dwSvr).nServerMaxClient;
		}
	}

    NET_AUTH_STATE_ANS Msg(curServerInfo);
	
	if ( SendAuthServer(&Msg, Msg.Size()) == NET_ERROR )
	{
		sc::writeLogError(std::string("MsgAuthHeartbeatProcess NET_ERROR"));		
	}
}

void CSessionServer::MsgAuthEventInfoRequest(NET_MSG_GENERIC* nmg)
{   
    if ( nmg == NULL ) 
		return;

	NET_AUTH_EVENT_INFO_REQ* nsm = reinterpret_cast<NET_AUTH_EVENT_INFO_REQ*> (nmg);

	int nAgentCount = m_pClientManager->FindServerCount(net::SERVER_AGENT);
	DWORD AgentSlot = 0;

	if ( nAgentCount != 0 )
	{
		for ( int i = 0; i < nAgentCount; i++ )
		{
			AgentSlot = m_pClientManager->FindServer(net::SERVER_AGENT, i);

			if ( AgentSlot == NET_ERROR ) 
			{
				// 해당 서버를 찾지 못함
				sc::writeLogError( sc::string::format( "MsgAuthEventInfoRequest. Can not find the Server [%1%]", i));
				return;
			}

			SendClient(AgentSlot, nmg);
		}
	}
}

void CSessionServer::MsgAuthEventInfoResponse(DWORD dwClient, MSG_LIST* pMsg)
{
    if ( !pMsg ) 
		return;

	if( !m_bConnectedAuthServer ) return; 

	NET_EVENT_INFO_AUTH_ANS* eventInfo = reinterpret_cast <NET_EVENT_INFO_AUTH_ANS*> (pMsg->Buffer);

	if ( SendAuthServer(eventInfo, eventInfo->Size()) == NET_ERROR )
	{
		sc::writeLogError(std::string("Event info relay ERROR"));		
	}
}

void CSessionServer::MsgAuthServerStateRequest()
{   
	ResponseAuthServerState();
}

// 인증서버에 현재 자신에게 연결된 서버들을 알려줌
void CSessionServer::ResponseAuthServerState()
{   
	if( !m_bConnectedAuthServer ) return; 

	G_AUTH_INFO serverInfo;

	for (DWORD dwSvr=0; dwSvr<(DWORD) m_pClientManager->GetMaxClient(); ++dwSvr)
	{
		if (m_pClientManager->IsOnline(dwSvr) == true && m_pClientManager->GetSvrInfo(dwSvr).ServerType != net::SERVER_UNKNOWN && m_pClientManager->GetSvrInfo(dwSvr).ServerType != net::SERVER_SESSION)
		{
			serverInfo.nSessionSvrID = m_nSessionSvrID;
			serverInfo.nCounrtyType =  m_pConfig->GetServiceProvider(); // 세션에 연결된 서버들은 같은 서비스 프로바이더를 사용한다고 가정
			serverInfo.ServerType = m_pClientManager->GetSvrInfo(dwSvr).ServerType;
			serverInfo.nServicePort = m_pClientManager->GetSvrInfo(dwSvr).nServicePort;	
			serverInfo.SetServerIp(m_pClientManager->GetSvrInfo(dwSvr).szServerIP);

			NET_AUTH_SERVER_STATE_ANS Msg(serverInfo);
	
			if ( SendAuthServer(&Msg, Msg.Size()) == NET_ERROR )
			{
				sc::writeLogError(std::string("ResponseAuthServerState NET_ERROR"));		
			}
		}
	}
}


void CSessionServer::CountryAccessApprove( NET_MSG_GENERIC* nmg )
{
	for (DWORD dwSvr=0; dwSvr<(DWORD) m_pClientManager->GetMaxClient(); ++dwSvr)
	{
		if (m_pClientManager->IsOnline(dwSvr) == true && m_pClientManager->GetSvrInfo(dwSvr).ServerType == net::SERVER_LOGIN )
		{
			SendClient( dwSvr, nmg );
		}
	}
}
