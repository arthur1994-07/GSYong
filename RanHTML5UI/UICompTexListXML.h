#pragma once

#include <string>
#include <vector>

#include <GUIBase/UIDataType.h>

namespace HTML5UI
{

namespace UICompTexList
{
	struct SCOMPONENT
	{
		std::string		m_textureID;
		std::string		m_strFileName;
		D3DXVECTOR4		m_vPos;
		D3DXVECTOR2		m_vSize;
		D3DXVECTOR4		m_vBorder;
		GUIBase::UIRECT m_rcTexturePos;

		SCOMPONENT()
			: m_vPos( 0.0f, 0.0f, 0.0f, 0.0f )
			, m_vSize( 0.0f, 0.0 )
			, m_vBorder( 0.0f, 0.0f, 0.0f, 0.0f )
		{
		}
	};

	typedef std::vector< SCOMPONENT >			SCOMPONENT_VECTOR;
	typedef SCOMPONENT_VECTOR::iterator			SCOMPONENT_VECTOR_ITER;
	typedef SCOMPONENT_VECTOR::const_iterator	SCOMPONENT_VECTOR_CITER;

	class CXmlLoader
	{
	public:
		bool Load( const std::string& strFilePath, SCOMPONENT_VECTOR& componentVector );
	};

}

}