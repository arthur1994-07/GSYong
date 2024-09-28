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
#include "./AgentServerConfigXml.h"
#include "../../RanLogic/GLUseFeatures.h"
// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

AgentServerConfigXml::AgentServerConfigXml()
{
    m_WhiteRockServer.Address = DefaultWhiteRockName;
    m_WhiteRockServer.Port = ServerConfig::DEFAULT_WHITE_LOCK_PORT;
    m_bNewServer = false;
	m_bSubServer = false;
	m_nWorldBattleServerNumber = 0;
    m_nMaxFieldNumber = 0;
	m_nMaxAgentNumber = 0;
    m_bGameGuardAuth = false;
    m_nServerChannel = 0;
    m_nServerChannelNumber = 1;
    m_bKorShutdown = false;	
	m_bAutoClubJoin = false;
	bPARTY_2OTHERSCHOOL = FALSE;
	bCLUB_2OTHERSCHOOL = FALSE;
	fITEM_DROP_SCALE = 1.0f;
	fMONEY_DROP_SCALE = 1.0f;
	fITEM_DROP_LOWER_SCALE = 1.0f;
	dwCRITICAL_MAX = 40;
	fNONPK_ITEM_DROP = 0.0f;
	fPK_ITEM_DROP = 0.0f;
	bUseRanMobile = false;
}

AgentServerConfigXml::~AgentServerConfigXml()
{
}

bool AgentServerConfigXml::Parse()
{
    if (!ServerConfigXml::Parse())
        return false;
    
    if (!ParseServerChannelNumber())
        return false;

    if (!ServerConfigHelper::ParseSessionServer(m_pXml, m_SessionServer))
        return false;

    if (!ServerConfigHelper::ParseCacheServer(m_pXml, m_CacheServer))
        return false;

	if (GLUseFeatures::GetInstance().IsUsingMatchSystem())
	{
		if (!ServerConfigHelper::ParseMatchServer(m_pXml, m_MatchServer))
			return false;
	}

    if (!ServerConfigHelper::ParseGameGuard(m_pXml, m_bGameGuardAuth))
        return false;

	if (m_ServiceProvider == SP_JAPAN)
	{
		ParseWhiteRockServer();
	}

	if ( SP_OFFICE_TEST == m_ServiceProvider || SP_KOREA == m_ServiceProvider || SP_KOREA_TEST == m_ServiceProvider  )
	{
		ParseKorShutdown();
	}

    if (!ServerConfigHelper::ParseServerChannel(m_pXml, m_nServerChannel))
        return false;

    if (!ParseChannelInfo())
        return false;

	if (!ParseAgentServerInfo())
		return false;

	if ( !ParseSubServer() )
		return false;

    if (!ParseNewServer())
        return false;

	if (!ParseAutoClubJoin())
		return false;

	if (!ParseAutoClubDbNum())
		return false;

	ParsePARTY_2OTHERSCHOOL();
	ParseCLUB_2OTHERSCHOOL();
	ParseITEM_DROP_SCALE();
	ParseMONEY_DROP_SCALE();
	ParseITEM_DROP_LOWER_SCALE();
	ParseCRITICAL_MAX();
	ParseNONPK_ITEM_DROP();
	ParsePK_ITEM_DROP();
	ParseUseRanMobile();

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
	
	if ( m_ServiceProvider == SP_PHILIPPINES || m_ServiceProvider == SP_MALAYSIA || m_ServiceProvider == SP_THAILAND )
	{
		ServerConfigHelper::ParseDatabase( m_pXml, "TERRA_DB", m_Terra );
	}
	
	if ( m_ServiceProvider == SP_KOREA || m_ServiceProvider == SP_KOREA_TEST )
	{
		ServerConfigHelper::ParseDatabase( m_pXml, "KOREA_DB", m_Kor );
	}
	
	if ( m_ServiceProvider == SP_MALAYSIA )
	{
		ServerConfigHelper::ParseDatabase( m_pXml, "MAL_DB", m_Mal );
	}


	//mjeon.ado	
	ServerConfigHelper::ParseDatabase(m_pXml, "USER_DB", m_UserAdo);
    ServerConfigHelper::ParseDatabase(m_pXml, "GAME_DB", m_GameAdo);
    ServerConfigHelper::ParseDatabase(m_pXml, "LOG_DB",  m_LogAdo);
    ServerConfigHelper::ParseDatabase(m_pXml, "SHOP_DB", m_ShopAdo);	

	if(  m_ServiceProvider == SP_OFFICE_TEST || m_ServiceProvider == SP_GS || m_ServiceProvider == SP_KOREA_TEST || m_ServiceProvider == SP_KOREA )
		ServerConfigHelper::ParseDatabase(m_pXml, "WORLDBATTLE_RELATED_DB", m_WorldBattleAdo);

	// 란모바일 ADO 연결
	if( true == GetUseRanMobile() )
		ServerConfigHelper::ParseDatabase(m_pXml, "RANMOBILE_DB", m_RanMobileAdo);

    return true;
}

bool AgentServerConfigXml::ParseServerChannelNumber()
{
    try
    {        
        ticpp::Element* pElem = (ticpp::Element*) m_pXml->FirstChild("RAN_SERVER")->FirstChild("SERVER_CHANNEL_NUMBER");
        pElem->GetTextOrDefault<int, int> (&m_nServerChannelNumber, ServerConfig::DEFAULT_SERVER_CHANNEL_NUMBER);
        sc::writeLogInfo(
            sc::string::format(
            "loaded config: SERVER_CHANNEL_NUMBER %1%", m_nServerChannelNumber));
        return true;
    }
    catch (ticpp::Exception& ex)
    {
        // Report the error
        WriteErrorLog(ex);
        m_nServerChannelNumber = ServerConfig::DEFAULT_SERVER_CHANNEL_NUMBER;
        sc::writeLogWarn(
            sc::string::format(
            "RAN_SERVER/SERVER_CHANNEL_NUMBER does not exist. It will be set to default value:%1%", ServerConfig::DEFAULT_SERVER_CHANNEL_NUMBER));
        return true;
    }
}

bool AgentServerConfigXml::ParseNewServer()
{
    try
    {
        int NewServer = 0;
        ticpp::Element* pElem = (ticpp::Element*) m_pXml->FirstChild("RAN_SERVER")->FirstChild("NEW_SERVER");
        pElem->GetTextOrDefault<int, int> (&NewServer, 0);
        if (NewServer == 0)
        {
            m_bNewServer = false;
            sc::writeLogInfo(std::string("New server false"));
        }
        else
        {
            m_bNewServer = true;
            sc::writeLogInfo(std::string("New server true"));
        }
        return true;
    }
    catch (ticpp::Exception& ex)
    {
        // Report the error
        WriteErrorLog(ex);
        m_bNewServer = false;
        sc::writeLogWarn(std::string("RAN_SERVER/NEW_SERVER does not exist. It will be set to default value:false"));
        return true;
    }
}

bool AgentServerConfigXml::ParseChannelInfo()
{
    try
    {
        ticpp::Element* pParent = NULL;
        for (pParent = (ticpp::Element*) m_pXml->FirstChild("RAN_SERVER")->FirstChild("CHANNEL");
            pParent;
            pParent = (ticpp::Element*) pParent->NextSiblingElement())
        {
            // 채널번호
            int nChannelNumber;
            pParent->GetAttribute("NUMBER", &nChannelNumber);
            sc::writeLogInfo(
                sc::string::format(
                "loaded config: CHANNEL:%1%", nChannelNumber));

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
                    int nNumber; // 필드번호
                    pChild->GetAttribute("NUMBER", &nNumber);

					// Name, 이름
					std::string strName;
					pChild->GetAttribute("NAME", &strName);
					StringCchCopy(m_sFIELDServer[nChannelNumber][nNumber].szServerName, SERVER_NAME_LENGTH+1, strName.c_str());

                    // 공개 IP 세팅
                    std::string strPublicIp;
                    pChild->GetAttribute("PUBLIC_IP", &strPublicIp);                    
                    unsigned long ulPublicIP = sc::net::inetAddr(strPublicIp.c_str());
                    StringCchCopy(m_sFIELDServer[nChannelNumber][nNumber].szPublicIp, MAX_IP_LENGTH+1, strPublicIp.c_str());
                    m_sFIELDServer[nChannelNumber][nNumber].ulPublicIp = ulPublicIP;

                    // 내부 IP 세팅
                    std::string strPrivateIp;
                    pChild->GetAttribute("PRIVATE_IP", &strPrivateIp);
                    unsigned long ulPrivateIP = sc::net::inetAddr(strPrivateIp.c_str());
                    StringCchCopy(m_sFIELDServer[nChannelNumber][nNumber].szPrivateIP, MAX_IP_LENGTH+1, strPrivateIp.c_str());
                    m_sFIELDServer[nChannelNumber][nNumber].ulPrivateIP = ulPrivateIP;

                    // Port 세팅
                    int nPort;
                    pChild->GetAttribute("PORT", &nPort);

                    m_sFIELDServer[nChannelNumber][nNumber].nServicePort = nPort;

					// 인던 생성 가능 세팅;
					int nCreateInstanceLand = 0;
					pChild->GetAttribute( "CREATE_INSTANCE_LAND", & nCreateInstanceLand );
					m_sFIELDServer[nChannelNumber][nNumber].bCreateInstanceLand = ( nCreateInstanceLand > 0 ) ? true : false;


                    // 세팅완료 설정
                    m_sFIELDServer[nChannelNumber][nNumber].bLoad = true;

					++m_nMaxFieldNumber;

                    std::string TempPk("false");
                    
					if (m_sFIELDServer[nChannelNumber][nNumber].bPk)
                        TempPk = "true";
                    
					sc::writeLogInfo(
                        sc::string::format(
                        "loaded config: PK %1% Channel[%2%][%3%] %4% %5% %6%",
                        TempPk,
                        nChannelNumber,
                        nNumber,
                        strPublicIp,
                        strPrivateIp,
                        nPort));
                }
            }
        }

//         m_nMaxFieldNumber = 0;
//         for (int i=0; i<FIELDSERVER_MAX; ++i)
//         {
//             if (m_sFIELDServer[0][i].bLoad)
//                 ++m_nMaxFieldNumber;
//         }
    }
    catch (ticpp::Exception& ex)
    {
        sc::writeLogError(std::string(ex.what()));
    }
    return true;
}

bool AgentServerConfigXml::ParseAgentServerInfo()
{
	try
	{
		ticpp::Element* pParent = NULL;
		for ( pParent = (ticpp::Element*) m_pXml->FirstChild("RAN_SERVER")->FirstChild("AGENT_SERVER_LIST");
			pParent;
			pParent = (ticpp::Element*) pParent->NextSiblingElement() )
		{
			ticpp::Element* pChild = NULL;
			for ( pChild = (ticpp::Element*) pParent->FirstChild( false );
				pChild;
				pChild = (ticpp::Element*) pChild->NextSiblingElement( false ) )
			{
				std::string strChannel = pChild->Value();

				// 전장서버 Number;
				if ( strChannel == "WORLDBATTLE" ) 
				{
					int nNumber = -1;
					pChild->GetAttribute( "NUMBER", &nNumber );

					m_nWorldBattleServerNumber = nNumber;
				}
				else if ( strChannel == "AGENT_SERVER" )
				{
					int nNumber; // Agent 번호;
					pChild->GetAttribute( "NUMBER", &nNumber );

					if ( AGENTSERVER_MAX <= nNumber || nNumber < 0 )
					{
						sc::writeLogError( "Invalid Agent Server Group Number!!" );

						return true;
					}

					// Name, 이름;
					std::string strName;
					pChild->GetAttribute( "NAME", &strName );
					StringCchCopy( m_sAGENTServer[nNumber].szServerName, SERVER_NAME_LENGTH+1, strName.c_str() );

					// 공개 IP 세팅;
					std::string strIP;
					pChild->GetAttribute( "IP", &strIP );
					unsigned long ulPublicIP = sc::net::inetAddr( strIP.c_str() );
					StringCchCopy( m_sAGENTServer[nNumber].szPublicIp, MAX_IP_LENGTH+1, strIP.c_str() );
					m_sAGENTServer[nNumber].ulPublicIp = ulPublicIP;

					// Port 세팅;
					int nPort;
					pChild->GetAttribute( "PORT", &nPort );

					m_sAGENTServer[nNumber].nServicePort = nPort;

					// 세팅완료 설정;
					m_sAGENTServer[nNumber].bLoad = true;

					++m_nMaxAgentNumber;

					sc::writeLogInfo(
						sc::string::format(
						"loaded config: AgentServer[%1%] %2% %3%",
						nNumber,
						strIP,
						nPort));
				}
			}
		}
	}
	catch ( ticpp::Exception& ex )
	{
		sc::writeLogError( std::string( ex.what() ) );
	}

	return true;
}

bool AgentServerConfigXml::ParseSubServer()
{
	try
	{
		int nSubServer = 0;

		ticpp::Element* pElement = static_cast< ticpp::Element* >(
			m_pXml->FirstChild("RAN_SERVER")->FirstChild("SUB_SERVER") );
		if ( pElement )
			pElement->GetTextOrDefault< int, int >( &nSubServer, 0 );

		if ( nSubServer == 0 )
		{
			m_bSubServer = false;
			sc::writeLogInfo( std::string( "[ Agent XML ] [ This Server is not sub server ]" ) );
		}
		else
		{
			m_bSubServer = true;
			sc::writeLogInfo( std::string( "[ Agent XML ] [ This Server is sub server ]" ) );
		}

		return true;
	}
	catch ( ticpp::Exception& ex )
	{
		// Report the error
		WriteErrorLog( ex );
		m_bSubServer = false;
		sc::writeLogWarn( std::string( "[ Agent XML ] [ RAN_SERVER/SUB_SERVER does not exist. It will be set to default value:false ]" ) );

		return true;
	}
}

bool AgentServerConfigXml::IsPkChannel(int nChannel) const
{
    if (nChannel < 0 || nChannel >= MAX_CHANNEL_NUMBER)
        return true;
    else
        return m_sFIELDServer[nChannel][0].bPk ? true : false;
}

F_SERVER_INFO* AgentServerConfigXml::GetFieldServer(int nServerID, int nServerChannel)
{
    if (nServerID < 0 || 
        nServerID >= FIELDSERVER_MAX ||
        nServerChannel < 0 ||
        nServerChannel >= MAX_CHANNEL_NUMBER)
    {
        return NULL;
    }
    else
    {
        return &m_sFIELDServer[nServerChannel][nServerID];
    }
}

A_SERVER_INFO* AgentServerConfigXml::GetAgentServer ( int nServerID )
{
	if ( nServerID < 0 || 
		nServerID >= AGENTSERVER_MAX )
	{
		return NULL;
	}
	else
	{
		return &m_sAGENTServer[ nServerID ];
	}
}

// F_SERVER_INFO* AgentServerConfigXml::GetFieldServerArray()
// { 
//     return &m_sFIELDServer[0][0];
// }

bool AgentServerConfigXml::ParseKorShutdown()
{
    try
    {
        int KorShutdown = 0;
        ticpp::Element* pElem = (ticpp::Element*) m_pXml->FirstChild("RAN_SERVER")->FirstChild("KOR_SHUTDOWN");
        pElem->GetTextOrDefault<int, int> (&KorShutdown, 0);        
        if (KorShutdown == 1)
        {
            m_bKorShutdown = true;
            sc::writeLogInfo(std::string("loaded config: KOR_SHUTDOWN 1"));
        }
        else
        {
            m_bKorShutdown = false;
            sc::writeLogInfo(std::string("loaded config: KOR_SHUTDOWN 0"));
        }
        return true;
    }
    catch (ticpp::Exception& ex)
    {
        // Report the error
        WriteWarningLog(ex);
        m_bKorShutdown = false;
        sc::writeLogWarn(
            std::string(
                "RAN_SERVER/KOR_SHUTDOWN does not exist. It will be set to default value:false"));
        return true;
    }
}

bool AgentServerConfigXml::ParseAutoClubJoin()
{
	try
	{
		int AutoClubJoin = 0;
		ticpp::Element* pElem = ( ticpp::Element* ) m_pXml->FirstChild( "RAN_SERVER" )->FirstChild( "AUTO_CLUB_JOIN" );
		pElem->GetTextOrDefault< int, int > ( &AutoClubJoin, 0 );
		if ( AutoClubJoin == 1 )
		{
			m_bAutoClubJoin = true;
			sc::writeLogInfo( std::string( "loaded config: AUTO_CLUB_JOIN 1" ) );
		}
		else
		{
			m_bAutoClubJoin = false;
			sc::writeLogInfo(std::string( "loaded config: AUTO_CLUB_JOIN 0" ) );
		}

		return true;
	}
	catch ( ticpp::Exception& ex )
	{
		// Report the error
		WriteWarningLog( ex );
		m_bAutoClubJoin = false;
		sc::writeLogWarn(
			std::string(
			"RAN_SERVER/AUTO_CLUB_JOIN does not exist. It will be set to default value:0"));
		return true;
	}
}

bool AgentServerConfigXml::ParseAutoClubDbNum()
{
	try
	{
		ticpp::Element* pElem = NULL;		
		pElem = ( ticpp::Element* ) m_pXml->FirstChild( "RAN_SERVER" )->FirstChild( "AUTO_JOIN_CLUB_NUM" );
		pElem->GetAttribute( "SACRED_GATE", &m_nSacredGateSchoolClubNum );
		pElem->GetAttribute( "MYSTIC_PEAK", &m_nMysticPeakSchoolClubNum );
		pElem->GetAttribute( "PHOENIX", &m_nPhoenixSchoolClubNum );
		sc::writeLogInfo(
			sc::string::format(
			"loaded config: AUTO_JOIN_CLUB_NUM SACRED_GATE %1% PHOENIX %2% MYSTIC_PEAK %3%", m_nSacredGateSchoolClubNum, m_nPhoenixSchoolClubNum, m_nMysticPeakSchoolClubNum) );
		return true;
	}
	catch ( ticpp::Exception& ex )
	{
		// Report the error
		sc::writeLogError( std::string( ex.what() ) );
		sc::writeLogError( std::string( "RAN_SERVER/AUTO_JOIN_CLUB_NUM not exist" ) );        
		return true;
	}
}

bool AgentServerConfigXml::ParsePARTY_2OTHERSCHOOL()
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

bool AgentServerConfigXml::ParseCLUB_2OTHERSCHOOL()
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

bool AgentServerConfigXml::ParseITEM_DROP_SCALE()
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

bool AgentServerConfigXml::ParseMONEY_DROP_SCALE()
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

bool AgentServerConfigXml::ParseITEM_DROP_LOWER_SCALE()
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

bool AgentServerConfigXml::ParseCRITICAL_MAX()
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

bool AgentServerConfigXml::ParseNONPK_ITEM_DROP()
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

bool AgentServerConfigXml::ParsePK_ITEM_DROP()
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

bool AgentServerConfigXml::ParseUseRanMobile()
{
	try
	{
		int Value = 0;
		ticpp::Element* pElem = ( ticpp::Element* ) m_pXml->FirstChild( "RAN_SERVER" )->FirstChild( "bUseRanMobile" );
		pElem->GetTextOrDefault< int, int > ( &Value, 0 );
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

bool AgentServerConfigXml::ParseWhiteRockServer()
{
    try
    {        
        ticpp::Element* pElem = (ticpp::Element*) m_pXml->FirstChild("RAN_SERVER")->FirstChild("WHITEROCK");
        pElem->GetAttribute("NAME", &m_WhiteRockServer.Address);
        pElem->GetAttribute("PORT", &m_WhiteRockServer.Port);
        sc::writeLogInfo(
            sc::string::format(
                "loaded config: WHITEROCK %1% %2%", m_WhiteRockServer.Address, m_WhiteRockServer.Port));
        return true;
    }
    catch (ticpp::Exception& ex)
    {
        // Report the error
        WriteErrorLog(ex);
        m_WhiteRockServer.Address = DefaultWhiteRockName;
        m_WhiteRockServer.Port = ServerConfig::DEFAULT_WHITE_LOCK_PORT;
        sc::writeLogWarn(
            sc::string::format(
                "RAN_SERVER/WHITEROCK_NAME does not exist. It will be set to default value:%1% %2%",
                DefaultWhiteRockName,
                ServerConfig::DEFAULT_WHITE_LOCK_PORT));
        return false;
    }
}

int AgentServerConfigXml::GetAutoClubJoinClubDbNum( WORD wSchool )
{
	if ( false == m_bAutoClubJoin )
	{
		return 0;
	}

	int nClubDbNum = 0;
	switch ( wSchool )
	{
	case 0:
		nClubDbNum = m_nSacredGateSchoolClubNum;
		break;
	case 1:
		nClubDbNum = m_nMysticPeakSchoolClubNum;
		break;
	case 2:
		nClubDbNum = m_nPhoenixSchoolClubNum;
		break;
	default:
		sc::writeLogError( std::string( "GetAutoClubJoinClubDbNum school num error." ) );
		break;
	}
	return nClubDbNum;
}