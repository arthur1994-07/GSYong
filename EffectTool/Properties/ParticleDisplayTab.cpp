// ParticleDisplayTab.cpp : implementation file
//

#include "stdafx.h"
#include "../EffectTool.h"
#include "../MainFrm.h"

#include "../EffectToolDoc.h"
#include "../EffectToolView.h"

#include "../Dialog/ColorPickerDlg.h"
#include "../Dialog/SetVar.h"
#include "../Dialog/SetMultiSlider.h"

#include "../../EngineLib/DxTools/TextureManager.h"

#include "ParticleDisplayTab.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// CParticleDisplayTab dialog


CParticleDisplayTab::CParticleDisplayTab(CWnd* pParent /*=NULL*/)
	: CDialog(CParticleDisplayTab::IDD, pParent),
	m_pProperty(NULL)
{
	//{{AFX_DATA_INIT(CParticleDisplayTab)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CParticleDisplayTab::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CParticleDisplayTab)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_COMBO_POWER2, m_comboPOWER);
	DDX_Control(pDX, IDC_COMBO_BLEND2, m_comboBLEND);
}


BEGIN_MESSAGE_MAP(CParticleDisplayTab, CDialog)
	//{{AFX_MSG_MAP(CParticleDisplayTab)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_TEX_SCALE, OnBnClickedButtonTexScale)
	ON_BN_CLICKED(IDC_BUTTON_TEXROTATE, OnBnClickedButtonTexrotate)
	ON_BN_CLICKED(IDC_BUTTON_SEQUENCE, OnBnClickedButtonSequence)
	ON_BN_CLICKED(IDC_BUTTON_SCALE, OnBnClickedButtonScale)
	ON_BN_CLICKED(IDC_BUTTON_BILLALL, OnBnClickedButtonBillall)
	ON_BN_CLICKED(IDC_BUTTON_BILLUP, OnBnClickedButtonBillup)
	ON_BN_CLICKED(IDC_BUTTON_GROUND, OnBnClickedButtonGround)
	ON_BN_CLICKED(IDC_BUTTON_ALPHA, OnBnClickedButtonAlpha)
	ON_BN_CLICKED(IDC_BUTTON_COLOR_S, OnBnClickedButtonColorS)
	ON_BN_CLICKED(IDC_BUTTON_COLOR_V, OnBnClickedButtonColorV)
	ON_BN_CLICKED(IDC_BUTTONCOLOR_E, OnBnClickedButtoncolorE)
	ON_BN_CLICKED(IDC_CHECK_MESH, OnBnClickedCheckMesh)
	ON_BN_CLICKED(IDC_CHECK_TEX, OnBnClickedCheckTex)
	ON_BN_CLICKED(IDC_BUTTON_MESH, OnBnClickedButtonMesh)
	ON_BN_CLICKED(IDC_BUTTON_TEXTURE, OnBnClickedButtonTexture)
	ON_BN_CLICKED(IDC_BUTTON_TEX_SCALE_DISABLE, OnBnClickedButtonTexScaleDisable)
	ON_BN_CLICKED(IDC_BUTTON_TEXROTATE_DISABLE, OnBnClickedButtonTexrotateDisable)
	ON_BN_CLICKED(IDC_BUTTON_SEQUENCE_DISABLE, OnBnClickedButtonSequenceDisable)
	ON_CBN_SELCHANGE(IDC_COMBO_POWER2, OnCbnSelchangeComboPower2)
	ON_CBN_SELCHANGE(IDC_COMBO_BLEND2, OnCbnSelchangeComboBlend2)
	ON_BN_CLICKED(IDC_BUTTON_NORMAL, OnBnClickedButtonNormal)
	ON_BN_CLICKED(IDC_BUTTON_GROUND_TEX, OnBnClickedButtonGroundTex)
	ON_BN_CLICKED(IDC_CHECK_NEWEFF, OnBnClickedCheckNeweff)
	ON_BN_CLICKED(IDC_BUTTON_NEWEFF, OnBnClickedButtonNeweff)
	ON_BN_CLICKED(IDC_RADIO_NEWEFF_FIRST, OnBnClickedRadioNeweffFirst)
	ON_BN_CLICKED(IDC_RADIO_NEWEFF_END, OnBnClickedRadioNeweffEnd)
	ON_BN_CLICKED(IDC_RADIO_NEWEFF_END_STOP, OnBnClickedRadioNeweffEndStop)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CParticleDisplayTab message handlers

void CParticleDisplayTab::Init ( PARTICLESYS_PROPERTY* pProp )
{
	m_pProperty = pProp;

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
		char szValue[256];

		pWnd = GetDlgItem ( IDC_EDIT_PARTICLE_S );
		_gcvt ( m_pProperty->m_uParticlesPerSec, 7, szValue );
		pWnd->SetWindowText ( szValue );

		pWnd = GetDlgItem ( IDC_EDIT_ANGEL );
		_gcvt ( m_pProperty->m_fTheta, 7, szValue );
		pWnd->SetWindowText ( szValue );
	}

	{
		//	Note : 메쉬 생성 아님 그냥 파티클
		if( m_pProperty->m_dwFlag & USEMESH )
		{
			pWnd = GetDlgItem ( IDC_BUTTON_TEXROTATE );
			pWnd->EnableWindow ( FALSE );
			pWnd = GetDlgItem ( IDC_BUTTON_TEX_SCALE );
			pWnd->EnableWindow ( FALSE );
			
			pWnd = GetDlgItem ( IDC_STATIC2 );
			pWnd->SetWindowText ("파티클 메쉬(Mesh)");
		}
		else
		{
			pWnd = GetDlgItem ( IDC_BUTTON_TEXROTATE );
			pWnd->EnableWindow ( TRUE );
			pWnd = GetDlgItem ( IDC_BUTTON_TEX_SCALE );
			pWnd->EnableWindow ( TRUE );

			pWnd = GetDlgItem ( IDC_STATIC2 );
			pWnd->SetWindowText ("파티클 (Paticle)");
		}

		pWnd = GetDlgItem ( IDC_EDIT_MESH );
		pWnd->SetWindowText ( m_pProperty->m_szMeshFile );

		pWnd = GetDlgItem ( IDC_EDIT_TEXTURE );
		pWnd->SetWindowText ( m_pProperty->m_szTexture );

		pWnd = GetDlgItem( IDC_EDIT_NEWEFF );
		pWnd->SetWindowText( m_pProperty->m_szEffFile );

		pWnd = GetDlgItem ( IDC_BUTTON_MESH );
		pButton = (CButton*)GetDlgItem ( IDC_CHECK_MESH );
		if ( m_pProperty->m_dwFlag & USEMESH )
		{
			pWnd->EnableWindow ( TRUE );
			pButton->SetCheck( TRUE );
		}
		else
		{
			pWnd->EnableWindow ( FALSE );
			pButton->SetCheck( FALSE );
		}

		pWnd = GetDlgItem ( IDC_BUTTON_TEXTURE );
		pButton = (CButton*)GetDlgItem ( IDC_CHECK_TEX );
		if ( m_pProperty->m_dwFlag & USETEXTURE )	
		{
			pWnd->EnableWindow ( TRUE );
			pButton->SetCheck( TRUE );
		}
		else
		{
			pWnd->EnableWindow ( FALSE );
			pButton->SetCheck( FALSE );
		}

		pWnd = GetDlgItem ( IDC_BUTTON_NEWEFF );
		pButton = (CButton*)GetDlgItem ( IDC_CHECK_NEWEFF );
		if ( m_pProperty->m_dwFlag & USENEWEFF )	
		{
			pWnd->EnableWindow ( TRUE );
			pButton->SetCheck( TRUE );
		}
		else
		{
			pWnd->EnableWindow ( FALSE );
			pButton->SetCheck( FALSE );
		}

		if( m_pProperty->m_dwFlag & USENEWEFF_END )
		{
			pButton = (CButton*)GetDlgItem ( IDC_RADIO_NEWEFF_FIRST );
			pButton->SetCheck( FALSE );

			pButton = (CButton*)GetDlgItem ( IDC_RADIO_NEWEFF_END );
			pButton->SetCheck( TRUE );

			pButton = (CButton*)GetDlgItem ( IDC_RADIO_NEWEFF_END_STOP );
			pButton->SetCheck( FALSE );
		}
		else if( m_pProperty->m_dwFlag & USENEWEFF_END_STOP )
		{
			pButton = (CButton*)GetDlgItem ( IDC_RADIO_NEWEFF_FIRST );
			pButton->SetCheck( FALSE );

			pButton = (CButton*)GetDlgItem ( IDC_RADIO_NEWEFF_END );
			pButton->SetCheck( FALSE );

			pButton = (CButton*)GetDlgItem ( IDC_RADIO_NEWEFF_END_STOP );
			pButton->SetCheck( TRUE );
		}
		else
		{
			pButton = (CButton*)GetDlgItem ( IDC_RADIO_NEWEFF_FIRST );
			pButton->SetCheck( TRUE );

			pButton = (CButton*)GetDlgItem ( IDC_RADIO_NEWEFF_END );
			pButton->SetCheck( FALSE );

			pButton = (CButton*)GetDlgItem ( IDC_RADIO_NEWEFF_END_STOP );
			pButton->SetCheck( FALSE );
		}
	}

	{
		m_Color.SubclassDlgItem ( IDC_STATIC_COLOR, this );
		m_Color.m_d3dStartColor = m_pProperty->m_cColorStart;
		m_Color.m_d3dEndColor = m_pProperty->m_cColorEnd;
		m_Color.RedrawWindow();
	}

	{
		m_cClrTexScale.SubclassDlgItem ( IDC_BUTTON_TEX_SCALE, this );
		m_cClrTexRotate.SubclassDlgItem ( IDC_BUTTON_TEXROTATE, this );
		m_cClrSequence.SubclassDlgItem ( IDC_BUTTON_SEQUENCE, this );
		m_cClrBillALL.SubclassDlgItem ( IDC_BUTTON_BILLALL, this );
		m_cClrBillUP.SubclassDlgItem ( IDC_BUTTON_BILLUP, this );
		m_cClrGround.SubclassDlgItem ( IDC_BUTTON_GROUND, this );
		m_cClrNormal.SubclassDlgItem ( IDC_BUTTON_NORMAL, this );
		m_cClrGroundTex.SubclassDlgItem ( IDC_BUTTON_GROUND_TEX, this );

		COLORREF cColorBLUE, cColorGRAY, cColorTEXT;
		cColorGRAY = RGB(212, 208, 200);
		cColorBLUE = RGB(192, 192, 255);
		cColorTEXT = RGB(0, 0, 0);

		if ( pProp->m_dwFlag & USETEXSCALE )		m_cClrTexScale.SetColour ( cColorTEXT, cColorBLUE );
		else										m_cClrTexScale.SetColour ( cColorTEXT, cColorGRAY );

		if ( pProp->m_dwFlag & USETEXROTATE )		m_cClrTexRotate.SetColour ( cColorTEXT, cColorBLUE );
		else										m_cClrTexRotate.SetColour ( cColorTEXT, cColorGRAY );

		if ( pProp->m_dwFlag & USESEQUENCE )		m_cClrSequence.SetColour ( cColorTEXT, cColorBLUE );
		else										m_cClrSequence.SetColour ( cColorTEXT, cColorGRAY );

		if ( pProp->m_dwFlag & USEBILLBOARDALL )	m_cClrBillALL.SetColour ( cColorTEXT, cColorBLUE );
		else										m_cClrBillALL.SetColour ( cColorTEXT, cColorGRAY );

		if ( pProp->m_dwFlag & USEBILLBOARDUP )		m_cClrBillUP.SetColour ( cColorTEXT, cColorBLUE );
		else										m_cClrBillUP.SetColour ( cColorTEXT, cColorGRAY );

		if ( pProp->m_dwFlag & USEGROUND )			m_cClrGround.SetColour ( cColorTEXT, cColorBLUE );
		else										m_cClrGround.SetColour ( cColorTEXT, cColorGRAY );

		if ( m_pProperty->m_dwFlag & USENORMAL2 )	m_cClrNormal.SetColour ( cColorTEXT, cColorBLUE );
		else										m_cClrNormal.SetColour ( cColorTEXT, cColorGRAY );

		if ( pProp->m_dwFlag & USEGROUNDTEX )		m_cClrGroundTex.SetColour ( cColorTEXT, cColorBLUE );
		else										m_cClrGroundTex.SetColour ( cColorTEXT, cColorGRAY );
	}
}

void CParticleDisplayTab::Apply ()
{
	CString strText;

	CWnd *pWnd;

	pWnd = GetDlgItem ( IDC_EDIT_PARTICLE_S );
	pWnd->GetWindowText ( strText );
	m_pProperty->m_uParticlesPerSec = (DWORD) atoi ( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_ANGEL );
	pWnd->GetWindowText ( strText );
	m_pProperty->m_fTheta = (float)atof ( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_MESH );
	pWnd->GetWindowText ( strText );
	strcpy ( m_pProperty->m_szMeshFile, strText.GetString() );	// 메쉬

	pWnd = GetDlgItem ( IDC_EDIT_TEXTURE );
	pWnd->GetWindowText ( strText );
	strcpy ( m_pProperty->m_szTexture, strText.GetString() );	// 텍스쳐

	pWnd = GetDlgItem ( IDC_EDIT_NEWEFF );
	pWnd->GetWindowText( strText );
	strcpy ( m_pProperty->m_szEffFile, strText.GetString() );	// 텍스쳐
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//

void CParticleDisplayTab::OnCbnSelchangeComboPower2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nIndex, nData;
	nIndex = m_comboPOWER.GetCurSel();
	nData = m_comboPOWER.GetItemData ( nIndex );

	m_pProperty->m_nPower = nData;
}

void CParticleDisplayTab::OnCbnSelchangeComboBlend2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nIndex, nData;
	nIndex = m_comboBLEND.GetCurSel();
	nData = m_comboBLEND.GetItemData ( nIndex );

	m_pProperty->m_nBlend = nData;
}

void CParticleDisplayTab::OnBnClickedButtonTexScale()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	CEffectToolView *pView = (CEffectToolView*) pFrame->GetActiveView ();
	//pView->SetCurClick( FALSE );

	CSetVar DlgSetVar;
	DlgSetVar.m_StrTitle = "Set Texture Scale";
	DlgSetVar.m_StrRadio1	= "위";
	DlgSetVar.m_StrRadio2	= "가운데";
	DlgSetVar.m_StrComm1	= "처음 X";
	DlgSetVar.m_StrComm2	= "처음 Y";
	DlgSetVar.m_StrComm3	= "나중 X";
	DlgSetVar.m_StrComm4	= "나중 Y";
	DlgSetVar.m_dwFlag = USERADIO | USEDOWN | USESTARTSCROLL | USESTARTVARSCROLL | USEENDSCROLL | USEEND4SCROLL;
	DlgSetVar.m_bZeroBase = TRUE;
	DlgSetVar.m_ToScale = 20.0f;
	
	DlgSetVar.m_bUse = ( m_pProperty->m_dwFlag & USETEXSCALE ) ? TRUE : FALSE;

	DlgSetVar.m_iRadio = m_pProperty->m_iCenterPoint;

	DlgSetVar.m_fStart = m_pProperty->m_vTexScale.x;
	DlgSetVar.m_fStartVar = m_pProperty->m_vTexScale.y;
	DlgSetVar.m_fEnd = m_pProperty->m_vTexScale.z;
	DlgSetVar.m_fEnd4 = m_pProperty->m_vTexScale.w;
	
	if ( IDOK == DlgSetVar.DoModal() )
	{
		m_pProperty->m_iCenterPoint = DlgSetVar.m_iRadio;

		m_pProperty->m_vTexScale.x = DlgSetVar.m_fStart;
		m_pProperty->m_vTexScale.y = DlgSetVar.m_fStartVar;
		m_pProperty->m_vTexScale.z = DlgSetVar.m_fEnd;
		m_pProperty->m_vTexScale.w = DlgSetVar.m_fEnd4;


		m_pProperty->m_dwFlag |= USETEXSCALE;
		m_pProperty->m_dwFlag &= ~USETEXROTATE;
		m_pProperty->m_dwFlag &= ~USERANGEHIGH;

		ReSetButton ();

		//CWnd *pWnd;
		//pWnd = GetDlgItem ( IDC_BUTTON_TEX_SCALE );
		//if ( m_pProperty->m_dwFlag & USETEXSCALE )	pWnd->SetWindowText ( "텍스쳐 늘이기 O" );
		//else										pWnd->SetWindowText ( "텍스쳐 늘이기 X" );

		//if ( m_pProperty->m_dwFlag & USETEXSCALE )
		//{
		//	pWnd->SetWindowText ( "텍스쳐 늘이기 O" );

		//	m_pProperty->m_dwFlag &= ~USERANGEHIGH;		// 회전 사용 못하도록
		//	pWnd = GetDlgItem ( IDC_BUTTON_RANGE_HIGH );
		//	pWnd->SetWindowText ( "범위 고급 X" );
		//}
		//else	pWnd->SetWindowText ( "텍스쳐 늘이기 X" );

		//if ( DlgSetVar.m_bUse )
		//{
		//	m_pProperty->m_dwFlag &= ~USETEXROTATE;

		//	pWnd = GetDlgItem ( IDC_BUTTON_TEXROTATE );
		//	pWnd->SetWindowText ( "텍스쳐 회전 X" );
		//}
	}
}

void CParticleDisplayTab::ReSetButton ()
{
	COLORREF cColorBLUE = RGB(192, 192, 255);
	COLORREF cColorRED = RGB(255, 192, 192);
	COLORREF cColorTEXT = RGB(0, 0, 0);

	m_cClrTexScale.SetColour ( cColorTEXT, cColorRED );
	m_cClrTexRotate.SetColour ( cColorTEXT, cColorRED );

	if ( m_pProperty->m_dwFlag&USETEXSCALE )	m_cClrTexScale.SetColour ( cColorTEXT, cColorBLUE );
	if ( m_pProperty->m_dwFlag&USETEXROTATE )	m_cClrTexRotate.SetColour ( cColorTEXT, cColorBLUE );
}

void CParticleDisplayTab::OnBnClickedButtonTexrotate()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	CEffectToolView *pView = (CEffectToolView*) pFrame->GetActiveView ();
	//pView->SetCurClick( FALSE );

	CSetVar DlgSetVar;
	DlgSetVar.m_StrTitle	= "텍스쳐회전";
	DlgSetVar.m_StrComm1	= "회전속도";
	DlgSetVar.m_dwFlag = USESTARTSCROLL;
	DlgSetVar.m_ToScale = 4.0f;

	DlgSetVar.m_fStart = m_pProperty->m_fTexRotateAngel;
	
	if ( IDOK == DlgSetVar.DoModal() )
	{

		m_pProperty->m_fTexRotateAngel = DlgSetVar.m_fStart;


		m_pProperty->m_dwFlag |= USETEXROTATE;
		m_pProperty->m_dwFlag &= ~USETEXSCALE;
		m_pProperty->m_dwFlag &= ~USERANGEHIGH;

		ReSetButton ();

		//CWnd *pWnd;
		//pWnd = GetDlgItem ( IDC_BUTTON_TEXROTATE );

		//if ( m_pProperty->m_dwFlag & USETEXROTATE )
		//{
		//	pWnd->SetWindowText ( "텍스쳐 회전 O" );

		//	m_pProperty->m_dwFlag &= ~USERANGEHIGH;		// 회전 사용 못하도록
		//	pWnd = GetDlgItem ( IDC_BUTTON_RANGE_HIGH );
		//	pWnd->SetWindowText ( "범위 고급 X" );
		//}
		//else	pWnd->SetWindowText ( "텍스쳐 회전 X" );

		//if ( DlgSetVar.m_bUse )
		//{
		//	m_pProperty->m_dwFlag &= ~USETEXSCALE;
		//	
		//	pWnd = GetDlgItem ( IDC_BUTTON_TEX_SCALE );
		//	pWnd->SetWindowText ( "텍스쳐 늘이기 X" );
		//}
	}
}

void CParticleDisplayTab::OnBnClickedButtonSequence()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	CEffectToolView *pView = (CEffectToolView*) pFrame->GetActiveView ();
	//pView->SetCurClick( FALSE );

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
}

void CParticleDisplayTab::OnBnClickedButtonTexScaleDisable()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pProperty->m_dwFlag &= ~USETEXSCALE;

	COLORREF cColorTEXT	= RGB(0, 0, 0);
	COLORREF cColorGRAY = RGB(212, 208, 200);
	m_cClrTexScale.SetColour ( cColorTEXT, cColorGRAY );
}

void CParticleDisplayTab::OnBnClickedButtonTexrotateDisable()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pProperty->m_dwFlag &= ~USETEXROTATE;

	COLORREF cColorTEXT	= RGB(0, 0, 0);
	COLORREF cColorGRAY = RGB(212, 208, 200);
	m_cClrTexRotate.SetColour ( cColorTEXT, cColorGRAY );
}

void CParticleDisplayTab::OnBnClickedButtonSequenceDisable()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pProperty->m_dwFlag &= ~USESEQUENCE;

	COLORREF cColorTEXT	= RGB(0, 0, 0);
	COLORREF cColorGRAY = RGB(212, 208, 200);
	m_cClrSequence.SetColour ( cColorTEXT, cColorGRAY );
}

void CParticleDisplayTab::OnBnClickedButtonScale()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	CEffectToolView *pView = (CEffectToolView*) pFrame->GetActiveView ();
	//pView->SetCurClick( FALSE );

	CSetVar DlgSetVar;
	DlgSetVar.m_StrTitle = "Set Particle Size";
	DlgSetVar.m_dwFlag = USESTARTSCROLL | USESTARTVARSCROLL | USEENDSCROLL;
	DlgSetVar.m_bZeroBase = TRUE;
	DlgSetVar.m_ToScale = 40.0f;

	DlgSetVar.m_fStart = m_pProperty->m_fSizeStart;
	DlgSetVar.m_fStartVar = m_pProperty->m_fSizeVar;
	DlgSetVar.m_fEnd = m_pProperty->m_fSizeEnd;
	
	if ( IDOK == DlgSetVar.DoModal() )
	{
		m_pProperty->m_fSizeStart = DlgSetVar.m_fStart;
		m_pProperty->m_fSizeVar = DlgSetVar.m_fStartVar;
		m_pProperty->m_fSizeEnd = DlgSetVar.m_fEnd;
	}
}

void CParticleDisplayTab::OnBnClickedButtonBillall()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	COLORREF cColorBLUE, cColorGRAY, cColorTEXT;
	cColorGRAY = RGB(212, 208, 200);
	cColorBLUE = RGB(192, 192, 255);
	cColorTEXT = RGB(0, 0, 0);

	m_pProperty->m_dwFlag ^= USEBILLBOARDALL;

	if ( m_pProperty->m_dwFlag & USEBILLBOARDALL )	m_cClrBillALL.SetColour ( cColorTEXT, cColorBLUE );
	else											m_cClrBillALL.SetColour ( cColorTEXT, cColorGRAY );
}

void CParticleDisplayTab::OnBnClickedButtonBillup()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	COLORREF cColorBLUE, cColorGRAY, cColorTEXT;
	cColorGRAY = RGB(212, 208, 200);
	cColorBLUE = RGB(192, 192, 255);
	cColorTEXT = RGB(0, 0, 0);

	m_pProperty->m_dwFlag ^= USEBILLBOARDUP;

	if ( m_pProperty->m_dwFlag & USEBILLBOARDUP )	m_cClrBillUP.SetColour ( cColorTEXT, cColorBLUE );
	else											m_cClrBillUP.SetColour ( cColorTEXT, cColorGRAY );
}

void CParticleDisplayTab::OnBnClickedButtonGround()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	COLORREF cColorBLUE, cColorGRAY, cColorTEXT;
	cColorGRAY = RGB(212, 208, 200);
	cColorBLUE = RGB(192, 192, 255);
	cColorTEXT = RGB(0, 0, 0);

	m_pProperty->m_dwFlag ^= USEGROUND;

	if ( m_pProperty->m_dwFlag & USEGROUND )		m_cClrGround.SetColour ( cColorTEXT, cColorBLUE );
	else											m_cClrGround.SetColour ( cColorTEXT, cColorGRAY );
}

void CParticleDisplayTab::OnBnClickedButtonAlpha()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	CEffectToolView *pView = (CEffectToolView*) pFrame->GetActiveView ();
	//pView->SetCurClick( FALSE );

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
}

void CParticleDisplayTab::OnBnClickedButtonColorS()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	CEffectToolView *pView = (CEffectToolView*) pFrame->GetActiveView ();
	//pView->SetCurClick( FALSE );

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
}

void CParticleDisplayTab::OnBnClickedButtonColorV()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	CEffectToolView *pView = (CEffectToolView*) pFrame->GetActiveView ();
	//pView->SetCurClick( FALSE );

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
}

void CParticleDisplayTab::OnBnClickedButtoncolorE()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
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
}

void CParticleDisplayTab::OnBnClickedCheckMesh()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CWnd*		pWnd;
	CButton*	pButton;

	pButton = (CButton*)GetDlgItem ( IDC_CHECK_MESH );
	if ( pButton->GetCheck() )
	{
		m_pProperty->m_dwFlag |= USEMESH;

		pWnd = GetDlgItem ( IDC_BUTTON_MESH );
		pWnd->EnableWindow ( TRUE );

		pWnd = GetDlgItem ( IDC_BUTTON_TEX_SCALE );
		pWnd->EnableWindow ( FALSE );

		pWnd = GetDlgItem ( IDC_BUTTON_TEXROTATE );
		pWnd->EnableWindow ( FALSE );

		pWnd = GetDlgItem ( IDC_STATIC2 );
		pWnd->SetWindowText (" 파티클 메쉬(Mesh)");
	}
	else
	{
		m_pProperty->m_dwFlag &= ~USEMESH;

		pWnd = GetDlgItem ( IDC_BUTTON_MESH );
		pWnd->EnableWindow ( FALSE );
			
		pWnd = GetDlgItem ( IDC_BUTTON_TEX_SCALE );
		pWnd->EnableWindow ( TRUE );

		pWnd = GetDlgItem ( IDC_BUTTON_TEXROTATE );
		pWnd->EnableWindow ( TRUE );

		pWnd = GetDlgItem ( IDC_STATIC2 );
		pWnd->SetWindowText (" 파티클");
	}
}

void CParticleDisplayTab::OnBnClickedCheckTex()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CWnd*		pWnd;
	CButton*	pButton;

	pButton = (CButton*)GetDlgItem ( IDC_CHECK_TEX );
	if ( pButton->GetCheck() )
	{
		m_pProperty->m_dwFlag |= USETEXTURE;

		pWnd = GetDlgItem ( IDC_BUTTON_TEXTURE );
		pWnd->EnableWindow ( TRUE );
	}
	else
	{
		m_pProperty->m_dwFlag &= ~USETEXTURE;

		pWnd = GetDlgItem ( IDC_BUTTON_TEXTURE );
		pWnd->EnableWindow ( FALSE );
	}
}

void CParticleDisplayTab::OnBnClickedButtonMesh()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	CEffectToolView *pView = (CEffectToolView*) pFrame->GetActiveView ();
	pView->SetCurClick( FALSE );

	CString szFilter = "x File (*.x)|*.x|";
	CFileDialog dlg ( TRUE, ".x", NULL, 
		OFN_HIDEREADONLY, szFilter, this );

	CString StrInitDir = DxSimpleMeshMan::GetInstance().GetPath().c_str();
	dlg.m_ofn.lpstrInitialDir = StrInitDir;

	if ( dlg.DoModal() == IDOK )
	{
		CString FilePath = dlg.GetFileName ();

		CWnd *pWnd;
		pWnd = GetDlgItem ( IDC_EDIT_MESH );
		pWnd->SetWindowText ( FilePath );
	}

	pView->SetCurClick( TRUE );	// 커서 클릭을 살린다.
}

void CParticleDisplayTab::OnBnClickedButtonTexture()
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

void CParticleDisplayTab::OnBnClickedButtonNormal()
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
void CParticleDisplayTab::OnBnClickedButtonGroundTex()
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

void CParticleDisplayTab::OnBnClickedCheckNeweff()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CButton*	pButton;

	CWnd* pWnd = GetDlgItem ( IDC_BUTTON_NEWEFF );
	pButton = (CButton*)GetDlgItem ( IDC_CHECK_NEWEFF );
	if ( pButton->GetCheck() )
	{
		pWnd->EnableWindow ( TRUE );
		m_pProperty->m_dwFlag |= USENEWEFF;
	}
	else
	{
		pWnd->EnableWindow ( FALSE );
		m_pProperty->m_dwFlag &= ~USENEWEFF;
	}
}

void CParticleDisplayTab::OnBnClickedButtonNeweff()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	CEffectToolView *pView = (CEffectToolView*) pFrame->GetActiveView ();
	pView->SetCurClick( FALSE );

	CString szFilter = "Effect Group (*.egp)|*.egp|";
	CFileDialog dlg ( TRUE, ".egp", NULL, OFN_HIDEREADONLY, szFilter, this );

	dlg.m_ofn.lpstrInitialDir = DxEffSinglePropGMan::GetInstance().GetPath().c_str();
	if ( dlg.DoModal() == IDOK )
	{
		CString FilePath = dlg.GetFileName ();

		CWnd *pWnd;
		pWnd = GetDlgItem ( IDC_EDIT_NEWEFF );
		pWnd->SetWindowText ( FilePath );
	}

	pView->SetCurClick( TRUE );	// 커서 클릭을 살린다.
}

void CParticleDisplayTab::OnBnClickedRadioNeweffFirst()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pProperty->m_dwFlag &= ~USENEWEFF_END;
	m_pProperty->m_dwFlag &= ~USENEWEFF_END_STOP;
}

void CParticleDisplayTab::OnBnClickedRadioNeweffEnd()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pProperty->m_dwFlag |= USENEWEFF_END;
	m_pProperty->m_dwFlag &= ~USENEWEFF_END_STOP;
}

void CParticleDisplayTab::OnBnClickedRadioNeweffEndStop()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pProperty->m_dwFlag |= USENEWEFF_END_STOP;
	m_pProperty->m_dwFlag &= ~USENEWEFF_END;
}






