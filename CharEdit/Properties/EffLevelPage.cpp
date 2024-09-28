// EffSpecularPage2.cpp : ���� �����Դϴ�.
//
#include "stdafx.h"
#include "../CharEdit.h"

#include "../MainFrm.h"
#include "../CharEditDoc.h"
#include "../CharEditView.h"

#include "SheetWithTabChar.h"
#include "../Dialog/ColorPickerDlg.h"

#include "../SigmaCore/Util/Util.h"

#include "../EngineLib/DxTools/TextureManager.h"
#include "../EngineLib/DxEffect/Single/DxEffSinglePropGMan.h"

#include "EffLevelPage.h"

#pragma warning(disable:4996)

#include "../../SigmaCore/DebugInclude.h"

// CEffLevelPage ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CEffLevelPage, CPropertyPage)
CEffLevelPage::CEffLevelPage()
	: CEffCharPage(CEffLevelPage::IDD)
{
}

CEffLevelPage::~CEffLevelPage()
{
}

void CEffLevelPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_COLOROP, m_ComboColorOP);
	DDX_Control(pDX, IDC_COMBO_MATRIAL, m_ComboMatrials);
}


BEGIN_MESSAGE_MAP(CEffLevelPage, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnBnClickedButtonApply)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_COLOR, OnBnClickedButtonColor)
	ON_BN_CLICKED(IDC_BUTTON_TEXTURE, OnBnClickedButtonTexture)
	ON_BN_CLICKED(IDC_CHECK_SRCTEX, OnBnClickedCheckSrctex)
	ON_CBN_SELCHANGE(IDC_COMBO_MATRIAL, OnCbnSelchangeComboMatrial)
END_MESSAGE_MAP()


// CEffLevelPage �޽��� ó�����Դϴ�.
void CEffLevelPage::SetProperty ( EFFCHAR_PROPERTY *pProp )
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxSkinPiece *pSkinPiece = pView->GetSkinPiece ();

	if ( pProp )
		m_Property = *((EFFCHAR_PROPERTY_LEVEL*)pProp);

	CString	Str;

	CWnd*		pWnd;
	CButton*	pButton;
	int		nIndex;

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

	if ( !m_pEffChar )
	{
		m_pEffChar = DxEffCharMan::GetInstance().CreateEffect
		(
			DxEffCharLevel::TYPEID,
			&m_Property,
			NULL,
			pSkinPiece
		);

		if ( !m_pEffChar )
		{
			MessageBox ( "����Ʈ ������ �����Ͽ����ϴ�.", "ERROR", MB_OK );
			return;
		}

		pSkinPiece->AddEffList ( m_pEffChar );

		m_Property = *((EFFCHAR_PROPERTY_LEVEL*)m_pEffChar->GetProperty ());
	}

	DWORD	dwMaterials = m_Property.m_dwMaterials;

	DXMATERIAL_CHAR_EFF*	pMatrials = m_pEffChar->GetMaterials();

	m_ComboMatrials.ResetContent();
	for ( DWORD i=0; i<dwMaterials; ++i )
	{
		Str.Format ( "%s",pMatrials[i].szTexture );
		nIndex = m_ComboMatrials.AddString ( Str );
		m_ComboMatrials.SetItemData ( nIndex, i );
	}

	int nCurSel = 0;
	m_ComboMatrials.SetCurSel( nCurSel );			// �ʱⰪ.. !!

	pButton = (CButton*) GetDlgItem ( IDC_CHECK_SRCTEX );
	if ( pMatrials[nCurSel].IsType( EMCET_BASE ) )		pButton->SetCheck ( TRUE );
	else									pButton->SetCheck ( FALSE );

	char	szValue[256];
	pWnd = GetDlgItem ( IDC_EDIT_POWER );
	_gcvt ( m_Property.m_fPower, 7, szValue );
	pWnd->SetWindowText ( szValue );

	pWnd = GetDlgItem ( IDC_EDIT_TEXTURE );
	pWnd->SetWindowText ( pMatrials[nCurSel].szEffTex );

	//	EnableWindow
	CheckEnableWindow ( nCurSel );
}

void CEffLevelPage::OnBnClickedButtonApply()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxSkinPiece *pSkinPiece = pView->GetSkinPiece ();

	CString	strTexture;
	CWnd	*pWnd;
	const char*	szTexture;

	int nIndex, nData;

	nIndex = m_ComboColorOP.GetCurSel ();
	nData = m_ComboColorOP.GetItemData ( nIndex );
	m_Property.m_dwColorOP = nData;

	DXMATERIAL_CHAR_EFF*	pMatrials = m_pEffChar->GetMaterials();

	int	nCurSel = m_ComboMatrials.GetCurSel();

	pWnd = GetDlgItem ( IDC_EDIT_POWER );
	pWnd->GetWindowText ( strTexture );
	m_Property.m_fPower = (float) atof ( strTexture.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_TEXTURE );
	pWnd->GetWindowText ( strTexture );
	szTexture = strTexture.GetString();
	strcpy ( pMatrials[nCurSel].szEffTex, szTexture );

	m_pEffChar->SetProperty ( &m_Property );
	m_pEffChar->Create ( pView->GetD3dDevice() );

    EffectApply();
}

void CEffLevelPage::OnBnClickedOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	OnBnClickedButtonApply();

    ReturnPage();
}

void CEffLevelPage::OnBnClickedCancel()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
    ReturnPage();
}

void CEffLevelPage::OnBnClickedButtonColor()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	int ThisR = int ( m_Property.m_cSpecular.r * 255.0f );
	int ThisG = int ( m_Property.m_cSpecular.g * 255.0f );
	int ThisB = int ( m_Property.m_cSpecular.b * 255.0f );

	CColorPickerDlg ColorPicker ( RGB(ThisR,ThisG,ThisB) );
	
	if ( IDOK == ColorPicker.DoModal() )
	{
		COLORREF SelectColor = ColorPicker.GetColor ();
		m_Property.m_cSpecular.r = GetRValue(SelectColor)/255.0f;
		m_Property.m_cSpecular.g = GetGValue(SelectColor)/255.0f;
		m_Property.m_cSpecular.b = GetBValue(SelectColor)/255.0f;
	}
}

void CEffLevelPage::OnBnClickedButtonTexture()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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

void CEffLevelPage::OnBnClickedCheckSrctex()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	DXMATERIAL_CHAR_EFF*	pMatrials = m_pEffChar->GetMaterials();

	int nCurSel = m_ComboMatrials.GetCurSel();

	CButton*	pButton;
	pButton = (CButton*) GetDlgItem ( IDC_CHECK_SRCTEX );
	if ( pButton->GetCheck() )		pMatrials[nCurSel].SetType( EMCET_BASE );
	else							pMatrials[nCurSel].SetType( EMCET_NULL );

	//	EnableWindow
	CheckEnableWindow ( nCurSel );
}

void CEffLevelPage::OnCbnSelchangeComboMatrial()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CWnd*		pWnd;
	CButton*	pButton;

	int nCurSel = m_ComboMatrials.GetCurSel();

	DXMATERIAL_CHAR_EFF*	pMatrials = m_pEffChar->GetMaterials();

	pButton = (CButton*) GetDlgItem ( IDC_CHECK_SRCTEX );
	if ( pMatrials[nCurSel].IsType( EMCET_BASE ) )		pButton->SetCheck ( TRUE );
	else									pButton->SetCheck ( FALSE );

	char	szValue[256];
	pWnd = GetDlgItem ( IDC_EDIT_POWER );
	_gcvt ( m_Property.m_fPower, 7, szValue );
	pWnd->SetWindowText ( szValue );

	pWnd = GetDlgItem ( IDC_EDIT_TEXTURE );
	pWnd->SetWindowText ( pMatrials[nCurSel].szEffTex );

	//	EnableWindow
	CheckEnableWindow ( nCurSel );
}

void CEffLevelPage::CheckEnableWindow ( int nCurSel )
{
	//CWnd*		pWnd;

	//CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	//CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	//LPDIRECT3DDEVICEQ	pd3dDevice = pView->GetD3dDevice();

	//DXMATERIAL_CHAR_EFF*	pMatrials = m_pEffChar->GetMaterials();

	//if ( pMatrials[nCurSel].IsType( EMCET_BASE ) )
	//{
	//	pWnd = GetDlgItem ( IDC_BUTTON_TEXTURE );
	//	pWnd->EnableWindow ( TRUE );

	//	pWnd = GetDlgItem ( IDC_EDIT_POWER );
	//	pWnd->EnableWindow ( TRUE );

	//	if ( !strlen(pMatrials[nCurSel].szEffTex) )
	//	{
	//		std::string	strSrcTex = pMatrials[nCurSel].szTexture;

	//		std::string	strName;
	//		strName = sc::util::GetSpecularName ( strSrcTex, "_s" );

	//		if( !TextureManager::GetInstance().IsPathFile( strName ) )
 //               strcpy( pMatrials[nCurSel].szEffTex, "" );
	//	}
	//}
	//else
	//{
	//	pWnd = GetDlgItem ( IDC_BUTTON_TEXTURE );
	//	pWnd->EnableWindow ( FALSE );

	//	pWnd = GetDlgItem ( IDC_EDIT_POWER );
	//	pWnd->EnableWindow ( FALSE );
	//}
}
