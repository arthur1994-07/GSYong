#include "pch.h"
#include "./D3DViewport.h"

#include "../DxTools/DxBackUpRendTarget.h"

#include "./RenderDevice.h"

#include "../../SigmaCore/DebugInclude.h"

namespace
{
    const BOOL c_bSwapChain = TRUE;
}

CD3DViewport::CD3DViewport()
    : m_bVisible( TRUE )
    , m_bActive( TRUE )
    , m_fZoom( 1.0f )

    , m_cSourceRect( 0, 0, 0, 0 )
    , m_cDestRect( 0, 0, 0, 0 )
    , m_cHitRect( 0, 0, 0, 0 )

    , m_pSwapChain( NULL )
    , m_pBackBuffer( NULL )
{
    m_pBackupTarget = new DxBackupTarget;
}

CD3DViewport::~CD3DViewport()
{
    SAFE_DELETE( m_pBackupTarget );
}

LRESULT CD3DViewport::WindowProc( UINT message, WPARAM wParam, LPARAM lParam )
{
    switch( message )
    {
    case WM_CREATE:
        {
            CalcSourceRect();
            CalcDestRect();
            CalcHitRect();

            CRenderDevice::CreateObject( this );
            CRenderDevice::AttachObject( this );
        }
        break;

    case WM_DESTROY:
        {
            CRenderDevice::DetachObject( this );
            CRenderDevice::DestroyObject( this );
        }
        break;

    case WM_SIZE:
        {
            CalcSourceRect();
            CalcDestRect();
            CalcHitRect();

            CRenderDevice::ResetObject( this );
        }
        break;
    }

    return 0;
}

VOID CD3DViewport::CalcSourceRect()
{
    GetClientRect( GetTargetWnd(), &m_cSourceRect );
}

VOID CD3DViewport::CalcDestRect()
{
    GetClientRect( GetTargetWnd(), &m_cDestRect );
}

VOID CD3DViewport::CalcHitRect()
{
    m_cHitRect = m_cDestRect;
}

HRESULT CD3DViewport::CreateBackBuffer( IDirect3DDevice9* pd3dDevice )
{
    if( !pd3dDevice )
        return E_FAIL;

    HWND hWnd = GetTargetWnd();
    if( !hWnd )
        return E_FAIL;

    const CRect& cDestRect = GetDestRect();
    if( cDestRect.Width() <= 0 || cDestRect.Height() <= 0 )
        return E_FAIL;

    if( c_bSwapChain )
    {
        D3DPRESENT_PARAMETERS sPresentParams;
        ZeroMemory( &sPresentParams, sizeof( sPresentParams ) );
        sPresentParams.BackBufferWidth = cDestRect.Width();
        sPresentParams.BackBufferHeight = cDestRect.Height();
        sPresentParams.SwapEffect = D3DSWAPEFFECT_COPY;
        sPresentParams.hDeviceWindow = hWnd;
        sPresentParams.Windowed = TRUE;

        pd3dDevice->CreateAdditionalSwapChain( &sPresentParams, &m_pSwapChain );
        if( !m_pSwapChain )
            return E_FAIL;

        m_pSwapChain->GetBackBuffer( 0, D3DBACKBUFFER_TYPE_MONO, &m_pBackBuffer );
        if( !m_pBackBuffer )
            return E_FAIL;
    }

    return S_OK;
}

VOID CD3DViewport::DestroyBackBuffer()
{
    SAFE_RELEASE( m_pBackBuffer );
    SAFE_RELEASE( m_pSwapChain );
}

VOID CD3DViewport::BeginScene(
    IDirect3DDevice9* pd3dDevice,
    DWORD nClearColor,
    DWORD nClearFlags,
    FLOAT fClearZ,
    DWORD nClearStencil,
    WORD nBackupTargetFlags )
{
    m_pBackupTarget->Backup( pd3dDevice, nBackupTargetFlags );

    const CRect& cSourceRect = GetSourceRect();
    const D3DRECT sRect = { cSourceRect.left, cSourceRect.top, cSourceRect.right, cSourceRect.bottom };

    if( CRenderDevice::s_bHitTesting )
    {
        pd3dDevice->SetRenderTarget( 0, CRenderDevice::s_pHitProxyBuffer );

        pd3dDevice->Clear( 1, &sRect, D3DCLEAR_TARGET, 0, 0.0f, 0 );
    }
    else
    {
        if( m_pBackBuffer )
            pd3dDevice->SetRenderTarget( 0, m_pBackBuffer );

        pd3dDevice->Clear( 1, &sRect, nClearFlags, nClearColor, fClearZ, nClearStencil );
    }

    pd3dDevice->BeginScene();
}

VOID CD3DViewport::EndScene( IDirect3DDevice9* pd3dDevice )
{
    pd3dDevice->EndScene();

    if( !CRenderDevice::s_bHitTesting )
    {
        if( m_pSwapChain )
            m_pSwapChain->Present( &GetSourceRect(), &GetDestRect(), GetTargetWnd(), NULL, 0 );
        else
            pd3dDevice->Present( &GetSourceRect(), &GetDestRect(), GetTargetWnd(), NULL );
    }

    m_pBackupTarget->Restore();
}

HRESULT CD3DViewport::OnDeviceReset(
    IDirect3DDevice9* pd3dDevice,
    const D3DSURFACE_DESC* pBackBufferSurfaceDesc )
{
    return CreateBackBuffer( pd3dDevice );
}

VOID CD3DViewport::OnDeviceLost()
{
    DestroyBackBuffer();
}