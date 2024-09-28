#pragma once

#include <string>
#include <map>

#include "UIDataType.h"

namespace UITextureList
{
	struct SControl
	{
		std::string m_strFileName;
		D3DXVECTOR4 m_vPos;
		D3DXVECTOR2 m_vSize;
		D3DXVECTOR4 m_vBorder;
        UIRECT m_rcTexturePos;

		SControl()
			: m_vPos( 0.0f, 0.0f, 0.0f, 0.0f )
			, m_vSize( 0.0f, 0.0 )
			, m_vBorder( 0.0f, 0.0f, 0.0f, 0.0f )
		{
		}
	};

	typedef std::map< std::string, SControl > ControlMap;
	typedef ControlMap::iterator ControlMapIter;
	typedef ControlMap::value_type ControlMapValue;

	class CXmlLoader
	{
	public:
		bool Load( const std::string& strFilePath, ControlMap& mapControl );
	};

	class CXmlSaver
	{
	public:
		bool Save( const std::string& strFilePath, ControlMap& mapControl );
	};

    class CPsdXmlToUiXml
    {
    public:
        bool AddPsdXml( const std::string& strFilePath, ControlMap& mapControl  );

    };
}