// EffReflectionPage.cpp : ���� �����Դϴ�.
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

#include "EffDustPage.h"

#pragma warning(disable:4996)

#include "../../SigmaCore/DebugInclude.h"

// CEffDustPage ��ȭ �����Դϴ�.
IMPLEMENT_DYNAMIC(CEffDustPage, CPropertyPage)
CEffDustPage::CEffDustPage()
	: CEffCharPage(CEffDustPage::IDD)
{
}

CEffDustPage::~CEffDustPage()
{
}

void CEffDustPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CEffDustPage, CPropertyPage)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnBnClickedButtonApply)
	ON_BN_CLICKED(IDC_BUTTON_REFLECT_TEX, OnBnClickedButtonReflectTex)
END_MESSAGE_MAP()


// CEffDustPage �޽��� ó�����Դϴ�.

BOOL CEffDustPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void CEffDustPage::SetProperty ( EFFCHAR_PROPERTY *pProp )
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxSkinPiece *pSkinPiece = pView->GetSkinPiece ();

	if ( pProp )
		m_Property = *((EFFCHAR_PROPERTY_DUST*)pProp);

	if ( !m_pEffChar )
	{
		m_pEffChar = DxEffCharMan::GetInstance().CreateEffect
		(
			DxEffCharDust::TYPEID,
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
	}

	CWnd*		pWnd;
	pWnd = GetDlgItem ( IDC_EDIT_REFLECT_TEX );
	pWnd->SetWindowText ( m_Property.m_szTexture );
}

void CEffDustPage::OnBnClickedButtonApply()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxSkinPiece *pSkinPiece = pView->GetSkinPiece ();

	CString	strText;
	CWnd *pWnd;

	const char* szTexture;
	pWnd = GetDlgItem ( IDC_EDIT_REFLECT_TEX );
	pWnd->GetWindowText ( strText );
	szTexture = strText.GetString();
	strcpy ( m_Property.m_szTexture, szTexture );

	m_pEffChar->SetProperty ( &m_Property );
	m_pEffChar->Create ( pView->GetD3dDevice() );

    EffectApply();
}

void CEffDustPage::OnBnClickedOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	OnBnClickedButtonApply();

    ReturnPage();
}

void CEffDustPage::OnBnClickedCancel()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
    ReturnPage();
}

void CEffDustPage::OnBnClickedButtonReflectTex()
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
		pWnd = GetDlgItem ( IDC_EDIT_REFLECT_TEX );
		pWnd->SetWindowText ( FilePath );
	}
}