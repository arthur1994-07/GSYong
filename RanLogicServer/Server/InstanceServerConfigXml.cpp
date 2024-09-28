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
#include "./InstanceServerConfigXml.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

InstanceServerConfigXml::InstanceServerConfigXml()
{
    m_ExcessExpProcessType = ServerConfig::DEFAULT_EXCESS_EXP_PROCESS_TYPE;
    m_bGameGuardAuth = false;
    m_bPKServer = false;
	m_dwInstanceServerNum = 0;
}

InstanceServerConfigXml::~InstanceServerConfigXml()
{
}

bool InstanceServerConfigXml::Parse()
{
    if (!ServerConfigXml::Parse())
        return false;

    if (!ServerConfigHelper::ParseSessionServer(m_pXml, m_SessionServer))
        return false;

    if (!ServerConfigHelper::ParseCacheServer(m_pXml, m_CacheServer))
        return false;

	if (!ServerConfigHelper::ParseMatchServer(m_pXml, m_MatchServer))
        return false;

	if (!ServerConfigHelper::ParseInstanceServerNum(m_pXml, m_dwInstanceServerNum))
        return false;

    if (!ServerConfigHelper::ParseGameGuard(m_pXml, m_bGameGuardAuth))
        return false;
   
    if (!ParseExcessXpProcessType())
        return false;

    if (!ParsePkServer())
        return false;
    
    ServerConfigHelper::ParseDatabase(m_pXml, "GAME_DB", m_GameDb[0]);
    ServerConfigHelper::ParseDatabase(m_pXml, "LOG_DB", m_LogDb);
	
	//mjeon.ado
    ServerConfigHelper::ParseDatabase(m_pXml, "GAME_DB", m_GameAdo[0]);
    ServerConfigHelper::ParseDatabase(m_pXml, "LOG_DB",  m_LogAdo);

    return true;
}

bool InstanceServerConfigXml::ParsePkServer()
{
    try
    {
        int nPk = 0;
        ticpp::Element* pElem = (ticpp::Element*) m_pXml->FirstChild("RAN_SERVER")->FirstChild("PKSERVER");
        pElem->GetTextOrDefault<int, int> (&nPk, 0);
        if (nPk == 1)
        {
            sc::writeLogInfo(std::string("loaded config: PKSERVER 1"));
            m_bPKServer = true;
        }
        else
        {
            sc::writeLogInfo(std::string("loaded config: PKSERVER 0"));
            m_bPKServer = false;
        }        
        return true;
    }
    catch (ticpp::Exception& ex)
    {
        // Report the error
        WriteErrorLog(ex);
        m_bPKServer = false;
        sc::writeLogWarn(std::string("RAN_SERVER/PKSERVER does not exist. It will be set to default value:false"));
        return true;
    }
}

bool InstanceServerConfigXml::ParseExcessXpProcessType()
{
    try
    {
        int ExpType = 0;
        ticpp::Element* pElem = (ticpp::Element*) m_pXml->FirstChild("RAN_SERVER")->FirstChild("EXCESSE_XP_PROCESS_TYPE");
        pElem->GetTextOrDefault<int, int> (&ExpType, ServerConfig::DEFAULT_EXCESS_EXP_PROCESS_TYPE);
        m_ExcessExpProcessType = ExpType;
        sc::writeLogInfo(
            sc::string::format(
            "loaded config: EXCESSE_XP_PROCESS_TYPE %1%", m_ExcessExpProcessType));
        return true;
    }
    catch (ticpp::Exception& ex)
    {
        // Report the error
        WriteErrorLog(ex);
        m_ExcessExpProcessType = ServerConfig::DEFAULT_EXCESS_EXP_PROCESS_TYPE;
        sc::writeLogWarn(
            sc::string::format(
            "RAN_SERVER/EXCESSE_XP_PROCESS_TYPE does not exist. It will be set to default value:%1%", ServerConfig::DEFAULT_EXCESS_EXP_PROCESS_TYPE));
        return true;
    }
}

bool InstanceServerConfigXml::ParseDatabaseGroup()
{
	return true;
	/*
	try
	{
		ticpp::Element* pParent = NULL;
		for (pParent = (ticpp::Element*) m_pXml->FirstChild("RAN_SERVER")->FirstChild("GAMEDB_GROUP");
			pParent;
			pParent = (ticpp::Element*) pParent->NextSiblingElement())
		{
			DWORD dwGroupNum = 0;

			pParent->GetAttribute("NUMBER", &dwGroupNum);
			sc::writeLogInfo(
				sc::string::format(
				"loaded config: GAMEDB_GROUP:%1%", dwGroupNum));

			ticpp::Element* pChild = NULL;
			for (pChild = (ticpp::Element*) pParent->FirstChild(false);
				pChild;
				pChild = (ticpp::Element*) pChild->NextSiblingElement(false) )
			{
				std::string strChannel = pChild->Value();
				// 채널 PK 가능 불가능
				if (strChannel == "PK") 
				{
					// on <PK>1</PK>, off <PK>0</PK>
					WORD wPK=1;
					pChild->GetTextOrDefault<WORD, WORD>( &wPK, 1 );

					// 채널의 모든 필드서버 PK 상태 세팅
					for (int Loop=0; Loop<FIELDSERVER_MAX; ++Loop)
					{
						if (wPK == 1)
							m_sFIELDServer[nChannelNumber][Loop].bPk = true;
						else
							m_sFIELDServer[nChannelNumber][Loop].bPk = false;
					}
				}
				if (strChannel == "FIELD")
				{
	*/
}
