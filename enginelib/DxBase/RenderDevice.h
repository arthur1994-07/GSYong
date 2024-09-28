/**
 * \date    2011/01/24
 * \author  Jun-Hyuk Choi
 * \ref     Unreal3
 */
#pragma once

#include <vector>
#include "../../InternalCommonLib/Core/TemplateValue.h"
#include "../DxCommon9/DXUT.h"

namespace ic
{
    struct SHitProxy;
}

class CD3DObject;
class CD3DViewport;

class CRenderDevice
{
public:
    enum
    {
        EM_INSERT_FRONT = 0x0001,
    };

public:
    typedef std::vector< CD3DObject* > D3DObjectVec;
    typedef D3DObjectVec::iterator D3DObjectIter;

    typedef std::vector< ic::SHitProxy* > HitProxyVec;
    typedef HitProxyVec::iterator HitProxyIter;

public:
    static HWND s_hWnd;

    static IDirect3DDevice9* s_pd3dDevice;
    static D3DSURFACE_DESC s_sSurfaceDesc;

    static D3DObjectVec s_vecObject;

    static LPDIRECT3DSURFACEQ s_pHitProxyBuffer;
    static ic::Int2 s_sHitProxyBufferSize;
    static BOOL s_bHitTesting;
    static DWORD s_nHitProxyIndex;
    static CD3DViewport* s_pHitProxyViewport;
    static HitProxyVec s_vecHitProxy;

public:
    static BOOL OnCreate(
        HWND hWndFocus,
        HWND hWndDeviceFullScreen,
        HWND hWndDeviceWindowed,
        bool bVSync,
        bool bAutoCallPresent );

    static VOID OnDestroy();
    static VOID OnSize( UINT nType, INT cx, INT cy );
    static VOID OnActivate( UINT nState, BOOL bMinimized );
    static BOOL OnLoop();

public:
    static BOOL AttachObject( CD3DObject* pObject, WORD nFlags = 0 );
    static BOOL DetachObject( CD3DObject* pObject );
    static VOID CreateObject( CD3DObject* pObject );
    static VOID DestroyObject( CD3DObject* pObject );
    static VOID RecreateObject( CD3DObject* pObject );
    static VOID ResetObject( CD3DObject* pObject );
    static VOID FindMaxRect( ic::Int2& sViewport, ic::Int2& sHitProxy );

public:
    static VOID CreateHitProxyBuffer();
    static VOID DestroyHitProxyBuffer();
    static VOID InvalidateHitProxyCache();

    static ic::SHitProxy* GetHitProxy(
        CD3DViewport* pViewport,
        const ic::Int2& sPos );

    static VOID GetHitProxyMap(
        CD3DViewport* pViewport,
        ic::Int2 sMin,
        ic::Int2 sMax,
        HitProxyVec& vecOut );

public:
    static HRESULT CALLBACK OnDeviceCreated(
        IDirect3DDevice9* pd3dDevice,
        const D3DSURFACE_DESC* pBackBufferSurfaceDesc,
        void* pUserContext );

    static VOID CALLBACK OnDeviceDestroyed( void* pUserContext );

    static HRESULT CALLBACK OnDeviceReset(
        IDirect3DDevice9* pd3dDevice,
        const D3DSURFACE_DESC* pBackBufferSurfaceDesc,
        void* pUserContext );

    static VOID CALLBACK OnDeviceLost( void* pUserContext );

    static VOID CALLBACK OnFrameMove(
        IDirect3DDevice9* pd3dDevice,
        double fTime,
        float fElapsedTime,
        void* pUserContext );

    static VOID CALLBACK OnFrameRender(
        IDirect3DDevice9* pd3dDevice,
        double fTime,
        float fElapsedTime,
        void* pUserContext );

public:
    static bool CALLBACK IsDeviceAcceptable(
        D3DCAPS9* pCaps,
        D3DFORMAT AdapterFormat,
        D3DFORMAT BackBufferFormat,
        bool bWindowed,
        void* pUserContext );

    static bool CALLBACK ModifyDeviceSettings(
        DXUTDeviceSettings* pDeviceSettings,
        const D3DCAPS9* pCaps,
        void* pUserContext );
};