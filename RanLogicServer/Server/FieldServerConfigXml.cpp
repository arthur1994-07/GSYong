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
#include "./FieldServerConfigXml.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

FieldServerConfigXml::FieldServerConfigXml()
{
    m_ExcessExpProcessType = ServerConfig::DEFAULT_EXCESS_EXP_PROCESS_TYPE;
    m_nServerField = ServerConfig::DEFAULT_SERVER_FIELD;
    m_bGameGuardAuth = false;
    m_bPKServer = false;
    m_nServerChannel = 0;	

	bPARTY_2OTHERSCHOOL = FALSE;
	bCLUB_2OTHERSCHOOL = FALSE;
	fITEM_DROP_SCALE = 1.0f;
	fMONEY_DROP_SCALE = 1.0f;
	fITEM_DROP_LOWER_SCALE = 1.0f;
	dwCRITICAL_MAX = 40;
	fNONPK_ITEM_DROP = 0.0f;
	fPK_ITEM_DROP = 0.0f;
	bUseRanMobile = false;

	ServerRestartDayOfWeek = -1;
	ServerRestartHour = -1;
	ServerRestartWorkingsetSize = 0;

	ForceLogItemRandomOption = 0;
}

FieldServerConfigXml::~FieldServerConfigXml()
{
}

bool FieldServerConfigXml::Parse()
{
    if (!ServerConfigXml::Parse())
        return false;

    if (!ServerConfigHelper::ParseSessionServer(m_pXml, m_SessionServer))
        return false;

    if (!ServerConfigHelper::ParseCacheServer(m_pXml, m_CacheServer))
        return false;

	if ( !ServerConfigHelper::ParseIntegrationCacheServer( m_pXml, m_IntegrationCacheServer ) )
		return false;

    if (!ServerConfigHelper::ParseGameGuard(m_pXml, m_bGameGuardAuth))
        return false;

    if (!ParseAgentServer())
        return false;

    if (!ParseExcessXpProcessType())
        return false;

    if (!ParseServerField())
        return false;

    if (!ParsePkServer())
        return false;

    if (!ServerConfigHelper::ParseServerChannel(m_pXml, m_nServerChannel))
        return false;

	ParsePARTY_2OTHERSCHOOL();
	ParseCLUB_2OTHERSCHOOL();
	ParseITEM_DROP_SCALE();
	ParseMONEY_DROP_SCALE();
	ParseITEM_DROP_LOWER_SCALE();
	ParseCRITICAL_MAX();
	ParseNONPK_ITEM_DROP();
	ParsePK_ITEM_DROP();
	PaseUseRanMobile();

	ParseServerRestartDayOfWeek();
	ParseServerRestartHour();
	ParseServerRestartWorkingsetSize();

	ParseForceLogItemRandomOption();

    ServerConfigHelper::ParseDatabase(m_pXml, "USER_DB", m_UserDb);
    ServerConfigHelper::ParseDatabase(m_pXml, "GAME_DB", m_GameDb);
    ServerConfigHelper::ParseDatabase(m_pXml, "LOG_DB", m_LogDb);    
    ServerConfigHelper::ParseDatabase(m_pXml, "SHOP_DB", m_ShopDb);

	// 전장서버는 GS와 알파에서만 사용
	if(  m_ServiceProvider == SP_OFFICE_TEST || m_ServiceProvider == SP_GS || m_ServiceProvider == SP_KOREA_TEST || m_ServiceProvider == SP_KOREA )
		ServerConfigHelper::ParseDatabase(m_pXml, "WORLDBATTLE_RELATED_DB", m_WorldBattleDb);

	// 란모바일 ODBC 연결
	if( true == GetUseRanMobile() )
		ServerConfigHelper::ParseDatabase(m_pXml, "RANMOBILE_DB", m_RanMobileDb);

	//ServerConfigHelper::ParseDatabase(m_pXml, "BOARD_DB", m_BoardDb);
	
	if (m_ServiceProvider == SP_PHILIPPINES || m_ServiceProvider == SP_MALAYSIA || m_ServiceProvider == SP_THAILAND)
	{
		ServerConfigHelper::ParseDatabase(m_pXml, "TERRA_DB", m_Terra);
	}
	//else if (m_ServiceProvider == SP_PHILIPPINES || m_ServiceProvider == SP_MALAYSIA )
	//{
	//	ServerConfigHelper::ParseDatabase(m_pXml, "GSP_DB", m_Gsp);
	//}
	else if (m_ServiceProvider == SP_KOREA || m_ServiceProvider == SP_KOREA_TEST )
	{
		ServerConfigHelper::ParseDatabase(m_pXml, "KOREA_DB", m_Kor);
	}
	//else if (m_ServiceProvider == SP_MALAYSIA )
	//{
	//	ServerConfigHelper::ParseDatabase(m_pXml, "MAL_DB", m_Mal);
	//}

	//mjeon.ado	
	ServerConfigHelper::ParseDatabase(m_pXml, "USER_DB", m_UserAdo);
    ServerConfigHelper::ParseDatabase(m_pXml, "GAME_DB", m_GameAdo);
    ServerConfigHelper::ParseDatabase(m_pXml, "LOG_DB",  m_LogAdo);
    ServerConfigHelper::ParseDatabase(m_pXml, "SHOP_DB", m_ShopAdo);
	ServerConfigHelper::ParseDatabase( m_pXml, "TEXASHOLDEM_DB", m_TexasHoldemIFAdo );

	if(  m_ServiceProvider == SP_OFFICE_TEST || m_ServiceProvider == SP_GS || m_ServiceProvider == SP_KOREA_TEST || m_ServiceProvider == SP_KOREA  )
		ServerConfigHelper::ParseDatabase(m_pXml, "WORLDBATTLE_RELATED_DB", m_WorldBattleAdo);

	// 란모바일 ADO 연결
	if( true == GetUseRanMobile() )
		ServerConfigHelper::ParseDatabase(m_pXml, "RANMOBILE_DB", m_RanMobileAdo);

    return true;
}

bool FieldServerConfigXml::ParsePkServer()
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

bool FieldServerConfigXml::ParseServerField()
{
    try
    {        
        ticpp::Element* pElem = (ticpp::Element*) m_pXml->FirstChild("RAN_SERVER")->FirstChild("SERVER_FIELD");
        pElem->GetTextOrDefault<int, int> (&m_nServerField, ServerConfig::DEFAULT_SERVER_FIELD);
        sc::writeLogInfo(
            sc::string::format(
            "loaded config: SERVER_FIELD %1%", m_nServerField));
        return true;
    }
    catch (ticpp::Exception& ex)
    {
        // Report the error
        WriteErrorLog(ex);
        m_nServerField = ServerConfig::DEFAULT_SERVER_NUMBER;
        sc::writeLogWarn(std::string("RAN_SERVER/SERVER_FIELD does not exist. It will be set to default value:0"));
        return true;
    }
}

bool FieldServerConfigXml::ParseExcessXpProcessType()
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

bool FieldServerConfigXml::ParseAgentServer()
{
    try
    {
        ticpp::Element* pElem = NULL;		
        pElem = (ticpp::Element*) m_pXml->FirstChild("RAN_SERVER")->FirstChild("AGENT_SERVER");
        pElem->GetAttribute("IP", &m_AgentServer.Address);        
        sc::writeLogInfo(
            sc::string::format(
            "loaded config: AGENT_SERVER %1%", m_AgentServer.Address));
        return true;
    }
    catch (ticpp::Exception& ex)
    {
        // Report the error
        sc::writeLogError(std::string("RAN_SERVER/AGENT_SERVER not exist"));
        WriteErrorLog(ex);
        return false;
    }
}

bool FieldServerConfigXml::ParsePARTY_2OTHERSCHOOL()
{
	try
	{
		int Value = 0;
		ticpp::Element* pElem = ( ticpp::Element* ) m_pXml->FirstChild( "RAN_SERVER" )->FirstChild( "bPARTY_2OTHERSCHOOL" );
		pElem->GetTextOrDefault< int, int > ( &Value, FALSE );
		if ( Value == 1 )
		{
			bPARTY_2OTHERSCHOOL = TRUE;
			sc::writeLogInfo( std::string( "loaded config: bPARTY_2OTHERSCHOOL 1" ) );
		}
		else
		{
			bPARTY_2OTHERSCHOOL = FALSE;
			sc::writeLogInfo(std::string( "loaded config: bPARTY_2OTHERSCHOOL 0" ) );
		}

		return true;
	}
	catch ( ticpp::Exception& ex )
	{
		// Report the error
		WriteWarningLog( ex );
		bPARTY_2OTHERSCHOOL = FALSE;
		sc::writeLogWarn(
			std::string(
			"RAN_SERVER/bPARTY_2OTHERSCHOOL does not exist. It will be set to default value:0"));
		return true;
	}
}

bool FieldServerConfigXml::ParseCLUB_2OTHERSCHOOL()
{
	try
	{
		int Value = 0;
		ticpp::Element* pElem = ( ticpp::Element* ) m_pXml->FirstChild( "RAN_SERVER" )->FirstChild( "bCLUB_2OTHERSCHOOL" );
		pElem->GetTextOrDefault< int, int > ( &Value, FALSE );
		if ( Value == 1 )
		{
			bCLUB_2OTHERSCHOOL = TRUE;
			sc::writeLogInfo( std::string( "loaded config: bCLUB_2OTHERSCHOOL 1" ) );
		}
		else
		{
			bCLUB_2OTHERSCHOOL = FALSE;
			sc::writeLogInfo(std::string( "loaded config: bCLUB_2OTHERSCHOOL 0" ) );
		}

		return true;
	}
	catch ( ticpp::Exception& ex )
	{
		// Report the error
		WriteWarningLog( ex );
		bCLUB_2OTHERSCHOOL = FALSE;
		sc::writeLogWarn(
			std::string(
			"RAN_SERVER/bCLUB_2OTHERSCHOOL does not exist. It will be set to default value:0"));
		return true;
	}
}

bool FieldServerConfigXml::ParseITEM_DROP_SCALE()
{
	try
	{
		float Value = 0;
		ticpp::Element* pElem = ( ticpp::Element* ) m_pXml->FirstChild( "RAN_SERVER" )->FirstChild( "fITEM_DROP_SCALE" );
		pElem->GetTextOrDefault< float, float > ( &Value, 1.0f );
		fITEM_DROP_SCALE = Value;
		sc::writeLogInfo( sc::string::format( "loaded config: fITEM_DROP_SCALE %1%", fITEM_DROP_SCALE ) );
		return true;
	}
	catch ( ticpp::Exception& ex )
	{
		// Report the error
		WriteWarningLog( ex );
		fITEM_DROP_SCALE = 1.0f;
		sc::writeLogWarn(
			std::string(
			"RAN_SERVER/fITEM_DROP_SCALE does not exist. It will be set to default value:1.0f"));
		return true;
	}
}

bool FieldServerConfigXml::ParseMONEY_DROP_SCALE()
{
	try
	{
		float Value = 0;
		ticpp::Element* pElem = ( ticpp::Element* ) m_pXml->FirstChild( "RAN_SERVER" )->FirstChild( "fMONEY_DROP_SCALE" );
		pElem->GetTextOrDefault< float, float > ( &Value, 1.0f );
		fMONEY_DROP_SCALE = Value;
		sc::writeLogInfo( sc::string::format( "loaded config: fMONEY_DROP_SCALE %1%", fMONEY_DROP_SCALE ) );
		return true;
	}
	catch ( ticpp::Exception& ex )
	{
		// Report the error
		WriteWarningLog( ex );
		fMONEY_DROP_SCALE = 1.0f;
		sc::writeLogWarn(
			std::string(
			"RAN_SERVER/fMONEY_DROP_SCALE does not exist. It will be set to default value:1.0f"));
		return true;
	}
}

bool FieldServerConfigXml::ParseITEM_DROP_LOWER_SCALE()
{
	try
	{
		float Value = 0;
		ticpp::Element* pElem = ( ticpp::Element* ) m_pXml->FirstChild( "RAN_SERVER" )->FirstChild( "fITEM_DROP_LOWER_SCALE" );
		pElem->GetTextOrDefault< float, float > ( &Value, 1.0f );
		fITEM_DROP_LOWER_SCALE = Value;
		sc::writeLogInfo( sc::string::format( "loaded config: fITEM_DROP_LOWER_SCALE %1%", fITEM_DROP_LOWER_SCALE ) );
		return true;
	}
	catch ( ticpp::Exception& ex )
	{
		// Report the error
		WriteWarningLog( ex );
		fITEM_DROP_LOWER_SCALE = 1.0f;
		sc::writeLogWarn(
			std::string(
			"RAN_SERVER/fITEM_DROP_LOWER_SCALE does not exist. It will be set to default value:1.0f"));
		return true;
	}
}

bool FieldServerConfigXml::ParseCRITICAL_MAX()
{
	try
	{
		DWORD Value = 0;
		ticpp::Element* pElem = ( ticpp::Element* ) m_pXml->FirstChild( "RAN_SERVER" )->FirstChild( "dwCRITICAL_MAX" );
		pElem->GetTextOrDefault< DWORD, DWORD > ( &Value, 40 );
		dwCRITICAL_MAX = Value;
		sc::writeLogInfo( sc::string::format( "loaded config: dwCRITICAL_MAX %1%", dwCRITICAL_MAX ) );
		return true;
	}
	catch ( ticpp::Exception& ex )
	{
		// Report the error
		WriteWarningLog( ex );
		dwCRITICAL_MAX = 40;
		sc::writeLogWarn(
			std::string(
			"RAN_SERVER/dwCRITICAL_MAX does not exist. It will be set to default value:40"));
		return true;
	}
}

bool FieldServerConfigXml::ParseNONPK_ITEM_DROP()
{
	try
	{
		float Value = 0;
		ticpp::Element* pElem = ( ticpp::Element* ) m_pXml->FirstChild( "RAN_SERVER" )->FirstChild( "fNONPK_ITEM_DROP" );
		pElem->GetTextOrDefault< float, float > ( &Value, 0.0f );
		fNONPK_ITEM_DROP = Value;
		sc::writeLogInfo( sc::string::format( "loaded config: fNONPK_ITEM_DROP %1%", fNONPK_ITEM_DROP ) );
		return true;
	}
	catch ( ticpp::Exception& ex )
	{
		// Report the error
		WriteWarningLog( ex );
		fNONPK_ITEM_DROP = 1.0f;
		sc::writeLogWarn(
			std::string(
			"RAN_SERVER/fNONPK_ITEM_DROP does not exist. It will be set to default value:0.0f"));
		return true;
	}
}

bool FieldServerConfigXml::ParsePK_ITEM_DROP()
{
	try
	{
		float Value = 0;
		ticpp::Element* pElem = ( ticpp::Element* ) m_pXml->FirstChild( "RAN_SERVER" )->FirstChild( "fPK_ITEM_DROP" );
		pElem->GetTextOrDefault< float, float > ( &Value, 0.0f );
		fPK_ITEM_DROP = Value;
		sc::writeLogInfo( sc::string::format( "loaded config: fPK_ITEM_DROP %1%", fPK_ITEM_DROP ) );
		return true;
	}
	catch ( ticpp::Exception& ex )
	{
		// Report the error
		WriteWarningLog( ex );
		fPK_ITEM_DROP = 1.0f;
		sc::writeLogWarn(
			std::string(
			"RAN_SERVER/fPK_ITEM_DROP does not exist. It will be set to default value:0.0f"));
		return true;
	}
}

bool FieldServerConfigXml::PaseUseRanMobile()
{
	try
	{
		int Value = 0;
		ticpp::Element* pElem = ( ticpp::Element* ) m_pXml->FirstChild( "RAN_SERVER" )->FirstChild( "bUseRanMobile" );
		pElem->GetTextOrDefault< int, int > ( &Value, 0.0f );
		bUseRanMobile = 0 == Value ? false : true;
		sc::writeLogInfo( sc::string::format( "loaded config: bUseRanMobile %1%", true == bUseRanMobile ? 1 : 0 ) );
		return true;
	}
	catch ( ticpp::Exception& ex )
	{
		// Report the error
		WriteWarningLog( ex );
		bUseRanMobile = false;
		sc::writeLogWarn(
			std::string(
			"RAN_SERVER/bUseRanMobile does not exist. It will be set to default value:0"));
		return true;
	}
}

bool FieldServerConfigXml::ParseServerRestartDayOfWeek()
{
	try
	{
		int Value = -1;
		ticpp::Element* pElem = ( ticpp::Element* ) m_pXml->FirstChild( "RAN_SERVER" )->FirstChild( "RESTART_DAYOFWEEK" );
		pElem->GetTextOrDefault< int, int > ( &Value, -1 );
		sc::writeLogInfo( sc::string::format( "loaded config: RESTART_DAYOFWEEK %1%", Value ) );
		ServerRestartDayOfWeek = Value;
		return true;
	}
	catch ( ticpp::Exception& ex )
	{
		WriteWarningLog( ex );
		ServerRestartDayOfWeek = -1;
		sc::writeLogWarn( std::string( "RAN_SERVER/RESTART_DAYOFWEEK does not exist. It will be set to default value:-1") );
		return true;
	}
}

bool FieldServerConfigXml::ParseServerRestartHour()
{
	try
	{
		int Value = -1;
		ticpp::Element* pElem = ( ticpp::Element* ) m_pXml->FirstChild( "RAN_SERVER" )->FirstChild( "RESTART_HOUR" );
		pElem->GetTextOrDefault< int, int > ( &Value, -1 );
		sc::writeLogInfo( sc::string::format( "loaded config: RESTART_HOUR %1%", Value ) );
		ServerRestartHour = Value;
		return true;
	}
	catch ( ticpp::Exception& ex )
	{
		WriteWarningLog( ex );
		ServerRestartHour = -1;
		sc::writeLogWarn( std::string( "RAN_SERVER/RESTART_HOUR does not exist. It will be set to default value:-1") );
		return true;
	}
}

bool FieldServerConfigXml::ParseServerRestartWorkingsetSize()
{
	try
	{
		int Value = 0;
		ticpp::Element* pElem = ( ticpp::Element* ) m_pXml->FirstChild( "RAN_SERVER" )->FirstChild( "RESTART_WORKINGSET_SIZE" );
		pElem->GetTextOrDefault< int, int > ( &Value, 0 );
		sc::writeLogInfo( sc::string::format( "loaded config: RESTART_WORKINGSET_SIZE %1%", Value ) );
		ServerRestartWorkingsetSize = Value;
		return true;
	}
	catch ( ticpp::Exception& ex )
	{
		WriteWarningLog( ex );
		ServerRestartWorkingsetSize = 0;
		sc::writeLogWarn( std::string( "RAN_SERVER/RESTART_WORKINGSET_SIZE does not exist. It will be set to default value:0") );
		return true;
	}
}

bool FieldServerConfigXml::ParseForceLogItemRandomOption()
{
	try
	{
		int Value = 0;
		ticpp::Element* pElem = ( ticpp::Element* ) m_pXml->FirstChild( "RAN_SERVER" )->FirstChild( "FORCE_LOG_ITEM_RANDOM_OPTION" );
		pElem->GetTextOrDefault< int, int > ( &Value, 0 );
		sc::writeLogInfo( sc::string::format( "loaded config: FORCE_LOG_ITEM_RANDOM_OPTION %1%", Value ) );
		ForceLogItemRandomOption = Value;
		return true;
	}
	catch ( ticpp::Exception& ex )
	{
		WriteWarningLog( ex );
		ForceLogItemRandomOption = 0;
		sc::writeLogWarn( std::string( "RAN_SERVER/FORCE_LOG_ITEM_RANDOM_OPTION does not exist. It will be set to default value:0") );
		return true;
	}
}

