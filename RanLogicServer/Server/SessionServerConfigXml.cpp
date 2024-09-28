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
	// ������ Config���� �������� �ּҸ� �о�;������� config ������ �������� �ʱ� ���ؼ� �ϴ��� ������ �����Ƿ� �����ϵ��� �Ѵ�.
	// ���Ŀ��� config ���Ͽ�   <AUTH_SERVER IP="172.16.2.71" PORT="15000" />  ó�� �������� ������ �߰��ؾ���
	// for global auth server
	//if (!ServerConfigHelper::ParseAuthServer(m_pXml, m_AuthServer))
    //    return false;

#ifdef _RELEASED
	// ���� �������� �ּ�. Release��忡���� ���ȴ�.
	m_AuthServer.Address = "172.16.2.87";
	m_AuthServer.Port = 17802;
#else
	// ���� �������� �ּ�. Release��忡���� ���ȴ�.
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