// EffToonPage.cpp : 구현 파일입니다.
//
#include "stdafx.h"
#include "../CharEdit.h"

#include "../MainFrm.h"
#include "../CharEditDoc.h"
#include "../CharEditView.h"

#include "SheetWithTabChar.h"
#include "../Dialog/ColorPickerDlg.h"

#include "../EngineLib/DxEffect/Single/DxEffSinglePropGMan.h"

#include "EffToonPage.h"
#include ".\efftoonpage.h"

#pragma warning(disable:4996)

#include "../../SigmaCore/DebugInclude.h"

// CEffToonPage 대화 상자입니다.

IMPLEMENT_DYNAMIC(CEffToonPage, CPropertyPage)
CEffToonPage::CEffToonPage()
	: CEffCharPage(CEffToonPage::IDD)
{
}

CEffToonPage::~CEffToonPage()
{
}

void CEffToonPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CEffToonPage, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnBnClickedButtonApply)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_TEXTURE, OnBnClickedButtonTexture)
	ON_BN_CLICKED(IDC_CHECK_OBLIQUE_LINE, OnBnClickedCheckObliqueLine)
	ON_BN_CLICKED(IDC_BUTTON_OBLIQUE_TEX, OnBnClickedButtonObliqueTex)
END_MESSAGE_MAP()


// CEffToonPage 메시지 처리기입니다.
void CEffToonPage::SetProperty ( EFFCHAR_PROPERTY *pProp )
{
	if ( pProp )
		m_Property = *((EFFCHAR_PROPERTY_TOON*)pProp);

	TCHAR szValue[256];
	CWnd* pWnd = GetDlgItem( IDC_EDIT_THICKNESS );
	_gcvt( m_Property.m_fSize, 7, szValue );
	pWnd->SetWindowText( szValue );

	pWnd = GetDlgItem( IDC_EDIT_ADD_THICKNESS );
	_gcvt( m_Property.m_fSizeADD, 7, szValue );
	pWnd->SetWindowText( szValue );

	pWnd = GetDlgItem( IDC_EDIT_TEXTURE );
	pWnd->SetWindowText( m_Property.m_szToonTex );

	pWnd = GetDlgItem( IDC_EDIT_OBLIQUE_TEX );
	pWnd->SetWindowText( m_Property.m_szObliqueTex );

	CButton* pButton = (CButton*)GetDlgItem( IDC_CHECK_OBLIQUE_LINE );
	if( m_Property.m_dwFlag&USE_OBLIQUE )	pButton->SetCheck( TRUE );
	else									pButton->SetCheck( FALSE );
}

void CEffToonPage::OnBnClickedButtonApply()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxSkinPiece *pSkinPiece = pView->GetSkinPiece ();

	CString	strText;
	CWnd* pWnd = GetDlgItem( IDC_EDIT_THICKNESS );
	pWnd->GetWindowText ( strText );
	m_Property.m_fSize = (float)_tstof( strText.GetString() );

	pWnd = GetDlgItem( IDC_EDIT_ADD_THICKNESS );
	pWnd->GetWindowText ( strText );
	m_Property.m_fSizeADD = (float)_tstof( strText.GetString() );

	

	pWnd = GetDlgItem( IDC_EDIT_TEXTURE );
	pWnd->GetWindowText ( strText );
	StringCchCopy( m_Property.m_szToonTex, 32, strText.GetString() );

	pWnd = GetDlgItem( IDC_EDIT_OBLIQUE_TEX );
	pWnd->GetWindowText ( strText );
	StringCchCopy( m_Property.m_szObliqueTex, 32, strText.GetString() );


	if ( !m_pEffChar )
	{
		m_pEffChar = DxEffCharMan::GetInstance().CreateEffect
		(
			DxEffCharToon::TYPEID,
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
	}
	else
	{
		m_pEffChar->SetProperty ( &m_Property );
		m_pEffChar->Create ( pView->GetD3dDevice() );
	}

	float fThickness(0.1f);
	pSkinPiece->CreateCartoonMesh( pView->GetD3dDevice(), m_Property.m_fSize, m_Property.m_fSizeADD );

    EffectApply();
}

void CEffToonPage::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	OnBnClickedButtonApply();

    ReturnPage();
}

void CEffToonPage::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    ReturnPage();
}

void CEffToonPage::OnBnClickedButtonTexture()
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

void CEffToonPage::OnBnClickedCheckObliqueLine()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CButton* pButton = (CButton*)GetDlgItem( IDC_CHECK_OBLIQUE_LINE );
	if( pButton->GetCheck() )	m_Property.m_dwFlag |= USE_OBLIQUE;
	else						m_Property.m_dwFlag &= ~USE_OBLIQUE;

	m_pEffChar->SetProperty( &m_Property );
}

void CEffToonPage::OnBnClickedButtonObliqueTex()
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
		pWnd = GetDlgItem ( IDC_EDIT_OBLIQUE_TEX );
		pWnd->SetWindowText ( FilePath );
	}
}



