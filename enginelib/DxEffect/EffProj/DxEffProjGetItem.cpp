#include "pch.h"
#include "./DxEffProjGetItem.h"

#include "../../DxTools/DxDynamicVB.h"
#include "../../DxTools/TextureManager.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

#define SEQUENCE_COL		5
#define SEQUENCECOUNT		SEQUENCE_COL*SEQUENCE_COL
#define SEQUENCECOUNT_H		int(SEQUENCECOUNT*0.5f)

DxEffProjGetItem::DxEffProjGetItem() :
	m_pSavedRS(NULL),
	m_pDrawRS(NULL),
	m_bEnableON(FALSE),
	m_nCount00(0),
	m_fAlpha(0.f)
{
	m_strTexName = "Vortex00.dds";
}

DxEffProjGetItem::~DxEffProjGetItem()
{
}

void DxEffProjGetItem::OnCreateDevice( LPDIRECT3DDEVICEQ pd3dDevice )
{
    m_textureRes = TextureManager::GetInstance().LoadTexture(
        m_strTexName,
        false,
        TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
        1,
        D3DFMT_UNKNOWN,
        TEXTURE_RESIZE_NONE,
        TEXTURE_EFFECT,
		true,
		FALSE );

	m_sMaterial.Diffuse.a = m_sMaterial.Diffuse.r = m_sMaterial.Diffuse.g = m_sMaterial.Diffuse.b = 1.f;
	m_sMaterial.Ambient.a = m_sMaterial.Ambient.r = m_sMaterial.Ambient.g = m_sMaterial.Ambient.b = 1.f;
	m_sMaterial.Specular.a = m_sMaterial.Specular.r = m_sMaterial.Specular.g = m_sMaterial.Specular.b = 1.f;
	m_sMaterial.Emissive.a = m_sMaterial.Emissive.r = m_sMaterial.Emissive.g = m_sMaterial.Emissive.b = 0.f;

	for( DWORD i=0; i<4; ++i )
	{
		m_sVertex[i].vPos = D3DXVECTOR4( 0.f, 0.f, 1.f, 1.f );
		m_sVertex[i].vTex = D3DXVECTOR2( 0.f, 0.f );
	}
}

void DxEffProjGetItem::OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice )
{
	LPDIRECT3DSURFACEQ	pBackBuffer;
	D3DSURFACE_DESC		d3dsdBackBuffer;
	pd3dDevice->GetBackBuffer ( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer );

    pBackBuffer->GetDesc( &d3dsdBackBuffer );
	float dwWidth = (float)d3dsdBackBuffer.Width;
	float dwHeight = (float)d3dsdBackBuffer.Height;
    pBackBuffer->Release();

	m_sVertex[0].vPos = D3DXVECTOR4( 0.f, 0.f, 1.f, 1.f );
	m_sVertex[1].vPos = D3DXVECTOR4( dwWidth, 0.f, 1.f, 1.f );
	m_sVertex[2].vPos = D3DXVECTOR4( 0.f, dwHeight, 1.f, 1.f );
	m_sVertex[3].vPos = D3DXVECTOR4( dwWidth, dwHeight, 1.f, 1.f );

	m_sVertex[0].vTex = D3DXVECTOR2( 0.f, 0.f);	// 아마 시퀀스 쓰면 계속 변해야 할 것이다.
	m_sVertex[1].vTex = D3DXVECTOR2( 1.f, 0.f );
	m_sVertex[2].vTex = D3DXVECTOR2( 0.f, 1.f );
	m_sVertex[3].vTex = D3DXVECTOR2( 1.f, 1.f );

	// Create the state blocks for rendering text
	for( int which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,		TRUE );
		pd3dDevice->SetRenderState( D3DRS_SRCBLEND,				D3DBLEND_ONE );
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND,			D3DBLEND_ONE );

		pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,			FALSE );
		pd3dDevice->SetRenderState( D3DRS_ZENABLE,				FALSE );
		pd3dDevice->SetRenderState( D3DRS_FOGENABLE,			FALSE );
		pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,		0xffffffff );

		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1,	D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2,	D3DTA_TFACTOR );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,	D3DTOP_MODULATE );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSavedRS );
		else			pd3dDevice->EndStateBlock( &m_pDrawRS );
	}
}

void DxEffProjGetItem::OnLostDevice( LPDIRECT3DDEVICEQ pd3dDevice )
{
	SAFE_RELEASE( m_pSavedRS );
	SAFE_RELEASE( m_pDrawRS );
}

void DxEffProjGetItem::OnDestroyDevice()
{
	m_textureRes.Clear();
}

void DxEffProjGetItem::FrameMove( float fElapsedTime )
{
	if( m_bEnableON )	// 활성화 하라
	{
		m_bPlayON = TRUE;		// Play 해라.
		m_fAlpha += fElapsedTime;
		if( m_fAlpha > 1.f )	m_fAlpha = 1.f;
	}
	else				// 비활성화 하라.
	{
		m_fAlpha -= fElapsedTime;
		if( m_fAlpha < 0.f )
		{
			m_bPlayON = FALSE;	// Play 하지 않음.
			m_fAlpha = 0.f;

			// Note : 시퀀스 번호
			m_nCount00 = 0;
		}
	}

	m_fElapsedTime += fElapsedTime;
	if( m_fElapsedTime < 0.066f )	return;	// 너무 작은 순간이면 작업 안함.

	m_fElapsedTime = 0.f;

	if( m_bPlayON )
	{
		m_nCount00 += 1;
		if( m_nCount00 >= SEQUENCECOUNT )	m_nCount00 = 0;
	}
}

void DxEffProjGetItem::Render( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if( !m_bPlayON )
        return;

    if( !m_textureRes.Update() )
        return;

	m_pSavedRS->Capture();
	m_pDrawRS->Apply();

	DWORD dwAlpha = static_cast<DWORD>( m_fAlpha * 255.f );
	if ( dwAlpha > 255 )
		dwAlpha = 255;

	pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, D3DCOLOR_XRGB(dwAlpha,dwAlpha,dwAlpha) );

	pd3dDevice->SetTexture( 0L, m_textureRes.GetTexture() );

	SequnceMake( m_nCount00 );
	RenderEX( pd3dDevice );

	m_pSavedRS->Apply();
}

void DxEffProjGetItem::SequnceMake( int nCount )
{
	int nCol = 	nCount % SEQUENCE_COL;
	int nRow = 	nCount / SEQUENCE_COL;
	float fCol = (float)nCol/SEQUENCE_COL;
	float fRow = (float)nRow/SEQUENCE_COL;
	float fOne = 1.f/SEQUENCE_COL;
	float fCol_Add = fCol + fOne;
	float fRow_Add = fRow + fOne;

	float fRate = 1020.f/1024.f;

	fCol		*= fRate;
	fRow		*= fRate;
	fCol_Add	*= fRate;
	fRow_Add	*= fRate;

	m_sVertex[0].vTex = D3DXVECTOR2( fCol,		fRow );
	m_sVertex[1].vTex = D3DXVECTOR2( fCol_Add,	fRow );
	m_sVertex[2].vTex = D3DXVECTOR2( fCol,		fRow_Add );
	m_sVertex[3].vTex = D3DXVECTOR2( fCol_Add,	fRow_Add );
}

void DxEffProjGetItem::RenderEX( LPDIRECT3DDEVICEQ pd3dDevice )
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

void DxEffProjGetItem::EnableEFF( BOOL bUse )
{
	m_bEnableON = bUse;
}


