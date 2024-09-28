// EffSpecularPage.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "../CharEdit.h"

#include "../MainFrm.h"
#include "../CharEditDoc.h"
#include "../CharEditView.h"

#include "SheetWithTabChar.h"
#include "../Dialog/ColorPickerDlg.h"

#include "../EngineLib/DxEffect/Single/DxEffSinglePropGMan.h"

#include "EffSpecularPage.h"


// CEffSpecularPage ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CEffSpecularPage, CPropertyPage)
CEffSpecularPage::CEffSpecularPage()
	: CEffCharPage(CEffSpecularPage::IDD)
{
}

CEffSpecularPage::~CEffSpecularPage()
{
}

void CEffSpecularPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CEffSpecularPage, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnBnClickedButtonApply)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(IDC_CHECK_SRCTEX_1, OnBnClickedCheckSrctex1)
	ON_BN_CLICKED(IDC_CHECK_SRCTEX_2, OnBnClickedCheckSrctex2)
	ON_BN_CLICKED(IDC_CHECK_SRCTEX_3, OnBnClickedCheckSrctex3)
END_MESSAGE_MAP()


// CEffSpecularPage �޽��� ó�����Դϴ�.
void CEffSpecularPage::SetProperty ( EFFCHAR_PROPERTY *pProp )
{
	if ( pProp )
		m_Property = *((EFFCHAR_PROPERTY_SPECULAR*)pProp);

	CWnd*		pWnd;
	CButton*	pButton;

	//	Note : 1
	//
	pButton = (CButton*) GetDlgItem ( IDC_CHECK_SRCTEX_1 );
	pWnd	= GetDlgItem ( IDC_EDIT_SRCTEX_1 );
	if ( m_Property.m_dwFlag & ISTEXTURE1 )
	{
		pButton->ShowWindow ( TRUE );
		pWnd->ShowWindow ( TRUE );

		pWnd->SetWindowText ( m_Property.m_szSrcTex_1 );
	}
	else
	{
		pButton->ShowWindow ( FALSE );
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
	}
	else
	{
		pButton->ShowWindow ( FALSE );
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
	}
	else
	{
		pButton->ShowWindow ( FALSE );
		pWnd->ShowWindow ( FALSE );
	}
	if ( m_Property.m_dwFlag & USETEXTURE3 )		pButton->SetCheck ( TRUE );
	else											pButton->SetCheck ( FALSE );
}

void CEffSpecularPage::OnBnClickedButtonApply()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxSkinPiece *pSkinPiece = pView->GetSkinPiece ();

	CString	strText;

	if ( !m_pEffChar )
	{
		m_pEffChar = DxEffCharMan::GetInstance().CreateEffect
		(
			DxEffCharSpecular::TYPEID,
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

		//	Note : �׸�.. �׸�.... ��............... ................ �ؽ��� �˾ƿ��� ���ؼ�
		//
		m_Property = *(EFFCHAR_PROPERTY_SPECULAR*)m_pEffChar->GetProperty ();
		ResetCheck();
	}
	else
	{
		m_pEffChar->SetProperty ( &m_Property );
		m_pEffChar->Create ( pView->GetD3dDevice() );

		//	Note : �׸�.. �׸�.... ��............... ................ �ؽ��� �˾ƿ��� ���ؼ�
		//
		m_Property = *(EFFCHAR_PROPERTY_SPECULAR*)m_pEffChar->GetProperty ();
		ResetCheck();
	}

    EffectApply();
}

void CEffSpecularPage::OnBnClickedOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	OnBnClickedButtonApply();

    ReturnPage();
}

void CEffSpecularPage::OnBnClickedCancel()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
    ReturnPage();
}

void CEffSpecularPage::OnBnClickedCheckSrctex1()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CButton*	pButton;
	pButton = (CButton*) GetDlgItem ( IDC_CHECK_SRCTEX_1 );
	if ( pButton->GetCheck() )	m_Property.m_dwFlag |= USETEXTURE1;
	else						m_Property.m_dwFlag &= ~USETEXTURE1;
}

void CEffSpecularPage::OnBnClickedCheckSrctex2()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CButton*	pButton;
	pButton = (CButton*) GetDlgItem ( IDC_CHECK_SRCTEX_2 );
	if ( pButton->GetCheck() )	m_Property.m_dwFlag |= USETEXTURE2;
	else						m_Property.m_dwFlag &= ~USETEXTURE2;
}

void CEffSpecularPage::OnBnClickedCheckSrctex3()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CButton*	pButton;
	pButton = (CButton*) GetDlgItem ( IDC_CHECK_SRCTEX_3 );
	if ( pButton->GetCheck() )	m_Property.m_dwFlag |= USETEXTURE3;
	else						m_Property.m_dwFlag &= ~USETEXTURE3;
}

void CEffSpecularPage::ResetCheck ()
{
	CWnd*		pWnd;
	CButton*	pButton;

	//	Note : 1
	//
	pButton = (CButton*) GetDlgItem ( IDC_CHECK_SRCTEX_1 );
	pWnd	= GetDlgItem ( IDC_EDIT_SRCTEX_1 );
	if ( m_Property.m_dwFlag & ISTEXTURE1 )
	{
		pButton->ShowWindow ( TRUE );
		pWnd->ShowWindow ( TRUE );

		pWnd->SetWindowText ( m_Property.m_szSrcTex_1 );
	}
	else
	{
		pButton->ShowWindow ( FALSE );
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
	}
	else
	{
		pButton->ShowWindow ( FALSE );
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
	}
	else
	{
		pButton->ShowWindow ( FALSE );
		pWnd->ShowWindow ( FALSE );
	}
	if ( m_Property.m_dwFlag & USETEXTURE3 )		pButton->SetCheck ( TRUE );
	else											pButton->SetCheck ( FALSE );
}