// Dot3EffEdit.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "WorldEdit.h"
#include "MainFrm.h"
#include "WorldEditDoc.h"
#include "WorldEditView.h"

#include "../EngineLib/DxTools/TextureManager.h"
#include "SheetWithTab.h"

#include "../EngineLib/DxEffect/Frame/DxEffectDot3.h"

#include "Dot3EffEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CDot3EffEdit 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDot3EffEdit, CPropertyPage)
CDot3EffEdit::CDot3EffEdit()
	: CPropertyPage(CDot3EffEdit::IDD)
{
}

CDot3EffEdit::~CDot3EffEdit()
{
}

void CDot3EffEdit::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDot3EffEdit, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_TEXTURE, OnBnClickedButtonTexture)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnBnClickedButtonApply)
	ON_BN_CLICKED(IDC_BUTTON_OK, OnBnClickedButtonOk)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, OnBnClickedButtonCancel)
	ON_BN_CLICKED(IDC_RADIO_DOT3, OnBnClickedRadioDot3)
	ON_BN_CLICKED(IDC_RADIO_DETAIL, OnBnClickedRadioDetail)
END_MESSAGE_MAP()


// CDot3EffEdit 메시지 처리기입니다.
void CDot3EffEdit::SetInstance ( DxEffectDot3 *pDot3Eff )
{
	m_pDot3Eff = pDot3Eff;
	m_OldDot3Property = m_Dot3Property = m_pDot3Eff->GetProperty();

	//	Note : 값 설정.
	//
	CWnd *pWnd = GetDlgItem ( IDC_EDIT_FRAME_EFF_NAME );
	pWnd->SetWindowText ( m_pDot3Eff->GetName() );


	if ( m_Dot3Property.m_bDot3 )
	{
		CButton *pButton;
		pButton = (CButton*) GetDlgItem ( IDC_RADIO_DOT3 );
		pButton->SetCheck ( 1 );

		pButton = (CButton*) GetDlgItem ( IDC_RADIO_DETAIL );
		pButton->SetCheck ( 0 );

		pWnd = GetDlgItem ( IDC_EDIT_POWER );
		pWnd->EnableWindow ( TRUE );
	}
	else
	{
		CButton *pButton;
		pButton = (CButton*) GetDlgItem ( IDC_RADIO_DOT3 );
		pButton->SetCheck ( 0 );

		pButton = (CButton*) GetDlgItem ( IDC_RADIO_DETAIL );
		pButton->SetCheck ( 1 );

		pWnd = GetDlgItem ( IDC_EDIT_POWER );
		pWnd->EnableWindow ( FALSE );
	}


	char szValue[256];
	pWnd = GetDlgItem ( IDC_EDIT_POWER );
	_gcvt ( m_Dot3Property.m_fPower, 7, szValue );
	pWnd->SetWindowText ( szValue );

	pWnd = GetDlgItem ( IDC_EDIT_SCALE_X );
	_gcvt ( m_Dot3Property.m_fTexX, 7, szValue );
	pWnd->SetWindowText ( szValue );

	pWnd = GetDlgItem ( IDC_EDIT_SCALE_Y );
	_gcvt ( m_Dot3Property.m_fTexY, 7, szValue );
	pWnd->SetWindowText ( szValue );

	pWnd = GetDlgItem ( IDC_EDIT_TEXTURE );
	pWnd->SetWindowText ( m_Dot3Property.m_szTexture );
}

void CDot3EffEdit::OnBnClickedButtonTexture()
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
		pWnd = GetDlgItem ( IDC_EDIT_TEXTURE );
		pWnd->SetWindowText ( FilePath );
	}
}

void CDot3EffEdit::OnBnClickedButtonApply()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CWnd *pWnd;
	CString strText;

	pWnd = GetDlgItem ( IDC_EDIT_POWER );
	pWnd->GetWindowText ( strText );
	m_Dot3Property.m_fPower = (float)atof ( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_SCALE_X );
	pWnd->GetWindowText ( strText );
	m_Dot3Property.m_fTexX = (float)atof ( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_SCALE_Y );
	pWnd->GetWindowText ( strText );
	m_Dot3Property.m_fTexY = (float)atof ( strText.GetString() );

	CString strTexture;
	pWnd = GetDlgItem ( IDC_EDIT_TEXTURE );
	pWnd->GetWindowText ( strTexture );
	strcpy ( m_Dot3Property.m_szTexture, strTexture.GetString() );

	//	Note : 물속성을 적용.
	//
	m_pDot3Eff->SetProperty ( m_Dot3Property );
	m_pDot3Eff->CleanUp ();

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	m_pDot3Eff->Create ( pView->GetD3DDevice() );
}

void CDot3EffEdit::OnBnClickedButtonOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pSheetTab->SetActivePage ( 0 );

	//	Note : 속성을 적용.
	//
	OnBnClickedButtonApply();

}

void CDot3EffEdit::OnBnClickedButtonCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pSheetTab->SetActivePage ( 0 );

	//	Note : 물속성을 되돌림.
	//
	m_pDot3Eff->SetProperty ( m_OldDot3Property );
	m_pDot3Eff->CleanUp ();

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	m_pDot3Eff->Create ( pView->GetD3DDevice() );
}

void CDot3EffEdit::OnBnClickedRadioDot3()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	// 옷을 움직이지 않는 상태로 되돌린다.
	m_Dot3Property.m_bDot3 = TRUE;
	m_pDot3Eff->SetProperty ( m_Dot3Property );

	CWnd *pWnd;
	pWnd = GetDlgItem ( IDC_EDIT_POWER );
	pWnd->EnableWindow ( TRUE );
}

void CDot3EffEdit::OnBnClickedRadioDetail()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	// 옷을 움직이지 않는 상태로 되돌린다.
	m_Dot3Property.m_bDot3 = FALSE;
	m_pDot3Eff->SetProperty ( m_Dot3Property );

	CWnd *pWnd;
	pWnd = GetDlgItem ( IDC_EDIT_POWER );
	pWnd->EnableWindow ( FALSE );
}
