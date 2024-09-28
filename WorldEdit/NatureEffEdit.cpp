// NatureEffEdit.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "WorldEdit.h"
#include "MainFrm.h"
#include "WorldEditDoc.h"
#include "WorldEditView.h"

#include "SheetWithTab.h"

#include "../EngineLib/DxTools/TextureManager.h"
#include "../EngineLib/DxTools/DxMethods.h"
#include "../EngineLib/DxEffect/Frame/DxEffectNature.h"

#include "NatureEffEdit.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CNatureEffEdit 대화 상자입니다.

IMPLEMENT_DYNAMIC(CNatureEffEdit, CPropertyPage)
CNatureEffEdit::CNatureEffEdit()
	: CPropertyPage(CNatureEffEdit::IDD)
{
}

CNatureEffEdit::~CNatureEffEdit()
{
}

void CNatureEffEdit::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CNatureEffEdit, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_TEXTURE, OnBnClickedButtonTexture)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnBnClickedButtonApply)
	ON_BN_CLICKED(IDC_BUTTON_OK, OnBnClickedButtonOk)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, OnBnClickedButtonCancel)
END_MESSAGE_MAP()


// CNatureEffEdit 메시지 처리기입니다.
void CNatureEffEdit::SetInstance ( DxEffectNature *pNatureEff )
{
	m_pNatureEff = pNatureEff;
	m_OldNatureProperty = m_NatureProperty = m_pNatureEff->GetProperty();

	//	Note : 값 설정.
	//
	CWnd *pWnd = GetDlgItem ( IDC_EDIT_FRAME_EFF_NAME );
	pWnd->SetWindowText ( m_pNatureEff->GetName() );


	char szValue[256];
	pWnd = GetDlgItem ( IDC_EDIT_RATE );
	_gcvt ( m_NatureProperty.m_fRate, 7, szValue );
	pWnd->SetWindowText ( szValue );

	pWnd = GetDlgItem ( IDC_EDIT_BUTTER_RATE );
	_gcvt ( m_NatureProperty.m_fButterRate, 7, szValue );
	pWnd->SetWindowText ( szValue );

	pWnd = GetDlgItem ( IDC_EDIT_BUTTER_NUM );
	_gcvt ( m_pNatureEff->GetButterNum(), 7, szValue );
	pWnd->SetWindowText ( szValue );

	pWnd = GetDlgItem ( IDC_EDIT_BUTTER_WIDTH );
	_gcvt ( m_NatureProperty.m_fButterWidth, 7, szValue );
	pWnd->SetWindowText ( szValue );

	pWnd = GetDlgItem ( IDC_EDIT_BUTTER_HEIGHT );
	_gcvt ( m_NatureProperty.m_fButterHeight, 7, szValue );
	pWnd->SetWindowText ( szValue );

	pWnd = GetDlgItem ( IDC_EDIT_BUTTER_SPEED );
	_gcvt ( m_NatureProperty.m_fButterSpeed, 7, szValue );
	pWnd->SetWindowText ( szValue );

	pWnd = GetDlgItem ( IDC_EDIT_BUTTER_RANGE );
	_gcvt ( m_NatureProperty.m_fButterRange, 7, szValue );
	pWnd->SetWindowText ( szValue );

	pWnd = GetDlgItem ( IDC_EDIT_BUTTER_HEIGHT_UP );
	_gcvt ( m_NatureProperty.m_fButterHeightUP, 7, szValue );
	pWnd->SetWindowText ( szValue );

	pWnd = GetDlgItem ( IDC_EDIT_BUTTER_HEIGHT_DOWN );
	_gcvt ( m_NatureProperty.m_fButterHeightDOWN, 7, szValue );
	pWnd->SetWindowText ( szValue );



	pWnd = GetDlgItem ( IDC_EDIT_BUTTER_TEXTURE );
	pWnd->SetWindowText ( m_NatureProperty.m_szButterTex );

}
void CNatureEffEdit::OnBnClickedButtonTexture()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString szFilter = "Texture Image (*.bmp,*.dds,*.tga,*.jpg)|*.bmp;*.dds;*.tga;*.jpg|";
	CFileDialog dlg ( TRUE, ".BMP", NULL, 
		OFN_HIDEREADONLY, szFilter, this );

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
		dlg.m_ofn.lpstrInitialDir = TextureManager::GetInstance().GetPath().c_str();

	if ( dlg.DoModal() == IDOK )
	{
		CString FilePath = dlg.GetFileName ();

		CWnd *pWnd;
		pWnd = GetDlgItem ( IDC_EDIT_BUTTER_TEXTURE );
		pWnd->SetWindowText ( FilePath );
	}
}

void CNatureEffEdit::OnBnClickedButtonApply()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString strText;

	CWnd *pWnd;
	pWnd = GetDlgItem ( IDC_EDIT_RATE );
	pWnd->GetWindowText ( strText );
	m_NatureProperty.m_fRate = (float)atof ( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_BUTTER_RATE );
	pWnd->GetWindowText ( strText );
	m_NatureProperty.m_fButterRate = (float)atof ( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_BUTTER_WIDTH );
	pWnd->GetWindowText ( strText );
	m_NatureProperty.m_fButterWidth = (float)atof ( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_BUTTER_HEIGHT );
	pWnd->GetWindowText ( strText );
	m_NatureProperty.m_fButterHeight = (float)atof ( strText.GetString() );
	
	pWnd = GetDlgItem ( IDC_EDIT_BUTTER_SPEED );
	pWnd->GetWindowText ( strText );
	m_NatureProperty.m_fButterSpeed = (float)atof ( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_BUTTER_RANGE );
	pWnd->GetWindowText ( strText );
	m_NatureProperty.m_fButterRange = (float)atof ( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_BUTTER_HEIGHT_UP );
	pWnd->GetWindowText ( strText );
	m_NatureProperty.m_fButterHeightUP = (float)atof ( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_BUTTER_HEIGHT_DOWN );
	pWnd->GetWindowText ( strText );
	m_NatureProperty.m_fButterHeightDOWN = (float)atof ( strText.GetString() );


	CString strTexture;
	pWnd = GetDlgItem ( IDC_EDIT_BUTTER_TEXTURE );
	pWnd->GetWindowText ( strTexture );
	strcpy ( m_NatureProperty.m_szButterTex, strTexture.GetString() );


	//	Note : 속성을 적용.
	//
	m_pNatureEff->SetProperty ( m_NatureProperty );
	m_pNatureEff->CleanUp ();

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	m_pNatureEff->Create ( pView->GetD3DDevice() );

	m_pNatureEff->CreateBugMesh ( pView->GetD3DDevice() );
	m_pNatureEff->CreateButterMesh ( pView->GetD3DDevice() );

	char szValue[256];
	pWnd = GetDlgItem ( IDC_EDIT_BUTTER_NUM );
	_gcvt ( m_pNatureEff->GetButterNum(), 7, szValue );
	pWnd->SetWindowText ( szValue );
}

void CNatureEffEdit::OnBnClickedButtonOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pSheetTab->SetActivePage ( 0 );

	//	Note : 속성을 적용.
	//
	OnBnClickedButtonApply();
}

void CNatureEffEdit::OnBnClickedButtonCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pSheetTab->SetActivePage ( 0 );

	//	Note : 속성을 되돌림.
	//
	m_pNatureEff->SetProperty ( m_OldNatureProperty );
	m_pNatureEff->CleanUp ();

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	m_pNatureEff->Create ( pView->GetD3DDevice() );
}
