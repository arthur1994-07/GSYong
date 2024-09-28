#pragma once

#include "./GLContinent.h"

namespace Country
{

	/************************************************************************/
	/* Country Manager ( Virtual Class )                                    */
	/* Server and Client have to implemented. ( Singleton Pattern )         */
	/************************************************************************/
	class GLCountryMan
	{
	protected:
		typedef std::vector< GLContinent* >		CONTINENT_VEC;
		typedef CONTINENT_VEC::iterator			CONTINENT_VEC_ITER;
		typedef CONTINENT_VEC::const_iterator	CONTINENT_VEC_CITER;

	public:
		GLCountryMan () { }
		virtual ~GLCountryMan () { Destroy(); }

	public:
		// 가상 함수 - 스크립트 파일을 읽는 역할을 한다;
		virtual bool LoadData ();

	public:
		// 파괴;
		void Destroy ();

	public:
		// 국가가 이미 존재하는지 확인한다;
		bool ExistCountry ( GLCountry* pCountry ) const;
		bool ExistCountry ( CONTINENT_ID wContinent, COUNTRY_ID wCountry ) const;

		// 대륙이 이미 존재하는지 확인한다;
		bool ExistContinent ( GLContinent* pContinent ) const;

		// 대륙을 추가한다;
		bool AddContinent ( GLContinent* pContinent );

		// 국가 정보를 가져온다;
		GLCountry* GetCountryUsingID ( DWORD dwCountryID ) const;
		GLCountry* GetCountry ( CONTINENT_ID wContinent, COUNTRY_ID wCountry ) const;
		GLCountry* GetCountry ( const std::string& strCountryName ) const;

		// 대륙 정보를 가져온다;
		GLContinent* GetContinent ( CONTINENT_ID wContinent ) const;
		GLContinent* GetContinent ( std::string& strContinentName ) const;
		GLContinent* GetContinentUsingIndex ( int index ) const;

		// 총 대륙 개수를 가져온다;
		DWORD GetContinentCount ();

	protected:
		// 대륙 정보;
		CONTINENT_VEC m_vecContinent;
	};

}