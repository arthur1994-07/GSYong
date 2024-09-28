// Tool_VisualMaterialView.h : CTool_VisualMaterialView 클래스의 인터페이스
//


#pragma once


#include "../InternalCommonLib/dxstdafx.h"
//#include "../EngineLib/DxCommon/DxStdAfx_Min.h"
#include "../EngineLib/DxCommon/d3dapp.h"
#include "../EngineLib/DxCommon9/DXUT.h"

class DxVisualMaterial;
class CTool_VisualMaterialDoc;

class CTool_VisualMaterialView : public CView, public CD3DApplication
{
protected: // serialization에서만 만들어집니다.
	CTool_VisualMaterialView();
	DECLARE_DYNCREATE(CTool_VisualMaterialView)

// 특성입니다.
public:
	CTool_VisualMaterialDoc* GetDocument() const;

private:
	BOOL			m_bCreated;
	HCURSOR			m_hCursorDefault;

private:
	RECT m_rcWindowClient;
	HWND m_hwndRenderFullScreen;

private:
    LPDIRECT3DSWAPCHAIN9 m_pSwapChainMain;

// 작업입니다.
public:
	void OnFrameMove( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime );
	void OnFrameRender( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime );

	HRESULT OnCreateDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc );
	void	OnDestroyDevice();
	HRESULT OnResetDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc );
	void	OnLostDevice();

	void OnToggleFullscreen(void) { DXUTToggleFullScreen(); }
   // const D3DSURFACE_DESC*	GetBackBufferSurfaceDesc()	{ return DXUTGetBackBufferSurfaceDesc(); }

private:
    HRESULT RenderText();

public:
	BOOL RenderApp();
    LPDIRECT3DDEVICEQ	GetD3DDevice ()			{ return m_pd3dDevice; }    //{ return DXUTGetD3DDevice(); }

//// DXUT Callbacks
//    static  void    CALLBACK OnFrameMoveCB( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext );
//    static  void    CALLBACK OnFrameRenderCB( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext );
//    static  HRESULT CALLBACK OnCreateDeviceCB( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext );
//    static  HRESULT CALLBACK OnResetDeviceCB( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext );
//    static  void    CALLBACK OnLostDeviceCB( void* pUserContext );
//    static  void    CALLBACK OnDestroyDeviceCB( void* pUserContext );

    virtual HRESULT ConfirmDevice( D3DCAPSQ*,DWORD,D3DFORMAT );
	virtual HRESULT OneTimeSceneInit();
	virtual HRESULT InitDeviceObjects();
	virtual HRESULT RestoreDeviceObjects();
	virtual HRESULT FrameMove( BOOL bNotRendering );
	virtual HRESULT Render();
	virtual HRESULT InvalidateDeviceObjects();
	virtual HRESULT DeleteDeviceObjects();
	virtual HRESULT FinalCleanup();

    HRESULT OnSize ( int cx, int cy )
	{
		m_d3dpp.BackBufferWidth  = cx;
		m_d3dpp.BackBufferHeight = cy;

        if ( !m_pd3dDevice )
            return S_OK;

		return Resize3DEnvironment();
	}

    DWORD GetBackBufferWidth()      { return m_d3dpp.BackBufferWidth; }
    DWORD GetBackBufferHeight()     { return m_d3dpp.BackBufferHeight; }

    // Tool 에서 사용하는 MainData
public:
    DxVisualMaterial*   m_pVisualMaterial;

// 재정의입니다.
public:
	virtual void OnDraw(CDC* pDC);  // 이 뷰를 그리기 위해 재정의되었습니다.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    virtual void OnInitialUpdate();

protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
    virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
    virtual void PostNcDestroy();

// 구현입니다.
public:
	virtual ~CTool_VisualMaterialView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 생성된 메시지 맵 함수
protected:
	DECLARE_MESSAGE_MAP()
    afx_msg void OnSize(UINT nType, int cx, int cy);    
public:
    afx_msg void OnDestroy();
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
};

#ifndef _DEBUG  // Tool_VisualMaterialView.cpp의 디버그 버전
inline CTool_VisualMaterialDoc* CTool_VisualMaterialView::GetDocument() const
   { return reinterpret_cast<CTool_VisualMaterialDoc*>(m_pDocument); }
#endif

