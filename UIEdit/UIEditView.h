// UIEditView.h : iCUIEditView 클래스의 인터페이스
//
#include "stdafx.h"
#include "afxcmn.h"

#pragma once

class CUIMan;
class EngineDeviceMan;

class CUIEditView : public CView
{
	enum
	{
		UI_EDIT_MODE,
		UI_INNER_MODE,
		UI_OUTER_MODE,
		UI_LOADING_MODE,
	};

protected: // serialization에서만 만들어집니다.
	CUIEditView();
	DECLARE_DYNCREATE(CUIEditView)

// 특성
public:
	CUIEditDoc* GetDocument() const;

// 작업
private:
	BOOL		m_bCreated;
    
private:
	RECT m_rcWindowClient;
	HWND m_hwndRenderFullScreen;

	int			m_nActiveMode;
	//int			m_nViewMode;

	int			m_nViewWidth;
	int			m_nViewHeight;

public:
	LPDIRECT3DSWAPCHAIN9 m_pSwapChainMain;
	LPDIRECT3DSWAPCHAIN9 m_pSwapChainTextureView;
	LPDIRECT3DSWAPCHAIN9 m_pSwapChainMagnifyingGlass;
	LPDIRECT3DSWAPCHAIN9 m_pSwapChainAniPreView;

public:
	VOID CreateMain();
	VOID CreateTextureView();
	VOID CreateMagnifyingGlass();
	VOID CreateAniPreView();

	void ResetAllMode( int nBeforeMode );
	CUIMan* GetActiveUIMan();
	int GetActiveMode() { return m_nActiveMode; }
	void SetActiveMode(int nMode);
	void SetEditMode(int nBeforeMode = -1);

	void OnFrameMove( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime );
	void OnFrameRender( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime );
	HRESULT OnCreateDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc );
	void	OnDestroyDevice();
	HRESULT OnResetDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc );
	void	OnLostDevice();

	void OnToggleFullscreen(void) { DXUTToggleFullScreen(); }

	BOOL Render();

	void DrawGraphPaper();
	void DrawViewSizeLine(int nWidth,int nHeight , const D3DCOLOR& _diffuse );
	void DrawLine(D3DXVECTOR2 vStart, D3DXVECTOR2 vEnd, const D3DCOLOR& _diffuse);

	// DXUT Callbacks
    static  void    CALLBACK OnFrameMoveCB( LPDIRECT3DDEVICEQ pd3dDevice, double fTime, float fElapsedTime, void* pUserContext );
    static  void    CALLBACK OnFrameRenderCB( LPDIRECT3DDEVICEQ pd3dDevice, double fTime, float fElapsedTime, void* pUserContext );
    static  HRESULT CALLBACK OnCreateDeviceCB( LPDIRECT3DDEVICEQ pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext );
    static  HRESULT CALLBACK OnResetDeviceCB( LPDIRECT3DDEVICEQ pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext );
    static  void    CALLBACK OnLostDeviceCB( void* pUserContext );
    static  void    CALLBACK OnDestroyDeviceCB( void* pUserContext );

public:
	LPDIRECT3DDEVICEQ	GetD3dDevice()			{ return DXUTGetD3DDevice(); }


// 재정의
	public:
	virtual void OnDraw(CDC* pDC);  // 이 뷰를 그리기 위해 재정의되었습니다.
virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 구현
public:
	virtual ~CUIEditView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 메시지 맵 함수를 생성했습니다.
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
protected:
	virtual void PostNcDestroy();

public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
};

#ifndef _DEBUG  // UIEditView.cpp의 디버그 버전
inline CUIEditDoc* CUIEditView::GetDocument() const
   { return reinterpret_cast<CUIEditDoc*>(m_pDocument); }
#endif

