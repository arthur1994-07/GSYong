// EffDot3Page.cpp : 구현 파일입니다.
//
#include "stdafx.h"
#include "../CharEdit.h"

#include "../MainFrm.h"
#include "../CharEditDoc.h"
#include "../CharEditView.h"

#include "SheetWithTabChar.h"
#include "../Dialog/ColorPickerDlg.h"

#include "../EngineLib/DxTools/TextureManager.h"
#include "../EngineLib/DxEffect/Single/DxEffSinglePropGMan.h"

#include "EffDot3Page.h"

#pragma warning(disable:4996)

#include "../../SigmaCore/DebugInclude.h"

// CEffDot3Page 대화 상자입니다.

IMPLEMENT_DYNAMIC(CEffDot3Page, CPropertyPage)
CEffDot3Page::CEffDot3Page()
	: CEffCharPage(CEffDot3Page::IDD)
{
}

CEffDot3Page::~CEffDot3Page()
{
}

void CEffDot3Page::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_OPTION_1, m_ComboOption1);
	DDX_Control(pDX, IDC_COMBO_OPTION_2, m_ComboOption2);
	DDX_Control(pDX, IDC_COMBO_OPTION_3, m_ComboOption3);
}


BEGIN_MESSAGE_MAP(CEffDot3Page, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnBnClickedButtonApply)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(IDC_CHECK_SRCTEX_1, OnBnClickedCheckSrctex1)
	ON_BN_CLICKED(IDC_CHECK_SRCTEX_2, OnBnClickedCheckSrctex2)
	ON_BN_CLICKED(IDC_CHECK_SRCTEX_3, OnBnClickedCheckSrctex3)
	ON_BN_CLICKED(IDC_BUTTON_TEXTURE_1, OnBnClickedButtonTexture1)
	ON_BN_CLICKED(IDC_BUTTON_TEXTURE_2, OnBnClickedButtonTexture2)
	ON_BN_CLICKED(IDC_BUTTON_TEXTURE_3, OnBnClickedButtonTexture3)
	ON_BN_CLICKED(IDC_BUTTON_TEXTURE_BUMP_1, OnBnClickedButtonTextureBump1)
	ON_BN_CLICKED(IDC_BUTTON_TEXTURE_BUMP_2, OnBnClickedButtonTextureBump2)
	ON_BN_CLICKED(IDC_BUTTON_TEXTURE_BUMP_3, OnBnClickedButtonTextureBump3)
	ON_BN_CLICKED(IDC_BUTTON_REFLECT_TEX, OnBnClickedButtonReflectTex)
END_MESSAGE_MAP()


// CEffDot3Page 메시지 처리기입니다.
void CEffDot3Page::SetProperty ( EFFCHAR_PROPERTY *pProp )
{
	if ( pProp )
		m_Property = *((EFFCHAR_PROPERTY_DOT3*)pProp);

	CString	Str;

	CWnd*		pWnd;

	pWnd = GetDlgItem ( IDC_EDIT_REFLECT_TEX );
	pWnd->SetWindowText ( m_Property.m_szTexture );

	pWnd = GetDlgItem ( IDC_EDIT_TEXTURE_1 );
	pWnd->SetWindowText ( m_Property.m_szTexture_1 );

	pWnd = GetDlgItem ( IDC_EDIT_TEXTURE_2 );
	pWnd->SetWindowText ( m_Property.m_szTexture_2 );

	pWnd = GetDlgItem ( IDC_EDIT_TEXTURE_3 );
	pWnd->SetWindowText ( m_Property.m_szTexture_3 );

	pWnd = GetDlgItem ( IDC_EDIT_TEXTURE_BUMP_1 );
	pWnd->SetWindowText ( m_Property.m_szDot3Tex_1 );

	pWnd = GetDlgItem ( IDC_EDIT_TEXTURE_BUMP_2 );
	pWnd->SetWindowText ( m_Property.m_szDot3Tex_2 );

	pWnd = GetDlgItem ( IDC_EDIT_TEXTURE_BUMP_3 );
	pWnd->SetWindowText ( m_Property.m_szDot3Tex_3 );

	int		nIndex;

	m_ComboOption1.ResetContent();
	Str.Format ( "없음" );
	nIndex = m_ComboOption1.AddString ( Str );
	m_ComboOption1.SetItemData ( nIndex, 0 );
	Str.Format ( "스펙큘러" );
	nIndex = m_ComboOption1.AddString ( Str );
	m_ComboOption1.SetItemData ( nIndex, 1 );
	Str.Format ( "반사" );
	nIndex = m_ComboOption1.AddString ( Str );
	m_ComboOption1.SetItemData ( nIndex, 2 );
	Str.Format ( "범프+스펙큘러" );
	nIndex = m_ComboOption1.AddString ( Str );
	m_ComboOption1.SetItemData ( nIndex, 3 );
	Str.Format ( "범프+반사" );
	nIndex = m_ComboOption1.AddString ( Str );
	m_ComboOption1.SetItemData ( nIndex, 4 );
	m_ComboOption1.SetCurSel ( m_Property.m_dwDetail_1 );

	m_ComboOption2.ResetContent();
	Str.Format ( "없음" );
	nIndex = m_ComboOption2.AddString ( Str );
	m_ComboOption2.SetItemData ( nIndex, 0 );
	Str.Format ( "스펙큘러" );
	nIndex = m_ComboOption2.AddString ( Str );
	m_ComboOption2.SetItemData ( nIndex, 1 );
	Str.Format ( "반사" );
	nIndex = m_ComboOption2.AddString ( Str );
	m_ComboOption2.SetItemData ( nIndex, 2 );
	Str.Format ( "범프+스펙큘러" );
	nIndex = m_ComboOption2.AddString ( Str );
	m_ComboOption2.SetItemData ( nIndex, 3 );
	Str.Format ( "범프+반사" );
	nIndex = m_ComboOption2.AddString ( Str );
	m_ComboOption2.SetItemData ( nIndex, 4 );
	m_ComboOption2.SetCurSel ( m_Property.m_dwDetail_2 );

	m_ComboOption3.ResetContent();
	Str.Format ( "없음" );
	nIndex = m_ComboOption3.AddString ( Str );
	m_ComboOption3.SetItemData ( nIndex, 0 );
	Str.Format ( "스펙큘러" );
	nIndex = m_ComboOption3.AddString ( Str );
	m_ComboOption3.SetItemData ( nIndex, 1 );
	Str.Format ( "반사" );
	nIndex = m_ComboOption3.AddString ( Str );
	m_ComboOption3.SetItemData ( nIndex, 2 );
	Str.Format ( "범프+스펙큘러" );
	nIndex = m_ComboOption3.AddString ( Str );
	m_ComboOption3.SetItemData ( nIndex, 3 );
	Str.Format ( "범프+반사" );
	nIndex = m_ComboOption3.AddString ( Str );
	m_ComboOption3.SetItemData ( nIndex, 4 );
	m_ComboOption3.SetCurSel ( m_Property.m_dwDetail_3 );



	//	Note : 필요한것은 보이고 불필요한 것은 보이지 않는다.
	//
	ResetCheck ();
}

void CEffDot3Page::OnBnClickedButtonApply()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxSkinPiece *pSkinPiece = pView->GetSkinPiece ();

	CString	strText;
	CWnd	*pWnd;

	const char* szTexture;
	pWnd = GetDlgItem ( IDC_EDIT_REFLECT_TEX );
	pWnd->GetWindowText ( strText );
	szTexture = strText.GetString();
	strcpy ( m_Property.m_szTexture, szTexture );

	pWnd = GetDlgItem ( IDC_EDIT_TEXTURE_1 );
	pWnd->GetWindowText ( strText );
	szTexture = strText.GetString();
	strcpy ( m_Property.m_szTexture_1, szTexture );

	pWnd = GetDlgItem ( IDC_EDIT_TEXTURE_2 );
	pWnd->GetWindowText ( strText );
	szTexture = strText.GetString();
	strcpy ( m_Property.m_szTexture_2, szTexture );

	pWnd = GetDlgItem ( IDC_EDIT_TEXTURE_3 );
	pWnd->GetWindowText ( strText );
	szTexture = strText.GetString();
	strcpy ( m_Property.m_szTexture_3, szTexture );


	pWnd = GetDlgItem ( IDC_EDIT_TEXTURE_BUMP_1 );
	pWnd->GetWindowText ( strText );
	szTexture = strText.GetString();
	strcpy ( m_Property.m_szDot3Tex_1, szTexture );

	pWnd = GetDlgItem ( IDC_EDIT_TEXTURE_BUMP_2 );
	pWnd->GetWindowText ( strText );
	szTexture = strText.GetString();
	strcpy ( m_Property.m_szDot3Tex_2, szTexture );

	pWnd = GetDlgItem ( IDC_EDIT_TEXTURE_BUMP_3 );
	pWnd->GetWindowText ( strText );
	szTexture = strText.GetString();
	strcpy ( m_Property.m_szDot3Tex_3, szTexture );

	int nIndex, nData;

	nIndex = m_ComboOption1.GetCurSel ();
	nData = m_ComboOption1.GetItemData ( nIndex );
	m_Property.m_dwDetail_1 = (CRDT_TYPE)nData;

	nIndex = m_ComboOption2.GetCurSel ();
	nData = m_ComboOption2.GetItemData ( nIndex );
	m_Property.m_dwDetail_2 = (CRDT_TYPE)nData;

	nIndex = m_ComboOption3.GetCurSel ();
	nData = m_ComboOption3.GetItemData ( nIndex );
	m_Property.m_dwDetail_3 = (CRDT_TYPE)nData;


	if ( !m_pEffChar )
	{
		m_pEffChar = DxEffCharMan::GetInstance().CreateEffect
		(
			DxEffCharDot3::TYPEID,
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

		//	Note : 그림.. 그림.... 음............... ................ 텍스쳐 알아오기 위해서
		//
		m_Property = *(EFFCHAR_PROPERTY_DOT3*)m_pEffChar->GetProperty ();
		ResetCheck();
	}
	else
	{
		m_pEffChar->SetProperty ( &m_Property );
		m_pEffChar->Create ( pView->GetD3dDevice() );

		//	Note : 그림.. 그림.... 음............... ................ 텍스쳐 알아오기 위해서
		//
		m_Property = *(EFFCHAR_PROPERTY_DOT3*)m_pEffChar->GetProperty ();
		ResetCheck();
	}

    EffectApply();
}

void CEffDot3Page::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	OnBnClickedButtonApply();

    ReturnPage();
}

void CEffDot3Page::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    ReturnPage();
}

void CEffDot3Page::OnBnClickedCheckSrctex1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CButton*	pButton;
	pButton = (CButton*) GetDlgItem ( IDC_CHECK_SRCTEX_1 );
	if ( pButton->GetCheck() )
	{
		m_Property.m_dwFlag |= USETEXTURE1;
		m_ComboOption1.EnableWindow ( TRUE );
	}
	else
	{
		m_Property.m_dwFlag &= ~USETEXTURE1;
		m_ComboOption1.EnableWindow ( FALSE );
	}
}

void CEffDot3Page::OnBnClickedCheckSrctex2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CButton*	pButton;
	pButton = (CButton*) GetDlgItem ( IDC_CHECK_SRCTEX_2 );
	if ( pButton->GetCheck() )
	{
		m_Property.m_dwFlag |= USETEXTURE2;
		m_ComboOption2.EnableWindow ( TRUE );
	}
	else
	{
		m_Property.m_dwFlag &= ~USETEXTURE2;
		m_ComboOption2.EnableWindow ( FALSE );
	}
}

void CEffDot3Page::OnBnClickedCheckSrctex3()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CButton*	pButton;
	pButton = (CButton*) GetDlgItem ( IDC_CHECK_SRCTEX_3 );
	if ( pButton->GetCheck() )
	{
		m_Property.m_dwFlag |= USETEXTURE3;
		m_ComboOption3.EnableWindow ( TRUE );
	}
	else
	{
		m_Property.m_dwFlag &= ~USETEXTURE3;
		m_ComboOption3.EnableWindow ( FALSE );
	}
}

void CEffDot3Page::ResetCheck ()
{
	CWnd*		pWnd;
	CButton*	pButton;


	if ( m_Property.m_dwFlag & USETEXTURE1 )	m_ComboOption1.EnableWindow ( TRUE );
	else										m_ComboOption1.EnableWindow ( FALSE );

	if ( m_Property.m_dwFlag & USETEXTURE2 )	m_ComboOption2.EnableWindow ( TRUE );
	else										m_ComboOption2.EnableWindow ( FALSE );

	if ( m_Property.m_dwFlag & USETEXTURE3 )	m_ComboOption3.EnableWindow ( TRUE );
	else										m_ComboOption3.EnableWindow ( FALSE );

	//	Note : 1
	//
	pButton = (CButton*) GetDlgItem ( IDC_CHECK_SRCTEX_1 );
	pWnd	= GetDlgItem ( IDC_EDIT_SRCTEX_1 );
	if ( m_Property.m_dwFlag & ISTEXTURE1 )
	{
		pButton->ShowWindow ( TRUE );
		pWnd->ShowWindow ( TRUE );
		pWnd->SetWindowText ( m_Property.m_szSrcTex_1 );

		m_ComboOption1.ShowWindow ( TRUE );

		pWnd	= GetDlgItem ( IDC_EDIT_TEXTURE_1 );
		pWnd->ShowWindow ( TRUE );
		pWnd	= GetDlgItem ( IDC_BUTTON_TEXTURE_1 );
		pWnd->ShowWindow ( TRUE );

		pWnd	= GetDlgItem ( IDC_EDIT_TEXTURE_BUMP_1 );
		pWnd->ShowWindow ( TRUE );
		pWnd	= GetDlgItem ( IDC_BUTTON_TEXTURE_BUMP_1 );
		pWnd->ShowWindow ( TRUE );
	}
	else
	{
		pButton->ShowWindow ( FALSE );
		pWnd->ShowWindow ( FALSE );

		m_ComboOption1.ShowWindow ( FALSE );

		pWnd	= GetDlgItem ( IDC_EDIT_TEXTURE_1 );
		pWnd->ShowWindow ( FALSE );
		pWnd	= GetDlgItem ( IDC_BUTTON_TEXTURE_1 );
		pWnd->ShowWindow ( FALSE );

		pWnd	= GetDlgItem ( IDC_EDIT_TEXTURE_BUMP_1 );
		pWnd->ShowWindow ( FALSE );
		pWnd	= GetDlgItem ( IDC_BUTTON_TEXTURE_BUMP_1 );
		pWnd->ShowWindow ( FALSE );
	}
	if ( m_Property.m_dwFlag & USETEXTURE1 )		pButton->SetCheck ( TRUE );
	else											pButton->SetCheck ( FALSE );

	//	Note : 2
	//
	pButton = (CButton*) GetDlgItem ( IDC_CHECK_SRCTEX_2 );
	pWnd	= GetDlgItem ( IDC_EDIT_SRCTEX_2 );
	if ( m_Property.m_dwFlag & ISTEXTURE2 )
	{
		pButton->ShowWindow ( TRUE );
		pWnd->ShowWindow ( TRUE );
		pWnd->SetWindowText ( m_Property.m_szSrcTex_2 );

		m_ComboOption2.ShowWindow ( TRUE );

		pWnd	= GetDlgItem ( IDC_EDIT_TEXTURE_2 );
		pWnd->ShowWindow ( TRUE );
		pWnd	= GetDlgItem ( IDC_BUTTON_TEXTURE_2 );
		pWnd->ShowWindow ( TRUE );

		pWnd	= GetDlgItem ( IDC_EDIT_TEXTURE_BUMP_2 );
		pWnd->ShowWindow ( TRUE );
		pWnd	= GetDlgItem ( IDC_BUTTON_TEXTURE_BUMP_2 );
		pWnd->ShowWindow ( TRUE );
	}
	else
	{
		pButton->ShowWindow ( FALSE );
		pWnd->ShowWindow ( FALSE );

		m_ComboOption2.ShowWindow ( FALSE );

		pWnd	= GetDlgItem ( IDC_EDIT_TEXTURE_2 );
		pWnd->ShowWindow ( FALSE );
		pWnd	= GetDlgItem ( IDC_BUTTON_TEXTURE_2 );
		pWnd->ShowWindow ( FALSE );

		pWnd	= GetDlgItem ( IDC_EDIT_TEXTURE_BUMP_2 );
		pWnd->ShowWindow ( FALSE );
		pWnd	= GetDlgItem ( IDC_BUTTON_TEXTURE_BUMP_2 );
		pWnd->ShowWindow ( FALSE );
	}
	if ( m_Property.m_dwFlag & USETEXTURE2 )		pButton->SetCheck ( TRUE );
	else											pButton->SetCheck ( FALSE );

	//	Note : 3
	//
	pButton = (CButton*) GetDlgItem ( IDC_CHECK_SRCTEX_3 );
	pWnd	= GetDlgItem ( IDC_EDIT_SRCTEX_3 );
	if ( m_Property.m_dwFlag & ISTEXTURE3 )
	{
		pButton->ShowWindow ( TRUE );
		pWnd->ShowWindow ( TRUE );
		pWnd->SetWindowText ( m_Property.m_szSrcTex_3 );

		m_ComboOption3.ShowWindow ( TRUE );

		pWnd	= GetDlgItem ( IDC_EDIT_TEXTURE_3 );
		pWnd->ShowWindow ( TRUE );
		pWnd	= GetDlgItem ( IDC_BUTTON_TEXTURE_3 );
		pWnd->ShowWindow ( TRUE );

		pWnd	= GetDlgItem ( IDC_EDIT_TEXTURE_BUMP_3 );
		pWnd->ShowWindow ( TRUE );
		pWnd	= GetDlgItem ( IDC_BUTTON_TEXTURE_BUMP_3 );
		pWnd->ShowWindow ( TRUE );
	}
	else
	{
		pButton->ShowWindow ( FALSE );
		pWnd->ShowWindow ( FALSE );

		m_ComboOption3.ShowWindow ( FALSE );

		pWnd	= GetDlgItem ( IDC_EDIT_TEXTURE_3 );
		pWnd->ShowWindow ( FALSE );
		pWnd	= GetDlgItem ( IDC_BUTTON_TEXTURE_3 );
		pWnd->ShowWindow ( FALSE );

		pWnd	= GetDlgItem ( IDC_EDIT_TEXTURE_BUMP_3 );
		pWnd->ShowWindow ( FALSE );
		pWnd	= GetDlgItem ( IDC_BUTTON_TEXTURE_BUMP_3 );
		pWnd->ShowWindow ( FALSE );
	}
	//if ( m_Property.m_dwFlag & USETEXTURE3 )		pButton->SetCheck ( TRUE );
	//else											pButton->SetCheck ( FALSE );
}

void CEffDot3Page::OnBnClickedButtonTexture1()
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
		pWnd = GetDlgItem ( IDC_EDIT_TEXTURE_1 );
		pWnd->SetWindowText ( FilePath );
	}
}

void CEffDot3Page::OnBnClickedButtonTexture2()
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
		pWnd = GetDlgItem ( IDC_EDIT_TEXTURE_2 );
		pWnd->SetWindowText ( FilePath );
	}
}

void CEffDot3Page::OnBnClickedButtonTexture3()
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
		pWnd = GetDlgItem ( IDC_EDIT_TEXTURE_3 );
		pWnd->SetWindowText ( FilePath );
	}
}

void CEffDot3Page::OnBnClickedButtonTextureBump1()
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
		pWnd = GetDlgItem ( IDC_EDIT_TEXTURE_BUMP_1 );
		pWnd->SetWindowText ( FilePath );
	}
}

void CEffDot3Page::OnBnClickedButtonTextureBump2()
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
		pWnd = GetDlgItem ( IDC_EDIT_TEXTURE_BUMP_2 );
		pWnd->SetWindowText ( FilePath );
	}
}

void CEffDot3Page::OnBnClickedButtonTextureBump3()
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
		pWnd = GetDlgItem ( IDC_EDIT_TEXTURE_BUMP_3 );
		pWnd->SetWindowText ( FilePath );
	}
}

void CEffDot3Page::OnBnClickedButtonReflectTex()
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
