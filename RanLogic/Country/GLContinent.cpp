#include "pch.h"

#include "./GLContinent.h"

namespace Country
{

	bool GLContinent::Init ( const char* szFile )
	{
		if ( !DefaultInit( szFile ) )
			return false;

		m_wContinent = GetGlobal< CONTINENT_ID >( "Continent" );
		m_strContinentName = GetGlobal< const char* >( "ContinentName" );

		lua_tinker::table tableCountryList = GetGlobal< lua_tinker::table >( "Country" );
		for ( int i=0; i<tableCountryList.TableLen(); ++i )
		{
			lua_tinker::table tableCountry = tableCountryList.get< lua_tinker::table >( i+1 );

			GLCountry* pCountry = new GLCountry();
			if ( NULL == pCountry )
			{
				return false;
			}

			pCountry->SetCountry( tableCountry.get< COUNTRY_ID >( 1 ) );
			pCountry->SetCountryName( tableCountry.get< const char* >( 2 ) );
			pCountry->SetCountryImageID( tableCountry.get< const char* >( 3 ) );
			pCountry->SetCountryMiniImageID ( tableCountry.get< const char* >( 4 ) );
			pCountry->SetCountryImageFileName( tableCountry.get< const char* >( 5 ) );

			pCountry->SetContinent( m_wContinent );

			AddCountry( pCountry );
		}

		return true;
	}

	bool GLContinent::Reload ()
	{
		return true;
	}

	void GLContinent::Destroy ()
	{
		for ( COUNTRY_VEC_ITER iter = m_vecCountry.begin();
			iter != m_vecCountry.end(); ++iter )
		{
			SAFE_DELETE( *iter );
		}

		m_vecCountry.clear();
	}

	bool GLContinent::AddCountry ( GLCountry* pCountry )
	{
		if ( NULL == pCountry )
			return false;

		// 이미 있는 국가인지 확인한다;
		if ( ExistCountry( pCountry ) )
		{
			// 지운다;
			delete pCountry;

			/************************************************************************/
			/* 이미 있는 국가를 추가하려고 한 경우이다;                         */
			/* 스크립트에 잘못된 정보가 들어가 있다는 것이다;                   */
			/************************************************************************/	
			sc::writeLogError(
				"[ Country Log ] [ Exist Country!! ( GLContinent::AddCountry ) ]" );

			// ReleaseD의 경우 ASSERT;
			GASSERT ( 0 && "[ Country Log ] [ Exist Country!! ( GLContinent::AddCountry ) ]" );

			return false;
		}

		// 국가를 대륙에 추가한다;
		m_vecCountry.push_back( pCountry );

		return true;
	}

	bool GLContinent::ExistCountry ( GLCountry* pCountry ) const
	{
		if ( NULL == pCountry )
			return false;

		for ( COUNTRY_VEC_CITER iter = m_vecCountry.begin();
			iter != m_vecCountry.end(); ++iter )
		{
			GLCountry* pCountryTemp = *iter;
			if ( NULL == pCountryTemp )
				return false;

			if ( pCountryTemp->GetCountry() == pCountry->GetCountry() )
				return true;
		}

		return false;
	}

	bool GLContinent::ExistCountryUsingID ( COUNTRY_ID wCountry ) const
	{
		for ( COUNTRY_VEC_CITER iter = m_vecCountry.begin();
			iter != m_vecCountry.end(); ++iter )
		{
			GLCountry* pCountryTemp = *iter;
			if ( NULL == pCountryTemp )
				return false;

			if ( pCountryTemp->GetCountry() == wCountry )
				return true;
		}

		return false;
	}

	GLCountry* GLContinent::GetCountryUsingID ( DWORD dwCountryID ) const
	{
		for ( COUNTRY_VEC_CITER iter = m_vecCountry.begin();
			iter != m_vecCountry.end(); ++iter )
		{
			GLCountry* pCountryReturn = *iter;
			if ( NULL == pCountryReturn )
				return NULL;

			if ( pCountryReturn->GetCountryID() == dwCountryID )
				return pCountryReturn;
		}

		return NULL;
	}

	GLCountry* GLContinent::GetCountry ( COUNTRY_ID wCountry ) const
	{
		for ( COUNTRY_VEC_CITER iter = m_vecCountry.begin();
			iter != m_vecCountry.end(); ++iter )
		{
			GLCountry* pCountryReturn = *iter;
			if ( NULL == pCountryReturn )
				return NULL;

			if ( pCountryReturn->GetCountry() == wCountry )
				return pCountryReturn;
		}

		return NULL;
	}

	GLCountry* GLContinent::GetCountry ( const std::string& strCountryName ) const
	{
		for ( COUNTRY_VEC_CITER iter = m_vecCountry.begin();
			iter != m_vecCountry.end(); ++iter )
		{
			GLCountry* pCountryReturn = *iter;
			if ( NULL == pCountryReturn )
				return NULL;

			if ( pCountryReturn->GetCountryName() == strCountryName )
				return pCountryReturn;
		}

		return NULL;
	}

	GLCountry* GLContinent::GetCountryUsingIndex ( int index ) const
	{
		if ( index >= 0 && index < m_vecCountry.size() )
			return m_vecCountry.at( index );

		return NULL;
	}

	DWORD GLContinent::GetCountryCount () const
	{
		return static_cast< DWORD >( m_vecCountry.size() );
	}

	const GLContinent::COUNTRY_VEC GLContinent::GetCountryList () const
	{
		return m_vecCountry;
	}

	void GLContinent::LogContinentInfo ()
	{
		sc::writeLogInfo ( "[ Country Log ] [ --------------------------- ]" );
		sc::writeLogInfo ( "[ Country Log ] [ Continent Info ]" );
		sc::writeLogInfo ( sc::string::format(
			"[ Country Log ] -> [ Continent ID : %1% ]", m_wContinent ) );
		sc::writeLogInfo ( sc::string::format( 
			"[ Country Log ] -> [ Continent Name : %1% ]", m_strContinentName ) );
		sc::writeLogInfo ( "[ Country Log ] [ --------------------------- ]" );

		for ( COUNTRY_VEC_CITER iter = m_vecCountry.begin();
			iter != m_vecCountry.end(); ++iter )
		{
			GLCountry* pCountry = *iter;
			if ( NULL == pCountry )
				return;

			sc::writeLogInfo ( "[ Country Log ] [ Country Info ]" );
			sc::writeLogInfo ( sc::string::format(
				"[ Country Log ] -> [ Country ID : %1% ]", pCountry->GetCountry() ) );
			sc::writeLogInfo ( sc::string::format(
				"[ Country Log ] -> [ Country Name : %1% ]", pCountry->GetCountryName() ) );
		}

		sc::writeLogInfo ( "[ Country Log ] [ --------------------------- ]" );
	}

}