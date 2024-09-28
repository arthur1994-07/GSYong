// EffTS_Overlay.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "../MainFrm.h"
#include "../CharEditDoc.h"
#include "../CharEditView.h"

#include "../EngineLib/DxEffect/Char/DxEffCharTotalShader.h"

#include "sheetWithTabChar.h"
#include "sheetWithTabTS.h"
#include "../Dialog/ColorPickerDlg.h"

#include "../MfcExLib/ExLibs/EtcFunction.h"

#include "sheetWithTabTS.h"
#include "EffTS_Overlay.h"
#include ".\effts_overlay.h"


// CEffTS_Overlay 대화 상자입니다.

IMPLEMENT_DYNAMIC(CEffTS_Overlay, CPropertyPage)
CEffTS_Overlay::CEffTS_Overlay()
	: CPropertyPage(CEffTS_Overlay::IDD)
{
}

CEffTS_Overlay::~CEffTS_Overlay()
{
}

void CEffTS_Overlay::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SLIDER_SPECLEVEL, m_slider_SpecLV);
	DDX_Control(pDX, IDC_COMBO_MATRIAL, m_combo_Material);
}


BEGIN_MESSAGE_MAP(CEffTS_Overlay, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_TEXTURE, OnBnClickedButtonTexture)
	ON_BN_CLICKED(IDC_BUTTON_COLOR1, OnBnClickedButtonColor1)
	ON_BN_CLICKED(IDC_BUTTON_COLOR2, OnBnClickedButtonColor2)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_SPECLEVEL, OnNMCustomdrawSliderSpeclevel)
END_MESSAGE_MAP()


// CEffTS_Overlay 메시지 처리기입니다.
void CEffTS_Overlay::Apply()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_PropOverlay.fSpecularLevel = GetWin_Num_float( this, IDC_EDIT_SPECLEVEL );
	StringCchCopy( m_PropOverlay.szOverlayTex, MAX_PATH, GetWin_Text( this, IDC_EDIT_TEXTURE ).GetString() );
	m_PropOverlay.dwSelectMaterial = m_combo_Material.GetCurSel();
}



void CEffTS_Overlay::SetProperty( EFFCHAR_PROPERTY_TS_OVERLAY Property ) 
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxSkinPiece *pSkinPiece = pView->GetSkinPiece ();

	PSMESHCONTAINER pmcMesh = pSkinPiece->m_pmcMesh;
	DWORD	dwMaterials = pmcMesh->GetNumMaterials();
	int nIndex = 0;
	CString	Str;
	m_combo_Material.ResetContent();
	for ( DWORD i=0; i<dwMaterials; ++i )
	{
		Str.Format ( "%s",pmcMesh->pMaterials[i].pTextureFilename );
		nIndex = m_combo_Material.AddString ( Str );
		m_combo_Material.SetItemData ( nIndex, i );
	}

	m_PropOverlay = Property; 

	SetWin_Num_float( this, IDC_EDIT_SPECLEVEL, m_PropOverlay.fSpecularLevel );
	SetWin_Text( this, IDC_EDIT_TEXTURE, m_PropOverlay.szOverlayTex );
	m_combo_Material.SetCurSel( m_PropOverlay.dwSelectMaterial );
	
	
}

void CEffTS_Overlay::OnBnClickedButtonTexture()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString szFilter = _T("Texture Image (*.bmp,*.dds,*.tga,*.jpg)|*.bmp;*.dds;*.tga;*.jpg|");
	CFileDialog dlg ( TRUE, _T(".BMP"), NULL, 
		OFN_HIDEREADONLY, szFilter, this );

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;	
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	dlg.m_ofn.lpstrInitialDir = TextureManager::GetInstance().GetPath().c_str();

	if ( dlg.DoModal() == IDOK )
	{
		CString FilePath = dlg.GetFileName ();

		CWnd *pWnd;
		pWnd = GetDlgItem ( IDC_EDIT_TEXTURE );
		pWnd->SetWindowText ( FilePath );
	}
}

void CEffTS_Overlay::OnBnClickedButtonColor1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int ThisR = int ( m_PropOverlay.cColor1.r * 255.0f );
	int ThisG = int ( m_PropOverlay.cColor1.g * 255.0f );
	int ThisB = int ( m_PropOverlay.cColor1.b * 255.0f );

	CColorPickerDlg ColorPicker ( RGB(ThisR,ThisG,ThisB) );

	if ( IDOK == ColorPicker.DoModal() )
	{
		COLORREF SelectColor = ColorPicker.GetColor ();
		m_PropOverlay.cColor1.r = GetRValue(SelectColor)/255.0f;
		m_PropOverlay.cColor1.g = GetGValue(SelectColor)/255.0f;
		m_PropOverlay.cColor1.b = GetBValue(SelectColor)/255.0f;

		Apply();
	}
}

void CEffTS_Overlay::OnBnClickedButtonColor2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int ThisR = int ( m_PropOverlay.cColor2.r * 255.0f );
	int ThisG = int ( m_PropOverlay.cColor2.g * 255.0f );
	int ThisB = int ( m_PropOverlay.cColor2.b * 255.0f );

	CColorPickerDlg ColorPicker ( RGB(ThisR,ThisG,ThisB) );

	if ( IDOK == ColorPicker.DoModal() )
	{
		COLORREF SelectColor = ColorPicker.GetColor ();
		m_PropOverlay.cColor2.r = GetRValue(SelectColor)/255.0f;
		m_PropOverlay.cColor2.g = GetGValue(SelectColor)/255.0f;
		m_PropOverlay.cColor2.b = GetBValue(SelectColor)/255.0f;

		Apply();
	}
}

void CEffTS_Overlay::OnNMCustomdrawSliderSpeclevel(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	SetWin_Num_float( this, IDC_EDIT_SPECLEVEL, (float)m_slider_SpecLV.GetPos() );
	*pResult = 0;
}
