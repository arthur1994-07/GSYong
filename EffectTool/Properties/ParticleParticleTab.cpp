// ParticleParticleTab.cpp : implementation file
//

#include "stdafx.h"
#include "../EffectTool.h"
#include "../MainFrm.h"

#include "../EffectToolDoc.h"
#include "../EffectToolView.h"

#include "../Dialog/Range.h"
#include "../Dialog/SetVar.h"
#include "../Dialog/SetMultiSlider.h"

#include "ParticleParticleTab.h"
#include ".\particleparticletab.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// CParticleParticleTab dialog


CParticleParticleTab::CParticleParticleTab(CWnd* pParent /*=NULL*/)
	: CDialog(CParticleParticleTab::IDD, pParent),
	m_pProperty(NULL)
{
	//{{AFX_DATA_INIT(CParticleParticleTab)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CParticleParticleTab::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CParticleParticleTab)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CParticleParticleTab, CDialog)
	//{{AFX_MSG_MAP(CParticleParticleTab)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_LIFE, OnBnClickedButtonLife)
	ON_BN_CLICKED(IDC_BUTTON_VELOCITY, OnBnClickedButtonVelocity)
	ON_BN_CLICKED(IDC_BUTTON_WINDDIR, OnBnClickedButtonWinddir)
	ON_BN_CLICKED(IDC_BUTTON_THROW, OnBnClickedButtonThrow)
	ON_BN_CLICKED(IDC_BUTTON_FROMTO, OnBnClickedButtonFromto)
	ON_BN_CLICKED(IDC_BUTTON_GRAVITY, OnBnClickedButtonGravity)
	ON_BN_CLICKED(IDC_BUTTON_COLLISION, OnBnClickedButtonCollision)
	ON_BN_CLICKED(IDC_BUTTON_ATTRACTION, OnBnClickedButtonAttraction)
	ON_BN_CLICKED(IDC_BUTTON_CENTER, OnBnClickedButtonCenter)
	ON_BN_CLICKED(IDC_BUTTON_DIRECTION, OnBnClickedButtonDirection)
	ON_BN_CLICKED(IDC_BUTTON_HEIGHT_NO, OnBnClickedButtonHeightNo)
	ON_BN_CLICKED(IDC_BUTTON_MOVE_MATRIX, OnBnClickedButtonMoveMatrix)
	ON_BN_CLICKED(IDC_BUTTON_ROTATE_MATRIX, OnBnClickedButtonRotateMatrix)
	ON_BN_CLICKED(IDC_BUTTON_ROTATE_WORLD, OnBnClickedButtonRotateWorld)
	ON_BN_CLICKED(IDC_BUTTON_RANDOM_PLAY, OnBnClickedButtonRandomPlay)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CParticleParticleTab message handlers

void CParticleParticleTab::Init ( PARTICLESYS_PROPERTY* pProp, CsheetWithTab* pSheetTab )
{
	m_pProperty = pProp;
	m_pSheetTab	= pSheetTab;

	//CWnd*		pWnd;
	//CButton*	pButton;

	{
		m_cClrThrow.SubclassDlgItem ( IDC_BUTTON_THROW, this );
		m_cClrFromTo.SubclassDlgItem ( IDC_BUTTON_FROMTO, this );
		m_cClrCollision.SubclassDlgItem ( IDC_BUTTON_COLLISION, this );
		m_cClrAttraction.SubclassDlgItem ( IDC_BUTTON_ATTRACTION, this );
		m_cClrCenter.SubclassDlgItem ( IDC_BUTTON_CENTER, this );
		m_cClrDirection.SubclassDlgItem ( IDC_BUTTON_DIRECTION, this );
		m_cClrHeightNO.SubclassDlgItem ( IDC_BUTTON_HEIGHT_NO, this );
		m_cClrMoveMatrix.SubclassDlgItem ( IDC_BUTTON_MOVE_MATRIX, this );
		m_cClrRotateMatrix.SubclassDlgItem ( IDC_BUTTON_ROTATE_MATRIX, this );
		m_cClrRotateWorld.SubclassDlgItem ( IDC_BUTTON_ROTATE_WORLD, this );
		m_cClrRandomPlay.SubclassDlgItem ( IDC_BUTTON_RANDOM_PLAY, this );

		COLORREF cColorBLUE, cColorGRAY, cColorTEXT;
		cColorGRAY = RGB(212, 208, 200);
		cColorBLUE = RGB(192, 192, 255);
		cColorTEXT = RGB(0, 0, 0);

		if ( pProp->m_dwFlag & USEBEZIER_P )	m_cClrThrow.SetColour ( cColorTEXT, cColorBLUE );
		else									m_cClrThrow.SetColour ( cColorTEXT, cColorGRAY );

		if ( pProp->m_dwFlag & USEFROMTO )		m_cClrFromTo.SetColour ( cColorTEXT, cColorBLUE );
		else									m_cClrFromTo.SetColour ( cColorTEXT, cColorGRAY );

		if ( pProp->m_dwFlag & USECOLLISION )	m_cClrCollision.SetColour ( cColorTEXT, cColorBLUE );
		else									m_cClrCollision.SetColour ( cColorTEXT, cColorGRAY );

		if ( pProp->m_dwFlag & USEATTRACTION )	m_cClrAttraction.SetColour ( cColorTEXT, cColorBLUE );
		else									m_cClrAttraction.SetColour ( cColorTEXT, cColorGRAY );

		if ( pProp->m_dwFlag & USECENTER )		m_cClrCenter.SetColour ( cColorTEXT, cColorBLUE );
		else									m_cClrCenter.SetColour ( cColorTEXT, cColorGRAY );

		if ( pProp->m_dwFlag & USEDIRECTION )	m_cClrDirection.SetColour ( cColorTEXT, cColorBLUE );
		else									m_cClrDirection.SetColour ( cColorTEXT, cColorGRAY );

		if ( pProp->m_dwFlag & USEHEIGHTNO )	m_cClrHeightNO.SetColour ( cColorTEXT, cColorBLUE );
		else									m_cClrHeightNO.SetColour ( cColorTEXT, cColorGRAY );

		if ( pProp->m_dwFlag & USEPARENTMOVE )	m_cClrMoveMatrix.SetColour ( cColorTEXT, cColorBLUE );
		else									m_cClrMoveMatrix.SetColour ( cColorTEXT, cColorGRAY );

		if ( pProp->m_dwFlag & USEPARENTROTATE_FOR_PARTICLE )	m_cClrRotateMatrix.SetColour ( cColorTEXT, cColorBLUE );
		else										m_cClrRotateMatrix.SetColour ( cColorTEXT, cColorGRAY );

		if ( pProp->m_dwFlag & USEPARENTROTATEWORLD )	m_cClrRotateWorld.SetColour ( cColorTEXT, cColorBLUE );
		else											m_cClrRotateWorld.SetColour ( cColorTEXT, cColorGRAY );

		if ( pProp->m_dwFlag & USERANDOMPLAY )			m_cClrRandomPlay.SetColour ( cColorTEXT, cColorBLUE );
		else											m_cClrRandomPlay.SetColour ( cColorTEXT, cColorGRAY );
	}
}

void CParticleParticleTab::Apply ()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//


void CParticleParticleTab::OnBnClickedButtonLife()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CSetVar DlgSetVar;

	DlgSetVar.m_StrTitle = "Set Particle Life";
	DlgSetVar.m_dwFlag = USESTARTSCROLL | USESTARTVARSCROLL;
	DlgSetVar.m_bZeroBase = TRUE;
	DlgSetVar.m_ToScale = 40.0f;

	DlgSetVar.m_fStart = m_pProperty->m_fLife;
	DlgSetVar.m_fStartVar = m_pProperty->m_fLifeVar;
	
	if ( IDOK == DlgSetVar.DoModal() )
	{
		m_pProperty->m_fLife = DlgSetVar.m_fStart;
		m_pProperty->m_fLifeVar = DlgSetVar.m_fStartVar;
	}
}

void CParticleParticleTab::OnBnClickedButtonVelocity()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CSetVar DlgSetVar;

	DlgSetVar.m_StrTitle = "Set Particle Speed";
	DlgSetVar.m_dwFlag = USESTARTSCROLL | USESTARTVARSCROLL;
	DlgSetVar.m_ToScale = 20.0f;
	
	DlgSetVar.m_fStart = m_pProperty->m_fSpeed;
	DlgSetVar.m_fStartVar = m_pProperty->m_fSpeedVar;
	
	if ( IDOK == DlgSetVar.DoModal() )
	{
		m_pProperty->m_fSpeed = DlgSetVar.m_fStart;
		m_pProperty->m_fSpeedVar = DlgSetVar.m_fStartVar;
	}
}

void CParticleParticleTab::OnBnClickedButtonWinddir()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pSheetTab->ActiveDirectionPage ( (D3DXVECTOR3*) &m_pProperty->m_vWindDir, 5.f, PARTICLEPAGEINDEX );
}

void CParticleParticleTab::OnBnClickedButtonGravity()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CSetVar DlgSetVar;

	DlgSetVar.m_StrTitle = "Set Particle Gravity";
	DlgSetVar.m_dwFlag = USESTARTSCROLL | USESTARTVARSCROLL | USEENDSCROLL;

	DlgSetVar.m_fStart = m_pProperty->m_fGravityStart;
	DlgSetVar.m_fStartVar = m_pProperty->m_fGravityVar;
	DlgSetVar.m_fEnd = m_pProperty->m_fGravityEnd;
	DlgSetVar.m_ToScale = 400.0f;
	
	if ( IDOK == DlgSetVar.DoModal() )
	{
		m_pProperty->m_fGravityStart = DlgSetVar.m_fStart;
		m_pProperty->m_fGravityVar = DlgSetVar.m_fStartVar;
		m_pProperty->m_fGravityEnd = DlgSetVar.m_fEnd;
	}
}

void CParticleParticleTab::OnBnClickedButtonThrow()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	COLORREF cColorBLUE, cColorGRAY, cColorTEXT;
	cColorGRAY = RGB(212, 208, 200);
	cColorBLUE = RGB(192, 192, 255);
	cColorTEXT = RGB(0, 0, 0);

	m_pProperty->m_dwFlag ^= USEBEZIER_P;

	if ( m_pProperty->m_dwFlag & USEBEZIER_P )	m_cClrThrow.SetColour ( cColorTEXT, cColorBLUE );
	else										m_cClrThrow.SetColour ( cColorTEXT, cColorGRAY );
}

void CParticleParticleTab::OnBnClickedButtonFromto()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	COLORREF cColorBLUE, cColorGRAY, cColorTEXT;
	cColorGRAY = RGB(212, 208, 200);
	cColorBLUE = RGB(192, 192, 255);
	cColorTEXT = RGB(0, 0, 0);

	m_pProperty->m_dwFlag ^= USEFROMTO;

	if ( m_pProperty->m_dwFlag & USEFROMTO )	m_cClrFromTo.SetColour ( cColorTEXT, cColorBLUE );
	else										m_cClrFromTo.SetColour ( cColorTEXT, cColorGRAY );
}

void CParticleParticleTab::OnBnClickedButtonCollision()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	COLORREF cColorBLUE, cColorGRAY, cColorTEXT;
	cColorGRAY = RGB(212, 208, 200);
	cColorBLUE = RGB(192, 192, 255);
	cColorTEXT = RGB(0, 0, 0);

	m_pProperty->m_dwFlag ^= USECOLLISION;

	if ( m_pProperty->m_dwFlag & USECOLLISION )	m_cClrCollision.SetColour ( cColorTEXT, cColorBLUE );
	else										m_cClrCollision.SetColour ( cColorTEXT, cColorGRAY );
}

void CParticleParticleTab::OnBnClickedButtonAttraction()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	COLORREF cColorBLUE, cColorGRAY, cColorTEXT;
	cColorGRAY = RGB(212, 208, 200);
	cColorBLUE = RGB(192, 192, 255);
	cColorTEXT = RGB(0, 0, 0);

	m_pProperty->m_dwFlag ^= USEATTRACTION;

	if ( m_pProperty->m_dwFlag & USEATTRACTION )	m_cClrAttraction.SetColour ( cColorTEXT, cColorBLUE );
	else											m_cClrAttraction.SetColour ( cColorTEXT, cColorGRAY );
}

void CParticleParticleTab::OnBnClickedButtonCenter()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	COLORREF cColorBLUE, cColorGRAY, cColorTEXT;
	cColorGRAY = RGB(212, 208, 200);
	cColorBLUE = RGB(192, 192, 255);
	cColorTEXT = RGB(0, 0, 0);

	m_pProperty->m_dwFlag ^= USECENTER;

	if ( m_pProperty->m_dwFlag & USECENTER )	m_cClrCenter.SetColour ( cColorTEXT, cColorBLUE );
	else										m_cClrCenter.SetColour ( cColorTEXT, cColorGRAY );
}

void CParticleParticleTab::OnBnClickedButtonDirection()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	COLORREF cColorBLUE, cColorGRAY, cColorTEXT;
	cColorGRAY = RGB(212, 208, 200);
	cColorBLUE = RGB(192, 192, 255);
	cColorTEXT = RGB(0, 0, 0);

	m_pProperty->m_dwFlag ^= USEDIRECTION;

	if ( m_pProperty->m_dwFlag & USEDIRECTION )	m_cClrDirection.SetColour ( cColorTEXT, cColorBLUE );
	else										m_cClrDirection.SetColour ( cColorTEXT, cColorGRAY );
}

void CParticleParticleTab::OnBnClickedButtonHeightNo()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	COLORREF cColorBLUE, cColorGRAY, cColorTEXT;
	cColorGRAY = RGB(212, 208, 200);
	cColorBLUE = RGB(192, 192, 255);
	cColorTEXT = RGB(0, 0, 0);

	m_pProperty->m_dwFlag ^= USEHEIGHTNO;

	if ( m_pProperty->m_dwFlag & USEHEIGHTNO )	m_cClrHeightNO.SetColour ( cColorTEXT, cColorBLUE );
	else										m_cClrHeightNO.SetColour ( cColorTEXT, cColorGRAY );
}

void CParticleParticleTab::OnBnClickedButtonMoveMatrix()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	COLORREF cColorBLUE, cColorGRAY, cColorTEXT;
	cColorGRAY = RGB(212, 208, 200);
	cColorBLUE = RGB(192, 192, 255);
	cColorTEXT = RGB(0, 0, 0);

	m_pProperty->m_dwFlag ^= USEPARENTMOVE;

	if ( m_pProperty->m_dwFlag & USEPARENTMOVE )	m_cClrMoveMatrix.SetColour ( cColorTEXT, cColorBLUE );
	else											m_cClrMoveMatrix.SetColour ( cColorTEXT, cColorGRAY );
}

void CParticleParticleTab::OnBnClickedButtonRotateMatrix()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	COLORREF cColorBLUE, cColorGRAY, cColorTEXT;
	cColorGRAY = RGB(212, 208, 200);
	cColorBLUE = RGB(192, 192, 255);
	cColorTEXT = RGB(0, 0, 0);

	m_pProperty->m_dwFlag ^= USEPARENTROTATE_FOR_PARTICLE;

	if ( m_pProperty->m_dwFlag & USEPARENTROTATE_FOR_PARTICLE )	m_cClrRotateMatrix.SetColour ( cColorTEXT, cColorBLUE );
	else											m_cClrRotateMatrix.SetColour ( cColorTEXT, cColorGRAY );
}

void CParticleParticleTab::OnBnClickedButtonRotateWorld()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	COLORREF cColorBLUE, cColorGRAY, cColorTEXT;
	cColorGRAY = RGB(212, 208, 200);
	cColorBLUE = RGB(192, 192, 255);
	cColorTEXT = RGB(0, 0, 0);

	m_pProperty->m_dwFlag ^= USEPARENTROTATEWORLD;

	if ( m_pProperty->m_dwFlag & USEPARENTROTATEWORLD )	m_cClrRotateWorld.SetColour ( cColorTEXT, cColorBLUE );
	else												m_cClrRotateWorld.SetColour ( cColorTEXT, cColorGRAY );
}
void CParticleParticleTab::OnBnClickedButtonRandomPlay()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	COLORREF cColorBLUE, cColorGRAY, cColorTEXT;
	cColorGRAY = RGB(212, 208, 200);
	cColorBLUE = RGB(192, 192, 255);
	cColorTEXT = RGB(0, 0, 0);

	m_pProperty->m_dwFlag ^= USERANDOMPLAY;

	if ( m_pProperty->m_dwFlag & USERANDOMPLAY )
	{
		m_cClrRandomPlay.SetColour ( cColorTEXT, cColorBLUE );

		CRange	sDlg;
		sDlg.m_bRandomDirect = m_pProperty->m_dwFlag&USERANDOMDIRECT;
		sDlg.m_fPlayTimeS = m_pProperty->m_vPlayTime.x;
		sDlg.m_fPlayTimeE = m_pProperty->m_vPlayTime.y;
		sDlg.m_fSleepTimeS = m_pProperty->m_vSleepTime.x;
		sDlg.m_fSleepTimeE = m_pProperty->m_vSleepTime.y;
		if( IDOK == sDlg.DoModal() )
		{
			if( sDlg.m_bRandomDirect )	m_pProperty->m_dwFlag |= USERANDOMDIRECT;
			else						m_pProperty->m_dwFlag &= ~USERANDOMDIRECT;
			
			m_pProperty->m_vPlayTime.x = sDlg.m_fPlayTimeS;
			m_pProperty->m_vPlayTime.y = sDlg.m_fPlayTimeE;
			m_pProperty->m_vSleepTime.x = sDlg.m_fSleepTimeS;
			m_pProperty->m_vSleepTime.y = sDlg.m_fSleepTimeE;
		}
	}
	else											m_cClrRandomPlay.SetColour ( cColorTEXT, cColorGRAY );
}



