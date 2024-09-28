// GlowEffEdit.cpp : 구현 파일입니다.
//
#include "stdafx.h"
#include "WorldEdit.h"
#include "MainFrm.h"
#include "WorldEditDoc.h"
#include "WorldEditView.h"

#include "ColorPickerDlg.h"
#include "SheetWithTab.h"

#include "../EngineLib/DxTools/TextureManager.h"
#include "../EngineLib/DxTools/DxMethods.h"
#include "../EngineLib/DxEffect/Frame/DxEffectGlow.h"

#include "GlowEffEdit.h"

// CGlowEffEdit 대화 상자입니다.

IMPLEMENT_DYNAMIC(CGlowEffEdit, CPropertyPage)
CGlowEffEdit::CGlowEffEdit()
	: CPropertyPage(CGlowEffEdit::IDD)
{
	m_pEff = NULL;
}

CGlowEffEdit::~CGlowEffEdit()
{
}

void CGlowEffEdit::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_MATRIAL, m_ComboMatrial);
}


BEGIN_MESSAGE_MAP(CGlowEffEdit, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnBnClickedButtonApply)
	ON_BN_CLICKED(IDC_BUTTON_OK, OnBnClickedButtonOk)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, OnBnClickedButtonCancel)
	ON_CBN_SELCHANGE(IDC_COMBO_MATRIAL, OnCbnSelchangeComboMatrial)
	ON_BN_CLICKED(IDC_BUTTON_TEXTURE, OnBnClickedButtonTexture)
	ON_BN_CLICKED(IDC_BUTTON_COLOR, OnBnClickedButtonColor)
	ON_BN_CLICKED(IDC_CHECK_SRCTEX, OnBnClickedCheckSrctex)
	ON_BN_CLICKED(IDC_CHECK_COLOR, OnBnClickedCheckColor)
	ON_BN_CLICKED(IDC_CHECK_DAY, OnBnClickedCheckDay)
	ON_BN_CLICKED(IDC_CHECK_NIGHT, OnBnClickedCheckNight)
END_MESSAGE_MAP()


// CGlowEffEdit 메시지 처리기입니다.
void CGlowEffEdit::SetInstance ( DxEffectGlow *pEff )
{
	CMainFrame *pFrame = (CMainFrame *)AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView *)pFrame->GetActiveView();
	pView->SetActiveEffect( pEff );

	m_pEff = pEff;
	m_OldProperty = m_Property = m_pEff->GetProperty();

	CWnd*		pWnd;
	CButton*	pButton;

	pButton = (CButton*) GetDlgItem ( IDC_CHECK_DAY );
	if ( m_Property.m_dwFlag & USEDAY )		pButton->SetCheck ( TRUE );
	else									pButton->SetCheck ( FALSE );

	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	pButton = (CButton*) GetDlgItem ( IDC_CHECK_NIGHT );
	if ( m_Property.m_dwFlag & USENIGHT )	pButton->SetCheck ( TRUE );
	else									pButton->SetCheck ( FALSE );

	int		nIndex;
	CString Str, strText;
	DWORD	dwMaterials = m_Property.m_dwMaterials;

	DXUSERMATERIAL*	pMatrials = m_pEff->GetMaterials ();

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
	if ( pMatrials[nCurSel].bGlowUse )		pButton->SetCheck ( TRUE );
	else									pButton->SetCheck ( FALSE );

	pButton = (CButton*) GetDlgItem ( IDC_CHECK_COLOR );
	if ( pMatrials[nCurSel].bColorUse )		pButton->SetCheck ( TRUE );
	else									pButton->SetCheck ( FALSE );	

	pWnd = GetDlgItem ( IDC_EDIT_TEXTURE );
	pWnd->SetWindowText ( pMatrials[nCurSel].szGlowTex );

	//	EnableWindow
	CheckEnableWindow ( nCurSel );


	//	Note : 값 설정.
	//
	pWnd = GetDlgItem ( IDC_EDIT_FRAME_EFF_NAME );
	pWnd->SetWindowText ( m_pEff->GetName() );
}

void CGlowEffEdit::OnBnClickedButtonApply()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();

	CWnd*	pWnd;
	DXUSERMATERIAL*	pMatrials = m_pEff->GetMaterials ();

	int	nCurSel = m_ComboMatrial.GetCurSel();

	CString strTexture;
	pWnd = GetDlgItem ( IDC_EDIT_TEXTURE );
	pWnd->GetWindowText ( strTexture );
	strcpy ( pMatrials[nCurSel].szGlowTex, strTexture.GetString() );

	m_pEff->ResetTexture ( pView->GetD3DDevice() );		//		텍스쳐를 다시 설정한다.

	//	Note : 속성을 적용.
	//
	m_pEff->SetProperty ( m_Property );
	m_pEff->CleanUp ();

	m_pEff->Create ( pView->GetD3DDevice() );
}

void CGlowEffEdit::OnBnClickedButtonOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pSheetTab->SetActivePage ( 0 );

	//	Note : 속성을 적용.
	//
	OnBnClickedButtonApply();
}

void CGlowEffEdit::OnBnClickedButtonCancel()
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
}

void CGlowEffEdit::OnCbnSelchangeComboMatrial()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CWnd*		pWnd;
	CButton*	pButton;

	int nCurSel = m_ComboMatrial.GetCurSel();

	DXUSERMATERIAL*	pMatrials = m_pEff->GetMaterials ();

	pButton = (CButton*) GetDlgItem ( IDC_CHECK_SRCTEX );
	if ( pMatrials[nCurSel].bGlowUse )		pButton->SetCheck ( TRUE );
	else									pButton->SetCheck ( FALSE );

	pButton = (CButton*) GetDlgItem ( IDC_CHECK_COLOR );
	if ( pMatrials[nCurSel].bColorUse )		pButton->SetCheck ( TRUE );
	else									pButton->SetCheck ( FALSE );	

	pWnd = GetDlgItem ( IDC_EDIT_TEXTURE );
	pWnd->SetWindowText ( pMatrials[nCurSel].szGlowTex );

	//	EnableWindow
	CheckEnableWindow ( nCurSel );
}

void CGlowEffEdit::OnBnClickedCheckSrctex()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	DXUSERMATERIAL*	pMatrials = m_pEff->GetMaterials ();

	int nCurSel = m_ComboMatrial.GetCurSel();

	CButton*	pButton;
	pButton = (CButton*) GetDlgItem ( IDC_CHECK_SRCTEX );
	if ( pButton->GetCheck() )		pMatrials[nCurSel].bGlowUse = TRUE;
	else							pMatrials[nCurSel].bGlowUse = FALSE;

	//	EnableWindow
	CheckEnableWindow ( nCurSel );
}

void CGlowEffEdit::OnBnClickedCheckColor()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	DXUSERMATERIAL*	pMatrials = m_pEff->GetMaterials ();

	int nCurSel = m_ComboMatrial.GetCurSel();

	CButton*	pButton;
	pButton = (CButton*) GetDlgItem ( IDC_CHECK_COLOR );
	if ( pButton->GetCheck() )		pMatrials[nCurSel].bColorUse = TRUE;
	else							pMatrials[nCurSel].bColorUse = FALSE;

	//	EnableWindow
	CheckEnableWindow ( nCurSel );
}

void CGlowEffEdit::OnBnClickedButtonTexture()
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

void CGlowEffEdit::OnBnClickedButtonColor()
{
	DXUSERMATERIAL*	pMatrials = m_pEff->GetMaterials ();
	int nCurSel = m_ComboMatrial.GetCurSel();

	int ThisR = (pMatrials[nCurSel].cColor&0xff0000)>>16;
	int ThisG = (pMatrials[nCurSel].cColor&0xff00)>>8;
	int ThisB = (pMatrials[nCurSel].cColor&0xff);

	CColorPickerDlg ColorPicker ( RGB(ThisR,ThisG,ThisB) );
	
	if ( IDOK == ColorPicker.DoModal() )
	{
		COLORREF SelectColor = ColorPicker.GetColor ();
		pMatrials[nCurSel].cColor = 0xff000000;
		pMatrials[nCurSel].cColor += GetRValue(SelectColor)<<16;
		pMatrials[nCurSel].cColor += GetGValue(SelectColor)<<8;
		pMatrials[nCurSel].cColor += GetBValue(SelectColor);
	}
}

void CGlowEffEdit::CheckEnableWindow ( int nCurSel )
{
	CWnd*		pWnd;
	CButton*	pButton;

	DXUSERMATERIAL*	pMatrials = m_pEff->GetMaterials ();

	if ( pMatrials[nCurSel].bGlowUse )
	{
		pButton = (CButton*) GetDlgItem ( IDC_CHECK_COLOR );
		pButton->EnableWindow ( TRUE );
		pWnd = GetDlgItem ( IDC_BUTTON_TEXTURE );
		pWnd->EnableWindow ( TRUE );
	}
	else
	{
		pButton = (CButton*) GetDlgItem ( IDC_CHECK_COLOR );
		pButton->EnableWindow ( FALSE );
		pWnd = GetDlgItem ( IDC_BUTTON_TEXTURE );
		pWnd->EnableWindow ( FALSE );
	}

	if ( pMatrials[nCurSel].bColorUse )
	{
		pWnd = GetDlgItem ( IDC_BUTTON_COLOR );
		pWnd->EnableWindow ( TRUE );	
	}
	else
	{
		pWnd = GetDlgItem ( IDC_BUTTON_COLOR );
		pWnd->EnableWindow ( FALSE );
	}

}
void CGlowEffEdit::OnBnClickedCheckDay()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CButton*	pButton;
	pButton = (CButton*) GetDlgItem ( IDC_CHECK_DAY );
	if ( pButton->GetCheck() )		m_Property.m_dwFlag |= USEDAY;
	else							m_Property.m_dwFlag &= ~USEDAY;
}

void CGlowEffEdit::OnBnClickedCheckNight()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CButton*	pButton;
	pButton = (CButton*) GetDlgItem ( IDC_CHECK_NIGHT );
	if ( pButton->GetCheck() )		m_Property.m_dwFlag |= USENIGHT;
	else							m_Property.m_dwFlag &= ~USENIGHT;
}
