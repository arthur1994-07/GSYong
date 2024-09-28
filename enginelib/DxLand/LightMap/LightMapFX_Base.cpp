#include "pch.h"

#include "../../../SigmaCore/File/SerialFile.h"

#include "../../DxTools/DxViewPort.h"
#include "../../DxTools/DxFogMan.h"
#include "../../G-Logic/GLPeriod.h"

#include "LightMapFX_Base.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

extern BOOL g_bWIREFRAME_VIEW;

LightMapFX_Base::LightMapFX_Base()
	: m_pFX(NULL)
{

}

LightMapFX_Base::~LightMapFX_Base()
{
	CleanUp();
}

void LightMapFX_Base::CleanUp()
{
	SAFE_RELEASE( m_pFX );
}

HRESULT LightMapFX_Base::LoadFX( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr(S_OK);

	CleanUp();

	DWORD dwShaderFlags = D3DXFX_NOT_CLONEABLE;

#ifdef DEBUG_VS
	dwShaderFlags |= D3DXSHADER_FORCE_VS_SOFTWARE_NOOPT;
#endif
#ifdef DEBUG_PS
	dwShaderFlags |= D3DXSHADER_FORCE_PS_SOFTWARE_NOOPT;
#endif

	TSTRING strFX;
	SetFX( strFX );

	UINT dwBufferSize = static_cast<UINT>( strFX.size() ) + 1; 
	V_RETURN( D3DXCreateEffect( pd3dDevice, strFX.c_str(), dwBufferSize, NULL, NULL, D3DXFX_NOT_CLONEABLE, NULL, &m_pFX, NULL ) );

	return hr;
}

void LightMapFX_Base::FrameMove( const D3DXMATRIX& matVP )
{
	if ( !m_pFX )
		return;

	D3DXVECTOR4 vDayNightWeight4(GLPeriod::GetInstance().GetBlendFact(),1.f-GLPeriod::GetInstance().GetBlendFact(),1.f,1.f);
	m_pFX->SetVector( "g_vDayNightWeight", &vDayNightWeight4 );

	// Fog
	D3DXVECTOR4 vFog;
	if ( g_bWIREFRAME_VIEW )
	{
		vFog = D3DXVECTOR4
		(
			2.f, 
			1.f,
			1.f,
			1.f
		);
	}
	else
	{
		vFog = D3DXVECTOR4
		(
			DxFogMan::GetInstance().GetFogEnd_RealTime(), 
			DxFogMan::GetInstance().GetFogEnd_RealTime() - DxFogMan::GetInstance().GetFogStart_RealTime(),
			1.f,
			1.f
		);
	}
	
	m_pFX->SetVector( "g_vFOG", &vFog );

	m_matVP = matVP;
}

void LightMapFX_Base::ShortBlackFog()
{
	if ( !m_pFX )
		return;

	// Fog
	D3DXVECTOR4 vFog
	(
		2.f, 
		1.f,
		1.f,
		1.f
	);
	m_pFX->SetVector( "g_vFOG", &vFog );
}

void LightMapFX_Base::SetBaseTexture( LPDIRECT3DTEXTUREQ pTexture )
{
	if ( !m_pFX )
		return;

	if ( pTexture )
	{
		m_pFX->SetTexture( "BaseTex", pTexture );
	}
}

void LightMapFX_Base::SetLightMapTexture( LPDIRECT3DTEXTUREQ pTextureDay, LPDIRECT3DTEXTUREQ pTextureNight, const D3DXVECTOR4& vUV_Multiply )
{
	if ( !m_pFX )
		return;

	if ( pTextureDay )
	{
		m_pFX->SetTexture( "LightMapDayTex", pTextureDay );
	}
	if ( pTextureNight )
	{
		m_pFX->SetTexture( "LightMapNightTex", pTextureNight );
	}

	m_pFX->SetVector( "g_vLightMapUV_Multiply", &vUV_Multiply );
}

void LightMapFX_Base::SetLightMapUV_Offset( const D3DXVECTOR2& vLightMapUV_Offset )
{
	if ( !m_pFX )
		return;

	D3DXVECTOR4 vLightMapUV_Offset4( vLightMapUV_Offset.x, vLightMapUV_Offset.y, 1.f, 1.f );
	m_pFX->SetVector( "g_vLightMapUV_Offset", &vLightMapUV_Offset4 );
}

void LightMapFX_Base::SetWorld( const D3DXMATRIX& matWorld, float fAlpha )
{
	if ( !m_pFX )
		return;

	D3DXMATRIX matWVP;
	D3DXMatrixMultiply( &matWVP, &matWorld, &m_matVP );
	m_pFX->SetMatrixTranspose( "g_matWVP",		&matWVP );
	m_pFX->SetMatrixTranspose( "g_matWORLD",	&matWorld );
}
