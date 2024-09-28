#include "../pch.h"
#pragma warning(disable:4267)
#include "./ServerConfigHelper.h"
#include "./CacheServerConfigXml.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CacheServerConfigXml::CacheServerConfigXml()
: m_bUseStandAlonePointShop( false )
{
}

CacheServerConfigXml::~CacheServerConfigXml()
{
}

bool CacheServerConfigXml::Parse()
{
    if (!ServerConfigXml::Parse())
        return false;

    if (!ServerConfigHelper::ParseSessionServer(m_pXml, m_SessionServer))
        return false;
    
    if (!ServerConfigHelper::ParseDatabase(m_pXml, "LOG_DB", m_LogDb))
        return false;

    if (!ServerConfigHelper::ParseDatabase(m_pXml, "GAME_DB", m_GameDb))
        return false;

    if (!ServerConfigHelper::ParseDatabase(m_pXml, "SHOP_DB", m_ShopDb))
        return false;

	// 란모바일 ADO 연결
	if (!ServerConfigHelper::ParseDatabase(m_pXml, "RANMOBILE_DB", m_RanMobileAdo))
        return false;

	ParseStandAlonePointShop();

    return true;
}

bool CacheServerConfigXml::ParseStandAlonePointShop()
{
	try
	{
		int StandAlone = 0;
		ticpp::Element* pElem = ( ticpp::Element* ) m_pXml->FirstChild( "RAN_SERVER" )->FirstChild( "STAND_ALONE_POINTSHOP" );
		pElem->GetTextOrDefault< int, int > ( &StandAlone, 0 );
		if ( StandAlone == 1 )
		{
			m_bUseStandAlonePointShop = true;
			sc::writeLogInfo( std::string( "loaded config: STAND_ALONE_POINTSHOP 1" ) );
		}
		else
		{
			m_bUseStandAlonePointShop = false;
			sc::writeLogInfo(std::string( "loaded config: STAND_ALONE_POINTSHOP 0" ) );
		}

		return true;
	}
	catch ( ticpp::Exception& ex )
	{
		// Report the error
		WriteWarningLog( ex );
		m_bUseStandAlonePointShop = false;
		sc::writeLogWarn(
			std::string(
			"RAN_SERVER/STAND_ALONE_POINTSHOP does not exist. It will be set to default value:0"));
		return true;
	}
}
