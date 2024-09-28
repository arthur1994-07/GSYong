#include "pch.h"
#include "../../RanLogic/Msg/ServerMsg.h"
#include "../../RanLogic/s_NetGlobal.h"
#include "./LoginServer.h"
#include "./LoginServerConfigXml.h"
#include "../../RanLogic/Util/GlobalAuthManager.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

// 세션서버로 인증정보 요청
// 인증키를 읽어서 다른 정보들과 함께 전송한다 ( 인증키가 없으면 새로 생성 )
int CLoginServer::SendAuthInfoToSessionServer()
{
    G_AUTH_INFO authInfo;

	authInfo.nCounrtyType = m_pConfig->GetServiceProvider();
    authInfo.ServerType = m_pConfig->GetServerType();
    authInfo.nServicePort = m_pConfig->GetServicePort();	
    authInfo.SetServerIp(m_szAddress);

	m_pAuthManager->SetServerPort(m_pConfig->GetServicePort());
	m_pAuthManager->SetServerType(m_pConfig->GetServerType());
	m_pAuthManager->SetServiceProvider(m_pConfig->GetServiceProvider());
	m_pAuthManager->SetServerIP(std::string(m_szAddress));

	authInfo.SetAuthData(m_pAuthManager->GetEncryptedAuthData().c_str());

    NET_AUTH_CERTIFICATION_REQUEST Msg(authInfo);
	
    if ( SendClient(m_SessionServerSlot, &Msg) == NET_ERROR )
	{
        sc::writeLogError(std::string("Sending ERROR the Auth Info to session server"));
		return NET_ERROR;
	}
    else
	{
        sc::writeLogInfo(std::string("Sending SUCCESS the Auth Info to session server"));		
		return NET_OK;
	}
}

void CLoginServer::MsgAuthCertificationResponse(NET_MSG_GENERIC* pMsg)
{   
    if ( pMsg == NULL ) 
		return;

	NET_AUTH_CERTIFICATION_ANS* nsm = reinterpret_cast<NET_AUTH_CERTIFICATION_ANS*> (pMsg);

	if (nsm->gsi.ServerType != net::SERVER_LOGIN)
	{
		sc::writeLogInfo(std::string("Auth Msg Target is not match(LOGIN SERVER)"));
	}
	else
	{
		m_pAuthManager->ConfirmAuthData(std::string(nsm->gsi.szAuthData));
		//sc::writeLogInfo(sc::string::format("인증 결과 확인 RET[%d] IP[%s] Port[%d] Country[%d] SvrType[%d] Key[%s] ", m_pAuthManager->GetAuthData()->nCertification, m_pAuthManager->GetAuthData()->strIP, m_pAuthManager->GetAuthData()->nPort, m_pAuthManager->GetAuthData()->nServiceProvider, m_pAuthManager->GetAuthData()->nServerType, m_pAuthManager->GetAuthData()->strUniqKey));

		if( m_pAuthManager->GetAuthData()->nCertification <= 0 )
		{
			sc::writeLogError( std::string( "AUTH FAIL. Server STOP" ) );
			m_bUseEventThread = FALSE;
			::SendMessage(m_hWnd, WM_USER + 504, (WPARAM) 0, (LPARAM) 1 );
		}
		else
		{
			sc::writeLogInfo( std::string( "AUTH SUCCESS." ) );
		}
	}
}

void CLoginServer::MsgCountryAccessApprove( NET_MSG_GENERIC* pMsg )
{
	NET_MSG_PACK* pPacket = (NET_MSG_PACK*)pMsg;
	NET_COUNTRY_ACCESS_APPROVE RecvData;
	if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
		return;

	m_setCountryAccessApprove.clear();
	
	if( RecvData.vecContryList.empty() )
		return;

	sc::writeLogInfo( "AccessApprove Country List" );

	for ( int i = 0; i < RecvData.vecContryList.size() ; i++ )
		AddCountryAccessApprove( RecvData.vecContryList[i] );
}
