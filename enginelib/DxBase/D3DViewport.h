/**
 * \date	2011/01/24
 * \author	Jun-Hyuk Choi
 */
#pragma once

#include "../../InternalCommonLib/Core/TemplateValue.h"
#include "./D3DObject.h"

class DxBackupTarget;

class CD3DViewport : public CD3DObject
{
protected:
    BOOL m_bVisible;
    BOOL m_bActive;
    ic::Int2 m_sOrigin;
    FLOAT m_fZoom;

    CRect m_cSourceRect;
    CRect m_cDestRect;
    CRect m_cHitRect;

    DxBackupTarget* m_pBackupTarget;
    LPDIRECT3DSWAPCHAIN9 m_pSwapChain;
    LPDIRECT3DSURFACE9 m_pBackBuffer;

public:
    CD3DViewport();
    virtual ~CD3DViewport();

public:
    virtual LRESULT WindowProc( UINT message, WPARAM wParam, LPARAM lParam );

public:
    virtual VOID CalcSourceRect();
    virtual VOID CalcDestRect();
    virtual VOID CalcHitRect();

protected:
    virtual HRESULT CreateBackBuffer( IDirect3DDevice9* pd3dDevice );
    virtual VOID DestroyBackBuffer();

public:
    virtual VOID BeginScene(
        IDirect3DDevice9* pd3dDevice,
        DWORD nClearColor = 0,
        DWORD nClearFlags = D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
        FLOAT fClearZ = 1.0f,
        DWORD nClearStencil = 0,
        WORD nBackupTargetFlags = USHRT_MAX );

    virtual VOID EndScene( IDirect3DDevice9* pd3dDevice );

public:
    virtual HRESULT OnDeviceReset(
        IDirect3DDevice9* pd3dDevice,
        const D3DSURFACE_DESC* pBackBufferSurfaceDesc );

    virtual VOID OnDeviceLost();

public:
    virtual VOID SetVisible( BOOL bVisible ) { m_bVisible = bVisible; }
    virtual VOID SetActive( BOOL bActive ) { m_bActive = bActive; }
    virtual VOID SetOrigin( const ic::Int2& sOrigin ) { m_sOrigin = sOrigin; }
    virtual VOID SetZoom( FLOAT fZoom ) { m_fZoom = fZoom; }

public:
    virtual BOOL IsVisible() { return m_bVisible; }
    virtual BOOL IsActive() { return m_bActive; }
    virtual ic::Int2& GetOrigin() { return m_sOrigin; }
    virtual FLOAT GetZoom() { return m_fZoom; }

    virtual CRect GetSourceRect() { return m_cSourceRect; }
    virtual CRect GetDestRect() { return m_cDestRect; }
    virtual CRect GetHitRect() { return m_cHitRect; }
};