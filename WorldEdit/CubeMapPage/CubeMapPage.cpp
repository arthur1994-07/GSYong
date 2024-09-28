// D:\Ran\WorldEdit\CubeMapPage\LocalCubeMap.cpp : 구현 파일입니다.
//

#include "stdafx.h"

#include "../WorldEdit.h"
#include "../MainFrm.h"
#include "../WorldEditDoc.h"
#include "../WorldEditView.h"

#include "../../enginelib/DxTools/CubeMap/NSCubeMapCulling.h"
#include "../../enginelib/DxTools/CubeMap/NSCubeMapUtil.h"
#include "../../enginelib/DxTools/DxViewPort.h"

#include "../ExportCubeMapDlg.h"

#include "CubeMapPage.h"


extern D3DXVECTOR3 g_vCLOSED_POINT;

// CCubeMapPage 대화 상자입니다.

IMPLEMENT_DYNAMIC(CCubeMapPage, CPropertyPage)

CCubeMapPage::CCubeMapPage()
	: CPropertyPage(CCubeMapPage::IDD)
{

}

CCubeMapPage::~CCubeMapPage()
{
}

void CCubeMapPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CCubeMapPage, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_LOOK_CUBEMAP, &CCubeMapPage::OnBnClickedButtonLookCubemap)
	ON_BN_CLICKED(IDC_BUTTON_CREATE_LOCAL_CUBE_MAP, &CCubeMapPage::OnBnClickedButtonCreateLocalCubeMap)
	ON_BN_CLICKED(IDC_BUTTON_VIEW_SIMULATION_POSITION, &CCubeMapPage::OnBnClickedButtonViewSimulationPosition)
	ON_BN_CLICKED(IDC_BUTTON_VIEW_OCTREE_BOX, &CCubeMapPage::OnBnClickedButtonViewOctreeBox)
	ON_BN_CLICKED(IDC_BUTTON_CREATE_GLOBAL_CUBE_MAP, &CCubeMapPage::OnBnClickedButtonCreateGlobalCubeMap)
END_MESSAGE_MAP()


// CCubeMapPage 메시지 처리기입니다.

void CCubeMapPage::OnBnClickedButtonLookCubemap()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();

	pView->SetCollisionPos( DxViewPort::GetInstance().GetLookatPt() );
}

void CCubeMapPage::OnBnClickedButtonCreateLocalCubeMap()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd(); //AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView*) pFrame->GetActiveView();

	CString szFilter = "CubeMap (*.dds)|*.dds";
	CFileDialog dlg ( FALSE, ".dds", NULL, OFN_HIDEREADONLY, szFilter, this );

	dlg.m_ofn.lpstrInitialDir = TextureManager::GetInstance().GetPath().c_str();

	if ( dlg.DoModal() == IDOK )
	{
		CString strText;
		CWnd* pWnd = GetDlgItem( IDC_EDIT_TEXTURE_SIZE );
		pWnd->GetWindowText( strText );

		CString sFileName = dlg.GetPathName();
		NSCubeMapUtil::CreateLocalCubeMapOutputFile
		( 
			pView->GetD3DDevice(), 
			sFileName.GetString(), 
			_tstoi( strText.GetString() ), 
			DxViewPort::GetInstance().GetLookatPt(), 
			pView->GetLandMan() 
		);
	}
}

BOOL CCubeMapPage::OnSetActive()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	CWorldEditView::m_ViewType=OCTREE_VIEW;

	CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd(); //AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView*) pFrame->GetActiveView();
	pView->GetLandMan()->ActiveMap_Tool ( pView->GetD3DDevice(), pView->GetspLandMan() );

	CWorldEditView::g_bCubeMapPointRender = FALSE;
	CWorldEditView::g_bCubeMapOctreeRender = FALSE;

	CWnd* pWnd;
	pWnd = GetDlgItem( IDC_EDIT_SIMULATION_DETAIL_LENGTH );
	pWnd->SetWindowText( _T("50") );	// 5M 마다

	pWnd = GetDlgItem( IDC_EDIT_OCTREEBOX_LENGTH );
	pWnd->SetWindowText( _T("150") );	// 15M 마다

	pWnd = GetDlgItem( IDC_EDIT_GLOBAL_CUBEMAP_SIZE );
	pWnd->SetWindowText( _T("32") );

	pWnd = GetDlgItem( IDC_EDIT_TEXTURE_SIZE );
	pWnd->SetWindowText( _T("256") );

	return CPropertyPage::OnSetActive();
}

void CCubeMapPage::OnBnClickedButtonViewSimulationPosition()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();

	CWnd* pWnd;
	CString strText;
	pWnd = GetDlgItem( IDC_EDIT_SIMULATION_DETAIL_LENGTH );
	pWnd->GetWindowText( strText );
	int nSumulationDetailLength = atoi ( strText.GetString() );

	DxLandMan* pLandMan = pView->GetLandMan();
	boost::shared_ptr<NavigationMesh> spNaviMesh = pLandMan->GetNaviMesh();
	NSCubeMapCulling::SimulationPosition( spNaviMesh.get(), nSumulationDetailLength );

	CWorldEditView::g_bCubeMapPointRender = TRUE;
}

void CCubeMapPage::OnBnClickedButtonViewOctreeBox()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CWnd* pWnd;
	CString strText;
	pWnd = GetDlgItem( IDC_EDIT_OCTREEBOX_LENGTH );
	pWnd->GetWindowText( strText );
	int nOctreeBoxLength = atoi ( strText.GetString() );

	NSCubeMapCulling::SetOctreeBoxSize( nOctreeBoxLength );

	CWorldEditView::g_bCubeMapOctreeRender = TRUE;
}

void CCubeMapPage::OnBnClickedButtonCreateGlobalCubeMap()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();

	CWnd* pWnd;
	CString strText;

	pWnd = GetDlgItem( IDC_EDIT_SIMULATION_DETAIL_LENGTH );
	pWnd->GetWindowText( strText );
	int nSumulationDetailLength = atoi ( strText.GetString() );

	pWnd = GetDlgItem( IDC_EDIT_OCTREEBOX_LENGTH );
	pWnd->GetWindowText( strText );
	int nOctreeBoxLength = atoi ( strText.GetString() );

	pWnd = GetDlgItem( IDC_EDIT_GLOBAL_CUBEMAP_SIZE );
	pWnd->GetWindowText( strText );
	int nCubeMapSize = atoi ( strText.GetString() );

	DxLandMan* pLandMan = pView->GetLandMan();

	// 미리 로딩
	D3DXVECTOR3 vMax = D3DXVECTOR3(100000,100000,100000);
	D3DXVECTOR3 vMin = D3DXVECTOR3(-100000,-100000,-100000);
	pLandMan->LoadBasicPos( pView->GetD3DDevice(), vMax, vMin );

	CExportCubeMapDlg dlg;
	dlg.m_SParam.m_nSumulationDetailLength = nSumulationDetailLength;
	dlg.m_SParam.m_nOctreeBoxLength = nOctreeBoxLength;
	dlg.m_SParam.m_dwCubeMapSize = nCubeMapSize;
	dlg.DoModal();
}

BOOL CCubeMapPage::OnKillActive()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	CWorldEditView::m_ViewType=OCTREE_VIEW;

	CWorldEditView::g_bCubeMapPointRender = FALSE;
	CWorldEditView::g_bCubeMapOctreeRender = FALSE;

	return CPropertyPage::OnKillActive();
}
