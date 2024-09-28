#pragma once

namespace CountryCode
{
	__forceinline void Manager::_setGeoIP(GeoIP* const _pGeoIP)
	{
		this->pGeoIP = _pGeoIP;
	}

	__forceinline const unsigned int Manager::getCodeCountry(const std::string& _stringAddressIP) const
	{
		if ( this->pGeoIP == NULL )
			return 0;

		const char* const _cCodeCountry(::GeoIP_country_code3_by_addr(this->pGeoIP, _stringAddressIP.c_str()));
		if ( _cCodeCountry == NULL )
			return 0;

		const unsigned int _codeCountry(*(const unsigned int* const)_cCodeCountry);
		return _codeCountry;
	}
}