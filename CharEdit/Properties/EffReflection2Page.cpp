// EffReflection2Page.cpp : 구현 파일입니다.
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

#include "EffReflection2Page.h"
#include ".\effreflection2page.h"

#pragma warning(disable:4996)

#include "../../SigmaCore/DebugInclude.h"

// CEffReflection2Page 대화 상자입니다.
IMPLEMENT_DYNAMIC(CEffReflection2Page, CPropertyPage)
CEffReflection2Page::CEffReflection2Page()
	: CEffCharPage(CEffReflection2Page::IDD)
{
}

CEffReflection2Page::~CEffReflection2Page()
{
}

void CEffReflection2Page::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_COLOROP, m_ComboColorOP);
	DDX_Control(pDX, IDC_COMBO_MATRIAL, m_ComboMatrials);
}


BEGIN_MESSAGE_MAP(CEffReflection2Page, CPropertyPage)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnBnClickedButtonApply)
	ON_BN_CLICKED(IDC_BUTTON_REFLECT_TEX, OnBnClickedButtonReflectTex)
	ON_BN_CLICKED(IDC_BUTTON_COLOR, OnBnClickedButtonColor)
	ON_BN_CLICKED(IDC_CHECK_SRCTEX, OnBnClickedCheckSrctex)
	ON_BN_CLICKED(IDC_BUTTON_TEXTURE, OnBnClickedButtonTexture)
	ON_CBN_SELCHANGE(IDC_COMBO_MATRIAL, OnCbnSelchangeComboMatrial)
	ON_CBN_SETFOCUS(IDC_COMBO_MATRIAL, OnCbnSetfocusComboMatrial)
END_MESSAGE_MAP()


// CEffReflection2Page 메시지 처리기입니다.

BOOL CEffReflection2Page::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CEffReflection2Page::SetProperty ( EFFCHAR_PROPERTY *pProp )
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxSkinPiece *pSkinPiece = pView->GetSkinPiece ();

	if ( pProp )
		m_Property = *((EFFCHAR_PROPERTY_REFLECTION2*)pProp);

	CString	Str;

	CWnd*		pWnd;
	CButton*	pButton;
	int			nIndex;

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
	pWnd->SetWindowText ( m_Property.m_szTexture );

	if ( !m_pEffChar )
	{
		m_pEffChar = DxEffCharMan::GetInstance().CreateEffect
		(
			DxEffCharReflection2::TYPEID,
			&m_Property,
			NULL,
			pSkinPiece
		);

		if ( !m_pEffChar )
		{
			MessageBox ( "이팩트 생성에 실패하였습니다.", "ERROR", MB_OK );
			return;
		}

		pSkinPiece->AddEffList ( m_pEffChar );

		m_Property = *((EFFCHAR_PROPERTY_REFLECTION2*)m_pEffChar->GetProperty ());
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
	m_ComboMatrials.SetCurSel( nCurSel );			// 초기값.. !!

	pButton = (CButton*) GetDlgItem ( IDC_CHECK_SRCTEX );
	if ( pMatrials[nCurSel].IsType( EMCET_BASE ) )		pButton->SetCheck ( TRUE );
	else									pButton->SetCheck ( FALSE );

	char	szValue[256];
	pWnd = GetDlgItem ( IDC_EDIT_POWER );
	_gcvt ( pMatrials[nCurSel].d3dMaterial.Power, 7, szValue );
	pWnd->SetWindowText ( szValue );

	pWnd = GetDlgItem ( IDC_EDIT_TEXTURE );
	pWnd->SetWindowText ( pMatrials[nCurSel].szEffTex );

	//	EnableWindow
	CheckEnableWindow ( nCurSel );
}

void CEffReflection2Page::OnBnClickedButtonApply()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxSkinPiece *pSkinPiece = pView->GetSkinPiece ();

	CString	strTexture;
	CWnd	*pWnd;

	int nIndex, nData;

	nIndex = m_ComboColorOP.GetCurSel ();
	nData = m_ComboColorOP.GetItemData ( nIndex );
	m_Property.m_dwColorOP = nData;

	const char* szTexture;
	pWnd = GetDlgItem ( IDC_EDIT_REFLECT_TEX );
	pWnd->GetWindowText ( strTexture );
	szTexture = strTexture.GetString();
	strcpy ( m_Property.m_szTexture, szTexture );

	DXMATERIAL_CHAR_EFF*	pMatrials = m_pEffChar->GetMaterials();

	int	nCurSel = m_ComboMatrials.GetCurSel();

	pWnd = GetDlgItem ( IDC_EDIT_POWER );
	pWnd->GetWindowText ( strTexture );
	pMatrials[nCurSel].d3dMaterial.Power = (float) atof ( strTexture.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_TEXTURE );
	pWnd->GetWindowText ( strTexture );
	szTexture = strTexture.GetString();
	strcpy ( pMatrials[nCurSel].szEffTex, szTexture );

	m_pEffChar->SetProperty ( &m_Property );
	m_pEffChar->Create ( pView->GetD3dDevice() );

    EffectApply();
}

void CEffReflection2Page::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	OnBnClickedButtonApply();

    ReturnPage();
}

void CEffReflection2Page::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    ReturnPage();
}

void CEffReflection2Page::OnBnClickedButtonReflectTex()
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

void CEffReflection2Page::OnBnClickedButtonColor()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
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

void CEffReflection2Page::OnBnClickedCheckSrctex()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	DXMATERIAL_CHAR_EFF*	pMatrials = m_pEffChar->GetMaterials();

	int nCurSel = m_ComboMatrials.GetCurSel();

	CButton*	pButton;
	pButton = (CButton*) GetDlgItem ( IDC_CHECK_SRCTEX );
	if ( pButton->GetCheck() )		pMatrials[nCurSel].SetType( EMCET_BASE );
	else							pMatrials[nCurSel].SetType( EMCET_NULL );

	//	EnableWindow
	CheckEnableWindow ( nCurSel );
}

void CEffReflection2Page::OnBnClickedButtonTexture()
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

void CEffReflection2Page::OnCbnSelchangeComboMatrial()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CWnd*		pWnd;
	CButton*	pButton;

	int nCurSel = m_ComboMatrials.GetCurSel();

	DXMATERIAL_CHAR_EFF*	pMatrials = m_pEffChar->GetMaterials();

	pButton = (CButton*) GetDlgItem ( IDC_CHECK_SRCTEX );
	if ( pMatrials[nCurSel].IsType( EMCET_BASE ) )		pButton->SetCheck ( TRUE );
	else									pButton->SetCheck ( FALSE );

	char	szValue[256];
	pWnd = GetDlgItem ( IDC_EDIT_POWER );
	_gcvt ( pMatrials[nCurSel].d3dMaterial.Power, 7, szValue );
	pWnd->SetWindowText ( szValue );

	pWnd = GetDlgItem ( IDC_EDIT_TEXTURE );
	pWnd->SetWindowText ( pMatrials[nCurSel].szEffTex );

	//	EnableWindow
	CheckEnableWindow ( nCurSel );
}

void CEffReflection2Page::CheckEnableWindow ( int nCurSel )
{
	CWnd*		pWnd;

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	LPDIRECT3DDEVICEQ	pd3dDevice = pView->GetD3dDevice();

	DXMATERIAL_CHAR_EFF*	pMatrials = m_pEffChar->GetMaterials();

	if ( pMatrials[nCurSel].IsType( EMCET_BASE ) )
	{
		pWnd = GetDlgItem ( IDC_BUTTON_TEXTURE );
		pWnd->EnableWindow ( TRUE );

		pWnd = GetDlgItem ( IDC_EDIT_POWER );
		pWnd->EnableWindow ( TRUE );

		if ( !strlen(pMatrials[nCurSel].szEffTex) )
		{
			std::string	strSrcTex = pMatrials[nCurSel].szTexture;

			std::string	strName;
			strName = sc::util::GetSpecularName ( strSrcTex, "_s" );

			if( !TextureManager::GetInstance().IsPathFile( strName ) )
                strcpy( pMatrials[nCurSel].szEffTex, "" );
		}
	}
	else
	{
		pWnd = GetDlgItem ( IDC_BUTTON_TEXTURE );
		pWnd->EnableWindow ( FALSE );

		pWnd = GetDlgItem ( IDC_EDIT_POWER );
		pWnd->EnableWindow ( FALSE );
	}
}
void CEffReflection2Page::OnCbnSetfocusComboMatrial()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CWnd*		pWnd;

	int nCurSel = m_ComboMatrials.GetCurSel();

	DXMATERIAL_CHAR_EFF*	pMatrials = m_pEffChar->GetMaterials();

	CString	strTexture;
	pWnd = GetDlgItem ( IDC_EDIT_POWER );
	pWnd->GetWindowText ( strTexture );
	pMatrials[nCurSel].d3dMaterial.Power = (float) atof ( strTexture.GetString() );
}
