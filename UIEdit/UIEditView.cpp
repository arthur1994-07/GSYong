// UIEditView.cpp : CUIEditView 클래스의 구현
//

#pragma once

#include "stdafx.h"
#include "../EngineLib/GUInterface/UIMan.h"
#include "../EngineLib/DxResponseMan.h"
#include "../EngineLib/DxCommon9/DXUTmisc.h"

#include "UIEdit.h"
#include "UIEditDoc.h"
#include "UIEditView.h"
#include "MainUI/InterfaceAdmin.h"
#include "MainFrm.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

// CUIEditView

IMPLEMENT_DYNCREATE(CUIEditView, CView)

BEGIN_MESSAGE_MAP(CUIEditView, CView)
	// 표준 인쇄 명령입니다.
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
	ON_WM_SIZE()
END_MESSAGE_MAP()

// CUIEditView 생성/소멸

static CUIEditView* g_pCharEditView = NULL;

CUIEditView::CUIEditView()
    : m_bCreated( FALSE )
    , m_pSwapChainMain( NULL )
    , m_pSwapChainTextureView( NULL )
    , m_pSwapChainMagnifyingGlass( NULL )
    , m_pSwapChainAniPreView( NULL )
    , m_nActiveMode(UI_EDIT_MODE)
    , m_nViewWidth(1024)
    , m_nViewHeight(768)
{
	// TODO: 여기에 생성 코드를 추가합니다.    
}

CUIEditView::~CUIEditView()
{    
}

BOOL CUIEditView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	// Window 클래스 또는 스타일을 수정합니다.

	return CView::PreCreateWindow(cs);
}

// CUIEditView 그리기

void CUIEditView::OnDraw(CDC* /*pDC*/)
{
	CUIEditDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 여기에 원시 데이터에 대한 그리기 코드를 추가합니다.
}


// CUIEditView 인쇄

BOOL CUIEditView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 기본적인 준비
	return DoPreparePrinting(pInfo);
}

void CUIEditView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄하기 전에 추가 초기화 작업을 추가합니다.
}

void CUIEditView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄 후 정리 작업을 추가합니다.
}


// CUIEditView 진단

#ifdef _DEBUG
void CUIEditView::AssertValid() const
{
	CView::AssertValid();
}

void CUIEditView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CUIEditDoc* CUIEditView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CUIEditDoc)));
	return (CUIEditDoc*)m_pDocument;
}
#endif //_DEBUG

// CUIEditView 메시지 처리기
LRESULT CALLBACK FullScreenWndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
bool CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, const D3DCAPS9* pCaps, void* pUserContext );
bool CALLBACK IsDeviceAcceptable( D3DCAPS9* pCaps, D3DFORMAT AdapterFormat, 
								 D3DFORMAT BackBufferFormat, bool bWindowed, void* pUserContext );

CUIMan* CUIEditView::GetActiveUIMan()
{
    CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
    CInterfaceAdmin* pInterfaceAdmin = pFrame->GetInterfaceAdmin();
	switch(m_nActiveMode)
	{
	case UI_EDIT_MODE:        
        return pInterfaceAdmin; // CInterfaceAdmin::GetInstance();
	//case UI_INNER_MODE: return CInnerInterface::GetInstance();
	//case UI_OUTER_MODE: return COuterInterface::GetInstance();
	//case UI_LOADING_MODE: return CLoadingInterface::GetInstance();
	}

	return NULL;
}	

void CUIEditView::SetActiveMode(int nMode) 
{ 
	switch(nMode)
	{
	case UI_EDIT_MODE: SetEditMode(m_nActiveMode); break;
	//case UI_INNER_MODE: SetInnerMode(m_nActiveMode); break;
	//case UI_OUTER_MODE: SetOuterMode(m_nActiveMode); break;
	//case UI_LOADING_MODE: SetLoadingMode(m_nActiveMode); break;
	}

	m_nActiveMode = nMode;

	//CMainFrame* pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	//pFrame->m_wndToolBar.SetButtonStyle(UI_EDIT_MODE, MAKELONG( (nMode == UI_EDIT_MODE), TBSTATE_PRESSED));
	//pFrame->m_wndToolBar.SetButtonStyle(UI_INNER_MODE, MAKELONG( (nMode == UI_INNER_MODE), TBSTATE_PRESSED));
	//pFrame->m_wndToolBar.SetButtonStyle(UI_OUTER_MODE, MAKELONG( (nMode == UI_OUTER_MODE), TBSTATE_PRESSED));
	//pFrame->m_wndToolBar.SetButtonStyle(UI_LOADING_MODE, MAKELONG( (nMode == UI_LOADING_MODE), TBSTATE_PRESSED));

	CMenu *pMenu = GetTopLevelParent()->GetMenu ();
	//pMenu->CheckMenuItem ( ID_APP_PLAY, (nMode == UI_EDIT_MODE)?MF_CHECKED:MF_UNCHECKED );
	//pMenu->CheckMenuItem ( ID_APP_OUT_PLAY, (nMode == UI_OUTER_MODE)?MF_CHECKED:MF_UNCHECKED );
	//pMenu->CheckMenuItem ( ID_APP_IN_PLAY, (nMode == UI_INNER_MODE)?MF_CHECKED:MF_UNCHECKED );
	//pMenu->CheckMenuItem ( ID_APP_LOADING_PLAY, (nMode == UI_LOADING_MODE)?MF_CHECKED:MF_UNCHECKED );
}


// CUIEditView 메시지 처리기
//-----------------------------------------------------------------------------
// Name: FullScreenWndProc()
// Desc: The WndProc funtion used when the app is in fullscreen mode. This is
//       needed simply to trap the ESC key.
//-----------------------------------------------------------------------------
LRESULT CALLBACK FullScreenWndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	if( msg == WM_CREATE )
	{
		g_pCharEditView = (CUIEditView*)((CREATESTRUCT*)lParam)->lpCreateParams;
	}
	else if( msg == WM_CLOSE )
	{

	}
	else if( msg == WM_SETCURSOR )
	{
		SetCursor( NULL );
	}
	else if( ( msg == WM_KEYUP && wParam == VK_ESCAPE ) ||
		( msg == WM_SYSCHAR && wParam == VK_RETURN ) )
	{
		// User wants to leave fullscreen mode
		g_pCharEditView->OnToggleFullscreen();
		return 0;
	}
	else if ( msg == WM_LBUTTONDOWN ||
		msg == WM_LBUTTONUP ||
		msg == WM_RBUTTONDOWN ||
		msg == WM_RBUTTONUP ||
		msg == WM_MOUSEMOVE )
	{
		// Map the coordinates from fullscreen window to
		// RenderView window.
		POINT pt = { GET_X_LPARAM( lParam ), GET_Y_LPARAM( lParam ) };
		::MapWindowPoints( hWnd, g_pCharEditView->GetSafeHwnd(), &pt, 1 );
		lParam = MAKELPARAM( pt.x, pt.y );

		// forward mouse messages to the render view window
		g_pCharEditView->PostMessage( msg, wParam, lParam );
	}

	return DefWindowProc( hWnd, msg, wParam, lParam );
}

//--------------------------------------------------------------------------------------
// Called during device initialization, this code checks the device for some 
// minimum set of capabilities, and rejects those that don't pass by returning false.
//--------------------------------------------------------------------------------------
bool CALLBACK IsDeviceAcceptable( D3DCAPS9* pCaps, D3DFORMAT AdapterFormat, 
								 D3DFORMAT BackBufferFormat, bool bWindowed, void* pUserContext )
{
	// Skip backbuffer formats that don't support alpha blending
	IDirect3D9* pD3D = DXUTGetD3DObject(); 
	if( FAILED( pD3D->CheckDeviceFormat( pCaps->AdapterOrdinal, pCaps->DeviceType,
		AdapterFormat, D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING, 
		D3DRTYPE_TEXTURE, BackBufferFormat ) ) )
		return false;

	return true;
}

//--------------------------------------------------------------------------------------
// This callback function is called immediately before a device is created to allow the 
// application to modify the device settings. The supplied pDeviceSettings parameter 
// contains the settings that the framework has selected for the new device, and the 
// application can make any desired changes directly to this structure.  Note however that 
// DXUT will not correct invalid device settings so care must be taken 
// to return valid device settings, otherwise IDirect3D9::CreateDevice() will fail.  
//--------------------------------------------------------------------------------------
bool CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, const D3DCAPS9* pCaps, void* pUserContext )
{
	// If device doesn't support HW T&L or doesn't support 1.1 vertex shaders in HW 
	// then switch to SWVP.
	if( (pCaps->DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) == 0 ||
		pCaps->VertexShaderVersion < D3DVS_VERSION(1,1) )
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


//--------------------------------------------------------------------------------------------------------------------------------------------------
// DXUT Callbacks
//--------------------------------------------------------------------------------------------------------------------------------------------------
void CUIEditView::OnFrameMoveCB( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext )
{
	g_pCharEditView->OnFrameMove( pd3dDevice, fTime, fElapsedTime );
}


void CUIEditView::OnFrameRenderCB( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext )
{
	g_pCharEditView->OnFrameRender( pd3dDevice, fTime, fElapsedTime );
}


HRESULT CUIEditView::OnCreateDeviceCB( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext )
{
	return g_pCharEditView->OnCreateDevice( pd3dDevice, pBackBufferSurfaceDesc );
}


HRESULT CUIEditView::OnResetDeviceCB( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext )
{
	return g_pCharEditView->OnResetDevice( pd3dDevice, pBackBufferSurfaceDesc );
}


void CUIEditView::OnLostDeviceCB( void* pUserContext )
{
	g_pCharEditView->OnLostDevice();
}


void CUIEditView::OnDestroyDeviceCB( void* pUserContext )
{
	g_pCharEditView->OnDestroyDevice();
}


void CUIEditView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	// Save static reference to the render window
	CView::m_hWnd = CView::m_hWnd;

	// Register a class for a fullscreen window
	WNDCLASS wndClass = { CS_HREDRAW | CS_VREDRAW, FullScreenWndProc, 0, 0, NULL,
		NULL, LoadCursor(NULL, IDC_ARROW), (HBRUSH)GetStockObject(WHITE_BRUSH), NULL,
		_T("Fullscreen Window") };
	RegisterClass( &wndClass );

	m_hwndRenderFullScreen = CreateWindow( _T("Fullscreen Window"), NULL,
		WS_POPUP, 0, 0, 100, 100,
		GetTopLevelParent()->GetSafeHwnd(), 0L, NULL, this );

	// If we haven't initialized the D3D framework, do so now
	if( !DXUTGetD3DDevice() )
	{
		CRect rc;
		::GetClientRect( CView::m_hWnd, &rc );
		if( rc.Width() == 0 || rc.Height() == 0 )
		{
			MessageBox( TEXT("The render view must be visible when EffectEdit starts.  Please change the window size or splitters and start EffectEdit again."),
				TEXT("EffectEdit") );
		}
		else
		{
			// Set up the DXUT callbacks
			DXUTSetCallbackDeviceCreated( OnCreateDeviceCB );
			DXUTSetCallbackDeviceReset( OnResetDeviceCB );
			DXUTSetCallbackDeviceLost( OnLostDeviceCB );
			DXUTSetCallbackDeviceDestroyed( OnDestroyDeviceCB );
			DXUTSetCallbackFrameRender( OnFrameRenderCB );
			DXUTSetCallbackFrameMove( OnFrameMoveCB );

			// Show the cursor and clip it when in full screen
			DXUTSetCursorSettings( true, true );

			DXUTInit( true, true, true ); // Parse the command line, handle the default hotkeys, and show msgboxes

			// Note that for the MFC samples, the device window and focus window
			// are not the same.
			DXUTSetWindow( m_hwndRenderFullScreen, m_hwndRenderFullScreen, CView::m_hWnd, false );
			::GetClientRect( CView::m_hWnd, &m_rcWindowClient );

			HRESULT hr = DXUTCreateDevice( D3DADAPTER_DEFAULT, true, m_rcWindowClient.right - m_rcWindowClient.left,
				m_rcWindowClient.bottom - m_rcWindowClient.top, IsDeviceAcceptable, ModifyDeviceSettings );

			if( FAILED( hr ) )
			{
				// If D3D is not functional, do not continue.  End the application now.
				CFrameWnd *pTop = this->GetTopLevelFrame();
				if( pTop )
					pTop->PostMessage( WM_CLOSE, 0, 0 );

				m_bCreated = FALSE;
			}
		}
	}

	m_bCreated = TRUE;
}
void CUIEditView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if( DXUTGetD3DDevice() != NULL )
	{
		switch( nType )
		{
		case SIZE_MINIMIZED:
		case SIZE_MAXIMIZED:
			{
				if(GetActiveUIMan() && GetActiveMode() == UI_EDIT_MODE ) 
                {
					//GetActiveUIMan()->ReSizeWindow(cx,cy);
                }
				GetActiveUIMan()->InvalidateDeviceObjects();
				GetActiveUIMan()->RestoreDeviceObjects( DXUTGetD3DDevice() );
			}
			break;
		case SIZE_RESTORED:
			{
				GetClientRect( &m_rcWindowClient );
				DXUTStaticWndProc( GetSafeHwnd(), WM_SIZE, nType, MAKELPARAM( cx, cy ) );
				//DXUTStaticWndProc( GetSafeHwnd(), WM_EXITSIZEMOVE, 0, 0 );

				//CUIMan::SetResolution( MAKELONG(cx,cy) );
				if(GetActiveUIMan() && GetActiveMode() == UI_EDIT_MODE ) 
                {
					//GetActiveUIMan()->ReSizeWindow(cx,cy);
                }
				GetActiveUIMan()->InvalidateDeviceObjects();
				GetActiveUIMan()->RestoreDeviceObjects( DXUTGetD3DDevice() );
			}
			break;
		}		
	}
	
}
