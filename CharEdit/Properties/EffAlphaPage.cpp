// EffAlphaPage.cpp : ���� �����Դϴ�.
//
#include "stdafx.h"
#include "../CharEdit.h"

#include "../MainFrm.h"
#include "../CharEditDoc.h"
#include "../CharEditView.h"

#include "SheetWithTabChar.h"
#include "../Dialog/ColorPickerDlg.h"

#include "../EngineLib/DxEffect/Single/DxEffSinglePropGMan.h"

#include "EffAlphaPage.h"
#include ".\effalphapage.h"

#pragma warning(disable:4996)

#include "../../SigmaCore/DebugInclude.h"

// CEffAlphaPage ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CEffAlphaPage, CPropertyPage)
CEffAlphaPage::CEffAlphaPage()
	: CEffCharPage(CEffAlphaPage::IDD)

{
}

CEffAlphaPage::~CEffAlphaPage()
{
}

void CEffAlphaPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CEffAlphaPage, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnBnClickedButtonApply)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(IDC_CHECK_TRANSLUCENT, OnBnClickedCheckTranslucent)
END_MESSAGE_MAP()


// CEffAlphaPage �޽��� ó�����Դϴ�.

void CEffAlphaPage::SetProperty ( EFFCHAR_PROPERTY *pProp )
{
//	char	szValue[256];
	if ( pProp )
		m_Property = *((EFFCHAR_PROPERTY_ALPHA*)pProp);

	CWnd*		pWnd;
	CButton*	pButton;
	char		szValue[256];

	pButton = (CButton*)GetDlgItem ( IDC_EDIT_MIN_ALPHA );
	pButton->SetCheck ( m_Property.m_dwFlags&USETRANSLUCENT );

	pWnd = GetDlgItem ( IDC_EDIT_MIN_ALPHA );
	_gcvt ( m_Property.m_fAlpha, 7, szValue );
	pWnd->SetWindowText ( szValue );

	pWnd = GetDlgItem ( IDC_EDIT_WAIT );
	_gcvt ( m_Property.m_fWaitTime, 7, szValue );
	pWnd->SetWindowText ( szValue );

	pWnd = GetDlgItem ( IDC_EDIT_PLAY );
	_gcvt ( m_Property.m_fPlayTime, 7, szValue );
	pWnd->SetWindowText ( szValue );
}

void CEffAlphaPage::OnBnClickedButtonApply()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxSkinPiece *pSkinPiece = pView->GetSkinPiece ();

	CString		strText;
	CWnd		*pWnd;

	pWnd = GetDlgItem ( IDC_EDIT_MIN_ALPHA );
	pWnd->GetWindowText ( strText );
	m_Property.m_fAlpha = (float)atof ( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_WAIT );
	pWnd->GetWindowText ( strText );
	m_Property.m_fWaitTime = (float)atof ( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_PLAY );
	pWnd->GetWindowText ( strText );
	m_Property.m_fPlayTime = (float)atof ( strText.GetString() );


	if ( !m_pEffChar )
	{
		m_pEffChar = DxEffCharMan::GetInstance().CreateEffect
		(
			DxEffCharAlpha::TYPEID,
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

void CEffAlphaPage::OnBnClickedOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	OnBnClickedButtonApply();

    ReturnPage();
}

void CEffAlphaPage::OnBnClickedCancel()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
    ReturnPage();
}

void CEffAlphaPage::OnBnClickedCheckTranslucent()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CButton*	pButton;

	pButton = (CButton*)GetDlgItem ( IDC_EDIT_MIN_ALPHA );
	if ( pButton->GetCheck() )	m_Property.m_dwFlags |= USETRANSLUCENT;
	else						m_Property.m_dwFlags &= ~USETRANSLUCENT;
}
