// EffToonPage.cpp : ���� �����Դϴ�.
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

// CEffToonPage ��ȭ �����Դϴ�.

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


// CEffToonPage �޽��� ó�����Դϴ�.
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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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
			MessageBox ( _T("����Ʈ ������ �����Ͽ����ϴ�."), _T("ERROR"), MB_OK );
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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	OnBnClickedButtonApply();

    ReturnPage();
}

void CEffToonPage::OnBnClickedCancel()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
    ReturnPage();
}

void CEffToonPage::OnBnClickedButtonTexture()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CButton* pButton = (CButton*)GetDlgItem( IDC_CHECK_OBLIQUE_LINE );
	if( pButton->GetCheck() )	m_Property.m_dwFlag |= USE_OBLIQUE;
	else						m_Property.m_dwFlag &= ~USE_OBLIQUE;

	m_pEffChar->SetProperty( &m_Property );
}

void CEffToonPage::OnBnClickedButtonObliqueTex()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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



