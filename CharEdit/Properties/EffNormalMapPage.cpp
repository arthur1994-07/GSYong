// EffNormalMap.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "../CharEdit.h"

#include "../MainFrm.h"
#include "../CharEditDoc.h"
#include "../CharEditView.h"

#include "SheetWithTabChar.h"
#include "../Dialog/ColorPickerDlg.h"

#include "../MfcExLib/ExLibs/EtcFunction.h"

#include "EffNormalMapPage.h"
#include ".\effnormalmapPage.h"


// CEffNormalMapPage 대화 상자입니다.

IMPLEMENT_DYNAMIC(CEffNormalMapPage, CPropertyPage)
CEffNormalMapPage::CEffNormalMapPage()
	: CEffCharPage(CEffNormalMapPage::IDD)
{
}

CEffNormalMapPage::~CEffNormalMapPage()
{
}

void CEffNormalMapPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CEffNormalMapPage, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnBnClickedButtonApply)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_TEXTURE_NORMAL, OnBnClickedButtonTextureNormal)
	ON_BN_CLICKED(IDC_BUTTON_COLOR, OnBnClickedButtonColor)
	ON_BN_CLICKED(IDC_BUTTON_TEXTURE_SPECULAR, OnBnClickedButtonTextureSpecular)
	ON_BN_CLICKED(IDC_CHECK_SPECULAR, OnBnClickedCheckSpecular)
	ON_BN_CLICKED(IDC_BUTTON_COLOR3, OnBnClickedButtonColor3)
	ON_BN_CLICKED(IDC_BUTTON_COLOR2, OnBnClickedButtonColor2)
	ON_BN_CLICKED(IDC_BUTTON_TEXTURE_SPECULAR2, OnBnClickedButtonTextureSpecular2)
	ON_BN_CLICKED(IDC_CHECK_SPECULAR2, OnBnClickedCheckSpecular2)
	ON_BN_CLICKED(IDC_CHECK_GLOWTEX, OnBnClickedCheckGlowtex)
	ON_BN_CLICKED(IDC_BUTTON_TEXTURE_GLOWTEX, OnBnClickedButtonTextureGlowtex)
END_MESSAGE_MAP()


// CEffNormalMapPage 메시지 처리기입니다.

void CEffNormalMapPage::SetProperty( EFFCHAR_PROPERTY *pProp )
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxSkinPiece *pSkinPiece = pView->GetSkinPiece ();

	if( pProp )	m_Property = *((EFFCHAR_PROPERTY_NORMAL*)pProp);
// 	else
// 	{
// 		EFFCHAR_PROPERTY_NORMAL sProp;
// 		m_Property = sProp;
// 	}

	if ( !m_pEffChar )
	{
		m_pEffChar = /*(DxEffCharNormal*)*/DxEffCharMan::GetInstance().CreateEffect
			(
			DxEffCharNormal::TYPEID,
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
		m_Property = *((EFFCHAR_PROPERTY_NORMAL*)m_pEffChar->GetProperty ());
	}
	else
	{
		m_pEffChar->SetProperty ( &m_Property );
		m_pEffChar->Create ( pView->GetD3dDevice() );
	}

	SetWin_Text( this, IDC_EDIT_TEXTURE_NORMAL, m_Property.m_szNormalMap );
	SetWin_Check( this, IDC_CHECK_SPECULAR, m_Property.m_bUseSpecularTex );
	SetWin_Check( this, IDC_CHECK_SPECULAR2, m_Property.m_bUseGlossinessTex );
	SetWin_Check( this, IDC_CHECK_GLOWTEX, m_Property.m_bUseGlowTex );

	SetWin_Num_float( this, IDC_EDIT_SPECULARPOWER, m_Property.m_fSpecularPower );	

	if( m_Property.m_bUseSpecularTex )
	{
		SetWin_Enable( this, IDC_BUTTON_TEXTURE_SPECULAR, TRUE );
		SetWin_Text( this, IDC_EDIT_TEXTURE_SPECULAR, m_Property.m_szSpecularTex );
	}else{
		SetWin_Enable( this, IDC_BUTTON_TEXTURE_SPECULAR, FALSE );
		SetWin_Text( this, IDC_EDIT_TEXTURE_SPECULAR, "" );
	}

	if( m_Property.m_bUseGlossinessTex )
	{
		SetWin_Enable( this, IDC_BUTTON_TEXTURE_SPECULAR2, TRUE );
		SetWin_Text( this, IDC_EDIT_TEXTURE_SPECULAR2, m_Property.m_szGlossinessTex );
	}else{
		SetWin_Enable( this, IDC_BUTTON_TEXTURE_SPECULAR2, FALSE );
		SetWin_Text( this, IDC_EDIT_TEXTURE_SPECULAR2, "" );
	}

	SetWin_Num_float( this, IDC_EDIT_GLOWBRIGHT, m_Property.m_fGlowBright );
	SetWin_Num_float( this, IDC_EDIT_GLOWALPHA, m_Property.m_fGlowAlpha );
	SetWin_Num_float( this, IDC_EDIT_GLOWBRIGHT, m_Property.m_fGlowBright );
	SetWin_Num_float( this, IDC_EDIT_GLOWSPEED, m_Property.m_fGlowAniSpeed );

	if( m_Property.m_bUseGlowTex )
	{
		SetWin_Enable( this, IDC_BUTTON_TEXTURE_GLOWTEX, TRUE );
		SetWin_Enable( this, IDC_EDIT_GLOWBRIGHT, TRUE );
		SetWin_Enable( this, IDC_EDIT_GLOWALPHA, TRUE );
		SetWin_Enable( this, IDC_EDIT_GLOWBRIGHT, TRUE );
		SetWin_Enable( this, IDC_EDIT_GLOWSPEED, TRUE );
		SetWin_Text( this, IDC_EDIT_TEXTURE_GLOWTEX, m_Property.m_szGlowTex );
	}else{
		SetWin_Enable( this, IDC_BUTTON_TEXTURE_GLOWTEX, FALSE );
		SetWin_Enable( this, IDC_EDIT_GLOWBRIGHT, FALSE );
		SetWin_Enable( this, IDC_EDIT_GLOWALPHA, FALSE );
		SetWin_Enable( this, IDC_EDIT_GLOWBRIGHT, FALSE );
		SetWin_Enable( this, IDC_EDIT_GLOWSPEED, FALSE );
		SetWin_Text( this, IDC_EDIT_TEXTURE_GLOWTEX, "" );
	}

	

}

BOOL CEffNormalMapPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}



void CEffNormalMapPage::OnBnClickedButtonApply()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();

	StringCchCopy( m_Property.m_szNormalMap, MAX_PATH, GetWin_Text( this, IDC_EDIT_TEXTURE_NORMAL ).GetString() );
	StringCchCopy( m_Property.m_szSpecularTex,	MAX_PATH, GetWin_Text( this, IDC_EDIT_TEXTURE_SPECULAR ).GetString() );
	StringCchCopy( m_Property.m_szGlossinessTex,	MAX_PATH, GetWin_Text( this, IDC_EDIT_TEXTURE_SPECULAR2 ).GetString() );
	StringCchCopy( m_Property.m_szGlowTex,	MAX_PATH, GetWin_Text( this, IDC_EDIT_TEXTURE_GLOWTEX ).GetString() );

	m_Property.m_fSpecularPower			= GetWin_Num_float( this, IDC_EDIT_SPECULARPOWER );

	m_Property.m_fGlowBright = GetWin_Num_float( this, IDC_EDIT_GLOWBRIGHT );
	m_Property.m_fGlowAlpha  = min( GetWin_Num_float( this, IDC_EDIT_GLOWALPHA ), 1.0f );
	m_Property.m_fGlowBright = GetWin_Num_float( this, IDC_EDIT_GLOWBRIGHT );
	m_Property.m_fGlowAniSpeed = GetWin_Num_float( this, IDC_EDIT_GLOWSPEED );


    m_Property.m_bUseSpecularTex	= GetWin_Check( this, IDC_CHECK_SPECULAR ) ? true : false;
	m_Property.m_bUseGlossinessTex	= GetWin_Check( this, IDC_CHECK_SPECULAR2 ) ? true : false;
	m_Property.m_bUseGlowTex		= GetWin_Check( this, IDC_CHECK_GLOWTEX ) ? true : false;

	m_pEffChar->SetProperty ( &m_Property );
	m_pEffChar->Create ( pView->GetD3dDevice() );

    EffectApply();
}

void CEffNormalMapPage::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	OnBnClickedButtonApply();
	OnBnClickedCancel();

}

void CEffNormalMapPage::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    ReturnPage();

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	pView->SetRenderTypeDefault();
}

void CEffNormalMapPage::OnBnClickedButtonTextureNormal()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString szFilter = _T("Texture Image (*.bmp,*.dds,*.tga,*.jpg)|*.bmp;*.dds;*.tga;*.jpg|");
	CFileDialog dlg ( TRUE, _T(".BMP"), NULL, 
		OFN_HIDEREADONLY, szFilter, this );

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;	
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	dlg.m_ofn.lpstrInitialDir = TextureManager::GetInstance().GetPath().c_str();

	if ( dlg.DoModal() == IDOK )
	{
		CString FilePath = dlg.GetFileName ();

		CWnd *pWnd;
		pWnd = GetDlgItem ( IDC_EDIT_TEXTURE_NORMAL );
		pWnd->SetWindowText ( FilePath );
	}
}


void CEffNormalMapPage::OnBnClickedButtonColor()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int ThisR = int ( m_Property.m_cAmbient.r * 255.0f );
	int ThisG = int ( m_Property.m_cAmbient.g * 255.0f );
	int ThisB = int ( m_Property.m_cAmbient.b * 255.0f );

	CColorPickerDlg ColorPicker ( RGB(ThisR,ThisG,ThisB) );

	if ( IDOK == ColorPicker.DoModal() )
	{
		COLORREF SelectColor = ColorPicker.GetColor ();
		m_Property.m_cAmbient.r = GetRValue(SelectColor)/255.0f;
		m_Property.m_cAmbient.g = GetGValue(SelectColor)/255.0f;
		m_Property.m_cAmbient.b = GetBValue(SelectColor)/255.0f;

		OnBnClickedButtonApply();
	}
}

void CEffNormalMapPage::OnBnClickedButtonColor2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int ThisR = int ( m_Property.m_cDiffuse.r * 255.0f );
	int ThisG = int ( m_Property.m_cDiffuse.g * 255.0f );
	int ThisB = int ( m_Property.m_cDiffuse.b * 255.0f );

	CColorPickerDlg ColorPicker ( RGB(ThisR,ThisG,ThisB) );

	if ( IDOK == ColorPicker.DoModal() )
	{
		COLORREF SelectColor = ColorPicker.GetColor ();
		m_Property.m_cDiffuse.r = GetRValue(SelectColor)/255.0f;
		m_Property.m_cDiffuse.g = GetGValue(SelectColor)/255.0f;
		m_Property.m_cDiffuse.b = GetBValue(SelectColor)/255.0f;

		OnBnClickedButtonApply();
	}
}



void CEffNormalMapPage::OnBnClickedButtonColor3()
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

		OnBnClickedButtonApply();
	}
}



void CEffNormalMapPage::OnBnClickedButtonTextureSpecular()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString szFilter = _T("Texture Image (*.bmp,*.dds,*.tga,*.jpg)|*.bmp;*.dds;*.tga;*.jpg|");
	CFileDialog dlg ( TRUE, _T(".BMP"), NULL, 
		OFN_HIDEREADONLY, szFilter, this );

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;	
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	dlg.m_ofn.lpstrInitialDir = TextureManager::GetInstance().GetPath().c_str();

	if ( dlg.DoModal() == IDOK )
	{
		CString FilePath = dlg.GetFileName ();

		CWnd *pWnd;
		pWnd = GetDlgItem ( IDC_EDIT_TEXTURE_SPECULAR );
		pWnd->SetWindowText ( FilePath );
	}
}


void CEffNormalMapPage::OnBnClickedButtonTextureSpecular2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString szFilter = _T("Texture Image (*.bmp,*.dds,*.tga,*.jpg)|*.bmp;*.dds;*.tga;*.jpg|");
	CFileDialog dlg ( TRUE, _T(".BMP"), NULL, 
		OFN_HIDEREADONLY, szFilter, this );

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;	
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	dlg.m_ofn.lpstrInitialDir = TextureManager::GetInstance().GetPath().c_str();

	if ( dlg.DoModal() == IDOK )
	{
		CString FilePath = dlg.GetFileName ();

		CWnd *pWnd;
		pWnd = GetDlgItem ( IDC_EDIT_TEXTURE_SPECULAR2 );
		pWnd->SetWindowText ( FilePath );
	}
}



void CEffNormalMapPage::OnBnClickedCheckSpecular()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_Property.m_bUseSpecularTex = GetWin_Check( this, IDC_CHECK_SPECULAR ) ? true : false;

	if( m_Property.m_bUseSpecularTex )
	{
		SetWin_Enable( this, IDC_BUTTON_TEXTURE_SPECULAR, TRUE );
	}else{
		ZeroMemory( m_Property.m_szSpecularTex, MAX_PATH );
		SetWin_Enable( this, IDC_BUTTON_TEXTURE_SPECULAR, FALSE );
	}
}


void CEffNormalMapPage::OnBnClickedCheckSpecular2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_Property.m_bUseGlossinessTex = GetWin_Check( this, IDC_CHECK_SPECULAR2 ) ? true : false;

	if( m_Property.m_bUseGlossinessTex )
	{
		SetWin_Enable( this, IDC_BUTTON_TEXTURE_SPECULAR2, TRUE );
	}else{
		ZeroMemory( m_Property.m_szGlossinessTex, MAX_PATH );
		SetWin_Enable( this, IDC_BUTTON_TEXTURE_SPECULAR2, FALSE );
	}
}

void CEffNormalMapPage::OnBnClickedCheckGlowtex()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_Property.m_bUseGlowTex = GetWin_Check( this, IDC_CHECK_GLOWTEX ) ? true : false;

	if( m_Property.m_bUseGlowTex )
	{
		SetWin_Enable( this, IDC_BUTTON_TEXTURE_GLOWTEX, TRUE );
		SetWin_Enable( this, IDC_EDIT_GLOWBRIGHT, TRUE );
		SetWin_Enable( this, IDC_EDIT_GLOWALPHA, TRUE );
		SetWin_Enable( this, IDC_EDIT_GLOWBRIGHT, TRUE );
		SetWin_Enable( this, IDC_EDIT_GLOWSPEED, TRUE );
	}else{
		ZeroMemory( m_Property.m_szGlowTex, MAX_PATH );
		SetWin_Enable( this, IDC_BUTTON_TEXTURE_GLOWTEX, FALSE );
		SetWin_Enable( this, IDC_EDIT_GLOWBRIGHT, FALSE );
		SetWin_Enable( this, IDC_EDIT_GLOWALPHA, FALSE );
		SetWin_Enable( this, IDC_EDIT_GLOWBRIGHT, FALSE );
		SetWin_Enable( this, IDC_EDIT_GLOWSPEED, FALSE );
	}
}

void CEffNormalMapPage::OnBnClickedButtonTextureGlowtex()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString szFilter = _T("Texture Image (*.bmp,*.dds,*.tga,*.jpg)|*.bmp;*.dds;*.tga;*.jpg|");
	CFileDialog dlg ( TRUE, _T(".BMP"), NULL, 
		OFN_HIDEREADONLY, szFilter, this );

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;	
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	dlg.m_ofn.lpstrInitialDir = TextureManager::GetInstance().GetPath().c_str();

	if ( dlg.DoModal() == IDOK )
	{
		CString FilePath = dlg.GetFileName ();

		CWnd *pWnd;
		pWnd = GetDlgItem ( IDC_EDIT_TEXTURE_GLOWTEX );
		pWnd->SetWindowText ( FilePath );
	}
}
