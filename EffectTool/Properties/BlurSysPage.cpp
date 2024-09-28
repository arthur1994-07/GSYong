// BlurSysPage.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "../EffectTool.h"
#include "../MainFrm.h"

#include "../EffectToolDoc.h"
#include "../EffectToolView.h"
#include "EffSheetWithTab.h"

#include "../../EngineLib/DxTools/TextureManager.h"
#include "../../EngineLib/DxEffect/Single/DxEffGroupPlayer.h"
#include "../../EngineLib/DxEffect/Single/DxEffectBlurSys.h"
#include "EffGroupPage.h"

#include "BlurSysPage.h"

#include "../Dialog/ColorPickerDlg.h"
#include "../Dialog/SetVar.h"
#include "../Dialog/SetMultiSlider.h"
#include ".\blursyspage.h"


// CCBlurSysPage ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CBlurSysPage, CPropertyPage)
CBlurSysPage::CBlurSysPage()
	: CPropertyPage(CBlurSysPage::IDD)
{
}

CBlurSysPage::~CBlurSysPage()
{
}

void CBlurSysPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_BLEND, m_pComboBlend);
}


BEGIN_MESSAGE_MAP(CBlurSysPage, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_PLAY, OnBnClickedButtonPlay)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnBnClickedButtonApply)
	ON_BN_CLICKED(IDC_BUTTON_OK, OnBnClickedButtonOk)
	ON_BN_CLICKED(IDC_BUTTON_TEXTURE, OnBnClickedButtonTexture)
	ON_BN_CLICKED(IDC_BUTTON_COLOR_S, OnBnClickedButtonColorS)
	ON_BN_CLICKED(IDC_BUTTONCOLOR_E, OnBnClickedButtoncolorE)
	ON_BN_CLICKED(IDC_BUTTON_SCALE, OnBnClickedButtonScale)
	ON_BN_CLICKED(IDC_BUTTON_ALPHA, OnBnClickedButtonAlpha)
	ON_BN_CLICKED(IDC_RADIO_ABSOLUTE, OnBnClickedRadioAbsolute)
	ON_BN_CLICKED(IDC_RADIO_ABSOLUTENO, OnBnClickedRadioAbsoluteno)
	ON_BN_CLICKED(IDC_CHECK_LOOP_RESET, OnBnClickedCheckLoopReset)
END_MESSAGE_MAP()


// CBlurSysPage �޽��� ó�����Դϴ�.

BOOL CBlurSysPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	m_Color.SubclassDlgItem ( IDC_STATIC_COLOR, this );

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

// CParticleSysPage �޽��� ó�����Դϴ�.
void CBlurSysPage::SetProperty ( EFF_PROPERTY* pProp )
{
	m_pProperty = (BLURSYS_PROPERTY*) pProp;

	m_Color.m_d3dStartColor = m_pProperty->m_cColorStart;
	m_Color.m_d3dEndColor = m_pProperty->m_cColorEnd;
	m_Color.RedrawWindow();

	char szValue[256];

	CWnd*	pWnd;

	// Reset~!!
	int	nIndex;
	CString	strText;
	m_pComboBlend.ResetContent();

	strText.Format ( "1.���" );
	nIndex = m_pComboBlend.AddString ( strText );
	m_pComboBlend.SetItemData ( nIndex, 1 );
	strText.Format ( "2.��� 2��" );
	nIndex = m_pComboBlend.AddString ( strText );
	m_pComboBlend.SetItemData ( nIndex, 2 );
	strText.Format ( "3.��� 4��" );
	nIndex = m_pComboBlend.AddString ( strText );
	m_pComboBlend.SetItemData ( nIndex, 3 );
	strText.Format ( "4.��Ӱ�" );
	nIndex = m_pComboBlend.AddString ( strText );
	m_pComboBlend.SetItemData ( nIndex, 4 );
	strText.Format ( "5.�⺻��" );
	nIndex = m_pComboBlend.AddString ( strText );
	m_pComboBlend.SetItemData ( nIndex, 5 );

	m_pComboBlend.SetCurSel ( m_pProperty->m_nBlend - 1 );



	CButton* pButton;
	pButton = (CButton*) GetDlgItem ( IDC_CHECK_LOOP_RESET );
	if ( m_pProperty->m_dwFlag & USE_LOOP_RESET )	pButton->SetCheck ( TRUE );
	else											pButton->SetCheck ( FALSE );

	pButton = (CButton*) GetDlgItem ( IDC_RADIO_ABSOLUTE );
	pButton->SetCheck ( m_pProperty->m_dwFlag & USEABSOLUTE );
	pButton = (CButton*) GetDlgItem ( IDC_RADIO_ABSOLUTENO );
	pButton->SetCheck ( !(m_pProperty->m_dwFlag&USEABSOLUTE) );

	pWnd = GetDlgItem ( IDC_EDIT_LIFE );
	_gcvt ( m_pProperty->m_fLife, 7, szValue );
	pWnd->SetWindowText ( szValue );

	pWnd = GetDlgItem ( IDC_EDIT_TEXTURE );
	pWnd->SetWindowText ( m_pProperty->m_szTexture );

}
void CBlurSysPage::OnBnClickedButtonPlay()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	//	Note : ���� ����.
	//
	OnBnClickedButtonApply();

	//	Note : ����Ʈ ����.
	//
	D3DXMATRIX matTrans;
	D3DXMatrixIdentity ( &matTrans );

	DxEffGroupPlayer::GetInstance().SetTime ( 0.0f );
	DxEffGroupPlayer::GetInstance().NewEffGroup ( &CEffGroupPage::m_EffPropGroup, m_pProperty, matTrans );
}

void CBlurSysPage::OnBnClickedButtonApply()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CString strText;

	int nIndex, nData;
	nIndex = m_pComboBlend.GetCurSel();
	nData = m_pComboBlend.GetItemData ( nIndex );

	m_pProperty->m_nBlend = nData;

	CWnd*	pWnd;
	pWnd = GetDlgItem ( IDC_EDIT_LIFE );
	pWnd->GetWindowText ( strText );
	m_pProperty->m_fLife = (float)atof ( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_TEXTURE );
	pWnd->GetWindowText ( strText );
	strcpy ( m_pProperty->m_szTexture, strText.GetString() );

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CEffectToolView *pView = (CEffectToolView*) pFrame->GetActiveView ();

	DxEffGroupPlayer::GetInstance().DeleteEff ( CEffGroupPage::m_EffPropGroup.m_strFileName.c_str() );
	m_pProperty->CleanUp ();
	m_pProperty->Create ( pView->GetD3dDevice(), FALSE );
}

void CBlurSysPage::OnBnClickedButtonOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CEffectToolView::m_curType = _CUR_NONE;
	m_pSheetTab->ActiveEffGroupPage ();

	//	Note : �Ӽ��� ����.
	//
	OnBnClickedButtonApply();

	//	Note : Tap ����
	//
	CEffectToolView::m_iSheetTap = 0;
}

void CBlurSysPage::OnBnClickedButtonTexture()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	CEffectToolView *pView = (CEffectToolView*) pFrame->GetActiveView ();
	pView->SetCurClick( FALSE );

	CString szFilter = "Texture Image (*.bmp,*.dds,*.tga,*.jpg)|*.bmp;*.dds;*.tga;*.jpg|";
	CFileDialog dlg ( TRUE, ".BMP", NULL, OFN_HIDEREADONLY, szFilter, this );
	dlg.m_ofn.lpstrInitialDir = TextureManager::GetInstance().GetPath().c_str();

	if ( dlg.DoModal() == IDOK )
	{
		CString FilePath = dlg.GetFileName ();

		CWnd *pWnd;
		pWnd = GetDlgItem ( IDC_EDIT_TEXTURE );
		pWnd->SetWindowText ( FilePath );
	}

	pView->SetCurClick( TRUE );	// Ŀ�� Ŭ���� �츰��.
}

void CBlurSysPage::OnBnClickedButtonAlpha()
{
	CMainFrame *pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	CEffectToolView *pView = (CEffectToolView*) pFrame->GetActiveView ();
	pView->SetCurClick( FALSE );

	CSetVar DlgSetVar;
	DlgSetVar.m_StrTitle = "Set Particle Size";
	DlgSetVar.m_dwFlag = USESTARTSCROLL | USEENDSCROLL;
	DlgSetVar.m_bZeroBase = TRUE;
	DlgSetVar.m_ToScale = 800.0f;

	DlgSetVar.m_fStart = m_pProperty->m_fAlphaStart;
	DlgSetVar.m_fEnd = m_pProperty->m_fAlphaEnd;
	
	if ( IDOK == DlgSetVar.DoModal() )
	{
		m_pProperty->m_fAlphaStart = DlgSetVar.m_fStart;
		m_pProperty->m_fAlphaEnd = DlgSetVar.m_fEnd;
	}

	pView->SetCurClick( TRUE );	// Ŀ�� Ŭ���� �츰��.
}

void CBlurSysPage::OnBnClickedButtonColorS()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	CEffectToolView *pView = (CEffectToolView*) pFrame->GetActiveView ();
	pView->SetCurClick( FALSE );

	int ThisR = int ( m_pProperty->m_cColorStart.r * 255.0f );
	int ThisG = int ( m_pProperty->m_cColorStart.g * 255.0f );
	int ThisB = int ( m_pProperty->m_cColorStart.b * 255.0f );
	CColorPickerDlg ColorPicker ( RGB(ThisR,ThisG,ThisB) );
	
	if ( IDOK == ColorPicker.DoModal() )
	{
		COLORREF SelectColor = ColorPicker.GetColor ();
		m_pProperty->m_cColorStart.r = GetRValue(SelectColor)/255.0f;
		m_pProperty->m_cColorStart.g = GetGValue(SelectColor)/255.0f;
		m_pProperty->m_cColorStart.b = GetBValue(SelectColor)/255.0f;
	}

	m_Color.m_d3dStartColor = m_pProperty->m_cColorStart;
	m_Color.m_d3dEndColor = m_pProperty->m_cColorEnd;
	m_Color.RedrawWindow();

	pView->SetCurClick( TRUE );	// Ŀ�� Ŭ���� �츰��.
}

void CBlurSysPage::OnBnClickedButtoncolorE()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	CEffectToolView *pView = (CEffectToolView*) pFrame->GetActiveView ();
	pView->SetCurClick( FALSE );

	int ThisR = int ( m_pProperty->m_cColorEnd.r * 255.0f );
	int ThisG = int ( m_pProperty->m_cColorEnd.g * 255.0f );
	int ThisB = int ( m_pProperty->m_cColorEnd.b * 255.0f );
	CColorPickerDlg ColorPicker ( RGB(ThisR,ThisG,ThisB) );
	
	if ( IDOK == ColorPicker.DoModal() )
	{
		COLORREF SelectColor = ColorPicker.GetColor ();
		m_pProperty->m_cColorEnd.r = GetRValue(SelectColor)/255.0f;
		m_pProperty->m_cColorEnd.g = GetGValue(SelectColor)/255.0f;
		m_pProperty->m_cColorEnd.b = GetBValue(SelectColor)/255.0f;
	}

	m_Color.m_d3dStartColor = m_pProperty->m_cColorStart;
	m_Color.m_d3dEndColor = m_pProperty->m_cColorEnd;
	m_Color.RedrawWindow();

	pView->SetCurClick( TRUE );	// Ŀ�� Ŭ���� �츰��.
}

void CBlurSysPage::OnBnClickedButtonScale()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	CEffectToolView *pView = (CEffectToolView*) pFrame->GetActiveView ();
	pView->SetCurClick( FALSE );

	CSetVar DlgSetVar;
	DlgSetVar.m_StrTitle = "Set Particle Size";
	DlgSetVar.m_dwFlag = USESTARTSCROLL | USEENDSCROLL;
	DlgSetVar.m_bZeroBase = TRUE;
	DlgSetVar.m_ToScale = 40.0f;

	DlgSetVar.m_fStart = m_pProperty->m_fLengthStart;
	DlgSetVar.m_fEnd = m_pProperty->m_fLengthEnd;
	
	if ( IDOK == DlgSetVar.DoModal() )
	{
		m_pProperty->m_fLengthStart = DlgSetVar.m_fStart;
		m_pProperty->m_fLengthEnd = DlgSetVar.m_fEnd;
	}

	pView->SetCurClick( TRUE );	// Ŀ�� Ŭ���� �츰��.
}

void CBlurSysPage::OnBnClickedRadioAbsolute()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CButton* pButton;
	pButton = (CButton*) GetDlgItem ( IDC_RADIO_ABSOLUTE );
	pButton->SetCheck ( TRUE );
	pButton = (CButton*) GetDlgItem ( IDC_RADIO_ABSOLUTENO );
	pButton->SetCheck ( FALSE );

	m_pProperty->m_dwFlag |= USEABSOLUTE;
}

void CBlurSysPage::OnBnClickedRadioAbsoluteno()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CButton* pButton;
	pButton = (CButton*) GetDlgItem ( IDC_RADIO_ABSOLUTE );
	pButton->SetCheck ( FALSE );
	pButton = (CButton*) GetDlgItem ( IDC_RADIO_ABSOLUTENO );
	pButton->SetCheck ( TRUE );

	m_pProperty->m_dwFlag &= ~USEABSOLUTE;
}
void CBlurSysPage::OnBnClickedCheckLoopReset()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CButton* pButton;

	pButton = (CButton*) GetDlgItem ( IDC_CHECK_LOOP_RESET );
	if ( pButton->GetCheck() )		m_pProperty->m_dwFlag |= USE_LOOP_RESET;
	else							m_pProperty->m_dwFlag &= ~USE_LOOP_RESET;
}
