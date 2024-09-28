#include "pch.h"

#include "../../EngineLib/Common/SUBPATH.h"

#include "../../SigmaCore/String/StringFormat.h"
#include "../../SigmaCore/Util/SystemInfo.h"

#include "../../RanLogic/GLogicDataMan.h"
#include "../../RanLogic/Msg/GLContrlCountry.h"

#include "./GLCountryMan.h"

namespace Country
{

	bool GLCountryMan::LoadData ()
	{
		// Initialize Lua;
		sc::lua_init();

		// Script 파일의 주소;
		SUBPATH* pPath = GLogicData::GetInstance().GetPath();
		if( !pPath )
		{
			sc::writeLogError( "[ Country Log ] [ Invalid Path!! ]" );

			GASSERT ( 0 && "[ Country Log ] [ Invalid Path!! ]" );

			return false;
		}

		// Path;
		std::string strScriptPath( sc::getAppPath() );
		strScriptPath += pPath->GLogicPath();

		if( !sc::lua_doFile(
			strScriptPath +
			GLogicData::GetInstance().GetCountrySystemFileName() ) )
			return false;

		// [shhan][2015.07.21] LuaPlus::LuaObject basicGroup 값이 소멸되는 시점이 lua_destroyState 보다는 빨리 되야 한다.
		// 
		{
			LuaPlus::LuaObject basicGroup =
				sc::lua_getGlobalFromName( "Continent" );
			if ( basicGroup.IsNil() )
			{
				sc::writeLogError( "[ Country Log ] [ Failed Load 'Continent'. ]" );

				GASSERT( 0 && "[ Country Log ] [ Failed Load 'Continent'. ]" );

				return false;
			}

			for ( LuaPlus::LuaTableIterator iter( basicGroup );
				iter; iter.Next() )
			{
				// 각 대륙 파일 이름을 얻는다;
				std::string strTempFileName = strScriptPath;
				strTempFileName += sc::GetStringByObject( iter.GetValue() );

				GLContinent* pNewContinent = new GLContinent();
				if ( NULL == pNewContinent )
					return false;

				// 대륙을 세팅한다;
				pNewContinent->Init( strTempFileName.c_str() );

				// 대륙을 추가한다;
				AddContinent ( pNewContinent );

				// 로그를 남긴다;
				pNewContinent->LogContinentInfo();
			}
		}

		// Destroy Lua;
		sc::lua_destroyState();

		return true;
	}

	void GLCountryMan::Destroy ()
	{
		for ( CONTINENT_VEC_ITER iter = m_vecContinent.begin();
			iter != m_vecContinent.end(); ++iter )
		{
			SAFE_DELETE( *iter );
		}

		m_vecContinent.clear();
	}

	bool GLCountryMan::ExistCountry ( GLCountry* pCountry ) const
	{
		if ( NULL == pCountry )
			return false;

		for ( CONTINENT_VEC_CITER iter = m_vecContinent.begin();
			iter != m_vecContinent.end(); ++iter )
		{
			GLContinent* pContinentTemp = *iter;
			if ( NULL == pContinentTemp )
				return false;

			if ( pContinentTemp->GetContinent() == pCountry->GetContinent() )
			{
				return pContinentTemp->ExistCountry( pCountry );
			}
		}

		return false;
	}

	bool GLCountryMan::ExistCountry ( CONTINENT_ID wContinent, COUNTRY_ID wCountry ) const
	{
		for ( CONTINENT_VEC_CITER iter = m_vecContinent.begin();
			iter != m_vecContinent.end(); ++iter )
		{
			GLContinent* pContinentTemp = *iter;
			if ( NULL == pContinentTemp )
				return false;

			if ( pContinentTemp->GetContinent() == wContinent )
			{
				return pContinentTemp->ExistCountryUsingID( wCountry );
			}
		}

		return false;
	}

	bool GLCountryMan::ExistContinent ( GLContinent* pContinent ) const
	{
		if ( NULL == pContinent )
			return false;

		for ( CONTINENT_VEC_CITER iter = m_vecContinent.begin();
			iter != m_vecContinent.end(); ++iter )
		{
			GLContinent* pContinentReturn = *iter;
			if ( NULL == pContinentReturn )
				return false;

			if ( pContinentReturn->GetContinent() == pContinent->GetContinent() )
			{
				return true;
			}
		}

		return false;
	}

	bool GLCountryMan::AddContinent ( GLContinent* pContinent )
	{
		if ( NULL == pContinent )
			return false;

		// 이미 있는 국가인지 확인한다;
		if ( ExistContinent( pContinent ) )
		{
			// 지운다;
			delete pContinent;

			/************************************************************************/
			/* 이미 있는 대륙을 추가하려고 한 경우이다;                         */
			/* 스크립트에 잘못된 정보가 들어가 있다는 것이다;                   */
			/************************************************************************/	
			sc::writeLogError(
				"[ Country Log ] [ Exist Continent!! ( GLCountryManServer::LoadData ) ]" );

			// ReleaseD의 경우 ASSERT;
			GASSERT ( 0 && "[ Country Log ] [ Exist Continent!! ( GLCountryManServer::LoadData ) ]" );

			return false;
		}

		// 대륙을 추가한다;
		m_vecContinent.push_back( pContinent );

		return true;
	}

	GLCountry* GLCountryMan::GetCountryUsingID ( DWORD dwCountryID ) const
	{
		for ( CONTINENT_VEC_CITER iter = m_vecContinent.begin();
			iter != m_vecContinent.end(); ++iter )
		{
			GLContinent* pContinentTemp = *iter;
			if ( NULL == pContinentTemp )
				return NULL;

			GLCountry* pCountryReturn =
				pContinentTemp->GetCountryUsingID( dwCountryID );
			if ( pCountryReturn )
				return pCountryReturn;
		}

		return NULL;
	}

	GLCountry* GLCountryMan::GetCountry (
		CONTINENT_ID wContinent, COUNTRY_ID wCountry ) const
	{
		for ( CONTINENT_VEC_CITER iter = m_vecContinent.begin();
			iter != m_vecContinent.end(); ++iter )
		{
			GLContinent* pContinentTemp = *iter;
			if ( NULL == pContinentTemp )
				return NULL;

			if ( pContinentTemp->GetContinent() == wContinent )
			{
				GLCountry* pCountryReturn =
					pContinentTemp->GetCountry( wCountry );

				return pCountryReturn;
			}
		}

		return NULL;
	}

	GLCountry* GLCountryMan::GetCountry ( 
		const std::string& strCountryName ) const
	{
		for ( CONTINENT_VEC_CITER iter = m_vecContinent.begin();
			iter != m_vecContinent.end(); ++iter )
		{
			GLContinent* pContinentTemp = *iter;
			if ( NULL == pContinentTemp )
				return NULL;

			GLCountry* pCountryReturn =
				pContinentTemp->GetCountry( strCountryName );

			return pCountryReturn;
		}

		return NULL;
	}

	GLContinent* GLCountryMan::GetContinent (
		CONTINENT_ID wContinent ) const
	{
		for ( CONTINENT_VEC_CITER iter = m_vecContinent.begin();
			iter != m_vecContinent.end(); ++iter )
		{
			GLContinent* pContinentReturn = *iter;
			if ( NULL == pContinentReturn )
				return NULL;

			if ( pContinentReturn->GetContinent() == wContinent )
			{
				return pContinentReturn;
			}
		}

		return NULL;
	}

	GLContinent* GLCountryMan::GetContinent (
		std::string& strContinentName ) const
	{
		for ( CONTINENT_VEC_CITER iter = m_vecContinent.begin();
			iter != m_vecContinent.end(); ++iter )
		{
			GLContinent* pContinentReturn = *iter;
			if ( NULL == pContinentReturn )
				return NULL;

			if ( pContinentReturn->GetContinentName() == strContinentName )
			{
				return pContinentReturn;
			}
		}

		return NULL;
	}

	GLContinent* GLCountryMan::GetContinentUsingIndex ( int index ) const
	{
		if ( index >= 0 && index < m_vecContinent.size() )
			return m_vecContinent.at( index );

		return NULL;
	}

	DWORD GLCountryMan::GetContinentCount ()
	{
		return static_cast< DWORD >( m_vecContinent.size() );
	}

}