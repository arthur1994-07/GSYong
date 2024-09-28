#include "stdafx.h"
#include "DxVmrAllocator.h"

DxVmrAllocator::DxVmrAllocator( HWND hWnd, LPDIRECT3D9 pD3D, LPDIRECT3DDEVICE9 pDevice )
:	m_lRefCount( 1 )
,	m_pD3D( pD3D )
,	m_pDevice( pDevice )
,	m_hWnd( hWnd )
{
	m_pDevice->GetRenderTarget( 0, &m_pRenderTarget );
}

DxVmrAllocator::~DxVmrAllocator()
{
	DeleteSurfaces();
}

void DxVmrAllocator::DeleteSurfaces()
{
	CAutoLock Lock( &m_ObjectLock );

	// clear out the private texture
	m_pPrivateTexture.Release();

	for( size_t i = 0; i < m_vecSurfaces.size(); ++i ) 
		m_vecSurfaces[ i ].Release();
}


//IVMRSurfaceAllocator9
HRESULT DxVmrAllocator::InitializeDevice( DWORD_PTR dwUserID, VMR9AllocationInfo* lpAllocInfo, DWORD* lpNumBuffers )
{
	D3DCAPS9 d3dcaps;
	DWORD dwWidth = 1;
	DWORD dwHeight = 1;

	if( lpNumBuffers == NULL )
		return E_POINTER;

	if( m_pVMRSurfAllocNotify == NULL )
		return E_FAIL;

	HRESULT hr = S_OK;

	m_pDevice->GetDeviceCaps( &d3dcaps );
	if( d3dcaps.TextureCaps & D3DPTEXTURECAPS_POW2 )
	{
		while( dwWidth < lpAllocInfo->dwWidth )
			dwWidth = dwWidth << 1;
		while( dwHeight < lpAllocInfo->dwHeight )
			dwHeight = dwHeight << 1;

		lpAllocInfo->dwWidth = dwWidth;
		lpAllocInfo->dwHeight = dwHeight;
	}

	// NOTE:
	// we need to make sure that we create textures because
	// surfaces can not be textured onto a primitive.
	lpAllocInfo->dwFlags |= VMR9AllocFlag_TextureSurface;

	DeleteSurfaces();
	m_vecSurfaces.resize( *lpNumBuffers );
	hr = m_pVMRSurfAllocNotify->AllocateSurfaceHelper( lpAllocInfo, lpNumBuffers, &m_vecSurfaces.at( 0 ) );

	// If we couldn't create a texture surface and 
	// the format is not an alpha format,
	// then we probably cannot create a texture.
	// So what we need to do is create a private texture
	// and copy the decoded images onto it.
	if( FAILED( hr ) && !( lpAllocInfo->dwFlags & VMR9AllocFlag_3DRenderTarget ) )
	{
		DeleteSurfaces();

		// is surface YUV ?
		if (lpAllocInfo->Format > '0000') 
		{           
			D3DDISPLAYMODE dm; 
			FAIL_RET( m_pDevice->GetDisplayMode(NULL,  &dm ) );

			FAIL_RET( m_pDevice->CreateTexture(lpAllocInfo->dwWidth, lpAllocInfo->dwHeight,
				1, 
				D3DUSAGE_RENDERTARGET, 
				dm.Format, 
				D3DPOOL_DEFAULT,
				& m_pPrivateTexture, NULL ) );
		}


		lpAllocInfo->dwFlags &= ~VMR9AllocFlag_TextureSurface;
		lpAllocInfo->dwFlags |= VMR9AllocFlag_OffscreenSurface;

		FAIL_RET( m_pVMRSurfAllocNotify->AllocateSurfaceHelper( lpAllocInfo, lpNumBuffers, &m_vecSurfaces.at( 0 ) ) );
	}

	m_Scene.Init();

	return S_OK;
}

HRESULT DxVmrAllocator::TerminateDevice( DWORD_PTR dwID )
{
	DeleteSurfaces();
	return S_OK;
}

HRESULT DxVmrAllocator::GetSurface( DWORD_PTR dwUserID, DWORD SurfaceIndex, DWORD SurfaceFlags, IDirect3DSurface9 **lplpSurface )
{
	if( lplpSurface == NULL )
		return E_POINTER;

	if( SurfaceIndex >= m_vecSurfaces.size() ) 
		return E_FAIL;

	CAutoLock Lock( &m_ObjectLock );

	*lplpSurface = m_vecSurfaces[ SurfaceIndex ];
	(*lplpSurface)->AddRef();

	return S_OK;
}

HRESULT DxVmrAllocator::AdviseNotify( IVMRSurfaceAllocatorNotify9* lpIVMRSurfAllocNotify )
{
	CAutoLock Lock(&m_ObjectLock);

	HRESULT hr;

	m_pVMRSurfAllocNotify = lpIVMRSurfAllocNotify;

	HMONITOR hMonitor = m_pD3D->GetAdapterMonitor( D3DADAPTER_DEFAULT );
	FAIL_RET( m_pVMRSurfAllocNotify->SetD3DDevice( m_pDevice, hMonitor ) );

	return hr;
}

HRESULT DxVmrAllocator::StartPresenting( DWORD_PTR dwUserID )
{
	CAutoLock Lock(&m_ObjectLock);

	ASSERT( m_pDevice );
	if( m_pDevice == NULL )
		return E_FAIL;

	return S_OK;
}

HRESULT DxVmrAllocator::StopPresenting( DWORD_PTR dwUserID )
{
	return S_OK;
}

HRESULT DxVmrAllocator::PresentImage( DWORD_PTR dwUserID, VMR9PresentationInfo* lpPresInfo )
{
	HRESULT hr;
	CAutoLock Lock(&m_ObjectLock);

	// if we are in the middle of the display change
	if( NeedToHandleDisplayChange() )
	{
		// NOTE: this piece of code is left as a user exercise.  
		// The D3DDevice here needs to be switched
		// to the device that is using another adapter
	}

	hr = PresentHelper( lpPresInfo );

	return hr;
}

HRESULT DxVmrAllocator::PresentHelper( VMR9PresentationInfo* lpPresInfo )
{
	if( NULL == lpPresInfo )
		return E_POINTER;
	else if( NULL == lpPresInfo->lpSurf )
		return E_POINTER;

	CAutoLock Lock( &m_ObjectLock );
	HRESULT hr;

	m_pDevice->SetRenderTarget( 0, m_pRenderTarget );

	D3DVIEWPORT9 viewPort;
	m_pDevice->GetViewport( &viewPort );

	float fResizeHeight = float( viewPort.Width ) * float( lpPresInfo->szAspectRatio.cy ) / float( lpPresInfo->szAspectRatio.cx );
	float fResetPosY = ( (float) viewPort.Height - fResizeHeight ) / 2.0f;

	if( NULL != m_pPrivateTexture )
	{
		LPDIRECT3DSURFACE9 pSurface;
		FAIL_RET( m_pPrivateTexture->GetSurfaceLevel( 0 , &pSurface ) );

		FAIL_RET( m_pDevice->StretchRect( lpPresInfo->lpSurf, NULL,
				  pSurface, NULL, D3DTEXF_NONE ) );

		m_Scene.RenderScene( m_pDevice, m_pPrivateTexture, 
							 0.0f, fResetPosY, (float) viewPort.Width, fResizeHeight + fResetPosY );

		pSurface->Release();
		pSurface = NULL;
	}
	else
	{
		LPDIRECT3DTEXTURE9 pTexture;
		FAIL_RET( lpPresInfo->lpSurf->GetContainer( __uuidof(IDirect3DTexture9), (LPVOID*) &pTexture ) );

		m_Scene.RenderScene( m_pDevice, pTexture, 
							 0.0f, fResetPosY, (float) viewPort.Width, fResizeHeight + fResetPosY );

		pTexture->Release();
		pTexture = NULL;
	}

	m_pDevice->Present( NULL, NULL, NULL, NULL );

	return hr;
}

bool DxVmrAllocator::NeedToHandleDisplayChange()
{
	if( NULL == m_pVMRSurfAllocNotify )
		return false;

	D3DDEVICE_CREATION_PARAMETERS Parameters;
	if( FAILED( m_pDevice->GetCreationParameters( &Parameters ) ) )
		return false;

	HMONITOR currentMonitor = m_pD3D->GetAdapterMonitor( Parameters.AdapterOrdinal );
	HMONITOR hMonitor = m_pD3D->GetAdapterMonitor( D3DADAPTER_DEFAULT );

	return hMonitor != currentMonitor;
}


// IUnknown
HRESULT DxVmrAllocator::QueryInterface( REFIID riid, void** ppvObject )
{
	HRESULT hr = E_NOINTERFACE;

	if( ppvObject == NULL )
	{
		hr = E_POINTER;
	}
	else if( riid == IID_IVMRSurfaceAllocator9 )
	{
		*ppvObject = static_cast<IVMRSurfaceAllocator9*>( this );
		AddRef();
		hr = S_OK;
	} 
	else if( riid == IID_IVMRImagePresenter9 )
	{
		*ppvObject = static_cast<IVMRImagePresenter9*>( this );
		AddRef();
		hr = S_OK;
	} 
	else if( riid == IID_IUnknown )
	{
		*ppvObject = static_cast<IUnknown*>( static_cast<IVMRSurfaceAllocator9*>( this ) );
		AddRef();
		hr = S_OK;    
	}

	return hr;
}

ULONG DxVmrAllocator::AddRef()
{
	return InterlockedIncrement( &m_lRefCount );
}

ULONG DxVmrAllocator::Release()
{
	ULONG ret = InterlockedDecrement( &m_lRefCount );
	if( ret == 0 )
		delete this;

	return ret;
}