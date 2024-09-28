
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "../../VisualMaterialLib/ViewPort/DxViewPortPreview.h"
#include "../../VisualMaterialLib/Preview/NSVM_PREVIEW.h"
#include "../../VisualMaterialLib/DxVisualMaterial.h"

#include "../../EngineLib/DxTools/PostProcessing/NSGlow.h"
#include "../../EngineLib/DxTools/DxBackUpRendTarget.h"
#include "../../EngineLib/DxTools/DxInputDevice.h"
#include "../../EngineLib/DxTools/DxSurfaceTex.h"

#include "../Tool_VisualMaterialView.h"
#include "../MainFrm.h"
#include "PreviewWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



CPreviewWnd::CPreviewWnd()
    : m_pSwapChainPreview(NULL)
    , m_nScreenX(100)
    , m_nScreenY(100)
{
    m_pViewPort = new DxViewPortPreview;
}

CPreviewWnd::~CPreviewWnd()
{
    SAFE_RELEASE( m_pSwapChainPreview );
    SAFE_DELETE( m_pViewPort );
}

BEGIN_MESSAGE_MAP(CPreviewWnd, CWnd)
    ON_WM_SIZE()
END_MESSAGE_MAP()

BOOL CPreviewWnd::PreCreateWindow( CREATESTRUCT& cs )
{
	if( !CWnd::PreCreateWindow( cs ) )
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(
        CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS,
        LoadCursor( NULL, IDC_ARROW ),
        (HBRUSH)GetStockObject( GRAY_BRUSH ),
        NULL );

	return TRUE;
}

HRESULT CPreviewWnd::OnCreateDevice( IDirect3DDevice9* pd3dDevice )
{
    // DxViewPortVM 관련 초기화
    {
        m_pViewPort->OnCreateDevice( pd3dDevice );

        m_pViewPort->SetViewPort();

        D3DXVECTOR3 vFromPt		= D3DXVECTOR3( 0.f, 0.f, -50.f );
	    D3DXVECTOR3 vLookatPt	= D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	    D3DXVECTOR3 vUpVec		= D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
        m_pViewPort->SetViewTrans( vFromPt, vLookatPt, vUpVec );

        m_pViewPort->SetProjection
        ( 
            D3DX_PI/4, 
            (float)m_nScreenX, 
            (float)m_nScreenY, 
            5.f,
            3000.0f 
        );
    }

    return S_OK;
}

HRESULT CPreviewWnd::OnResetDevice( IDirect3DDevice9* pd3dDevice )
{
    CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
    if ( !pFrame )
        return S_OK;

    CTool_VisualMaterialView *pView = (CTool_VisualMaterialView*) pFrame->GetActiveView ();
    if ( !pView )
        return S_OK;

    int cx = m_nScreenX;
    int cy = m_nScreenY;

    if ( cx > static_cast<int>( pView->GetBackBufferWidth() ) )
        cx = pView->GetBackBufferWidth();
    if ( cy > static_cast<int>( pView->GetBackBufferHeight() ) )
        cy = pView->GetBackBufferHeight();

    CreateSwapChain( cx, cy );
    NSVM_PREVIEW::OnResetDevice( pd3dDevice );
    return S_OK;
}

void CPreviewWnd::OnLostDevice()
{
    SAFE_RELEASE( m_pSwapChainPreview );
    NSVM_PREVIEW::OnLostDevice();
}

void CPreviewWnd::OnFrameMove( float fElapsedTime )
{
    if ( GetSafeHwnd() == 0 )
        return;

    m_pViewPort->SetHwnd( GetSafeHwnd() );
    m_pViewPort->FrameMove( fElapsedTime );

    POINT ptCurPos;
	GetCursorPos ( &ptCurPos );
    BOOL bTARPOSWIN = m_pViewPort->GetMouseTargetPosWnd( D3DXVECTOR3((float)ptCurPos.x,(float)ptCurPos.y,0) );

    NSVM_PREVIEW::OnFrameMove( bTARPOSWIN, fElapsedTime );
}

HRESULT CPreviewWnd::Render()
{
    if ( GetSafeHwnd() == 0 )
        return S_OK;

    if ( !m_pSwapChainPreview )
        return S_OK;

    // Device에 카메라 셋팅 정보를 등록한다.
    m_pViewPort->SetTransform();

    CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
    CTool_VisualMaterialView *pView = (CTool_VisualMaterialView*) pFrame->GetActiveView ();
    LPDIRECT3DDEVICEQ pd3dDevice = pView->GetD3DDevice();

    DxBackupTarget sBackupTarget;
    sBackupTarget.Backup( pd3dDevice );

	LPDIRECT3DSURFACE9 pBackBuffer = NULL;
	m_pSwapChainPreview->GetBackBuffer( 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer );
	pd3dDevice->SetRenderTarget( 0, pBackBuffer );
	pd3dDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0,0,0), 1.0f, 0);
	pd3dDevice->BeginScene();

    BOOL bActivePL(FALSE);
    POINT ptCurPos;
	GetCursorPos ( &ptCurPos );
    BOOL bTARPOSWIN = m_pViewPort->GetMouseTargetPosWnd( D3DXVECTOR3((float)ptCurPos.x,(float)ptCurPos.y,0) );
    if ( bTARPOSWIN )
    {
        if( DxInputDevice::GetInstance().GetKeyState(DIK_L) & DXKEY_DOWNED )
        {
            bActivePL = TRUE;
        }
    }

    NSVM_PREVIEW::Render( pd3dDevice, m_pViewPort->GetFromPt(), m_pViewPort->GetLookatPt(), bActivePL );

	
	if( DxSurfaceTex::GetInstance().m_pGlowSuf_SRC )
	{
		pd3dDevice->SetRenderTarget ( 0, DxSurfaceTex::GetInstance().m_pGlowSuf_SRC );
		pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,0), 1.0f, 0L);

		NSGlow::SetActiveGlow();
		NSVM_PREVIEW::Render_StarGlow( pd3dDevice );

		// Note : Glow 작업을 한다.
		NSGlow::OnRender( pd3dDevice, pBackBuffer );
	}

    pd3dDevice->EndScene();
    m_pSwapChainPreview->Present( NULL, NULL, GetSafeHwnd(), NULL, 0 );

	pBackBuffer->Release();

    return S_OK;
}

void CPreviewWnd::OnSize(UINT nType, int cx, int cy)
{
    CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
    if ( !pFrame )
        return;

    CTool_VisualMaterialView *pView = (CTool_VisualMaterialView*) pFrame->GetActiveView ();
    if ( !pView )
        return;

    CWnd::OnSize(nType, cx, cy);

    // TODO: 여기에 메시지 처리기 코드를 추가합니다.
    m_nScreenX = cx;
    m_nScreenY = cy;

    if ( cx > static_cast<int>( pView->GetBackBufferWidth() ) )
        cx = pView->GetBackBufferWidth();
    if ( cy > static_cast<int>( pView->GetBackBufferHeight() ) )
        cy = pView->GetBackBufferHeight();

    CreateSwapChain( cx, cy );
}

void CPreviewWnd::CreateSwapChain( int cx, int cy )
{
    CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
    if ( !pFrame )
        return;

    CTool_VisualMaterialView *pView = (CTool_VisualMaterialView*) pFrame->GetActiveView ();
    if ( !pView )
        return;

    LPDIRECT3DDEVICEQ pd3dDevice = pView->GetD3DDevice();
    if ( !pd3dDevice )
        return;


    D3DPRESENT_PARAMETERS presentParams;

    ZeroMemory(&presentParams, sizeof(presentParams));

    presentParams.Windowed = TRUE;
    presentParams.SwapEffect = D3DSWAPEFFECT_COPY;
    presentParams.hDeviceWindow = GetSafeHwnd();
    presentParams.BackBufferWidth = cx;
    presentParams.BackBufferHeight = cy;
    presentParams.BackBufferFormat = D3DFORMAT(D3DADAPTER_DEFAULT);

    SAFE_RELEASE( m_pSwapChainPreview );
    pd3dDevice->CreateAdditionalSwapChain( &presentParams, &m_pSwapChainPreview );

    // ViewPort
    m_pViewPort->SetViewPort();
    m_pViewPort->SetProjection
    ( 
        D3DX_PI/4, 
        (float)m_nScreenX, 
        (float)m_nScreenY, 
        5.f,
        3000.0f 
    );
}
