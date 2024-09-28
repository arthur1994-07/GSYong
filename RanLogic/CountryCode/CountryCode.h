#pragma once

#include "./CountryCodeDefine.h"

namespace CountryCode
{
	extern HRESULT LoadFile(const std::string& _stringFileName, const bool _bPastLoad);

	class Manager
	{
	public:
		__forceinline const unsigned int getCodeCountry(const std::string& _stringAddressIP) const;

	private:
		__forceinline void _setGeoIP(GeoIP* const _pGeoIP = NULL);

	public:
		__forceinline const CountryCode::Manager& operator =(const CountryCode::Manager& _rhs){}
		static CountryCode::Manager& getInstance(void);
		~Manager(void);

	private:
		Manager(void);

	private:
		friend HRESULT LoadFile(const std::string& _stringFileName, const bool _bPastLoad);
		GeoIP* pGeoIP;
	};
}

#include "./CountryCode.hpp"
