// WaterSurfPage.cpp : 구현 파일입니다.
//

#include "stdafx.h"

#include "../Enginelib/DxTools/TextureManager.h"
#include "../Enginelib/Water/NSWaterMain.h"
#include "../Enginelib/Water/WaterSurf.h"
#include "../Enginelib/Water/WaterSurfPointMan.h"

#include "./WorldEdit.h"
#include "./MainFrm.h"
#include "./WorldEditDoc.h"
#include "./WorldEditView.h"
#include "./SheetWithTab.h"

#include "State/WorldStateMachine.h"

#include "WaterSurfPage.h"

extern D3DXVECTOR3	g_vCLOSED_POINT;

// CWaterSurfPage 대화 상자입니다.

IMPLEMENT_DYNAMIC(CWaterSurfPage, CPropertyPage)

CWaterSurfPage::CWaterSurfPage()
	: CPropertyPage(CWaterSurfPage::IDD)
{

}

CWaterSurfPage::~CWaterSurfPage()
{
}

void CWaterSurfPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_SURF, m_listSurf);
}


BEGIN_MESSAGE_MAP(CWaterSurfPage, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_CREATE, &CWaterSurfPage::OnBnClickedButtonCreate)
	ON_LBN_SELCHANGE(IDC_LIST_SURF, &CWaterSurfPage::OnLbnSelchangeListSurf)
	ON_BN_CLICKED(IDC_BUTTON_MODIFY, &CWaterSurfPage::OnBnClickedButtonModify)
	ON_BN_CLICKED(IDC_BUTTON_INVERSE_UV, &CWaterSurfPage::OnBnClickedButtonInverseUv)
	ON_BN_CLICKED(IDC_BUTTON_MESH_MODIFY, &CWaterSurfPage::OnBnClickedButtonMeshModify)
	ON_BN_CLICKED(IDC_BUTTON_ADD_HEIGHT, &CWaterSurfPage::OnBnClickedButtonAddHeight)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, &CWaterSurfPage::OnBnClickedButtonDelete)
	ON_BN_CLICKED(IDC_BUTTON_OK, &CWaterSurfPage::OnBnClickedButtonOk)
	ON_BN_CLICKED(IDC_BUTTON_MODIFY2, &CWaterSurfPage::OnBnClickedButtonModify2)
END_MESSAGE_MAP()


// CWaterSurfPage 메시지 처리기입니다.

BOOL CWaterSurfPage::OnSetActive()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	pView->m_ViewType = WATER_SURF_VIEW;
//	CWaterMain::m_bSURF_EDIT = TRUE;

	CWnd* pWnd;
	CButton* pButton;
	CString Str;

	pButton = (CButton*)GetDlgItem( IDC_RADIO_LEFT );
	pButton->SetCheck( TRUE );
	pButton = (CButton*)GetDlgItem( IDC_RADIO_RIGHT );
	pButton->SetCheck( FALSE );

	pWnd = GetDlgItem( IDC_EDIT_POINT_INTERVAL );
	Str.Format( _T("50.f") );
	pWnd->SetWindowText( Str.GetString() );

	UpdateList();

	return CPropertyPage::OnSetActive();
}

BOOL CWaterSurfPage::OnKillActive()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	pView->m_ViewType = FRAME_VIEW;
//	CWaterMain::m_bSURF_EDIT = FALSE;

	WaterSurf*	pWaterSurf	= NSWaterMainPSF::GetWaterSurf();
	pWaterSurf->ClearIndex();								// Index Clear

	return CPropertyPage::OnKillActive();
}

void CWaterSurfPage::OnBnClickedButtonCreate()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();

	CButton* pButton;

	BOOL bLeft(FALSE);
	pButton = (CButton*)GetDlgItem( IDC_RADIO_LEFT );
	if( pButton->GetCheck() )
	{
		bLeft = TRUE;
	}

	CString strText;
	CWnd* pWnd = GetDlgItem( IDC_EDIT_POINT_INTERVAL );
	pWnd->GetWindowText( strText );
	float fInterval = (FLOAT)_tstof( strText.GetString() );
	
	WaterSurf* pWaterSurf = NSWaterMainPSF::GetWaterSurf();
	pWaterSurf->CreateSurf( pView->GetD3DDevice(), pView->GetSetLandMan(), g_vCLOSED_POINT, fInterval, bLeft );

    WorldStateMachine::EditAsset(_T("CREATE"));
	UpdateList();
}

void CWaterSurfPage::UpdateList()
{
	m_listSurf.ResetContent ();

	WaterSurf* pWaterSurf = NSWaterMainPSF::GetWaterSurf();
	WaterSurf::VEC_WATERSURFPOINTMAN_ITER iter = pWaterSurf->m_vecWaterSurfPointMan.begin();
	for( ; iter!=pWaterSurf->m_vecWaterSurfPointMan.end(); ++iter )
	{
		int nIndex = m_listSurf.AddString( _T("New") );
		m_listSurf.SetItemData ( nIndex, nIndex );
	}
}

void CWaterSurfPage::OnLbnSelchangeListSurf()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();

	int nIndex = m_listSurf.GetCurSel ();
	if ( nIndex == LB_ERR )		return;

	WaterSurf*				pWaterSurf	= NSWaterMainPSF::GetWaterSurf();
	WaterSurfPointMan*		pSurf		= pWaterSurf->m_vecWaterSurfPointMan[nIndex];

	pWaterSurf->MoveCamera( pView->GetD3DDevice(), nIndex );

	CWnd* pWnd;
	CString Str;

	pWnd = GetDlgItem( IDC_EDIT_PLANE_WIDTH );
	Str.Format( _T("%3.2f"), pSurf->m_fPlaneWidth );
	pWnd->SetWindowText( Str.GetString() );

	pWnd = GetDlgItem( IDC_EDIT_PLANE_HEIGHT );
	Str.Format( _T("%3.2f"), pSurf->m_fPlaneHeight );
	pWnd->SetWindowText( Str.GetString() );

	float fAddHeight(0.f);
	pWnd = GetDlgItem( IDC_EDIT_ADD_HEIGHT );
	Str.Format( _T("%3.2f"), fAddHeight );
	pWnd->SetWindowText( Str.GetString() );

	pWnd = GetDlgItem( IDC_EDIT_SURF_SPEED );
	Str.Format( _T("%3.2f"), pSurf->m_fSurfVelocityINIT );
	pWnd->SetWindowText( Str.GetString() );

	pWnd = GetDlgItem( IDC_EDIT_SURF_ACCELERATION );
	Str.Format( _T("%3.2f"), pSurf->m_fEdit_SurfAcceleration );
	pWnd->SetWindowText( Str.GetString() );

	pWnd = GetDlgItem( IDC_EDIT_SURF_SLEEP_TIME );
	Str.Format( _T("%3.2f"), pSurf->m_fSurfSleepTimeINIT );
	pWnd->SetWindowText( Str.GetString() );


	pWnd = GetDlgItem( IDC_EDIT_ALPHA_ADD );
	Str.Format( _T("%3.2f"), pSurf->m_fAlphaAddition );
	pWnd->SetWindowText( Str.GetString() );

	pWnd = GetDlgItem( IDC_EDIT_ALPHA_SUB_TIME );
	Str.Format( _T("%3.2f"), pSurf->m_fAlphaSubtractionTime );
	pWnd->SetWindowText( Str.GetString() );

	pWnd = GetDlgItem( IDC_EDIT_ALPHA_SUB );
	Str.Format( _T("%3.2f"), pSurf->m_fAlphaSubtraction );
	pWnd->SetWindowText( Str.GetString() );


	//pWnd = GetDlgItem( IDC_EDIT_NEXT_SURF_INTERVAL );
	//Str.Format( _T("%3.2f"), pSurf->m_fNextSurfInterval );
	//pWnd->SetWindowText( Str.GetString() );

	pWnd = GetDlgItem( IDC_EDIT_SIDE_LINE_INTERVAL );
	Str.Format( _T("%3.2f"), pSurf->m_fSideLineInterval );
	pWnd->SetWindowText( Str.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_TEXTURE );
	pWnd->SetWindowText ( pSurf->m_strName.c_str() );
}

void CWaterSurfPage::OnBnClickedButtonModify()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();

	int nIndex = m_listSurf.GetCurSel ();
	if ( nIndex == LB_ERR )		return;

	WaterSurf*				pWaterSurf	= NSWaterMainPSF::GetWaterSurf();
	WaterSurfPointMan*		pSurf		= pWaterSurf->m_vecWaterSurfPointMan[nIndex];

	CWnd* pWnd;
	CString strText;

	pWnd = GetDlgItem( IDC_EDIT_SURF_SPEED );
	pWnd->GetWindowText( strText );
	pSurf->m_fSurfVelocityINIT = static_cast<float>( _tstof( strText.GetString() ) );

	pWnd = GetDlgItem( IDC_EDIT_SURF_ACCELERATION );
	pWnd->GetWindowText( strText );
	pSurf->m_fEdit_SurfAcceleration = static_cast<float>( _tstof( strText.GetString() ) );

	pWnd = GetDlgItem( IDC_EDIT_SURF_SLEEP_TIME );
	pWnd->GetWindowText( strText );
	pSurf->m_fSurfSleepTimeINIT = static_cast<float>( _tstof( strText.GetString() ) );


	pWnd = GetDlgItem( IDC_EDIT_ALPHA_ADD );
	pWnd->GetWindowText( strText );
	pSurf->m_fAlphaAddition = static_cast<float>( _tstof( strText.GetString() ) );

	pWnd = GetDlgItem( IDC_EDIT_ALPHA_SUB_TIME );
	pWnd->GetWindowText( strText );
	pSurf->m_fAlphaSubtractionTime = static_cast<float>( _tstof( strText.GetString() ) );

	pWnd = GetDlgItem( IDC_EDIT_ALPHA_SUB );
	pWnd->GetWindowText( strText );
	pSurf->m_fAlphaSubtraction = static_cast<float>( _tstof( strText.GetString() ) );


	//pWnd = GetDlgItem( IDC_EDIT_NEXT_SURF_INTERVAL );
	//pWnd->GetWindowText( strText );
	//pSurf->m_fNextSurfInterval = static_cast<float>( _tstof( strText.GetString() ) );

	pWnd = GetDlgItem( IDC_EDIT_SIDE_LINE_INTERVAL );
	pWnd->GetWindowText( strText );
	pSurf->m_fSideLineInterval = static_cast<float>( _tstof( strText.GetString() ) );

	pWnd = GetDlgItem ( IDC_EDIT_TEXTURE );
	pWnd->GetWindowText( strText );
	pSurf->m_strName = strText.GetString();

	pSurf->ResetData();

	pSurf->OnDestroyDevice();
	pSurf->OnCreateDevice( pView->GetD3DDevice() );
}

void CWaterSurfPage::OnBnClickedButtonInverseUv()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();

	int nIndex = m_listSurf.GetCurSel ();
	if ( nIndex == LB_ERR )		return;

	WaterSurf*				pWaterSurf	= NSWaterMainPSF::GetWaterSurf();
	WaterSurfPointMan*		pSurf		= pWaterSurf->m_vecWaterSurfPointMan[nIndex];

	pSurf->m_bPlaneInverse = pSurf->m_bPlaneInverse ? FALSE : TRUE;

	pSurf->PlaneRemake( pView->GetSetLandMan() );
}

void CWaterSurfPage::OnBnClickedButtonMeshModify()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();

	int nIndex = m_listSurf.GetCurSel ();
	if ( nIndex == LB_ERR )		return;

	WaterSurf* pWaterSurf = NSWaterMainPSF::GetWaterSurf();
	WaterSurfPointMan* pSurf = pWaterSurf->m_vecWaterSurfPointMan[nIndex];

	CWnd* pWnd;
	CString strText;
	
	pWnd = GetDlgItem( IDC_EDIT_PLANE_WIDTH );
	pWnd->GetWindowText( strText );
	float fWidth = static_cast<float>( _tstof( strText.GetString() ) );

	pWnd = GetDlgItem( IDC_EDIT_PLANE_HEIGHT );
	pWnd->GetWindowText( strText );
	float fHeight = static_cast<float>( _tstof( strText.GetString() ) );

	pSurf->m_fPlaneWidth	= fWidth;
	pSurf->m_fPlaneHeight	= fHeight;

	pSurf->PlaneRemake( pView->GetSetLandMan() );
}

void CWaterSurfPage::OnBnClickedButtonAddHeight()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nIndex = m_listSurf.GetCurSel ();
	if ( nIndex == LB_ERR )		return;

	WaterSurf*				pWaterSurf	= NSWaterMainPSF::GetWaterSurf();
	WaterSurfPointMan*		pSurf		= pWaterSurf->m_vecWaterSurfPointMan[nIndex];

	CWnd* pWnd;
	CString strText;

	pWnd = GetDlgItem( IDC_EDIT_ADD_HEIGHT );
	pWnd->GetWindowText( strText );
	float fAddHeight = static_cast<float>( _tstof( strText.GetString() ) );

	pSurf->AddHeight( fAddHeight );
}

void CWaterSurfPage::OnBnClickedButtonDelete()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nIndex = m_listSurf.GetCurSel ();
	if ( nIndex == LB_ERR )		return;

	WaterSurf* pWaterSurf = NSWaterMainPSF::GetWaterSurf();
	pWaterSurf->DeleteSurf( nIndex );

    WorldStateMachine::EditAsset(_T("DELETE"));
	UpdateList();
}

void CWaterSurfPage::OnBnClickedButtonOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pSheetTabParent->SetActiveBackUpPage( WATERSURFPAGEINDEX );
}

void CWaterSurfPage::OnBnClickedButtonModify2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString szFilter = _T("Texture Image (*.bmp,*.dds,*.tga,*.jpg)|*.bmp;*.dds;*.tga;*.jpg|");
	CFileDialog dlg ( TRUE, _T(".BMP"), NULL, 
		OFN_HIDEREADONLY, szFilter, this );

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	dlg.m_ofn.lpstrInitialDir = TextureManager::GetInstance().GetPath().c_str();

	if ( dlg.DoModal() == IDOK )
	{
		CString FilePath = dlg.GetFileName ();

		CWnd *pWnd;
		pWnd = GetDlgItem ( IDC_EDIT_TEXTURE );
		pWnd->SetWindowText ( FilePath );
	}
}
