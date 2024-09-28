#include "StdAfx.h"
#include "GfxScene.h"

GfxScene::GfxScene(void)
{
}

GfxScene::~GfxScene(void)
{
	m_pMovie.Clear();
}

HRESULT GfxScene::OnResetDevice( LPDIRECT3DDEVICE9 pDevice )
{
	D3DVIEWPORT9 vp;
	pDevice->GetViewport( &vp );

	// 최소화 상태다. 걍 리턴하자.
	if( 10 == vp.Width && 10 == vp.Height )
		return S_OK;

	if( NULL != m_pMovie )
		m_pMovie->SetViewport( vp.Width, vp.Height, 0, 0, vp.Width, vp.Height );

	return S_OK;
}

HRESULT GfxScene::OnDestroyDevice()
{
	return S_OK;
}

HRESULT GfxScene::OnFrameMove( float fElapsedTime )
{
	// 로스트 디바이스 상태에서 advance 호출하면 창 복구시 프레임이 엄청 떨어진다.
	if( true == GetCore()->IsLostDevice() )
		return E_FAIL;

	if( NULL == m_pMovie )
		return E_FAIL;

	m_pMovie->Advance( fElapsedTime, 0 );

	return S_OK;
}

HRESULT GfxScene::OnFrameRender( float fElapsedTime )
{
	Render::D3D9::HAL* pRenderHAL = GetCore()->GetRenderHAL();
	if( NULL == pRenderHAL )
		return E_FAIL;

	pRenderHAL->BeginFrame();

	if( true == m_hMovie.NextCapture( pRenderHAL->GetContextNotify() ) )
		pRenderHAL->Display( m_hMovie );

	pRenderHAL->EndFrame();

	return S_OK;
}

HRESULT GfxScene::LoadMovie( LPDIRECT3DDEVICE9 pDevice )
{
	m_pMovieDef = *( GetCore()->GetLoader().CreateMovie( m_strFilePath.c_str(), Loader::LoadWaitFrame1 ) );
	if( NULL == m_pMovieDef )
		return E_FAIL;

	m_pMovie = *m_pMovieDef->CreateInstance( true, 0, 0, GetCore()->GetRenderThread() );
	if( NULL == m_pMovie )
		return E_FAIL; 

	m_pMovie->SetMouseCursorCount( 1 );
	m_pMovie->SetControllerCount( 1 );
	m_pMovie->Advance( 0.0f, 0 );
	m_pMovie->SetBackgroundAlpha( 0.0f );
	m_pMovie->HandleEvent( GFx::SetFocusEvent() );

	D3DVIEWPORT9 vp;
	if( D3D_OK == pDevice->GetViewport( &vp ) )
		m_pMovie->SetViewport( vp.Width, vp.Height, 0, 0, vp.Width, vp.Height );
	else
		return E_FAIL;

	m_hMovie = m_pMovie->GetDisplayHandle();

	return S_OK;
}