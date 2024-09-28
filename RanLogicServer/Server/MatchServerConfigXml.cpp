#include "pch.h"
#pragma warning(disable:4267)
#include <boost/lexical_cast.hpp>
#include <boost/spirit/include/classic.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include "../../SigmaCore/Encrypt/DbEncrypt.h"
#include "../../SigmaCore/Log/LogMan.h"
#include "../../SigmaCore/Net/NetUtil.h"
#include "../../SigmaCore/String/StringFormat.h"
#include "../../SigmaCore/String/StringUtil.h"
#include "../../SigmaCore/Xml/2.5.3/ticpp.h"
#include "../../SigmaCore/Util/SystemInfo.h"
#include "./ServerConfigHelper.h"
#include "./MatchServerConfigXml.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

MatchServerConfigXml::MatchServerConfigXml()
{
}

MatchServerConfigXml::~MatchServerConfigXml()
{
}

bool MatchServerConfigXml::Parse()
{
    if (!ServerConfigXml::Parse())
        return false;

    if (!ServerConfigHelper::ParseSessionServer(m_pXml, m_SessionServer))
        return false;

    if (!ParseAgentServerToMatch())
        return false;
    
    return true;
}


bool MatchServerConfigXml::ParseAgentServerToMatch()
{
	try
	{
		ticpp::Element* pParent = NULL;

		pParent = (ticpp::Element*)m_pXml->FirstChild("RAN_SERVER")->FirstChild("AGENT_SERVER_TO_MATCH");

		if(pParent == NULL)
		{
			return false;
		}
		
		ticpp::Element* pChild = NULL;
		UINT i=0;
		for (pChild = (ticpp::Element*) pParent->FirstChild(false), i=0; pChild; pChild = (ticpp::Element*) pChild->NextSiblingElement(false), ++i)
		{
			std::string strKey = pChild->Value();
			
			if (strKey == "AGENT")
			{
				int nNumber;
				pChild->GetAttribute("NUMBER", &nNumber);

				//IP
				std::string strPublicIp;
				pChild->GetAttribute("PUBLIC_IP", &strPublicIp);
				StringCchCopy(m_AgentServerToMatch[i].szPublicIp, MAX_IP_LENGTH+1, strPublicIp.c_str());
				m_AgentServerToMatch[i].ulPublicIp = sc::net::inetAddr(strPublicIp.c_str());

				//Loaded
				m_AgentServerToMatch[i].bLoad = true;

				sc::writeLogInfo(
					sc::string::format(
					"loaded config: AgentToMatch[%1%] %2%"
					,nNumber
					,strPublicIp
					));
			}
		}
		
	}
	catch (ticpp::Exception& ex)
	{
		sc::writeLogError(std::string(ex.what()));
	}
	return true;
}


A_SERVER_INFO* MatchServerConfigXml::GetAgentServerInfo(DWORD n)
{
	GASSERT(n < MAXNUM_OF_AGENTSERVER_TO_MATCH);

	if (n > MAXNUM_OF_AGENTSERVER_TO_MATCH)
	{
		sc::writeLogError(sc::string::format("MatchServerConfigXml::GetAgentServerInfo() - index %1% is greater than MAXNUM_OF_AGENTSERVER_TO_MATCH(%2%)", n, MAXNUM_OF_AGENTSERVER_TO_MATCH));
		return NULL;
	}

	return &m_AgentServerToMatch[n];
}