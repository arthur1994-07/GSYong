// EffAmbientPage.cpp : 구현 파일입니다.
//
#include "stdafx.h"
#include "../CharEdit.h"

#include "../MainFrm.h"
#include "../CharEditDoc.h"
#include "../CharEditView.h"

#include "SheetWithTabChar.h"
#include "../Dialog/ColorPickerDlg.h"

#include "../EngineLib/DxEffect/Single/DxEffSinglePropGMan.h"

#include "EffUserColorPage.h"
#include ".\effusercolorpage.h"

#pragma warning(disable:4996)

#include "../../SigmaCore/DebugInclude.h"

// CEffUserColorPage 대화 상자입니다.

IMPLEMENT_DYNAMIC(CEffUserColorPage, CPropertyPage)
CEffUserColorPage::CEffUserColorPage()
	: CEffCharPage(CEffUserColorPage::IDD)
{
}

CEffUserColorPage::~CEffUserColorPage()
{
}

void CEffUserColorPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_MATRIAL, m_ComboMatrials);
}


BEGIN_MESSAGE_MAP(CEffUserColorPage, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnBnClickedButtonApply)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_COLOR, OnBnClickedButtonColor)
	ON_CBN_SELCHANGE(IDC_COMBO_MATRIAL, OnCbnSelchangeComboMatrial)
	ON_BN_CLICKED(IDC_BUTTON_TEXTURE, OnBnClickedButtonTexture)
	ON_BN_CLICKED(IDC_CHECK_SRCTEX, OnBnClickedCheckSrctex)
END_MESSAGE_MAP()


// CEffUserColorPage 메시지 처리기입니다.

void CEffUserColorPage::SetProperty ( EFFCHAR_PROPERTY *pProp )
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxSkinPiece *pSkinPiece = pView->GetSkinPiece ();

	if ( pProp )
		m_Property = *((EFFCHAR_PROPERTY_USERCOLOR*)pProp);

	CString	Str;

	CWnd*		pWnd;
	CButton*	pButton;

	if ( !m_pEffChar )
	{
		m_pEffChar = DxEffCharMan::GetInstance().CreateEffect
		(
			DxEffCharUserColor::TYPEID,
			&m_Property,
			NULL,
			pSkinPiece
		);

		if ( !m_pEffChar )
		{
			MessageBox ( _T("이팩트 생성에 실패하였습니다."), _T("ERROR"), MB_OK );
			return;
		}

		pSkinPiece->AddEffList ( m_pEffChar );

		m_Property = *((EFFCHAR_PROPERTY_USERCOLOR*)m_pEffChar->GetProperty ());
	}

	DWORD	dwMaterials = m_Property.m_dwMaterials;

	DXMATERIAL_CHAR_EFF*	pMatrials = m_pEffChar->GetMaterials();

	int nIndex = 0;
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
	if ( pMatrials[nCurSel].IsType(EMCET_BASE) )	pButton->SetCheck ( TRUE );
	else											pButton->SetCheck ( FALSE );

	pWnd = GetDlgItem ( IDC_EDIT_TEXTURE );
	pWnd->SetWindowText ( pMatrials[nCurSel].szEffTex );

	//	EnableWindow
	CheckEnableTexWindow ( nCurSel );
}

void CEffUserColorPage::OnBnClickedButtonApply()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxSkinPiece *pSkinPiece = pView->GetSkinPiece ();

	CString		strTexture;
	CWnd		*pWnd;
	const char*	szTexture;

	DXMATERIAL_CHAR_EFF*	pMatrials = m_pEffChar->GetMaterials();

	int	nCurSel = m_ComboMatrials.GetCurSel();

	pWnd = GetDlgItem ( IDC_EDIT_TEXTURE );
	pWnd->GetWindowText ( strTexture );
	szTexture = strTexture.GetString();
	strcpy ( pMatrials[nCurSel].szEffTex, szTexture );

	m_pEffChar->SetProperty ( &m_Property );
	m_pEffChar->Create ( pView->GetD3dDevice() );

    EffectApply();
}

void CEffUserColorPage::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	OnBnClickedButtonApply();

    ReturnPage();	
}

void CEffUserColorPage::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    ReturnPage();
}

void CEffUserColorPage::OnBnClickedButtonColor()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int ThisR = int ( m_Property.m_cColor.r * 255.0f );
	int ThisG = int ( m_Property.m_cColor.g * 255.0f );
	int ThisB = int ( m_Property.m_cColor.b * 255.0f );

	CColorPickerDlg ColorPicker ( RGB(ThisR,ThisG,ThisB) );
	
	if ( IDOK == ColorPicker.DoModal() )
	{
		COLORREF SelectColor = ColorPicker.GetColor ();
		m_Property.m_cColor.r = GetRValue(SelectColor)/255.0f;
		m_Property.m_cColor.g = GetGValue(SelectColor)/255.0f;
		m_Property.m_cColor.b = GetBValue(SelectColor)/255.0f;

		OnBnClickedButtonApply();
	}
}

void CEffUserColorPage::OnCbnSelchangeComboMatrial()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CWnd*		pWnd;
	CButton*	pButton;

	int nCurSel = m_ComboMatrials.GetCurSel();

	DXMATERIAL_CHAR_EFF*	pMatrials = m_pEffChar->GetMaterials();

	pButton = (CButton*) GetDlgItem ( IDC_CHECK_SRCTEX );
	if ( pMatrials[nCurSel].IsType( EMCET_BASE) )	pButton->SetCheck ( TRUE );
	else											pButton->SetCheck ( FALSE );

	pWnd = GetDlgItem ( IDC_EDIT_TEXTURE );
	pWnd->SetWindowText ( pMatrials[nCurSel].szEffTex );

	//	EnableWindow
	CheckEnableTexWindow ( nCurSel );
}

void CEffUserColorPage::OnBnClickedButtonTexture()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString szFilter = _T("Texture Image (*.bmp,*.dds,*.tga,*.jpg)|*.bmp;*.dds;*.tga;*.jpg|");
	CFileDialog dlg ( TRUE, _T(".BMP"), NULL, 
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

void CEffUserColorPage::OnBnClickedCheckSrctex()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	DXMATERIAL_CHAR_EFF*	pMatrials = m_pEffChar->GetMaterials();

	int nCurSel = m_ComboMatrials.GetCurSel();

	CButton*	pButton;
	pButton = (CButton*) GetDlgItem ( IDC_CHECK_SRCTEX );
	if ( pButton->GetCheck() )		pMatrials[nCurSel].SetType( EMCET_BASE );
	else							pMatrials[nCurSel].SetType( EMCET_NULL );

	//	EnableWindow
	CheckEnableTexWindow ( nCurSel );
}

void CEffUserColorPage::CheckEnableTexWindow ( int nCurSel )
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

		//if( !strlen(pMatrials[nCurSel].szEffTex) )
		//{
		//	std::string	strSrcTex = pMatrials[nCurSel].szTexture;

		//	std::string	strName;
		//	strName = sc::util::GetSpecularName ( strSrcTex, "_s" );

		//	if( !TextureManager::GetInstance().IsPathFile( strName ) )
        //      strcpy( pMatrials[nCurSel].szEffTex, "" );
		//}
	}
	else
	{
		pWnd = GetDlgItem ( IDC_BUTTON_TEXTURE );
		pWnd->EnableWindow ( FALSE );
	}
}

