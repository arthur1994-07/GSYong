#include "pch.h"
#include <boost/foreach.hpp>

#include "../../InternalCommonLib/Core/CommonUtil.h"
#include "../../InternalCommonLib/Core/HitProxy.h"

#include "../DxTools/DxInputDevice.h"
#include "../DxCommon9/DXUTmisc.h"
#include "./D3DObject.h"
#include "./D3DViewport.h"
#include "./RenderDevice.h"

#include "../../SigmaCore/DebugInclude.h"

HWND CRenderDevice::s_hWnd = NULL;

IDirect3DDevice9* CRenderDevice::s_pd3dDevice = NULL;
D3DSURFACE_DESC CRenderDevice::s_sSurfaceDesc;

CRenderDevice::D3DObjectVec CRenderDevice::s_vecObject;

LPDIRECT3DSURFACEQ CRenderDevice::s_pHitProxyBuffer = NULL;
ic::Int2 CRenderDevice::s_sHitProxyBufferSize;
BOOL CRenderDevice::s_bHitTesting = FALSE;
DWORD CRenderDevice::s_nHitProxyIndex = 0;
CD3DViewport* CRenderDevice::s_pHitProxyViewport = NULL;
CRenderDevice::HitProxyVec CRenderDevice::s_vecHitProxy;

BOOL CRenderDevice::OnCreate(
    HWND hWndFocus,
    HWND hWndDeviceFullScreen,
    HWND hWndDeviceWindowed,
    bool bVSync,
    bool bAutoCallPresent )
{
    s_hWnd = hWndFocus;

    if( DXUTGetD3DDevice() )
        return FALSE;

    DXUTSetCallbackDeviceCreated( OnDeviceCreated );
    DXUTSetCallbackDeviceDestroyed( OnDeviceDestroyed );
    DXUTSetCallbackDeviceReset( OnDeviceReset );
    DXUTSetCallbackDeviceLost( OnDeviceLost );
    DXUTSetCallbackFrameMove( OnFrameMove );
    DXUTSetCallbackFrameRender( OnFrameRender );

    DXUTSetCursorSettings( true, true );
    DXUTInit();
    DXUTSetVSyncSetting( bVSync );
    DXUTSetPresentEnable( bAutoCallPresent );
    DXUTSetWindow( hWndFocus, hWndDeviceFullScreen, hWndDeviceWindowed, false );

    CRect cViewRect;
    GetClientRect( hWndFocus, &cViewRect );
    if( cViewRect.Width() == 0 || cViewRect.Height() == 0 )
        return FALSE;

    HRESULT hr = DXUTCreateDevice(
	    D3DADAPTER_DEFAULT,
	    true,
        cViewRect.Width(),
        cViewRect.Height(),
	    IsDeviceAcceptable,
	    ModifyDeviceSettings );

    if( FAILED( hr ) )
        return FALSE;

    return TRUE;
}

VOID CRenderDevice::OnDestroy()
{
    s_hWnd = NULL;

    DXUTShutdown();

    s_vecObject.clear();
}

VOID CRenderDevice::OnSize( UINT nType, INT cx, INT cy )
{
    if( !s_hWnd )
        return;

    if( nType == SIZE_MINIMIZED )
        return;

    if( !DXUTGetD3DDevice() )
        return;

    DXUTStaticWndProc( s_hWnd, WM_SIZE, nType, MAKELPARAM( cx, cy ) );
    DXUTStaticWndProc( s_hWnd, WM_EXITSIZEMOVE, 0, 0 );
}

VOID CRenderDevice::OnActivate( UINT nState, BOOL bMinimized )
{
    if( nState != WA_ACTIVE || bMinimized == FALSE )
		DxInputDevice::GetInstance().OnActivate( nState != WA_INACTIVE );
}

BOOL CRenderDevice::OnLoop()
{
    if( !s_hWnd )
        return FALSE;

    if( !DXUTGetD3DDevice() )
        return FALSE;

    DXUTRender3DEnvironment();

    return TRUE;
}

BOOL CRenderDevice::AttachObject( CD3DObject* pObject, WORD nFlags )
{
    D3DObjectIter it = std::find( s_vecObject.begin(), s_vecObject.end(), pObject );
    if( it != s_vecObject.end() )
        return FALSE;

    if( nFlags & EM_INSERT_FRONT )
        s_vecObject.insert( s_vecObject.begin(), pObject );
    else
        s_vecObject.push_back( pObject );

    CreateHitProxyBuffer();

    return TRUE;
}

BOOL CRenderDevice::DetachObject( CD3DObject* pObject )
{
    D3DObjectIter it = std::find( s_vecObject.begin(), s_vecObject.end(), pObject );
    if( it != s_vecObject.end() )
    {
        s_vecObject.erase( it );

        return FALSE;
    }

    CreateHitProxyBuffer();

    return TRUE;
}

VOID CRenderDevice::CreateObject( CD3DObject* pObject )
{
    if( !pObject )
        return;

    pObject->OnDeviceCreated( s_pd3dDevice, &s_sSurfaceDesc );
    pObject->OnDeviceReset( s_pd3dDevice, &s_sSurfaceDesc );
}

VOID CRenderDevice::DestroyObject( CD3DObject* pObject )
{
    if( !pObject )
        return;

    pObject->OnDeviceLost();
    pObject->OnDeviceDestroyed();
}

VOID CRenderDevice::RecreateObject( CD3DObject* pObject )
{
    if( !pObject )
        return;

    pObject->OnDeviceLost();
    pObject->OnDeviceDestroyed();
    pObject->OnDeviceCreated( s_pd3dDevice, &s_sSurfaceDesc );
    pObject->OnDeviceReset( s_pd3dDevice, &s_sSurfaceDesc );
}

VOID CRenderDevice::ResetObject( CD3DObject* pObject )
{
    if( !pObject )
        return;

    pObject->OnDeviceLost();
    pObject->OnDeviceReset( s_pd3dDevice, &s_sSurfaceDesc );
}

VOID CRenderDevice::FindMaxRect( ic::Int2& sViewport, ic::Int2& sHitProxy )
{
    BOOST_FOREACH( CD3DObject* pCur, s_vecObject )
    {
        CRect cViewRect = pCur->GetDestRect();
        CRect cHitRect = pCur->GetHitRect();

        sViewport.x = new_max( sViewport.x, (INT)cViewRect.Width() );
        sViewport.y = new_max( sViewport.y, (INT)cViewRect.Height() );

        sHitProxy.x = new_max( sHitProxy.x, (INT)cHitRect.Width() );
        sHitProxy.y = new_max( sHitProxy.y, (INT)cHitRect.Height() );
    }
}

VOID CRenderDevice::CreateHitProxyBuffer()
{
    if( !DXUTGetD3DDevice() )
        return;

    ic::Int2 sViewport;
    ic::Int2 sHitProxy;
    FindMaxRect( sViewport, sHitProxy );

    // 이미 크게 만들어져 있다면 일부러 줄이지는 않는다??
    // 이 조건은 필요한지 다시 생각해보자
    if( s_sHitProxyBufferSize.x >= sHitProxy.x &&
        s_sHitProxyBufferSize.y >= sHitProxy.y )
    {
        return;
    }

    DestroyHitProxyBuffer();

    s_pd3dDevice->CreateRenderTarget(
        sHitProxy.x,
        sHitProxy.y,
        D3DFMT_A8R8G8B8,
        D3DMULTISAMPLE_NONE,
        0,
        TRUE,
        &s_pHitProxyBuffer,
        NULL );

    if( s_pHitProxyBuffer )
        s_sHitProxyBufferSize = sHitProxy;
}

VOID CRenderDevice::DestroyHitProxyBuffer()
{
    SAFE_RELEASE( s_pHitProxyBuffer );
    s_sHitProxyBufferSize = ic::Int2( 0, 0 );

    InvalidateHitProxyCache();
}

VOID CRenderDevice::InvalidateHitProxyCache()
{
    s_bHitTesting = FALSE;
    s_nHitProxyIndex = 0;
    s_pHitProxyViewport = NULL;

    BOOST_FOREACH( ic::SHitProxy* pCur, s_vecHitProxy )
        SAFE_DELETE( pCur );
    s_vecHitProxy.clear();
}

ic::SHitProxy* CRenderDevice::GetHitProxy(
    CD3DViewport* pViewport,
    const ic::Int2& sPos )
{
    const INT HitOffset = 2;

    CRect cViewRect = pViewport->GetDestRect();

    ic::Int2 sMin( sPos.x - HitOffset, sPos.y - HitOffset );
    ic::Int2 sMax( sPos.x + HitOffset, sPos.y + HitOffset );

    sMin.x = new_max( sMin.x, 0 );
	sMin.y = new_max( sMin.y, 0 );
	sMax.x = new_min( sMax.x, (INT)cViewRect.Width() - 1 );
	sMax.y = new_min( sMax.y, (INT)cViewRect.Height() - 1 );

    ic::Int2 sTestSize( sMax.x - sMin.x + 1, sMax.y - sMin.y + 1 );
    if( sTestSize.x <= 0 || sTestSize.y <= 0 )
        return NULL;

    HitProxyVec vecProxyMap;
	GetHitProxyMap( pViewport, sMin, sMax, vecProxyMap );
    if( sTestSize.x * sTestSize.y != (INT)vecProxyMap.size() )
        return NULL;

	ic::SHitProxy* pHitProxy = NULL;
	for( INT y=0; y<sTestSize.y; ++y )
	{
		for( INT x=0; x<sTestSize.x; ++x )
		{
			ic::SHitProxy* pTestProxy = vecProxyMap[ x + y * sTestSize.x ];
			if( pTestProxy && ( !pHitProxy || pTestProxy->nOrder > pHitProxy->nOrder ) )
				pHitProxy = pTestProxy;
		}
	}

	return pHitProxy;
}

VOID CRenderDevice::GetHitProxyMap(
    CD3DViewport* pViewport,
    ic::Int2 sMin,
    ic::Int2 sMax,
    HitProxyVec& vecOut )
{
    if( !s_pHitProxyBuffer )
        return;

    CRect cViewRect = pViewport->GetDestRect();

    sMin.x = ic::Clamp< INT >( sMin.x, 0, cViewRect.Width() - 1 );
	sMin.y = ic::Clamp< INT >( sMin.y, 0, cViewRect.Height() - 1 );
	sMax.x = ic::Clamp< INT >( sMax.x, 0, cViewRect.Width() - 1 );
	sMax.y = ic::Clamp< INT >( sMax.y, 0, cViewRect.Height() - 1 );

    ic::Int2 sSize( sMax.x - sMin.x + 1, sMax.y - sMin.y + 1 );
    INT nSizeTotal = sSize.x * sSize.y;

    for( INT i=0; i<nSizeTotal; ++i )
        vecOut.push_back( NULL );

    if( s_pHitProxyViewport != pViewport )
    {
        InvalidateHitProxyCache();

        s_bHitTesting = TRUE;

        pViewport->OnFrameRender( s_pd3dDevice, 0.0f, 0.0f );

        s_bHitTesting = FALSE;
        s_pHitProxyViewport = pViewport;
    }

	RECT sHitRect;
	sHitRect.left = sMin.x;
	sHitRect.top = sMin.y;
	sHitRect.right = sMax.x + 1;
	sHitRect.bottom = sMax.y + 1;

    D3DLOCKED_RECT sLockedRect;
    HRESULT	hr = s_pHitProxyBuffer->LockRect( &sLockedRect, &sHitRect, D3DLOCK_READONLY );
	if( FAILED( hr ) )
        return;

	for( INT y=sMin.y; y<=sMax.y; ++y )
	{
		DWORD* pSrc = (DWORD*)( (BYTE*)sLockedRect.pBits + ( y - sMin.y ) * sLockedRect.Pitch );
		for( INT x=sMin.x; x<=sMax.x; ++x )
		{
			DWORD nHitProxyIndex = *pSrc++ & 0x00ffffff;

            if( nHitProxyIndex <= 0 )
                continue;

            if( --nHitProxyIndex >= (DWORD)s_vecHitProxy.size() )
                continue;

            vecOut[ x - sMin.x + ( ( y - sMin.y ) * sSize.x ) ] = s_vecHitProxy[ nHitProxyIndex ];
		}
	}

	s_pHitProxyBuffer->UnlockRect();
}

HRESULT CALLBACK CRenderDevice::OnDeviceCreated(
    IDirect3DDevice9* pd3dDevice,
    const D3DSURFACE_DESC* pBackBufferSurfaceDesc,
    void* pUserContext )
{
    s_pd3dDevice = pd3dDevice;
    memcpy( &s_sSurfaceDesc, pBackBufferSurfaceDesc, sizeof( s_sSurfaceDesc ) );

    BOOST_FOREACH( CD3DObject* pCur, s_vecObject )
        pCur->OnDeviceCreated( pd3dDevice, pBackBufferSurfaceDesc );

    return S_OK;
}

VOID CALLBACK CRenderDevice::OnDeviceDestroyed( void* pUserContext )
{
    BOOST_REVERSE_FOREACH( CD3DObject* pCur, s_vecObject )
        pCur->OnDeviceDestroyed();
}

HRESULT CALLBACK CRenderDevice::OnDeviceReset(
    IDirect3DDevice9* pd3dDevice,
    const D3DSURFACE_DESC* pBackBufferSurfaceDesc,
    void* pUserContext )
{
    s_pd3dDevice = pd3dDevice;
    memcpy( &s_sSurfaceDesc, pBackBufferSurfaceDesc, sizeof( s_sSurfaceDesc ) );

    BOOST_FOREACH( CD3DObject* pCur, s_vecObject )
        pCur->OnDeviceReset( pd3dDevice, pBackBufferSurfaceDesc );

    CreateHitProxyBuffer();

    return S_OK;
}

VOID CALLBACK CRenderDevice::OnDeviceLost( void* pUserContext )
{
    DestroyHitProxyBuffer();

    BOOST_REVERSE_FOREACH( CD3DObject* pCur, s_vecObject )
        pCur->OnDeviceLost();
}

VOID CALLBACK CRenderDevice::OnFrameMove(
    IDirect3DDevice9* pd3dDevice,
    double fTime,
    float fElapsedTime,
    void* pUserContext )
{
    BOOST_FOREACH( CD3DObject* pCur, s_vecObject )
        pCur->OnFrameMove( pd3dDevice, fTime, fElapsedTime );
}

VOID CALLBACK CRenderDevice::OnFrameRender(
    IDirect3DDevice9* pd3dDevice,
    double fTime,
    float fElapsedTime,
    void* pUserContext )
{
    BOOST_FOREACH( CD3DObject* pCur, s_vecObject )
        pCur->OnFrameRender( pd3dDevice, fTime, fElapsedTime );
}

bool CALLBACK CRenderDevice::IsDeviceAcceptable(
    D3DCAPS9* pCaps,
    D3DFORMAT AdapterFormat,
    D3DFORMAT BackBufferFormat,
    bool bWindowed,
    void* pUserContext )
{
    IDirect3D9* pD3D = DXUTGetD3DObject();
    if( FAILED( pD3D->CheckDeviceFormat(
        pCaps->AdapterOrdinal,
        pCaps->DeviceType,
        AdapterFormat,
        D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING,
        D3DRTYPE_TEXTURE,
        BackBufferFormat ) ) )
    {
        return false;
    }

    return true;
}

bool CALLBACK CRenderDevice::ModifyDeviceSettings(
    DXUTDeviceSettings* pDeviceSettings,
    const D3DCAPS9* pCaps,
    void* pUserContext )
{
    // If device doesn't support HW T&L or doesn't support 1.1 vertex shaders in HW 
    // then switch to SWVP.
    if( ( pCaps->DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT ) == 0 ||
         pCaps->VertexShaderVersion < D3DVS_VERSION( 1, 1 ) )
    {
        pDeviceSettings->BehaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
    }

    // If the hardware cannot do vertex blending, use software vertex processing.
    if( pCaps->MaxVertexBlendMatrices < 2 )
        pDeviceSettings->BehaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;

    // If using hardware vertex processing, change to mixed vertex processing
    // so there is a fallback.
    if( pDeviceSettings->BehaviorFlags & D3DCREATE_HARDWARE_VERTEXPROCESSING )
        pDeviceSettings->BehaviorFlags = D3DCREATE_MIXED_VERTEXPROCESSING;    

    // Debugging vertex shaders requires either REF or software vertex processing 
    // and debugging pixel shaders requires REF.  
#ifdef DEBUG_VS
    if( pDeviceSettings->DeviceType != D3DDEVTYPE_REF )
    {
        pDeviceSettings->BehaviorFlags &= ~D3DCREATE_HARDWARE_VERTEXPROCESSING;
        pDeviceSettings->BehaviorFlags &= ~D3DCREATE_PUREDEVICE;
        pDeviceSettings->BehaviorFlags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;
    }
#endif
#ifdef DEBUG_PS
    pDeviceSettings->DeviceType = D3DDEVTYPE_REF;
#endif
    // For the first device created if its a REF device, optionally display a warning dialog box
    static bool s_bFirstTime = true;
    if( s_bFirstTime )
    {
        s_bFirstTime = false;
	    if( pDeviceSettings->DeviceType == D3DDEVTYPE_REF )
            DXUTDisplaySwitchingToREFWarning();
    }

    return true;
}