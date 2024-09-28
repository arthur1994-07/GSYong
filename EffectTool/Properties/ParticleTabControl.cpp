// ParticleTabControl.cpp : implementation file
//
/////////////////////////////////////////////////////
// This class is provided as is and Ben Hill takes no
// responsibility for any loss of any kind in connection
// to this code.
/////////////////////////////////////////////////////
// Is is meant purely as a educational tool and may
// contain bugs.
/////////////////////////////////////////////////////
// ben@shido.fsnet.co.uk
// http://www.shido.fsnet.co.uk
/////////////////////////////////////////////////////
// Thanks to a mystery poster in the C++ forum on 
// www.codeguru.com I can't find your name to say thanks
// for your Control drawing code. If you are that person 
// thank you very much. I have been able to use some of 
// you ideas to produce this sample application.
/////////////////////////////////////////////////////

#include "stdafx.h"
#include "../EffectTool.h"

#include "EffSheetWithTab.h"

#include "ParticleEmitterTab.h"
#include "ParticleParticleTab.h"
#include "ParticleDisplayTab.h"

#include "ParticleTabControl.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// CParticleTabControl

CParticleTabControl::CParticleTabControl()
{
	m_tabPages[0]=new CParticleEmitterTab;
	m_tabPages[1]=new CParticleParticleTab;
	m_tabPages[2]=new CParticleDisplayTab;
}

CParticleTabControl::~CParticleTabControl()
{
	for(int nCount=0; nCount < NUMBEROFPAGE; nCount++)
	{
		delete m_tabPages[nCount];
	}
}

void CParticleTabControl::Init ( PARTICLESYS_PROPERTY* pProp, CsheetWithTab* pSheetTab )
{
	m_tabCurrent=0;

	m_tabPages[0]->DestroyWindow();
	m_tabPages[1]->DestroyWindow();
	m_tabPages[2]->DestroyWindow();

	m_tabPages[0]->Create(IDD_DIALOG_PARTICLE_EMITTER, this);
	m_tabPages[1]->Create(IDD_DIALOG_PARTICLE_PARTICLE, this);
	m_tabPages[2]->Create(IDD_DIALOG_PARTICLE_DISPLAY, this);

	for ( int i=0; i<NUMBEROFPAGE; ++i )
	{
		m_tabPages[i]->ShowWindow(SW_HIDE);
	}
	m_tabPages[m_tabCurrent]->ShowWindow(SW_SHOW);
	m_tabPages[m_tabCurrent]->SetFocus();

	((CParticleEmitterTab*)m_tabPages[0])->Init(pProp,pSheetTab);
	((CParticleParticleTab*)m_tabPages[1])->Init(pProp,pSheetTab);
	((CParticleDisplayTab*)m_tabPages[2])->Init(pProp);

	SetRectangle();
}

void CParticleTabControl::SetRectangle()
{
	CRect tabRect, itemRect;
	int nX, nY, nXc, nYc;

	GetClientRect(&tabRect);
	GetItemRect(0, &itemRect);

	nX=itemRect.left;
	nY=itemRect.bottom+1;
	nXc=tabRect.right-itemRect.left-1;
	nYc=tabRect.bottom-nY-1;

	m_tabPages[0]->SetWindowPos(&wndTop, nX, nY, nXc, nYc, SWP_SHOWWINDOW);
	for(int nCount=1; nCount < NUMBEROFPAGE; nCount++)
	{
		m_tabPages[nCount]->SetWindowPos(&wndTop, nX, nY, nXc, nYc, SWP_HIDEWINDOW);
	}
}

void CParticleTabControl::Apply()
{
	((CParticleEmitterTab*)m_tabPages[0])->Apply();
	((CParticleParticleTab*)m_tabPages[1])->Apply();
	((CParticleDisplayTab*)m_tabPages[2])->Apply();
}

void CParticleTabControl::SetActive()
{
	((CParticleEmitterTab*)m_tabPages[0])->SetActive();
	//((CParticleParticleTab*)m_tabPages[1])->SetActive();
	//((CParticleDisplayTab*)m_tabPages[2])->SetActive();
}

BEGIN_MESSAGE_MAP(CParticleTabControl, CTabCtrl)
	//{{AFX_MSG_MAP(CParticleTabControl)
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CParticleTabControl message handlers

void CParticleTabControl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CTabCtrl::OnLButtonDown(nFlags, point);

	if(m_tabCurrent != GetCurFocus())
	{
		m_tabPages[m_tabCurrent]->ShowWindow(SW_HIDE);
		m_tabCurrent=GetCurFocus();
		m_tabPages[m_tabCurrent]->ShowWindow(SW_SHOW);
		m_tabPages[m_tabCurrent]->SetFocus();
	}
}
