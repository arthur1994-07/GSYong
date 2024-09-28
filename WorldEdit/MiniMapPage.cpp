// MiniMapPage.cpp : 구현 파일입니다.
//

#include "stdafx.h"

#include "MainFrm.h"
#include "sheetWithTab.h"
#include "WorldEdit.h"
#include "WorldEditDoc.h"
#include "WorldEditView.h"

#include "MiniMapPage.h"


// CMiniMapPage 대화 상자입니다.

IMPLEMENT_DYNAMIC(CMiniMapPage, CPropertyPage)

CMiniMapPage::CMiniMapPage()
	: CPropertyPage(CMiniMapPage::IDD)
	, m_dwTexSize(256)
{

}

CMiniMapPage::~CMiniMapPage()
{
}

void CMiniMapPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_TEXSIZE, m_comboTexSize);
}


BEGIN_MESSAGE_MAP(CMiniMapPage, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_MAKE_MINIMAP, &CMiniMapPage::OnBnClickedButtonMakeMinimap)
	ON_BN_CLICKED(IDC_BUTTON_OK, &CMiniMapPage::OnBnClickedButtonOk)
	ON_CBN_SELCHANGE(IDC_COMBO_TEXSIZE, &CMiniMapPage::OnCbnSelchangeComboTexsize)
END_MESSAGE_MAP()


// CMiniMapPage 메시지 처리기입니다.

BOOL CMiniMapPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	TSTRING strName = _T("0.f");
	CWnd* pWnd = GetDlgItem( IDC_EDIT_MIN_X );
	pWnd->SetWindowText( strName.c_str() );

	pWnd = GetDlgItem( IDC_EDIT_MIN_Z );
	pWnd->SetWindowText( strName.c_str() );

	pWnd = GetDlgItem( IDC_EDIT_MAX_X );
	pWnd->SetWindowText( strName.c_str() );

	pWnd = GetDlgItem( IDC_EDIT_MAX_Z );
	pWnd->SetWindowText( strName.c_str() );

	DWORD nIndex;
	nIndex = m_comboTexSize.AddString ( _T("256") );
	m_comboTexSize.SetItemData ( nIndex, nIndex );

	nIndex = m_comboTexSize.AddString ( _T("512") );
	m_comboTexSize.SetItemData ( nIndex, nIndex );

	nIndex = m_comboTexSize.AddString ( _T("1024") );
	m_comboTexSize.SetItemData ( nIndex, nIndex );

	nIndex = m_comboTexSize.AddString ( _T("2048") );
	m_comboTexSize.SetItemData ( nIndex, nIndex );

	nIndex = m_comboTexSize.AddString ( _T("4096") );
	m_comboTexSize.SetItemData ( nIndex, nIndex );

	m_comboTexSize.SetCurSel( 0 );

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CMiniMapPage::OnCbnSelchangeComboTexsize()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nIndex = m_comboTexSize.GetCurSel();
	if( nIndex == -1 )	return;

	switch( nIndex )
	{
	case 0:
		m_dwTexSize = 256;
		break;
	case 1:
		m_dwTexSize = 512;
		break;
	case 2:
		m_dwTexSize = 1024;
		break;
	case 3:
		m_dwTexSize = 2048;
		break;
	case 4:
		m_dwTexSize = 4096;
		break;
	};
}

void CMiniMapPage::OnBnClickedButtonMakeMinimap()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView ();

	CString strName;
	CWnd* pWnd = GetDlgItem ( IDC_EDIT_NAME );
	pWnd->GetWindowText( strName );

	D3DXVECTOR2 vMin(0.f,0.f);
	D3DXVECTOR2 vMax(0.f,0.f);
	pView->MakeMiniMap( vMin, vMax, m_dwTexSize, strName.GetString() );

	pWnd = GetDlgItem( IDC_EDIT_MIN_X );
	SetWindowText_gcvt( pWnd, vMin.x, 7 );

	pWnd = GetDlgItem( IDC_EDIT_MIN_Z );
	SetWindowText_gcvt( pWnd, vMin.y, 7 );

	pWnd = GetDlgItem( IDC_EDIT_MAX_X );
	SetWindowText_gcvt( pWnd, vMax.x, 7 );

	pWnd = GetDlgItem( IDC_EDIT_MAX_Z );
	SetWindowText_gcvt( pWnd, vMax.y, 7 );
}

void CMiniMapPage::OnBnClickedButtonOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pSheetTab->SetActivePage( FRAMEPAGEINDEX );
}

BOOL CMiniMapPage::OnSetActive()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	CWorldEditView::g_bMiniMap = TRUE;
	CWorldEditView::g_bOrthoView = TRUE;
	DxViewPort::GetInstance().SetToolCameraMiniMap();

	CWorldEditView::m_ViewType=OCTREE_VIEW;

	return CPropertyPage::OnSetActive();
}

BOOL CMiniMapPage::OnKillActive()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView ();

	CWorldEditView::g_bMiniMap = FALSE;
	CWorldEditView::g_bOrthoView = FALSE;

	D3DXVECTOR3 vLookatPt = DxViewPort::GetInstance().GetLookatPt();
	D3DXVECTOR3 vFromPt = DxViewPort::GetInstance().GetFromPt();

	D3DXVECTOR3 vDir = vFromPt - vLookatPt;
	float fLength = D3DXVec3Length( &vDir );

	// Note : View Port를 설정한다.
	D3DXMATRIX matView;
	vFromPt = vLookatPt + D3DXVECTOR3( 0.f, 0.f, fLength );
	D3DXVECTOR3 vUpVec( 0.f, 1.f, 0.f );
	DxViewPort::GetInstance().SetViewTrans ( vFromPt, vLookatPt, vUpVec );

	LPDIRECT3DSURFACEQ pBackBuffer;
	D3DSURFACE_DESC d3dsdBackBuffer;
	pView->GetD3DDevice()->GetBackBuffer ( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer );
	pBackBuffer->GetDesc( &d3dsdBackBuffer );
	pBackBuffer->Release();

	DxViewPort::GetInstance().SetProjection ( D3DX_PI/4, (float)d3dsdBackBuffer.Width, 
												(float)d3dsdBackBuffer.Height, 5.0f, 3000.0f );
	DxViewPort::GetInstance().SetToolCamera();

	CWorldEditView::m_ViewType=OCTREE_VIEW;

	return CPropertyPage::OnKillActive();
}
