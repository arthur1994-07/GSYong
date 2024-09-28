// Tool_VisualMaterialView.cpp : CTool_VisualMaterialView 클래스의 구현
//

#include "stdafx.h"

#include "../VisualMaterialLib/DxVisualMaterial.h"

#include "../EngineLib/dxtools/TextureManager.h"
#include "../EngineLib/DxCommon9/DXUTmisc.h"

#include "./MainFrm.h"
#include "Tool_VisualMaterial.h"
#include "Tool_VisualMaterialDoc.h"
#include "Tool_VisualMaterialView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CTool_VisualMaterialView

IMPLEMENT_DYNCREATE(CTool_VisualMaterialView, CView)

BEGIN_MESSAGE_MAP(CTool_VisualMaterialView, CView)
	// 표준 인쇄 명령입니다.
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
    ON_WM_SIZE()
    ON_WM_DESTROY()
    ON_WM_LBUTTONUP()
    ON_WM_SETCURSOR()
END_MESSAGE_MAP()

// CTool_VisualMaterialView 생성/소멸

CTool_VisualMaterialView::CTool_VisualMaterialView()
    : m_pSwapChainMain(NULL)
    , CD3DApplication(SP_OFFICE_TEST)
{
	// TODO: 여기에 생성 코드를 추가합니다.
    SetServiceProvider(SP_OFFICE_TEST);

    m_pVisualMaterial = new DxVisualMaterial;

    m_bUseDepthBuffer	= TRUE;
}

CTool_VisualMaterialView::~CTool_VisualMaterialView()
{
    //CORE_COMMON::getFileSystem()->releaseInstance();

    SAFE_DELETE( m_pVisualMaterial );
}

BOOL CTool_VisualMaterialView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return CView::PreCreateWindow(cs);
}

// CTool_VisualMaterialView 그리기

void CTool_VisualMaterialView::OnDraw(CDC* /*pDC*/)
{
	CTool_VisualMaterialDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 여기에 원시 데이터에 대한 그리기 코드를 추가합니다.
}


// CTool_VisualMaterialView 인쇄

BOOL CTool_VisualMaterialView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 기본적인 준비
	return DoPreparePrinting(pInfo);
}

void CTool_VisualMaterialView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄하기 전에 추가 초기화 작업을 추가합니다.
}

void CTool_VisualMaterialView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄 후 정리 작업을 추가합니다.
}


// CTool_VisualMaterialView 진단

#ifdef _DEBUG
void CTool_VisualMaterialView::AssertValid() const
{
	CView::AssertValid();
}

void CTool_VisualMaterialView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CTool_VisualMaterialDoc* CTool_VisualMaterialView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CTool_VisualMaterialDoc)));
	return (CTool_VisualMaterialDoc*)m_pDocument;
}
#endif //_DEBUG


// CTool_VisualMaterialView 메시지 처리기
static CTool_VisualMaterialView* g_pView = NULL;
LRESULT CALLBACK FullScreenWndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
bool CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, const D3DCAPS9* pCaps, void* pUserContext );
bool CALLBACK IsDeviceAcceptable( D3DCAPS9* pCaps, D3DFORMAT AdapterFormat, 
                                  D3DFORMAT BackBufferFormat, bool bWindowed, void* pUserContext );
//
////--------------------------------------------------------------------------------------------------------------------------------------------------
//// DXUT Callbacks
////--------------------------------------------------------------------------------------------------------------------------------------------------
//void CTool_VisualMaterialView::OnFrameMoveCB( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext )
//{
//	g_pView->OnFrameMove( pd3dDevice, fTime, fElapsedTime );
//
//    CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
//    pFrame->OnFrameMove( fElapsedTime );
//}
//
//void CTool_VisualMaterialView::OnFrameRenderCB( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext )
//{
//	g_pView->OnFrameRender( pd3dDevice, fTime, fElapsedTime );
//}
//
//HRESULT CTool_VisualMaterialView::OnCreateDeviceCB( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext )
//{
//	g_pView->OnCreateDevice( pd3dDevice, pBackBufferSurfaceDesc );
//
//    CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
//    pFrame->OnCreateDevice( pd3dDevice );
//
//    return S_OK;
//}
//
//HRESULT CTool_VisualMaterialView::OnResetDeviceCB( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext )
//{
//	g_pView->OnResetDevice( pd3dDevice, pBackBufferSurfaceDesc );
//
//    CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
//    pFrame->OnResetDevice( pd3dDevice );
//
//    return S_OK;
//}
//
//void CTool_VisualMaterialView::OnLostDeviceCB( void* pUserContext )
//{
//	g_pView->OnLostDevice();
//
//    CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
//    pFrame->OnLostDevice();
//}
//
//void CTool_VisualMaterialView::OnDestroyDeviceCB( void* pUserContext )
//{
//	g_pView->OnDestroyDevice();
//}

//-----------------------------------------------------------------------------
// Name: FullScreenWndProc()
// Desc: The WndProc funtion used when the app is in fullscreen mode. This is
//       needed simply to trap the ESC key.
//-----------------------------------------------------------------------------
LRESULT CALLBACK FullScreenWndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    if( msg == WM_CREATE )
    {
        g_pView = (CTool_VisualMaterialView*)((CREATESTRUCT*)lParam)->lpCreateParams;
    }
    else if( msg == WM_CLOSE )
    {
        // User wants to exit, so go back to windowed mode and exit for real
        g_pView->OnToggleFullscreen();
        g_pView->GetActiveWindow()->PostMessage( WM_CLOSE, 0, 0 );
        //g_App.GetMainWnd()->PostMessage( WM_CLOSE, 0, 0 );
    }
    else if( msg == WM_SETCURSOR )
    {
        SetCursor( NULL );
    }
    else if( ( msg == WM_KEYUP && wParam == VK_ESCAPE ) ||
             ( msg == WM_SYSCHAR && wParam == VK_RETURN ) )
    {
        // User wants to leave fullscreen mode
        g_pView->OnToggleFullscreen();
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
        ::MapWindowPoints( hWnd, g_pView->GetSafeHwnd(), &pt, 1 );
        lParam = MAKELPARAM( pt.x, pt.y );

        // forward mouse messages to the render view window
        g_pView->PostMessage( msg, wParam, lParam );
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

void CTool_VisualMaterialView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView)
{
    // TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
    CTool_VisualMaterialApp *pApp = (CTool_VisualMaterialApp*) AfxGetApp();
	pApp->SetActive( bActivate );

    CView::OnActivateView(bActivate, pActivateView, pDeactiveView);
}

void CTool_VisualMaterialView::OnInitialUpdate()
{
    CView::OnInitialUpdate();

    // TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
    CView::m_hWnd = CView::m_hWnd;

    //	Note : D3D APP 생성.
	//
    HRESULT hr(S_OK);
	hr = CD3DApplication::Create ( CView::m_hWnd, CView::m_hWnd, AfxGetInstanceHandle() );
	if ( FAILED(hr) )
	{
		m_bCreated = FALSE;
		return;
	}

	m_bCreated = TRUE;

//	// Register a class for a fullscreen window
//    WNDCLASS wndClass = { CS_HREDRAW | CS_VREDRAW, FullScreenWndProc, 0, 0, NULL,
//                          NULL, LoadCursor(NULL, IDC_ARROW), (HBRUSH)GetStockObject(WHITE_BRUSH), NULL,
//                          _T("Fullscreen Window") };
//    RegisterClass( &wndClass );
//
//	m_hwndRenderFullScreen = CreateWindow( _T("Fullscreen Window"), NULL,
//                                           WS_POPUP, 0, 0, 100, 100,
//                                           GetTopLevelParent()->GetSafeHwnd(), 0L, NULL, this );
// 
//    // If we haven't initialized the D3D framework, do so now
//    if( !DXUTGetD3DDevice() )
//    {
//        CRect rc;
//        ::GetClientRect( CView::m_hWnd, &rc );
//        if( rc.Width() == 0 || rc.Height() == 0 )
//        {
//            MessageBox( TEXT("The render view must be visible when EffectEdit starts.  Please change the window size or splitters and start EffectEdit again."),
//                TEXT("EffectEdit") );
//        }
//        else
//        {
////            m_SettingsDlg.Init( &m_DialogResourceManager );
//
//            // Set up the DXUT callbacks
//            DXUTSetCallbackDeviceCreated( OnCreateDeviceCB );
//            DXUTSetCallbackDeviceReset( OnResetDeviceCB );
//            DXUTSetCallbackDeviceLost( OnLostDeviceCB );
//            DXUTSetCallbackDeviceDestroyed( OnDestroyDeviceCB );
//            DXUTSetCallbackFrameRender( OnFrameRenderCB );
//            DXUTSetCallbackFrameMove( OnFrameMoveCB );
//
//            // Show the cursor and clip it when in full screen
//            DXUTSetCursorSettings( true, true );
//
//            DXUTInit( true, true, true ); // Parse the command line, handle the default hotkeys, and show msgboxes
//
//            // Note that for the MFC samples, the device window and focus window
//            // are not the same.
//			DXUTSetWindow( CView::m_hWnd, m_hwndRenderFullScreen, CView::m_hWnd, false );
//            ::GetClientRect( CView::m_hWnd, &m_rcWindowClient );
//            HRESULT hr = DXUTCreateDevice( D3DADAPTER_DEFAULT, true, m_rcWindowClient.right - m_rcWindowClient.left,
//                                           m_rcWindowClient.bottom - m_rcWindowClient.top, IsDeviceAcceptable, ModifyDeviceSettings );
//            if( FAILED( hr ) )
//            {
//                // If D3D is not functional, do not continue.  End the application now.
//                CFrameWnd *pTop = this->GetTopLevelFrame();
//                if( pTop )
//                    pTop->PostMessage( WM_CLOSE, 0, 0 );
//
//				m_bCreated = FALSE;
//            }
//        }
//    }
//
//	m_bCreated = TRUE;
}

void CTool_VisualMaterialView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
	
    // TODO: Add your message handler code here
	if ( m_bCreated )	
        OnSize ( cx, cy );

	// TODO: Add your message handler code here
	if( GetD3DDevice() != NULL )
    {
		GetClientRect( &m_rcWindowClient );
        DXUTStaticWndProc( GetSafeHwnd(), WM_SIZE, nType, MAKELPARAM( cx, cy ) );
        DXUTStaticWndProc( GetSafeHwnd(), WM_EXITSIZEMOVE, 0, 0 );
    }
}

void CTool_VisualMaterialView::PostNcDestroy()
{
    // TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

    //	Note : D3D APP 파괴.
	//
	Cleanup3DEnvironment();

    //DXUTShutdown();

	::SetCursor ( NULL );
	::DestroyCursor ( m_hCursorDefault );

    CView::PostNcDestroy();
}

void CTool_VisualMaterialView::OnDestroy()
{
    CView::OnDestroy();

    // TODO: 여기에 메시지 처리기 코드를 추가합니다.
    ::DestroyWindow( m_hwndRenderFullScreen );
}

void CTool_VisualMaterialView::OnLButtonUp(UINT nFlags, CPoint point)
{
    // TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

    if ( !m_pVisualMaterial )
        return;

    if ( NSVISUAL_MATERIAL::g_nSelectCommandVM == -1 )
        return;

    m_pVisualMaterial->CreateCommand( static_cast<VMC_TYPE>(NSVISUAL_MATERIAL::g_nSelectCommandVM) );

    NSVISUAL_MATERIAL::g_nSelectCommandVM = -1;

    CView::OnLButtonUp(nFlags, point);
}

BOOL CTool_VisualMaterialView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
    // TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

    if ( NSVISUAL_MATERIAL::g_nSelectCommandVM == -1 )
    {
        SetCursor( LoadCursor( NULL, IDC_ARROW ) );
    }
    else
    {
        SetCursor( LoadCursor( NULL, IDC_HAND ) );
    }

    return TRUE;
    //return CView::OnSetCursor(pWnd, nHitTest, message);
}
