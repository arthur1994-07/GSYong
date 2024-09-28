#pragma once

#include <string>
#include <map>

namespace UIControlList
{
	struct SControl
	{
		D3DXVECTOR4 m_vPos;
		std::string m_strTextureID;

		SControl()
			: m_vPos( 0.0f, 0.0f, 0.0f, 0.0f )
		{
		}
	};

	typedef std::map< std::string, SControl > ControlMap;
	typedef ControlMap::iterator ControlMapIter;
	typedef ControlMap::value_type ControlMapValue;

	class CXmlLoader
	{
	public:
		bool Load( const std::string& strPath, const std::string& strListName, ControlMap& mapControl );
	};
}