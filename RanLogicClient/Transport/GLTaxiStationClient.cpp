#include "pch.h"

#include "../../RanLogicClient/Transport/GLTaxiStationClient.h"

#include "../../RanLogicClient/GLGaeaClient.h"
#include "../../RanLogic/Transport/GLTaxiStation.h"
#include "../RanGfxUI/GfxInterface.h"

GLTaxiStationClient::GLTaxiStationClient(void)
{

}

GLTaxiStationClient::GLTaxiStationClient( const GLTaxiStationClient& value )
{

}

GLTaxiStationClient::~GLTaxiStationClient(void)
{

}

GLTaxiStationClient& GLTaxiStationClient::GetInstance ()
{
	static GLTaxiStationClient _instance;
	return _instance;
}

const bool GLTaxiStationClient::OpenTaxiCard( WORD wPosX, WORD wPosY )
{
	m_wPosX = wPosX; m_wPosY = wPosY;

	GLWidgetScript::GetInstance().LuaCallFunc< bool >( NSWIDGET_SCRIPT::g_strFunc_TaxiCard_Open );

	return true;
}

const int GLTaxiStationClient::GetMapInfoSize(void) const
{
	return GLTaxiStation::GetInstance().m_vecTaxiMap.size();
}

lua_tinker::table GLTaxiStationClient::GetMapInfo(void)
{
	lua_tinker::table tbTaixMapInfo( GLWidgetScript::GetInstance().GetLuaState() );

	size_t nSize = GLTaxiStation::GetInstance().m_vecTaxiMap.size();

	for( size_t i = 0; i < nSize; ++i )
	{
		const STAXI_MAP& pMapInfo = GLTaxiStation::GetInstance().m_vecTaxiMap[ i ];

		lua_tinker::table tbStation( GLWidgetScript::GetInstance().GetLuaState() );
		for( size_t j = 0; j < pMapInfo.vecTaxiStation.size(); ++j )
		{
			const STAXI_STATION& pStationInfo = pMapInfo.vecTaxiStation[ j ];
			tbStation.set( j + 1, pStationInfo.strSTATION );	
		}

		lua_tinker::table tbMapInfo( GLWidgetScript::GetInstance().GetLuaState() );
		tbMapInfo.set( 1, pMapInfo.strMAP );
		tbMapInfo.set( 2, tbStation );

		tbTaixMapInfo.set( i + 1, tbMapInfo );
	}

	return tbTaixMapInfo;
}

const LONGLONG GLTaxiStationClient::GetCalcTaxiCharge( int nSelectMap, int nSelectStop ) const
{
	LONGLONG llCharge = 0;
	GLGaeaClient* pClient = GfxInterface::GetInstance()->GetGaeaClient();
	if(pClient)
	{
		if ( nSelectStop < 0 || nSelectMap < 0 ) return llCharge;
		llCharge = pClient->GetCharacter()->GetCalcTaxiCharge(nSelectMap, nSelectStop);
	}

	return llCharge;
}

const bool GLTaxiStationClient::ReqTaxiStation( int nSelectMap, int nSelectStop ) const
{

	GLGaeaClient* pClient = GfxInterface::GetInstance()->GetGaeaClient();
	if(pClient)
	{
		if ( nSelectStop < 0 || nSelectMap > GLTaxiStation::GetInstance().m_vecTaxiMap.size() ) return false;
		pClient->GetCharacter()->ReqTaxiStation ( static_cast<WORD>(m_wPosX), static_cast<WORD>(m_wPosY), nSelectMap, nSelectStop );

		return true;
	}

	return false;
}