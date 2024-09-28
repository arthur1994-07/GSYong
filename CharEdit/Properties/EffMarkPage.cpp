// EffParticlePage.cpp : ���� �����Դϴ�.
//
#include "stdafx.h"
#include "../CharEdit.h"

#include "../MainFrm.h"
#include "../CharEditDoc.h"
#include "../CharEditView.h"

#include "../Dialog/SetVar.h"

#include "SheetWithTabChar.h"
#include "../Dialog/ColorPickerDlg.h"

#include "../EngineLib/DxTools/TextureManager.h"
#include "../EngineLib/DxEffect/Single/DxEffSinglePropGMan.h"

#include "EffMarkPage.h"

#pragma warning(disable:4996)

#include "../../SigmaCore/DebugInclude.h"

// CEffMarkPage ��ȭ �����Դϴ�.
IMPLEMENT_DYNAMIC(CEffMarkPage, CPropertyPage)
CEffMarkPage::CEffMarkPage()
	: CEffCharPage(CEffMarkPage::IDD)
{
}

CEffMarkPage::~CEffMarkPage()
{
}

void CEffMarkPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CEffMarkPage, CPropertyPage)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnBnClickedButtonApply)
	ON_BN_CLICKED(IDC_BUTTON_TEX1, OnBnClickedButtonTex1)
	ON_BN_CLICKED(IDC_BUTTON_TEX2, OnBnClickedButtonTex2)
	ON_BN_CLICKED(IDC_BUTTON_TEX_RE1, OnBnClickedButtonTexRe1)
	ON_BN_CLICKED(IDC_BUTTON_TEX_RE2, OnBnClickedButtonTexRe2)
	ON_BN_CLICKED(IDC_BUTTON_MOVE_TEX1, OnBnClickedButtonMoveTex1)
	ON_BN_CLICKED(IDC_BUTTON_MOVE_TEX2, OnBnClickedButtonMoveTex2)
	ON_BN_CLICKED(IDC_BUTTON_COLOR1, OnBnClickedButtonColor1)
	ON_BN_CLICKED(IDC_BUTTON_COLOR2, OnBnClickedButtonColor2)
	ON_BN_CLICKED(IDC_RADIO_TEX1, OnBnClickedRadioTex1)
	ON_BN_CLICKED(IDC_RADIO_TEX2, OnBnClickedRadioTex2)
END_MESSAGE_MAP()


// CEffMarkPage �޽��� ó�����Դϴ�.

void CEffMarkPage::SetProperty ( EFFCHAR_PROPERTY *pProp )
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxSkinPiece *pSkinPiece = pView->GetSkinPiece ();

	if ( pProp )
		m_Property = *((EFFCHAR_PROPERTY_MARK*)pProp);

	if ( !m_pEffChar )
	{
		m_pEffChar = DxEffCharMan::GetInstance().CreateEffect
		(
			DxEffCharMark::TYPEID,
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

		m_Property = *((EFFCHAR_PROPERTY_MARK*)m_pEffChar->GetProperty ());
	}
	
	CWnd*		pWnd;
	CButton*	pButton;

	if ( m_Property.m_nTexNum == 1 )
	{
		pButton = (CButton*)GetDlgItem ( IDC_RADIO_TEX1 );
		pButton->SetCheck ( TRUE );
		pButton = (CButton*)GetDlgItem ( IDC_RADIO_TEX2 );
		pButton->SetCheck ( FALSE );
	}
	else
	{
		pButton = (CButton*)GetDlgItem ( IDC_RADIO_TEX1 );
		pButton->SetCheck ( FALSE );
		pButton = (CButton*)GetDlgItem ( IDC_RADIO_TEX2 );
		pButton->SetCheck ( TRUE );
	}

	pWnd = GetDlgItem ( IDC_EDIT_TEX1 );
	pWnd->SetWindowText ( m_Property.m_szTex1 );

	pWnd = GetDlgItem ( IDC_EDIT_TEX2 );
	pWnd->SetWindowText ( m_Property.m_szTex2 );

	//	Enable �� Disable �з�
	if ( m_Property.m_nTexNum == 1 )
	{
		pWnd = GetDlgItem ( IDC_BUTTON_TEX_RE2 );
		pWnd->EnableWindow ( FALSE );
		pWnd = GetDlgItem ( IDC_BUTTON_TEX2 );
		pWnd->EnableWindow ( FALSE );
		pWnd = GetDlgItem ( IDC_BUTTON_MOVE_TEX2 );
		pWnd->EnableWindow ( FALSE );
		pWnd = GetDlgItem ( IDC_BUTTON_COLOR2 );
		pWnd->EnableWindow ( FALSE );
	}
	else
	{
		pWnd = GetDlgItem ( IDC_BUTTON_TEX_RE2 );
		pWnd->EnableWindow ( TRUE );
		pWnd = GetDlgItem ( IDC_BUTTON_TEX2 );
		pWnd->EnableWindow ( TRUE );
		pWnd = GetDlgItem ( IDC_BUTTON_MOVE_TEX2 );
		pWnd->EnableWindow ( TRUE );
		pWnd = GetDlgItem ( IDC_BUTTON_COLOR2 );
		pWnd->EnableWindow ( TRUE );
	}
}

void CEffMarkPage::OnBnClickedButtonApply()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxSkinPiece *pSkinPiece = pView->GetSkinPiece ();

	CString	strTexture;
	CWnd *pWnd;

	const char* szTexture;
	pWnd = GetDlgItem ( IDC_EDIT_TEX1 );
	pWnd->GetWindowText ( strTexture );
	szTexture = strTexture.GetString();
	strcpy ( m_Property.m_szTex1, szTexture );

	pWnd = GetDlgItem ( IDC_EDIT_TEX2 );
	pWnd->GetWindowText ( strTexture );
	szTexture = strTexture.GetString();
	strcpy ( m_Property.m_szTex2, szTexture );

	m_pEffChar->SetProperty ( &m_Property );
	m_pEffChar->Create ( pView->GetD3dDevice() );

    EffectApply();
}

void CEffMarkPage::OnBnClickedOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	OnBnClickedButtonApply();

    ReturnPage();
}

void CEffMarkPage::OnBnClickedCancel()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
    ReturnPage();
}
void CEffMarkPage::OnBnClickedButtonTex1()
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
		pWnd = GetDlgItem ( IDC_EDIT_TEX1 );
		pWnd->SetWindowText ( FilePath );
	}
}

void CEffMarkPage::OnBnClickedButtonTex2()
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
		pWnd = GetDlgItem ( IDC_EDIT_TEX2 );
		pWnd->SetWindowText ( FilePath );
	}
}

void CEffMarkPage::OnBnClickedRadioTex1()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_Property.m_nTexNum = 1;

	CWnd*	pWnd;
	pWnd = GetDlgItem ( IDC_BUTTON_TEX_RE2 );
	pWnd->EnableWindow ( FALSE );
	pWnd = GetDlgItem ( IDC_BUTTON_TEX2 );
	pWnd->EnableWindow ( FALSE );
	pWnd = GetDlgItem ( IDC_BUTTON_MOVE_TEX2 );
	pWnd->EnableWindow ( FALSE );
	pWnd = GetDlgItem ( IDC_BUTTON_COLOR2 );
	pWnd->EnableWindow ( FALSE );
}

void CEffMarkPage::OnBnClickedRadioTex2()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_Property.m_nTexNum = 2;

	CWnd*	pWnd;
	pWnd = GetDlgItem ( IDC_BUTTON_TEX_RE2 );
	pWnd->EnableWindow ( TRUE );
	pWnd = GetDlgItem ( IDC_BUTTON_TEX2 );
	pWnd->EnableWindow ( TRUE );
	pWnd = GetDlgItem ( IDC_BUTTON_MOVE_TEX2 );
	pWnd->EnableWindow ( TRUE );
	pWnd = GetDlgItem ( IDC_BUTTON_COLOR2 );
	pWnd->EnableWindow ( TRUE );
}

void CEffMarkPage::OnBnClickedButtonTexRe1()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CSetVar DlgSetVar;

	DlgSetVar.m_StrTitle = "Tex �ݺ� 1";
	DlgSetVar.m_StrComm1 = "U��ǥ :";
	DlgSetVar.m_StrComm3 = "V��ǥ :";
	DlgSetVar.m_dwFlag = USESTARTSCROLL | USEENDSCROLL;
	DlgSetVar.m_bZeroBase = FALSE;
	DlgSetVar.m_ToScale = 80.0f;

	DlgSetVar.m_fStart = m_Property.m_vTexUV01.x;
	DlgSetVar.m_fEnd = m_Property.m_vTexUV01.y;
	
	if ( IDOK == DlgSetVar.DoModal() )
	{
		m_Property.m_vTexUV01.x = DlgSetVar.m_fStart;
		m_Property.m_vTexUV01.y = DlgSetVar.m_fEnd;
	}
}

void CEffMarkPage::OnBnClickedButtonTexRe2()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CSetVar DlgSetVar;

	DlgSetVar.m_StrTitle = "Tex �ݺ� 2";
	DlgSetVar.m_StrComm1 = "U��ǥ :";
	DlgSetVar.m_StrComm3 = "V��ǥ :";
	DlgSetVar.m_dwFlag = USESTARTSCROLL | USEENDSCROLL;
	DlgSetVar.m_bZeroBase = FALSE;
	DlgSetVar.m_ToScale = 80.0f;

	DlgSetVar.m_fStart = m_Property.m_vTexUV02.x;
	DlgSetVar.m_fEnd = m_Property.m_vTexUV02.y;
	
	if ( IDOK == DlgSetVar.DoModal() )
	{
		m_Property.m_vTexUV02.x = DlgSetVar.m_fStart;
		m_Property.m_vTexUV02.y = DlgSetVar.m_fEnd;
	}
}

void CEffMarkPage::OnBnClickedButtonMoveTex1()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CSetVar DlgSetVar;

	DlgSetVar.m_StrTitle = "Tex �̵� 1";
	DlgSetVar.m_StrComm1 = "X�� �̵�";
	DlgSetVar.m_StrComm3 = "Y�� �̵�";
	DlgSetVar.m_dwFlag = USESTARTSCROLL | USEENDSCROLL;
	DlgSetVar.m_bZeroBase = FALSE;
	DlgSetVar.m_ToScale = 800.0f;

	DlgSetVar.m_fStart = m_Property.m_vTex01.x;
	DlgSetVar.m_fEnd = m_Property.m_vTex01.y;
	
	if ( IDOK == DlgSetVar.DoModal() )
	{
		m_Property.m_vTex01.x = DlgSetVar.m_fStart;
		m_Property.m_vTex01.y = DlgSetVar.m_fEnd;
	}
}

void CEffMarkPage::OnBnClickedButtonMoveTex2()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CSetVar DlgSetVar;

	DlgSetVar.m_StrTitle = "Tex �̵� 2";
	DlgSetVar.m_StrComm1 = "X�� �̵�";
	DlgSetVar.m_StrComm3 = "Y�� �̵�";
	DlgSetVar.m_dwFlag = USESTARTSCROLL | USEENDSCROLL;
	DlgSetVar.m_bZeroBase = FALSE;
	DlgSetVar.m_ToScale = 800.0f;

	DlgSetVar.m_fStart = m_Property.m_vTex02.x;
	DlgSetVar.m_fEnd = m_Property.m_vTex02.y;
	
	if ( IDOK == DlgSetVar.DoModal() )
	{
		m_Property.m_vTex02.x = DlgSetVar.m_fStart;
		m_Property.m_vTex02.y = DlgSetVar.m_fEnd;
	}
}

void CEffMarkPage::OnBnClickedButtonColor1()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	int ThisR = (m_Property.m_cDiffuse1&0xff0000)>>16;
	int ThisG = (m_Property.m_cDiffuse1&0xff00)>>8;
	int ThisB = (m_Property.m_cDiffuse1&0xff);

	CColorPickerDlg ColorPicker ( RGB(ThisR,ThisG,ThisB) );
	
	if ( IDOK == ColorPicker.DoModal() )
	{
		COLORREF SelectColor = ColorPicker.GetColor ();
		m_Property.m_cDiffuse1 = 0xff000000;
		m_Property.m_cDiffuse1 += GetRValue(SelectColor)<<16;
		m_Property.m_cDiffuse1 += GetGValue(SelectColor)<<8;
		m_Property.m_cDiffuse1 += GetBValue(SelectColor);
	}
}

void CEffMarkPage::OnBnClickedButtonColor2()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	int ThisR = (m_Property.m_cDiffuse2&0xff0000)>>16;
	int ThisG = (m_Property.m_cDiffuse2&0xff00)>>8;
	int ThisB = (m_Property.m_cDiffuse2&0xff);

	CColorPickerDlg ColorPicker ( RGB(ThisR,ThisG,ThisB) );
	
	if ( IDOK == ColorPicker.DoModal() )
	{
		COLORREF SelectColor = ColorPicker.GetColor ();
		m_Property.m_cDiffuse2 = 0xff000000;
		m_Property.m_cDiffuse2 += GetRValue(SelectColor)<<16;
		m_Property.m_cDiffuse2 += GetGValue(SelectColor)<<8;
		m_Property.m_cDiffuse2 += GetBValue(SelectColor);
	}
}
