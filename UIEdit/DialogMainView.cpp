#include "stdafx.h"
#include "./DialogMainView.h"

#include "../EngineLib/GUInterface/UIDataType.h"
#include "../EngineLib/TextTexture/TextUtil.h"

#include "./MainUI/InterfaceAdmin.h"

#include "./UIEdit.h"
#include "./UIEditDoc.h"
#include "./UIEditView.h"
#include "./MainFrm.h"

IMPLEMENT_DYNAMIC( CDlgMainView, CDialog )

BEGIN_MESSAGE_MAP( CDlgMainView, CDialog )
	ON_WM_TIMER()
	ON_WM_KEYUP()
	//	ON_WM_ENABLE()
END_MESSAGE_MAP()

CDlgMainView::CDlgMainView( CWnd* pParent /*=NULL*/ )
: CDialog( CDlgMainView::IDD, pParent )

, m_fMagnify( 4.0f )
{
}

CDlgMainView::~CDlgMainView()
{
}

BOOL CDlgMainView::OnInitDialog()
{
	CDialog::OnInitDialog();

	CRect rtClient;
	GetClientRect( &rtClient );

	if( rtClient.Width() != DLG_SIZE ||
		rtClient.Height() != DLG_SIZE )
	{
		LONG lOffsetW = rtClient.Width() - DLG_SIZE;
		LONG lOffsetH = rtClient.Height() - DLG_SIZE;

		CRect rtWindow;
		GetWindowRect( &rtWindow );

		rtWindow.right -= lOffsetW;
		rtWindow.bottom -= lOffsetH;

		SetWindowPos(
			NULL,
			0, 0,
			rtWindow.Width(), rtWindow.Height(),
			SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER );
	}

	SetTimer( 0, 100, NULL );

	return TRUE;
}

VOID CDlgMainView::OnTimer( UINT_PTR nIDEvent )
{
	if( IsWindowVisible() )
		Render();

	CDialog::OnTimer( nIDEvent );
}

VOID CDlgMainView::OnKeyUp( UINT nChar, UINT nRepCnt, UINT nFlags )
{
	if( nChar == '1' )
		m_fMagnify = 1.0f;
	else if( nChar == '2' )
		m_fMagnify = 2.0f;
	else if( nChar == '3' )
		m_fMagnify = 4.0f;
	else if( nChar == '4' )
		m_fMagnify = 8.0f;
	else if( nChar == '5' )
		m_fMagnify = 16.0f;
}

VOID CDlgMainView::Render()
{
	CMainFrame* pFrame = ( CMainFrame* )AfxGetApp()->m_pMainWnd;
	if( !pFrame )
		return;

	CUIEditView* pView = ( CUIEditView* )pFrame->GetActiveView();
	if( !pView )
		return;

	LPDIRECT3DDEVICEQ pd3dDevice = pView->GetD3dDevice();
	if( !pd3dDevice )
		return;

	LPDIRECT3DSWAPCHAIN9 pMain = pView->m_pSwapChainMain;
	if( !pMain )
		return;

	LPDIRECT3DSWAPCHAIN9 pSwapChain = pView->m_pSwapChainMagnifyingGlass;
	if( !pSwapChain )
		return;

	POINT ptCursor;
	GetCursorPos( &ptCursor );
	pView->ScreenToClient( &ptCursor );

	CRect rtView;
	GetClientRect( &rtView );
	rtView.SetRect
		(
		0,
		0,
		LONG( (FLOAT)rtView.Width() / m_fMagnify ),
		LONG( (FLOAT)rtView.Height() / m_fMagnify )
		);

	D3DPRESENT_PARAMETERS paramMain;
	pMain->GetPresentParameters( &paramMain );
	RECT rtCopy =
	{
		ptCursor.x - rtView.Width() / 2,
			ptCursor.y - rtView.Height() / 2,
			ptCursor.x + rtView.Width() / 2,
			ptCursor.y + rtView.Height() / 2,
	};

	if( rtCopy.left < 0 )
	{
		rtCopy.left = 0;
		rtCopy.right = rtView.Width();
	}
	if( rtCopy.top < 0 )
	{
		rtCopy.top = 0;
		rtCopy.bottom = rtView.Height();
	}
	if( rtCopy.right > (LONG)paramMain.BackBufferWidth )
	{
		rtCopy.left = paramMain.BackBufferWidth - rtView.Width();
		rtCopy.right = paramMain.BackBufferWidth;
	}
	if( rtCopy.bottom > (LONG)paramMain.BackBufferHeight )
	{
		rtCopy.top = paramMain.BackBufferHeight - rtView.Height();
		rtCopy.bottom = paramMain.BackBufferHeight;
	}

	LPDIRECT3DSURFACE9 pSurfaceMain = NULL;
	pMain->GetBackBuffer( 0, D3DBACKBUFFER_TYPE_MONO, &pSurfaceMain );

	LPDIRECT3DSURFACE9 pSurfaceSwap = NULL;
	pSwapChain->GetBackBuffer( 0, D3DBACKBUFFER_TYPE_MONO, &pSurfaceSwap );

	pd3dDevice->StretchRect( pSurfaceMain, &rtCopy, pSurfaceSwap, NULL, D3DTEXF_POINT );

	SAFE_RELEASE( pSurfaceSwap );
	SAFE_RELEASE( pSurfaceMain );

	pSwapChain->Present( NULL, NULL, this->m_hWnd, NULL, 0 );
}

void CDlgMainView::OnCancel()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
    CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
    CInterfaceAdmin* pInterfaceAdmin = pFrame->GetInterfaceAdmin();
	pInterfaceAdmin->m_pMainDlg->m_CheckZoom.SetCheck(FALSE);
	CDialog::OnCancel();
}
