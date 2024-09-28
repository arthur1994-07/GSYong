// RenderStateEffEdit.cpp : 구현 파일입니다.
///

#include "stdafx.h"
#include "WorldEdit.h"
#include "MainFrm.h"
#include "WorldEditDoc.h"
#include "WorldEditView.h"

#include "SheetWithTab.h"

#include "../EngineLib/DxTools/DxMethods.h"
#include "../EngineLib/DxEffect/Frame/DxEffectRenderState.h"

#include "RenderStateEffEdit.h"
#include "RenderLight.h"

// CRenderStateEffEdit 대화 상자입니다.

IMPLEMENT_DYNAMIC(CRenderStateEffEdit, CPropertyPage)
CRenderStateEffEdit::CRenderStateEffEdit()
	: CPropertyPage(CRenderStateEffEdit::IDD)
{
}

CRenderStateEffEdit::~CRenderStateEffEdit()
{
}

void CRenderStateEffEdit::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_FILLMODE, m_ComboFillMode);
	DDX_Control(pDX, IDC_COMBO_CULLMODE, m_ComboCullMode);
	DDX_Control(pDX, IDC_COMBO_LIGHTING, m_ComboLighting);
	DDX_Control(pDX, IDC_COMBO_ZENABLE, m_ComboZnEable);
	DDX_Control(pDX, IDC_COMBO_ALPHABLEND, m_ComboAlphaBlend);
	DDX_Control(pDX, IDC_COMBO_SRCBLEND, m_ComboSrcBlend);
	DDX_Control(pDX, IDC_COMBO_DESTBLEND, m_ComboDestBlend);
	DDX_Control(pDX, IDC_COMBO_COLOROP, m_ComboColorOP);
	DDX_Control(pDX, IDC_COMBO_COLORARG1, m_ComboColorARG1);
	DDX_Control(pDX, IDC_COMBO_COLORARG2, m_ComboColorARG2);
	DDX_Control(pDX, IDC_COMBO_ALPHAOP, m_ComboAlphaOP);
	DDX_Control(pDX, IDC_COMBO_ALPHAARG1, m_ComboAlphaARG1);
	DDX_Control(pDX, IDC_COMBO_ALPHAARG2, m_ComboAlphaARG2);
	DDX_Control(pDX, IDC_COMBO_TYPE, m_ComboType);
}


BEGIN_MESSAGE_MAP(CRenderStateEffEdit, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnBnClickedButtonApply)
	ON_BN_CLICKED(IDC_BUTTON_OK, OnBnClickedButtonOk)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, OnBnClickedButtonCancel)
	ON_CBN_SELCHANGE(IDC_COMBO_TYPE, OnCbnSelchangeComboType)
	ON_BN_CLICKED(IDC_CHECK_LIGHT, OnBnClickedCheckLight)
	ON_BN_CLICKED(IDC_BUTTON_LIGHT, OnBnClickedButtonLight)
END_MESSAGE_MAP()


// CRenderStateEffEdit 메시지 처리기입니다.
void CRenderStateEffEdit::SetInstance ( DxEffectRenderState *pRenderStateEff )
{
	m_pRenderStateEff = pRenderStateEff;
	m_OldRenderStateProperty = m_RenderStateProperty = m_pRenderStateEff->GetProperty();

	//	Note : 값 설정.
	//
	CWnd *pWnd = GetDlgItem ( IDC_EDIT_FRAME_EFF_NAME );
	pWnd->SetWindowText ( m_pRenderStateEff->GetName() );

	int nIndex;
	CString Str;

	m_ComboType.ResetContent();
	
	Str.Format ( "사용자 정의" );
	nIndex = m_ComboType.AddString ( Str );
	m_ComboType.SetItemData ( nIndex, 0 );
	Str.Format ( "빛,.. " );
	nIndex = m_ComboType.AddString ( Str );
	m_ComboType.SetItemData ( nIndex, 1 );

	m_ComboType.SetCurSel ( m_RenderStateProperty.m_nType );


	m_ComboFillMode.ResetContent();

	Str.Format ( "FILL_POINT" );
	nIndex = m_ComboFillMode.AddString ( Str );
	m_ComboFillMode.SetItemData ( nIndex, 1 );
	Str.Format ( "FILL_WIREFRAME" );
	nIndex = m_ComboFillMode.AddString ( Str );
	m_ComboFillMode.SetItemData ( nIndex, 2 );
	Str.Format ( "FILL_SOLID" );
	nIndex = m_ComboFillMode.AddString ( Str );
	m_ComboFillMode.SetItemData ( nIndex, 3 );

	m_ComboFillMode.SetCurSel ( m_RenderStateProperty.m_FillMode - 1 );


	m_ComboCullMode.ResetContent();

	Str.Format ( "D3DCULL_NONE" );
	nIndex = m_ComboCullMode.AddString ( Str );
	m_ComboCullMode.SetItemData ( nIndex, 1 );
	Str.Format ( "D3DCULL_CW" );
	nIndex = m_ComboCullMode.AddString ( Str );
	m_ComboCullMode.SetItemData ( nIndex, 2 );
	Str.Format ( "D3DCULL_CCW" );
	nIndex = m_ComboCullMode.AddString ( Str );
	m_ComboCullMode.SetItemData ( nIndex, 3 );

	m_ComboCullMode.SetCurSel ( m_RenderStateProperty.m_CullMode - 1 );


	m_ComboLighting.ResetContent();

	Str.Format ( "FALSE" );
	nIndex = m_ComboLighting.AddString ( Str );
	m_ComboLighting.SetItemData ( nIndex, 0 );
	Str.Format ( "TRUE" );
	nIndex = m_ComboLighting.AddString ( Str );
	m_ComboLighting.SetItemData ( nIndex, 1 );

	m_ComboLighting.SetCurSel ( m_RenderStateProperty.m_Lighting );



	m_ComboZnEable.ResetContent();

	Str.Format ( "FALSE" );
	nIndex = m_ComboZnEable.AddString ( Str );
	m_ComboZnEable.SetItemData ( nIndex, 0 );
	Str.Format ( "TRUE" );
	nIndex = m_ComboZnEable.AddString ( Str );
	m_ComboZnEable.SetItemData ( nIndex, 1 );

	m_ComboZnEable.SetCurSel ( m_RenderStateProperty.m_ZWriteEnable );



	m_ComboAlphaBlend.ResetContent();

	Str.Format ( "FALSE" );
	nIndex = m_ComboAlphaBlend.AddString ( Str );
	m_ComboAlphaBlend.SetItemData ( nIndex, 0 );
	Str.Format ( "TRUE" );
	nIndex = m_ComboAlphaBlend.AddString ( Str );
	m_ComboAlphaBlend.SetItemData ( nIndex, 1 );

	m_ComboAlphaBlend.SetCurSel ( m_RenderStateProperty.m_AlphaBlendeEnable );



	m_ComboSrcBlend.ResetContent();
	m_ComboDestBlend.ResetContent();

	Str.Format ( "ZERO" );
	nIndex = m_ComboSrcBlend.AddString ( Str );
	nIndex = m_ComboDestBlend.AddString ( Str );
	m_ComboSrcBlend.SetItemData ( nIndex, 1 );
	m_ComboDestBlend.SetItemData ( nIndex, 1 );
	Str.Format ( "ONE" );
	nIndex = m_ComboSrcBlend.AddString ( Str );
	nIndex = m_ComboDestBlend.AddString ( Str );
	m_ComboSrcBlend.SetItemData ( nIndex, 2 );
	m_ComboDestBlend.SetItemData ( nIndex, 2 );
	Str.Format ( "SRCCOLOR" );
	nIndex = m_ComboSrcBlend.AddString ( Str );
	nIndex = m_ComboDestBlend.AddString ( Str );
	m_ComboSrcBlend.SetItemData ( nIndex, 3 );
	m_ComboDestBlend.SetItemData ( nIndex, 3 );
	Str.Format ( "INVSRCCOLOR" );
	nIndex = m_ComboSrcBlend.AddString ( Str );
	nIndex = m_ComboDestBlend.AddString ( Str );
	m_ComboSrcBlend.SetItemData ( nIndex, 4 );
	m_ComboDestBlend.SetItemData ( nIndex, 4 );
	Str.Format ( "SRCALPHA" );
	nIndex = m_ComboSrcBlend.AddString ( Str );
	nIndex = m_ComboDestBlend.AddString ( Str );
	m_ComboSrcBlend.SetItemData ( nIndex, 5 );
	m_ComboDestBlend.SetItemData ( nIndex, 5 );
	Str.Format ( "INVSRCALPHA" );
	nIndex = m_ComboSrcBlend.AddString ( Str );
	nIndex = m_ComboDestBlend.AddString ( Str );
	m_ComboSrcBlend.SetItemData ( nIndex, 6 );
	m_ComboDestBlend.SetItemData ( nIndex, 6 );
	Str.Format ( "DESTALPHA" );
	nIndex = m_ComboSrcBlend.AddString ( Str );
	nIndex = m_ComboDestBlend.AddString ( Str );
	m_ComboSrcBlend.SetItemData ( nIndex, 7 );
	m_ComboDestBlend.SetItemData ( nIndex, 7 );
	Str.Format ( "INVDESTALPHA" );
	nIndex = m_ComboSrcBlend.AddString ( Str );
	nIndex = m_ComboDestBlend.AddString ( Str );
	m_ComboSrcBlend.SetItemData ( nIndex, 8 );
	m_ComboDestBlend.SetItemData ( nIndex, 8 );
	Str.Format ( "DESTCOLOR" );
	nIndex = m_ComboSrcBlend.AddString ( Str );
	nIndex = m_ComboDestBlend.AddString ( Str );
	m_ComboSrcBlend.SetItemData ( nIndex, 9 );
	m_ComboDestBlend.SetItemData ( nIndex, 9 );
	Str.Format ( "INVDESTCOLOR" );
	nIndex = m_ComboSrcBlend.AddString ( Str );
	nIndex = m_ComboDestBlend.AddString ( Str );
	m_ComboSrcBlend.SetItemData ( nIndex, 10 );
	m_ComboDestBlend.SetItemData ( nIndex, 10 );
	Str.Format ( "SRCALPHASAT" );
	nIndex = m_ComboSrcBlend.AddString ( Str );
	nIndex = m_ComboDestBlend.AddString ( Str );
	m_ComboSrcBlend.SetItemData ( nIndex, 11 );
	m_ComboDestBlend.SetItemData ( nIndex, 11 );
	Str.Format ( "BOTHSRCALPHA" );
	nIndex = m_ComboSrcBlend.AddString ( Str );
	nIndex = m_ComboDestBlend.AddString ( Str );
	m_ComboSrcBlend.SetItemData ( nIndex, 12 );
	m_ComboDestBlend.SetItemData ( nIndex, 12 );
	Str.Format ( "BOTHINVSRCALPHA" );
	nIndex = m_ComboSrcBlend.AddString ( Str );
	nIndex = m_ComboDestBlend.AddString ( Str );
	m_ComboSrcBlend.SetItemData ( nIndex, 13 );
	m_ComboDestBlend.SetItemData ( nIndex, 12 );

	m_ComboSrcBlend.SetCurSel ( m_RenderStateProperty.m_SrcBlend - 1 );
	m_ComboDestBlend.SetCurSel ( m_RenderStateProperty.m_DestBlend - 1 );



	m_ComboColorOP.ResetContent();
	m_ComboAlphaOP.ResetContent();

	Str.Format ( "DISABLE" );
	nIndex = m_ComboColorOP.AddString ( Str );
	nIndex = m_ComboAlphaOP.AddString ( Str );
	m_ComboColorOP.SetItemData ( nIndex, 1 );
	m_ComboAlphaOP.SetItemData ( nIndex, 1 );
	Str.Format ( "SELECTARG1" );
	nIndex = m_ComboColorOP.AddString ( Str );
	nIndex = m_ComboAlphaOP.AddString ( Str );
	m_ComboColorOP.SetItemData ( nIndex, 2 );
	m_ComboAlphaOP.SetItemData ( nIndex, 2 );
	Str.Format ( "SELECTARG2" );
	nIndex = m_ComboColorOP.AddString ( Str );
	nIndex = m_ComboAlphaOP.AddString ( Str );
	m_ComboColorOP.SetItemData ( nIndex, 3 );
	m_ComboAlphaOP.SetItemData ( nIndex, 3 );
	Str.Format ( "MODULATE" );
	nIndex = m_ComboColorOP.AddString ( Str );
	nIndex = m_ComboAlphaOP.AddString ( Str );
	m_ComboColorOP.SetItemData ( nIndex, 4 );
	m_ComboAlphaOP.SetItemData ( nIndex, 4 );
	Str.Format ( "MODULATE2X" );
	nIndex = m_ComboColorOP.AddString ( Str );
	nIndex = m_ComboAlphaOP.AddString ( Str );
	m_ComboColorOP.SetItemData ( nIndex, 5 );
	m_ComboAlphaOP.SetItemData ( nIndex, 5 );
	Str.Format ( "MODULATE4X" );
	nIndex = m_ComboColorOP.AddString ( Str );
	nIndex = m_ComboAlphaOP.AddString ( Str );
	m_ComboColorOP.SetItemData ( nIndex, 6 );
	m_ComboAlphaOP.SetItemData ( nIndex, 6 );
	Str.Format ( "ADD" );
	nIndex = m_ComboColorOP.AddString ( Str );
	nIndex = m_ComboAlphaOP.AddString ( Str );
	m_ComboColorOP.SetItemData ( nIndex, 7 );
	m_ComboAlphaOP.SetItemData ( nIndex, 7 );
	Str.Format ( "ADDSIGNED" );
	nIndex = m_ComboColorOP.AddString ( Str );
	nIndex = m_ComboAlphaOP.AddString ( Str );
	m_ComboColorOP.SetItemData ( nIndex, 8 );
	m_ComboAlphaOP.SetItemData ( nIndex, 8 );
	Str.Format ( "ADDSIGNED2X" );
	nIndex = m_ComboColorOP.AddString ( Str );
	nIndex = m_ComboAlphaOP.AddString ( Str );
	m_ComboColorOP.SetItemData ( nIndex, 9 );
	m_ComboAlphaOP.SetItemData ( nIndex, 9 );
	Str.Format ( "SUBTRACT" );
	nIndex = m_ComboColorOP.AddString ( Str );
	nIndex = m_ComboAlphaOP.AddString ( Str );
	m_ComboColorOP.SetItemData ( nIndex, 10 );
	m_ComboAlphaOP.SetItemData ( nIndex, 10 );
	Str.Format ( "ADDSMOOTH" );
	nIndex = m_ComboColorOP.AddString ( Str );
	nIndex = m_ComboAlphaOP.AddString ( Str );
	m_ComboColorOP.SetItemData ( nIndex, 11 );
	m_ComboAlphaOP.SetItemData ( nIndex, 11 );

	m_ComboColorOP.SetCurSel ( m_RenderStateProperty.m_ColorOP_0 - 1 );
	m_ComboAlphaOP.SetCurSel ( m_RenderStateProperty.m_AlphaOP_0 - 1 );



	m_ComboColorARG1.ResetContent();
	m_ComboColorARG2.ResetContent();
	m_ComboAlphaARG1.ResetContent();
	m_ComboAlphaARG2.ResetContent();

	Str.Format ( "DIFFUSE" );
	nIndex = m_ComboColorARG1.AddString ( Str );
	nIndex = m_ComboColorARG2.AddString ( Str );
	nIndex = m_ComboAlphaARG1.AddString ( Str );
	nIndex = m_ComboAlphaARG2.AddString ( Str );
	m_ComboColorARG1.SetItemData ( nIndex, 0 );
	m_ComboColorARG2.SetItemData ( nIndex, 0 );
	m_ComboAlphaARG1.SetItemData ( nIndex, 0 );
	m_ComboAlphaARG2.SetItemData ( nIndex, 0 );
	Str.Format ( "CURRENT" );
	nIndex = m_ComboColorARG1.AddString ( Str );
	nIndex = m_ComboColorARG2.AddString ( Str );
	nIndex = m_ComboAlphaARG1.AddString ( Str );
	nIndex = m_ComboAlphaARG2.AddString ( Str );
	m_ComboColorARG1.SetItemData ( nIndex, 1 );
	m_ComboColorARG2.SetItemData ( nIndex, 1 );
	m_ComboAlphaARG1.SetItemData ( nIndex, 1 );
	m_ComboAlphaARG2.SetItemData ( nIndex, 1 );
	Str.Format ( "TEXTURE" );
	nIndex = m_ComboColorARG1.AddString ( Str );
	nIndex = m_ComboColorARG2.AddString ( Str );
	nIndex = m_ComboAlphaARG1.AddString ( Str );
	nIndex = m_ComboAlphaARG2.AddString ( Str );
	m_ComboColorARG1.SetItemData ( nIndex, 2 );
	m_ComboColorARG2.SetItemData ( nIndex, 2 );
	m_ComboAlphaARG1.SetItemData ( nIndex, 2 );
	m_ComboAlphaARG2.SetItemData ( nIndex, 2 );
	Str.Format ( "TFACTOR" );
	nIndex = m_ComboColorARG1.AddString ( Str );
	nIndex = m_ComboColorARG2.AddString ( Str );
	nIndex = m_ComboAlphaARG1.AddString ( Str );
	nIndex = m_ComboAlphaARG2.AddString ( Str );
	m_ComboColorARG1.SetItemData ( nIndex, 3 );
	m_ComboColorARG2.SetItemData ( nIndex, 3 );
	m_ComboAlphaARG1.SetItemData ( nIndex, 3 );
	m_ComboAlphaARG2.SetItemData ( nIndex, 3 );
	Str.Format ( "SPECULAR" );
	nIndex = m_ComboColorARG1.AddString ( Str );
	nIndex = m_ComboColorARG2.AddString ( Str );
	nIndex = m_ComboAlphaARG1.AddString ( Str );
	nIndex = m_ComboAlphaARG2.AddString ( Str );
	m_ComboColorARG1.SetItemData ( nIndex, 4 );
	m_ComboColorARG2.SetItemData ( nIndex, 4 );
	m_ComboAlphaARG1.SetItemData ( nIndex, 4 );
	m_ComboAlphaARG2.SetItemData ( nIndex, 4 );
	Str.Format ( "TEMP" );
	nIndex = m_ComboColorARG1.AddString ( Str );
	nIndex = m_ComboColorARG2.AddString ( Str );
	nIndex = m_ComboAlphaARG1.AddString ( Str );
	nIndex = m_ComboAlphaARG2.AddString ( Str );
	m_ComboColorARG1.SetItemData ( nIndex, 5 );
	m_ComboColorARG2.SetItemData ( nIndex, 5 );
	m_ComboAlphaARG1.SetItemData ( nIndex, 5 );
	m_ComboAlphaARG2.SetItemData ( nIndex, 5 );

	m_ComboColorARG1.SetCurSel ( m_RenderStateProperty.m_ColorARG1_0 );
	m_ComboColorARG2.SetCurSel ( m_RenderStateProperty.m_ColorARG2_0 );
	m_ComboAlphaARG1.SetCurSel ( m_RenderStateProperty.m_AlphaARG1_0 );
	m_ComboAlphaARG2.SetCurSel ( m_RenderStateProperty.m_AlphaARG2_0 );

	pWnd	= GetDlgItem ( IDC_BUTTON_LIGHT );
	CButton*	pButton;
	pButton	= (CButton*)GetDlgItem ( IDC_CHECK_LIGHT );
	if ( m_RenderStateProperty.m_bLight )
	{
		pButton->SetCheck ( TRUE );
		pWnd->EnableWindow ( TRUE );
	}
	else
	{
		pButton->SetCheck ( FALSE );
		pWnd->EnableWindow ( FALSE );
	}

}
void CRenderStateEffEdit::OnBnClickedButtonApply()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	int nIndex, nData;

	nIndex = m_ComboType.GetCurSel ();
	nData = m_ComboType.GetItemData ( nIndex );
	m_RenderStateProperty.m_nType = nData;

	nIndex = m_ComboFillMode.GetCurSel ();
	nData = m_ComboFillMode.GetItemData ( nIndex );
	m_RenderStateProperty.m_FillMode = nData;

	nIndex = m_ComboCullMode.GetCurSel ();
	nData = m_ComboCullMode.GetItemData ( nIndex );
	m_RenderStateProperty.m_CullMode = nData;

	nIndex = m_ComboLighting.GetCurSel ();
	nData = m_ComboLighting.GetItemData ( nIndex );
	m_RenderStateProperty.m_Lighting = nData;

	nIndex = m_ComboZnEable.GetCurSel ();
	nData = m_ComboZnEable.GetItemData ( nIndex );
	m_RenderStateProperty.m_ZWriteEnable = nData;

	nIndex = m_ComboAlphaBlend.GetCurSel ();
	nData = m_ComboAlphaBlend.GetItemData ( nIndex );
	m_RenderStateProperty.m_AlphaBlendeEnable = nData;

	nIndex = m_ComboSrcBlend.GetCurSel ();
	nData = m_ComboSrcBlend.GetItemData ( nIndex );
	m_RenderStateProperty.m_SrcBlend = nData;

	nIndex = m_ComboDestBlend.GetCurSel ();
	nData = m_ComboDestBlend.GetItemData ( nIndex );
	m_RenderStateProperty.m_DestBlend = nData;

	nIndex = m_ComboColorOP.GetCurSel ();
	nData = m_ComboColorOP.GetItemData ( nIndex );
	m_RenderStateProperty.m_ColorOP_0 = nData;

	nIndex = m_ComboAlphaOP.GetCurSel ();
	nData = m_ComboAlphaOP.GetItemData ( nIndex );
	m_RenderStateProperty.m_AlphaOP_0 = nData;

	nIndex = m_ComboColorARG1.GetCurSel ();
	nData = m_ComboColorARG1.GetItemData ( nIndex );
	m_RenderStateProperty.m_ColorARG1_0 = nData;

	nIndex = m_ComboColorARG2.GetCurSel ();
	nData = m_ComboColorARG2.GetItemData ( nIndex );
	m_RenderStateProperty.m_ColorARG2_0 = nData;

	nIndex = m_ComboAlphaARG1.GetCurSel ();
	nData = m_ComboAlphaARG1.GetItemData ( nIndex );
	m_RenderStateProperty.m_AlphaARG1_0 = nData;

	nIndex = m_ComboAlphaARG2.GetCurSel ();
	nData = m_ComboAlphaARG2.GetItemData ( nIndex );
	m_RenderStateProperty.m_AlphaARG2_0 = nData;

	//	Note : 속성을 적용.
	//
	m_pRenderStateEff->SetProperty ( m_RenderStateProperty );
	m_pRenderStateEff->CleanUp ();

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	m_pRenderStateEff->Create ( pView->GetD3DDevice() );
}

void CRenderStateEffEdit::OnBnClickedButtonOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pSheetTab->SetActivePage ( 0 );

	//	Note : 속성을 적용.
	//
	OnBnClickedButtonApply();
}

void CRenderStateEffEdit::OnBnClickedButtonCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pSheetTab->SetActivePage ( 0 );

	//	Note : 속성을 되돌림.
	//
	m_pRenderStateEff->SetProperty ( m_OldRenderStateProperty );
	m_pRenderStateEff->CleanUp ();

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	m_pRenderStateEff->Create ( pView->GetD3DDevice() );
}

void CRenderStateEffEdit::OnCbnSelchangeComboType()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nIndex = m_ComboType.GetCurSel ();
	int nData = m_ComboType.GetItemData ( nIndex );
	if ( nData == 0 )
	{
	}
	else if ( nData == 1 )
	{
		m_ComboFillMode.SetCurSel(2);		// 이상
		m_ComboCullMode.SetCurSel(0);		// 이상
		m_ComboLighting.SetCurSel(0);
		m_ComboZnEable.SetCurSel(0);
		m_ComboAlphaBlend.SetCurSel(1);
		m_ComboSrcBlend.SetCurSel(4);		// 이상
		m_ComboDestBlend.SetCurSel(1);		// 이상
		m_ComboColorOP.SetCurSel(1);		// 이상
		m_ComboColorARG1.SetCurSel(2);
		m_ComboColorARG2.SetCurSel(0);
		m_ComboAlphaOP.SetCurSel(1);		// 이상
		m_ComboAlphaARG1.SetCurSel(2);
		m_ComboAlphaARG2.SetCurSel(0);
	}
}

void CRenderStateEffEdit::OnBnClickedCheckLight()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CWnd*		pWnd;
	CButton*	pButton;
	pWnd	= GetDlgItem ( IDC_BUTTON_LIGHT );
	pButton	= (CButton*)GetDlgItem ( IDC_CHECK_LIGHT );
	if ( pButton->GetCheck() )
	{
		m_RenderStateProperty.m_bLight = TRUE;
		pWnd->EnableWindow ( TRUE );
	}
	else
	{
		m_RenderStateProperty.m_bLight = FALSE;
		pWnd->EnableWindow ( FALSE );
	}

}

void CRenderStateEffEdit::OnBnClickedButtonLight()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CRenderLight			pDlg;
	pDlg.m_bNearLight		= m_RenderStateProperty.m_bNearLight;
	pDlg.m_fColorPower		= m_RenderStateProperty.m_fColorPower;
	pDlg.m_nRate			= m_RenderStateProperty.m_nRate;
	pDlg.m_fSpeed			= m_RenderStateProperty.m_fSpeed;

	if ( IDOK == pDlg.DoModal() )
	{
		m_RenderStateProperty.m_bNearLight	= pDlg.m_bNearLight;
		m_RenderStateProperty.m_fColorPower	= pDlg.m_fColorPower;
		m_RenderStateProperty.m_nRate		= pDlg.m_nRate;
		m_RenderStateProperty.m_fSpeed		= pDlg.m_fSpeed;	
	}
}
