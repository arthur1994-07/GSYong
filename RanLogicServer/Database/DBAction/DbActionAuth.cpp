#include "pch.h"
#include "../../../SigmaCore/Util/DateTime.h"
#include "DbActionAuth.h"
#include "../DbManager.h"

#include "../../Server/AuthServer.h"

#include "../../../RanLogic/Util/GlobalAuthManager.h"
#include "../../../RanLogic/Network/NetLogicDefine.h"
#include "../../../SigmaCore/DebugInclude.h"

namespace db
{

LogAuthState::LogAuthState(int nCounrty, int nServerType, TCHAR *szIP, int nPort, int nCurUser, int nMaxUser) 
	: m_nCounrty(nCounrty), m_nServerType(nServerType), m_nPort(nPort), m_nCurUser(nCurUser), m_nMaxUser(nMaxUser)
{
	StringCchCopy( m_szIP, LogAuthState::MAX_IP_LEN, szIP );
    m_LogDate = sc::time::GetCurrentTime();
}

int LogAuthState::Execute(NetServer* pServer)
{
	return m_pDbManager->AddAuthServerState( m_nCounrty, m_nServerType, m_szIP, m_nPort, m_nCurUser, m_nMaxUser, m_LogDate );
}

CertificationAuthData::CertificationAuthData(DWORD dwClient, sc::SAuthData *authData)
    : m_AuthData(*authData), m_dwClient(dwClient)
{
}

int CertificationAuthData::Execute(NetServer* pServer)
{
	if ( pServer == NULL )
		return NET_ERROR;

	CAuthServer* pAuthServer = reinterpret_cast<CAuthServer*> (pServer);

	int nSessionSvrID = 0;
	int nIsSessionSvr = 0;

	if (  m_AuthData.nServerType == net::SERVER_SESSION )
		nIsSessionSvr = 1;

	if ( m_pDbManager->ProcessCertificationForAuth(nIsSessionSvr, m_AuthData.nServiceProvider, m_AuthData.nServerType, m_AuthData.strIP.c_str(), m_AuthData.nPort, m_AuthData.strUniqKey, m_AuthData.nCertification, m_AuthData.strNewUniqKey, nSessionSvrID) == sc::db::DB_ERROR )
	{
		return NET_ERROR;
	}
	else
	{
		// 국가별 클라이언트 접속 제한.
		std::vector<int> vecIndex;
		m_pDbManager->AuthCountryAccessApproveSelect( m_AuthData.nServiceProvider, m_AuthData.nServerType, m_AuthData.strIP.c_str(), m_AuthData.nPort, vecIndex );
		pAuthServer->SendCountryAccessApprove(m_dwClient, vecIndex);

		G_AUTH_INFO authInfo;

		authInfo.nCounrtyType = m_AuthData.nServiceProvider;
		authInfo.ServerType = (net::EMSERVERTYPE)m_AuthData.nServerType;
		StringCchCopy( authInfo.szServerIP, db::LogAuthState::MAX_IP_LEN, m_AuthData.strIP.c_str() );
		authInfo.nServicePort = m_AuthData.nPort;
		
		if ( authInfo.ServerType == net::SERVER_SESSION )
			authInfo.nSessionSvrID = nSessionSvrID;
		else
			authInfo.nSessionSvrID = 0;

		///////////////////////////////////////////////////////////////////////
		// DB에서 인증한 결과를 InnerMsg로 Logic으로 보내서 전송하도록 하는것이 맞지만 시간 관계상 여기서 세션서버로 전송도록 처리
		pAuthServer->m_pAuthManager->SetCertification(m_AuthData.nCertification);
		pAuthServer->m_pAuthManager->SetNewUniqKey(m_AuthData.strNewUniqKey);
		
		std::string encryptedData = pAuthServer->m_pAuthManager->EncryptAuthData(pAuthServer->m_pAuthManager->CombineCertificationData());
		
		if ( encryptedData.length() > AUTH_DATA_LENGTH )
			return NET_ERROR;

		StringCchCopy( authInfo.szAuthData, encryptedData.length()+1, encryptedData.c_str() );

		if ( pAuthServer->SendAuthCertificationToSessionServer(m_dwClient, authInfo) == NET_ERROR )
		{
			return NET_ERROR;
		}
		///////////////////////////////////////////////////////////////////////

		return NET_OK;
	}
}

AuthEventInfo::AuthEventInfo(G_AUTH_EVENT_INFO &eventInfo)
    : m_EventInfo(eventInfo)
{
}

int AuthEventInfo::Execute(NetServer* pServer)
{
	return m_pDbManager->AddAuthEventInfo(&m_EventInfo);
}

LogAuthServerStateInit::LogAuthServerStateInit() 
{
}

int LogAuthServerStateInit::Execute(NetServer* pServer)
{
	if ( pServer == NULL )
		return NET_ERROR;

	if ( m_pDbManager->AddAuthServerConnectionStateInit() == sc::db::DB_ERROR )
	{
		return NET_ERROR;
	}
	else
	{
		CAuthServer* pAuthServer = reinterpret_cast<CAuthServer*> (pServer);

		pAuthServer->SendRequestServerStateInfo();
		return NET_OK;
	}
    return NET_OK;
}


LogAuthServerState::LogAuthServerState(int nSessionSvrID, int nCounrty, int nServerType, TCHAR *szIP, int nPort) 
	: m_nSessionSvrID(nSessionSvrID), m_nCounrty(nCounrty), m_nServerType(nServerType), m_nPort(nPort)
{
	StringCchCopy( m_szIP, LogAuthState::MAX_IP_LEN, szIP );
}

int LogAuthServerState::Execute(NetServer* pServer)
{
	return m_pDbManager->AddAuthServerConnectionState( m_nSessionSvrID, m_nCounrty, m_nServerType, m_szIP, m_nPort );
}

} // namespace db
