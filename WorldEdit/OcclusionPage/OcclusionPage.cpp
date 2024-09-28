// OcclusionPage/OcclusionPage.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "../MainFrm.h"
#include "../sheetWithTab.h"
#include "../WorldEdit.h"
#include "../WorldEditDoc.h"
#include "../WorldEditView.h"

#include "../../enginelib/DxTools/Occlusion/NSOcclusionCulling.h"
#include "../ExportMiniDlg.h"

#include "OcclusionPage.h"


// COcclusionPage 대화 상자입니다.

IMPLEMENT_DYNAMIC(COcclusionPage, CPropertyPage)

COcclusionPage::COcclusionPage()
	: CPropertyPage(COcclusionPage::IDD)
{

}

COcclusionPage::~COcclusionPage()
{
}

void COcclusionPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(COcclusionPage, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_VIEW_SIMULATION_POSITION, &COcclusionPage::OnBnClickedButtonViewSimulationPosition)
	ON_BN_CLICKED(IDC_BUTTON_BAKE, &COcclusionPage::OnBnClickedButtonBake)
	ON_BN_CLICKED(IDC_BUTTON_BAKE_THIS_PLACE, &COcclusionPage::OnBnClickedButtonBakeThisPlace)
	ON_BN_CLICKED(IDC_BUTTON_OK, &COcclusionPage::OnBnClickedButtonOk)
	ON_BN_CLICKED(IDC_BUTTON_VIEW_OCTREE_BOX, &COcclusionPage::OnBnClickedButtonViewOctreeBox)
	ON_BN_CLICKED(IDC_CHECK_FIELD, &COcclusionPage::OnBnClickedCheckField)
	ON_BN_CLICKED(IDC_CHECK_PIECE, &COcclusionPage::OnBnClickedCheckPiece)
	ON_BN_CLICKED(IDC_CHECK_CHAR, &COcclusionPage::OnBnClickedCheckChar)
END_MESSAGE_MAP()


// COcclusionPage 메시지 처리기입니다.

BOOL COcclusionPage::OnSetActive()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	CWorldEditView::m_ViewType=OCTREE_VIEW;

	CWorldEditView::g_bOcclsionPointRender = FALSE;
	CWorldEditView::g_bOcclsionOctreeRender = FALSE;

	CWnd* pWnd;
	pWnd = GetDlgItem( IDC_EDIT_SIMULATION_DETAIL_LENGTH );
	pWnd->SetWindowText( _T("20") );	// 2M 마다

	pWnd = GetDlgItem( IDC_EDIT_OCTREEBOX_LENGTH );
	pWnd->SetWindowText( _T("80") );	// 8M 마다

	CButton* pButton;
	pButton = (CButton*)GetDlgItem( IDC_CHECK_FIELD );
	pButton->SetCheck( TRUE );

	pButton = (CButton*)GetDlgItem( IDC_CHECK_PIECE );
	pButton->SetCheck( TRUE );

	pButton = (CButton*)GetDlgItem( IDC_CHECK_CHAR );
	pButton->SetCheck( FALSE );

	//pWnd = GetDlgItem( IDC_EDIT_OCTREEBOX_LENGTH_CHAR );
	//pWnd->SetWindowText( _T("160") );	// 16M 마다

	return CPropertyPage::OnSetActive();
}

BOOL COcclusionPage::OnKillActive()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	CWorldEditView::m_ViewType=OCTREE_VIEW;

	CWorldEditView::g_bOcclsionPointRender = FALSE;
	CWorldEditView::g_bOcclsionOctreeRender = FALSE;

	return CPropertyPage::OnKillActive();
}

void COcclusionPage::OnBnClickedButtonViewSimulationPosition()
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
	NSOcclusionCulling::SimulationPosition( spNaviMesh.get(), nSumulationDetailLength );

	CWorldEditView::g_bOcclsionPointRender = TRUE;
}

void COcclusionPage::OnBnClickedButtonBake()
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

	CButton* pButton;
	pButton = (CButton*)GetDlgItem( IDC_CHECK_FIELD );
	BOOL bFieldOcclusion = static_cast<BOOL>( pButton->GetCheck() );

	pButton = (CButton*)GetDlgItem( IDC_CHECK_PIECE );
	BOOL bPieceOcclusion = static_cast<BOOL>( pButton->GetCheck() );

	pButton = (CButton*)GetDlgItem( IDC_CHECK_CHAR );
	BOOL bCharOcclusion = static_cast<BOOL>( pButton->GetCheck() );

	//pWnd = GetDlgItem( IDC_EDIT_OCTREEBOX_LENGTH_CHAR );
	//pWnd->GetWindowText( strText );
	//int nOctreeBoxLength_CHAR = atoi ( strText.GetString() );

	DxLandMan* pLandMan = pView->GetLandMan();

	// 미리 로딩
	D3DXVECTOR3 vMax = D3DXVECTOR3(100000,100000,100000);
	D3DXVECTOR3 vMin = D3DXVECTOR3(-100000,-100000,-100000);
	pLandMan->LoadBasicPos( pView->GetD3DDevice(), vMax, vMin );

	CExportMiniDlg	dlg;
	dlg.m_SParam.m_nSumulationDetailLength = nSumulationDetailLength;
	dlg.m_SParam.m_nOctreeBoxLength = nOctreeBoxLength;
	dlg.m_SParam.m_bFieldOcclusion = bFieldOcclusion;
	dlg.m_SParam.m_bPieceOcclusion = bPieceOcclusion;
	dlg.m_SParam.m_bCharOcclusion = bCharOcclusion;
	//dlg.m_SParam.m_nOctreeBoxLength_CHAR = nOctreeBoxLength_CHAR;
	dlg.DoModal();

	//if ( !NSOcclusionCulling::Bake( pView->GetD3DDevice(), pLandMan, nSumulationDetailLength, nOctreeBoxLength ) )
	//{
	//	return;
	//}

	//CTime Time = CTime::GetCurrentTime();
	//CString strTime;
	//strTime.Format ( "%d%02d%02d_%02d_%02d_%02d.wld", Time.GetYear(), Time.GetMonth(), Time.GetDay(),
	//	Time.GetHour(), Time.GetMinute(), Time.GetSecond() );
	//CString strBackUp(pLandMan->GetNameWLD().c_str());
	//strBackUp += _T("_");
	//strBackUp += strTime.GetString();

	//// 
	//NSOcclusionCulling::Save( strBackUp.GetString() );			// BackUp

	//if ( pLandMan->GetNameWLD().empty() )
	//{
	//	AfxMessageBox("wld 이름이 없습니다.");
	//}
	//else
	//{
	//	NSOcclusionCulling::Save( pLandMan->GetNameWLD().c_str() ); // 지형
	//}
}

void COcclusionPage::OnBnClickedButtonBakeThisPlace()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();

	DxLandMan* pLandMan = pView->GetLandMan();

	CWnd* pWnd;
	CString strText;

	pWnd = GetDlgItem( IDC_EDIT_SIMULATION_DETAIL_LENGTH );
	pWnd->GetWindowText( strText );
	int nSumulationDetailLength = atoi ( strText.GetString() );

	// 미리 로딩
	D3DXVECTOR3 vMax = D3DXVECTOR3(100000,100000,100000);
	D3DXVECTOR3 vMin = D3DXVECTOR3(-100000,-100000,-100000);
	pLandMan->LoadBasicPos( pView->GetD3DDevice(), vMax, vMin );

	if ( NSOcclusionCulling::BakeOneCell( pView->GetD3DDevice(), 
											pView->GetLandMan(),
											DxViewPort::GetInstance().GetLookatPt(),
											nSumulationDetailLength ) )
	{
		CTime Time = CTime::GetCurrentTime();
		CString strTime;
		strTime.Format ( "%d_%02d_%02d_%02d_%02d_%02d.wld", Time.GetYear(), Time.GetMonth(), Time.GetDay(),
			Time.GetHour(), Time.GetMinute(), Time.GetSecond() );
		CString strBackUp(pLandMan->GetNameWLD().c_str());
		strBackUp += _T("_");
		strBackUp += strTime.GetString();

		// 
		NSOcclusionCulling::Save( pLandMan->GetMakeTimeWLD(), strBackUp.GetString() );			// BackUp

		if ( pLandMan->GetNameWLD().empty() )
		{
			AfxMessageBox("wld 이름이 없습니다.");
		}
		else
		{
			NSOcclusionCulling::Save( pLandMan->GetMakeTimeWLD(), pLandMan->GetNameWLD().c_str() ); // 지형
		}
	}
}

void COcclusionPage::OnBnClickedButtonOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pSheetTab->SetActivePage( FRAMEPAGEINDEX );

	CWorldEditView::m_ViewType=OCTREE_VIEW;
}

void COcclusionPage::OnBnClickedButtonViewOctreeBox()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CWnd* pWnd;
	CString strText;
	pWnd = GetDlgItem( IDC_EDIT_OCTREEBOX_LENGTH );
	pWnd->GetWindowText( strText );
	int nOctreeBoxLength = atoi ( strText.GetString() );
	
	NSOcclusionCulling::SetOctreeBoxSize( nOctreeBoxLength );
	
	CWorldEditView::g_bOcclsionOctreeRender = TRUE;
}

void COcclusionPage::OnBnClickedCheckField()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void COcclusionPage::OnBnClickedCheckPiece()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void COcclusionPage::OnBnClickedCheckChar()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}
