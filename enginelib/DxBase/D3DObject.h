/**
 * \date	2011/01/24
 * \author	Jun-Hyuk Choi
 * \brief   EngineLib/DxTools/DxObject.h 파일에 유사 클래스가 존재 하는데,
 *          인터페이스로서의 역할을 하려고 만든 것 같지만,
 *          실제로 활용하지 않으므로 통합하는 것이 좋을 것 같다.
 */
#pragma once

class CD3DObject
{
public:
    CD3DObject() {}
    virtual ~CD3DObject() {}

public:
    virtual HRESULT OnDeviceCreated(
        IDirect3DDevice9* pd3dDevice,
        const D3DSURFACE_DESC* pBackBufferSurfaceDesc ) { return S_OK; }

    virtual VOID OnDeviceDestroyed() {}

    virtual HRESULT OnDeviceReset(
        IDirect3DDevice9* pd3dDevice,
        const D3DSURFACE_DESC* pBackBufferSurfaceDesc ) { return S_OK; }

    virtual VOID OnDeviceLost() {}

    virtual VOID OnFrameMove(
        IDirect3DDevice9* pd3dDevice,
        DOUBLE fTime,
        FLOAT fElapsedTime ) {}

    virtual VOID OnFrameRender(
        IDirect3DDevice9* pd3dDevice,
        DOUBLE fTime,
        FLOAT fElapsedTime ) {}

public:
    virtual VOID SetVisible( BOOL bVisible ) {}
    virtual VOID SetActive( BOOL bActive ) {}

public:
    virtual BOOL IsVisible() { return FALSE; }
    virtual BOOL IsActive() { return FALSE; }

    virtual CRect GetSourceRect() { return CRect( 0, 0, 0, 0 ); }
    virtual CRect GetDestRect() { return CRect( 0, 0, 0, 0 ); }
    virtual CRect GetHitRect() { return CRect( 0, 0, 0, 0 ); }

    virtual HWND GetTargetWnd() { return NULL; }
};