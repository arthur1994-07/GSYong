// EffAmbientPage.cpp : ���� �����Դϴ�.
//
#include "stdafx.h"
#include "../CharEdit.h"

#include "../MainFrm.h"
#include "../CharEditDoc.h"
#include "../CharEditView.h"

#include "SheetWithTabChar.h"
#include "../Dialog/ColorPickerDlg.h"

#include "../EngineLib/DxEffect/Single/DxEffSinglePropGMan.h"

#include "EffAmbientPage.h"
#include ".\effambientpage.h"

#pragma warning(disable:4996)

#include "../../SigmaCore/DebugInclude.h"

// CEffAmbientPage ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CEffAmbientPage, CPropertyPage)
CEffAmbientPage::CEffAmbientPage()
	: CEffCharPage(CEffAmbientPage::IDD)
{
	m_vecEffCharList.clear();
}

CEffAmbientPage::~CEffAmbientPage()
{
}

void CEffAmbientPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CEffAmbientPage, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnBnClickedButtonApply)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(IDC_RADIO_TIME, OnBnClickedRadioTime)
	ON_BN_CLICKED(IDC_RADIO_ALL, OnBnClickedRadioAll)
	ON_BN_CLICKED(IDC_BUTTON_COLOR, OnBnClickedButtonColor)
	ON_BN_CLICKED(IDC_RADIO_TEXCOLOR, OnBnClickedRadioTexcolor)
	ON_BN_CLICKED(IDC_RADIO_AMBIENT, OnBnClickedRadioAmbient)
END_MESSAGE_MAP()


// CEffAmbientPage �޽��� ó�����Դϴ�.

void CEffAmbientPage::SetProperty ( EFFCHAR_PROPERTY *pProp )
{
	if ( pProp )
		m_Property = *((EFFCHAR_PROPERTY_AMBIENT*)pProp);

	CWnd*		pWnd;
	CButton*	pButton;
	char		szValue[256];

	if ( m_Property.m_dwFlage & USEALL )
	{
		pButton = (CButton*)GetDlgItem(IDC_RADIO_TIME);
		pButton->SetCheck ( FALSE );
		pButton = (CButton*)GetDlgItem(IDC_RADIO_ALL);
		pButton->SetCheck ( TRUE );

		pWnd = GetDlgItem(IDC_EDIT_TIME);
		pWnd->EnableWindow ( FALSE );
	}
	else
	{
		pButton = (CButton*)GetDlgItem(IDC_RADIO_TIME);
		pButton->SetCheck ( TRUE );
		pButton = (CButton*)GetDlgItem(IDC_RADIO_ALL);
		pButton->SetCheck ( FALSE );

		pWnd = GetDlgItem(IDC_EDIT_TIME);
		pWnd->EnableWindow ( TRUE );
	}

	if ( m_Property.m_dwFlage & USETEXCOLOR )
	{
		pButton = (CButton*)GetDlgItem(IDC_RADIO_TEXCOLOR);
		pButton->SetCheck( TRUE );
		pButton = (CButton*)GetDlgItem(IDC_RADIO_AMBIENT);
		pButton->SetCheck( FALSE );
	}
	else
	{
		pButton = (CButton*)GetDlgItem(IDC_RADIO_TEXCOLOR);
		pButton->SetCheck( FALSE );
		pButton = (CButton*)GetDlgItem(IDC_RADIO_AMBIENT);
		pButton->SetCheck( TRUE );
	}

	pWnd = GetDlgItem(IDC_EDIT_TIME);
	_gcvt ( m_Property.m_fFullTime, 7, szValue );
	pWnd->SetWindowText ( szValue );
}

void CEffAmbientPage::OnBnClickedButtonApply()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();

	CString	strText;
	CWnd	*pWnd;

	pWnd = GetDlgItem ( IDC_EDIT_TIME );
	pWnd->GetWindowText ( strText );
	m_Property.m_fFullTime = (float)atof ( strText.GetString() );

	if( CCharEditView::m_emObjType == EOBJ_SKINPIECE )
	{
		DxSkinPiece *pSkinPiece = pView->GetSkinPiece ();
		if ( !m_pEffChar )
		{
			m_pEffChar = DxEffCharMan::GetInstance().CreateEffect( DxEffCharAmbient::TYPEID, &m_Property, NULL, pSkinPiece );

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
	}

    EffectApply();
}

void CEffAmbientPage::OnBnClickedOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	OnBnClickedButtonApply();

#ifndef DEF_SKIN_SYNC
	if( CCharEditView::m_emObjType == EOBJ_SKINPIECE )
	{
		m_pEffChar = NULL;
		m_pSheetTab->m_SkinPiecePage.EffectListting ();
		m_pSheetTab->SetActivePage ( CHARTAB_CHARPIECE );
	}
#else
    ReturnPage();
#endif
}

void CEffAmbientPage::OnBnClickedCancel()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

#ifndef DEF_SKIN_SYNC
	if( CCharEditView::m_emObjType == EOBJ_SKINPIECE )
	{
		m_pEffChar = NULL;
		m_pSheetTab->m_SkinPiecePage.EffectListting ();
		m_pSheetTab->SetActivePage ( CHARTAB_CHARPIECE );
	}
#else
    ReturnPage();
#endif
}

void CEffAmbientPage::OnBnClickedRadioTime()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CWnd*		pWnd;
	CButton*	pButton;

	m_Property.m_dwFlage &= ~USEALL;

	pButton = (CButton*)GetDlgItem(IDC_RADIO_TIME);
	pButton->SetCheck ( TRUE );
	pButton = (CButton*)GetDlgItem(IDC_RADIO_ALL);
	pButton->SetCheck ( FALSE );

	pWnd = GetDlgItem(IDC_EDIT_TIME);
	pWnd->EnableWindow ( TRUE );
}

void CEffAmbientPage::OnBnClickedRadioAll()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CWnd*		pWnd;
	CButton*	pButton;

	m_Property.m_dwFlage |= USEALL;

	pButton = (CButton*)GetDlgItem(IDC_RADIO_TIME);
	pButton->SetCheck ( FALSE );
	pButton = (CButton*)GetDlgItem(IDC_RADIO_ALL);
	pButton->SetCheck ( TRUE );

	pWnd = GetDlgItem(IDC_EDIT_TIME);
	pWnd->EnableWindow ( FALSE );
}

void CEffAmbientPage::OnBnClickedButtonColor()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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

void CEffAmbientPage::OnBnClickedRadioTexcolor()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_Property.m_dwFlage |= USETEXCOLOR;

	OnBnClickedButtonApply();
}

void CEffAmbientPage::OnBnClickedRadioAmbient()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_Property.m_dwFlage &= ~USETEXCOLOR;

	OnBnClickedButtonApply();
}


