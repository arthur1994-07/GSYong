// EffMultiTexPage.cpp : 구현 파일입니다.
//
#include "stdafx.h"
#include "../CharEdit.h"

#include "../MainFrm.h"
#include "../CharEditDoc.h"
#include "../CharEditView.h"

#include "SheetWithTabChar.h"
#include "../Dialog/ColorPickerDlg.h"

#include "../Dialog/SetVar.h"

#include "../EngineLib/DxTools/TextureManager.h"
#include "../EngineLib/DxEffect/Single/DxEffSinglePropGMan.h"

#include "EffMultiTexPage.h"
#include ".\effmultitexpage.h"

#pragma warning(disable:4996)

#include "../../SigmaCore/DebugInclude.h"

// CEffMultiTexPage 대화 상자입니다.

IMPLEMENT_DYNAMIC(CEffMultiTexPage, CPropertyPage)
CEffMultiTexPage::CEffMultiTexPage()
	: CEffCharPage(CEffMultiTexPage::IDD)
{
}

CEffMultiTexPage::~CEffMultiTexPage()
{
}

void CEffMultiTexPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_COLOROP, m_ComboColorOP);
	DDX_Control(pDX, IDC_COMBO_MATRIAL, m_ComboMatrials);
}


BEGIN_MESSAGE_MAP(CEffMultiTexPage, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_TEX1, OnBnClickedButtonTex1)
	ON_BN_CLICKED(IDC_BUTTON_TEX2, OnBnClickedButtonTex2)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnBnClickedButtonApply)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(IDC_RADIO_TEX1, OnBnClickedRadioTex1)
	ON_BN_CLICKED(IDC_RADIO_TEX2, OnBnClickedRadioTex2)
	ON_BN_CLICKED(IDC_BUTTON_MOVE_TEX1, OnBnClickedButtonMoveTex1)
	ON_BN_CLICKED(IDC_BUTTON_MOVE_TEX2, OnBnClickedButtonMoveTex2)
	ON_BN_CLICKED(IDC_BUTTON_COLOR1, OnBnClickedButtonColor1)
	ON_BN_CLICKED(IDC_BUTTON_COLOR2, OnBnClickedButtonColor2)
	ON_BN_CLICKED(IDC_BUTTON_TEXTURE, OnBnClickedButtonTexture)
	ON_BN_CLICKED(IDC_CHECK_VIEW_TEX, OnBnClickedCheckViewTex)
	ON_BN_CLICKED(IDC_BUTTON_TEX_RE1, OnBnClickedButtonTexRe1)
	ON_BN_CLICKED(IDC_BUTTON_TEX_RE2, OnBnClickedButtonTexRe2)
	ON_BN_CLICKED(IDC_CHECK_SRCTEX, OnBnClickedCheckSrctex)
	ON_CBN_SELCHANGE(IDC_COMBO_MATRIAL, OnCbnSelchangeComboMatrial)
	ON_BN_CLICKED(IDC_RADIO_ALL, OnBnClickedRadioAll)
	ON_BN_CLICKED(IDC_RADIO_SELECT, OnBnClickedRadioSelect)
END_MESSAGE_MAP()


// CEffMultiTexPage 메시지 처리기입니다.
void CEffMultiTexPage::SetProperty ( EFFCHAR_PROPERTY *pProp )
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxSkinPiece *pSkinPiece = pView->GetSkinPiece ();

	if ( pProp )
		m_Property = *((EFFCHAR_PROPERTY_MULTITEX*)pProp);

	// Reset~!!
	CString	Str;

	CWnd*		pWnd;
	CButton*	pButton;
	int		nIndex;

	m_ComboColorOP.ResetContent();

	Str.Format ( "밝게" );
	nIndex = m_ComboColorOP.AddString ( Str );
	m_ComboColorOP.SetItemData ( nIndex, 0 );
	Str.Format ( "어둡게" );
	nIndex = m_ComboColorOP.AddString ( Str );
	m_ComboColorOP.SetItemData ( nIndex, 1 );
	Str.Format ( "기본값" );
	nIndex = m_ComboColorOP.AddString ( Str );
	m_ComboColorOP.SetItemData ( nIndex, 2 );

	m_ComboColorOP.SetCurSel ( m_Property.m_nBlend );

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

	pButton = (CButton*)GetDlgItem ( IDC_CHECK_VIEW_TEX );
	if ( m_Property.m_dwFlag & USETEXMAP )			pButton->SetCheck ( TRUE );
	else											pButton->SetCheck ( FALSE );


	pWnd = GetDlgItem ( IDC_EDIT_TEX1 );
	pWnd->SetWindowText ( m_Property.m_szTex1 );

	pWnd = GetDlgItem ( IDC_EDIT_TEX2 );
	pWnd->SetWindowText ( m_Property.m_szTex2 );


	pWnd = GetDlgItem ( IDC_EDIT_TEXTURE );
	pWnd->SetWindowText ( m_Property.m_szTexture );

	//	Enable 과 Disable 분류
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

	//	Check All Select
	pButton = (CButton*) GetDlgItem ( IDC_RADIO_ALL );
	pButton->SetCheck ( FALSE );
	pButton = (CButton*) GetDlgItem ( IDC_RADIO_SELECT );
	pButton->SetCheck ( FALSE );

	if ( m_Property.m_dwFlag & USE_ALL_TEX )
	{
		pButton = (CButton*) GetDlgItem ( IDC_RADIO_ALL );
		pButton->SetCheck ( TRUE );
	}
	else if ( m_Property.m_dwFlag & USE_SELECT_TEX )
	{
		pButton = (CButton*) GetDlgItem ( IDC_RADIO_SELECT );
		pButton->SetCheck ( TRUE );
	}

	Check_All_Select ();


	if ( !m_pEffChar )
	{
		m_pEffChar = DxEffCharMan::GetInstance().CreateEffect
		(
			DxEffCharMultiTex::TYPEID,
			&m_Property,
			NULL,
			pSkinPiece
		);

		if ( !m_pEffChar )
		{
			MessageBox ( "이팩트 생성에 실패하였습니다.", "ERROR", MB_OK );
			return;
		}

		pSkinPiece->AddEffList ( m_pEffChar );

		m_Property = *((EFFCHAR_PROPERTY_MULTITEX*)m_pEffChar->GetProperty ());
	}

	DWORD	dwMaterials = m_Property.m_dwMaterials;

	DXMATERIAL_CHAR_EFF*	pMatrials = m_pEffChar->GetMaterials();

	m_ComboMatrials.ResetContent();
	for ( DWORD i=0; i<dwMaterials; ++i )
	{
		Str.Format ( "%s",pMatrials[i].szTexture );
		nIndex = m_ComboMatrials.AddString ( Str );
		m_ComboMatrials.SetItemData ( nIndex, i );
	}

	int nCurSel = 0;
	m_ComboMatrials.SetCurSel( nCurSel );			// 초기값.. !!

	pButton = (CButton*) GetDlgItem ( IDC_CHECK_SRCTEX );
	if ( pMatrials[nCurSel].IsType( EMCET_BASE ) )		pButton->SetCheck ( TRUE );
	else									pButton->SetCheck ( FALSE );
}

void CEffMultiTexPage::OnBnClickedButtonApply()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxSkinPiece *pSkinPiece = pView->GetSkinPiece ();

	CString	strTexture;
	CWnd *pWnd;

	int nIndex, nData;

	nIndex = m_ComboColorOP.GetCurSel ();
	nData = m_ComboColorOP.GetItemData ( nIndex );
	m_Property.m_nBlend = nData;

	const char* szTexture;
	pWnd = GetDlgItem ( IDC_EDIT_TEX1 );
	pWnd->GetWindowText ( strTexture );
	szTexture = strTexture.GetString();
	strcpy ( m_Property.m_szTex1, szTexture );

	pWnd = GetDlgItem ( IDC_EDIT_TEX2 );
	pWnd->GetWindowText ( strTexture );
	szTexture = strTexture.GetString();
	strcpy ( m_Property.m_szTex2, szTexture );


	pWnd = GetDlgItem ( IDC_EDIT_TEXTURE );
	pWnd->GetWindowText ( strTexture );
	szTexture = strTexture.GetString();
	strcpy ( m_Property.m_szTexture, szTexture );

	m_pEffChar->SetProperty ( &m_Property );
	m_pEffChar->Create ( pView->GetD3dDevice() );

    EffectApply();
}

void CEffMultiTexPage::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	OnBnClickedButtonApply();

    ReturnPage();
}

void CEffMultiTexPage::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    ReturnPage();
}

void CEffMultiTexPage::OnBnClickedButtonTex1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
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

void CEffMultiTexPage::OnBnClickedButtonTex2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
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

void CEffMultiTexPage::OnBnClickedRadioTex1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
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

void CEffMultiTexPage::OnBnClickedRadioTex2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
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

void CEffMultiTexPage::OnBnClickedButtonMoveTex1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CSetVar DlgSetVar;

	DlgSetVar.m_StrTitle = "Tex 이동 1";
	DlgSetVar.m_StrComm1 = "X축 이동";
	DlgSetVar.m_StrComm3 = "Y축 이동";
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

void CEffMultiTexPage::OnBnClickedButtonMoveTex2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CSetVar DlgSetVar;

	DlgSetVar.m_StrTitle = "Tex 이동 2";
	DlgSetVar.m_StrComm1 = "X축 이동";
	DlgSetVar.m_StrComm3 = "Y축 이동";
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

void CEffMultiTexPage::OnBnClickedButtonColor1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
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

void CEffMultiTexPage::OnBnClickedButtonColor2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
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

void CEffMultiTexPage::OnBnClickedButtonTexture()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
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

void CEffMultiTexPage::OnBnClickedCheckViewTex()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CButton*	pButton;
	pButton = (CButton*)GetDlgItem ( IDC_CHECK_VIEW_TEX );
	if ( pButton->GetCheck() )			m_Property.m_dwFlag |= USETEXMAP;
	else								m_Property.m_dwFlag &= ~USETEXMAP;
}

void CEffMultiTexPage::OnBnClickedButtonTexRe1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CSetVar DlgSetVar;

	DlgSetVar.m_StrTitle = "Tex 반복 1";
	DlgSetVar.m_StrComm1 = "U좌표 :";
	DlgSetVar.m_StrComm3 = "V좌표 :";
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

void CEffMultiTexPage::OnBnClickedButtonTexRe2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CSetVar DlgSetVar;

	DlgSetVar.m_StrTitle = "Tex 반복 2";
	DlgSetVar.m_StrComm1 = "U좌표 :";
	DlgSetVar.m_StrComm3 = "V좌표 :";
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

void CEffMultiTexPage::OnBnClickedCheckSrctex()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	DXMATERIAL_CHAR_EFF*	pMatrials = m_pEffChar->GetMaterials();

	int nCurSel = m_ComboMatrials.GetCurSel();

	CButton*	pButton;
	pButton = (CButton*) GetDlgItem ( IDC_CHECK_SRCTEX );
	if ( pButton->GetCheck() )		pMatrials[nCurSel].SetType( EMCET_BASE );
	else							pMatrials[nCurSel].SetType( EMCET_NULL );
}

void CEffMultiTexPage::OnCbnSelchangeComboMatrial()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CButton*	pButton;

	int nCurSel = m_ComboMatrials.GetCurSel();

	DXMATERIAL_CHAR_EFF*	pMatrials = m_pEffChar->GetMaterials();

	pButton = (CButton*) GetDlgItem ( IDC_CHECK_SRCTEX );
	if ( pMatrials[nCurSel].IsType( EMCET_BASE ) )		pButton->SetCheck ( TRUE );
	else									pButton->SetCheck ( FALSE );
}

void CEffMultiTexPage::OnBnClickedRadioAll()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_Property.m_dwFlag &= ~USE_SELECT_TEX;
	m_Property.m_dwFlag |= USE_ALL_TEX;

	Check_All_Select ();
}

void CEffMultiTexPage::OnBnClickedRadioSelect()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_Property.m_dwFlag &= ~USE_ALL_TEX;
	m_Property.m_dwFlag |= USE_SELECT_TEX;
	

	Check_All_Select ();
}

void CEffMultiTexPage::Check_All_Select ()
{
	CButton*	pButton;
	pButton = (CButton*) GetDlgItem ( IDC_CHECK_SRCTEX );
	
	if ( m_Property.m_dwFlag & USE_ALL_TEX )
	{
		m_ComboMatrials.EnableWindow ( FALSE );
		pButton->EnableWindow ( FALSE );
	}
	else if ( m_Property.m_dwFlag & USE_SELECT_TEX )
	{
		m_ComboMatrials.EnableWindow ( TRUE );
		pButton->EnableWindow ( TRUE );
	}
}
