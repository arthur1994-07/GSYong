#include "pch.h"
#include "./DxBackUpRendTarget.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

DxBackupTarget::DxBackupTarget()
    : m_pd3dDevice( NULL )
    , m_wFlags( 0 )
    , m_pTarget( NULL )
    , m_pDepth( NULL )
{
}

DxBackupTarget::~DxBackupTarget()
{
    Restore();
}

bool DxBackupTarget::Backup( LPDIRECT3DDEVICEQ pd3dDevice, WORD wFlags )
{
    MIN_ASSERT( NULL != pd3dDevice );
    MIN_ASSERT( 0 != wFlags );
    MIN_ASSERT( 0 == m_wFlags );

    m_pd3dDevice = pd3dDevice;

    if( wFlags > ALL )
        wFlags = ALL;

    if( wFlags & TARGET )
    {
        HRESULT hr = pd3dDevice->GetRenderTarget( 0, &m_pTarget );
        if( SUCCEEDED( hr ) )
            m_wFlags |= TARGET;
    }

    if( wFlags & DEPTH )
    {
        HRESULT hr = pd3dDevice->GetDepthStencilSurface( &m_pDepth );
        if( SUCCEEDED( hr ) )
            m_wFlags |= DEPTH;
    }

    return ( m_wFlags == wFlags ) ? true : false;
}

void DxBackupTarget::Restore()
{
    if( m_wFlags & TARGET )
    {
        m_pd3dDevice->SetRenderTarget( 0, m_pTarget );
        SAFE_RELEASE( m_pTarget );
        m_wFlags &= ~TARGET;
    }

    if( m_wFlags & DEPTH )
    {
        m_pd3dDevice->SetDepthStencilSurface( m_pDepth );
        SAFE_RELEASE( m_pDepth );
        m_wFlags &= ~DEPTH;
    }
}