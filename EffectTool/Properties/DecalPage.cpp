// DecalPage.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "../EffectTool.h"
#include "../MainFrm.h"

#include "../EffectToolDoc.h"
#include "../EffectToolView.h"
#include "EffSheetWithTab.h"

#include "../../EngineLib/DxTools/TextureManager.h"
#include "../../EngineLib/DxEffect/Single/DxEffGroupPlayer.h"
#include "EffGroupPage.h"

#include "../../EngineLib/DxEffect/Single/DxEffectDecal.h"
#include "DecalPage.h"

#include "../Dialog/ColorPickerDlg.h"
#include "../Dialog/SetVar.h"
#include "../Dialog/SetMultiSlider.h"


// CDecalPage ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CDecalPage, CPropertyPage)
CDecalPage::CDecalPage()
	: CPropertyPage(CDecalPage::IDD)
{
}

CDecalPage::~CDecalPage()
{
}

void CDecalPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_BLEND, m_pComboBlend);
}


BEGIN_MESSAGE_MAP(CDecalPage, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_PLAY, OnBnClickedButtonPlay)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnBnClickedButtonApply)
	ON_BN_CLICKED(IDC_BUTTON_OK, OnBnClickedButtonOk)
	ON_CBN_SELCHANGE(IDC_COMBO_BLEND, OnCbnSelchangeComboBlend)
	ON_BN_CLICKED(IDC_BUTTON_SCALE, OnBnClickedButtonScale)
	ON_BN_CLICKED(IDC_BUTTON_ALPHA, OnBnClickedButtonAlpha)
	ON_BN_CLICKED(IDC_BUTTON_COLOR_S, OnBnClickedButtonColorS)
	ON_BN_CLICKED(IDC_BUTTON_COLOR_V, OnBnClickedButtonColorV)
	ON_BN_CLICKED(IDC_BUTTONCOLOR_E, OnBnClickedButtoncolorE)
	ON_BN_CLICKED(IDC_BUTTON_TEXTURE, OnBnClickedButtonTexture)
	ON_BN_CLICKED(IDC_BUTTON_ROTATE, OnBnClickedButtonRotate)
	ON_BN_CLICKED(IDC_BUTTON_ANI, OnBnClickedButtonAni)
	ON_BN_CLICKED(IDC_BUTTON_AREA, OnBnClickedButtonArea)
	ON_BN_CLICKED(IDC_BUTTON_EDITSOUND, OnBnClickedButtonEditsound)
END_MESSAGE_MAP()


// CDecalPage �޽��� ó�����Դϴ�.

BOOL CDecalPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	m_Color.SubclassDlgItem ( IDC_STATIC_COLOR, this );

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

// CDecalPage �޽��� ó�����Դϴ�.
void CDecalPage::SetProperty ( EFF_PROPERTY* pProp )
{
	m_pProperty = (DECAL_PROPERTY*) pProp;

	m_Color.m_d3dStartColor = m_pProperty->m_cColorStart;
	m_Color.m_d3dEndColor = m_pProperty->m_cColorEnd;
	m_Color.RedrawWindow();

	CWnd*		pWnd;

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

	pWnd = GetDlgItem ( IDC_BUTTON_ROTATE );
	if ( m_pProperty->m_dwFlag & USEROTATE )	pWnd->SetWindowText ( "�ؽ��� ȸ�� O" );
	else										pWnd->SetWindowText ( "�ؽ��� ȸ�� X" );

	pWnd = GetDlgItem ( IDC_BUTTON_ANI );
	if ( m_pProperty->m_dwFlag & USESEQUENCE )	pWnd->SetWindowText ( "������ ���� O" );
	else										pWnd->SetWindowText ( "������ ���� X" );

	pWnd = GetDlgItem ( IDC_BUTTON_SCALE );
	if ( m_pProperty->m_dwFlag & USETEXSCALE )	pWnd->SetWindowText ( "�ؽ��� ũ�� O" );
	else										pWnd->SetWindowText ( "�ؽ��� ũ�� X" );

	pWnd = GetDlgItem ( IDC_EDIT_TEXTURE );
	pWnd->SetWindowText ( m_pProperty->m_szTexture );

}
void CDecalPage::OnBnClickedButtonPlay()
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

void CDecalPage::OnBnClickedButtonApply()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CString strText;

	int nIndex, nData;
	nIndex = m_pComboBlend.GetCurSel();
	nData = m_pComboBlend.GetItemData ( nIndex );

	m_pProperty->m_nBlend = nData;


	CWnd *pWnd;

	pWnd = GetDlgItem ( IDC_EDIT_TEXTURE );
	pWnd->GetWindowText ( strText );
	strcpy ( m_pProperty->m_szTexture, strText.GetString() );

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CEffectToolView *pView = (CEffectToolView*) pFrame->GetActiveView ();

	DxEffGroupPlayer::GetInstance().DeleteEff ( CEffGroupPage::m_EffPropGroup.m_strFileName.c_str() );
	m_pProperty->CleanUp ();
	m_pProperty->Create ( pView->GetD3dDevice(), FALSE );
}

void CDecalPage::OnBnClickedButtonOk()
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

void CDecalPage::OnBnClickedButtonTexture()
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

void CDecalPage::OnCbnSelchangeComboBlend()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	int nIndex, nData;
	nIndex = m_pComboBlend.GetCurSel();
	nData = m_pComboBlend.GetItemData ( nIndex );

	m_pProperty->m_nBlend = nData;
}

void CDecalPage::OnBnClickedButtonAlpha()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	CEffectToolView *pView = (CEffectToolView*) pFrame->GetActiveView ();
	pView->SetCurClick( FALSE );

	CSetMultiSlider DlgSetSlider;
	DlgSetSlider.m_StrTitle = "Set Alpha";
	DlgSetSlider.m_StrTemp4 = "���İ�";

	DlgSetSlider.m_values[0] = m_pProperty->m_fAlphaRate1;
	DlgSetSlider.m_values[1] = m_pProperty->m_fAlphaRate2;

	DlgSetSlider.m_fStart = m_pProperty->m_fAlphaStart;
	DlgSetSlider.m_fMid1 = m_pProperty->m_fAlphaMid1;
	DlgSetSlider.m_fMid2 = m_pProperty->m_fAlphaMid2;
	DlgSetSlider.m_fEnd = m_pProperty->m_fAlphaEnd;
	
	if ( IDOK == DlgSetSlider.DoModal() )
	{
		m_pProperty->m_fAlphaRate1 = DlgSetSlider.m_values[0];
		m_pProperty->m_fAlphaRate2 = DlgSetSlider.m_values[1];
	
		m_pProperty->m_fAlphaStart = DlgSetSlider.m_fStart;
		m_pProperty->m_fAlphaMid1 = DlgSetSlider.m_fMid1;
		m_pProperty->m_fAlphaMid2 = DlgSetSlider.m_fMid2;
		m_pProperty->m_fAlphaEnd = DlgSetSlider.m_fEnd;
	}

	pView->SetCurClick( TRUE );	// Ŀ�� Ŭ���� �츰��.
}

void CDecalPage::OnBnClickedButtonColorS()
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

void CDecalPage::OnBnClickedButtonColorV()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	CEffectToolView *pView = (CEffectToolView*) pFrame->GetActiveView ();
	pView->SetCurClick( FALSE );

	CSetVar DlgSetVar;
	DlgSetVar.m_StrTitle = "Set Particle Color Variation";
	DlgSetVar.m_StrComm1 = "R";
	DlgSetVar.m_StrComm2 = "G";
	DlgSetVar.m_StrComm3 = "B";
	
	DlgSetVar.m_dwFlag = USESTARTSCROLL | USESTARTVARSCROLL | USEENDSCROLL;
	DlgSetVar.m_bZeroBase = TRUE;
	DlgSetVar.m_ToScale = 800.0f;

	DlgSetVar.m_fStart = m_pProperty->m_cColorVar.r;
	DlgSetVar.m_fStartVar = m_pProperty->m_cColorVar.g;
	DlgSetVar.m_fEnd = m_pProperty->m_cColorVar.b;
	
	if ( IDOK == DlgSetVar.DoModal() )
	{
		m_pProperty->m_cColorVar.r = DlgSetVar.m_fStart;
		m_pProperty->m_cColorVar.g = DlgSetVar.m_fStartVar;
		m_pProperty->m_cColorVar.b = DlgSetVar.m_fEnd;
	}

	pView->SetCurClick( TRUE );	// Ŀ�� Ŭ���� �츰��.
}

void CDecalPage::OnBnClickedButtoncolorE()
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

void CDecalPage::OnBnClickedButtonScale()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	CEffectToolView *pView = (CEffectToolView*) pFrame->GetActiveView ();
	pView->SetCurClick( FALSE );

	CSetMultiSlider DlgSetSlider;
	DlgSetSlider.m_StrTitle = "Set Scale";
	DlgSetSlider.m_StrUse	= "ũ�� ���";
	DlgSetSlider.m_StrTemp4 = "������";
	DlgSetSlider.m_dwFlag = USECHECK;

	DlgSetSlider.m_values[0] = m_pProperty->m_fSizeRate1;
	DlgSetSlider.m_values[1] = m_pProperty->m_fSizeRate2;

	DlgSetSlider.m_bUse = ((m_pProperty->m_dwFlag & USETEXSCALE) ? TRUE : FALSE);

	DlgSetSlider.m_fStart = m_pProperty->m_fSizeStart;
	DlgSetSlider.m_fMid1 = m_pProperty->m_fSizeMid1;
	DlgSetSlider.m_fMid2 = m_pProperty->m_fSizeMid2;
	DlgSetSlider.m_fEnd = m_pProperty->m_fSizeEnd;
	
	if ( IDOK == DlgSetSlider.DoModal() )
	{
		m_pProperty->m_fSizeRate1 = DlgSetSlider.m_values[0];
		m_pProperty->m_fSizeRate2 = DlgSetSlider.m_values[1];
	
		m_pProperty->m_dwFlag |= USETEXSCALE;
		if ( !DlgSetSlider.m_bUse )	m_pProperty->m_dwFlag &= ~USETEXSCALE;

		m_pProperty->m_fSizeStart = DlgSetSlider.m_fStart;
		m_pProperty->m_fSizeMid1 = DlgSetSlider.m_fMid1;
		m_pProperty->m_fSizeMid2 = DlgSetSlider.m_fMid2;
		m_pProperty->m_fSizeEnd = DlgSetSlider.m_fEnd;

		CWnd *pWnd;
		pWnd = GetDlgItem ( IDC_BUTTON_SCALE );
		if ( m_pProperty->m_dwFlag & USETEXSCALE )	pWnd->SetWindowText ( "�ؽ��� ũ�� O" );
		else										pWnd->SetWindowText ( "�ؽ��� ũ�� X" );
	}

	pView->SetCurClick( TRUE );	// Ŀ�� Ŭ���� �츰��.
}

void CDecalPage::OnBnClickedButtonRotate()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	CEffectToolView *pView = (CEffectToolView*) pFrame->GetActiveView ();
	pView->SetCurClick( FALSE );

	CSetVar DlgSetVar;
	DlgSetVar.m_StrTitle = "Set Rotation";
	DlgSetVar.m_StrUse = "ȸ�� ���";
	DlgSetVar.m_StrComm1 = "ȸ�� �ӵ�";
	DlgSetVar.m_dwFlag = USECHECK | USESTARTSCROLL;
	DlgSetVar.m_bZeroBase = TRUE;
	DlgSetVar.m_ToScale = 10.0f;

	DlgSetVar.m_bUse = ((m_pProperty->m_dwFlag & USEROTATE) ? TRUE : FALSE);
	DlgSetVar.m_fStart = m_pProperty->m_fRotateAngel;
	
	if ( IDOK == DlgSetVar.DoModal() )
	{
		m_pProperty->m_dwFlag |= USEROTATE;
		if ( !DlgSetVar.m_bUse )	m_pProperty->m_dwFlag &= ~USEROTATE;

		m_pProperty->m_fRotateAngel = DlgSetVar.m_fStart;

		CWnd *pWnd;
		pWnd = GetDlgItem ( IDC_BUTTON_ROTATE );
		if ( m_pProperty->m_dwFlag & USEROTATE )	pWnd->SetWindowText ( "ȸ�� ���� O" );
		else										pWnd->SetWindowText ( "ȸ�� ���� X" );
	}

	pView->SetCurClick( TRUE );	// Ŀ�� Ŭ���� �츰��.
}

void CDecalPage::OnBnClickedButtonAni()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	CEffectToolView *pView = (CEffectToolView*) pFrame->GetActiveView ();
	pView->SetCurClick( FALSE );

	CSetVar DlgSetVar;
	DlgSetVar.m_StrTitle = "Set Animation";
	DlgSetVar.m_StrUse = "���������";
	DlgSetVar.m_StrComm1 = "��";
	DlgSetVar.m_StrComm2 = "��";
	DlgSetVar.m_StrComm3 = "��";
	DlgSetVar.m_StrRadio1 = "�ܹ���";
	DlgSetVar.m_StrRadio2 = "�����";
	DlgSetVar.m_dwFlag = USECHECK | USERADIO | USESTARTSCROLL | USESTARTVARSCROLL | USEENDSCROLL;
	DlgSetVar.m_bZeroBase = TRUE;
	DlgSetVar.m_ToScale = 40.0f;

	DlgSetVar.m_bUse = ((m_pProperty->m_dwFlag & USESEQUENCE) ? TRUE : FALSE);
	DlgSetVar.m_iRadio = ((m_pProperty->m_dwFlag & USESEQUENCELOOP) ? 1 : 0 );
	DlgSetVar.m_fStart = m_pProperty->m_fAniTime;
	DlgSetVar.m_fStartVar = (float)m_pProperty->m_iCol;
	DlgSetVar.m_fEnd = (float)m_pProperty->m_iRow;
	
	if ( IDOK == DlgSetVar.DoModal() )
	{
		m_pProperty->m_dwFlag |= USESEQUENCE;
		if ( !DlgSetVar.m_bUse )	m_pProperty->m_dwFlag &= ~USESEQUENCE;

		if ( DlgSetVar.m_iRadio == 0 )			m_pProperty->m_dwFlag &= ~USESEQUENCELOOP;
		else if ( DlgSetVar.m_iRadio == 1 )		m_pProperty->m_dwFlag |= USESEQUENCELOOP;

		m_pProperty->m_fAniTime = DlgSetVar.m_fStart;
		m_pProperty->m_iCol = (int)DlgSetVar.m_fStartVar;
		m_pProperty->m_iRow = (int)DlgSetVar.m_fEnd;

		CWnd *pWnd;
		pWnd = GetDlgItem ( IDC_BUTTON_ANI );
		if ( m_pProperty->m_dwFlag & USESEQUENCE )	pWnd->SetWindowText ( "������ ���� O" );
		else										pWnd->SetWindowText ( "������ ���� X" );
	}

	pView->SetCurClick( TRUE );	// Ŀ�� Ŭ���� �츰��.
}

void CDecalPage::OnBnClickedButtonArea()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	CEffectToolView *pView = (CEffectToolView*) pFrame->GetActiveView ();
	pView->SetCurClick( FALSE );

	CSetVar DlgSetVar;
	DlgSetVar.m_StrTitle = "Set Decal Area";
	DlgSetVar.m_StrComm1 = "X ��";
	DlgSetVar.m_StrComm2 = "Y ��";
	DlgSetVar.m_StrComm3 = "Z ��";
	DlgSetVar.m_bZeroBase = TRUE;

	DlgSetVar.m_dwFlag = USESTARTSCROLL | USESTARTVARSCROLL | USEENDSCROLL;

	DlgSetVar.m_fStart = m_pProperty->m_vArea.x;
	DlgSetVar.m_fStartVar = m_pProperty->m_vArea.y;
	DlgSetVar.m_fEnd = m_pProperty->m_vArea.z;
	
	if ( IDOK == DlgSetVar.DoModal() )
	{
		m_pProperty->m_vArea.x = DlgSetVar.m_fStart;
		m_pProperty->m_vArea.y = DlgSetVar.m_fStartVar;
		m_pProperty->m_vArea.z = DlgSetVar.m_fEnd;
	}
	pView->SetCurClick( TRUE );	// Ŀ�� Ŭ���� �츰��.
}

void CDecalPage::OnBnClickedButtonEditsound()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_pSheetTab->ActiveEffSoundPage ( &m_pProperty->m_MovSound, DECALINDEX );
}
