#pragma once

#include "./GLCountryDefine.h"

namespace Country
{

	/************************************************************************/
	/* Country Data Class                                                   */
	/* 하나의 국가 관련 정보를 담고 있는 클래스;                        */
	/************************************************************************/
	class GLCountry
	{
	public:
		GLCountry () : m_wCountry ( 0 ) { }
		~GLCountry () { }

	public:
		// 국가 정보를 설정한다;
		inline void SetCountry ( COUNTRY_ID wCountry ) { m_wCountry = wCountry; }
		inline void SetCountryName ( const std::string& strCountryName ) { m_strCountryName = strCountryName; }
		inline void SetCountryImageID ( const std::string& strImageID ) { m_strImageID = strImageID; }
		inline void SetCountryMiniImageID ( const std::string& strMiniImageID ) { m_strMiniImageID = strMiniImageID; }
		inline void SetCountryImageFileName ( const std::string& strImageFileName ) { m_strImageFileName = strImageFileName; }

		// 대륙 정보를 설정한다;
		inline void SetContinent ( CONTINENT_ID wContinent ) { m_wContinent = wContinent; }

	public:
		// 국가 정보를 가져온다;
		inline SCOUNTRY_INFO GetCountryInfo () const { return SCOUNTRY_INFO( m_wContinent, m_wCountry ); }
		inline const DWORD GetCountryID () const { return GetCountryInfo().dwID; }
		inline const COUNTRY_ID GetCountry () const { return m_wCountry; }
		inline const std::string& GetCountryName () const { return m_strCountryName; }
		inline const std::string& GetCountryImageID () const { return m_strImageID; }
		inline const std::string& GetCountryMiniImageID () const { return m_strMiniImageID; }
		inline const std::string& GetCountryImageFileName () const { return m_strImageFileName; }

		// 대륙 정보를 가져온다;
		inline const CONTINENT_ID GetContinent () const { return m_wContinent; }

	private:
		// 국가 정보;
		COUNTRY_ID m_wCountry;
		std::string m_strCountryName;
		std::string m_strImageID;
		std::string m_strMiniImageID;
		std::string m_strImageFileName;

		// 대륙 정보;
		CONTINENT_ID m_wContinent;
	};

}