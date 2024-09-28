// ParticleEmitterTab.cpp : implementation file
//

#include "stdafx.h"
#include "../EffectTool.h"
#include "../MainFrm.h"

#include "../EffectToolDoc.h"
#include "../EffectToolView.h"

#include "EffSheetWithTab.h"
#include "../Dialog/SetVar.h"
#include "../Dialog/SetMultiSlider.h"

#include "ParticleEmitterTab.h"
#include ".\particleemittertab.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// CParticleEmitterTab dialog


CParticleEmitterTab::CParticleEmitterTab(CWnd* pParent /*=NULL*/)
	: CDialog(CParticleEmitterTab::IDD, pParent),
	m_pProperty(NULL)
{
	//{{AFX_DATA_INIT(CParticleEmitterTab)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CParticleEmitterTab::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CParticleEmitterTab)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CParticleEmitterTab, CDialog)
	//{{AFX_MSG_MAP(CParticleEmitterTab)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_RANGE, OnBnClickedButtonRange)
	ON_BN_CLICKED(IDC_BUTTON_RANGE_HIGH, OnBnClickedButtonRangeHigh)
	ON_BN_CLICKED(IDC_BUTTON_ROTATE, OnBnClickedButtonRotate)
	ON_BN_CLICKED(IDC_BUTTON_INIT_DIRECT, OnBnClickedButtonInitDirect)
	ON_BN_CLICKED(IDC_BUTTON_GRAVITY_MAIN, OnBnClickedButtonGravityMain)
	ON_BN_CLICKED(IDC_BUTTON_GRAVITY_MAIN_E, OnBnClickedButtonGravityMainE)
	ON_BN_CLICKED(IDC_BUTTON_GOTOCENTER, OnBnClickedButtonGotocenter)
	ON_BN_CLICKED(IDC_BUTTON_RANGE_DISABLE, OnBnClickedButtonRangeDisable)
	ON_BN_CLICKED(IDC_BUTTON_RANGE_HIGH_DISABLE, OnBnClickedButtonRangeHighDisable)
	ON_BN_CLICKED(IDC_BUTTON_ROTATE_DISABLE, OnBnClickedButtonRotateDisable)
	ON_BN_CLICKED(IDC_BUTTON_DIRECT_DISABLE, OnBnClickedButtonDirectDisable)
	ON_BN_CLICKED(IDC_BUTTON_GRAVITY_S_DISABLE, OnBnClickedButtonGravitySDisable)
	ON_BN_CLICKED(IDC_BUTTON_GRAVITY_E_DISABLE, OnBnClickedButtonGravityEDisable)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CParticleEmitterTab message handlers

void CParticleEmitterTab::Init ( PARTICLESYS_PROPERTY* pProp, CsheetWithTab* pSheetTab )
{
	m_pSheetTab = pSheetTab;
	m_pProperty = pProp;

	//	//	Note : 생성 가능한 것과 불가능한 것
	////
	//pWnd = GetDlgItem ( IDC_BUTTON_RANGE );
	//pWnd->EnableWindow ( TRUE );
	//pWnd = GetDlgItem ( IDC_BUTTON_RANGE_HIGH );
	//pWnd->EnableWindow ( FALSE );
	//pWnd = GetDlgItem ( IDC_BUTTON_ROTATEL );
	//pWnd->EnableWindow ( TRUE );

	//if ( m_pProperty->m_dwFlag & USERANGE )
	//{
	//	pWnd = GetDlgItem ( IDC_BUTTON_RANGE );
	//	pWnd->EnableWindow ( TRUE );
	//	pWnd = GetDlgItem ( IDC_BUTTON_RANGE_HIGH );
	//	pWnd->EnableWindow ( TRUE );	
	//	pWnd = GetDlgItem ( IDC_BUTTON_ROTATEL );
	//	pWnd->EnableWindow ( FALSE );

	//	if  ( m_pProperty->m_dwFlag & USERANGEHIGH )
	//	{
	//		pWnd = GetDlgItem ( IDC_BUTTON_TEXROTATE );
	//		pWnd->EnableWindow ( FALSE );
	//		pWnd = GetDlgItem ( IDC_BUTTON_TEX_SCALE );
	//		pWnd->EnableWindow ( FALSE );
	//	}
	//}
	//else if ( m_pProperty->m_dwFlag & USEROTATEL )	
	//{
	//	pWnd = GetDlgItem ( IDC_BUTTON_RANGE );
	//	pWnd->EnableWindow ( FALSE );
	//	pWnd = GetDlgItem ( IDC_BUTTON_RANGE_HIGH );
	//	pWnd->EnableWindow ( FALSE );
	//	pWnd = GetDlgItem ( IDC_BUTTON_ROTATEL );
	//	pWnd->EnableWindow ( TRUE );
	//}

	{
		m_cClrRange.SubclassDlgItem ( IDC_BUTTON_RANGE, this );
		m_cClrRangeHigh.SubclassDlgItem ( IDC_BUTTON_RANGE_HIGH, this );
		m_cClrRotate.SubclassDlgItem ( IDC_BUTTON_ROTATE, this );

		m_cClrDirect.SubclassDlgItem ( IDC_BUTTON_INIT_DIRECT, this );
		m_cClrGravityS.SubclassDlgItem ( IDC_BUTTON_GRAVITY_MAIN, this );
		m_cClrGravityE.SubclassDlgItem ( IDC_BUTTON_GRAVITY_MAIN_E, this );
		m_cClrCollision.SubclassDlgItem ( IDC_BUTTON_GCOLLISION, this );
		m_cClrGotoCenter.SubclassDlgItem ( IDC_BUTTON_GOTOCENTER, this );

		COLORREF cColorBLUE, cColorGRAY, cColorTEXT;
		cColorGRAY = RGB(212, 208, 200);
		cColorBLUE = RGB(192, 192, 255);
		cColorTEXT = RGB(0, 0, 0);

		if ( pProp->m_dwFlag & USERANGE )		m_cClrRange.SetColour ( cColorTEXT, cColorBLUE );
		else									m_cClrRange.SetColour ( cColorTEXT, cColorGRAY );

		if ( pProp->m_dwFlag & USERANGEHIGH )	m_cClrRangeHigh.SetColour ( cColorTEXT, cColorBLUE );
		else									m_cClrRangeHigh.SetColour ( cColorTEXT, cColorGRAY );

		if ( pProp->m_dwFlag & USEROTATEL )		m_cClrRotate.SetColour ( cColorTEXT, cColorBLUE );
		else									m_cClrRotate.SetColour ( cColorTEXT, cColorGRAY );


		if ( (m_pProperty->m_vGVelocity.x==0.f) && 
			(m_pProperty->m_vGVelocity.y==0.f) &&
			(m_pProperty->m_vGVelocity.z==0.f) )
		{
			m_cClrDirect.SetColour ( cColorTEXT, cColorGRAY );
		}
		else	m_cClrDirect.SetColour ( cColorTEXT, cColorBLUE );

		if ( (m_pProperty->m_vGGravityStart.x==0.f) && 
			(m_pProperty->m_vGGravityStart.y==0.f) &&
			(m_pProperty->m_vGGravityStart.z==0.f) )
		{
			m_cClrGravityS.SetColour ( cColorTEXT, cColorGRAY );
		}
		else	m_cClrGravityS.SetColour ( cColorTEXT, cColorBLUE );

		if ( (m_pProperty->m_vGGravityEnd.x==0.f) && 
			(m_pProperty->m_vGGravityEnd.y==0.f) &&
			(m_pProperty->m_vGGravityEnd.z==0.f) )
		{
			m_cClrGravityE.SetColour ( cColorTEXT, cColorGRAY );
		}
		else	m_cClrGravityE.SetColour ( cColorTEXT, cColorBLUE );

		if ( pProp->m_dwFlag & USEGOTOCENTER )	m_cClrGotoCenter.SetColour ( cColorTEXT, cColorBLUE );
		else									m_cClrGotoCenter.SetColour ( cColorTEXT, cColorGRAY );
	}
}

void CParticleEmitterTab::Apply ()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//

void CParticleEmitterTab::OnBnClickedButtonRange()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CSetVar DlgSetVar;

	DlgSetVar.m_StrTitle	= "Set Particle Range";
	DlgSetVar.m_StrUse		= "Screen";
	DlgSetVar.m_StrComm1	= "X 축";
	DlgSetVar.m_StrComm2	= "Y 축";
	DlgSetVar.m_StrComm3	= "Z 축";
	DlgSetVar.m_bZeroBase	= TRUE;

	DlgSetVar.m_dwFlag = USECHECK | USESTARTSCROLL | USESTARTVARSCROLL | USEENDSCROLL;

	DlgSetVar.m_bUse = ((m_pProperty->m_dwFlagEX & USE_EX_RANGE_SCREEN) ? TRUE : FALSE);
	DlgSetVar.m_fStart = m_pProperty->m_vRange.x;
	DlgSetVar.m_fStartVar = m_pProperty->m_vRange.y;
	DlgSetVar.m_fEnd = m_pProperty->m_vRange.z;
	
	if ( IDOK == DlgSetVar.DoModal() )
	{
		m_pProperty->m_vRange.x = DlgSetVar.m_fStart;
		m_pProperty->m_vRange.y = DlgSetVar.m_fStartVar;
		m_pProperty->m_vRange.z = DlgSetVar.m_fEnd;

		m_pProperty->m_dwFlag |= USERANGE;

		if ( DlgSetVar.m_bUse )
		{
			m_pProperty->m_dwFlagEX |= USE_EX_RANGE_SCREEN;
		}
		else
		{
			m_pProperty->m_dwFlagEX &= ~USE_EX_RANGE_SCREEN;
		}

		COLORREF cColorBLUE = RGB(192, 192, 255);
		COLORREF cColorTEXT = RGB(0, 0, 0);
		m_cClrRange.SetColour ( cColorTEXT, cColorBLUE );
	}
}

void CParticleEmitterTab::OnBnClickedButtonRangeHigh()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CSetVar DlgSetVar;

	DlgSetVar.m_StrTitle = "Set Particle Range High";
	DlgSetVar.m_StrRadio1 = "원";
	DlgSetVar.m_StrRadio2 = "직사각형";
	DlgSetVar.m_StrComm1 = "회전스피드";
	DlgSetVar.m_StrComm2 = "2차 범위";
	DlgSetVar.m_ToScale = 1.0f;

	DlgSetVar.m_dwFlag = USERADIO | USESTARTSCROLL | USESTARTVARSCROLL;

	DlgSetVar.m_iRadio = ((m_pProperty->m_dwFlag & USESPHERE) ? 0 : 1 );
	DlgSetVar.m_fStart = m_pProperty->m_fRotateAngel;
	DlgSetVar.m_fStartVar = m_pProperty->m_fRangeRate;

	
	if ( IDOK == DlgSetVar.DoModal() )
	{
		if ( DlgSetVar.m_iRadio == 0 )			m_pProperty->m_dwFlag |= USESPHERE;
		else if ( DlgSetVar.m_iRadio == 1 )		m_pProperty->m_dwFlag &= ~USESPHERE;

		m_pProperty->m_fRotateAngel = DlgSetVar.m_fStart;
		m_pProperty->m_fRangeRate = DlgSetVar.m_fStartVar;

		m_pProperty->m_dwFlag |= USERANGEHIGH;

		COLORREF cColorBLUE = RGB(192, 192, 255);
		COLORREF cColorTEXT = RGB(0, 0, 0);
		m_cClrRangeHigh.SetColour ( cColorTEXT, cColorBLUE );
	}
}

void CParticleEmitterTab::OnBnClickedButtonRotate()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CSetMultiSlider DlgSetSlider;

	DlgSetSlider.m_StrTitle = "Set Rotation";
	DlgSetSlider.m_StrTemp	= "1초 회전수";
	DlgSetSlider.m_StrTemp4 = "반지름 :";

	DlgSetSlider.m_dwFlag	= USEEDIT;

	DlgSetSlider.m_fTemp	= m_pProperty->m_fRotateLAngel;

	DlgSetSlider.m_values[0] = m_pProperty->m_fRotateLRate1;
	DlgSetSlider.m_values[1] = m_pProperty->m_fRotateLRate2;

	DlgSetSlider.m_fStart	= m_pProperty->m_fLengthStart;
	DlgSetSlider.m_fMid1	= m_pProperty->m_fLengthMid1;
	DlgSetSlider.m_fMid2	= m_pProperty->m_fLengthMid2;
	DlgSetSlider.m_fEnd		= m_pProperty->m_fLengthEnd;
	
	if ( IDOK == DlgSetSlider.DoModal() )
	{
		m_pProperty->m_fRotateLAngel	= DlgSetSlider.m_fTemp;

		m_pProperty->m_fRotateLRate1	= DlgSetSlider.m_values[0];
		m_pProperty->m_fRotateLRate2	= DlgSetSlider.m_values[1];
	
		m_pProperty->m_fLengthStart		= DlgSetSlider.m_fStart;
		m_pProperty->m_fLengthMid1		= DlgSetSlider.m_fMid1;
		m_pProperty->m_fLengthMid2		= DlgSetSlider.m_fMid2;
		m_pProperty->m_fLengthEnd		= DlgSetSlider.m_fEnd;


		m_pProperty->m_dwFlag |= USEROTATEL;

		COLORREF cColorBLUE = RGB(192, 192, 255);
		COLORREF cColorTEXT = RGB(0, 0, 0);
		m_cClrRotate.SetColour ( cColorTEXT, cColorBLUE );
	}
}

void CParticleEmitterTab::OnBnClickedButtonRangeDisable()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pProperty->m_dwFlag &= ~USERANGE;

	COLORREF cColorTEXT	= RGB(0, 0, 0);
	COLORREF cColorGRAY = RGB(212, 208, 200);
	m_cClrRange.SetColour ( cColorTEXT, cColorGRAY );
}

void CParticleEmitterTab::OnBnClickedButtonRangeHighDisable()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pProperty->m_dwFlag &= ~USERANGEHIGH;

	COLORREF cColorTEXT	= RGB(0, 0, 0);
	COLORREF cColorGRAY = RGB(212, 208, 200);
	m_cClrRangeHigh.SetColour ( cColorTEXT, cColorGRAY );
}

void CParticleEmitterTab::OnBnClickedButtonRotateDisable()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pProperty->m_dwFlag &= ~USEROTATEL;

	COLORREF cColorTEXT	= RGB(0, 0, 0);
	COLORREF cColorGRAY = RGB(212, 208, 200);
	m_cClrRotate.SetColour ( cColorTEXT, cColorGRAY );
}

void CParticleEmitterTab::ReSetButton ()
{
	////	Note : 생성 가능한 것과 불가능한 것
	////
	//if ( m_pProperty->m_dwFlag & USERANGE )
	//{
	//	pWnd = GetDlgItem ( IDC_BUTTON_RANGE );
	//	pWnd->EnableWindow ( TRUE );
	//	pWnd = GetDlgItem ( IDC_BUTTON_RANGE_HIGH );
	//	pWnd->EnableWindow ( TRUE );	
	//	pWnd = GetDlgItem ( IDC_BUTTON_ROTATEL );
	//	pWnd->EnableWindow ( FALSE );
	//}
	//else
	//{
	//	pWnd = GetDlgItem ( IDC_BUTTON_RANGE );
	//	pWnd->EnableWindow ( TRUE );
	//	pWnd = GetDlgItem ( IDC_BUTTON_RANGE_HIGH );
	//	pWnd->EnableWindow ( FALSE );	
	//	pWnd = GetDlgItem ( IDC_BUTTON_ROTATEL );
	//	pWnd->EnableWindow ( TRUE );
	//}


	////	범위 회전을 할때는 이런거 못한다.
	//CWnd *pWnd;
	//if ( !DlgSetVar.m_bUse )
	//{
	//	m_pProperty->m_dwFlag &= ~USERANGEHIGH;

	//	pWnd = GetDlgItem ( IDC_BUTTON_TEXROTATE );
	//	pWnd->EnableWindow ( TRUE );
	//	pWnd = GetDlgItem ( IDC_BUTTON_TEX_SCALE );
	//	pWnd->EnableWindow ( TRUE );

	//	pWnd = GetDlgItem ( IDC_BUTTON_RANGE_HIGH );
	//	pWnd->SetWindowText ( "범위 고급 X" );
	//}
	//else
	//{
	//	m_pProperty->m_dwFlag |= USERANGEHIGH;
	//	pWnd = GetDlgItem ( IDC_BUTTON_RANGE_HIGH );
	//	pWnd->SetWindowText ( "범위 고급 O" );

	//	m_pProperty->m_dwFlag &= ~USETEXROTATE;
	//	pWnd = GetDlgItem ( IDC_BUTTON_TEXROTATE );
	//	pWnd->EnableWindow ( FALSE );
	//	pWnd->SetWindowText ( "텍스쳐 회전 X" );

	//	m_pProperty->m_dwFlag &= ~USETEXSCALE;
	//	pWnd = GetDlgItem ( IDC_BUTTON_TEX_SCALE );
	//	pWnd->EnableWindow ( FALSE );
	//	pWnd->SetWindowText ( "텍스쳐 늘이기 X" );
	//}


	////	Note : 생성 가능한 것과 불가능한 것
	////
	//CWnd *pWnd;

	//if ( m_pProperty->m_dwFlag & USEROTATEL )	//	 파티클 회전을 사용 할때
	//{
	//	pWnd = GetDlgItem ( IDC_BUTTON_RANGE );
	//	pWnd->EnableWindow ( FALSE );
	//	pWnd = GetDlgItem ( IDC_BUTTON_RANGE_HIGH );
	//	pWnd->EnableWindow ( FALSE );	
	//	pWnd = GetDlgItem ( IDC_BUTTON_ROTATEL );
	//	pWnd->EnableWindow ( TRUE );
	//}
	//else if ( m_pProperty->m_dwFlag & USERANGE )	// 범위 회전을 사용 할때
	//{
	//	pWnd = GetDlgItem ( IDC_BUTTON_RANGE );
	//	pWnd->EnableWindow ( TRUE );
	//	pWnd = GetDlgItem ( IDC_BUTTON_RANGE_HIGH );
	//	pWnd->EnableWindow ( TRUE );	
	//	pWnd = GetDlgItem ( IDC_BUTTON_ROTATEL );
	//	pWnd->EnableWindow ( FALSE );
	//}
	//else
	//{
	//	pWnd = GetDlgItem ( IDC_BUTTON_RANGE );
	//	pWnd->EnableWindow ( TRUE );
	//	pWnd = GetDlgItem ( IDC_BUTTON_RANGE_HIGH );
	//	pWnd->EnableWindow ( FALSE );	
	//	pWnd = GetDlgItem ( IDC_BUTTON_ROTATEL );
	//	pWnd->EnableWindow ( TRUE );
	//}
}


//------------------------------------------------------------------------------------------------------------------------------------
//											새로운 Edit 만들시에는 삭제 하도록 하자.
//------------------------------------------------------------------------------------------------------------------------------------

void CParticleEmitterTab::OnBnClickedButtonInitDirect()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pSheetTab->ActiveDirectionPage ( (D3DXVECTOR3*) &m_pProperty->m_vGVelocity, 20.f, PARTICLEPAGEINDEX );	
}

void CParticleEmitterTab::OnBnClickedButtonGravityMain()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pSheetTab->ActiveDirectionPage ( (D3DXVECTOR3*) &m_pProperty->m_vGGravityStart, 20.f, PARTICLEPAGEINDEX );
}

void CParticleEmitterTab::OnBnClickedButtonGravityMainE()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pSheetTab->ActiveDirectionPage ( (D3DXVECTOR3*) &m_pProperty->m_vGGravityEnd, 20.f, PARTICLEPAGEINDEX );
}

void CParticleEmitterTab::SetActive()
{
	if ( !m_pProperty )	return;

	COLORREF cColorBLUE, cColorGRAY, cColorTEXT;
	cColorGRAY = RGB(212, 208, 200);
	cColorBLUE = RGB(192, 192, 255);
	cColorTEXT = RGB(0, 0, 0);

	if ( (m_pProperty->m_vGVelocity.x==0.f) && 
		(m_pProperty->m_vGVelocity.y==0.f) &&
		(m_pProperty->m_vGVelocity.z==0.f) )
	{
		m_cClrDirect.SetColour ( cColorTEXT, cColorGRAY );
	}
	else	m_cClrDirect.SetColour ( cColorTEXT, cColorBLUE );

	if ( (m_pProperty->m_vGGravityStart.x==0.f) && 
		(m_pProperty->m_vGGravityStart.y==0.f) &&
		(m_pProperty->m_vGGravityStart.z==0.f) )
	{
		m_cClrGravityS.SetColour ( cColorTEXT, cColorGRAY );
	}
	else	m_cClrGravityS.SetColour ( cColorTEXT, cColorBLUE );

	if ( (m_pProperty->m_vGGravityEnd.x==0.f) && 
		(m_pProperty->m_vGGravityEnd.y==0.f) &&
		(m_pProperty->m_vGGravityEnd.z==0.f) )
	{
		m_cClrGravityE.SetColour ( cColorTEXT, cColorGRAY );
	}
	else	m_cClrGravityE.SetColour ( cColorTEXT, cColorBLUE );
}

void CParticleEmitterTab::OnBnClickedButtonDirectDisable()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	m_pProperty->m_vGVelocity = D3DXVECTOR3 ( 0.f, 0.f, 0.f );

	COLORREF cColorTEXT	= RGB(0, 0, 0);
	COLORREF cColorGRAY = RGB(212, 208, 200);
	m_cClrDirect.SetColour ( cColorTEXT, cColorGRAY );
}

void CParticleEmitterTab::OnBnClickedButtonGravitySDisable()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	m_pProperty->m_vGGravityStart = D3DXVECTOR3 ( 0.f, 0.f, 0.f );

	COLORREF cColorTEXT	= RGB(0, 0, 0);
	COLORREF cColorGRAY = RGB(212, 208, 200);
	m_cClrGravityS.SetColour ( cColorTEXT, cColorGRAY );
}

void CParticleEmitterTab::OnBnClickedButtonGravityEDisable()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	m_pProperty->m_vGGravityEnd = D3DXVECTOR3 ( 0.f, 0.f, 0.f );

	COLORREF cColorTEXT	= RGB(0, 0, 0);
	COLORREF cColorGRAY = RGB(212, 208, 200);
	m_cClrGravityE.SetColour ( cColorTEXT, cColorGRAY );
}

void CParticleEmitterTab::OnBnClickedButtonGotocenter()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	COLORREF cColorBLUE, cColorGRAY, cColorTEXT;
	cColorGRAY = RGB(212, 208, 200);
	cColorBLUE = RGB(192, 192, 255);
	cColorTEXT = RGB(0, 0, 0);

	m_pProperty->m_dwFlag ^= USEGOTOCENTER;

	if ( m_pProperty->m_dwFlag & USEGOTOCENTER )	m_cClrGotoCenter.SetColour ( cColorTEXT, cColorBLUE );
	else											m_cClrGotoCenter.SetColour ( cColorTEXT, cColorGRAY );
}