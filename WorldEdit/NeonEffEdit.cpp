// NeonEffEdit.cpp : 구현 파일입니다.
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
#include "../EngineLib/DxEffect/Frame/DxEffectNeon.h"

#include "NeonEffEdit.h"
#include ".\neoneffedit.h"


// CNeonEffEdit 대화 상자입니다.

IMPLEMENT_DYNAMIC(CNeonEffEdit, CPropertyPage)
CNeonEffEdit::CNeonEffEdit()
	: CPropertyPage(CNeonEffEdit::IDD)
{
}

CNeonEffEdit::~CNeonEffEdit()
{
}

void CNeonEffEdit::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_MATRIAL, m_ComboMatrial);
}


BEGIN_MESSAGE_MAP(CNeonEffEdit, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnBnClickedButtonApply)
	ON_BN_CLICKED(IDC_BUTTON_OK, OnBnClickedButtonOk)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, OnBnClickedButtonCancel)
	ON_BN_CLICKED(IDC_BUTTON_TEXTURE, OnBnClickedButtonTexture)
	ON_BN_CLICKED(IDC_BUTTON_DIFFUSE, OnBnClickedButtonDiffuse)
	ON_BN_CLICKED(IDC_CHECK_SRCTEX, OnBnClickedCheckSrctex)
	ON_CBN_SELCHANGE(IDC_COMBO_MATRIAL, OnCbnSelchangeComboMatrial)
END_MESSAGE_MAP()


// CNeonEffEdit 메시지 처리기입니다.
void CNeonEffEdit::SetInstance ( DxEffectNeon *pEff )
{
	CMainFrame *pFrame = (CMainFrame *)AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView *)pFrame->GetActiveView();
	pView->SetActiveEffect( pEff );

	m_pEff = pEff;
	m_OldProperty = m_Property = m_pEff->GetProperty();

	CWnd*		pWnd;
	CButton*	pButton;

	int		nIndex;
	CString Str, strText;
	DWORD	dwMaterials = m_Property.m_dwMaterials;

	DXMATERIAL_NEON*	pMatrials = m_pEff->GetMaterials ();

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
	if ( pMatrials[nCurSel].bUse )		pButton->SetCheck ( TRUE );
	else								pButton->SetCheck ( FALSE );

	char		szValue[256];

	pWnd = GetDlgItem ( IDC_EDIT_SCALETEXU );
	_gcvt ( pMatrials[nCurSel].vScaleUV.x, 7, szValue );
	pWnd->SetWindowText ( szValue );

	pWnd = GetDlgItem ( IDC_EDIT_SCALETEXV );
	_gcvt ( pMatrials[nCurSel].vScaleUV.y, 7, szValue );
	pWnd->SetWindowText ( szValue );

	pWnd = GetDlgItem ( IDC_EDIT_MOVETEXU );
	_gcvt ( pMatrials[nCurSel].vMoveTex.x, 7, szValue );
	pWnd->SetWindowText ( szValue );

	pWnd = GetDlgItem ( IDC_EDIT_MOVETEXV );
	_gcvt ( pMatrials[nCurSel].vMoveTex.y, 7, szValue );
	pWnd->SetWindowText ( szValue );

	D3DCOLOR d3dColor = pMatrials[nCurSel].vColor;

	sprintf ( szValue, "%d", int ((d3dColor&0xff0000)>>16) );
	GetDlgItem(IDC_EDIT_DR)->SetWindowText(szValue);
	sprintf ( szValue, "%d", int ((d3dColor&0xff00)>>8) );
	GetDlgItem(IDC_EDIT_DG)->SetWindowText(szValue);
	sprintf ( szValue, "%d", int (d3dColor&0xff) );
	GetDlgItem(IDC_EDIT_DB)->SetWindowText(szValue);

	pWnd = GetDlgItem ( IDC_EDIT_TEXTURE );
	pWnd->SetWindowText ( pMatrials[nCurSel].szNeonTex );

	//	EnableWindow
	CheckEnableWindow ( nCurSel );

	//	Note : 값 설정.
	//
	pWnd = GetDlgItem ( IDC_EDIT_FRAME_EFF_NAME );
	pWnd->SetWindowText ( m_pEff->GetName() );
}

void CNeonEffEdit::OnBnClickedButtonApply()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();

	CWnd*	pWnd;
	CString	strText;

	DXMATERIAL_NEON*	pMatrials = m_pEff->GetMaterials ();

	int	nCurSel = m_ComboMatrial.GetCurSel();


	pWnd = GetDlgItem ( IDC_EDIT_SCALETEXU );
	pWnd->GetWindowText ( strText );
	pMatrials[nCurSel].vScaleUV.x = (float)atof ( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_SCALETEXV );
	pWnd->GetWindowText ( strText );
	pMatrials[nCurSel].vScaleUV.y = (float)atof ( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_MOVETEXU );
	pWnd->GetWindowText ( strText );
	pMatrials[nCurSel].vMoveTex.x = (float)atof ( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_MOVETEXV );
	pWnd->GetWindowText ( strText );
	pMatrials[nCurSel].vMoveTex.y = (float)atof ( strText.GetString() );

	CString strTexture;
	pWnd = GetDlgItem ( IDC_EDIT_TEXTURE );
	pWnd->GetWindowText ( strTexture );
	strcpy ( pMatrials[nCurSel].szNeonTex, strTexture.GetString() );

	m_pEff->ResetTexture ( pView->GetD3DDevice() );		//		텍스쳐를 다시 설정한다.

	//	Note : 속성을 적용.
	//
	m_pEff->SetProperty ( m_Property );
	m_pEff->CleanUp ();

	m_pEff->Create ( pView->GetD3DDevice() );
}

void CNeonEffEdit::OnBnClickedButtonOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	OnBnClickedButtonApply();

	m_pSheetTab->SetActivePage ( 0 );
	m_pEff = NULL;
}

void CNeonEffEdit::OnBnClickedButtonCancel()
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

	m_pSheetTab->SetActivePage ( 0 );
	m_pEff = NULL;
}

void CNeonEffEdit::OnBnClickedButtonTexture()
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



void CNeonEffEdit::OnBnClickedButtonDiffuse()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nCurSel = m_ComboMatrial.GetCurSel();

	DXMATERIAL_NEON*	pMatrials = m_pEff->GetMaterials ();

	D3DCOLOR &d3dColor = pMatrials[nCurSel].vColor;
	COLORREF retColor;
	COLORREF Color = RGB ( ((d3dColor&0xff0000)>>16), ((d3dColor&0xff00)>>8), (d3dColor&0xff) );
	CColorPickerDlg colorDlg ( Color );

	int	R,G,B;
	R = (d3dColor&0xff0000)>>16;
	G = (d3dColor&0xff00)>>8;
	B = d3dColor&0xff;

	if ( IDOK == colorDlg.DoModal () )
	{
		DWORD dwAlpha = (0x00000000);

		retColor = colorDlg.GetColor ();

		R = GetRValue ( retColor );
		G = GetGValue ( retColor );
		B = GetBValue ( retColor );
		pMatrials[nCurSel].vColor = D3DCOLOR_ARGB ( dwAlpha, R, G, B );
	}
	
	char szValue[128];
	sprintf ( szValue, "%d", R );
	GetDlgItem(IDC_EDIT_DR)->SetWindowText(szValue);
	sprintf ( szValue, "%d", G );
	GetDlgItem(IDC_EDIT_DG)->SetWindowText(szValue);
	sprintf ( szValue, "%d", B );
	GetDlgItem(IDC_EDIT_DB)->SetWindowText(szValue);


	m_pEff->SetProperty ( m_Property );
	
//	m_pEff->UpdateDiffuse ();		// 바꾸어 준다.
}

void CNeonEffEdit::OnBnClickedCheckSrctex()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	DXMATERIAL_NEON*	pMatrials = m_pEff->GetMaterials ();

	int nCurSel = m_ComboMatrial.GetCurSel();

	CButton*	pButton;
	pButton = (CButton*) GetDlgItem ( IDC_CHECK_SRCTEX );
	if ( pButton->GetCheck() )		pMatrials[nCurSel].bUse = TRUE;
	else							pMatrials[nCurSel].bUse = FALSE;

	//	EnableWindow
	CheckEnableWindow ( nCurSel );
}

void CNeonEffEdit::OnCbnSelchangeComboMatrial()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CWnd*		pWnd;
	CButton*	pButton;

	int nCurSel = m_ComboMatrial.GetCurSel();

	DXMATERIAL_NEON*	pMatrials = m_pEff->GetMaterials ();

	pButton = (CButton*) GetDlgItem ( IDC_CHECK_SRCTEX );
	if ( pMatrials[nCurSel].bUse )		pButton->SetCheck ( TRUE );
	else									pButton->SetCheck ( FALSE );

	char		szValue[256];

	pWnd = GetDlgItem ( IDC_EDIT_SCALETEXU );
	_gcvt ( pMatrials[nCurSel].vScaleUV.x, 7, szValue );
	pWnd->SetWindowText ( szValue );

	pWnd = GetDlgItem ( IDC_EDIT_SCALETEXV );
	_gcvt ( pMatrials[nCurSel].vScaleUV.y, 7, szValue );
	pWnd->SetWindowText ( szValue );

	pWnd = GetDlgItem ( IDC_EDIT_MOVETEXU );
	_gcvt ( pMatrials[nCurSel].vMoveTex.x, 7, szValue );
	pWnd->SetWindowText ( szValue );

	pWnd = GetDlgItem ( IDC_EDIT_MOVETEXV );
	_gcvt ( pMatrials[nCurSel].vMoveTex.y, 7, szValue );
	pWnd->SetWindowText ( szValue );

	D3DCOLOR d3dColor = pMatrials[nCurSel].vColor;

	sprintf ( szValue, "%d", int ((d3dColor&0xff0000)>>16) );
	GetDlgItem(IDC_EDIT_DR)->SetWindowText(szValue);
	sprintf ( szValue, "%d", int ((d3dColor&0xff00)>>8) );
	GetDlgItem(IDC_EDIT_DG)->SetWindowText(szValue);
	sprintf ( szValue, "%d", int (d3dColor&0xff) );

	GetDlgItem(IDC_EDIT_DB)->SetWindowText(szValue);
	pWnd = GetDlgItem ( IDC_EDIT_TEXTURE );
	pWnd->SetWindowText ( pMatrials[nCurSel].szNeonTex );

	//	EnableWindow
	CheckEnableWindow ( nCurSel );
}

void CNeonEffEdit::CheckEnableWindow ( int nCurSel )
{
	CWnd*		pWnd;

	DXMATERIAL_NEON*	pMatrials = m_pEff->GetMaterials ();

	if ( pMatrials[nCurSel].bUse )
	{
		pWnd = GetDlgItem ( IDC_BUTTON_TEXTURE );
		pWnd->EnableWindow ( TRUE );
		pWnd = GetDlgItem ( IDC_BUTTON_DIFFUSE );
		pWnd->EnableWindow ( TRUE );
		pWnd = GetDlgItem ( IDC_EDIT_SCALETEXU );
		pWnd->EnableWindow ( TRUE );
		pWnd = GetDlgItem ( IDC_EDIT_SCALETEXV );
		pWnd->EnableWindow ( TRUE );
		pWnd = GetDlgItem ( IDC_EDIT_MOVETEXU );
		pWnd->EnableWindow ( TRUE );
		pWnd = GetDlgItem ( IDC_EDIT_MOVETEXV );
		pWnd->EnableWindow ( TRUE );
	}
	else
	{
		pWnd = GetDlgItem ( IDC_BUTTON_TEXTURE );
		pWnd->EnableWindow ( FALSE );
		pWnd = GetDlgItem ( IDC_BUTTON_DIFFUSE );
		pWnd->EnableWindow ( FALSE );
		pWnd = GetDlgItem ( IDC_EDIT_SCALETEXU );
		pWnd->EnableWindow ( FALSE );
		pWnd = GetDlgItem ( IDC_EDIT_SCALETEXV );
		pWnd->EnableWindow ( FALSE );
		pWnd = GetDlgItem ( IDC_EDIT_MOVETEXU );
		pWnd->EnableWindow ( FALSE );
		pWnd = GetDlgItem ( IDC_EDIT_MOVETEXV );
		pWnd->EnableWindow ( FALSE );
	}
}