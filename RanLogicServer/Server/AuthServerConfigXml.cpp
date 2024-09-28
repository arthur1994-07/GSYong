#include "pch.h"
#pragma warning(disable:4267)
#include "./ServerConfigHelper.h"
#include "./AuthServerConfigXml.h"
#include "../../SigmaCore/DebugInclude.h"

AuthServerConfigXml::AuthServerConfigXml()
{
}

AuthServerConfigXml::~AuthServerConfigXml()
{
}

bool AuthServerConfigXml::Parse()
{
    if ( !ServerConfigXml::Parse() )
        return false;

/*    ServerConfigHelper::ParseDatabase(m_pXml, "USER_DB", m_UserDb);
    ServerConfigHelper::ParseDatabase(m_pXml, "LOG_DB", m_LogDb);
    
	if ( m_ServiceProvider == SP_PHILIPPINES || m_ServiceProvider == SP_MALAYSIA || m_ServiceProvider == SP_THAILAND )
	{
		ServerConfigHelper::ParseDatabase(m_pXml, "TERRA_DB", m_Terra);
	}
	else if ( m_ServiceProvider == SP_GLOBAL )
	{
		ServerConfigHelper::ParseDatabase(m_pXml, "GSP_DB", m_Gsp);
	}

	ServerConfigHelper::ParseDatabase(m_pXml, "USER_DB", m_UserAdo); */   
	ServerConfigHelper::ParseAuthDatabase(m_pXml, "AUTH_DB",  m_AuthAdo);

    return true;
}