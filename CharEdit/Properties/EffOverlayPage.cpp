// EffOverlayPage.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "../CharEdit.h"
#include "../MainFrm.h"
#include "../CharEditDoc.h"
#include "../CharEditView.h"

#include "SheetWithTabChar.h"
#include "../Dialog/ColorPickerDlg.h"

#include "../MfcExLib/ExLibs/EtcFunction.h"
#include "EffOverlayPage.h"
#include ".\effoverlaypage.h"

#include "../EngineLib/DxTools/DxInputDevice.h"

COLORREF	g_ColorRef; // by luxes.
int			g_iColorResult; // by luxes.

// CEffOverlayPage 대화 상자입니다.

IMPLEMENT_DYNAMIC(CEffOverlayPage, CPropertyPage)
CEffOverlayPage::CEffOverlayPage()
	: CEffCharPage(CEffOverlayPage::IDD)
	, m_pColorPicker(NULL)
{
}

CEffOverlayPage::~CEffOverlayPage()
{
}

void CEffOverlayPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SLIDER_SPECPOWER, m_slider_SpecPW);
	DDX_Control(pDX, IDC_COMBO_MATRIAL, m_combo_Material);
	DDX_Control(pDX, IDC_SLIDER_SPECLEVEL, m_slider_SpecLV);
	DDX_Control(pDX, IDC_SLIDER_ALPHA_REF, m_slider_AlphaRef);
}


BEGIN_MESSAGE_MAP(CEffOverlayPage, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnBnClickedButtonApply)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_TEXTURE, OnBnClickedButtonTexture)
	ON_BN_CLICKED(IDC_BUTTON_COLOR1, OnBnClickedButtonColor1)
	ON_BN_CLICKED(IDC_BUTTON_COLOR2, OnBnClickedButtonColor2)
	ON_BN_CLICKED(IDC_BUTTON_COLOR4, OnBnClickedButtonColor4)
	ON_BN_CLICKED(IDC_BUTTON_COLOR5, OnBnClickedButtonColor5)
	ON_BN_CLICKED(IDC_BUTTON_COLOR6, OnBnClickedButtonColor6)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_SPECPOWER, OnNMCustomdrawSliderSpecpower)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_SPECLEVEL, OnNMCustomdrawSliderSpeclevel)
	ON_BN_CLICKED(IDC_CHECK_GLOWTEX, OnBnClickedCheckGlowtex)
	ON_BN_CLICKED(IDC_BUTTON_TEXTURE_GLOWTEX, OnBnClickedButtonTextureGlowtex)
	ON_BN_CLICKED(IDC_CHECK_REFLECTTEX, OnBnClickedCheckReflecttex)
	ON_BN_CLICKED(IDC_BUTTON_TEXTURE_REFLECTTEX, OnBnClickedButtonTextureReflecttex)
	ON_BN_CLICKED(IDC_BUTTON_TEXTURE_REFLECTTEX2, OnBnClickedButtonTextureReflecttex2)
	ON_BN_CLICKED(IDC_CHECK_REFLECTTEX2, OnBnClickedCheckReflecttex2)
	ON_BN_CLICKED(IDC_CHECK_USE_SPECULAR, OnBnClickedUseSpecular)
ON_BN_CLICKED(IDC_CHECK_USE_OVERLAY, OnBnClickedCheckUseOverlay)
ON_CBN_SELCHANGE(IDC_COMBO_MATRIAL, OnCbnSelchangeComboMatrial)
ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_ALPHA_REF, OnNMCustomdrawSliderAlphaRef)
ON_BN_CLICKED(IDC_CHECK_ENABLE_CHANGE, OnBnClickedCheckEnableChange)
END_MESSAGE_MAP()


// CEffOverlayPage 메시지 처리기입니다.
void CEffOverlayPage::SetProperty( EFFCHAR_PROPERTY_OVERLAY *pProp )
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxSkinPiece *pSkinPiece = pView->GetSkinPiece ();

	if ( !m_pEffChar )
	{
		m_pEffChar = DxEffCharMan::GetInstance().CreateEffect
			(
			DxEffCharOverlay::TYPEID,
			&m_Property,
			NULL,
			pSkinPiece
			);

		if ( !m_pEffChar )
		{
			MessageBox ( _T("이팩트 생성에 실패하였습니다."), _T("ERROR"), MB_OK );
			return;
		}

		pSkinPiece->AddEffList ( m_pEffChar );
	}
	else
	{
		m_pEffChar->SetProperty ( &m_Property );
		m_pEffChar->Create ( pView->GetD3dDevice() );
	}


	SetWin_Check( this, IDC_CHECK_USE_SPECULAR, m_Property.m_bUseSpecular ); // by luxes.
	SetWin_Check( this, IDC_CHECK_GLOWTEX, m_Property.m_bUseGlowTex );
	SetWin_Check( this, IDC_CHECK_REFLECTTEX, m_Property.m_bUseReflect );
	SetWin_Check( this, IDC_CHECK_REFLECTTEX2, m_Property.m_bUseAlphaReflect );

	SetWin_Text( this, IDC_EDIT_TEXTURE_REFLECTTEX, m_Property.m_szReflectTex );
	SetWin_Text( this, IDC_EDIT_TEXTURE_REFLECTTEX2, m_Property.m_szReflectAlphaTex );
	SetWin_Num_float( this, IDC_EDIT_REFLECTACT, m_Property.m_fReflectActivity );

	SetWin_Text( this, IDC_EDIT_TEXTURE, m_Property.m_szOverlayTex );
	SetWin_Num_float( this, IDC_EDIT_SPECPOWER, m_Property.m_fSpecularPower );
	SetWin_Num_float( this, IDC_EDIT_SPECLEVEL, m_Property.m_fSpecularLevel * 10.0f );
	SetWin_Num_float( this, IDC_EDIT_ALPHA_REF, m_Property.m_fAlphaRef );
	m_slider_SpecPW.SetPos( (int)m_Property.m_fSpecularPower );
	m_slider_SpecLV.SetPos( (int)(m_Property.m_fSpecularLevel * 10.0f) );
	m_slider_AlphaRef.SetPos( (int)m_Property.m_fAlphaRef );


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

	m_combo_Material.SetCurSel( m_Property.m_dwSelectMaterial );
	SetWin_Check( this, IDC_CHECK_ENABLE_CHANGE, m_combo_Material.GetCurSel() == m_Property.m_nSelectOverlay ); // by luxes.
	SetWin_Check( this, IDC_CHECK_USE_OVERLAY, m_Property.m_bUseOverlay[ m_Property.m_dwSelectMaterial ] );

	SetWin_Num_float( this, IDC_EDIT_GLOWBRIGHT, m_Property.m_fGlowBright );
	SetWin_Num_float( this, IDC_EDIT_GLOWALPHA, m_Property.m_fGlowAlpha );
	SetWin_Num_float( this, IDC_EDIT_GLOWBRIGHT, m_Property.m_fGlowBright );
	SetWin_Num_float( this, IDC_EDIT_GLOWSPEED, m_Property.m_fGlowAniSpeed );

	if ( m_Property.m_bUseSpecular )
	{
		SetWin_Enable( this, IDC_SLIDER_SPECLEVEL, FALSE );
		SetWin_ShowWindow( this, IDC_SLIDER_SPECLEVEL, FALSE );
		SetWin_ShowWindow( this, IDC_EDIT_SPECLEVEL, FALSE );
	}
	else
	{
		SetWin_Enable( this, IDC_SLIDER_SPECLEVEL, TRUE );
		SetWin_ShowWindow( this, IDC_SLIDER_SPECLEVEL, TRUE );
		SetWin_ShowWindow( this, IDC_EDIT_SPECLEVEL, TRUE );
	}

	if( m_Property.m_bUseGlowTex )
	{
		SetWin_Enable( this, IDC_BUTTON_TEXTURE_GLOWTEX, TRUE );
		SetWin_Enable( this, IDC_EDIT_GLOWBRIGHT, TRUE );
		SetWin_Enable( this, IDC_EDIT_GLOWALPHA, TRUE );
		SetWin_Enable( this, IDC_EDIT_GLOWBRIGHT, TRUE );
		SetWin_Enable( this, IDC_EDIT_GLOWSPEED, TRUE );
		SetWin_Text( this, IDC_EDIT_TEXTURE_GLOWTEX, m_Property.m_szGlowTex );
	}else{
		SetWin_Enable( this, IDC_BUTTON_TEXTURE_GLOWTEX, FALSE );
		SetWin_Enable( this, IDC_EDIT_GLOWBRIGHT, FALSE );
		SetWin_Enable( this, IDC_EDIT_GLOWALPHA, FALSE );
		SetWin_Enable( this, IDC_EDIT_GLOWBRIGHT, FALSE );
		SetWin_Enable( this, IDC_EDIT_GLOWSPEED, FALSE );
		SetWin_Text( this, IDC_EDIT_TEXTURE_GLOWTEX, "" );
	}

	if( m_Property.m_bUseReflect )
	{
		SetWin_Text( this, IDC_EDIT_TEXTURE_REFLECTTEX, m_Property.m_szReflectTex );
		SetWin_Enable( this, IDC_CHECK_REFLECTTEX2, TRUE );
		SetWin_Enable( this, IDC_BUTTON_TEXTURE_REFLECTTEX, TRUE );
		SetWin_Enable( this, IDC_EDIT_REFLECTACT, TRUE );
	}else{
		ZeroMemory( m_Property.m_szReflectTex, MAX_PATH );
		SetWin_Text( this, IDC_EDIT_TEXTURE_REFLECTTEX, m_Property.m_szReflectTex );
		SetWin_Enable( this, IDC_CHECK_REFLECTTEX2, FALSE );
		SetWin_Enable( this, IDC_BUTTON_TEXTURE_REFLECTTEX, FALSE );
		SetWin_Enable( this, IDC_EDIT_REFLECTACT, FALSE );
	}

	if( m_Property.m_bUseAlphaReflect )
	{
		SetWin_Text( this, IDC_EDIT_TEXTURE_REFLECTTEX2, m_Property.m_szReflectAlphaTex );
		SetWin_Enable( this, IDC_BUTTON_TEXTURE_REFLECTTEX2, TRUE );
	}else{
		//ZeroMemory( m_Property.m_szReflectAlphaTex, MAX_PATH );
		SetWin_Text( this, IDC_EDIT_TEXTURE_REFLECTTEX2, m_Property.m_szReflectAlphaTex );
		SetWin_Enable( this, IDC_BUTTON_TEXTURE_REFLECTTEX2, FALSE );
	}
}

void CEffOverlayPage::SetProperty ( EFFCHAR_PROPERTY *pProp )
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxSkinPiece *pSkinPiece = pView->GetSkinPiece ();

	if ( pProp )
	{	// Memo : 속성을 수정했을 경우. by luxes.
		m_Property = *((EFFCHAR_PROPERTY_OVERLAY*)pProp);

		m_iState = MODIFY_OVERLAY;
	}
	else
	{	// Memo : 속성을 새로 만들었을 경우. by luxes
		EFFCHAR_PROPERTY_OVERLAY sProp;
		m_Property = sProp;

		m_iState = CREATE_OVERLAY;
	}

	if ( !m_pEffChar )
	{
		m_pEffChar = DxEffCharMan::GetInstance().CreateEffect
			(
			DxEffCharOverlay::TYPEID,
			&m_Property,
			NULL,
			pSkinPiece
			);

		if ( !m_pEffChar )
		{
			MessageBox ( _T("이팩트 생성에 실패하였습니다."), _T("ERROR"), MB_OK );
			return;
		}

		pSkinPiece->AddEffList ( m_pEffChar );
	}
	else
	{
		m_pEffChar->SetProperty ( &m_Property );
		m_pEffChar->Create ( pView->GetD3dDevice() );
	}

	if ( m_pEffChar )
	{
		DxEffCharOverlay* pEffCharOverlay = ((DxEffCharOverlay*)m_pEffChar);
		m_Property.Assign( pEffCharOverlay->GetPropertyOverlay( m_Property.m_dwSelectMaterial ), m_Property.m_dwSelectMaterial );
	}


	SetWin_Check( this, IDC_CHECK_USE_SPECULAR, m_Property.m_bUseSpecular ); // by luxes.
	SetWin_Check( this, IDC_CHECK_GLOWTEX, m_Property.m_bUseGlowTex );
	SetWin_Check( this, IDC_CHECK_REFLECTTEX, m_Property.m_bUseReflect );
	SetWin_Check( this, IDC_CHECK_REFLECTTEX2, m_Property.m_bUseAlphaReflect );

	SetWin_Text( this, IDC_EDIT_TEXTURE_REFLECTTEX, m_Property.m_szReflectTex );
	SetWin_Text( this, IDC_EDIT_TEXTURE_REFLECTTEX2, m_Property.m_szReflectAlphaTex );
	SetWin_Num_float( this, IDC_EDIT_REFLECTACT, m_Property.m_fReflectActivity );

	SetWin_Text( this, IDC_EDIT_TEXTURE, m_Property.m_szOverlayTex );
	SetWin_Num_float( this, IDC_EDIT_SPECPOWER, m_Property.m_fSpecularPower );
	SetWin_Num_float( this, IDC_EDIT_SPECLEVEL, m_Property.m_fSpecularLevel * 10.0f );
	SetWin_Num_float( this, IDC_EDIT_ALPHA_REF, m_Property.m_fAlphaRef );
	m_slider_SpecPW.SetPos( (int)m_Property.m_fSpecularPower );
	m_slider_SpecLV.SetPos( (int)(m_Property.m_fSpecularLevel * 10.0f) );
	m_slider_AlphaRef.SetPos( (int)m_Property.m_fAlphaRef );


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

	m_combo_Material.SetCurSel( m_Property.m_dwSelectMaterial );
	SetWin_Check( this, IDC_CHECK_ENABLE_CHANGE, m_combo_Material.GetCurSel() == m_Property.m_nSelectOverlay ); // by luxes.
	SetWin_Check( this, IDC_CHECK_USE_OVERLAY, m_Property.m_bUseOverlay[ m_Property.m_dwSelectMaterial ] );

	SetWin_Num_float( this, IDC_EDIT_GLOWBRIGHT, m_Property.m_fGlowBright );
	SetWin_Num_float( this, IDC_EDIT_GLOWALPHA, m_Property.m_fGlowAlpha );
	SetWin_Num_float( this, IDC_EDIT_GLOWBRIGHT, m_Property.m_fGlowBright );
	SetWin_Num_float( this, IDC_EDIT_GLOWSPEED, m_Property.m_fGlowAniSpeed );

	if ( m_Property.m_bUseSpecular )
	{
		SetWin_Enable( this, IDC_SLIDER_SPECLEVEL, FALSE );
		SetWin_ShowWindow( this, IDC_SLIDER_SPECLEVEL, FALSE );
		SetWin_ShowWindow( this, IDC_EDIT_SPECLEVEL, FALSE );
	}
	else
	{
		SetWin_Enable( this, IDC_SLIDER_SPECLEVEL, TRUE );
		SetWin_ShowWindow( this, IDC_SLIDER_SPECLEVEL, TRUE );
		SetWin_ShowWindow( this, IDC_EDIT_SPECLEVEL, TRUE );
	}

	if( m_Property.m_bUseGlowTex )
	{
		SetWin_Enable( this, IDC_BUTTON_TEXTURE_GLOWTEX, TRUE );
		SetWin_Enable( this, IDC_EDIT_GLOWBRIGHT, TRUE );
		SetWin_Enable( this, IDC_EDIT_GLOWALPHA, TRUE );
		SetWin_Enable( this, IDC_EDIT_GLOWBRIGHT, TRUE );
		SetWin_Enable( this, IDC_EDIT_GLOWSPEED, TRUE );
		SetWin_Text( this, IDC_EDIT_TEXTURE_GLOWTEX, m_Property.m_szGlowTex );
	}else{
		SetWin_Enable( this, IDC_BUTTON_TEXTURE_GLOWTEX, FALSE );
		SetWin_Enable( this, IDC_EDIT_GLOWBRIGHT, FALSE );
		SetWin_Enable( this, IDC_EDIT_GLOWALPHA, FALSE );
		SetWin_Enable( this, IDC_EDIT_GLOWBRIGHT, FALSE );
		SetWin_Enable( this, IDC_EDIT_GLOWSPEED, FALSE );
		SetWin_Text( this, IDC_EDIT_TEXTURE_GLOWTEX, "" );
	}

	if( m_Property.m_bUseReflect )
	{
		SetWin_Text( this, IDC_EDIT_TEXTURE_REFLECTTEX, m_Property.m_szReflectTex );
		SetWin_Enable( this, IDC_CHECK_REFLECTTEX2, TRUE );
		SetWin_Enable( this, IDC_BUTTON_TEXTURE_REFLECTTEX, TRUE );
		SetWin_Enable( this, IDC_EDIT_REFLECTACT, TRUE );
	}else{
		ZeroMemory( m_Property.m_szReflectTex, MAX_PATH );
		SetWin_Text( this, IDC_EDIT_TEXTURE_REFLECTTEX, m_Property.m_szReflectTex );
		SetWin_Enable( this, IDC_CHECK_REFLECTTEX2, FALSE );
		SetWin_Enable( this, IDC_BUTTON_TEXTURE_REFLECTTEX, FALSE );
		SetWin_Enable( this, IDC_EDIT_REFLECTACT, FALSE );
	}

	if( m_Property.m_bUseAlphaReflect )
	{
		SetWin_Text( this, IDC_EDIT_TEXTURE_REFLECTTEX2, m_Property.m_szReflectAlphaTex );
		SetWin_Enable( this, IDC_BUTTON_TEXTURE_REFLECTTEX2, TRUE );
	}else{
		//ZeroMemory( m_Property.m_szReflectAlphaTex, MAX_PATH );
		SetWin_Text( this, IDC_EDIT_TEXTURE_REFLECTTEX2, m_Property.m_szReflectAlphaTex );
		SetWin_Enable( this, IDC_BUTTON_TEXTURE_REFLECTTEX2, FALSE );
	}

	
}

void CEffOverlayPage::OnBnClickedButtonApply()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();

	StringCchCopy( m_Property.m_szOverlayTex, MAX_PATH, GetWin_Text( this, IDC_EDIT_TEXTURE ).GetString() );
	StringCchCopy( m_Property.m_szGlowTex,	MAX_PATH, GetWin_Text( this, IDC_EDIT_TEXTURE_GLOWTEX ).GetString() );

	int nMin, nMax;
	float fSpecularPower, fSpecularLevel, fAlphaRef;

	fSpecularPower = GetWin_Num_float( this, IDC_EDIT_SPECPOWER );
	m_slider_SpecPW.GetRange( nMin, nMax );
	if ( fSpecularPower < nMin )
		fSpecularPower = (float)nMin;
	else if ( fSpecularPower > nMax )
		fSpecularPower = (float)nMax;
	m_Property.m_fSpecularPower = fSpecularPower;
	m_slider_SpecPW.SetPos( (int)m_Property.m_fSpecularPower );

	fSpecularLevel = GetWin_Num_float( this, IDC_EDIT_SPECLEVEL );
	m_slider_SpecLV.GetRange( nMin, nMax );
	if ( fSpecularLevel < nMin )
		fSpecularLevel = (float)nMin;
	else if ( fSpecularLevel > nMax )
		fSpecularLevel = (float)nMax;
	m_Property.m_fSpecularLevel = fSpecularLevel * 0.1f;
	m_slider_SpecLV.SetPos( (int)(m_Property.m_fSpecularLevel * 10.0f) );

	fAlphaRef = GetWin_Num_float( this, IDC_EDIT_ALPHA_REF );
	m_slider_AlphaRef.GetRange( nMin, nMax );
	if ( fAlphaRef < nMin )
		fAlphaRef = (float)nMin;
	else if ( fAlphaRef > nMax )
		fAlphaRef = (float)nMax;
	m_Property.m_fAlphaRef = fAlphaRef;
	m_slider_AlphaRef.SetPos( (int)(m_Property.m_fAlphaRef) );

	m_Property.m_dwSelectMaterial = m_combo_Material.GetCurSel();
	m_Property.m_bUseOverlay[ m_Property.m_dwSelectMaterial ] = GetWin_Check( this, IDC_CHECK_USE_OVERLAY );

	m_Property.m_fGlowAlpha  = min( GetWin_Num_float( this, IDC_EDIT_GLOWALPHA ), 1.0f );
	m_Property.m_fGlowBright = GetWin_Num_float( this, IDC_EDIT_GLOWBRIGHT );
	m_Property.m_fGlowAniSpeed = GetWin_Num_float( this, IDC_EDIT_GLOWSPEED );

	StringCchCopy( m_Property.m_szReflectTex, MAX_PATH, GetWin_Text( this, IDC_EDIT_TEXTURE_REFLECTTEX ).GetString() );
	StringCchCopy( m_Property.m_szReflectAlphaTex, MAX_PATH, GetWin_Text( this, IDC_EDIT_TEXTURE_REFLECTTEX2 ).GetString() );
	m_Property.m_fReflectActivity = GetWin_Num_float( this, IDC_EDIT_REFLECTACT );

	m_pEffChar->SetProperty ( &m_Property );
	m_pEffChar->Create ( pView->GetD3dDevice() );

    EffectApply();
}

void CEffOverlayPage::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	OnBnClickedButtonApply();
	OnBnClickedCancel();
}

void CEffOverlayPage::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    ReturnPage();

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	pView->SetRenderTypeDefault();
}

BOOL CEffOverlayPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_slider_SpecPW.SetRange( 0, 100 );
	m_slider_SpecLV.SetRange( 0, 100 );
	m_slider_AlphaRef.SetRange( 0, 255 );

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CEffOverlayPage::OnBnClickedButtonTexture()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString szFilter = _T("Texture Image (*.bmp,*.dds,*.tga,*.jpg)|*.bmp;*.dds;*.tga;*.jpg|");
	CFileDialog dlg ( TRUE, _T(".BMP"), NULL, 
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

void CEffOverlayPage::OnBnClickedButtonColor1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int ThisR = int ( m_Property.m_cColor1.r * 255.0f );
	int ThisG = int ( m_Property.m_cColor1.g * 255.0f );
	int ThisB = int ( m_Property.m_cColor1.b * 255.0f );

	if ( !m_pColorPicker )
	{
		m_originColor = m_Property.m_cColor1;

		m_pColorPicker = new CColorPickerDlg( RGB(ThisR,ThisG,ThisB) );
		m_pColorPicker->Create( IDD_DIALOG_COLORS );
		m_pColorPicker->SetIndex( COLOR1 );

		RECT recMain, recColor;
		CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
		pFrame->GetWindowRect( &recMain );
		m_pColorPicker->GetWindowRect( &recColor );

		recColor.left = recMain.right - ( recColor.right - recColor.left );
		recColor.top = recMain.bottom - ( recColor.bottom - recColor.top );
		recColor.right = recMain.right;
		recColor.bottom = recMain.bottom;
		m_pColorPicker->MoveWindow( &recColor );

		m_pColorPicker->ShowWindow( SW_SHOW );

		m_nColorIndex = COLOR1;
	}
	else
		m_pColorPicker->SetFocus();

	/*CColorPickerDlg ColorPicker( RGB(ThisR,ThisG,ThisB) );

	if ( IDOK == ColorPicker.DoModal() )
	{
		COLORREF SelectColor = ColorPicker.GetColor ();
		m_Property.m_cColor1.r = GetRValue(SelectColor)/255.0f;
		m_Property.m_cColor1.g = GetGValue(SelectColor)/255.0f;
		m_Property.m_cColor1.b = GetBValue(SelectColor)/255.0f;

		OnBnClickedButtonApply();
	}*/
}

void CEffOverlayPage::OnBnClickedButtonColor2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int ThisR = int ( m_Property.m_cColor2.r * 255.0f );
	int ThisG = int ( m_Property.m_cColor2.g * 255.0f );
	int ThisB = int ( m_Property.m_cColor2.b * 255.0f );

	if ( !m_pColorPicker )
	{
		m_pColorPicker = new CColorPickerDlg( RGB(ThisR,ThisG,ThisB) );
		m_pColorPicker->Create( IDD_DIALOG_COLORS );
		m_pColorPicker->SetIndex( COLOR2 );

		RECT recMain, recColor;
		CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
		pFrame->GetWindowRect( &recMain );
		m_pColorPicker->GetWindowRect( &recColor );

		recColor.left = recMain.right - ( recColor.right - recColor.left );
		recColor.top = recMain.bottom - ( recColor.bottom - recColor.top );
		recColor.right = recMain.right;
		recColor.bottom = recMain.bottom;
		m_pColorPicker->MoveWindow( &recColor );

		m_pColorPicker->ShowWindow( SW_SHOW );

		m_nColorIndex = COLOR2;
	}
	else
		m_pColorPicker->SetFocus();

	/*CColorPickerDlg ColorPicker ( RGB(ThisR,ThisG,ThisB) );

	if ( IDOK == ColorPicker.DoModal() )
	{
		COLORREF SelectColor = ColorPicker.GetColor ();
		m_Property.m_cColor2.r = GetRValue(SelectColor)/255.0f;
		m_Property.m_cColor2.g = GetGValue(SelectColor)/255.0f;
		m_Property.m_cColor2.b = GetBValue(SelectColor)/255.0f;

		OnBnClickedButtonApply();
	}*/
}

void CEffOverlayPage::OnBnClickedButtonColor4()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int ThisR = int ( m_Property.m_cAmbient.r * 255.0f );
	int ThisG = int ( m_Property.m_cAmbient.g * 255.0f );
	int ThisB = int ( m_Property.m_cAmbient.b * 255.0f );

	if ( !m_pColorPicker )
	{
		m_originColor = m_Property.m_cAmbient;

		m_pColorPicker = new CColorPickerDlg( RGB(ThisR,ThisG,ThisB) );
		m_pColorPicker->Create( IDD_DIALOG_COLORS );
		m_pColorPicker->SetIndex( COLOR4 );

		RECT recMain, recColor;
		CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
		pFrame->GetWindowRect( &recMain );
		m_pColorPicker->GetWindowRect( &recColor );

		recColor.left = recMain.right - ( recColor.right - recColor.left );
		recColor.top = recMain.bottom - ( recColor.bottom - recColor.top );
		recColor.right = recMain.right;
		recColor.bottom = recMain.bottom;
		m_pColorPicker->MoveWindow( &recColor );

		m_pColorPicker->ShowWindow( SW_SHOW );

		m_nColorIndex = COLOR4;
	}
	else
		m_pColorPicker->SetFocus();

	/*CColorPickerDlg ColorPicker ( RGB(ThisR,ThisG,ThisB) );


	if ( IDOK == ColorPicker.DoModal() )
	{
		COLORREF SelectColor = ColorPicker.GetColor ();
		m_Property.m_cAmbient.r = GetRValue(SelectColor)/255.0f;
		m_Property.m_cAmbient.g = GetGValue(SelectColor)/255.0f;
		m_Property.m_cAmbient.b = GetBValue(SelectColor)/255.0f;

		OnBnClickedButtonApply();
	}*/
}

void CEffOverlayPage::OnBnClickedButtonColor5()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int ThisR = int ( m_Property.m_cDiffuse.r * 255.0f );
	int ThisG = int ( m_Property.m_cDiffuse.g * 255.0f );
	int ThisB = int ( m_Property.m_cDiffuse.b * 255.0f );

	if ( !m_pColorPicker )
	{
		m_originColor = m_Property.m_cDiffuse;

		m_pColorPicker = new CColorPickerDlg( RGB(ThisR,ThisG,ThisB) );
		m_pColorPicker->Create( IDD_DIALOG_COLORS );
		m_pColorPicker->SetIndex( COLOR5 );

		RECT recMain, recColor;
		CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
		pFrame->GetWindowRect( &recMain );
		m_pColorPicker->GetWindowRect( &recColor );

		recColor.left = recMain.right - ( recColor.right - recColor.left );
		recColor.top = recMain.bottom - ( recColor.bottom - recColor.top );
		recColor.right = recMain.right;
		recColor.bottom = recMain.bottom;
		m_pColorPicker->MoveWindow( &recColor );

		m_pColorPicker->ShowWindow( SW_SHOW );

		m_nColorIndex = COLOR5;
	}
	else
		m_pColorPicker->SetFocus();

	/*CColorPickerDlg ColorPicker ( RGB(ThisR,ThisG,ThisB) );

	if ( IDOK == ColorPicker.DoModal() )
	{
		COLORREF SelectColor = ColorPicker.GetColor ();
		m_Property.m_cDiffuse.r = GetRValue(SelectColor)/255.0f;
		m_Property.m_cDiffuse.g = GetGValue(SelectColor)/255.0f;
		m_Property.m_cDiffuse.b = GetBValue(SelectColor)/255.0f;

		OnBnClickedButtonApply();
	}*/
}

void CEffOverlayPage::OnBnClickedButtonColor6()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int ThisR = int ( m_Property.m_cSpecular.r * 255.0f );
	int ThisG = int ( m_Property.m_cSpecular.g * 255.0f );
	int ThisB = int ( m_Property.m_cSpecular.b * 255.0f );

	if ( !m_pColorPicker )
	{
		m_originColor = m_Property.m_cSpecular;

		m_pColorPicker = new CColorPickerDlg( RGB(ThisR,ThisG,ThisB) );
		m_pColorPicker->Create( IDD_DIALOG_COLORS );
		m_pColorPicker->SetIndex( COLOR6 );

		RECT recMain, recColor;
		CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
		pFrame->GetWindowRect( &recMain );
		m_pColorPicker->GetWindowRect( &recColor );

		recColor.left = recMain.right - ( recColor.right - recColor.left );
		recColor.top = recMain.bottom - ( recColor.bottom - recColor.top );
		recColor.right = recMain.right;
		recColor.bottom = recMain.bottom;
		m_pColorPicker->MoveWindow( &recColor );

		m_pColorPicker->ShowWindow( SW_SHOW );

		m_nColorIndex = COLOR6;
	}
	else
		m_pColorPicker->SetFocus();

	/*CColorPickerDlg ColorPicker ( RGB(ThisR,ThisG,ThisB) );

	if ( IDOK == ColorPicker.DoModal() )
	{
		COLORREF SelectColor = ColorPicker.GetColor ();
		m_Property.m_cSpecular.r = GetRValue(SelectColor)/255.0f;
		m_Property.m_cSpecular.g = GetGValue(SelectColor)/255.0f;
		m_Property.m_cSpecular.b = GetBValue(SelectColor)/255.0f;

		OnBnClickedButtonApply();
	}*/
}

void CEffOverlayPage::OnNMCustomdrawSliderSpecpower(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	SetWin_Num_float( this, IDC_EDIT_SPECPOWER, (float)m_slider_SpecPW.GetPos() );
	*pResult = 0;
}

void CEffOverlayPage::OnNMCustomdrawSliderSpeclevel(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	SetWin_Num_float( this, IDC_EDIT_SPECLEVEL, (float)m_slider_SpecLV.GetPos() );
	*pResult = 0;
}

void CEffOverlayPage::OnBnClickedCheckGlowtex()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_Property.m_bUseGlowTex = GetWin_Check( this, IDC_CHECK_GLOWTEX );

	if( m_Property.m_bUseGlowTex )
	{
		SetWin_Enable( this, IDC_BUTTON_TEXTURE_GLOWTEX, TRUE );
		SetWin_Enable( this, IDC_EDIT_GLOWBRIGHT, TRUE );
		SetWin_Enable( this, IDC_EDIT_GLOWALPHA, TRUE );
		SetWin_Enable( this, IDC_EDIT_GLOWBRIGHT, TRUE );
		SetWin_Enable( this, IDC_EDIT_GLOWSPEED, TRUE );
	}else{
		ZeroMemory( m_Property.m_szGlowTex, MAX_PATH );
		SetWin_Enable( this, IDC_BUTTON_TEXTURE_GLOWTEX, FALSE );
		SetWin_Enable( this, IDC_EDIT_GLOWBRIGHT, FALSE );
		SetWin_Enable( this, IDC_EDIT_GLOWALPHA, FALSE );
		SetWin_Enable( this, IDC_EDIT_GLOWBRIGHT, FALSE );
		SetWin_Enable( this, IDC_EDIT_GLOWSPEED, FALSE );
	}
}

void CEffOverlayPage::OnBnClickedButtonTextureGlowtex()
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
		pWnd = GetDlgItem ( IDC_EDIT_TEXTURE_GLOWTEX );
		pWnd->SetWindowText ( FilePath );
	}
}

void CEffOverlayPage::OnBnClickedCheckReflecttex()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_Property.m_bUseReflect = GetWin_Check( this, IDC_CHECK_REFLECTTEX );

	if( m_Property.m_bUseReflect )
	{
		SetWin_Text( this, IDC_EDIT_TEXTURE_REFLECTTEX, m_Property.m_szReflectTex );
//		SetWin_Text( this, IDC_EDIT_TEXTURE_REFLECTTEX2, m_Property.m_szReflectAlphaTex );
		SetWin_Enable( this, IDC_CHECK_REFLECTTEX2, TRUE );
		SetWin_Enable( this, IDC_BUTTON_TEXTURE_REFLECTTEX, TRUE );
		SetWin_Enable( this, IDC_EDIT_REFLECTACT, TRUE );
	}else{
		ZeroMemory( m_Property.m_szReflectTex, MAX_PATH );
		SetWin_Text( this, IDC_EDIT_TEXTURE_REFLECTTEX, m_Property.m_szReflectTex );
		SetWin_Text( this, IDC_EDIT_TEXTURE_REFLECTTEX2, m_Property.m_szReflectAlphaTex );
		SetWin_Check( this, IDC_CHECK_REFLECTTEX2, FALSE );
		SetWin_Enable( this, IDC_CHECK_REFLECTTEX2, FALSE );
		SetWin_Enable( this, IDC_BUTTON_TEXTURE_REFLECTTEX, FALSE );
		SetWin_Enable( this, IDC_BUTTON_TEXTURE_REFLECTTEX2, FALSE );
		SetWin_Enable( this, IDC_EDIT_REFLECTACT, FALSE );

		m_Property.m_bUseAlphaReflect = FALSE;
	}
}

void CEffOverlayPage::OnBnClickedButtonTextureReflecttex()
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
		pWnd = GetDlgItem ( IDC_EDIT_TEXTURE_REFLECTTEX );
		pWnd->SetWindowText ( FilePath );
	}
}

void CEffOverlayPage::OnBnClickedButtonTextureReflecttex2()
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
		pWnd = GetDlgItem ( IDC_EDIT_TEXTURE_REFLECTTEX2 );
		pWnd->SetWindowText ( FilePath );
	}
}

void CEffOverlayPage::OnBnClickedCheckReflecttex2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_Property.m_bUseAlphaReflect = GetWin_Check( this, IDC_CHECK_REFLECTTEX2 );

	if( m_Property.m_bUseAlphaReflect )
	{
		SetWin_Text( this, IDC_EDIT_TEXTURE_REFLECTTEX2, m_Property.m_szReflectAlphaTex );
		SetWin_Enable( this, IDC_BUTTON_TEXTURE_REFLECTTEX2, TRUE );
	}else{
		//ZeroMemory( m_Property.m_szReflectAlphaTex, MAX_PATH );
		SetWin_Text( this, IDC_EDIT_TEXTURE_REFLECTTEX2, m_Property.m_szReflectAlphaTex );
		SetWin_Enable( this, IDC_BUTTON_TEXTURE_REFLECTTEX2, FALSE );
	}
}

void CEffOverlayPage::OnBnClickedUseSpecular()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_Property.m_bUseSpecular = GetWin_Check( this, IDC_CHECK_USE_SPECULAR );

	if ( m_Property.m_bUseSpecular )
	{
		SetWin_Enable( this, IDC_SLIDER_SPECLEVEL, FALSE );
		SetWin_ShowWindow( this, IDC_SLIDER_SPECLEVEL, FALSE );
		SetWin_ShowWindow( this, IDC_EDIT_SPECLEVEL, FALSE );
	}
	else
	{
		SetWin_Enable( this, IDC_SLIDER_SPECLEVEL, TRUE );
		SetWin_ShowWindow( this, IDC_SLIDER_SPECLEVEL, TRUE );
		SetWin_ShowWindow( this, IDC_EDIT_SPECLEVEL, TRUE );
	}
}

void CEffOverlayPage::OnBnClickedCheckUseOverlay()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_Property.m_bUseOverlay[ m_combo_Material.GetCurSel() ] = GetWin_Check( this, IDC_CHECK_USE_OVERLAY );
}

void CEffOverlayPage::OnCbnSelchangeComboMatrial()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//if ( m_iState == MODIFY_OVERLAY )
	{
		if ( m_pEffChar )
		{
			DxEffCharOverlay* pEffCharOverlay = ((DxEffCharOverlay*)m_pEffChar);
			m_Property.Assign( pEffCharOverlay->GetPropertyOverlay( m_combo_Material.GetCurSel()), m_combo_Material.GetCurSel() );
		}
	}

	SetWin_Check( this, IDC_CHECK_USE_OVERLAY, m_Property.m_bUseOverlay[ m_combo_Material.GetCurSel() ] );

	SetProperty( &m_Property );
	OnBnClickedButtonApply();
}

void CEffOverlayPage::OnNMCustomdrawSliderAlphaRef(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	SetWin_Num_float( this, IDC_EDIT_ALPHA_REF, (float)m_slider_AlphaRef.GetPos() );
	*pResult = 0;
}

void CEffOverlayPage::SetSpinControl( int nColorHeight )
{
	if ( !m_pColorPicker )	return;

	m_pColorPicker->SetSpinControl( nColorHeight );
    FrameMove();
}

void CEffOverlayPage::FrameMove()
{
    CCharEditApp *pApp = (CCharEditApp*) AfxGetApp();
    if ( !pApp )
        return;

	if ( !m_pColorPicker )
    {
        pApp->OnColorPickerWnd( false );

        return;
    }
    else
        pApp->OnColorPickerWnd( true );

	DWORD dwKey = DxInputDevice::GetInstance().GetKeyState( DIK_TAB );

	if ( dwKey & DXKEY_UP )
	{
		m_pColorPicker->NextDlgCtrl();
	}

	DrawColor( m_nColorIndex );

	if ( g_iColorResult == IDOK )
	{
		DrawSelectColor( m_nColorIndex );

		m_pColorPicker = NULL;
	}
	else if ( g_iColorResult == IDCANCEL )
	{
		switch ( m_nColorIndex )
		{
		case COLOR1:
			m_Property.m_cColor1 = m_originColor;
			OnBnClickedButtonApply();
			break;

		case COLOR2:
			m_Property.m_cColor2 = m_originColor;
			OnBnClickedButtonApply();
			break;

		case COLOR4:
			m_Property.m_cAmbient = m_originColor;
			OnBnClickedButtonApply();
			break;

		case  COLOR5:
			m_Property.m_cDiffuse = m_originColor;
			OnBnClickedButtonApply();
			break;

		case COLOR6:
			m_Property.m_cSpecular = m_originColor;
			OnBnClickedButtonApply();
			break;
		}

		m_pColorPicker = NULL;
	}
	else if ( g_iColorResult >= 0 )
	{
		m_pColorPicker = NULL;
	}
}

void CEffOverlayPage::DrawColor( int nIndex )
{
	COLORREF SelectColor;

	switch ( nIndex )
	{
	case COLOR1:
		SelectColor = m_pColorPicker->GetColor();
		m_Property.m_cColor1.r = GetRValue(SelectColor)/255.0f;
		m_Property.m_cColor1.g = GetGValue(SelectColor)/255.0f;
		m_Property.m_cColor1.b = GetBValue(SelectColor)/255.0f;

		OnBnClickedButtonApply();
		break;

	case COLOR2:
		SelectColor = m_pColorPicker->GetColor();
		m_Property.m_cColor2.r = GetRValue(SelectColor)/255.0f;
		m_Property.m_cColor2.g = GetGValue(SelectColor)/255.0f;
		m_Property.m_cColor2.b = GetBValue(SelectColor)/255.0f;

		OnBnClickedButtonApply();
		break;

	case COLOR4:
		SelectColor = m_pColorPicker->GetColor();
		m_Property.m_cAmbient.r = GetRValue(SelectColor)/255.0f;
		m_Property.m_cAmbient.g = GetGValue(SelectColor)/255.0f;
		m_Property.m_cAmbient.b = GetBValue(SelectColor)/255.0f;

		OnBnClickedButtonApply();
		break;

	case  COLOR5:
		SelectColor = m_pColorPicker->GetColor();
		m_Property.m_cDiffuse.r = GetRValue(SelectColor)/255.0f;
		m_Property.m_cDiffuse.g = GetGValue(SelectColor)/255.0f;
		m_Property.m_cDiffuse.b = GetBValue(SelectColor)/255.0f;

		OnBnClickedButtonApply();
		break;

	case COLOR6:
		SelectColor = m_pColorPicker->GetColor();
		m_Property.m_cSpecular.r = GetRValue(SelectColor)/255.0f;
		m_Property.m_cSpecular.g = GetGValue(SelectColor)/255.0f;
		m_Property.m_cSpecular.b = GetBValue(SelectColor)/255.0f;

		OnBnClickedButtonApply();
		break;
	}
}

void CEffOverlayPage::DrawSelectColor( int nIndex )
{
	COLORREF SelectColor;

	switch ( nIndex )
	{
	case COLOR1:
		SelectColor = g_ColorRef;
		m_Property.m_cColor1.r = GetRValue(SelectColor)/255.0f;
		m_Property.m_cColor1.g = GetGValue(SelectColor)/255.0f;
		m_Property.m_cColor1.b = GetBValue(SelectColor)/255.0f;

		OnBnClickedButtonApply();
		break;

	case COLOR2:
		SelectColor = g_ColorRef;
		m_Property.m_cColor2.r = GetRValue(SelectColor)/255.0f;
		m_Property.m_cColor2.g = GetGValue(SelectColor)/255.0f;
		m_Property.m_cColor2.b = GetBValue(SelectColor)/255.0f;

		OnBnClickedButtonApply();
		break;

	case COLOR4:
		SelectColor = g_ColorRef;
		m_Property.m_cAmbient.r = GetRValue(SelectColor)/255.0f;
		m_Property.m_cAmbient.g = GetGValue(SelectColor)/255.0f;
		m_Property.m_cAmbient.b = GetBValue(SelectColor)/255.0f;

		OnBnClickedButtonApply();
		break;

	case  COLOR5:
		SelectColor = g_ColorRef;
		m_Property.m_cDiffuse.r = GetRValue(SelectColor)/255.0f;
		m_Property.m_cDiffuse.g = GetGValue(SelectColor)/255.0f;
		m_Property.m_cDiffuse.b = GetBValue(SelectColor)/255.0f;

		OnBnClickedButtonApply();
		break;

	case COLOR6:
		SelectColor = g_ColorRef;
		m_Property.m_cSpecular.r = GetRValue(SelectColor)/255.0f;
		m_Property.m_cSpecular.g = GetGValue(SelectColor)/255.0f;
		m_Property.m_cSpecular.b = GetBValue(SelectColor)/255.0f;

		OnBnClickedButtonApply();
		break;
	}
}
void CEffOverlayPage::OnBnClickedCheckEnableChange()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if ( GetWin_Check( this, IDC_CHECK_ENABLE_CHANGE ) )
		m_Property.m_nSelectOverlay = m_combo_Material.GetCurSel();
	else
		m_Property.m_nSelectOverlay = 0;

	OnBnClickedButtonApply();
}
