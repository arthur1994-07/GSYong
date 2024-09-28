#include "pch.h"
#pragma warning(disable:4267)
#include "./ServerConfigHelper.h"
#include "./SessionServerConfigXml.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

SessionServerConfigXml::SessionServerConfigXml()
{
}

SessionServerConfigXml::~SessionServerConfigXml()
{
}

bool SessionServerConfigXml::Parse()
{
    if (!ServerConfigXml::Parse())
        return false;

	/////////////////////////////////////////////////////////////////////////////////
	// 원래는 Config에서 인증서버 주소를 읽어와야하지만 config 파일을 수정하지 않기 위해서 일단은 정해진 아이피로 접속하도록 한다.
	// 차후에는 config 파일에   <AUTH_SERVER IP="172.16.2.71" PORT="15000" />  처럼 인증서버 정보를 추가해야함
	// for global auth server
	//if (!ServerConfigHelper::ParseAuthServer(m_pXml, m_AuthServer))
    //    return false;

#ifdef _RELEASED
	// 실제 인증서버 주소. Release모드에서만 사용된다.
	m_AuthServer.Address = "172.16.2.87";
	m_AuthServer.Port = 17802;
#else
	// 실제 인증서버 주소. Release모드에서만 사용된다.
	m_AuthServer.Address = "59.23.229.207";
	m_AuthServer.Port = 17802;
#endif

	/////////////////////////////////////////////////////////////////////////////////

    ServerConfigHelper::ParseDatabase(m_pXml, "USER_DB", m_UserDb);
    ServerConfigHelper::ParseDatabase(m_pXml, "LOG_DB", m_LogDb);
    
	if (m_ServiceProvider == SP_PHILIPPINES || m_ServiceProvider == SP_MALAYSIA || m_ServiceProvider == SP_THAILAND)
	{
		ServerConfigHelper::ParseDatabase(m_pXml, "TERRA_DB", m_Terra);
	}
	else if (m_ServiceProvider == SP_GLOBAL)
	{
		ServerConfigHelper::ParseDatabase(m_pXml, "GSP_DB", m_Gsp);
	}

	//mjeon.ado	
	ServerConfigHelper::ParseDatabase(m_pXml, "USER_DB", m_UserAdo);    
    ServerConfigHelper::ParseDatabase(m_pXml, "LOG_DB",  m_LogAdo);

    return true;
}