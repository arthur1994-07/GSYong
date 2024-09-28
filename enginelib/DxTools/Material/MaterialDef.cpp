#include "pch.h"

#include "MaterialDef.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


//	--------------------------------------------------------------------------------------------------------------------
//													Textures
Textures::Textures( const std::string strParameter, const TSTRING& strTexture )
	: m_strParameter(strParameter)
	, m_strTexture(strTexture)
{
};

void Textures::LoadTexture( TEXTURE_ADD_DATA::TYPE sType, bool bThreadLoading )
{
	m_textureRes = TextureManager::GetInstance().LoadTexture( m_strTexture,
		false,
		sType,
		0,
		D3DFMT_UNKNOWN,
		TEXTURE_RESIZE_NONE,
		TEXTURE_MATERIALS,
		bThreadLoading,
		FALSE );
}


//	--------------------------------------------------------------------------------------------------------------------
//													Constants
Constants::Constants( const std::string strParameter, const D3DXVECTOR4& vValue )
	: m_strParameter(strParameter)
	, m_vValue(vValue)
{
};

Constants::Constants( const std::string strParameter, const float* pValue )
	: m_strParameter(strParameter)
	, m_vValue(pValue[0],pValue[1],pValue[2],pValue[3])
{
};


//	--------------------------------------------------------------------------------------------------------------------
//													MaterialSmall
MaterialSmall::MaterialSmall( DWORD dwVer )
	: m_dwVer(dwVer)
	, m_dwFVF(0L)
	, m_pDCRT(NULL)
	, m_pELMT(NULL)
	, m_bTangent(FALSE)
	, m_bBinormal(FALSE)
{

}

MaterialSmall::~MaterialSmall()
{
	SAFE_RELEASE ( m_pDCRT );
	SAFE_DELETE_ARRAY ( m_pELMT );
}