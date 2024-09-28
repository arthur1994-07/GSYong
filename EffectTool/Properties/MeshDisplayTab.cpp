// MeshDisplayTab.cpp : implementation file
//

#include "stdafx.h"
#include "../EffectTool.h"
#include "../MainFrm.h"
#include "../EffectToolDoc.h"
#include "../EffectToolView.h"

#include "EffSheetWithTab.h"
#include "../../EngineLib/DxTools/TextureManager.h"
#include "../Dialog/ColorPickerDlg.h"
#include "../Dialog/SetVar.h"
#include "../Dialog/SetMultiSlider.h"

#include "MeshDisplayTab.h"
#include ".\meshdisplaytab.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// CMeshDisplayTab dialog


CMeshDisplayTab::CMeshDisplayTab(CWnd* pParent /*=NULL*/)
	: CDialog(CMeshDisplayTab::IDD, pParent),
	m_pSheetTab(NULL),
	m_pProperty(NULL)
{
	//{{AFX_DATA_INIT(CMeshDisplayTab)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CMeshDisplayTab::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMeshDisplayTab)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_COMBO_POWER, m_comboPOWER);
	DDX_Control(pDX, IDC_COMBO_BLEND, m_comboBLEND);
}


BEGIN_MESSAGE_MAP(CMeshDisplayTab, CDialog)
	//{{AFX_MSG_MAP(CMeshDisplayTab)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_ALPHA, OnBnClickedButtonAlpha)
	ON_BN_CLICKED(IDC_BUTTON_COLOR_S, OnBnClickedButtonColorS)
	ON_BN_CLICKED(IDC_BUTTON_COLOR_E, OnBnClickedButtonColorE)
	ON_BN_CLICKED(IDC_BUTTON_TEXTURE, OnBnClickedButtonTexture)
	ON_BN_CLICKED(IDC_BUTTON_NORMAL, OnBnClickedButtonNormal)
	ON_BN_CLICKED(IDC_BUTTON_CULLMODE, OnBnClickedButtoemCullMode)
	ON_BN_CLICKED(IDC_BUTTON_TEXMOVE, OnBnClickedButtonTexmove)
	ON_BN_CLICKED(IDC_BUTTON_SEQUENCE2, OnBnClickedButtonSequence2)
	ON_CBN_SELCHANGE(IDC_COMBO_POWER, OnCbnSelchangeComboPower)
	ON_CBN_SELCHANGE(IDC_COMBO_BLEND, OnCbnSelchangeComboBlend)
	ON_BN_CLICKED(IDC_CHECK_TEXTURE, OnBnClickedCheckTexture)
	ON_BN_CLICKED(IDC_BUTTON_TEXMOVE_DISABLE, OnBnClickedButtonTexmoveDisable)
	ON_BN_CLICKED(IDC_BUTTON_SEQUENCE_DISABLE, OnBnClickedButtonSequenceDisable)
	ON_BN_CLICKED(IDC_BUTTON_GROUND_TEX, OnBnClickedButtonGroundTex)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMeshDisplayTab message handlers

void CMeshDisplayTab::Init ( MESH_PROPERTY* pProp, CsheetWithTab* pSheetTab )
{
	m_pProperty = pProp;
	m_pSheetTab = pSheetTab;

	CWnd*		pWnd;
	CButton*	pButton;

	int	nIndex;
	CString	strText;

	{
		m_comboBLEND.ResetContent();

		strText.Format ( "Src+Dest (ADD)" );
		nIndex = m_comboBLEND.AddString ( strText );
		m_comboBLEND.SetItemData ( nIndex, 0 );
		strText.Format ( "Src-Dest (SUBTRACT)" );
		nIndex = m_comboBLEND.AddString ( strText );
		m_comboBLEND.SetItemData ( nIndex, 1 );
		strText.Format ( "Dest-Src (REVSUBTRACT)" );
		nIndex = m_comboBLEND.AddString ( strText );
		m_comboBLEND.SetItemData ( nIndex, 2 );
		strText.Format ( "Color MIN" );
		nIndex = m_comboBLEND.AddString ( strText );
		m_comboBLEND.SetItemData ( nIndex, 3 );
		strText.Format ( "Color MAX" );
		nIndex = m_comboBLEND.AddString ( strText );
		m_comboBLEND.SetItemData ( nIndex, 4 );
		strText.Format ( "Src+Dest (ALPHA)" );
		nIndex = m_comboBLEND.AddString ( strText );
		m_comboBLEND.SetItemData ( nIndex, 5 );
		strText.Format ( "Src (MY)" );
		nIndex = m_comboBLEND.AddString ( strText );
		m_comboBLEND.SetItemData ( nIndex, 6 );

		m_comboBLEND.SetCurSel ( pProp->m_nBlend );
	}

	{
		m_comboPOWER.ResetContent();

		strText.Format ( "1x" );
		nIndex = m_comboPOWER.AddString ( strText );
		m_comboPOWER.SetItemData ( nIndex, 0 );
		strText.Format ( "2x" );
		nIndex = m_comboPOWER.AddString ( strText );
		m_comboPOWER.SetItemData ( nIndex, 1 );
		strText.Format ( "4x" );
		nIndex = m_comboPOWER.AddString ( strText );
		m_comboPOWER.SetItemData ( nIndex, 2 );

		m_comboPOWER.SetCurSel ( pProp->m_nPower );
	}

	{
		m_cClrCullMode.SubclassDlgItem ( IDC_BUTTON_CULLMODE, this );
		m_cClrNormal.SubclassDlgItem ( IDC_BUTTON_NORMAL, this );
		m_cClrTexMove.SubclassDlgItem ( IDC_BUTTON_TEXMOVE, this );
		m_cClrSequence.SubclassDlgItem ( IDC_BUTTON_SEQUENCE2, this );
		m_cClrGroundTex.SubclassDlgItem ( IDC_BUTTON_GROUND_TEX, this );

		COLORREF cColorBLUE, cColorGRAY, cColorTEXT;
		cColorGRAY = RGB(212, 208, 200);
		cColorBLUE = RGB(192, 192, 255);
		cColorTEXT = RGB(0, 0, 0);

		if ( pProp->m_dwFlag & USECULLNONE )	m_cClrCullMode.SetColour ( cColorTEXT, cColorBLUE );
		else									m_cClrCullMode.SetColour ( cColorTEXT, cColorGRAY );

		if ( pProp->m_dwFlag & USENORMAL2 )		m_cClrNormal.SetColour ( cColorTEXT, cColorBLUE );
		else									m_cClrNormal.SetColour ( cColorTEXT, cColorGRAY );

		if ( pProp->m_dwFlag & USETEXMOVE )		m_cClrTexMove.SetColour ( cColorTEXT, cColorBLUE );
		else									m_cClrTexMove.SetColour ( cColorTEXT, cColorGRAY );

		if ( pProp->m_dwFlag & USESEQUENCE )	m_cClrSequence.SetColour ( cColorTEXT, cColorBLUE );
		else									m_cClrSequence.SetColour ( cColorTEXT, cColorGRAY );

		if ( pProp->m_dwFlag & USEGROUNDTEX )	m_cClrGroundTex.SetColour ( cColorTEXT, cColorBLUE );
		else									m_cClrGroundTex.SetColour ( cColorTEXT, cColorGRAY );
	}

	{
		pButton = (CButton*) GetDlgItem ( IDC_CHECK_TEXTURE );
		if ( pProp->m_dwFlag & USEOTHERTEX)
		{
			pButton->SetCheck ( TRUE );

			pWnd = GetDlgItem ( IDC_BUTTON_TEXTURE );
			pWnd->EnableWindow ( TRUE );
		}
		else
		{
			pButton->SetCheck ( FALSE );

			pWnd = GetDlgItem ( IDC_BUTTON_TEXTURE );
			pWnd->EnableWindow ( FALSE );
		}

		pWnd = GetDlgItem ( IDC_EDIT_TEXTURE );
		pWnd->SetWindowText ( m_pProperty->m_szTexture );
	}

	{
		m_Color.SubclassDlgItem ( IDC_STATIC_COLOR, this );
		m_Color.m_d3dStartColor = m_pProperty->m_clrStart;
		m_Color.m_d3dEndColor = m_pProperty->m_clrEnd;
		m_Color.RedrawWindow();
	}
}

void CMeshDisplayTab::Apply ()
{
	CString strText;
	CWnd*	pWnd;
	pWnd = GetDlgItem ( IDC_EDIT_TEXTURE );
	pWnd->GetWindowText ( strText );
	strcpy ( m_pProperty->m_szTexture, strText.GetString() );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//

void CMeshDisplayTab::OnCbnSelchangeComboPower()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nIndex, nData;
	nIndex = m_comboPOWER.GetCurSel();
	nData = m_comboPOWER.GetItemData ( nIndex );

	m_pProperty->m_nPower = nData;
}

void CMeshDisplayTab::OnCbnSelchangeComboBlend()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nIndex, nData;
	nIndex = m_comboBLEND.GetCurSel();
	nData = m_comboBLEND.GetItemData ( nIndex );

	m_pProperty->m_nBlend = nData;
}

void CMeshDisplayTab::OnBnClickedButtonAlpha()
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

void CMeshDisplayTab::OnBnClickedButtonColorS()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	CEffectToolView *pView = (CEffectToolView*) pFrame->GetActiveView ();
	pView->SetCurClick( FALSE );

	int ThisR = int ( m_pProperty->m_clrStart.r * 255.0f );
	int ThisG = int ( m_pProperty->m_clrStart.g * 255.0f );
	int ThisB = int ( m_pProperty->m_clrStart.b * 255.0f );

	CColorPickerDlg ColorPicker ( RGB(ThisR,ThisG,ThisB) );
	if ( IDOK == ColorPicker.DoModal() )
	{
		COLORREF SelectColor = ColorPicker.GetColor ();
		m_pProperty->m_clrStart.r = GetRValue(SelectColor)/255.0f;
		m_pProperty->m_clrStart.g = GetGValue(SelectColor)/255.0f;
		m_pProperty->m_clrStart.b = GetBValue(SelectColor)/255.0f;
	}

	m_Color.m_d3dStartColor = m_pProperty->m_clrStart;	
	m_Color.RedrawWindow();

	pView->SetCurClick( TRUE );	// 커서 클릭을 살린다.
}

void CMeshDisplayTab::OnBnClickedButtonColorE()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	CEffectToolView *pView = (CEffectToolView*) pFrame->GetActiveView ();
	pView->SetCurClick( FALSE );

	int ThisR = int ( m_pProperty->m_clrEnd.r * 255.0f );
	int ThisG = int ( m_pProperty->m_clrEnd.g * 255.0f );
	int ThisB = int ( m_pProperty->m_clrEnd.b * 255.0f );

	CColorPickerDlg ColorPicker ( RGB(ThisR,ThisG,ThisB) );
	
	if ( IDOK == ColorPicker.DoModal() )
	{
		COLORREF SelectColor = ColorPicker.GetColor ();
		m_pProperty->m_clrEnd.r = GetRValue(SelectColor)/255.0f;
		m_pProperty->m_clrEnd.g = GetGValue(SelectColor)/255.0f;
		m_pProperty->m_clrEnd.b = GetBValue(SelectColor)/255.0f;
	}
	
	m_Color.m_d3dEndColor = m_pProperty->m_clrEnd;
	m_Color.RedrawWindow();

	pView->SetCurClick( TRUE );	// 커서 클릭을 살린다.
}

void CMeshDisplayTab::OnBnClickedButtonTexture()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	CEffectToolView *pView = (CEffectToolView*) pFrame->GetActiveView ();
	pView->SetCurClick( FALSE );

	CString szFilter = "Texture Image (*.bmp,*.dds,*.tga,*.jpg)|*.bmp;*.dds;*.tga;*.jpg|";
	CFileDialog dlg ( TRUE, ".BMP", NULL, 
		OFN_HIDEREADONLY, szFilter, this );

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

void CMeshDisplayTab::OnBnClickedButtonTexmove()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pProperty->m_dwFlag |= USETEXMOVE;

	COLORREF cColorBLUE = RGB(192, 192, 255);
	COLORREF cColorTEXT = RGB(0, 0, 0);
	m_cClrTexMove.SetColour ( cColorTEXT, cColorBLUE );

	if ( m_pProperty->m_dwFlag & USETEXMOVE )
	{
		m_pSheetTab->ActiveDirectionPage ( (D3DXVECTOR3*) &m_pProperty->m_vTexVel, 2.f, MESHPAGEINDEX );
	}
}

void CMeshDisplayTab::OnBnClickedButtonSequence2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	CEffectToolView *pView = (CEffectToolView*) pFrame->GetActiveView ();
	pView->SetCurClick( FALSE );

	CSetVar DlgSetVar;
	DlgSetVar.m_StrTitle = "Set Animation";
	DlgSetVar.m_StrComm1 = "초";
	DlgSetVar.m_StrComm2 = "행";
	DlgSetVar.m_StrComm3 = "열";
	DlgSetVar.m_StrRadio1 = "단방향";
	DlgSetVar.m_StrRadio2 = "양방향";
	DlgSetVar.m_dwFlag = USERADIO | USESTARTSCROLL | USESTARTVARSCROLL | USEENDSCROLL;
	DlgSetVar.m_bZeroBase = TRUE;
	DlgSetVar.m_ToScale = 40.0f;

	DlgSetVar.m_iRadio = ((m_pProperty->m_dwFlag & USESEQUENCELOOP) ? 1 : 0 );
	DlgSetVar.m_fStart = m_pProperty->m_fAniTime;
	DlgSetVar.m_fStartVar = (float)m_pProperty->m_nCol;
	DlgSetVar.m_fEnd = (float)m_pProperty->m_nRow;
	
	if ( IDOK == DlgSetVar.DoModal() )
	{
		if ( DlgSetVar.m_iRadio == 0 )			m_pProperty->m_dwFlag &= ~USESEQUENCELOOP;
		else if ( DlgSetVar.m_iRadio == 1 )		m_pProperty->m_dwFlag |= USESEQUENCELOOP;

		m_pProperty->m_fAniTime = DlgSetVar.m_fStart;
		m_pProperty->m_nCol = (int)DlgSetVar.m_fStartVar;
		m_pProperty->m_nRow = (int)DlgSetVar.m_fEnd;


		m_pProperty->m_dwFlag |= USESEQUENCE;

		COLORREF cColorBLUE = RGB(192, 192, 255);
		COLORREF cColorTEXT = RGB(0, 0, 0);
		m_cClrSequence.SetColour ( cColorTEXT, cColorBLUE );
	}

	pView->SetCurClick( TRUE );	// 커서 클릭을 살린다.
}

void CMeshDisplayTab::OnBnClickedButtonNormal()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	COLORREF cColorBLUE, cColorGRAY, cColorTEXT;
	cColorGRAY = RGB(212, 208, 200);
	cColorBLUE = RGB(192, 192, 255);
	cColorTEXT = RGB(0, 0, 0);

	m_pProperty->m_dwFlag ^= USENORMAL2;

	if ( m_pProperty->m_dwFlag & USENORMAL2 )	m_cClrNormal.SetColour ( cColorTEXT, cColorBLUE );
	else										m_cClrNormal.SetColour ( cColorTEXT, cColorGRAY );
}

void CMeshDisplayTab::OnBnClickedButtoemCullMode()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	COLORREF cColorBLUE, cColorGRAY, cColorTEXT;
	cColorGRAY = RGB(212, 208, 200);
	cColorBLUE = RGB(192, 192, 255);
	cColorTEXT = RGB(0, 0, 0);

	m_pProperty->m_dwFlag ^= USECULLNONE;

	if ( m_pProperty->m_dwFlag & USECULLNONE )	m_cClrCullMode.SetColour ( cColorTEXT, cColorBLUE );
	else										m_cClrCullMode.SetColour ( cColorTEXT, cColorGRAY );
}

void CMeshDisplayTab::OnBnClickedCheckTexture()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CWnd*		pWnd;
	CButton*	pButton;
	pButton = (CButton*) GetDlgItem ( IDC_CHECK_TEXTURE );

	if ( pButton->GetCheck() )
	{
		m_pProperty->m_dwFlag |= USEOTHERTEX;

		pWnd = GetDlgItem ( IDC_BUTTON_TEXTURE );
		pWnd->EnableWindow ( TRUE );
	}
	else
	{
		m_pProperty->m_dwFlag &= ~USEOTHERTEX;

		pWnd = GetDlgItem ( IDC_BUTTON_TEXTURE );
		pWnd->EnableWindow ( FALSE );
	}
}

void CMeshDisplayTab::OnBnClickedButtonTexmoveDisable()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pProperty->m_dwFlag &= ~USETEXMOVE;

	COLORREF cColorTEXT	= RGB(0, 0, 0);
	COLORREF cColorGRAY = RGB(212, 208, 200);
	m_cClrTexMove.SetColour ( cColorTEXT, cColorGRAY );
}

void CMeshDisplayTab::OnBnClickedButtonSequenceDisable()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pProperty->m_dwFlag &= ~USESEQUENCE;

	COLORREF cColorTEXT	= RGB(0, 0, 0);
	COLORREF cColorGRAY = RGB(212, 208, 200);
	m_cClrSequence.SetColour ( cColorTEXT, cColorGRAY );
}

void CMeshDisplayTab::OnBnClickedButtonGroundTex()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	COLORREF cColorBLUE, cColorGRAY, cColorTEXT;
	cColorGRAY = RGB(212, 208, 200);
	cColorBLUE = RGB(192, 192, 255);
	cColorTEXT = RGB(0, 0, 0);

	m_pProperty->m_dwFlag ^= USEGROUNDTEX;

	if ( m_pProperty->m_dwFlag & USEGROUNDTEX )	m_cClrGroundTex.SetColour ( cColorTEXT, cColorBLUE );
	else										m_cClrGroundTex.SetColour ( cColorTEXT, cColorGRAY );
}
