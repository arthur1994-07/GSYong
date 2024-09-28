#include "pch.h"

#include "../../../SigmaCore/File/SerialFile.h"
#include "../../DxTools/TextureManager.h"
#include "../../DxTools/DxCubeMap.h"

#include "./DxTexEffSpecular.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

const DWORD DxTexEffSpecular::TYPEID = DEF_TEXEFF_SPECULAR;
const DWORD	DxTexEffSpecular::VERSION = 0x00000101;
const char DxTexEffSpecular::NAME[] = "[ Specular ]";

// Note : TEXEFF_SPECULAR_PROPERTY
TEXEFF_SPECULAR_PROPERTY::TEXEFF_SPECULAR_PROPERTY()
{
}

TEXEFF_SPECULAR_PROPERTY::~TEXEFF_SPECULAR_PROPERTY()
{
}

void TEXEFF_SPECULAR_PROPERTY::Save( sc::SerialFile& SFile )
{
	SFile << strTex;
}

void TEXEFF_SPECULAR_PROPERTY::Load( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile )
{
	SFile >> strTex;

    m_textureRes = TextureManager::GetInstance().LoadTexture(
        strTex,
        false,
        TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
        0,
        D3DFMT_UNKNOWN,
        TEXTURE_RESIZE_NONE,
        TEXTURE_EFFECT,
		false,
		FALSE );
}

// Note : DxTexEffSpecular
LPDIRECT3DSTATEBLOCK9	DxTexEffSpecular::m_pSavedSB = NULL;
LPDIRECT3DSTATEBLOCK9	DxTexEffSpecular::m_pDrawSB = NULL;

DxTexEffSpecular::DxTexEffSpecular()
{
}

DxTexEffSpecular::~DxTexEffSpecular()
{
}

void DxTexEffSpecular::SetProperty( LPDIRECT3DDEVICEQ pd3dDevice, PBYTE &pProp )
{
	TEXEFF_SPECULAR_PROPERTY* pTemp = (TEXEFF_SPECULAR_PROPERTY*)pProp;
	m_sProp.strTex		= pTemp->strTex;

    m_sProp.m_textureRes = TextureManager::GetInstance().LoadTexture(
        m_sProp.strTex,
        false,
        TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
        0,
        D3DFMT_UNKNOWN,
        TEXTURE_RESIZE_NONE,
        TEXTURE_EFFECT,
		false,
		FALSE );
}

void DxTexEffSpecular::OnCreateDevice( LPDIRECT3DDEVICEQ pd3dDevice )
{
	D3DCAPSQ d3dCaps;
	pd3dDevice->GetDeviceCaps ( &d3dCaps );

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		//	Note : SetRenderState() 識情
		pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,		FALSE );
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	TRUE );
		pd3dDevice->SetRenderState( D3DRS_FOGCOLOR,			0x00000000 );

		pd3dDevice->SetRenderState( D3DRS_NORMALIZENORMALS, TRUE );
		pd3dDevice->SetRenderState( D3DRS_SRCBLEND,			D3DBLEND_ONE );
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND,		D3DBLEND_ONE );

		//	Note : SetTextureStageState() 識情
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,	D3DTOP_SELECTARG1 );
		pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,	D3DTOP_MODULATE );

		pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX,			D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR );
		pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXTURETRANSFORMFLAGS,	D3DTTFF_COUNT3 );

		if( which==0 )	pd3dDevice->EndStateBlock ( &m_pSavedSB );
		else			pd3dDevice->EndStateBlock ( &m_pDrawSB );
	}
}

void DxTexEffSpecular::OnReleaseDevice( LPDIRECT3DDEVICEQ pd3dDevice )
{
	SAFE_RELEASE( m_pSavedSB );
	SAFE_RELEASE( m_pDrawSB );
}

void DxTexEffSpecular::FrameMove( const float fElapsedTime )	{}

void DxTexEffSpecular::Render( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMESH pMesh, const DWORD dwAttrib )
{
	SetRenderBegin( pd3dDevice );
	pMesh->DrawSubset( dwAttrib );
	SetRenderEnd( pd3dDevice );
}

void DxTexEffSpecular::SetRenderBegin( LPDIRECT3DDEVICEQ pd3dDevice )
{
	pd3dDevice->SetTexture( 0, m_sProp.m_textureRes.GetTexture() );
	pd3dDevice->SetTexture( 1, DxCubeMap::GetInstance().GetCubeTexTEST() );
	
	m_pSavedSB->Capture();
	m_pDrawSB->Apply();
}

void DxTexEffSpecular::SetRenderEnd( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pSavedSB->Apply();

	pd3dDevice->SetTexture( 1, NULL );
}

void DxTexEffSpecular::SavePSF( sc::SerialFile& SFile )
{
	SFile << VERSION;

	SFile.BeginBlock();
	m_sProp.Save( SFile );
	SFile.EndBlock();
}

BOOL DxTexEffSpecular::LoadPSF( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile )
{
	DWORD dwSize;
	DWORD dwVer;
	SFile >> dwVer;
	SFile >> dwSize;

	if( dwVer==VERSION )
	{
		m_sProp.Load( pd3dDevice, SFile );
	}
	else if( dwVer == 0x00000100 )
	{
		m_sProp.Load( pd3dDevice, SFile );
	}
	else
	{
		DWORD dwCurBuffer = SFile.GetfTell();
		SFile.SetOffSet( dwCurBuffer+dwSize );
	}

	return TRUE;
}
