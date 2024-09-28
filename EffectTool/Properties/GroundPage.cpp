// GroundPage.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "../EffectTool.h"
#include "../MainFrm.h"

#include "../EffectToolDoc.h"
#include "../EffectToolView.h"
#include "EffSheetWithTab.h"

#include "../../EngineLib/DxTools/TextureManager.h"
#include "../../EngineLib/DxEffect/Single/DxEffGroupPlayer.h"
#include "../../EngineLib/DxEffect/Single/DxEffectGround.h"
#include "EffGroupPage.h"

#include "GroundPage.h"

#include "../Dialog/ColorPickerDlg.h"
#include "../Dialog/SetVar.h"
#include "../Dialog/SetMultiSlider.h"
#include ".\groundpage.h"


// CGroundPage 대화 상자입니다.

IMPLEMENT_DYNAMIC(CGroundPage, CPropertyPage)
CGroundPage::CGroundPage()
	: CPropertyPage(CGroundPage::IDD)
{
}

CGroundPage::~CGroundPage()
{
}

void CGroundPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_BLEND, m_pComboBlend);
}


BEGIN_MESSAGE_MAP(CGroundPage, CPropertyPage)
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
	ON_BN_CLICKED(IDC_BUTTON_HEIGHT, OnBnClickedButtonHeight)
	ON_BN_CLICKED(IDC_BUTTON_EDITSOUND, OnBnClickedButtonEditsound)
	ON_BN_CLICKED(IDC_CHECK_NORMAL, OnBnClickedCheckNormal)
	ON_BN_CLICKED(IDC_CHECK_PICKING, OnBnClickedCheckPicking)
END_MESSAGE_MAP()


// CGroundPage 메시지 처리기입니다.

BOOL CGroundPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_Color.SubclassDlgItem ( IDC_STATIC_COLOR, this );

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

// CParticleSysPage 메시지 처리기입니다.
void CGroundPage::SetProperty ( EFF_PROPERTY* pProp )
{
	m_pProperty = (GROUND_PROPERTY*) pProp;

	m_Color.m_d3dStartColor = m_pProperty->m_cColorStart;
	m_Color.m_d3dEndColor = m_pProperty->m_cColorEnd;
	m_Color.RedrawWindow();

	CWnd*		pWnd;
	CButton*	pButton;

	// Reset~!!
	int	nIndex;
	CString	strText;
	m_pComboBlend.ResetContent();

	strText.Format ( "1.밝게" );
	nIndex = m_pComboBlend.AddString ( strText );
	m_pComboBlend.SetItemData ( nIndex, 1 );
	strText.Format ( "2.밝게 2배" );
	nIndex = m_pComboBlend.AddString ( strText );
	m_pComboBlend.SetItemData ( nIndex, 2 );
	strText.Format ( "3.밝게 4배" );
	nIndex = m_pComboBlend.AddString ( strText );
	m_pComboBlend.SetItemData ( nIndex, 3 );
	strText.Format ( "4.어둡게" );
	nIndex = m_pComboBlend.AddString ( strText );
	m_pComboBlend.SetItemData ( nIndex, 4 );
	strText.Format ( "5.기본값" );
	nIndex = m_pComboBlend.AddString ( strText );
	m_pComboBlend.SetItemData ( nIndex, 5 );

	m_pComboBlend.SetCurSel ( m_pProperty->m_nBlend - 1 );

	pButton = (CButton*) GetDlgItem ( IDC_CHECK_NORMAL );
	if ( m_pProperty->m_dwFlag & USENORMAL )	pButton->SetCheck ( TRUE );
	else										pButton->SetCheck ( FALSE );

	pButton = (CButton*) GetDlgItem ( IDC_CHECK_PICKING );
	if ( m_pProperty->m_dwFlag & USEPICKING )	pButton->SetCheck ( TRUE );
	else										pButton->SetCheck ( FALSE );

	pWnd = GetDlgItem ( IDC_BUTTON_ROTATE );
	if ( m_pProperty->m_dwFlag & USEROTATE )	pWnd->SetWindowText ( "회전 설정 O" );
	else										pWnd->SetWindowText ( "회전 설정 X" );

	pWnd = GetDlgItem ( IDC_BUTTON_ANI );
	if ( m_pProperty->m_dwFlag & USEANI )		pWnd->SetWindowText ( "시퀀스 설정 O" );
	else										pWnd->SetWindowText ( "시퀀스 설정 X" );

	pWnd = GetDlgItem ( IDC_BUTTON_HEIGHT );
	if ( m_pProperty->m_dwFlag & USEHEIGHT )	pWnd->SetWindowText ( "높이 변화 O" );
	else										pWnd->SetWindowText ( "높이 변화 X" );

	pWnd = GetDlgItem ( IDC_EDIT_TEXTURE );
	pWnd->SetWindowText ( m_pProperty->m_szTexture );

}
void CGroundPage::OnBnClickedButtonPlay()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//	Note : 설정 적용.
	//
	OnBnClickedButtonApply();

	//	Note : 이펙트 생성.
	//
	D3DXMATRIX matTrans;
	D3DXMatrixIdentity ( &matTrans );

	DxEffGroupPlayer::GetInstance().SetTime ( 0.0f );
	//DxEffGroupPlayer::GetInstance().NewEffGroup ( &CEffGroupPage::m_EffPropGroup, m_pProperty, matTrans );

	DxEffGroupPlayer::GetInstance().NewEffGroup ( &CEffGroupPage::m_EffPropGroup, m_pProperty, matTrans );
}

void CGroundPage::OnBnClickedButtonApply()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
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

void CGroundPage::OnBnClickedButtonOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CEffectToolView::m_curType = _CUR_NONE;
	m_pSheetTab->ActiveEffGroupPage ();

	//	Note : 속성을 적용.
	//
	OnBnClickedButtonApply();

	//	Note : Tap 적용
	//
	CEffectToolView::m_iSheetTap = 0;
}

void CGroundPage::OnBnClickedButtonTexture()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
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

	pView->SetCurClick( TRUE );	// 커서 클릭을 살린다.
}

void CGroundPage::OnCbnSelchangeComboBlend()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nIndex, nData;
	nIndex = m_pComboBlend.GetCurSel();
	nData = m_pComboBlend.GetItemData ( nIndex );

	m_pProperty->m_nBlend = nData;
}

void CGroundPage::OnBnClickedButtonScale()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	CEffectToolView *pView = (CEffectToolView*) pFrame->GetActiveView ();
	pView->SetCurClick( FALSE );

	CSetMultiSlider DlgSetSlider;
	DlgSetSlider.m_StrTitle = "Set Scale";
	DlgSetSlider.m_StrTemp4 = "스케일";

	DlgSetSlider.m_values[0] = m_pProperty->m_fSizeRate1;
	DlgSetSlider.m_values[1] = m_pProperty->m_fSizeRate2;

	DlgSetSlider.m_fStart = m_pProperty->m_fSizeStart;
	DlgSetSlider.m_fMid1 = m_pProperty->m_fSizeMid1;
	DlgSetSlider.m_fMid2 = m_pProperty->m_fSizeMid2;
	DlgSetSlider.m_fEnd = m_pProperty->m_fSizeEnd;
	
	if ( IDOK == DlgSetSlider.DoModal() )
	{
		m_pProperty->m_fSizeRate1 = DlgSetSlider.m_values[0];
		m_pProperty->m_fSizeRate2 = DlgSetSlider.m_values[1];
	
		m_pProperty->m_fSizeStart = DlgSetSlider.m_fStart;
		m_pProperty->m_fSizeMid1 = DlgSetSlider.m_fMid1;
		m_pProperty->m_fSizeMid2 = DlgSetSlider.m_fMid2;
		m_pProperty->m_fSizeEnd = DlgSetSlider.m_fEnd;
	}

	pView->SetCurClick( TRUE );	// 커서 클릭을 살린다.
}

void CGroundPage::OnBnClickedButtonAlpha()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	CEffectToolView *pView = (CEffectToolView*) pFrame->GetActiveView ();
	pView->SetCurClick( FALSE );

	CSetMultiSlider DlgSetSlider;
	DlgSetSlider.m_StrTitle = "Set Alpha";
	DlgSetSlider.m_StrTemp4 = "알파값";

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

	pView->SetCurClick( TRUE );	// 커서 클릭을 살린다.
}

void CGroundPage::OnBnClickedButtonColorS()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
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

	pView->SetCurClick( TRUE );	// 커서 클릭을 살린다.
}

void CGroundPage::OnBnClickedButtonColorV()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
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

	pView->SetCurClick( TRUE );	// 커서 클릭을 살린다.
}

void CGroundPage::OnBnClickedButtoncolorE()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
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

	pView->SetCurClick( TRUE );	// 커서 클릭을 살린다.
}

void CGroundPage::OnBnClickedButtonRotate()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	CEffectToolView *pView = (CEffectToolView*) pFrame->GetActiveView ();
	pView->SetCurClick( FALSE );

	CSetVar DlgSetVar;
	DlgSetVar.m_StrTitle = "Set Rotation";
	DlgSetVar.m_StrUse = "회전 사용";
	DlgSetVar.m_StrComm1 = "회전 속도";
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
		if ( m_pProperty->m_dwFlag & USEROTATE )	pWnd->SetWindowText ( "회전 설정 O" );
		else										pWnd->SetWindowText ( "회전 설정 X" );
	}

	pView->SetCurClick( TRUE );	// 커서 클릭을 살린다.
}

void CGroundPage::OnBnClickedButtonAni()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	CEffectToolView *pView = (CEffectToolView*) pFrame->GetActiveView ();
	pView->SetCurClick( FALSE );

	CSetVar DlgSetVar;
	DlgSetVar.m_StrTitle = "Set Animation";
	DlgSetVar.m_StrUse = "시퀀스사용";
	DlgSetVar.m_StrComm1 = "초";
	DlgSetVar.m_StrComm2 = "행";
	DlgSetVar.m_StrComm3 = "열";
	DlgSetVar.m_StrRadio1 = "단방향";
	DlgSetVar.m_StrRadio2 = "양방향";
	DlgSetVar.m_dwFlag = USECHECK | USERADIO | USESTARTSCROLL | USESTARTVARSCROLL | USEENDSCROLL;
	DlgSetVar.m_bZeroBase = TRUE;
	DlgSetVar.m_ToScale = 40.0f;

	DlgSetVar.m_bUse = ((m_pProperty->m_dwFlag & USEANI) ? TRUE : FALSE);
	DlgSetVar.m_iRadio = ((m_pProperty->m_dwFlag & USESEQUENCELOOP) ? 1 : 0 );
	DlgSetVar.m_fStart = m_pProperty->m_fAniTime;
	DlgSetVar.m_fStartVar = (float)m_pProperty->m_iCol;
	DlgSetVar.m_fEnd = (float)m_pProperty->m_iRow;
	
	if ( IDOK == DlgSetVar.DoModal() )
	{
		m_pProperty->m_dwFlag |= USEANI;
		if ( !DlgSetVar.m_bUse )	m_pProperty->m_dwFlag &= ~USEANI;

		if ( DlgSetVar.m_iRadio == 0 )			m_pProperty->m_dwFlag &= ~USESEQUENCELOOP;
		else if ( DlgSetVar.m_iRadio == 1 )		m_pProperty->m_dwFlag |= USESEQUENCELOOP;

		m_pProperty->m_fAniTime = DlgSetVar.m_fStart;
		m_pProperty->m_iCol = (int)DlgSetVar.m_fStartVar;
		m_pProperty->m_iRow = (int)DlgSetVar.m_fEnd;

		CWnd *pWnd;
		pWnd = GetDlgItem ( IDC_BUTTON_ANI );
		if ( m_pProperty->m_dwFlag & USEANI )		pWnd->SetWindowText ( "시퀀스 설정 O" );
		else										pWnd->SetWindowText ( "시퀀스 설정 X" );
	}

	pView->SetCurClick( TRUE );	// 커서 클릭을 살린다.
}

void CGroundPage::OnBnClickedButtonHeight()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	CEffectToolView *pView = (CEffectToolView*) pFrame->GetActiveView ();
	pView->SetCurClick( FALSE );

	CSetMultiSlider DlgSetSlider;
	DlgSetSlider.m_StrTitle = "Set Height";
	DlgSetSlider.m_StrUse = "높이 변화";
	DlgSetSlider.m_StrTemp4 = "높이";
	DlgSetSlider.m_dwFlag = USECHECK;

	DlgSetSlider.m_values[0] = m_pProperty->m_fHeightRate1;
	DlgSetSlider.m_values[1] = m_pProperty->m_fHeightRate2;

	DlgSetSlider.m_bUse = ((m_pProperty->m_dwFlag & USEHEIGHT) ? TRUE : FALSE);
	DlgSetSlider.m_fStart = m_pProperty->m_fHeightStart;
	DlgSetSlider.m_fMid1 = m_pProperty->m_fHeightMid1;
	DlgSetSlider.m_fMid2 = m_pProperty->m_fHeightMid2;
	DlgSetSlider.m_fEnd = m_pProperty->m_fHeightEnd;
	
	if ( IDOK == DlgSetSlider.DoModal() )
	{
		m_pProperty->m_fHeightRate1 = DlgSetSlider.m_values[0];
		m_pProperty->m_fHeightRate2 = DlgSetSlider.m_values[1];
	
		m_pProperty->m_dwFlag |= USEHEIGHT;
		if ( !DlgSetSlider.m_bUse )	m_pProperty->m_dwFlag &= ~USEHEIGHT;

		m_pProperty->m_fHeightStart = DlgSetSlider.m_fStart;
		m_pProperty->m_fHeightMid1 = DlgSetSlider.m_fMid1;
		m_pProperty->m_fHeightMid2 = DlgSetSlider.m_fMid2;
		m_pProperty->m_fHeightEnd = DlgSetSlider.m_fEnd;

		CWnd *pWnd;
		pWnd = GetDlgItem ( IDC_BUTTON_HEIGHT );
		if ( m_pProperty->m_dwFlag & USEHEIGHT )	pWnd->SetWindowText ( "높이 변화 O" );
		else										pWnd->SetWindowText ( "높이 변화 X" );
	}

	pView->SetCurClick( TRUE );	// 커서 클릭을 살린다.
}

void CGroundPage::OnBnClickedButtonEditsound()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pSheetTab->ActiveEffSoundPage ( &m_pProperty->m_MovSound, GROUNDPAGEINDEX );
}

void CGroundPage::OnBnClickedCheckNormal()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CButton*	pButton;

	pButton = (CButton*) GetDlgItem ( IDC_CHECK_NORMAL );
	if ( pButton->GetCheck () )		m_pProperty->m_dwFlag |= USENORMAL;
	else							m_pProperty->m_dwFlag &= ~USENORMAL;
}

void CGroundPage::OnBnClickedCheckPicking()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CButton*	pButton;

	pButton = (CButton*) GetDlgItem ( IDC_CHECK_PICKING );
	if ( pButton->GetCheck () )		m_pProperty->m_dwFlag |= USEPICKING;
	else							m_pProperty->m_dwFlag &= ~USEPICKING;
}
