// CLightingPage.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "../EffectTool.h"
#include "../MainFrm.h"

#include "../EffectToolDoc.h"
#include "../EffectToolView.h"
#include "EffSheetWithTab.h"

#include "../../EngineLib/DxEffect/Single/DxEffGroupPlayer.h"
#include "EffGroupPage.h"

#include "../../EngineLib/DxTools/TextureManager.h"
#include "../../EngineLib/DxEffect/Single/DxEffectLighting.h"
#include "LightingPage.h"

#include "../Dialog/ColorPickerDlg.h"
#include "../Dialog/SetVar.h"
#include "../Dialog/SetMultiSlider.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

// CLightingPage 대화 상자입니다.

IMPLEMENT_DYNAMIC(CLightingPage, CPropertyPage)
CLightingPage::CLightingPage()
	: CPropertyPage(CLightingPage::IDD)
{
}

CLightingPage::~CLightingPage()
{
}

void CLightingPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_BLEND, m_pComboBlend);
}


BEGIN_MESSAGE_MAP(CLightingPage, CPropertyPage)
	ON_CBN_SELCHANGE(IDC_COMBO_BLEND, OnCbnSelchangeComboBlend)
	ON_BN_CLICKED(IDC_BUTTON_ALPHA, OnBnClickedButtonAlpha)
	ON_BN_CLICKED(IDC_BUTTON_COLOR_S, OnBnClickedButtonColorS)
	ON_BN_CLICKED(IDC_BUTTON_COLOR_E, OnBnClickedButtonColorE)
	ON_BN_CLICKED(IDC_BUTTON_EDITSOUND, OnBnClickedButtonEditsound)
	ON_BN_CLICKED(IDC_BUTTON_PLAY, OnBnClickedButtonPlay)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnBnClickedButtonApply)
	ON_BN_CLICKED(IDC_BUTTON_OK, OnBnClickedButtonOk)
	ON_BN_CLICKED(IDC_CHECK_IN, OnBnClickedCheckIn)
	ON_BN_CLICKED(IDC_CHECK_OUT, OnBnClickedCheckOut)
	ON_BN_CLICKED(IDC_CHECK_RANROTATE, OnBnClickedCheckRanrotate)
	ON_BN_CLICKED(IDC_BUTTON_TEXTURE_IN, OnBnClickedButtonTextureIn)
	ON_BN_CLICKED(IDC_BUTTON_TEXTURE_OUT, OnBnClickedButtonTextureOut)
	ON_BN_CLICKED(IDC_CHECK_START, OnBnClickedCheckStart)
	ON_BN_CLICKED(IDC_CHECK_END, OnBnClickedCheckEnd)
END_MESSAGE_MAP()


// CLightingPage 메시지 처리기입니다.
BOOL CLightingPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_Color.SubclassDlgItem ( IDC_STATIC_COLOR, this );

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CLightingPage::SetProperty ( EFF_PROPERTY* pProp )
{
	m_pProperty = (LIGHTNING_PROPERTY*) pProp;

	m_Color.m_d3dStartColor = m_pProperty->m_cColorStart;
	m_Color.m_d3dEndColor = m_pProperty->m_cColorEnd;
	m_Color.RedrawWindow();

	CWnd*	pWnd;

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

	char	szValue[256];

	pWnd = GetDlgItem ( IDC_EDIT_DIVISION );
	_gcvt ( m_pProperty->m_dwDivision, 7, szValue );
	pWnd->SetWindowText ( szValue );

	pWnd = GetDlgItem ( IDC_EDIT_LENGTH );
	_gcvt ( m_pProperty->m_fMaxLenth, 7, szValue );
	pWnd->SetWindowText ( szValue );

	pWnd = GetDlgItem ( IDC_EDIT_WIDTH_IN );
	_gcvt ( m_pProperty->m_fWidth_In, 7, szValue );
	pWnd->SetWindowText ( szValue );

	pWnd = GetDlgItem ( IDC_EDIT_WIDTH_OUT );
	_gcvt ( m_pProperty->m_fWidth_Out, 7, szValue );
	pWnd->SetWindowText ( szValue );

	pWnd = GetDlgItem ( IDC_EDIT_RANROTATE );
	_gcvt ( m_pProperty->m_fVelocity, 7, szValue );
	pWnd->SetWindowText ( szValue );

	pWnd = GetDlgItem ( IDC_EDIT_OUT_NUM );
	_gcvt ( m_pProperty->m_dwLightning, 7, szValue );
	pWnd->SetWindowText ( szValue );


	CButton*	pButton;

	pButton = (CButton*) GetDlgItem ( IDC_CHECK_IN );
	if ( m_pProperty->m_dwFlag & USEIN )			pButton->SetCheck ( TRUE );
	else											pButton->SetCheck ( FALSE );

	pWnd = GetDlgItem ( IDC_EDIT_WIDTH_IN );
	pWnd->EnableWindow ( pButton->GetCheck() );
	pWnd = GetDlgItem ( IDC_BUTTON_TEXTURE_IN );
	pWnd->EnableWindow ( pButton->GetCheck() );


	pButton = (CButton*) GetDlgItem ( IDC_CHECK_OUT );
	if ( m_pProperty->m_dwFlag & USEOUT )			pButton->SetCheck ( TRUE );
	else											pButton->SetCheck ( FALSE );

	pWnd = GetDlgItem ( IDC_EDIT_WIDTH_OUT );
	pWnd->EnableWindow ( pButton->GetCheck() );
	pWnd = GetDlgItem ( IDC_BUTTON_TEXTURE_OUT );
	pWnd->EnableWindow ( pButton->GetCheck() );


	pButton = (CButton*) GetDlgItem ( IDC_CHECK_RANROTATE );
	if ( m_pProperty->m_dwFlag & USERANROTATE )		pButton->SetCheck ( TRUE );
	else											pButton->SetCheck ( FALSE );

	pWnd = GetDlgItem ( IDC_EDIT_RANROTATE );
	pWnd->EnableWindow ( pButton->GetCheck() );

	pButton = (CButton*) GetDlgItem ( IDC_CHECK_START );
	if ( m_pProperty->m_dwFlag & USESTART )			pButton->SetCheck ( TRUE );
	else											pButton->SetCheck ( FALSE );

	pButton = (CButton*) GetDlgItem ( IDC_CHECK_END );
	if ( m_pProperty->m_dwFlag & USEEND )			pButton->SetCheck ( TRUE );
	else											pButton->SetCheck ( FALSE );
	
	pWnd = GetDlgItem ( IDC_EDIT_TEXTURE_IN );
	pWnd->SetWindowText ( m_pProperty->m_szTexture_In );

	pWnd = GetDlgItem ( IDC_EDIT_TEXTURE_OUT );
	pWnd->SetWindowText ( m_pProperty->m_szTexture_Out );

}

void CLightingPage::OnBnClickedButtonEditsound()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pSheetTab->ActiveEffSoundPage ( &m_pProperty->m_MovSound, LIGHTINGINDEX );
}

void CLightingPage::OnBnClickedButtonPlay()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다..
	//	Note : 설정 적용.
	//
	OnBnClickedButtonApply();

	//	Note : 이펙트 생성.
	//
	D3DXMATRIX matTrans;
	D3DXMatrixIdentity ( &matTrans );

	DxEffGroupPlayer::GetInstance().SetTime ( 0.0f );
	DxEffGroupPlayer::GetInstance().NewEffGroup ( &CEffGroupPage::m_EffPropGroup, m_pProperty, matTrans );
}

void CLightingPage::OnBnClickedButtonApply()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString strText;

	int nIndex, nData;
	nIndex = m_pComboBlend.GetCurSel();
	nData = m_pComboBlend.GetItemData ( nIndex );

	m_pProperty->m_nBlend = nData;

	CWnd*	pWnd;

	pWnd = GetDlgItem ( IDC_EDIT_DIVISION );
	pWnd->GetWindowText ( strText );
	m_pProperty->m_dwDivision = (DWORD) atoi ( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_LENGTH );
	pWnd->GetWindowText ( strText );
	m_pProperty->m_fMaxLenth = (float)atof ( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_WIDTH_IN );
	pWnd->GetWindowText ( strText );
	m_pProperty->m_fWidth_In = (float)atof ( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_WIDTH_OUT );
	pWnd->GetWindowText ( strText );
	m_pProperty->m_fWidth_Out = (float)atof ( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_RANROTATE );
	pWnd->GetWindowText ( strText );
	m_pProperty->m_fVelocity = (float)atof ( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_OUT_NUM );
	pWnd->GetWindowText ( strText );
	m_pProperty->m_dwLightning = (DWORD)atoi ( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_TEXTURE_IN );
	pWnd->GetWindowText ( strText );
	strcpy ( m_pProperty->m_szTexture_In, strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_TEXTURE_OUT );
	pWnd->GetWindowText ( strText );
	strcpy ( m_pProperty->m_szTexture_Out, strText.GetString() );

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CEffectToolView *pView = (CEffectToolView*) pFrame->GetActiveView ();

	DxEffGroupPlayer::GetInstance().DeleteEff ( CEffGroupPage::m_EffPropGroup.m_strFileName.c_str() );
	m_pProperty->CleanUp ();
	m_pProperty->Create ( pView->GetD3dDevice(), FALSE );
}

void CLightingPage::OnBnClickedButtonOk()
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

void CLightingPage::OnCbnSelchangeComboBlend()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nIndex, nData;
	nIndex = m_pComboBlend.GetCurSel();
	nData = m_pComboBlend.GetItemData ( nIndex );

	m_pProperty->m_nBlend = nData;
}

void CLightingPage::OnBnClickedButtonAlpha()
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

void CLightingPage::OnBnClickedButtonColorS()
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

void CLightingPage::OnBnClickedButtonColorE()
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
void CLightingPage::OnBnClickedCheckIn()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CButton*	pButton;
	pButton = (CButton*) GetDlgItem ( IDC_CHECK_IN );
	if ( pButton->GetCheck() )	m_pProperty->m_dwFlag |= USEIN;
	else						m_pProperty->m_dwFlag &= ~USEIN;

	CWnd*		pWnd;
	pWnd = GetDlgItem ( IDC_EDIT_WIDTH_IN );
	pWnd->EnableWindow ( pButton->GetCheck() );

	pWnd = GetDlgItem ( IDC_BUTTON_TEXTURE_IN );
	pWnd->EnableWindow ( pButton->GetCheck() );
}

void CLightingPage::OnBnClickedCheckOut()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CButton*	pButton;
	pButton = (CButton*) GetDlgItem ( IDC_CHECK_OUT );
	if ( pButton->GetCheck() )	m_pProperty->m_dwFlag |= USEOUT;
	else						m_pProperty->m_dwFlag &= ~USEOUT;

	CWnd*		pWnd;
	pWnd = GetDlgItem ( IDC_EDIT_WIDTH_OUT );
	pWnd->EnableWindow ( pButton->GetCheck() );

	pWnd = GetDlgItem ( IDC_BUTTON_TEXTURE_OUT );
	pWnd->EnableWindow ( pButton->GetCheck() );
}

void CLightingPage::OnBnClickedCheckRanrotate()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CButton*	pButton;
	pButton = (CButton*) GetDlgItem ( IDC_CHECK_RANROTATE );
	if ( pButton->GetCheck() )	m_pProperty->m_dwFlag |= USERANROTATE;
	else						m_pProperty->m_dwFlag &= ~USERANROTATE;

	CWnd*		pWnd;
	pWnd = GetDlgItem ( IDC_EDIT_RANROTATE );
	pWnd->EnableWindow ( pButton->GetCheck() );
}

void CLightingPage::OnBnClickedButtonTextureIn()
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
		pWnd = GetDlgItem ( IDC_EDIT_TEXTURE_IN );
		pWnd->SetWindowText ( FilePath );
	}

	pView->SetCurClick( TRUE );	// 커서 클릭을 살린다.
}

void CLightingPage::OnBnClickedButtonTextureOut()
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
		pWnd = GetDlgItem ( IDC_EDIT_TEXTURE_OUT );
		pWnd->SetWindowText ( FilePath );
	}

	pView->SetCurClick( TRUE );	// 커서 클릭을 살린다.
}

void CLightingPage::OnBnClickedCheckStart()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CButton*	pButton;
	pButton = (CButton*) GetDlgItem ( IDC_CHECK_START );
	if ( pButton->GetCheck() )	m_pProperty->m_dwFlag |= USESTART;
	else						m_pProperty->m_dwFlag &= ~USESTART;
}

void CLightingPage::OnBnClickedCheckEnd()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CButton*	pButton;
	pButton = (CButton*) GetDlgItem ( IDC_CHECK_END );
	if ( pButton->GetCheck() )	m_pProperty->m_dwFlag |= USEEND;
	else						m_pProperty->m_dwFlag &= ~USEEND;
}
