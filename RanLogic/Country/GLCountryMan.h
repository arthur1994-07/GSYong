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
		// ���� �Լ� - ��ũ��Ʈ ������ �д� ������ �Ѵ�;
		virtual bool LoadData ();

	public:
		// �ı�;
		void Destroy ();

	public:
		// ������ �̹� �����ϴ��� Ȯ���Ѵ�;
		bool ExistCountry ( GLCountry* pCountry ) const;
		bool ExistCountry ( CONTINENT_ID wContinent, COUNTRY_ID wCountry ) const;

		// ����� �̹� �����ϴ��� Ȯ���Ѵ�;
		bool ExistContinent ( GLContinent* pContinent ) const;

		// ����� �߰��Ѵ�;
		bool AddContinent ( GLContinent* pContinent );

		// ���� ������ �����´�;
		GLCountry* GetCountryUsingID ( DWORD dwCountryID ) const;
		GLCountry* GetCountry ( CONTINENT_ID wContinent, COUNTRY_ID wCountry ) const;
		GLCountry* GetCountry ( const std::string& strCountryName ) const;

		// ��� ������ �����´�;
		GLContinent* GetContinent ( CONTINENT_ID wContinent ) const;
		GLContinent* GetContinent ( std::string& strContinentName ) const;
		GLContinent* GetContinentUsingIndex ( int index ) const;

		// �� ��� ������ �����´�;
		DWORD GetContinentCount ();

	protected:
		// ��� ����;
		CONTINENT_VEC m_vecContinent;
	};

}