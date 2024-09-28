// EffTotalShaderPage.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "../MainFrm.h"
#include "../CharEditDoc.h"
#include "../CharEditView.h"

#include "sheetWithTabChar.h"
#include "sheetWithTabTS.h"
#include "../Dialog/ColorPickerDlg.h"

#include "../MfcExLib/ExLibs/EtcFunction.h"
#include "EffTotalShaderPage.h"
#include ".\efftotalshaderpage.h"


// CEffTotalShaderPage 대화 상자입니다.

IMPLEMENT_DYNAMIC(CEffTotalShaderPage, CPropertyPage)
CEffTotalShaderPage::CEffTotalShaderPage()
	: CEffCharPage(CEffTotalShaderPage::IDD)
	, m_pSheetTS(NULL)
{
}

CEffTotalShaderPage::~CEffTotalShaderPage()
{
}

void CEffTotalShaderPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_SLIDER_SPECPOWER, m_slider_SpecPW);
}


BEGIN_MESSAGE_MAP(CEffTotalShaderPage, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnBnClickedButtonApply)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(IDC_CHECK_USEEFFECT1, OnBnClickedCheckUseeffect1)
	ON_BN_CLICKED(IDC_CHECK_USEEFFECT2, OnBnClickedCheckUseeffect2)
	ON_BN_CLICKED(IDC_CHECK_USEEFFECT3, OnBnClickedCheckUseeffect3)
	ON_BN_CLICKED(IDC_CHECK_USEEFFECT4, OnBnClickedCheckUseeffect4)
	ON_BN_CLICKED(IDC_RADIO_SHADEREFFECT1, OnBnClickedRadioShadereffect1)
	ON_BN_CLICKED(IDC_RADIO_SHADEREFFECT2, OnBnClickedRadioShadereffect2)
	ON_BN_CLICKED(IDC_RADIO_SHADEREFFECT3, OnBnClickedRadioShadereffect3)
	ON_BN_CLICKED(IDC_RADIO_SHADEREFFECT4, OnBnClickedRadioShadereffect4)
	ON_BN_CLICKED(IDC_BUTTON_COLOR, OnBnClickedButtonColor)
	ON_BN_CLICKED(IDC_BUTTON_COLOR2, OnBnClickedButtonColor2)
	ON_BN_CLICKED(IDC_BUTTON_COLOR3, OnBnClickedButtonColor3)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_SPECPOWER, OnNMCustomdrawSliderSpecpower)
END_MESSAGE_MAP()


// CEffTotalShaderPage 메시지 처리기입니다.
void CEffTotalShaderPage::SetProperty ( EFFCHAR_PROPERTY *pProp )
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxSkinPiece *pSkinPiece = pView->GetSkinPiece ();

	m_slider_SpecPW.SetRange( 0, 99 );

	if ( pProp )
		m_Property = *((EFFCHAR_PROPERTY_TOTALSHADER*)pProp);
	else
	{
		EFFCHAR_PROPERTY_TOTALSHADER sProp;
		m_Property = sProp;
	}

	if ( !m_pEffChar )
	{
		m_pEffChar = DxEffCharMan::GetInstance().CreateEffect
			(
			DxEffCharTotalShader::TYPEID,
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

	UpdateCheckWindow();
	UpdateEnableWindow();

	m_pSheetTS->TSSetPageActive( 0, m_Property );
	m_pSheetTS->TSSetPageActive( 1, m_Property );
	m_pSheetTS->TSSetPageActive( 2, m_Property );
	m_pSheetTS->TSSetPageActive( 3, m_Property );


	if( m_Property.m_bUseNormalMap )
		m_pSheetTS->TSSetPageActive( 0, m_Property );
	else if( m_Property.m_bUseOverlay )
		m_pSheetTS->TSSetPageActive( 1, m_Property );
	else if( m_Property.m_bUseReflect )
		m_pSheetTS->TSSetPageActive( 2, m_Property );
	else if( m_Property.m_bUseTexUpDown )
		m_pSheetTS->TSSetPageActive( 3, m_Property );

	SetWin_Num_float( this, IDC_EDIT_SPECPOWER, m_Property.m_fSpecularPower );
	m_slider_SpecPW.SetPos( (int)m_Property.m_fSpecularPower);
	

	CheckRadioButton( IDC_RADIO_SHADEREFFECT1, IDC_RADIO_SHADEREFFECT4, IDC_RADIO_SHADEREFFECT1 + m_pSheetTS->GetActiveIndex() );


}
void CEffTotalShaderPage::OnBnClickedButtonApply()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if( !m_pEffChar ) return;

	m_Property.m_bUseNormalMap = GetWin_Check( this, IDC_CHECK_USEEFFECT1 );
	m_Property.m_bUseOverlay   = GetWin_Check( this, IDC_CHECK_USEEFFECT2 );
	m_Property.m_bUseReflect   = GetWin_Check( this, IDC_CHECK_USEEFFECT3 );
	m_Property.m_bUseTexUpDown = GetWin_Check( this, IDC_CHECK_USEEFFECT4 );

	m_Property.m_fSpecularPower = GetWin_Num_float( this, IDC_EDIT_SPECPOWER );


	if( m_Property.m_bUseNormalMap )
	{
		m_pSheetTS->m_EffTS_NormalMap.Apply();
		m_Property.m_ShaderNormalMap = m_pSheetTS->m_EffTS_NormalMap.GetProperty(); 
	}

	if( m_Property.m_bUseOverlay )
	{
		m_pSheetTS->m_EffTS_Overlay.Apply();
		m_Property.m_ShaderOverlay = m_pSheetTS->m_EffTS_Overlay.GetProperty();
	}

	if( m_Property.m_bUseTexUpDown )
	{
		m_pSheetTS->m_EffTS_TexUpDown.Apply();
		m_Property.m_ShaderTexUpDown = m_pSheetTS->m_EffTS_TexUpDown.GetProperty(); 
	}

	if( m_Property.m_bUseReflect )
	{
		m_pSheetTS->m_EffTS_Reflection.Apply();
		m_Property.m_ShaderReflection = m_pSheetTS->m_EffTS_Reflection.GetProperty();
	}

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();

	m_pEffChar->SetProperty ( &m_Property );
	m_pEffChar->Create ( pView->GetD3dDevice() );
	
    EffectApply();
}

void CEffTotalShaderPage::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	OnBnClickedButtonApply();
	OnBnClickedCancel();
}

void CEffTotalShaderPage::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    ReturnPage();

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	pView->SetRenderTypeDefault();
}

BOOL CEffTotalShaderPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();


	CRect ClientRect, SheetRect;
	CWnd *pWnd;

	//	Note : CsheetWithTabFrame 를 만든다.


	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	pWnd = GetDlgItem ( IDC_STATIC_SHEETTAP1 );
	pWnd->GetClientRect( &ClientRect ); 
	pWnd->GetWindowRect ( SheetRect );
//	pWnd->ScreenToClient( &SheetRect ); 
	ScreenToClient ( &SheetRect );
	ClientRect.top = SheetRect.top;

	
	m_pSheetTS = new CsheetWithTabTS ( ClientRect, this );
	m_pSheetTS->Create ( this, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0 );

	CheckRadioButton( IDC_RADIO_SHADEREFFECT1, IDC_RADIO_SHADEREFFECT4, IDC_RADIO_SHADEREFFECT1 );

	// 1: Normal Map
	// 2: Overlay
	// 3: Reflection
	// 4: Texture Up Down

	


	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CEffTotalShaderPage::UpdateCheckWindow()
{
	SetWin_Check( this, IDC_CHECK_USEEFFECT1, m_Property.m_bUseNormalMap );
	SetWin_Check( this, IDC_CHECK_USEEFFECT2, m_Property.m_bUseOverlay );
	SetWin_Check( this, IDC_CHECK_USEEFFECT3, m_Property.m_bUseReflect );
	SetWin_Check( this, IDC_CHECK_USEEFFECT4, m_Property.m_bUseTexUpDown );
}

void CEffTotalShaderPage::UpdateEnableWindow()
{
	int nSelectIndex = 0;
	int nSelectNum = 0;
	for( int i = 0; i < 4; i++ )
	{
		if( GetWin_Check( this, IDC_CHECK_USEEFFECT1 + i ) )
		{

			nSelectIndex = i;
			nSelectNum++;
			SetWin_Enable( this, IDC_RADIO_SHADEREFFECT1 + i, TRUE );

		}else{
			SetWin_Enable( this, IDC_RADIO_SHADEREFFECT1 + i, FALSE );
		}
	}

	if( nSelectNum == 1 )
	{
		m_pSheetTS->TSSetPageActive( nSelectIndex, m_Property );
		CheckRadioButton( IDC_RADIO_SHADEREFFECT1, IDC_RADIO_SHADEREFFECT4, IDC_RADIO_SHADEREFFECT1 + nSelectIndex );
	}
	m_pSheetTS->ShowWindow( nSelectNum != 0 );
}


void CEffTotalShaderPage::OnBnClickedCheckUseeffect1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateEnableWindow();
}

void CEffTotalShaderPage::OnBnClickedCheckUseeffect2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateEnableWindow();
}

void CEffTotalShaderPage::OnBnClickedCheckUseeffect3()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateEnableWindow();
}

void CEffTotalShaderPage::OnBnClickedCheckUseeffect4()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateEnableWindow();
}


void CEffTotalShaderPage::OnBnClickedRadioShadereffect1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pSheetTS->ShowWindow(TRUE);
	m_pSheetTS->TSSetPageActive( 0, m_Property );
}

void CEffTotalShaderPage::OnBnClickedRadioShadereffect2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pSheetTS->ShowWindow(TRUE);
	m_pSheetTS->TSSetPageActive( 1, m_Property );
}

void CEffTotalShaderPage::OnBnClickedRadioShadereffect3()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pSheetTS->ShowWindow(TRUE);
	m_pSheetTS->TSSetPageActive( 2, m_Property );
}

void CEffTotalShaderPage::OnBnClickedRadioShadereffect4()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pSheetTS->ShowWindow(TRUE);
	m_pSheetTS->TSSetPageActive( 3, m_Property );
}

void CEffTotalShaderPage::OnBnClickedButtonColor()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int ThisR = int ( m_Property.m_cAmbient.r * 255.0f );
	int ThisG = int ( m_Property.m_cAmbient.g * 255.0f );
	int ThisB = int ( m_Property.m_cAmbient.b * 255.0f );

	CColorPickerDlg ColorPicker ( RGB(ThisR,ThisG,ThisB) );

	if ( IDOK == ColorPicker.DoModal() )
	{
		COLORREF SelectColor = ColorPicker.GetColor ();
		m_Property.m_cAmbient.r = GetRValue(SelectColor)/255.0f;
		m_Property.m_cAmbient.g = GetGValue(SelectColor)/255.0f;
		m_Property.m_cAmbient.b = GetBValue(SelectColor)/255.0f;

		OnBnClickedButtonApply();
	}
}

void CEffTotalShaderPage::OnBnClickedButtonColor2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int ThisR = int ( m_Property.m_cDiffuse.r * 255.0f );
	int ThisG = int ( m_Property.m_cDiffuse.g * 255.0f );
	int ThisB = int ( m_Property.m_cDiffuse.b * 255.0f );

	CColorPickerDlg ColorPicker ( RGB(ThisR,ThisG,ThisB) );

	if ( IDOK == ColorPicker.DoModal() )
	{
		COLORREF SelectColor = ColorPicker.GetColor ();
		m_Property.m_cDiffuse.r = GetRValue(SelectColor)/255.0f;
		m_Property.m_cDiffuse.g = GetGValue(SelectColor)/255.0f;
		m_Property.m_cDiffuse.b = GetBValue(SelectColor)/255.0f;

		OnBnClickedButtonApply();
	}
}

void CEffTotalShaderPage::OnBnClickedButtonColor3()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int ThisR = int ( m_Property.m_cSpecular.r * 255.0f );
	int ThisG = int ( m_Property.m_cSpecular.g * 255.0f );
	int ThisB = int ( m_Property.m_cSpecular.b * 255.0f );

	CColorPickerDlg ColorPicker ( RGB(ThisR,ThisG,ThisB) );

	if ( IDOK == ColorPicker.DoModal() )
	{
		COLORREF SelectColor = ColorPicker.GetColor ();
		m_Property.m_cSpecular.r = GetRValue(SelectColor)/255.0f;
		m_Property.m_cSpecular.g = GetGValue(SelectColor)/255.0f;
		m_Property.m_cSpecular.b = GetBValue(SelectColor)/255.0f;

		OnBnClickedButtonApply();
	}
}

void CEffTotalShaderPage::OnNMCustomdrawSliderSpecpower(NMHDR *pNMHDR, LRESULT *pResult)
{
	if( !m_pEffChar ) return;
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	DWORD dwPos = GetWin_Num_int( this, IDC_EDIT_SPECPOWER );
	if( m_slider_SpecPW.GetPos() != dwPos )
	{
		SetWin_Num_float( this, IDC_EDIT_SPECPOWER, (float)m_slider_SpecPW.GetPos() );
		OnBnClickedButtonApply();
	}
	*pResult = 0;
}
