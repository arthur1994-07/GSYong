#include "pch.h"

#include "../../DxTools/DxDynamicVB.h"
#include "../../DxTools/TextureManager.h"

#include "DxEffProjTexture.h"



LPDIRECT3DSTATEBLOCK9 DxEffProjTexture::m_pSavedRS_STATIC(NULL);
LPDIRECT3DSTATEBLOCK9 DxEffProjTexture::m_pDrawRS_STATIC(NULL);



DxEffProjTexture::DxEffProjTexture( const TCHAR* szName, float fCreatingTime, float fDestroyingTime, BOOL bEnableON )
	: DxEffProjBase( bEnableON, fCreatingTime, fDestroyingTime )
	, m_strTexName(szName)
{
}

DxEffProjTexture::~DxEffProjTexture()
{
}



//////////////////////////////////////////////////////////////////////////
//								STATIC
void DxEffProjTexture::OnResetDevice_STATIC( LPDIRECT3DDEVICEQ pd3dDevice )
{
	SAFE_RELEASE( m_pSavedRS_STATIC );
	SAFE_RELEASE( m_pDrawRS_STATIC );

	// Create the state blocks for rendering text
	for( int which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,		TRUE );
		pd3dDevice->SetRenderState( D3DRS_SRCBLEND,				D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND,			D3DBLEND_INVSRCALPHA );

		pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,			FALSE );
		pd3dDevice->SetRenderState( D3DRS_ZENABLE,				FALSE );
		pd3dDevice->SetRenderState( D3DRS_FOGENABLE,			FALSE );
		pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,		0xffffffff );

		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1,	D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,	D3DTOP_SELECTARG1 );

		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1,	D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2,	D3DTA_TFACTOR );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,	D3DTOP_MODULATE );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSavedRS_STATIC );
		else			pd3dDevice->EndStateBlock( &m_pDrawRS_STATIC );
	}
}

void DxEffProjTexture::OnLostDevice_STATIC()
{
	SAFE_RELEASE( m_pSavedRS_STATIC );
	SAFE_RELEASE( m_pDrawRS_STATIC );
}
//								STATIC
//////////////////////////////////////////////////////////////////////////



BOOL DxEffProjTexture::OnCreateDevice( LPDIRECT3DDEVICEQ pd3dDevice )
{
	DxEffProjBase::OnCreateDevice( pd3dDevice );

	if ( !m_strTexName.empty() )
	{
		m_textureRes = TextureManager::GetInstance().LoadTexture
		(
			m_strTexName,
			false,
			TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
			1,
			D3DFMT_UNKNOWN,
			TEXTURE_RESIZE_NONE,
			TEXTURE_EFFECT,
			true,
			FALSE 
		);
	}

	return TRUE;
}

void DxEffProjTexture::OnDestroyDevice()
{
	m_textureRes.Clear();
}

// BackBuffer 에 Screen 효과를 렌더링 하도록 한다.
void DxEffProjTexture::RenderPostEffect( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if( !m_bPlayON )
        return;

    if( !m_textureRes.Update() )
        return;

	m_pSavedRS_STATIC->Capture();
	m_pDrawRS_STATIC->Apply();

	DWORD dwAlpha = static_cast<DWORD>( m_fAlpha * 255.f );
	if ( dwAlpha > 255 )
		dwAlpha = 255;

	pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(dwAlpha,dwAlpha,dwAlpha,dwAlpha) );

	pd3dDevice->SetTexture( 0, m_textureRes.GetTexture() );

	RenderEX( pd3dDevice );

	m_pSavedRS_STATIC->Apply();
}

void DxEffProjTexture::RenderEX( LPDIRECT3DDEVICEQ pd3dDevice )
{
	DWORD dwVertexSizeFULL = 4*DxDynamicVB::g_sVB_I_24BYTE.GetFVFSize();
    if( !DxDynamicVB::g_sVB_I_24BYTE.IsValidSize( dwVertexSizeFULL ) )
        return;

    UINT uiStartVertex = DxDynamicVB::g_sVB_I_24BYTE.CopyToVB(
	    m_sVertex,
	    dwVertexSizeFULL,
	    4 );

	pd3dDevice->SetFVF( D3DFVF_XYZRHW|D3DFVF_TEX1 );
	pd3dDevice->SetStreamSource( 0, DxDynamicVB::g_sVB_I_24BYTE.pVB, 0, DxDynamicVB::g_sVB_I_24BYTE.GetFVFSize() );

	pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, uiStartVertex, 2 );
}

