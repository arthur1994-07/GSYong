#pragma once

#include "./GLCountry.h"

#include "../../SigmaCore/Lua/MinLua.h"

namespace Country
{

	/************************************************************************/
	/* Continent Data Class                                                 */
	/* 하나의 대륙을 의미하는 클래스, 대륙에 속하는 국가를 관리한다;   */
	/************************************************************************/
	class GLContinent : public sc::IScriptBase
	{
	public:
		typedef std::vector< GLCountry* >		COUNTRY_VEC;
		typedef COUNTRY_VEC::iterator			COUNTRY_VEC_ITER;
		typedef COUNTRY_VEC::const_iterator		COUNTRY_VEC_CITER;

	public:
		GLContinent () : m_wContinent ( 0 ) { }
		~GLContinent () { Destroy(); }

	public:
		virtual bool Init ( const char* szFile );
		virtual bool Reload ();

		virtual void Destroy ();

	public:
		// 국가를 추가한다;
		bool AddCountry ( GLCountry* pCountry );

		// 국가가 존재하는지 확인한다;
		bool ExistCountry ( GLCountry* pCountry ) const;
		bool ExistCountryUsingID ( COUNTRY_ID wCountry ) const;

		// 국가 정보를 얻어온다;
		GLCountry* GetCountryUsingID ( DWORD dwCountryID ) const;
		GLCountry* GetCountry ( COUNTRY_ID wCountry ) const;
		GLCountry* GetCountry ( const std::string& strCountryName ) const;
		GLCountry* GetCountryUsingIndex ( int index ) const;

		// 총 국가 개수를 얻어온다;
		DWORD GetCountryCount () const;

		const COUNTRY_VEC GetCountryList () const;

	public:
		// Log 찍기;
		void LogContinentInfo ();

	public:
		// 대륙 정보를 설정한다;
		inline void SetContinent ( CONTINENT_ID wContinent ) { m_wContinent = wContinent; }
		inline void SetContinentName ( const std::string& strContinentName ) { m_strContinentName = strContinentName; }

	public:
		// 대륙 정보를 얻어온다;
		inline const CONTINENT_ID GetContinent () const { return m_wContinent; }
		inline const std::string& GetContinentName () const { return m_strContinentName; }

	private:
		// 대륙에 속한 국가 목록;
		COUNTRY_VEC m_vecCountry;

		// 대륙 정보;
		CONTINENT_ID m_wContinent;
		std::string m_strContinentName;
	};

}