// SpecReflectEffEdit.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "WorldEdit.h"
#include "MainFrm.h"
#include "WorldEditDoc.h"
#include "WorldEditView.h"

#include "SheetWithTab.h"

#include "../EngineLib/DxTools/TextureManager.h"
#include "../EngineLib/DxTools/DxMethods.h"
#include "../EngineLib/DxEffect/Frame/DxEffectSpecReflect.h"

#include "SpecReflectEffEdit.h"
#include ".\specreflecteffedit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// SpecReflectEffEdit 대화 상자입니다.

IMPLEMENT_DYNAMIC(CSpecReflectEffEdit, CPropertyPage)
CSpecReflectEffEdit::CSpecReflectEffEdit()
	: CPropertyPage(CSpecReflectEffEdit::IDD)
{
}

CSpecReflectEffEdit::~CSpecReflectEffEdit()
{
}

void CSpecReflectEffEdit::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_COLOROP, m_ComboColorOP);
	DDX_Control(pDX, IDC_COMBO_MATRIAL, m_ComboMatrial);
}


BEGIN_MESSAGE_MAP(CSpecReflectEffEdit, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_OK, OnBnClickedButtonOk)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnBnClickedButtonApply)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, OnBnClickedButtonCancel)
	ON_BN_CLICKED(IDC_BUTTON_TEXTURE, OnBnClickedButtonTexture)
	ON_BN_CLICKED(IDC_BUTTON_REFLECT_TEX, OnBnClickedButtonReflectTex)
	ON_BN_CLICKED(IDC_CHECK_SRCTEX, OnBnClickedCheckSrctex)
	ON_CBN_SELCHANGE(IDC_COMBO_MATRIAL, OnCbnSelchangeComboMatrial)
END_MESSAGE_MAP()


// SpecReflectEffEdit 메시지 처리기입니다.
void CSpecReflectEffEdit::SetInstance ( DxEffectSpecReflect *pEff )
{
	CMainFrame *pFrame = (CMainFrame *)AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView *)pFrame->GetActiveView();
	pView->SetActiveEffect( pEff );

	m_pEff = pEff;
	m_OldProperty = m_Property = m_pEff->GetProperty();

	CWnd*		pWnd;
	CButton*	pButton;

	GetDlgItem ( IDC_STATIC_SELECT )->ShowWindow ( FALSE );
	GetDlgItem ( IDC_RADIO_ALL )->ShowWindow ( FALSE );
	GetDlgItem ( IDC_RADIO_ALPHA )->ShowWindow ( FALSE );

	int		nIndex;
	CString	Str;

	m_ComboColorOP.ResetContent();

	Str.Format ( "MODULATE" );
	nIndex = m_ComboColorOP.AddString ( Str );
	m_ComboColorOP.SetItemData ( nIndex, 4 );
	Str.Format ( "MODULATE2X" );
	nIndex = m_ComboColorOP.AddString ( Str );
	m_ComboColorOP.SetItemData ( nIndex, 5 );
	Str.Format ( "MODULATE4X" );
	nIndex = m_ComboColorOP.AddString ( Str );
	m_ComboColorOP.SetItemData ( nIndex, 6 );

	m_ComboColorOP.SetCurSel ( m_Property.m_dwColorOP - 4 );

	pWnd = GetDlgItem ( IDC_EDIT_REFLECT_TEX );
	pWnd->SetWindowText ( m_Property.m_szReflectTex );


	DWORD	dwMaterials = m_Property.m_dwMaterials;

	DXMATERIAL_SPECREFLECT*	pMatrials = m_pEff->GetMaterials ();

	m_ComboMatrial.ResetContent();
	for ( DWORD i=0; i<dwMaterials; i++ )
	{
		Str.Format ( "%s",pMatrials[i].szTexture );
		nIndex = m_ComboMatrial.AddString ( Str );
		m_ComboMatrial.SetItemData ( nIndex, i );
	}

	int nCurSel = 0;
	m_ComboMatrial.SetCurSel( nCurSel );			// 초기값.. !!

	pButton = (CButton*) GetDlgItem ( IDC_CHECK_SRCTEX );
	if ( pMatrials[nCurSel].bSpecUse )		pButton->SetCheck ( TRUE );
	else									pButton->SetCheck ( FALSE );

	pWnd = GetDlgItem ( IDC_EDIT_TEXTURE );
	pWnd->SetWindowText ( pMatrials[nCurSel].szSpecTex );

	//	EnableWindow
	CheckEnableWindow ( nCurSel );

	//	Note : 값 설정.
	//
	pWnd = GetDlgItem ( IDC_EDIT_FRAME_EFF_NAME );
	pWnd->SetWindowText ( m_pEff->GetName() );
}

void CSpecReflectEffEdit::OnBnClickedButtonReflectTex()
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
		pWnd = GetDlgItem ( IDC_EDIT_REFLECT_TEX );
		pWnd->SetWindowText ( FilePath );
	}
}

void CSpecReflectEffEdit::OnBnClickedButtonTexture()
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

void CSpecReflectEffEdit::OnBnClickedButtonApply()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();

	CString strText;

	CWnd *pWnd;
	CString strTexture;

	int nIndex, nData;

	nIndex = m_ComboColorOP.GetCurSel ();
	nData = m_ComboColorOP.GetItemData ( nIndex );
	m_Property.m_dwColorOP = nData;


	pWnd = GetDlgItem ( IDC_EDIT_REFLECT_TEX );
	pWnd->GetWindowText ( strTexture );
	strcpy ( m_Property.m_szReflectTex, strTexture.GetString() );

	DXMATERIAL_SPECREFLECT*	pMatrials = m_pEff->GetMaterials ();
	int	nCurSel = m_ComboMatrial.GetCurSel();

	pWnd = GetDlgItem ( IDC_EDIT_TEXTURE );
	pWnd->GetWindowText ( strTexture );
	strcpy ( pMatrials[nCurSel].szSpecTex, strTexture.GetString() );

	m_pEff->ResetTexture ( pView->GetD3DDevice() );		//		텍스쳐를 다시 설정한다.


	//	Note : 속성을 적용.
	//
	m_pEff->SetProperty ( m_Property );
	m_pEff->CleanUp ();

	m_pEff->Create ( pView->GetD3DDevice() );
}

void CSpecReflectEffEdit::OnBnClickedButtonOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pSheetTab->SetActivePage ( 0 );

	//	Note : 속성을 적용.
	//
	OnBnClickedButtonApply();

	//	Note : 매트릭스 조정용
	//
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	pView->ResetActiveEffect();

}

void CSpecReflectEffEdit::OnBnClickedButtonCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pSheetTab->SetActivePage ( 0 );

	//	Note : 속성을 되돌림.
	//
	m_pEff->SetProperty ( m_OldProperty );
	m_pEff->CleanUp ();

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	m_pEff->Create ( pView->GetD3DDevice() );

	//	Note : CView 에서 Active Effect 선택 취소 - 매트릭스 조정용
	//
	pView->ResetActiveEffect();

}

void CSpecReflectEffEdit::OnBnClickedCheckSrctex()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	DXMATERIAL_SPECREFLECT*	pMatrials = m_pEff->GetMaterials ();

	int nCurSel = m_ComboMatrial.GetCurSel();

	CButton*	pButton;
	pButton = (CButton*) GetDlgItem ( IDC_CHECK_SRCTEX );
	if ( pButton->GetCheck() )		pMatrials[nCurSel].bSpecUse = TRUE;
	else							pMatrials[nCurSel].bSpecUse = FALSE;

	//	EnableWindow
	CheckEnableWindow ( nCurSel );
}

void CSpecReflectEffEdit::OnCbnSelchangeComboMatrial()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CWnd*		pWnd;
	CButton*	pButton;

	int nCurSel = m_ComboMatrial.GetCurSel();

	DXMATERIAL_SPECREFLECT*	pMatrials = m_pEff->GetMaterials ();

	pButton = (CButton*) GetDlgItem ( IDC_CHECK_SRCTEX );
	if ( pMatrials[nCurSel].bSpecUse )		pButton->SetCheck ( TRUE );
	else									pButton->SetCheck ( FALSE );

	pWnd = GetDlgItem ( IDC_EDIT_TEXTURE );
	pWnd->SetWindowText ( pMatrials[nCurSel].szSpecTex );

	//	EnableWindow
	CheckEnableWindow ( nCurSel );
}

void CSpecReflectEffEdit::CheckEnableWindow ( int nCurSel )
{
	CWnd*		pWnd;
	//CButton*	pButton;

	DXMATERIAL_SPECREFLECT*	pMatrials = m_pEff->GetMaterials ();

	if ( pMatrials[nCurSel].bSpecUse )
	{
		pWnd = GetDlgItem ( IDC_BUTTON_TEXTURE );
		pWnd->EnableWindow ( TRUE );
	}
	else
	{
		pWnd = GetDlgItem ( IDC_BUTTON_TEXTURE );
		pWnd->EnableWindow ( FALSE );
	}
}