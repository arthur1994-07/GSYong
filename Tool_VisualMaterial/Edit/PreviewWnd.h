#pragma once

#include "../Resource.h"

class DxViewPortPreview;

/////////////////////////////////////////////////////////////////////////////
// CPreviewWnd dialog

class CPreviewWnd : public CWnd
{
private:
    int m_nScreenX;
    int m_nScreenY;

public:
    CPreviewWnd();   // standard constructor

private:
    LPDIRECT3DSWAPCHAIN9    m_pSwapChainPreview;
    DxViewPortPreview*      m_pViewPort;

public:
    HRESULT OnCreateDevice( IDirect3DDevice9* pd3dDevice );
    HRESULT OnResetDevice( IDirect3DDevice9* pd3dDevice );
    void OnLostDevice();
    void OnFrameMove( float fElapsedTime );
    HRESULT Render();

private:
    void CreateSwapChain( int cx, int cy );

protected:
    virtual BOOL PreCreateWindow( CREATESTRUCT& cs );

// Construction
public:
    virtual ~CPreviewWnd();

// Implementation
protected:
    DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnSize(UINT nType, int cx, int cy);
};