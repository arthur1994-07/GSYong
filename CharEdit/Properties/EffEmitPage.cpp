// EffEmitPage.cpp : ���� �����Դϴ�.
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

#include "EffEmitPage.h"

#pragma warning(disable:4996)

#include "../../SigmaCore/DebugInclude.h"

// CEffEmitPage ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CEffEmitPage, CPropertyPage)
CEffEmitPage::CEffEmitPage()
	: CEffCharPage(CEffEmitPage::IDD)
{
}

CEffEmitPage::~CEffEmitPage()
{
}

void CEffEmitPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CEffEmitPage, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnBnClickedButtonApply)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_TEXTURE, OnBnClickedButtonTexture)
END_MESSAGE_MAP()


// CEffEmitPage �޽��� ó�����Դϴ�.

void CEffEmitPage::SetProperty ( EFFCHAR_PROPERTY *pProp )
{
	if ( pProp )
		m_Property = *((EFFCHAR_PROPERTY_EMIT*)pProp);

	CWnd*	pWnd;

	pWnd = GetDlgItem ( IDC_EDIT_TEXTURE );
	pWnd->SetWindowText ( m_Property.m_szTexture );
}

void CEffEmitPage::OnBnClickedButtonApply()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxSkinPiece *pSkinPiece = pView->GetSkinPiece ();

	CString	strText;
	CWnd	*pWnd;

	const char* szTexture;
	pWnd = GetDlgItem ( IDC_EDIT_TEXTURE );
	pWnd->GetWindowText ( strText );
	szTexture = strText.GetString();
	strcpy ( m_Property.m_szTexture, szTexture );

	if ( !m_pEffChar )
	{
		m_pEffChar = DxEffCharMan::GetInstance().CreateEffect
		(
			DxEffCharEmit::TYPEID,
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
	else
	{
		m_pEffChar->SetProperty ( &m_Property );
		m_pEffChar->Create ( pView->GetD3dDevice() );
	}

    EffectApply();
}

void CEffEmitPage::OnBnClickedOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	OnBnClickedButtonApply();

    ReturnPage();
}

void CEffEmitPage::OnBnClickedCancel()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
    ReturnPage();
}

void CEffEmitPage::OnBnClickedButtonTexture()
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
