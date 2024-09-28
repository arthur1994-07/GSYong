#pragma once

#include "./GLCountry.h"

#include "../../SigmaCore/Lua/MinLua.h"

namespace Country
{

	/************************************************************************/
	/* Continent Data Class                                                 */
	/* �ϳ��� ����� �ǹ��ϴ� Ŭ����, ����� ���ϴ� ������ �����Ѵ�;   */
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
		// ������ �߰��Ѵ�;
		bool AddCountry ( GLCountry* pCountry );

		// ������ �����ϴ��� Ȯ���Ѵ�;
		bool ExistCountry ( GLCountry* pCountry ) const;
		bool ExistCountryUsingID ( COUNTRY_ID wCountry ) const;

		// ���� ������ ���´�;
		GLCountry* GetCountryUsingID ( DWORD dwCountryID ) const;
		GLCountry* GetCountry ( COUNTRY_ID wCountry ) const;
		GLCountry* GetCountry ( const std::string& strCountryName ) const;
		GLCountry* GetCountryUsingIndex ( int index ) const;

		// �� ���� ������ ���´�;
		DWORD GetCountryCount () const;

		const COUNTRY_VEC GetCountryList () const;

	public:
		// Log ���;
		void LogContinentInfo ();

	public:
		// ��� ������ �����Ѵ�;
		inline void SetContinent ( CONTINENT_ID wContinent ) { m_wContinent = wContinent; }
		inline void SetContinentName ( const std::string& strContinentName ) { m_strContinentName = strContinentName; }

	public:
		// ��� ������ ���´�;
		inline const CONTINENT_ID GetContinent () const { return m_wContinent; }
		inline const std::string& GetContinentName () const { return m_strContinentName; }

	private:
		// ����� ���� ���� ���;
		COUNTRY_VEC m_vecCountry;

		// ��� ����;
		CONTINENT_ID m_wContinent;
		std::string m_strContinentName;
	};

}