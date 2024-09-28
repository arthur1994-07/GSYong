#include "pch.h"

#include "../../../SigmaCore/File/SerialFile.h"
#include "../../../SigmaCore/Math/Random.h"

#include "../../G-Logic/GLDefine.h"
#include "../../G-Logic/glperiod.h"
#include "../../DxTools/TextureManager.h"
#include "../../DxTools/Light/DxLightMan.h"

#include "./DxTexEffDiffuse.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

const DWORD DxTexEffDiffuse::TYPEID = DEF_TEXEFF_DIFFUSE;
const DWORD	DxTexEffDiffuse::VERSION = 0x00000103;
const char DxTexEffDiffuse::NAME[] = "[ Color  Up,Down ]";

// Note : TEXEFF_DIFFUSE_PROPERTY
TEXEFF_DIFFUSE_PROPERTY::TEXEFF_DIFFUSE_PROPERTY() :
	bSpeed(TRUE),
	fSpeed(1.f),
	fAlpha(1.f),
	bNightLight(FALSE)
{
}

TEXEFF_DIFFUSE_PROPERTY::~TEXEFF_DIFFUSE_PROPERTY()
{
}

void TEXEFF_DIFFUSE_PROPERTY::Save( sc::SerialFile& SFile )
{
	SFile << strTex;
	SFile << fSpeed;
	SFile << bSpeed;
	SFile << fAlpha;
	SFile << bNightLight;
}

void TEXEFF_DIFFUSE_PROPERTY::Load( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile )
{
	SFile >> strTex;
	SFile >> fSpeed;
	SFile >> bSpeed;
	SFile >> fAlpha;	
	SFile >> bNightLight;

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


void TEXEFF_DIFFUSE_PROPERTY::Load_102( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile )
{
	SFile >> strTex;
	SFile >> fSpeed;
	SFile >> bSpeed;
	SFile >> fAlpha;	

	bNightLight = FALSE;

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

void TEXEFF_DIFFUSE_PROPERTY::Load_101( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile )
{
	SFile >> bSpeed;
	SFile >> fSpeed;
	SFile >> fAlpha;
	SFile >> strTex;

	bNightLight = FALSE;

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
	

// Note : DxTexEffDiffuse
LPDIRECT3DSTATEBLOCK9	DxTexEffDiffuse::m_pSavedSB = NULL;
LPDIRECT3DSTATEBLOCK9	DxTexEffDiffuse::m_pDrawSB = NULL;

DxTexEffDiffuse::DxTexEffDiffuse() :
	m_bUp(TRUE),
	m_fTime(-D3DX_PI),
	m_fRandSpeed(1.f),
	m_dwColor(0L)
{
}

DxTexEffDiffuse::~DxTexEffDiffuse()
{
}

void DxTexEffDiffuse::SetProperty( LPDIRECT3DDEVICEQ pd3dDevice, PBYTE &pProp )
{
	TEXEFF_DIFFUSE_PROPERTY* pTemp = (TEXEFF_DIFFUSE_PROPERTY*)pProp;
	m_sProp.bSpeed		= pTemp->bSpeed;
	m_sProp.fSpeed		= pTemp->fSpeed;
	m_sProp.fAlpha		= pTemp->fAlpha;
	m_sProp.strTex		= pTemp->strTex;
	m_sProp.bNightLight = pTemp->bNightLight;

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

void DxTexEffDiffuse::OnCreateDevice( LPDIRECT3DDEVICEQ pd3dDevice )
{
	D3DCAPSQ d3dCaps;
	pd3dDevice->GetDeviceCaps ( &d3dCaps );

	//// Anisotropic Filter 확인
	//DWORD dwMinFilter;
	//if( d3dCaps.TextureFilterCaps & D3DPTFILTERCAPS_MINFANISOTROPIC )	dwMinFilter = D3DTEXF_ANISOTROPIC;
	//else if( d3dCaps.TextureFilterCaps & D3DPTFILTERCAPS_MINFLINEAR )	dwMinFilter = D3DTEXF_LINEAR;
	//else																dwMinFilter = D3DTEXF_POINT;

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		//if( dwMinFilter == D3DTEXF_ANISOTROPIC )
		//{
		//	pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, dwMinFilter );
		//	pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_NONE );
		//}

		// Note : SetRenderState()	 변해도 상관없음.
		pd3dDevice->SetRenderState( D3DRS_SRCBLEND,			D3DBLEND_ONE );
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND,		D3DBLEND_ONE );
		pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,	0xffffffff );

		// Note : SetRenderState()
		//float fBias = -0.00002f;
		//pd3dDevice->SetRenderState( D3DRS_DEPTHBIAS,		*((DWORD*)&fBias) );
		pd3dDevice->SetRenderState( D3DRS_LIGHTING,			FALSE );
		pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,		FALSE );
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	TRUE );
		pd3dDevice->SetRenderState( D3DRS_FOGCOLOR,			0x00000000 );

		// Note : SetTextureStageState()
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2,	D3DTA_TFACTOR );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,	D3DTOP_MODULATE );

		if ( which==0 )	pd3dDevice->EndStateBlock ( &m_pSavedSB );
		else			pd3dDevice->EndStateBlock ( &m_pDrawSB );
	}
}

void DxTexEffDiffuse::OnReleaseDevice( LPDIRECT3DDEVICEQ pd3dDevice )
{
	SAFE_RELEASE( m_pSavedSB );
	SAFE_RELEASE( m_pDrawSB );
}

void DxTexEffDiffuse::FrameMove( const float fElapsedTime )
{
	// Note : 알파가 변하게 할 것인가 ?
	if( m_sProp.bSpeed )
	{
		// Note : fTime을 더해준다.
		m_fTime += m_sProp.fSpeed * m_fRandSpeed * fElapsedTime;

		// Note : Random Speed Setting
		if( m_fTime > 0.f && m_bUp )		
		{
			m_bUp = FALSE;
			m_fRandSpeed = (sc::Random::RANDOM_POS() * 5.f + 1.f);
		}
		else if( m_fTime>D3DX_PI && !m_bUp )
		{
			m_fTime -= D3DX_PI*2.f;	// 값을 빼준다.
			m_bUp = TRUE;
			m_fRandSpeed = (sc::Random::RANDOM_POS() * 5.f + 1.f);
		}

		// Note : 오르락 내리락
		float fSpeed = cosf( m_fTime );
		fSpeed += 1.f;
		fSpeed *= 0.5f;

		// Note : Color 설정
		m_dwColor = (DWORD)(fSpeed*m_sProp.fAlpha*255.f);
		m_dwColor = 0xff000000 + (m_dwColor<<16) + (m_dwColor<<8) + m_dwColor;
	}
	else
	{
		// Note : Color 설정
		m_dwColor = (DWORD)(m_sProp.fAlpha*255.f);
		m_dwColor = 0xff000000 + (m_dwColor<<16) + (m_dwColor<<8) + m_dwColor;
	}	
}

void DxTexEffDiffuse::Render( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMESH pMesh, const DWORD dwAttrib )
{
	SetRenderBegin( pd3dDevice );
	pMesh->DrawSubset( dwAttrib );
	SetRenderEnd( pd3dDevice );
}

void DxTexEffDiffuse::SetRenderBegin( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( m_sProp.bNightLight && GLPeriod::GetInstance().IsOffLight() ) 		return;

	pd3dDevice->SetTexture( 0, m_sProp.m_textureRes.GetTexture() );
	
	m_pSavedSB->Capture();
	m_pDrawSB->Apply();

	pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, m_dwColor );
}

void DxTexEffDiffuse::SetRenderEnd( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( m_sProp.bNightLight && GLPeriod::GetInstance().IsOffLight() )	return;
	m_pSavedSB->Apply();
}

void DxTexEffDiffuse::SavePSF( sc::SerialFile& SFile )
{
	SFile << VERSION;

	SFile.BeginBlock();
	m_sProp.Save( SFile );
	SFile.EndBlock();
}

BOOL DxTexEffDiffuse::LoadPSF( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile )
{
	DWORD dwSize;
	DWORD dwVer;
	SFile >> dwVer;
	SFile >> dwSize;

	if( dwVer==VERSION )
	{
		m_sProp.Load( pd3dDevice, SFile );
	}else if( dwVer==0x00000102 )
	{
		m_sProp.Load_102( pd3dDevice, SFile );
	}
	else if( dwVer==0x00000101 )
	{
		m_sProp.Load_101( pd3dDevice, SFile );
	}
	else if( dwVer==0x00000100 )
	{
		SFile >> m_sProp.fSpeed;
		SFile >> m_sProp.strTex;

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

		// Note : Ver.101 에서 추가됨.
		m_sProp.bSpeed = TRUE;
		m_sProp.fAlpha = 1.f;
	}
	else
	{
		DWORD dwCurBuffer = SFile.GetfTell();
		SFile.SetOffSet( dwCurBuffer+dwSize );
	}

	return TRUE;
}
