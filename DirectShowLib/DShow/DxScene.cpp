#include "stdafx.h"
#include "DxScene.h"

DxScene::DxScene()
{
}

DxScene::~DxScene()
{
}

void DxScene::Init()
{
	m_fScene[ 0 ][ 0 ] = 0.0f;
	m_fScene[ 0 ][ 1 ] = 0.0f;
	m_fScene[ 0 ][ 2 ] = 0.0f;
	m_fScene[ 0 ][ 3 ] = 1.0f;
	m_fScene[ 0 ][ 4 ] = 0.0f;
	m_fScene[ 0 ][ 5 ] = 0.0f;

	m_fScene[ 1 ][ 0 ] = 0.0f;
	m_fScene[ 1 ][ 1 ] = 0.0f;
	m_fScene[ 1 ][ 2 ] = 0.0f;
	m_fScene[ 1 ][ 3 ] = 1.0f;
	m_fScene[ 1 ][ 4 ] = 1.0f;
	m_fScene[ 1 ][ 5 ] = 0.0f;

	m_fScene[ 2 ][ 0 ] = 0.0f;
	m_fScene[ 2 ][ 1 ] = 0.0f;
	m_fScene[ 2 ][ 2 ] = 0.0f;
	m_fScene[ 2 ][ 3 ] = 1.0f;
	m_fScene[ 2 ][ 4 ] = 0.0f;
	m_fScene[ 2 ][ 5 ] = 1.0f;

	m_fScene[ 3 ][ 0 ] = 0.0f;
	m_fScene[ 3 ][ 1 ] = 0.0f;
	m_fScene[ 3 ][ 2 ] = 0.0f;
	m_fScene[ 3 ][ 3 ] = 1.0f;
	m_fScene[ 3 ][ 4 ] = 1.0f;
	m_fScene[ 3 ][ 5 ] = 1.0f;
}


void DxScene::RenderScene( LPDIRECT3DDEVICE9 pDevice, LPDIRECT3DTEXTURE9 pTexture,
							   float fLeft, float fTop, float fRight, float fBottom ) 
{
	m_fScene[ 0 ][ 0 ] = fLeft;
	m_fScene[ 0 ][ 1 ] = fTop;

	m_fScene[ 1 ][ 0 ] = fRight;
	m_fScene[ 1 ][ 1 ] = fTop;

	m_fScene[ 2 ][ 0 ] = fLeft;
	m_fScene[ 2 ][ 1 ] = fBottom;

	m_fScene[ 3 ][ 0 ] = fRight;
	m_fScene[ 3 ][ 1 ] = fBottom;

	pDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB( 0, 0, 0 ), 1.0f, 0 );

	pDevice->BeginScene();

	pDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
	pDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );
	pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );

	pDevice->SetTexture( 0, pTexture );
	pDevice->SetFVF( D3DFVF_XYZRHW | D3DFVF_TEX1 );
	pDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, m_fScene, sizeof( m_fScene[ 0 ] ) );

	pDevice->EndScene();
}