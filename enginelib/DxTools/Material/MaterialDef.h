#pragma once

#include <vector>
#include <map>

#include "../TextureManager.h"

struct Textures
{
	std::string		m_strParameter;
	TSTRING			m_strTexture;
	TextureResource	m_textureRes;

	Textures( const std::string strParameter, const TSTRING& strTexture );
	void LoadTexture( TEXTURE_ADD_DATA::TYPE sType, bool bThreadLoading );
};

struct Constants
{
	std::string	m_strParameter;
	D3DXVECTOR4 m_vValue;

	Constants( const std::string strParameter, const D3DXVECTOR4& vValue );
	Constants( const std::string strParameter, const float* pValue );
};

struct MaterialSmall
{
	DWORD							m_dwFVF;
	LPDIRECT3DVERTEXDECLARATION9	m_pDCRT;
	D3DVERTEXELEMENT9*				m_pELMT;
	DWORD							m_dwVer;
	BOOL							m_bTangent;
	BOOL							m_bBinormal;

	MaterialSmall( DWORD dwVer );
	~MaterialSmall();
};

struct MaterialValue
{
	float				m_fCubeMapValue;		// Å¥ºê¸Ê Value
	float				m_fSpotSpecPower;		// SpotSpecPower
	float				m_fSpotSpecMulti;		// SpotSpecMulti

	MaterialValue()
		: m_fCubeMapValue(0.f)
		, m_fSpotSpecPower(256.f)
		, m_fSpotSpecMulti(0.f)
	{
	};

	MaterialValue( float fCubeMapValue, float fSpotSpecPower, float fSpotSpecMulti )
		: m_fCubeMapValue(fCubeMapValue)
		, m_fSpotSpecPower(fSpotSpecPower)
		, m_fSpotSpecMulti(fSpotSpecMulti)
	{
	};
};