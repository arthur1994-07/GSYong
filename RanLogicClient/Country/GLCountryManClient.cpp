#include "pch.h"

#include "../../EngineLib/Common/SUBPATH.h"

#include "../../SigmaCore/String/StringFormat.h"
#include "../../SigmaCore/Util/SystemInfo.h"

#include "../../RanLogic/GLogicDataMan.h"
#include "../../RanLogic/Msg/GLContrlCountry.h"

#include "./GLCountryManClient.h"

namespace Country
{

	bool GLCountryManClient::LoadData ()
	{
		GLCountryMan::LoadData();

		return true;
	}

	LuaTable GLCountryManClient::GetContinentList()
	{
		LuaTable tbList( GLWidgetScript::GetInstance().GetLuaState() );

		int nTableIndex = 1;
		for( DWORD i = 0; i < GetContinentCount(); ++i )
		{
			Country::GLContinent* pContinent = GetContinentUsingIndex( i );
			if( NULL == pContinent )
				continue;

			LuaTable tbData( GLWidgetScript::GetInstance().GetLuaState() );
			tbData.set( 1, pContinent->GetContinent() );
			tbData.set( 2, pContinent->GetContinentName() );
			tbList.set( nTableIndex++, tbData );
		}

		return tbList;
	}

	LuaTable GLCountryManClient::GetCountryList( DWORD dwContinentID )
	{
		LuaTable tbList( GLWidgetScript::GetInstance().GetLuaState() );

		Country::GLContinent* pContinent = GetContinent( dwContinentID );
		if( NULL == pContinent )
			return tbList;

		int nTableIndex = 1;
		for( DWORD i = 0; i < pContinent->GetCountryCount(); ++i )
		{
			Country::GLCountry* pCountry = pContinent->GetCountryUsingIndex( i );
			if( NULL == pCountry )
				continue;

			LuaTable tbData( GLWidgetScript::GetInstance().GetLuaState() );
			tbData.set( 1, pCountry->GetCountry() );
			tbData.set( 2, pCountry->GetCountryName() );
			tbList.set( nTableIndex++, tbData );
		}

		return tbList;
	}
}