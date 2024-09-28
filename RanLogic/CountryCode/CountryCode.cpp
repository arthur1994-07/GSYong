#include "pch.h"

#include "../../EngineLib/G-Logic/GLOGIC.h"
#include "./CountryCode.h"

namespace CountryCode
{
	HRESULT LoadFile(const std::string& _stringFileName, const bool _bPastLoad)
	{
		if ( _bPastLoad == false )
			return S_OK;

		std::string _stringPathFull(GLOGIC::GetServerPath());
		_stringPathFull.append(_stringFileName);

		GeoIP* const _pGeoIP(::GeoIP_open(_stringPathFull.c_str(), GEOIP_STANDARD));
		if ( _pGeoIP == NULL )
			return E_FAIL;

		CountryCode::Manager& _managerGeoIP(CountryCode::Manager::getInstance());
		_managerGeoIP._setGeoIP(_pGeoIP);

		return S_OK;
	}


	CountryCode::Manager& Manager::getInstance(void)
	{
		static CountryCode::Manager _instance;
		return _instance;
	}
	Manager::~Manager(void)
	{
		if ( this->pGeoIP != NULL )
		{
			::GeoIP_delete(this->pGeoIP);
			this->pGeoIP = NULL;
		}
	}
	Manager::Manager(void)
		: pGeoIP(NULL)
	{
	}	
}
