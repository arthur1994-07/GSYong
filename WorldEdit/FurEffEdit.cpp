// FurEffEdit.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "WorldEdit.h"
#include "MainFrm.h"
#include "WorldEditDoc.h"
#include "WorldEditView.h"

#include "SheetWithTab.h"

#include "../EngineLib/DxTools/TextureManager.h"
#include "../EngineLib/DxEffect/Frame/DxEffectFur.h"

#include "FurEffEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CFurEffEdit 대화 상자입니다.

IMPLEMENT_DYNAMIC(CFurEffEdit, CPropertyPage)
CFurEffEdit::CFurEffEdit()
	: CPropertyPage(CFurEffEdit::IDD)
{
}

CFurEffEdit::~CFurEffEdit()
{
}

void CFurEffEdit::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CFurEffEdit, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_TEXTURE, OnBnClickedButtonTexture)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnBnClickedButtonApply)
	ON_BN_CLICKED(IDC_BUTTON_OK, OnBnClickedButtonOk)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, OnBnClickedButtonCancel)
END_MESSAGE_MAP()


// CFurEffEdit 메시지 처리기입니다.
void CFurEffEdit::SetInstance ( DxEffectFur *pFurEff )
{
	m_pFurEff = pFurEff;
	m_OldFurProperty = m_FurProperty = m_pFurEff->GetProperty();

	//	Note : 값 설정.
	//
	CWnd *pWnd = GetDlgItem ( IDC_EDIT_FRAME_EFF_NAME );
	pWnd->SetWindowText ( m_pFurEff->GetName() );

	char szValue[256];
	pWnd = GetDlgItem ( IDC_EDIT_LAYER );
	_gcvt ( m_FurProperty.m_iLayers, 7, szValue );
	pWnd->SetWindowText ( szValue );

	pWnd = GetDlgItem ( IDC_EDIT_OFFSET );
	_gcvt ( m_FurProperty.m_fOffset, 7, szValue );
	pWnd->SetWindowText ( szValue );

	pWnd = GetDlgItem ( IDC_EDIT_SCALE_X );
	_gcvt ( m_FurProperty.m_fTexX, 7, szValue );
	pWnd->SetWindowText ( szValue );

	pWnd = GetDlgItem ( IDC_EDIT_SCALE_Y );
	_gcvt ( m_FurProperty.m_fTexY, 7, szValue );
	pWnd->SetWindowText ( szValue );

	pWnd = GetDlgItem ( IDC_EDIT_TEXTURE );
	pWnd->SetWindowText ( m_FurProperty.m_szTexture );
}

void CFurEffEdit::OnBnClickedButtonTexture()
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

//		char *szTexture = FilePath.GetString (256);
//		m_pAnisoEff->SetTexture ( szTexture );
	}
}

void CFurEffEdit::OnBnClickedButtonApply()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CWnd *pWnd;

	CString strText;
	pWnd = GetDlgItem ( IDC_EDIT_LAYER );
	pWnd->GetWindowText ( strText );
	m_FurProperty.m_iLayers = atoi ( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_OFFSET );
	pWnd->GetWindowText ( strText );
	m_FurProperty.m_fOffset = (float)atof ( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_SCALE_X );
	pWnd->GetWindowText ( strText );
	m_FurProperty.m_fTexX = (float)atof ( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_SCALE_Y );
	pWnd->GetWindowText ( strText );
	m_FurProperty.m_fTexY = (float)atof ( strText.GetString() );

	CString strTexture;
	pWnd = GetDlgItem ( IDC_EDIT_TEXTURE );
	pWnd->GetWindowText ( strTexture );
	strcpy ( m_FurProperty.m_szTexture, strTexture.GetString() );

	//	Note : 속성을 적용.
	//
	m_pFurEff->SetProperty ( m_FurProperty );
	m_pFurEff->CleanUp ();

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	m_pFurEff->Create ( pView->GetD3DDevice() );
}

void CFurEffEdit::OnBnClickedButtonOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pSheetTab->SetActivePage ( 0 );

	//	Note : 속성을 적용.
	//
	OnBnClickedButtonApply();
}

void CFurEffEdit::OnBnClickedButtonCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pSheetTab->SetActivePage ( 0 );

	//	Note : 물속성을 되돌림.
	//
	m_pFurEff->SetProperty ( m_OldFurProperty );
	m_pFurEff->CleanUp ();

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	m_pFurEff->Create ( pView->GetD3DDevice() );
}
